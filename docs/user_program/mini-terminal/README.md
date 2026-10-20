# 迷你命令终端

源码：`user-programs/programs/mini-terminal/mini_terminal.c`。
这是一个用户态内置命令解释器，不是完整操作系统 shell：没有文件系统、进程创建或程序加载功能。

## 构建与运行

项目根目录执行 `./build-tools/build.ps1 -Program mini-terminal`。
ROM 加载 `kernel/build/kernel.hex`，UROM 加载
`user-programs/programs/mini-terminal/build/mini-terminal.hex`，复位后出现 `aurora>`。
内核新增了 Telnet 接口，必须重新加载配套内核，不能继续使用旧 ROM。

## 命令

| 命令 | 作用 |
|---|---|
| `help` | 列出内置命令 |
| `echo TEXT` | 本地输出文本，保留参数内部空格与大小写 |
| `send TEXT` | 经 Telnet 设备发送文本，并追加 CRLF |
| `uptime` | 输出系统启动后的无符号毫秒数，模 2^32 |
| `clear` | 输出 form feed (`\f`) 清屏 |
| `about` | 显示本程序和系统能力边界 |
| `exit [CODE]` | 退出；默认 0，支持 -2147483648 到 2147483647 |

命令名大小写不敏感，忽略行首及命令后的分隔空格。回车提交，BS/DEL 删除一个字符；
针对 Digital Terminal，回显退格使用单个 BS，而非 ANSI 光标控制。
CRLF 合并为一次提交，空行只重新提示。无历史记录、方向键编辑、引号/转义、管道和重定向。
每行最多 95 个可打印 ASCII 字符。超长或键盘队列溢出时整行作废，必须按 Enter 重新开始，
防止截断后的尾部意外成为 `exit` 等命令。

## Telnet 收发

程序同时订阅键盘和 Telnet 回调。设备地址 `0x06000000`，中断向量 `0x03000002`。
当前保存电路中的 Digital Telnet 组件端口是 8081；是否监听、连接是否成功由仿真组件负责，
程序不创建网络 socket、不修改防火墙。连接配置改变时以电路为准。

`send Hello` 将 `Hello\r\n` 写入设备。成功只表示内核完成 MMIO 写入，不表示远端收到，
不检测连接状态，也没有网络确认/重传协议。
收到的数据先缓存在独立接收行中，**仅遇到 CR、LF 或 CRLF 时**显示为 `[net] ...`，
随后重画提示符和正在编辑的命令。普通字符到达时不输出、不刷新；CRLF 合并为一次提交，
即使 CR 与 LF 分开到达也不会多打印一行。空行仍会显示空的 `[net]` 行。
除换行符外，不可打印字节显示为 `.`，避免远端控制字符清屏或干扰终端。
每行最多保存 256 个数据字节；超长行整行作废，在下一个换行时提示，然后恢复接收。
**网络输入不会自动执行命令，也不会转发为本地键盘输入。**

用户 Telnet 队列为 64 槽（有效 63），满时丢弃待处理输入及未完成接收行，
等后续换行时才输出丢失提示；内核另有独立 63 字节有效接收队列。
接收量超出处理速度时可能丢数据，本例不是可靠流协议实现。普通键盘回调仍遵循内核忙时丢键策略。
所有解析/输出在前台进行，两个回调都只负责短时入队。

## 用户库接口

本地键盘使用 `aurora_kbqueue.h` 的 `kbqueue.Init()` 和 `kbqueue.Next()`，
与下面的 Telnet 接收队列独立。键盘溢出时作废正在编辑的命令，等待回车重置。
屏幕批量输出使用 `system.io.buffer`，不再依赖独立 text 库。

```c
system.telnet.PutChar('A');       // 返回 0：写入低 8 位
int c = system.telnet.GetChar(); // 内核队列非阻塞读取；空队列 -2
system.telnet.Write(words, n);   // 全局字数组，n <= 64，成功返回 n
system.telnet.Subscribe(on_rx); // 也可用 aurora_telnet_subscribe(on_rx)
system.telnet.Subscribe(0);     // 取消订阅，不清空剩余队列
```

`on_rx(aurora_word_t event)` 接收 `0xA6SSSSCC`：非零 16 位序号和低 8 位数据。
数据字节 0、255 都合法。回调正常 `return`，由公共跳板通过系统调用 7 恢复原现场。
GetChar 与回调消费同一内核队列，不应同时作为两条独立消费通道使用。
Write 仅接受 UROM/URAM 全局缓冲，不接受 UStack，不要求零终止；非法整段请求不会部分输出。

## 验证与目录清理

此前已通过原生逻辑测试和实际 kernel/user BIN 指令模型回归；历史结果见
[构建验证记录](../../build-tools/verification.md)。这些验证不等同于 Digital 电路时序测试。

2026-10-19 已移除程序目录中的测试脚本、替身和测试产物，程序仅保留 C 源码、README 和 build。
正常构建不依赖 tests/common；公共键盘队列与系统 IO 库仍保留在 `user-programs/include/`。
