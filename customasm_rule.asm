#bankdef global
{
    bits = 32
    addr = 0x00000000
    size = 0x00FFFFFF
    outp = 0
	labelalign = 32
}
#bank global
#addr 0x00000000
#subruledef src
{
	rom[{inner_addr: u24}] => 0x00 @ inner_addr`24
    ram[{inner_addr: u24}] => 0x01 @ inner_addr`24
    sp[{inner_addr: u24}] => 0x02 @ inner_addr`24
}

#ruledef
{
	NOP => 0x00000000

	MOV r{reg_a: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ 0x0 @ reg_w`4 @ 0x10
    ILD r{reg_w: u4}, r{reg_b: u4} => 0x0000 @ reg_b`4 @ reg_w`4 @ 0x11
	IST r{reg_a: u4}, r{reg_b: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ 0x012
	LDR r{reg_w: u4}, {addr: src} => 0x00000 @ reg_w`4 @ 0x13 @ addr`32
	STR r{reg_a: u4}, {addr: src} => 0x000 @ reg_a`4 @ 0x00 @ 0x14 @ addr`32
	LDI r{reg_w: u4}, {value: u32} => 0x00000 @ reg_w`4 @ 0x15 @ value`32

	ADD r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x20
	SUB r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x21
	INC r{reg_a: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ 0x0 @ reg_w`4 @ 0x22
	DEC r{reg_a: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ 0x0 @ reg_w`4 @ 0x23
	MUL r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x24
	DIV r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x25
	REM r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x26
	SFT r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x27
	AND r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x28
	NAND r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x29
	OR r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x2A
	NOR r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x2B
	XOR r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x2C
	XNOR r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x2D
	NOT r{reg_a: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ 0x0 @ reg_w`4 @ 0x2E
	CMP r{reg_a: u4}, r{reg_b: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ 0x02F

	JMP {addr: u32} => 0x00000030 @ addr`32
	JPR r{reg_a: u4} => 0x000 @ reg_a`4 @ 0x0031
	JC {addr: u32} => 0x00000032 @ addr`32
	JZ {addr: u32} => 0x00000033 @ addr`32
	JNC {addr: u32} => 0x00000034 @ addr`32
	JNZ {addr: u32} => 0x00000035 @ addr`32
	CALL r{reg_p: u4}, {addr: src} => 0x000 @ reg_p`4 @ 0x0 @ reg_p`4 @ 0x36 @ addr`32
	RET r{reg_p: u4} => 0x000 @ reg_p`4 @ 0x0 @ reg_p`4 @ 0x37

	HALT => 0x000000F0
}
; customasm <file_path> -f list,base:16,group:8,between:" ",before:"v2.0 raw\n",after:"" -o <output_path>