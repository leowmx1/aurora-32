# 四用户进程与内存银行

2026-11-07：银行寄存器地址由硬件作者确认为 **0x0D000000**，LDMA 不受它影响。
保存的电路使用 `Kernel Mode` 旁路银行拼接：内核态可直接访问各银行物理地址。

## 当前已接入

- 编译器/汇编器支持内核专用 LUSP(62)、LUFP(63)、OUSP(64)、OUFP(65)。
- aurora.h 提供对应 Get/SetUserStackPointer 和 Get/SetUserFramePointer。
- 用户全局变量限定为一银行的 `2^22` 字，用户代码构建检查同样的容量。
- 内核启动选择 bank 0，调度时选择目标进程银行；系统调用翻译用户缓冲区。
- 内核、九个用户程序和 storage 镜像使用此布局；ABI 仍为 3，系统调用跳板仍为 ROM 字 2。

当前是 SYSTEM + 四个用户槽。本地与远程各有一个前台、一个后台配额。
槽位按空闲顺序分配 1..4，银行号为槽位减 1，并非固定绑定终端；0 为 SYSTEM。
没有引入 malloc、分页、地址空间换出或 fork。相同程序可加载到不同银行并行运行。

## 命令和通知

```text
process-demo &
run calculator
```

另一终端可同样启动后台任务和前台程序。支持 `NAME &`、`run NAME &`、
`quickrun NAME &`；末尾 `&` 前可有或没有空格。每端最多一个后台，满时不执行 DMA。
`&` 仅用于启动程序，不是管道或通用 shell 作业语法；暂不提供 fg/bg/暂停/恢复命令。

`ps` 列出 SYSTEM 与四个用户槽：槽位、银行、local/remote、FG/BG、READY/SLEEP/WAIT/
EXITED/EMPTY、程序名、已退出的退出码、图形/MIDI 所有者。已退出信息保留到槽位复用。
因为 ps 自身由 SYSTEM 执行，其他可运行进程显示 READY，不虚称正在同一 CPU 上执行。
`kill 1` .. `kill 4` 按槽位强制终止，退出码 137。无参数或 `kill user` 仅在恰有一个
活跃用户进程时兼容；多进程时必须明确编号。槽位不是永不复用的 PID，执行 kill 前先看 ps。

退出通知立即写到进程所属终端，不论当时前台是系统命令行、用户输入等待还是游戏：

```text
[61234 ms] USER_PROCESS exited with code 0.
  name=process-demo slot=1 bank=0 local background
```

后台退出不退出前台、不清除前台输入队列、不重绘显存、不重置提示符。
后台用户的 stdout 被内核静默丢弃，本地和远程终端规则相同；没有隐藏日志队列或输出重定向。
PutChar/PutInt/PutString 的有效请求返回 OK；Write 返回请求字数，buffer.Flush 因此仍成功。
参数错误仍按原 ABI 返回错误，PutString 保留地址边界、4096 字上限和终止符检查。
Write 完成地址/长度检查后直接返回，不读取用户内容，也不写终端或增加 Telnet TX 计数。
后台请求中的清屏、退格、换行同样没有终端副作用。SYSTEM 的退出/故障通知不被过滤。
通知期间短暂保持 IE=0，保证通知自身不会被
另一个进程的输出插入，但这不是输入编辑器的自动重绘协议。

输入只路由到所属终端的前台；后台 Subscribe/GetChar 返回 EBUSY，不抢键盘或 Telnet。
后台适合无交互任务，不能直接把必须等待输入的编辑器/编译器当成可用的后台工作流。
`process-demo &` 不依赖输入，每秒 Sleep，60 秒（timer 时间）后正常退出。

### 主动转后台：system.Background()

系统调用 36，r0=36、r1/r2/r3=0，r0 返回状态；普通 C 写 `system.Background()`。
成功后立即将所属终端还给 SYSTEM，当前进程继续参与调度，不重载程序、不更换槽位或银行，
保留局部变量、用户栈/帧指针、全局数据、文件引用及 GPU/MIDI 所有权。
取消前台键盘订阅，丢弃此前排队的输入，并吞掉交接后的首个 LF（若首字符不是 LF 则正常处理）。
之后不能再 GetChar/Subscribe；无订阅时 WaitEvent 返回 EINVAL，后台等待应使用 Sleep/Yield。

所属终端已有后台任务，或在回调/DeferEvents 区域中调用，返回 EBUSY，原前台状态不变；
已是后台则返回 OK，属于幂等调用。保留参数非零或 SYSTEM 调用返回 EINVAL。
没有对应的自动转回前台接口。

compiler 正常前台启动，收齐 Source/Output 后询问 `[Y/n]`，Y/回车才调用此接口，N 保持前台；成功即释放命令行，静默编译。
选择 N 才能看到函数级进度和详细错误。后台配额已满则保留前台编译，不中止任务。
FS 遇到 SYSTEM 正在使用存储而返回 EBUSY 时，编译器 Yield 后重试。
看到提示符不代表编译已结束，应等带 compiler 名字的退出通知；退出码 0 才表示成功。

## 地址公式

按从 0 开始的位编号：

```text
CS = decode(logical_address[31:24])
用户态设备内部地址 = (bank << 22) | (logical_address & 0x003fffff)
内核态设备内部地址 = physical_address & 0x00ffffff
```

逻辑窗口分别为 UROM `80000000–803fffff`、URAM `81000000–813fffff`、
UStack `82000000–823fffff`。USP/UFP 中保留完整的逻辑地址，不存物理银行偏移。
每个设备四银行的物理内部起点为：

| bank | 内部起点 | 内部末尾 |
|---:|---:|---:|
| 0 | `000000` | `3fffff` |
| 1 | `400000` | `7fffff` |
| 2 | `800000` | `bfffff` |
| 3 | `c00000` | `ffffff` |

例如 bank 2 的用户地址 `0x81000100`，内核物理访问地址是 `0x81800100`。
翻译之前必须检查区号、长度、溢出及逻辑地址原 [23:22] 为 0，不能用掩码替代校验。
用户态的硬件截断产生地址别名；在没有额外逻辑前，不承诺对本银行越界产生异常。

LDMA origin 始终是 Storage 的物理内部偏移；target 为 `bank * 0x400000 + code_offset`。
必须检查复制范围不跨出目标银行，不允许直接把带 `0x80` 区号的地址写进 target。
内核自身 ROM、RAM、KStack、IVT、MMIO 地址均不加银行偏移。

## 调度与现场

1. PCB 独立保存银行号、USP/UFP、KSP、事件 FIFO、订阅、回调原现场、等待状态和文件引用。
2. 禁止中断后保存旧 USP/UFP，再选择目标 bank、恢复目标 USP/UFP/KSP，最后 IRET。
   硬件中断帧仍在 KStack，不能把 bank 当成内核栈选择寄存器。
3. 用户缓冲区读取/写入、字符串、位图包、MIDI 序列、文件参数必须统一翻译到所属银行。
   异步工作不能只保留一个无法确定归属的裸用户指针。
4. 装载器明确目标 bank，DMA 和 CPU 数据/BSS 写入使用对应物理地址；加载不覆盖其他进程。
   DMA busy 期间暂停用户调度（避免用户从被 DMA 占用的 UROM 总线取指），tick/输入仍应答入队。
   SYSTEM 命令保留 FS 工作区；其他用户 FS 系统调用返回 EBUSY，应重试。
5. 用户让出或被抢占后回 SYSTEM；SYSTEM 按轮转顺序选择可运行用户。WaitEvent/Sleep 跳过，
   Sleep 到期扫描和 MIDI 服务只在 tick 中进行，空闲 Yield 不重复扫描所有计时器。
6. 为避免固定短 tick 与调度开销产生相位锁定，首次恢复后若 tick 保存的 PC 仍等于刚恢复的
   PC，允许当前用户继续一次；每次派发最多一次宽限。下一次 tick 正常轮转，即使程序 JMP 自身。
   tick 周期仍须足够容纳 ISR 开销；这不是对任意高 IRQ 频率的实时保证。

每个用户有三个固定内核栈区域，普通栈起点为 `0x02010000+(slot-1)*0x30000`，
两个回调栈分别加 `0x10000`、`0x20000`。它们不是用户物理内存银行，也没有自动栈溢出保护。
回调只在目标进程银行/USP/UFP 已恢复后注入；中断了其他进程时仅入队。
回调返回重建原 FLAGS/PC/r0..r15，并恢复回调前 USP/UFP。四个进程可各有一个挂起的回调，
同一进程不嵌套回调。

**当前 PUSHU/POPU 微码只选择 USP，不改变 Kernel Mode。** 因此内核态直接 PUSHU
逻辑 `0x82xxxxxx` 地址只会访问物理 bank 0，而不会应用用户银行。回调注入不再使用 PUSHU：
内核读取 USP，将返回跳板写入 `USP + bank_base`，再用 LUSP 把逻辑 USP 加一。
回调 RET 在用户态正常弹当前银行的栈；CALLBACK_RETURN 再恢复原 USP/UFP。
此前“多进程时 Tetris 按一次键就报 Input queue overflow”正是返回跳板写错银行的表现，
不是一个按键真的填满 63 个事件。测试模型现已按保存的微码修正，并覆盖 bank 1 的该场景。

## 共享资源与限制

显存与 MIDI 各自只允许一个用户所有者；首次成功提交取得所有权，持续到进程退出/kill。
其他进程请求返回 EPERM，不允许偷偷覆盖。SYSTEM 绘制图标时短暂保留显存，用户请求返回
EBUSY；有用户图形所有者时系统不会重画图标。显存没有远程画面传输。
退出只停止本进程拥有的设备，其他进程的 MIDI、GPU 和订阅不受影响。

文件 ID 的引用按进程保存；任一活跃进程仍引用文件时 rm 返回 EBUSY。
不同进程仍可能先后保存同一文本文件，未实现文件锁、事务、写入版本冲突检测。
内核系统调用不重入，长文件保存/安装会延迟 tick；SYSTEM 的两端命令也仍串行处理。
用户保护异常保留原来的全系统蓝屏停机策略，本次没有改成仅杀异常进程。
银行复用不清除所有 .noinit/栈尾/旧代码尾部，不能作为同银行前后进程的保密擦除保证。

## 验证范围

`tests/user-context/test.ps1` 覆盖四条指令及 22 位全局分配边界；
`build/contract.hex` 可独立装入 ROM，验证实际硬件的 USP/UFP 与 PUSHU/POPU。
`tests/storage/run_multiprocess.py` 执行真实 BIN，覆盖四银行、嵌套用户栈、两端回调、
非零目标 LDMA、DMA 期间暂停取指、退出通知、资源独占/释放与槽位复用。
`tests/storage/run_remote.py` 同时运行两份计算器，验证独立输入/运算和文件引用保护。
`tests/storage/run_background.py` 检查主动转后台、忙时无副作用、同槽位续跑、两端输入交接。
`tests/tetris/run_tetris.py` 同时检查独占 bank 0 与后台任务并存时 bank 1 的完整游戏按键流程。
指令模型不能验证 Digital 的微码、总线互斥、传播延迟或 IRQ 时序。
