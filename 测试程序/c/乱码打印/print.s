	.file	"print.c"
	.text
	.globl	_start                          ; -- Begin function _start
	.p2align	2
	.type	_start,@function
_start:                                 ; @_start
; %bb.0:
	ldi	r14, 0x02000000
	lsp	r14
	lfp	r14
	osp	r14
	ldi	r15, 0x0000040e
	add	r14, r15, r14
	lsp	r14
	ofp	r14
	ldi	r15, 0x00000005
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000004
	add	r14, r15, r14
	ist	r5, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000003
	add	r14, r15, r14
	ist	r6, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000002
	add	r14, r15, r14
	ist	r7, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000001
	add	r14, r15, r14
	ist	r8, r14                         ; 4-byte Folded Spill
	ofp	r14
	ist	r9, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x03000001
	lda	r1, handle_input
	ist	r1, r0
	ldi	r0, 0x01ffffe1
	ofp	r14
	ldi	r15, 0x00000408
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r1, 0x00000050
	ofp	r14
	ldi	r15, 0x0000040d
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ist	r1, r0
	ldi	r0, 0x01ffffe2
	ldi	r1, 0x0000006c
	ofp	r14
	ldi	r15, 0x00000407
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ist	r1, r0
	ldi	r0, 0x01ffffe3
	ldi	r2, 0x00000065
	ist	r2, r0
	ldi	r0, 0x01ffffe4
	ldi	r13, 0x00000061
	ist	r13, r0
	ldi	r0, 0x01ffffe5
	ldi	r1, 0x00000073
	ist	r1, r0
	ldi	r0, 0x01ffffe6
	ist	r2, r0
	mov	r2, r10
	ofp	r14
	ldi	r15, 0x0000040b
	add	r14, r15, r14
	ist	r10, r14                        ; 4-byte Folded Spill
	ldi	r2, 0x01ffffe7
	ldi	r0, 0x00000020
	ist	r0, r2
	ldi	r2, 0x01ffffe8
	ldi	r3, 0x00000069
	ist	r3, r2
	mov	r3, r4
	ofp	r14
	ldi	r15, 0x0000040c
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ldi	r2, 0x01ffffe9
	ldi	r12, 0x0000006e
	ist	r12, r2
	ldi	r2, 0x01ffffea
	ldi	r3, 0x00000070
	ofp	r14
	ldi	r15, 0x0000040a
	add	r14, r15, r14
	ist	r3, r14                         ; 4-byte Folded Spill
	ist	r3, r2
	ldi	r2, 0x01ffffeb
	ldi	r3, 0x00000075
	ofp	r14
	ldi	r15, 0x00000409
	add	r14, r15, r14
	ist	r3, r14                         ; 4-byte Folded Spill
	ist	r3, r2
	ldi	r3, 0x01ffffec
	ldi	r2, 0x00000074
	ist	r2, r3
	ldi	r3, 0x01ffffed
	ist	r0, r3
	ldi	r3, 0x01ffffee
	ist	r2, r3
	ldi	r3, 0x01ffffef
	ist	r13, r3
	ldi	r3, 0x01fffff0
	ldi	r5, 0x00000072
	ist	r5, r3
	ldi	r3, 0x01fffff1
	ldi	r11, 0x00000067
	ist	r11, r3
	ldi	r3, 0x01fffff2
	ist	r10, r3
	ldi	r3, 0x01fffff3
	ist	r2, r3
	ldi	r3, 0x01fffff4
	ist	r0, r3
	ldi	r3, 0x01fffff5
	ist	r1, r3
	ldi	r3, 0x01fffff6
	ist	r2, r3
	ldi	r3, 0x01fffff7
	ist	r5, r3
	ldi	r3, 0x01fffff8
	ist	r4, r3
	ldi	r3, 0x01fffff9
	ist	r12, r3
	ldi	r3, 0x01fffffa
	ist	r11, r3
	ldi	r10, 0x01fffffb
	ldi	r3, 0x0000003a
	ist	r3, r10
	ldi	r10, 0x01fffffc
	ldi	r6, 0x0000000a
	ist	r6, r10
	ldi	r4, 0x01fffffd
	ldi	r10, 0x0000003e
	ist	r10, r4
	ldi	r4, 0x01fffffe
	ist	r0, r4
	ldi	r4, 0x01ffffff
	ldi	r7, 0x00000000
	ist	r7, r4
	ldi	r4, 0x01ffffe0
	ofp	r14
	ldi	r15, 0x00000406
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ist	r7, r4
	ldi	r4, 0x01ffffdf
	ist	r7, r4
	ldi	r9, 0x01ffff82
	ldi	r8, 0x00000038
	ist	r8, r9
	ldi	r8, 0x01ffff83
	ist	r1, r8
	ldi	r1, 0x01ffff84
	ldi	r8, 0x00000021
	ist	r8, r1
	ldi	r1, 0x01ffff85
	ldi	r8, 0x00000047
	ist	r8, r1
	ldi	r1, 0x01ffff86
	ldi	r8, 0x0000007a
	ist	r8, r1
	ldi	r1, 0x01ffff87
	ldi	r8, 0x00000029
	ist	r8, r1
	ldi	r1, 0x01ffff88
	ldi	r8, 0x00000033
	ist	r8, r1
	ldi	r1, 0x01ffff89
	ldi	r8, 0x00000058
	ist	r8, r1
	ldi	r1, 0x01ffff8a
	ldi	r8, 0x00000071
	ist	r8, r1
	ldi	r1, 0x01ffff8b
	ldi	r8, 0x0000003b
	ist	r8, r1
	ldi	r1, 0x01ffff8c
	ldi	r8, 0x0000004e
	ist	r8, r1
	ldi	r1, 0x01ffff8d
	ist	r0, r1
	ldi	r0, 0x01ffff8e
	ldi	r1, 0x00000055
	ist	r1, r0
	ldi	r0, 0x01ffff8f
	ldi	r1, 0x0000006d
	ist	r1, r0
	ldi	r0, 0x01ffff90
	ldi	r1, 0x00000041
	ist	r1, r0
	ldi	r0, 0x01ffff91
	ldi	r1, 0x0000002f
	ist	r1, r0
	ldi	r0, 0x01ffff92
	ldi	r1, 0x00000037
	ist	r1, r0
	ldi	r0, 0x01ffff93
	ldi	r1, 0x0000006b
	ist	r1, r0
	ldi	r0, 0x01ffff94
	ldi	r1, 0x0000003f
	ist	r1, r0
	ldi	r0, 0x01ffff95
	ldi	r1, 0x00000059
	ist	r1, r0
	ldi	r0, 0x01ffff96
	ldi	r1, 0x00000060
	ist	r1, r0
	ldi	r0, 0x01ffff97
	ldi	r1, 0x0000005e
	ist	r1, r0
	ldi	r0, 0x01ffff98
	ldi	r1, 0x00000066
	ist	r1, r0
	ldi	r0, 0x01ffff99
	ldi	r1, 0x00000034
	ist	r1, r0
	ldi	r0, 0x01ffff9a
	ldi	r1, 0x0000005c
	ist	r1, r0
	ldi	r0, 0x01ffff9b
	ldi	r1, 0x00000064
	ist	r1, r0
	ldi	r0, 0x01ffff9c
	ldi	r1, 0x0000006f
	ist	r1, r0
	ldi	r0, 0x01ffff9d
	ldi	r1, 0x00000052
	ist	r1, r0
	ldi	r0, 0x01ffff9e
	ist	r3, r0
	ldi	r0, 0x01ffff9f
	ldi	r1, 0x00000049
	ist	r1, r0
	ldi	r0, 0x01ffffa0
	ldi	r1, 0x00000023
	ist	r1, r0
	ldi	r0, 0x01ffffa1
	ldi	r1, 0x00000063
	ist	r1, r0
	ldi	r0, 0x01ffffa2
	ldi	r1, 0x0000004a
	ist	r1, r0
	ldi	r0, 0x01ffffa3
	ldi	r1, 0x00000030
	ist	r1, r0
	ldi	r0, 0x01ffffa4
	ofp	r14
	ldi	r15, 0x0000040d
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffa5
	ist	r10, r0
	ldi	r0, 0x01ffffa6
	ldi	r1, 0x00000062
	ist	r1, r0
	ldi	r0, 0x01ffffa7
	ldi	r1, 0x0000004c
	ist	r1, r0
	ldi	r0, 0x01ffffa8
	ldi	r1, 0x00000031
	ist	r1, r0
	ldi	r0, 0x01ffffa9
	ldi	r1, 0x0000007c
	ist	r1, r0
	ldi	r0, 0x01ffffaa
	ldi	r1, 0x00000076
	ist	r1, r0
	ldi	r0, 0x01ffffab
	ldi	r1, 0x00000048
	ist	r1, r0
	ldi	r0, 0x01ffffac
	ist	r2, r0
	ldi	r0, 0x01ffffad
	ldi	r1, 0x00000035
	ist	r1, r0
	ldi	r0, 0x01ffffae
	ldi	r1, 0x0000005a
	ist	r1, r0
	ldi	r0, 0x01ffffaf
	ldi	r1, 0x0000002e
	ist	r1, r0
	ldi	r0, 0x01ffffb0
	ldi	r1, 0x00000057
	ist	r1, r0
	ldi	r0, 0x01ffffb1
	ldi	r1, 0x00000053
	ist	r1, r0
	ldi	r0, 0x01ffffb2
	ldi	r1, 0x00000026
	ist	r1, r0
	ldi	r0, 0x01ffffb3
	ist	r13, r0
	ldi	r0, 0x01ffffb4
	ldi	r1, 0x0000003c
	ist	r1, r0
	ldi	r0, 0x01ffffb5
	ldi	r1, 0x00000039
	ist	r1, r0
	ldi	r0, 0x01ffffb6
	ldi	r1, 0x0000005d
	ist	r1, r0
	ldi	r0, 0x01ffffb7
	ldi	r1, 0x0000007b
	ist	r1, r0
	ldi	r0, 0x01ffffb8
	ldi	r1, 0x00000045
	ist	r1, r0
	ldi	r0, 0x01ffffb9
	ist	r12, r0
	ldi	r0, 0x01ffffba
	ist	r5, r0
	ldi	r0, 0x01ffffbb
	ldi	r1, 0x00000032
	ist	r1, r0
	ldi	r0, 0x01ffffbc
	ldi	r1, 0x00000040
	ist	r1, r0
	ldi	r0, 0x01ffffbd
	ldi	r1, 0x0000004d
	ist	r1, r0
	ldi	r0, 0x01ffffbe
	ldi	r1, 0x00000024
	ist	r1, r0
	ldi	r0, 0x01ffffbf
	ldi	r8, 0x0000005f
	ist	r8, r0
	ldi	r0, 0x01ffffc0
	ldi	r1, 0x00000068
	ist	r1, r0
	ldi	r0, 0x01ffffc1
	ldi	r1, 0x0000002c
	ist	r1, r0
	ldi	r0, 0x01ffffc2
	ldi	r1, 0x0000004f
	ist	r1, r0
	ldi	r0, 0x01ffffc3
	ofp	r14
	ldi	r15, 0x0000040c
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffc4
	ldi	r1, 0x00000051
	ist	r1, r0
	ldi	r0, 0x01ffffc5
	ldi	r1, 0x0000007e
	ist	r1, r0
	ldi	r0, 0x01ffffc6
	ldi	r1, 0x00000046
	ist	r1, r0
	ldi	r0, 0x01ffffc7
	ist	r11, r0
	ldi	r0, 0x01ffffc8
	ldi	r1, 0x00000036
	ist	r1, r0
	ldi	r0, 0x01ffffc9
	ldi	r1, 0x0000007d
	ist	r1, r0
	ldi	r0, 0x01ffffca
	ldi	r1, 0x0000002d
	ist	r1, r0
	ldi	r0, 0x01ffffcb
	ldi	r1, 0x00000022
	ist	r1, r0
	ldi	r0, 0x01ffffcc
	ldi	r1, 0x0000003d
	ist	r1, r0
	ldi	r0, 0x01ffffcd
	ldi	r1, 0x00000028
	ist	r1, r0
	ldi	r0, 0x01ffffce
	ldi	r1, 0x00000042
	ist	r1, r0
	ldi	r0, 0x01ffffcf
	ldi	r1, 0x00000025
	ist	r1, r0
	ldi	r0, 0x01ffffd0
	ldi	r1, 0x0000002a
	ist	r1, r0
	ldi	r0, 0x01ffffd1
	ldi	r1, 0x00000077
	ist	r1, r0
	ldi	r0, 0x01ffffd2
	ldi	r1, 0x0000004b
	ist	r1, r0
	ldi	r0, 0x01ffffd3
	ofp	r14
	ldi	r15, 0x00000407
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffd4
	ldi	r1, 0x00000079
	ist	r1, r0
	ldi	r0, 0x01ffffd5
	ldi	r1, 0x00000044
	ist	r1, r0
	ldi	r0, 0x01ffffd6
	ldi	r1, 0x00000027
	ist	r1, r0
	ldi	r0, 0x01ffffd7
	ldi	r1, 0x00000043
	ist	r1, r0
	ldi	r0, 0x01ffffd8
	ldi	r1, 0x0000005b
	ist	r1, r0
	ldi	r0, 0x01ffffd9
	ofp	r14
	ldi	r15, 0x0000040a
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffda
	ldi	r1, 0x00000054
	ist	r1, r0
	ldi	r0, 0x01ffffdb
	ofp	r14
	ldi	r15, 0x00000409
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffdc
	ofp	r14
	ldi	r15, 0x0000040b
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffdd
	ldi	r1, 0x0000006a
	ist	r1, r0
	ldi	r0, 0x01ffffde
	ldi	r1, 0x0000002b
	ist	r1, r0
	ldi	r0, 0x01fffb82
	ldi	r1, 0x00000400
	ofp	r14
	ldi	r15, 0x00000407
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	call	__aurora_zero_words
	;APP
	di

	;NO_APP
	ofp	r14
	ldi	r15, 0x00000406
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r7, r0
	ofp	r14
	ldi	r15, 0x00000408
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	call	__aurora_put_string
	;APP
	ei

	;NO_APP
.LBB0_1:                                ; =>This Inner Loop Header: Depth=1
	ild	r0, r4
	cmp	r0, r7
	jcc	z, .LBB0_1
; %bb.2:
	ldi	r0, 0x05000000
	ist	r6, r0
	ofp	r14
	ldi	r15, 0x00000406
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ild	r2, r1
	ldi	r1, 0x00000001
	cmp	r2, r1
	jcc	lt, .LBB0_17
; %bb.3:
	ofp	r2
	ldi	r15, 0x00000006
	add	r2, r15, r2
	ldi	r3, 0x00000100
	mov	r7, r10
	jmp	.LBB0_4
.LBB0_15:                               ;   in Loop: Header=BB0_4 Depth=1
	add	r2, r10, r12
	ist	r11, r0
	ist	r11, r12
	ist	r6, r0
.LBB0_16:                               ;   in Loop: Header=BB0_4 Depth=1
	add	r10, r1, r10
	ofp	r14
	ldi	r15, 0x00000406
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ild	r11, r11
	cmp	r10, r11
	jcc	ge, .LBB0_17
.LBB0_4:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_5 Depth 2
                                        ;       Child Loop BB0_8 Depth 3
                                        ;       Child Loop BB0_10 Depth 3
                                        ;     Child Loop BB0_14 Depth 2
	ofp	r14
	ldi	r15, 0x00000407
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	add	r10, r11, r11
	ild	r11, r11
	mov	r7, r12
.LBB0_5:                                ;   Parent Loop BB0_4 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB0_8 Depth 3
                                        ;       Child Loop BB0_10 Depth 3
	add	r12, r9, r13
	ild	r13, r13
	cmp	r11, r13
	jcc	z, .LBB0_12
; %bb.6:                                ;   in Loop: Header=BB0_5 Depth=2
	cmp	r10, r7
	jcc	z, .LBB0_9
; %bb.7:                                ;   in Loop: Header=BB0_5 Depth=2
	mov	r7, r4
.LBB0_8:                                ;   Parent Loop BB0_4 Depth=1
                                        ;     Parent Loop BB0_5 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	r2, r4, r5
	ild	r5, r5
	ist	r5, r0
	add	r4, r1, r4
	cmp	r10, r4
	jcc	nz, .LBB0_8
.LBB0_9:                                ;   in Loop: Header=BB0_5 Depth=2
	ist	r13, r0
	ist	r6, r0
	mov	r3, r13
.LBB0_10:                               ;   Parent Loop BB0_4 Depth=1
                                        ;     Parent Loop BB0_5 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	sub	r13, r1, r13
	;APP
	nop

	;NO_APP
	cmp	r13, r7
	jcc	nz, .LBB0_10
; %bb.11:                               ;   in Loop: Header=BB0_5 Depth=2
	add	r12, r1, r13
	cmp	r12, r8
	mov	r13, r12
	jcc	c, .LBB0_5
	jmp	.LBB0_16
.LBB0_12:                               ;   in Loop: Header=BB0_4 Depth=1
	cmp	r10, r7
	jcc	z, .LBB0_15
; %bb.13:                               ;   in Loop: Header=BB0_4 Depth=1
	mov	r7, r12
.LBB0_14:                               ;   Parent Loop BB0_4 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	r2, r12, r13
	ild	r13, r13
	ist	r13, r0
	add	r12, r1, r12
	cmp	r10, r12
	jcc	nz, .LBB0_14
	jmp	.LBB0_15
.LBB0_17:
	;APP
	halt

	;NO_APP
.Lfunc_end0:
	.size	_start, .Lfunc_end0-_start
                                        ; -- End function
	.globl	handle_input                    ; -- Begin function handle_input
	.p2align	2
	.type	handle_input,@function
handle_input:                           ; @handle_input
; %bb.0:
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	ldi	r1, 0x01ffffdf
	ild	r0, r1
	ldi	r2, 0x00000000
	cmp	r0, r2
	jcc	nz, .LBB1_6
; %bb.1:
	ldi	r0, 0x04000000
	ild	r0, r0
	ldi	r2, 0x0000000d
	cmp	r0, r2
	jcc	z, .LBB1_3
; %bb.2:
	ldi	r2, 0x0000000a
	cmp	r0, r2
	jcc	nz, .LBB1_4
.LBB1_3:
	ldi	r0, 0x00000001
	jmp	.LBB1_5
.LBB1_4:
	ldi	r1, 0x01ffffe0
	ild	r2, r1
	ldi	r3, 0x00000001
	add	r2, r3, r3
	ist	r3, r1
	ldi	r1, 0x01fffb82
	add	r2, r1, r1
	ist	r0, r1
	ldi	r1, 0x05000000
.LBB1_5:
	ist	r0, r1
.LBB1_6:
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	iret
.Lfunc_end1:
	.size	handle_input, .Lfunc_end1-handle_input
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_put_string
	.type	__aurora_put_string,@function
__aurora_put_string:                    ; @__aurora_put_string
; %bb.0:
	ild	r2, r0
	ldi	r1, 0x000000ff
	and	r2, r1, r3
	ldi	r2, 0x00000000
	cmp	r3, r2
	jcc	z, .LBB2_3
; %bb.1:
	ldi	r10, 0x00000001
	add	r0, r10, r11
	ldi	r0, 0x05000000
.LBB2_2:                                ; =>This Inner Loop Header: Depth=1
	ist	r3, r0
	add	r11, r10, r12
	ild	r3, r11
	and	r3, r1, r3
	cmp	r3, r2
	mov	r12, r11
	jcc	nz, .LBB2_2
.LBB2_3:
	ret
.Lfunc_end2:
	.size	__aurora_put_string, .Lfunc_end2-__aurora_put_string
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_zero_words
	.type	__aurora_zero_words,@function
__aurora_zero_words:                    ; @__aurora_zero_words
; %bb.0:
	jmp	.LBB3_1
.LBB3_1:                                ; =>This Inner Loop Header: Depth=1
	ldi	r2, 0x00000000
	ist	r2, r0
	ldi	r3, 0x00000001
	add	r0, r3, r0
	sub	r1, r3, r1
	cmp	r1, r2
	jcc	nz, .LBB3_1
	jmp	.LBB3_2
.LBB3_2:
	ret
.Lfunc_end3:
	.size	__aurora_zero_words, .Lfunc_end3-__aurora_zero_words
                                        ; -- End function
	.ident	"clang version 24.0.0git"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __aurora_zero_words
