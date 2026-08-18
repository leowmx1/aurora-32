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
    stack[{inner_addr: u24}] => 0x02 @ inner_addr`24
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
	FTS => 0x00000044
	STF => 0x00000045
	PUSHFP => 0x00000046
	POPFP => 0x00000047

	HALT => 0x000000F0
}
; customasm <file_path> -f list,base:16,group:8,between:" ",before:"v2.0 raw\n",after:"" -o <output_path>

LDC r15, input_a
STR r15, ivt[0x0]
LDA r15, stack[0x0]
LSP r15

LDI r14, 0x30
LDI r13, 0xA
LDI r11, 0x0
LDI r10, 0x3D
LDI r9, 0x2B
LDI r8, 0x2D
LDI r7, 0x2A
LDI r6, 0x2F


EI
loop:
	JMP loop

input_a:
 	LDR r12, keyboard
	CMP r12, r9
	JCC z, change_int_func_add
	CMP r12, r8
	JCC z, change_int_func_sub
	CMP r12, r7
	JCC z, change_int_func_mul
	CMP r12, r6
	JCC z, change_int_func_div
	STR r12, terminal
	SUB r12, r14, r12
	MUL r0, r13, r0
	ADD r0, r12, r0
	IRET
change_int_func_add:
	LDC r15, input_b
	STR r15, ivt[0x0]
	STR r9, terminal
	LDI r15, 0x0
	IRET
change_int_func_sub:
	LDC r15, input_b
	STR r15, ivt[0x0]
	STR r8, terminal
	LDI r15, 0x1
	IRET
change_int_func_mul:
	LDC r15, input_b
	STR r15, ivt[0x0]
	STR r7, terminal
	LDI r15, 0x2
	IRET
change_int_func_div:
	LDC r15, input_b
	STR r15, ivt[0x0]
	STR r6, terminal
	LDI r15, 0x3
	IRET
input_b:
	LDR r12, keyboard
	CMP r12, r13
	JCC z, calc
	STR r12, terminal
	SUB r12, r14, r12
	MUL r1, r13, r1
	ADD r1, r12, r1
	IRET
calc:
	STR r10, terminal
	LDI r2, 0x0
	CMP r15, r2
	CAC z, .add
	LDI r2, 0x1
	CMP r15, r2
	CAC z, .sub
	LDI r2, 0x2
	CMP r15, r2
	CAC z, .mul
	LDI r2, 0x3
	CMP r15, r2
	CAC z, .div
	LDR r2, ram[0x0]
	LDI r15, 0x0
	LDI r5, 0x0
	JMP .loop
	.add:
		ADD r0, r1, r2
		STR r2, ram[0x0]
		RET
	.sub:
		SUB r0, r1, r2
		STR r2, ram[0x0]
		RET
	.mul:
		MUL r0, r1, r2
		STR r2, ram[0x0]
		RET
	.div:
		DIV r0, r1, r2
		STR r2, ram[0x0]
		RET
	.loop:
		REM r2, r13, r3
		PUSH r3
		DIV r2, r13, r2
		CMP r2, r11
		INC r5, r5
		JCC z, .output
		JMP .loop
	.output:
		CMP r15, r5
		JCC z, .end
		POP r4
		ADD r4, r14, r4
		STR r4, terminal
		INC r15, r15
		JMP .output
	.end:
		LDC r15, input_a
		STR r15, ivt[0x0]
		LDI r0, 0x0
		LDI r1, 0x0
		STR r13, terminal
		IRET
	
	