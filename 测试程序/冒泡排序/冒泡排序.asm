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
    OSP r{reg_w: u4} => 0x00000 @ reg_w`4 @ 0x43

	HALT => 0x000000F0
}
; customasm <file_path> -f list,base:16,group:8,between:" ",before:"v2.0 raw\n",after:"" -o <output_path>

LDA r15, stack[0x000000]
LSP r15
LDA r10, ram[0x000000]
LDC r15, input
STR r15, ivt[0x0]
LDI r8, 0x30
LDI r7, 0xA
LDI r6, 0x5D
LDI r4, 0x3C
EI
waitfor_input:
	CMP r10, r11
	JCC c, loop
	JMP waitfor_input
loop:
	DI
	ILD r0, r10
	INC r10, r10
	ILD r1, r10
	CMP r1, r0
	CAC c, swap
	CMP r10, r11
	CAC z, next_round
	JMP loop
swap:
	IST r0, r10
	DEC r10, r10
	IST r1, r10
	INC r10, r10
	INC r12, r12
	RET
next_round:
	CMP r12, r13
	JCC z, end_sort
	LDA r10, ram[0x000000]
	LDI r12, 0x00000000
	RET
end_sort:
	LDA r10, ram[0x0]
	LDI r15, 0x0
	.to_str:
		ILD r0, r10
		..to_str_loop:
			REM r0, r7, r1
			DIV r0, r7, r0
			PUSH r1
			INC r2, r2
			CMP r0, r15
			JCC z, ..end_num_loop
			JMP ..to_str_loop
		..end_num_loop:
			POP r1
			ADD r1, r8, r1
			STR r1, terminal
			DEC r2, r2
			CMP r2, r15
			JCC z, ..next_output_num
			JMP ..end_num_loop
		..next_output_num:
			CMP r10, r11
			JCC z, .end_output
			STR r4, terminal
			INC r10, r10
			JMP .to_str
	.end_output:
		HALT


input:
	LDR r9, keyboard
	CMP r9, r7
	JCC z, next_num
	CMP r9, r6
	JCC z, end_input
	STR r9, terminal
	SUB r9, r8, r9
	MUL r5, r7, r5
	ADD r5, r9, r5
	IRET
next_num:
	IST r5, r10
	LDI r5, 0x0
	INC r10, r10
	STR r7, terminal
	IRET
end_input:
	DEC r10, r10
	MOV r10, r11
	LDA r10, ram[0x0]
	IRET
	