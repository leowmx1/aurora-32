#!/usr/bin/env python3
"""
微码控制字解析 / 生成工具 (支持主CU 30位 和 中断CU 32位)
用法：
  python ucode_tool.py
  进入交互后：
    · cu main / cu int ：切换CU模式
    · 默认模式 1：输入 hex → 输出信号（支持长串自动切分）
    · 输入 mode 2 切换到模式 2：输入信号列表 → 输出 hex
        支持多组输入，用 / 分隔，例如：ENDCYCLE,OE_MDREXT,WE / OE_Ra,LD_Rw
        输出不含 0x 前缀，每组一行。
    · 输入 q 退出
"""

# 主CU 30位 位定义
BITS_MAIN = {
    29: "ENDCYCLE",
    28: "CONDJMP",
    27: "ALUMODE3",
    26: "ALUMODE2",
    25: "ALUMODE1",
    24: "ALUMODE0",
    23: "WE",
    22: "OE",
    21: "INC_PC",
    20: "SET_IE",
    19: "CLR_IE",
    18: "OE_ALU",
    17: "OE_RFlags",
    16: "OE_Rb",
    15: "OE_Ra",
    14: "OE_MDREXT",
    13: "OE_MDRINT",
    12: "OE_PC",
    11: "OE_SP",
    10: "LD_RFlags_Bus",
    9:  "LD_RFlags",
    8:  "LD_Rw",
    7:  "LD_IR",
    6:  "LD_MDREXT",
    5:  "LD_MDRINT",
    4:  "LD_MAR",
    3:  "LD_PC",
    2:  "LD_SP",
    1:  "UP_SP",
    0:  "DN_SP",
}

# 中断CU 32位 位定义 (主CU 30位 + 高2位)
BITS_INT = {
    31: "INT_RET",
    30: "OE_INTAC",
    **{k: v for k, v in BITS_MAIN.items()},  # 继承主CU的低30位
}

# 当前CU模式和位宽 (全局变量)
current_cu = "main"
current_bits = BITS_MAIN
current_width = 30

def hex_to_signals(hex_str):
    """hex → 激活的信号列表（ALUMODE 总是显示）"""
    val = int(hex_str, 16)
    signals = []
    alumode_val = 0
    for bit in range(current_width):
        if (val >> bit) & 1:
            name = current_bits.get(bit)
            if name is None:
                continue
            if 24 <= bit <= 27:          # ALUMODE 位
                alumode_val |= (1 << (bit - 24))
            else:
                signals.append(name)
    signals.append(f"ALUMODE={alumode_val}")
    return ", ".join(signals)

def signals_to_hex(signal_str):
    """信号列表 → hex 字符串（不含 0x 前缀，默认 ALUMODE=15）"""
    signals = [s.strip() for s in signal_str.split(",") if s.strip()]
    val = 0
    alumode = 15          # 默认 15
    for sig in signals:
        if sig.upper().startswith("ALUMODE="):
            alumode = int(sig.split("=")[1])
        else:
            found = False
            for bit, name in current_bits.items():
                if name.upper() == sig.upper():
                    val |= (1 << bit)
                    found = True
                    break
            if not found:
                print(f"⚠ 未知信号 '{sig}'，已忽略")
    # 清除原 ALUMODE 位，设置新值
    val &= ~(0xF << 24)            # ALUMODE 在 24-27 位
    val |= (alumode & 0xF) << 24
    hex_len = 8 if current_width == 30 else 8
    return f"{val:0{hex_len}X}"

def split_hex(raw: str):
    """去掉空白，按 hex_len 位一组切分，过滤长度不足的片段"""
    hex_len = 8 if current_width == 30 else 8
    raw = raw.replace(" ", "").replace("\t", "")
    groups = []
    for i in range(0, len(raw), hex_len):
        chunk = raw[i:i+hex_len]
        if len(chunk) == hex_len:
            groups.append(chunk)
    return groups

def main():
    global current_cu, current_bits, current_width   # 声明全局变量，移到函数最前面
    mode = 1
    print("微码控制字工具 (主CU 30位 / 中断CU 32位) | mode 1/2 切换 | cu main/int | q 退出")
    print(f"[CU: {current_cu}] [模式 {mode}] hex → 信号")

    while True:
        try:
            user = input("> ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n退出")
            break
        if not user:
            continue
        if user.lower() == "q":
            break
        if user.lower().startswith("cu"):
            parts = user.split()
            if len(parts) == 2 and parts[1].lower() in ("main", "int"):
                current_cu = parts[1].lower()
                if current_cu == "main":
                    current_bits = BITS_MAIN
                    current_width = 30
                else:
                    current_bits = BITS_INT
                    current_width = 32
                print(f"[CU: {current_cu}] [模式 {mode}] {'hex → 信号' if mode==1 else '信号 → hex'}")
                continue
            else:
                print("用法: cu main 或 cu int")
                continue
        if user.lower().startswith("mode"):
            parts = user.split()
            if len(parts) == 2 and parts[1] in ("1", "2"):
                mode = int(parts[1])
                print(f"[CU: {current_cu}] [模式 {mode}] {'hex → 信号' if mode==1 else '信号 → hex'}")
                continue
            else:
                print("用法: mode 1 或 mode 2")
                continue

        if mode == 1:
            groups = split_hex(user)
            if not groups:          # 若没能按 hex_len 切分，尝试空格分隔
                groups = user.split()
            for h in groups:
                hex_len = 8 if current_width == 30 else 8
                if len(h) != hex_len:
                    print(f"⚠ 长度不为{hex_len}，跳过: {h}")
                    continue
                try:
                    sigs = hex_to_signals(h)
                    print(f"{h} : {sigs}")
                except ValueError:
                    print(f"❌ 非法 hex: {h}")
        else:
            # 模式 2：支持多组信号，用 / 分隔
            groups = user.split("/")
            for g in groups:
                hex_val = signals_to_hex(g.strip())
                print(hex_val)

if __name__ == "__main__":
    main()