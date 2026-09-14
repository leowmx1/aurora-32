# 当前后端指令速查

依据当前 `AuroraInstrInfo.td`、编码格式和打印器整理。操作码是最低 8 位。
本表使用 LLVM Aurora 汇编操作数顺序；旧 customasm 别名/伪语法可能不同。
“a,b,d”表示源 a、源 b、目标 d；尤其 ILD 的目标寄存器在前。

## 数据传送

| 指令 | opcode | LLVM 语法 | 用途 |
|---|---:|---|---|
| MOV | 10 | `mov a, d` | 寄存器复制 |
| ILD | 11 | `ild d, b` | 从寄存器 b 指向的地址读取 |
| IST | 12 | `ist a, b` | 把 a 写到寄存器 b 指向的地址 |
| LDR | 13 | `ldr d, address` | 绝对地址读取 |
| STR | 14 | `str a, address` | 绝对地址写入 |
| LDI | 15 | `ldi d, immediate` | 装入立即数 |
| LDA/LDC | 16 | `lda d, address` | 地址形式，LDC 为汇编别名 |
| LDO | 17 | `ldo a, b, d` | 后端保留的硬件取数指令；具体寻址语义核对微码 |

opcode 列按十六进制表示。涉及完整 immediate/address 的指令使用第二个 32 位字。
CPU 地址常量直接是字地址。LDA/LDI 等指令的硬件行为不能仅根据相似名称推导。

## 算术与逻辑

| 指令 | opcode | 操作数 | 说明 |
|---|---:|---|---|
| ADD / SUB | 20 / 21 | a,b,d | 32 位加减 |
| INC / DEC | 22 / 23 | a,d | 加一/减一 |
| MUL / DIV / REM | 24 / 25 / 26 | a,b,d | 乘法低 32 位，无符号除法/余数 |
| SFT | 27 | a,b,d | 硬件移位，计数方向有目标约定 |
| AND / NAND | 28 / 29 | a,b,d | 与/与非 |
| OR / NOR | 2a / 2b | a,b,d | 或/或非 |
| XOR / XNOR | 2c / 2d | a,b,d | 异或/同或 |
| NOT | 2e | a,d | 按位取反 |
| CMP | 2f | a,b | 更新比较 FLAGS，不返回通用寄存器值 |
| SMUL | 50 | a,b,d | 有符号乘法指令 |
| SDIV / SREM | 51 / 52 | a,b,d | 有符号除法/余数 |
| SSFT | 53 | a,b,d | 有符号移位指令 |

上述 ALU/CMP 在后端模型中定义 FLAGS。MUL 与 SMUL 的低 32 位乘积相同，
普通 LLVM mul 仍选择 MUL；除法与余数的有/无符号则不可混用。
SFT 的方向不能照搬其它 ISA 的正计数右移假设；C 右移有专用展开路径。
异常除数、除零和有符号溢出的硬件处理请结合电路，C 程序应先检查输入。

## 控制流

| 指令 | opcode | 语法 | 作用 |
|---|---:|---|---|
| JMP | 30 | `jmp address` | 绝对跳转 |
| JCC | 31 | `jcc condition, address` | 条件跳转 |
| JPR | 32 | `jpr a` | 寄存器间接跳转 |
| JRC | 33 | `jrc condition, a` | 条件间接跳转 |
| CALL | 34 | `call address` | 普通调用 |
| CAC | 35 | `cac condition, address` | 条件调用 |
| RET | 36 | `ret` | 当前模式栈上普通返回 |
| INT | 37 | `int rom_address` | 进入内核的直接软中断 |
| CINT | 38 | `cint condition, rom_address` | 条件软中断 |
| IRET | 39 | `iret` | 中断现场返回 |
| EI / DI | 3a / 3b | 无操作数 | 开/关中断，仅内核使用 |

条件编码位于条件型指令头的 [23:20]：

| 数值 | 条件 | 比较用途 |
|---:|---|---|
| 1 | c | 无符号小于，C 为借位约定 |
| 2 | z | 等于 |
| 3 | b | 无符号大于 |
| 4 | nc | 无符号大于等于 |
| 5 | nz | 不等于 |
| 6 | nb | 无符号小于等于 |
| 7 | n | 负号标志 |
| 8 | v | 溢出标志 |
| 9 / 10 | lt / ge | 有符号小于/大于等于 |
| 11 / 12 | gt / le | 有符号大于/小于等于 |

RET 不能代替 IRET；普通用户回调执行 RET 是返回到用户跳板，最终现场恢复仍由
内核 IRET 完成。INT 操作数不是 IVT 地址。

## 栈与帧

| 指令 | opcode | 语法 | 后端状态模型 |
|---|---:|---|---|
| PUSH / POP | 40 / 41 | `push a` / `pop d` | 操作当前模式 SP |
| LSP / OSP | 42 / 43 | `lsp a` / `osp d` | 设置/读取当前 SP |
| FTS / STF | 44 / 45 | 无操作数 | FP→SP / SP→FP |
| PUSHFP / POPFP | 46 / 47 | 无操作数 | 保存/恢复当前 FP |
| LFP / OFP | 48 / 49 | `lfp a` / `ofp d` | 设置/读取当前 FP |
| PUSHU / POPU | 60 / 61 | `pushu a` / `popu d` | 内核专用，临时使用 USP |
| NOP | 00 | `nop` | 空操作 |
| HALT | f0 | `halt` | 停机，仅内核使用 |

通用三寄存器编码：Ra 在 [19:16]、Rb 在 [15:12]、Rw 在 [11:8]。
未使用字段由具体指令形式确定。不要将源码中的 SELECT_CC_PSEUDO、FRAMEADDR 等
编译器伪指令当成硬件 opcode，它们必须在发射前展开。

用户构建会拒绝 EI、DI、HALT、IRET、PUSHU、POPU；其它指令的可用性仍受地址权限、
电路用户微码和具体使用方式约束，不表示未被该静态检查列出的指令均可任意访问内核。
