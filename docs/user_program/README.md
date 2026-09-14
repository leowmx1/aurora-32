# 用户程序总览

这里的示例运行于用户态，依赖同一版本的 `kernel/kernel.c` 和用户 ABI。
当前有三个源程序、四个构建目标：

| 构建名称 | 源文件（相对根目录） | 交互与结束方式 |
|---|---|---|
| `calculator` | `user-programs/programs/calculator/calculator.c` | 表达式、回车求值，大写 Q 立即退出 |
| `calculator-trace` | 同上，加跟踪宏 | 输出 IRQ 序号和解析状态的调试版本 |
| `tic-tac-toe` | `user-programs/programs/tic-tac-toe/tic_tac_toe.c` | 5×5 棋盘，连续五子获胜，结束退出 |
| `guess-number` | `user-programs/programs/guess-number/guess_number.c` | 自定义含负数范围，回车提交，猜中或 Q 退出 |

早期脚本中列出的 `keyboard-client`、`hello-loop`、`system-io-demo` 已不在当前
项目源文件中；它们不是本次可构建的示例，不会生成占位 HEX。

## 构建、加载和版本配对

```powershell
# 根目录执行：先内核，后全部用户程序
.\build-tools\build.ps1
# 先重建内核，再只编译猜数字
.\build-tools\build.ps1 -Program guess-number
# 内核没有变化时，只重新编译计算器
.\user-programs\build.ps1 -Program calculator
```

产物始终在各程序自己的 `build/`，包括 `.s`、`.o`、`.bin`、`.hex`。
内核加载到 ROM，所选用户 HEX 加载到 UROM；不要拼接两个 HEX，也不要把多个
用户镜像同时装到同一个 UROM。源文件名和构建目标名可能不同，以文档里的路径为准。

用户构建从 `kernel/build/kernel.o` 读取 `aurora_syscall_entry`，把 ELF 字节地址除以
4 后定义为 `AURORA_SYSCALL_ENTRY_ADDRESS`。内核体积变化会改变此地址。
`-KernelObject` 可以指定另一个内核对象，但运行时必须使用它对应的 ROM 镜像。

## 新示例的基本约定

自己新增的 C 文件不必加入内置示例清单。在项目根目录执行：

```powershell
.\build-tools\build-user.ps1 "C:\my-project\main.c"
```

会自动构建配套内核，并生成源码同目录下 `build/main.hex` 及 `.s/.o/.bin`。
详情见 [自定义源文件构建](../build-tools/README.md#一行编译自己的用户程序)。

源文件包含 `<aurora.h>` 和 `<aurora_system.h>`，文件作用域声明
`AURORA_USER_PROGRAM;`，入口定义为不返回的 `_start`，第一项源级操作是
`aurora.SetUpStackPointer()`。当前编译器在 `_start` 序言中实际设置 USP/UFP，
该库调用保留为启动约定，不能依靠普通 C 调用栈先存在再初始化栈。

终端输出使用 `system.io.PutChar`、`PutString`、`PutInt`，结束使用 `system.Exit`。
不直接读写键盘/终端 MMIO，不执行 `DI/EI/HALT/IRET/PUSHU/POPU`。
空闲主循环可执行 `aurora.Nop()`；tick 会抢占该循环。

## 键盘是异步回调，不是读取设备轮询

三个源程序都订阅 `aurora_keyboard_subscribe(callback)`。键盘硬中断进入内核，
内核构造用户回调现场，把事件字通过 r0 传入普通 C 回调。回调仅入队并 `return`。
回调 RET 到用户跳板后，跳板调用回调返回系统调用，由内核恢复此前的用户现场。

主循环检查**已经由回调填入的内存队列**，并执行回显、解析与计算；这不等于调用
`GetChar()` 轮询硬件。此设计避免在异步上下文里改变解析器或长时间输出。

事件字格式：`0xA5SSSSCC`，高 8 位为签名、中间 16 位为非零 IRQ 序号、低 8 位为字符。
用户处理前应校验签名和序号，再提取字符。不要把高位元数据直接当 ASCII 打印。
队列用 `volatile` 保存共享状态，生产者先写数据再发布写指针，消费者先取数据再
移动读指针。现有内核禁止回调重入，回调忙时新字符可能被丢弃。

## IO 接口的实际边界

| 接口 | 返回/行为 |
|---|---|
| `system.io.PutChar(c)` | 内核输出低 8 位，正常返回 0 |
| `system.io.PutInt(n)` | 有符号十进制，覆盖 INT_MIN |
| `system.io.PutString(p)` | 零终止，最多检查 4096 字，指针必须在 UROM/URAM |
| `system.io.GetChar()` | 非阻塞；无数据返回 `AURORA_EAGAIN=-2`；用于未订阅时的兼容模式 |
| `system.Exit(code)` | r1 携带退出码，成功不返回，内核打印日志并停机 |

当前字符串检查不包含 UStack，因此不能假定任意栈上临时字符串都能传给
`PutString`；现有示例使用由后端分配到合法用户区的字符串。
新增可写文本缓冲区建议每字符用一个 `int`，当前 `char` 窄存储支持仍不完整。

## 排查顺序

没有提示：先检查 ROM/UROM、微码版本、用户入口和实际 INT 地址；再检查 tick 是否到达。
有提示不能输入：检查键盘 IRQ、订阅返回值、回调入口和返回跳板。
重复或乱码：先区分电路是否重复 IRQ、事件签名/序号是否有效、回调是否正确恢复现场。
`calculator-trace` 可帮助观察这些状态，但跟踪输出会改变程序运行时序。
具体输入格式、错误和容量见各程序 README。
