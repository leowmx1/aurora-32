# 用户程序总览

这里的示例运行于用户态，依赖同一版本的 `kernel/kernel.c` 和用户 ABI。
当前有七个源程序、八个构建目标：

| 构建名称 | 源文件（相对根目录） | 交互与结束方式 |
|---|---|---|
| `calculator` | `user-programs/programs/calculator/calculator.c` | 表达式、回车求值，大写 Q 立即退出 |
| `calculator-trace` | 同上，加跟踪宏 | 输出 IRQ 序号和解析状态的调试版本 |
| `gomoku-screen` | `user-programs/programs/gomoku-screen/gomoku_screen.c` | 屏幕 5×5 五子棋，A3/3A 坐标输入；[说明](gomoku-screen/README.md) |
| `guess-number` | `user-programs/programs/guess-number/guess_number.c` | 自定义含负数范围，回车提交，猜中或 Q 退出 |
| `snake-screen` | `user-programs/programs/snake-screen/snake_screen.c` | 同规则显存版，差量矩形绘图；[说明](snake-screen/README.md) |
| `maze-screen` | `user-programs/programs/maze-screen/maze_screen.c` | 随机迷宫显存版，仅局部更新人物；[说明](maze-screen/README.md) |
| `mini-terminal` | `user-programs/programs/mini-terminal/mini_terminal.c` | 内置命令、Telnet 收发、exit 指定退出码 |
| `graphics-demo` | `user-programs/programs/graphics-demo/graphics_demo.c` | 用户态连续填充三个矩形，完成后退出；[说明](graphics-demo/README.md) |

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
空闲使用 `system.WaitEvent()`；仍有主动计算工作时用 `system.Yield()` 让出后继续。

## 内核事件队列与回调

键盘事件由内核按 FIFO 保存，并从队列取出后进入普通 C 回调；用户程序不再维护事件队列或过滤序号。
回调直接处理业务，可以输出、调用普通函数和被 tick 抢占；键盘与 Telnet 回调互不嵌套。
主循环使用 `system.WaitEvent()` 等待，事件到达后由内核唤醒。
内核 FIFO 可用 63 槽，满时丢新事件并累计 `system.EventDrops()`；本示例检测到溢出便提示并以代码 1 退出，避免继续使用缺失输入。

独立 `aurora_kbqueue.h`、`kbqueue.Init/Next` 已删除。路径现在是：
硬件 IRQ → 内核读取设备并入 FIFO → 内核取一条并构造用户现场 → 用户业务回调 → 返回跳板 → 恢复原用户现场。
无需用户再次入队，也不要在回调中调用 GetChar 取“刚才的按键”。

键盘事件字仍为 `0xA5SSSSCC`，Telnet 为 `0xA6SSSSCC`；使用
`aurora_event_character(event)` 提取低 8 位。序号仅用于诊断，不用来去重或排序，16 位回绕正常。
图形不产生完成事件；`graphics-demo` 使用 Busy 查询配合 Yield 等待，不使用 WaitEvent。

```c
#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;
static void on_key(aurora_word_t event) {
  system.io.PutChar((char)aurora_event_character(event));
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  if (system.io.Subscribe(on_key) != AURORA_OK) system.Exit(1);
  for (;;) system.WaitEvent();
}
```

| 接口 | 行为 |
|---|---|
| `system.io.Subscribe(callback)` | 注册键盘回调；0 取消；替换/取消时清除该来源的旧待投递事件 |
| `system.telnet.Subscribe(callback)` | 同上，Telnet 来源；与键盘共享 FIFO |
| `system.WaitEvent()` | 无待处理事件/唤醒记号时阻塞；等待期间 tick 跳过用户；回调内或暂缓区内返回 EBUSY |
| `system.Yield()` | 立即让给系统进程但保持可运行；系统空闲可以立即切回，不是定时睡眠 |
| `system.EventDrops()` | 订阅事件 FIFO 满时丢弃的新事件累计数，unsigned、模 2^32 |
| `system.DeferEvents()` | 嵌套计数加一，暂缓回调投递；硬件 IRQ 和 tick 保持正常 |
| `system.ResumeEvents()` | 减一；回到零允许立即投递一条；没有匹配 Defer 时 EINVAL |

主循环和回调都修改的状态必须放在成对 Defer/Resume 区间内，或只在回调里访问。
例如贪吃蛇保护更新和绘图，不让按键回调打断同一游戏状态/输出缓冲的修改。
不应长期暂缓；它不是扩大队列容量，也不能解决硬件设备本身漏收数据。

WaitEvent 使用合并的唤醒记号，避免事件刚在调用前回调完成、主循环却永久睡下；
因此它可能立即返回，不保证“一次调用严格等待一次未来 IRQ”。适合在检查业务条件的循环里使用。
未订阅任何来源时返回 EINVAL。回调可以 Yield，但不能 WaitEvent 等待另一个回调，因为回调禁止嵌套。

详细调度与限制见 [内核事件说明](../kernel/events.md)。

## IO 接口的实际边界

| 接口 | 返回/行为 |
|---|---|
| `system.io.PutChar(c)` | 内核输出低 8 位，正常返回 0 |
| `system.io.PutInt(n)` | 有符号十进制，覆盖 INT_MIN |
| `system.io.PutString(p)` | 零终止，最多检查 4096 字，指针必须在 UROM/URAM |
| `system.io.GetChar()` | 非阻塞；无数据返回 `AURORA_EAGAIN=-2`；用于未订阅时的兼容模式；已订阅返回 EBUSY=-5 |
| `system.io.Write(words, count)` | 每字一个字符，输出低 8 位，最多 64 字；成功返回 count；全局缓冲位于 UROM/URAM |
| `system.TimeMs()` | 无符号系统毫秒数，模 2^32，经系统调用访问内核 timer |
| `system.Exit(code)` | r1 携带退出码，成功不返回，内核打印日志并停机 |

当前字符串检查不包含 UStack，因此不能假定任意栈上临时字符串都能传给
`PutString`；现有示例使用由后端分配到合法用户区的字符串。
新增可写文本缓冲区建议每字符用一个 `int`，当前 `char` 窄存储支持仍不完整。
`aurora_system.h` 提供 `system.io.buffer` 专用缓冲输出接口和 `aurora_io_buffer` 类型。
贪吃蛇、迷宫和迷你终端使用此缓冲接口；缓冲区应为 static/全局变量，不要放在局部栈上。

```c
#include <aurora_system.h>
static aurora_io_buffer out;

// 在函数里调用：
system.io.buffer.Begin(&out);
system.io.buffer.String(&out, "Score: ");
system.io.buffer.Unsigned(&out, 123u);
system.io.buffer.Char(&out, '\n');
system.io.buffer.Flush(&out);
```

C 中这个分组由嵌套结构体实现，不是语言级的类。每次显式传入缓冲区，可以定义多个互不干扰的实例。
Begin 重置待输出计数和错误，不清屏，也不自动提交旧内容；满 64 字自动提交，Flush 提交剩余部分。
`.error` 非零表示失败，可调用 `system.io.buffer.ReportError(&out)` 输出诊断；不会自动退出或重试。
同一个实例不要同时在主循环和中断回调里修改。Unsigned 仅负责无符号十进制，非格式化 printf。
实现位于内部头文件 `aurora_buffer.h`，应用只需包含 `aurora_system.h`。
原独立 text 头文件及旧函数名已移除，不保留兼容入口。

## 排查顺序

没有提示：先检查 ROM/UROM、微码版本、用户入口和实际 INT 地址；再检查 tick 是否到达。
有提示不能输入：检查键盘 IRQ、订阅返回值、回调入口和返回跳板。
重复或乱码：先区分电路是否重复 IRQ、事件签名/序号是否有效、回调是否正确恢复现场。
`calculator-trace` 可帮助观察这些状态，但跟踪输出会改变程序运行时序。
具体输入格式、错误和容量见各程序 README。

图形示例：[屏幕迷宫](maze-screen/README.md)、[屏幕五子棋](gomoku-screen/README.md)。
终端示例：[迷你命令终端及 Telnet API](mini-terminal/README.md)。
这些程序使用内核 FIFO 和普通业务回调，不依赖贪吃蛇的游戏代码。
