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
每行最多 95 个可打印 ASCII 字符。超长时整行作废，必须按 Enter 重新开始，
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

键盘事件由内核按 FIFO 保存，并从队列取出后进入普通 C 回调；用户程序不再维护事件队列或过滤序号。
回调直接处理业务，可以输出、调用普通函数和被 tick 抢占；键盘与 Telnet 回调互不嵌套。
主循环使用 `system.WaitEvent()` 等待，事件到达后由内核唤醒。
内核 FIFO 可用 63 槽，满时丢新事件并累计 `system.EventDrops()`；本示例检测到溢出便提示并以代码 1 退出，避免继续使用缺失输入。
键盘与 Telnet 的业务解析和输出在互不嵌套的回调中运行；两个行缓冲是编辑状态，不是事件队列。

## 用户库接口

本地键盘使用 `system.io.Subscribe`，远端使用 `system.telnet.Subscribe`，空闲主循环调用 `system.WaitEvent()`。
屏幕批量输出使用 `system.io.buffer`，不再依赖独立 text 库。

```c
system.telnet.PutChar('A');       // 返回 0：写入低 8 位
int c = system.telnet.GetChar(); // 内核队列非阻塞读取；空队列 -2
system.telnet.Write(words, n);   // 全局字数组，n <= 64，成功返回 n
system.telnet.Subscribe(on_rx); // 也可用 aurora_telnet_subscribe(on_rx)
system.telnet.Subscribe(0);     // 取消订阅，清除该来源的待处理事件
```

`on_rx(aurora_word_t event)` 接收 `0xA6SSSSCC`：非零 16 位序号和低 8 位数据。
数据字节 0、255 都合法。回调正常 `return`，由公共跳板通过系统调用 7 恢复原现场。
GetChar 只用于未订阅模式；已经订阅时返回 EBUSY，不和回调竞争事件。
Write 仅接受 UROM/URAM 全局缓冲，不接受 UStack，不要求零终止；非法整段请求不会部分输出。

## 验证与目录清理

此前已通过原生逻辑测试和实际 kernel/user BIN 指令模型回归；历史结果见
[构建验证记录](../../build-tools/verification.md)。这些验证不等同于 Digital 电路时序测试。

2026-10-19 已移除程序目录中的测试脚本、替身和测试产物，程序仅保留 C 源码、README 和 build。
正常构建不依赖 tests/common；系统 IO/事件 API 头文件仍保留在 `user-programs/include/`；事件队列实现在内核。
