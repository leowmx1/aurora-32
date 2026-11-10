# USP/UFP 内核访问测试

测试新增的四条单字、内核专用指令：

| 指令 | opcode | 操作 |
|---|---|---|
| LUSP Ra | `0x62` | USP ← Ra |
| LUFP Ra | `0x63` | UFP ← Ra |
| OUSP Rw | `0x64` | Rw ← USP |
| OUFP Rw | `0x65` | Rw ← UFP |

输入 Ra 位于 [19:16]，输出 Rw 位于 [11:8]。它们应保持 KSP、KFP、FLAGS、IE、
当前内核模式和内存不变，不需要临时切换为用户模式。

## 运行测试

在仓库根目录执行：

```powershell
./tests/user-context/test.ps1
```

脚本用随包编译器重建 `build/contract.hex`，验证四条指令在用户构建中全部被拒绝，
核对全部 16 个通用寄存器的编码和反汇编（64 组），再执行 1024 组理想 ISA 状态检查。
最后在指令模型中执行实际编译出来的 ROM BIN。模型对于用户态执行这些指令报错；
不模拟硬件非法指令 IRQ 的锁存/响应序列。

同时测试用户全局变量为 1 字时分配到 `0x813fffff`，恰好 `2^22` 字时允许，
多一字时拒绝；同样的 `2^22+1` 字内核对象仍可编译，避免误缩减内核 RAM。

## Digital 实机检查

1. 停止仿真，将 `build/contract.hex` 装入 **ROM**，暂时替换生产内核。
2. 不需要装载任何 UROM 程序或 storage 镜像。
3. 复位运行。测试全程内核态、关闭 IRQ，不运行 tick 调度器，也不修改银行选择寄存器。
4. 正常完成后打印以下内容并 HALT；测试结束后重新加载生产内核。

```text
USP/UFP smoke test: kernel only, IRQs off.
PASS pointer save/restore, KSP/KFP unchanged.
PASS PUSHU/POPU use the selected USP.
PASS FLAGS preserved (ADD overflow).
USER CONTEXT ALL PASS.
```

测试交替恢复两组软件保存的指针 32 次，但这不是两个用户进程，也没有测试内存银行。
它暂时写入物理 bank 0 UStack 的 `0x82000120`，正常通过后恢复原字，以及原 USP/UFP。
自身使用内核 RAM/KStack；若中途失败停机，可能尚未恢复临时状态，须复位。

当前 PUSHU/POPU 只选择 USP，不切换权限；银行寄存器不会改变其内核态物理访存地址。
模型测试会解码保存的 mcproj，确认该语义，并在四种银行选择值下检查物理地址旁路。
完整银行回调路径另由 `tests/tetris/run_tetris.py` 和 `tests/storage/run_multiprocess.py` 覆盖。

失败编号：1/2 为两组指针读回，3 为交替恢复，4 为 KSP/KFP 隔离，5/6 为 PUSHU/POPU，
7 为压弹后的 KSP/KFP，8 为 ADD 产生的 Z/C/N/V 被改变，9 为恢复原指针失败。

软件模型只验证约定及生成代码，不能证明 Digital 电路或微码时序正确。
