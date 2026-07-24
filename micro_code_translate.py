#!/usr/bin/env python3
"""
微码控制字解析 / 生成工具（28 位）
用法：
  python ucode_tool.py
  进入交互后：
    · 默认模式 1：输入 hex → 输出信号（支持长串自动切分）
    · 输入 mode 2 切换到模式 2：输入信号列表 → 输出 hex
        支持多组输入，用 / 分隔，例如：ENDCYCLE,OE_MDREXT,WE / OE_Ra,LD_Rw
        输出不含 0x 前缀，每组一行。
    · 输入 q 退出
"""

BITS = {
    27: "ENDCYCLE",
    26: "CONDJMP",
    25: "ALUMODE3",
    24: "ALUMODE2",
    23: "ALUMODE1",
    22: "ALUMODE0",
    21: "WE",
    20: "OE",
    19: "INC_PC",
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

def hex_to_signals(hex_str):
    """7 位 hex → 激活的信号列表（ALUMODE 总是显示）"""
    val = int(hex_str, 16)
    signals = []
    alumode_val = 0
    for bit in range(28):
        if (val >> bit) & 1:
            name = BITS.get(bit)
            if name is None:
                continue
            if 22 <= bit <= 25:          # ALUMODE 位
                alumode_val |= (1 << (bit - 22))
            else:
                signals.append(name)
    signals.append(f"ALUMODE={alumode_val}")
    return ", ".join(signals)

def signals_to_hex(signal_str):
    """信号列表 → 7 位 hex 字符串（不含 0x 前缀）"""
    signals = [s.strip() for s in signal_str.split(",") if s.strip()]
    val = 0
    alumode = 15          # 默认 15
    for sig in signals:
        if sig.upper().startswith("ALUMODE="):
            alumode = int(sig.split("=")[1])
        else:
            found = False
            for bit, name in BITS.items():
                if name.upper() == sig.upper():
                    val |= (1 << bit)
                    found = True
                    break
            if not found:
                print(f"⚠ 未知信号 '{sig}'，已忽略")
    # 设置 ALUMODE
    val &= ~(0xF << 22)
    val |= (alumode & 0xF) << 22
    return f"{val:07X}"

def split_hex(raw: str):
    """去掉空白，按 7 位一组切分，过滤长度不足 7 的片段"""
    raw = raw.replace(" ", "").replace("\t", "")
    groups = []
    for i in range(0, len(raw), 7):
        chunk = raw[i:i+7]
        if len(chunk) == 7:
            groups.append(chunk)
    return groups

def main():
    mode = 1
    print("微码控制字工具 (28位) | mode 1/2 切换 | q 退出")
    print(f"[模式 {mode}] hex → 信号")

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
        if user.lower().startswith("mode"):
            parts = user.split()
            if len(parts) == 2 and parts[1] in ("1", "2"):
                mode = int(parts[1])
                print(f"[模式 {mode}] {'hex → 信号' if mode==1 else '信号 → hex'}")
                continue
            else:
                print("用法: mode 1 或 mode 2")
                continue

        if mode == 1:
            groups = split_hex(user)
            if not groups:          # 若没能按7位切分，尝试空格分隔
                groups = user.split()
            for h in groups:
                if len(h) != 7:
                    print(f"⚠ 长度不为7，跳过: {h}")
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