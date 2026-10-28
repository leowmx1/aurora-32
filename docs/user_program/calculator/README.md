# 计算器

源文件：[calculator.c](../../../user-programs/programs/calculator/calculator.c)。
产物：`user-programs/programs/calculator/build/calculator.hex`。
程序是早期裸机计算器的用户态移植，使用键盘回调和系统 IO，保留词法分析、
中缀转后缀、软件栈求值的基本流程。

## 使用

启动显示 `Enter expression, end with <Enter>:` 和 `>> `。输入 ASCII 表达式，
按 Enter 求值。成功显示 `Result: ...`，随后继续等待新表达式。
大写 `Q` **不需回车**，立即输出换行和 `Quit.`，并以退出码 0 退出。
小写 q 没有同样的退出语义。

支持十进制整数、二元 `+ - * /`、一元正负号、ASCII `(` `)` 和空格。
`~` 使用最近一次成功结果；此前没有结果时会给出警告，不能当成有效历史值依赖。
不支持浮点、指数、变量赋值、隐式乘法、通用命令或完整行编辑；输入括号时请切换到
英文输入法，中文括号不是同一字符。Backspace 当前没有实现撤销 token。

| 输入 | 预期结果 |
|---|---:|
| `8*3+2` | 26 |
| `8*(3+2)` | 40 |
| `-8*(2+3-(-3-1+5))` | -32 |
| `-8*-3/3` | 8 |
| `7/6` | 1 |
| `2-3+5` | 4 |
| 上次结果为 4 时输入 `~*5` | 20 |

整数除法向零截断。程序未实现大整数，也没有全面的有符号溢出检查；不要把超出
32 位有符号范围的表达式当成数学精确运算。特别是 `INT_MIN / -1` 不应作为正常输入。

## 计算过程

`process_input` 逐字符维护 `current_value`、`has_digit` 和 token 表。
每个 token 使用两个 32 位字：类型和数值/运算符；数字类型为 1，运算符为 2。
一元负号/正号用内部值 127/126 区分于二元 `-`/`+`。

回车提交后，当前回调使用运算符栈生成 `post_expr`：乘除高于加减，一元运算优先级最高。
同优先级二元运算左结合，连续一元符号右结合。遇到右括号，弹出运算符直到左括号。
然后顺序计算后缀表达式：数字入栈，一元运算取一个值，二元运算依次取 b、a，
计算 `a op b` 后压回。最终栈里必须恰好有一个结果。

`calculation_stack[]` 是 URAM 中的软件栈；`calculation_stack_pointer` 是数组索引。
它与 USP 上的 C 返回地址、寄存器 spill 和中断回调返回链相互独立。
因此修改计算栈宏时，不能重新用 PUSH/POP 直接操作硬件用户栈。

## 键盘与状态

键盘事件由内核按 FIFO 保存，并从队列取出后进入普通 C 回调；用户程序不再维护事件队列或过滤序号。
回调直接处理业务，可以输出、调用普通函数和被 tick 抢占；键盘与 Telnet 回调互不嵌套。
主循环使用 `system.WaitEvent()` 等待，事件到达后由内核唤醒。
内核 FIFO 可用 63 槽，满时丢新事件并累计 `system.EventDrops()`；本示例检测到溢出便提示并以代码 1 退出，避免继续使用缺失输入。

`input_done` 的完成值仍为 `0x13579BDF`。尾随 CR/LF 合并规则、token 和软件栈算法不变。
求值期间新事件继续在内核排队，不再主动丢弃下一行，也不再用大小比较拒绝回绕后的序号。

## 错误、容量和调试

常见提示包括 `No input received`、`Missing operand`、`Unexpected ')'`、
`Expected ')'`、`Invalid expression` 和 `Division by zero`。
错误后程序通常重回输入提示，上次成功结果仍可保留。

`MAX_TOKEN=512`，token/后缀数组各 1024 字，软件栈也是 1024 字；当前并没有为所有
输入路径提供完整边界检查。避免长表达式、过深括号和连续高速粘贴。
这些固定容量不是内存安全证明；复杂畸形输入仍需要后续加固。

跟踪版的说明见 [calculator-trace](../calculator-trace/README.md)。
两种镜像来自同一源文件，只能择一加载。

## 构建与验证

```powershell
.\build-tools\build.ps1 -Program calculator
```

总构建脚本先构建内核；已经有匹配内核时可以执行
`./user-programs/build.ps1 -Program calculator`。
建议上电后依次输入上表表达式、`1/0`、空回车、`~`，最后大写 Q。
预期退出后看到内核 `USER_PROCESS exited with code 0` 和停机日志。
