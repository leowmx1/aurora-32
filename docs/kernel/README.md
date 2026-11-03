# Aurora-32 内核

源码：[kernel/kernel.c](../../kernel/kernel.c)。这是当前固定双进程系统的内核，
提供启动、tick 抢占切换、终端输入回调、双端系统终端、用户异常日志和进程退出。
现在包含 EEPROM ASFS2 目录/文本文件系统、程序加载器和常驻系统终端；仍没有通用进程创建、
虚拟内存或多个并发用户进程。详见 [存储格式和启动流程](storage.md)。

## 构建和启动

根目录执行 `./build-tools/build.ps1` 会先编译内核再编译用户镜像；单独构建用
`./kernel/build.ps1`。内核产物在 `kernel/build/kernel.{s,o,bin,hex}`。
入口第一条机器指令固定为 `JMP aurora_reset_entry`：先设置 KSP/KFP、DI，输出不依赖全局数据的早期日志，
再跳入 `_start` 执行编译器全局初始化。ROM 字 2 的系统调用跳板保持不变。

`_start` 的第一项源级操作是 `aurora.SetUpStackPointer()`；实际后端序言先设置
KSP/KFP 为 `0x02000000`，再分配局部栈空间和执行初始化。启动顺序：

1. 初始化内核栈，输出栈初始化状态。
2. 禁用中断，初始化键盘指针、IRQ 序号、异常和退出记录。
3. 初始化系统/用户 PCB；默认没有可运行的用户进程。
4. 注册用户访存异常、非法指令、键盘、Telnet 和 tick 的硬件向量。
5. 输出初始化完成日志。
6. 挂载 EEPROM、自检后立即进入双终端，不再等待 3 秒；本地显示 BIOS 启动耗时。
7. 系统空闲时主动 Yield 保存系统现场并进入用户入口；不必等待第一次 tick。

启动日志格式是 `[123 ms] message`。时间来自内核 timer MMIO `0x08000000`，
直接使用无符号毫秒，没有除以 1000，timer 返回值按 2^32 回绕。
完成启动后没有周期性启动日志，避免与用户终端内容交错。

## 设备和异常向量

| 类型 | CPU 字地址 | 行为 |
|---|---|---|
| 键盘数据 | `0x04000000` | 键盘 ISR 读取一次并取低 8 位 |
| Telnet 数据 | `0x06000000` | ISR 读取应答，系统调用写入发送 |
| 终端数据 | `0x05000000` | 写字符 |
| tick 设备 | `0x07000000` | ISR 先读以应答，值不使用 |
| timer 设备 | `0x08000000` | 读系统运行毫秒数，不负责调度 |
| 图形参数 | `0x09000000`..`0x09000008` | 低 4 位译码 x/y/w/h/color/bgcolor/dataLow/dataHigh/mode |
| 图形状态 | `0x09000009` | 读 busy；写 1 启动，完成自动清零；写 0 停止 |
| MIDI | `0x0A000000`..`0x0A00007F` | 地址低 7 位音符/乐器；数据力度、OnOff、PC |
| EEPROM 存储 | `0x0B000000`..`0x0BFFFFFF` | 64 MiB，内核按字读写 ASFS2；用户通过系统调用访问 |
| LDMA | `0x0C000000`..`0x0C000004` | 内核专用代码搬运：origin/target/len/stat/sum；写 SUM 清零，读 UROM 写入后的读回累加和 |
| 键盘向量 | `0x03000001` | `aurora_keyboard_irq` |
| Telnet 向量 | `0x03000002` | `aurora_telnet_irq` |
| 用户内存异常 | `0x03000004` | `aurora_user_memory_fault` |
| 用户非法指令 | `0x03000005` | `aurora_user_illegal_instruction` |
| tick 向量 | `0x03000007` | `aurora_tick_irq` |

INT 不读取上述向量表。软件调用写 `INT 0x00000002`，跳板 `aurora_syscall_gateway`
再 JMP 到内部 `aurora_syscall_entry`。构建时检查固定入口，不使用旧动态地址或 IVT 地址。

## 中断现场和双进程切换

硬件进入中断时先锁存被打断状态与 FLAGS，再切内核态，并在 KSP 上依次保存
**FLAGS、PC**。ISR 按 r0 到 r15 的顺序 PUSH；栈向高地址增长，KSP 指向下一空位。
令 T 为压完通用寄存器后的 KSP，B=T-18：

| 位置 | 内容 |
|---|---|
| B+0 | 原 FLAGS，包含原 P 位 |
| B+1 | 原 PC |
| B+2 … B+17 | 原 r0 … r15 |
| T | 下一空位，不属于这 18 字现场 |

返回先 POP r15…r0，再 IRET 恢复 PC，最后恢复 FLAGS/P。P 必须在读取 PC 后才切换，
否则可能从 USP 取到错误 PC。INT 自带 CLR_IE，用户不执行 DI。

`aurora_schedule_from_tick(stack_top)` 当前只交换 PCB 的 `ksp`：系统被打断时
保存系统 KSP，若用户可运行才选择用户 KSP；用户被打断时反过来。
同一选择逻辑也用于系统空闲 Yield。用户在 WaitEvent/Sleep 阻塞时被跳过，Sleep 到期由 tick 恢复；内核空闲循环保持中断开启等待事件。现场留在各自内核栈，普通 tick
不会每次把全部寄存器再复制到 PCB 的 r[]。汇编入口取返回的 KSP，随后 POP/IRET
自然恢复目标进程。PCB 的 r[]/pc/flags 也用于用户首次启动和回调原现场快照。

| 栈基址 | 用途 |
|---|---|
| `0x02000000` | 系统/内核启动栈 |
| `0x02010000` | 用户进程普通陷入内核的现场 |
| `0x02020000`、`0x02030000` | 用户回调的专用内核栈 bank |
| `0x82000000` | 用户入口初始化的 USP/UFP |

这些是固定区域约定，不是具有边界保护的动态分配器。当前只有一个用户进程，
没有切换多个 USP/UFP 的完整通用方案。KSP/UStack 的区分必须由硬件保持。

## 用户键盘回调

PCB 保存 `subscribed_vector`、`callback`、`callback_trampoline`、`callback_active`
以及原现场和 callback 栈信息。标准用户订阅提交回调 UROM 地址和返回跳板地址。
内核检查地址范围，非零回调只允许位于 UROM。

一次键盘 IRQ 的当前流程：

1. 保存通用寄存器，读取键盘数据，掩去字符高位元数据。
2. 没有订阅时，把字符放入内核兼容读取队列；有订阅时走回调路径，不重复入队。
3. 有订阅时无论回调是否活跃都先进入共享事件 FIFO，满时丢新事件并计数。
4. 增加非零 16 位序号，构造 `0xA5SSSSCC` 事件字。
5. 若允许投递，从 FIFO 队首选择事件（键盘和 Telnet 按 IRQ 被内核接收的顺序）；选取用户的被打断现场（若系统正在运行，则取用户保存的 KSP）。
6. 把原 FLAGS/PC/r0-r15 快照到 PCB，复制现场到回调栈，替换 PC 为回调地址、r0 为事件。
7. 成功准备现场才移动 FIFO 读指针；通过内核专用 PUSHU 向用户栈放入返回跳板；IRET 后在用户态执行普通 C 回调。
8. C 回调 RET 到用户跳板；跳板请求私有系统调用 7，内核从 PCB 重建原现场，再 IRET。

用户回调允许被 tick 抢占、调用系统 IO 和 Yield；不允许 WaitEvent/Sleep，避免等待无法嵌套执行的回调。
标准示例直接在回调中处理业务。专用 bank 不代表允许递归回调，`callback_active` 保证跨设备不嵌套。
`aurora_keyboard_subscribe_entry` 是低层特殊入口，跳板地址为 0，不会 PUSHU 普通
返回地址；它必须自己通过回调返回接口结束，不能当作普通回调 `return`。

## 系统调用 ABI

硬件 INT 进入内核；入口保存 r0-r15，与 IRQ 统一为 18 字现场。
普通返回结果写入保存的 r0 槽，按保存的请求号识别回调返回/Yield/WaitEvent/Sleep，不再使用返回值魔数。普通请求经 C 分派后恢复
寄存器并 IRET。输入为 r0 请求号、r1-r3 参数，普通返回值在 r0。

| r0 | 名称 | r1 / r2 / r3 | 正常结果 |
|---:|---|---|---|
| 0 | ABI_VERSION | 未使用 | 3 |
| 1 | CONSOLE_PUTCHAR | 字符 / 0 / 0 | 0，输出低 8 位 |
| 2 | KEYBOARD_SUBSCRIBE | 回调 / 返回跳板 / 键盘向量 | 0；回调为 0 表示取消 |
| 3 | KEYBOARD_NEXT | 未使用 | 下一个字符，空队列 -2 |
| 4 | YIELD | 未使用 | 0；立即切换到另一可运行进程 |
| 5 | CONSOLE_PUTINT | 有符号整数位模式 / 0 / 0 | 0 |
| 6 | CONSOLE_PUTSTRING | 字符串字地址 / 0 / 0 | 0，或地址/长度错误 |
| 7 | CALLBACK_RETURN | 私有跳板接口 | 恢复被打断现场，不普通返回 |
| 8 | PROCESS_EXIT | 退出码 / 0 / 0 | 输出日志并返回所属系统终端，不返回用户 |
| 9 | CONSOLE_WRITE | 用户字符字数组地址 / 字数 / 0 | 输出低 8 位，成功返回字数，最多 64 字 |
| 10 | TIME_MS | 未使用 | 返回原始 32 位无符号毫秒数，模 2^32 |
| 11..14 | 原始 TELNET 请求 | 保留编号 | 统一返回 EBUSY；设备由远程系统终端独占 |
| 15 | WAIT_EVENT | 未使用 | 0；事件等待，回调内/暂缓区内 -5，无订阅 -1 |
| 16 | EVENT_DROPS | 未使用 | unsigned 累计队列满丢弃数，模 2^32 |
| 17 | DEFER_EVENTS | 未使用 | 0；增加回调投递暂缓深度 |
| 18 | RESUME_EVENTS | 未使用 | 0；减少深度，降到零允许投递，无匹配 -1 |
| 19 | GRAPHICS_FILL_RECT | x+256*y / w+256*h / color | 0 已提交，-5 忙，-1 矩形非法 |
| 20 | GRAPHICS_BUSY | 未使用 | 0 空闲，1 忙 |
| 22 | MIDI_NOTE_ON | note / velocity / 未使用 | 0，参数不在 0..127 则 -1 |
| 23 | MIDI_NOTE_OFF | note / 未使用 / 未使用 | 0，参数非法 -1 |
| 24 | MIDI_PROGRAM | program / 未使用 / 未使用 | 0，参数非法 -1 |
| 25 | MIDI_ALL_NOTES_OFF | 未使用 | 0，释放内核记录的活动音符 |
| 26 | MIDI_SEQUENCE | 数组地址 / 步数 / 模式 | 0 复制并启动，-1 参数非法，-4 指针非法 |
| 27 | MIDI_SEQUENCE_BUSY | 未使用 | bit0 背景，bit1 音效 |
| 28 | GRAPHICS_BITMAP | 八字描述符地址 / mode=1或2 / 0 | 0 已提交，-1 非法，-4 地址错误，-5 忙 |
| 29 | SLEEP | ms (0..INT32_MAX) / 0 / 0 | 0 到期或事件唤醒；0ms 让出；参数 -1，回调/暂缓区 -5 |

错误码：`AURORA_EINVAL=-1`、`AURORA_EAGAIN=-2`、`AURORA_ENOSYS=-3`、
`AURORA_EFAULT=-4`、`AURORA_EBUSY=-5`。未知请求返回 ENOSYS。回调返回在当前非用户进程或没有活跃
回调时被拒绝；退出请求也验证当前进程是用户。

PutString 首先验证起始地址并计算 UROM/URAM 可读边界，再按边界读取，最多 4096 字，遇 0 结束。
它不接受 UStack 指针；达到长度上限返回 EINVAL，途中越界返回 EFAULT。
错误前已输出的前缀不会撤销。没有 MMU、进程权限表或一般用户指针验证框架。

Write 的内核实现见 `kernel/console_write.h`。它先检查长度和完整地址区间，再输出：
0 字立即返回 0；超过 64 字返回 EINVAL；非法地址、溢出、跨区域或 UStack 返回 EFAULT，
验证失败不会输出前缀。缓冲区不要求零终止，一个硬件字一个字符。
分批长度用于限制每次关闭中断的输出时间；批次间 IRET 后允许正常 IRQ。
用户辅助接口为 `system.io.Write`，`system.io.buffer` 负责缓冲分批。

TimeMs 读取内核 timer 设备 `0x08000000`，与读应答的 tick `0x07000000` 不同。
返回值高位为 1 也属于合法时间，不应按负错误码处理；用户库显式转换为 `aurora_word_t`。
它不修改 tick 状态，不实现阻塞等待。旧系统调用号不变，当前 ABI_VERSION=3。

图形接口通过 `system.graphics.FillRect/Bitmap/BitmapTransparent` 异步提交，以 Busy 查询并 Yield。
屏幕完成中断已移除，内核不注册 0x03000003，不在关中断区等待绘制。
图形 Subscribe 已删除，旧请求 21 返回 ENOSYS；不能用 WaitEvent 等图形完成。
参数边界、并发语义和完成中断建议见 [graphics.md](graphics.md)。

MIDI 通过 `system.midi.NoteOn/NoteOff/Program/AllNotesOff` 操作固定的硬件通道，
另有 `Sequence/SequenceBusy` 内核定时序列，由现有 tick 推进，无需 MIDI 完成中断。
音效结束从背景原位置续播，播放不依赖绘图循环。退出及异常停机前取消序列、释放活动音符。
协议、时长安排及贪吃蛇音效测试见 [midi.md](midi.md)。

## 双终端、Telnet 与回调

详见 [本地与远程系统终端](remote-console.md)。Telnet 为系统独占，原始请求 11..14 返回 EBUSY。
本地程序订阅键盘，远程程序订阅 Telnet 映射的 A5 输入；两者都用 system.io，不需要改写程序。
程序只占所属端，另一端仍可执行系统命令和 kill，但不能启动第二个程序。
回调仍使用同一 63 槽 FIFO 和非嵌套返回协议，WaitEvent/Sleep 可被所属端输入唤醒。

## 退出与异常处理

内存异常和非法指令处理程序将用户标记为不可运行，清除订阅和回调状态，
打印带时间的错误原因，在显存绘制蓝底白字的 `:(`、异常原因和 `STOP 03000004/03000005`，
等待最后一笔绘图完成后 HALT，保留蓝屏，必须复位重新启动。不再 IRET 或返回系统终端。
`system.Exit(code)` 同样清理用户状态，并显示有符号退出码。会取消 MIDI 序列、关闭活动音符、
停止图形设备、清除订阅/回调/休眠/输入队列；不再恢复已经结束的用户现场。
若异常发生时不是有效用户进程，显示 KERNEL FAULT 并停机，不冒险恢复无效 SYSTEM 栈。
系统调用的非法参数/指针仍返回 EINVAL/EFAULT，不会仅因返回错误而蓝屏；触发条件是硬件保护异常向量。
普通 `halt` 会先清理 MIDI/用户状态，填充全屏黑色，等 GPU 完成再 HALT。
仅测试用 `kernel/build.ps1 -DirectBoot` 保留预加载 UROM/退出停机行为，退出也先刷黑屏。

`kernel/shutdown.h` 的停止路径全程关闭中断，不使用会执行 EI 的普通 console 绘图辅助函数，
防止 tick 调度或回调重新进入失败用户。GPU 和 timer 无需 CPU 接收中断即可推进；每次等待上限
10,000 ms，另有 1,000,000 次轮询兜底（timer 不推进时仍有限）。设备卡住则停止 GPU，
终端输出提示并直接 HALT，此时不保证屏幕完整。这里的“关机”是 CPU HALT，不是宿主电脑休眠/关机。

异常处理程序当前不尝试修复现场、跳过非法指令或重新执行访存，也没有实现可恢复
异常应答接口。若电路将异常应答与 tick 合并，需要相应核对硬件和内核；不能从
tick 的读应答推断异常现在也一定被读应答。启动时误发异常、IRQ 向量未保持等问题
应在电路侧确认，软件无法凭空恢复消失的向量号。

## 修改时必须联动验证

更改现场布局要同步硬中断序列、IRET、tick stub、键盘 stub、首次用户现场、
回调准备和回调返回。改 syscall ABI 要同步用户库及所有用户 HEX。
改代码/数据/栈基址要同步编译器、内核检查、用户入口和硬件片选。
`optnone/noinline` 及历史汇编掩码是保留的局部兼容措施，不能仅依据注释就判断
当前新编译器仍有同一个故障；移除前应单独验证机器码与硬件运行。
