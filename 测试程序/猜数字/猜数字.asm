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
	ivt[{inner_addr: u24}] => 0x03 @ inner_addr`24
	keyboard => 0x04000000
	terminal => 0x05000000
	telnet => 0x06000000
}
#subruledef cond
{
	c => 0x1
    z => 0x2
	b => 0x3
    nc => 0x4
	nz => 0x5
	nb => 0x6
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
	LDA r{reg_w: u4}, {addr: src} => 0x00000 @ reg_w`4 @ 0x16 @ addr`32
	LDC r{reg_w: u4}, {addr: u32} => 0x00000 @ reg_w`4 @ 0x16 @ addr`32
	LDO r{reg_a: u4}, r{reg_b: u4}, r{reg_w: u4} => 0x000 @ reg_a`4 @ reg_b`4 @ reg_w`4 @ 0x17

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
	JCC {cond_c: cond}, {addr: u32} => 0x00 @ cond_c`4 @ 0x00031 @ addr`32
	JPR r{reg_a: u4} => 0x000 @ reg_a`4 @ 0x0032
	JRC {cond_c: cond}, r{reg_a: u4} => 0x00 @ cond_c`4 @ reg_a`4 @ 0x0033
	CALL {addr: u32} => 0x000000 @ 0x34 @ addr`32
	CAC {cond_c: cond}, {addr: u32} => 0x00 @ cond_c`4 @ 0x000 @ 0x35 @ addr`32
	RET => 0x00000036
	INT {addr: u32} => 0x00000037 @ addr`32
	CINT {cond_c: cond}, {addr: u32} => 0x00 @ cond_c`4 @ 0x00038 @ addr`32
	IRET => 0x00000039
	EI => 0x0000003A
	DI => 0x0000003B

	PUSH r{reg_a: u4} => 0x000 @ reg_a`4 @ 0x0040
	POP r{reg_w: u4} => 0x00000 @ reg_w`4 @ 0x41
	LSP r{reg_a: u4} => 0x000 @ reg_a`4 @ 0x0042

	HALT => 0x000000F0
}
; customasm <file_path> -f list,base:16,group:8,between:" ",before:"v2.0 raw\n",after:"" -o <output_path>

LDA r15, sp[0x000000]
LSP r15
LDC r15, int_process
STR r15, ivt[0x000000]
LDI r0, 0x00000003
LDI r3, 0x41C64E6D
LDI r4, 0x00003039
LDI r5, 0x00000009
LDI r14, 0x00000030
EI
loop:
	JMP loop
int_process:
	LDR r1, keyboard
	SUB r1, r14, r1
	CMP r1, r0
	CAC c, smaller
	CAC b, bigger
	CAC z, success
	IRET
bigger:
	LDI r10, 0x0000003E
	STR r10, terminal
	LDI r10, 0x0000000A
	STR r10, terminal
	RET
smaller:
	LDI r10, 0x0000003C
	STR r10, terminal
	LDI r10, 0x0000000A
	STR r10, terminal
	RET
success:
	LDI r10, 0x0000003D
	STR r10, terminal
	LDI r10, 0x0000000A
	STR r10, terminal
	INC r11, r11
	ADD r11, r14, r12
	STR r12, terminal
	LDI r10, 0x0000000A
	STR r10, terminal
	CALL random
	RET
random:
	MUL r3, r0, r0
	ADD r0, r4, r0
	REM r0, r5, r0
	RET

