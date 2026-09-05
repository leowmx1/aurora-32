	.file	"print.c"
                                        ; Start of file scope inline assembly
	.globl	__aurora_rom_entry
__aurora_rom_entry:
	jmp	_start

                                        ; End of file scope inline assembly
	.text
	.globl	handle_timer                    ; -- Begin function handle_timer
	.p2align	2
	.type	handle_timer,@function
handle_timer:                           ; @handle_timer
; %bb.0:
	;APP
	push	r0

	ldi	r0, 0x07000000

	ild	r0, r0

	pop	r0

	iret


	;NO_APP
.Lfunc_end0:
	.size	handle_timer, .Lfunc_end0-handle_timer
                                        ; -- End function
	.globl	_start                          ; -- Begin function _start
	.p2align	2
	.type	_start,@function
_start:                                 ; @_start
; %bb.0:
	ldi	r14, 0x02000000
	lsp	r14
	lfp	r14
	osp	r14
	ldi	r15, 0x00000410
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
	ldi	r15, 0x00000406
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r1, 0x00000050
	ofp	r14
	ldi	r15, 0x0000040a
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
	ldi	r1, 0x00000065
	ist	r1, r0
	ldi	r0, 0x01ffffe4
	ldi	r13, 0x00000061
	ist	r13, r0
	ldi	r0, 0x01ffffe5
	ldi	r11, 0x00000073
	ist	r11, r0
	ldi	r0, 0x01ffffe6
	ist	r1, r0
	ofp	r14
	ldi	r15, 0x0000040c
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ldi	r2, 0x01ffffe7
	ldi	r12, 0x00000020
	ist	r12, r2
	ldi	r2, 0x01ffffe8
	ldi	r0, 0x00000069
	ist	r0, r2
	mov	r0, r10
	ofp	r14
	ldi	r15, 0x0000040e
	add	r14, r15, r14
	ist	r10, r14                        ; 4-byte Folded Spill
	ldi	r2, 0x01ffffe9
	ldi	r0, 0x0000006e
	ist	r0, r2
	mov	r0, r4
	ofp	r14
	ldi	r15, 0x0000040f
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ldi	r2, 0x01ffffea
	ldi	r0, 0x00000070
	ofp	r14
	ldi	r15, 0x0000040b
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r2
	ldi	r2, 0x01ffffeb
	ldi	r0, 0x00000075
	ofp	r14
	ldi	r15, 0x00000409
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r2
	ldi	r3, 0x01ffffec
	ldi	r2, 0x00000074
	ist	r2, r3
	ldi	r3, 0x01ffffed
	ist	r12, r3
	ldi	r3, 0x01ffffee
	ist	r2, r3
	ldi	r3, 0x01ffffef
	ist	r13, r3
	ldi	r3, 0x01fffff0
	ldi	r5, 0x00000072
	ist	r5, r3
	ldi	r3, 0x01fffff1
	ldi	r0, 0x00000067
	ist	r0, r3
	ofp	r14
	ldi	r15, 0x0000040d
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r3, 0x01fffff2
	ist	r1, r3
	ldi	r3, 0x01fffff3
	ist	r2, r3
	ldi	r3, 0x01fffff4
	ist	r12, r3
	ldi	r3, 0x01fffff5
	ist	r11, r3
	ldi	r3, 0x01fffff6
	ist	r2, r3
	ldi	r3, 0x01fffff7
	ist	r5, r3
	ldi	r3, 0x01fffff8
	ist	r10, r3
	ldi	r3, 0x01fffff9
	ist	r4, r3
	ldi	r3, 0x01fffffa
	ist	r0, r3
	ldi	r10, 0x01fffffb
	ldi	r3, 0x0000003a
	ist	r3, r10
	ldi	r10, 0x01fffffc
	ldi	r4, 0x0000000a
	ist	r4, r10
	ldi	r4, 0x01fffffd
	ldi	r10, 0x0000003e
	ist	r10, r4
	ldi	r4, 0x01fffffe
	ist	r12, r4
	ldi	r4, 0x01ffffff
	ldi	r1, 0x00000000
	ist	r1, r4
	ldi	r4, 0x01ffffc0
	lda	r6, __aurora_enable_interrupt
	ist	r6, r4
	ldi	r4, 0x01ffffc1
	lda	r9, __aurora_disable_interrupt
	ist	r9, r4
	ldi	r4, 0x01ffffc2
	lda	r7, __aurora_set_up_stack_pointer
	ist	r7, r4
	ldi	r4, 0x01ffffc3
	lda	r7, __aurora_set_stack_pointer
	ist	r7, r4
	ldi	r4, 0x01ffffc4
	lda	r0, __aurora_get_stack_pointer
	ist	r0, r4
	ldi	r4, 0x01ffffc5
	lda	r8, __aurora_halt
	ist	r8, r4
	ldi	r4, 0x01ffffc6
	lda	r8, __aurora_nop
	ist	r8, r4
	ldi	r4, 0x01ffffc7
	lda	r8, __aurora_read
	ist	r8, r4
	ldi	r4, 0x01ffffc8
	lda	r8, __aurora_write
	ist	r8, r4
	ldi	r4, 0x01ffffc9
	ist	r6, r4
	ldi	r4, 0x01ffffca
	ist	r9, r4
	ldi	r4, 0x01ffffcb
	lda	r6, __aurora_register_interrupt
	ist	r6, r4
	ldi	r4, 0x01ffffcc
	ist	r7, r4
	ldi	r4, 0x01ffffcd
	ist	r0, r4
	ldi	r0, 0x01ffffce
	lda	r4, __aurora_push
	ist	r4, r0
	ldi	r0, 0x01ffffcf
	lda	r4, __aurora_pop
	ist	r4, r0
	ldi	r0, 0x01ffffd0
	lda	r4, __aurora_get_char
	ist	r4, r0
	ldi	r0, 0x01ffffd1
	lda	r4, __aurora_put_char
	ist	r4, r0
	ldi	r0, 0x01ffffd2
	lda	r4, __aurora_put_int
	ist	r4, r0
	ldi	r0, 0x01ffffd3
	lda	r4, __aurora_put_string
	ist	r4, r0
	ldi	r0, 0x01ffffd4
	ist	r1, r0
	ldi	r0, 0x01ffffd5
	ldi	r4, 0x01000000
	ist	r4, r0
	ldi	r0, 0x01ffffd6
	ldi	r6, 0x02000000
	ist	r6, r0
	ldi	r0, 0x01ffffd7
	ldi	r6, 0x03000000
	ist	r6, r0
	ldi	r0, 0x01ffffd8
	ldi	r7, 0x04000000
	ist	r7, r0
	ldi	r0, 0x01ffffd9
	ldi	r8, 0x05000000
	ist	r8, r0
	ldi	r0, 0x01ffffda
	ldi	r9, 0x06000000
	ist	r9, r0
	ldi	r0, 0x01ffffdb
	ist	r1, r0
	ldi	r0, 0x01ffffdc
	ist	r4, r0
	ldi	r0, 0x01ffffdd
	ist	r6, r0
	ldi	r0, 0x01ffffde
	ist	r7, r0
	mov	r1, r7
	ldi	r0, 0x01ffffdf
	ist	r8, r0
	ldi	r0, 0x01ffffe0
	ist	r9, r0
	ldi	r6, 0x01ffffbf
	ist	r7, r6
	ldi	r4, 0x01ffffbe
	ist	r7, r4
	ldi	r1, 0x01ffff61
	ldi	r0, 0x00000038
	ofp	r14
	ldi	r15, 0x00000408
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ist	r0, r1
	ldi	r0, 0x01ffff62
	ist	r11, r0
	ldi	r0, 0x01ffff63
	ldi	r11, 0x00000021
	ist	r11, r0
	ldi	r0, 0x01ffff64
	ldi	r11, 0x00000047
	ist	r11, r0
	ldi	r0, 0x01ffff65
	ldi	r11, 0x0000007a
	ist	r11, r0
	ldi	r0, 0x01ffff66
	ldi	r11, 0x00000029
	ist	r11, r0
	ldi	r0, 0x01ffff67
	ldi	r11, 0x00000033
	ist	r11, r0
	ldi	r0, 0x01ffff68
	ldi	r11, 0x00000058
	ist	r11, r0
	ldi	r0, 0x01ffff69
	ldi	r11, 0x00000071
	ist	r11, r0
	ldi	r0, 0x01ffff6a
	ldi	r11, 0x0000003b
	ist	r11, r0
	ldi	r0, 0x01ffff6b
	ldi	r11, 0x0000004e
	ist	r11, r0
	ldi	r0, 0x01ffff6c
	ist	r12, r0
	ldi	r0, 0x01ffff6d
	ldi	r1, 0x00000055
	ist	r1, r0
	ldi	r0, 0x01ffff6e
	ldi	r1, 0x0000006d
	ist	r1, r0
	ldi	r0, 0x01ffff6f
	ldi	r1, 0x00000041
	ist	r1, r0
	ldi	r0, 0x01ffff70
	ldi	r1, 0x0000002f
	ist	r1, r0
	ldi	r0, 0x01ffff71
	ldi	r1, 0x00000037
	ist	r1, r0
	ldi	r0, 0x01ffff72
	ldi	r1, 0x0000006b
	ist	r1, r0
	ldi	r0, 0x01ffff73
	ldi	r1, 0x0000003f
	ist	r1, r0
	ldi	r0, 0x01ffff74
	ldi	r1, 0x00000059
	ist	r1, r0
	ldi	r0, 0x01ffff75
	ldi	r1, 0x00000060
	ist	r1, r0
	ldi	r0, 0x01ffff76
	ldi	r1, 0x0000005e
	ist	r1, r0
	ldi	r0, 0x01ffff77
	ldi	r1, 0x00000066
	ist	r1, r0
	ldi	r0, 0x01ffff78
	ldi	r1, 0x00000034
	ist	r1, r0
	ldi	r0, 0x01ffff79
	ldi	r1, 0x0000005c
	ist	r1, r0
	ldi	r0, 0x01ffff7a
	ldi	r1, 0x00000064
	ist	r1, r0
	ldi	r0, 0x01ffff7b
	ldi	r1, 0x0000006f
	ist	r1, r0
	ldi	r0, 0x01ffff7c
	ldi	r1, 0x00000052
	ist	r1, r0
	ldi	r0, 0x01ffff7d
	ist	r3, r0
	ldi	r0, 0x01ffff7e
	ldi	r1, 0x00000049
	ist	r1, r0
	ldi	r0, 0x01ffff7f
	ldi	r1, 0x00000023
	ist	r1, r0
	ldi	r0, 0x01ffff80
	ldi	r1, 0x00000063
	ist	r1, r0
	ldi	r0, 0x01ffff81
	ldi	r1, 0x0000004a
	ist	r1, r0
	ldi	r0, 0x01ffff82
	ldi	r1, 0x00000030
	ist	r1, r0
	ldi	r0, 0x01ffff83
	ofp	r14
	ldi	r15, 0x0000040a
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff84
	ist	r10, r0
	ldi	r0, 0x01ffff85
	ldi	r1, 0x00000062
	ist	r1, r0
	ldi	r0, 0x01ffff86
	ldi	r1, 0x0000004c
	ist	r1, r0
	ldi	r0, 0x01ffff87
	ldi	r1, 0x00000031
	ist	r1, r0
	ldi	r0, 0x01ffff88
	ldi	r1, 0x0000007c
	ist	r1, r0
	ldi	r0, 0x01ffff89
	ldi	r1, 0x00000076
	ist	r1, r0
	ldi	r0, 0x01ffff8a
	ldi	r1, 0x00000048
	ist	r1, r0
	ldi	r0, 0x01ffff8b
	ist	r2, r0
	ldi	r0, 0x01ffff8c
	ldi	r1, 0x00000035
	ist	r1, r0
	ldi	r0, 0x01ffff8d
	ldi	r1, 0x0000005a
	ist	r1, r0
	ldi	r0, 0x01ffff8e
	ldi	r1, 0x0000002e
	ist	r1, r0
	ldi	r0, 0x01ffff8f
	ldi	r1, 0x00000057
	ist	r1, r0
	ldi	r0, 0x01ffff90
	ldi	r1, 0x00000053
	ist	r1, r0
	ldi	r0, 0x01ffff91
	ldi	r1, 0x00000026
	ist	r1, r0
	ldi	r0, 0x01ffff92
	ist	r13, r0
	ldi	r0, 0x01ffff93
	ldi	r1, 0x0000003c
	ist	r1, r0
	ldi	r0, 0x01ffff94
	ldi	r1, 0x00000039
	ist	r1, r0
	ldi	r0, 0x01ffff95
	ldi	r1, 0x0000005d
	ist	r1, r0
	ldi	r0, 0x01ffff96
	ldi	r1, 0x0000007b
	ist	r1, r0
	ldi	r0, 0x01ffff97
	ldi	r1, 0x00000045
	ist	r1, r0
	ldi	r0, 0x01ffff98
	ofp	r14
	ldi	r15, 0x0000040f
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff99
	ist	r5, r0
	ldi	r0, 0x01ffff9a
	ldi	r1, 0x00000032
	ist	r1, r0
	ldi	r0, 0x01ffff9b
	ldi	r1, 0x00000040
	ist	r1, r0
	ldi	r0, 0x01ffff9c
	ldi	r1, 0x0000004d
	ist	r1, r0
	ldi	r0, 0x01ffff9d
	ldi	r1, 0x00000024
	ist	r1, r0
	ldi	r0, 0x01ffff9e
	ldi	r1, 0x0000005f
	ofp	r14
	ldi	r15, 0x0000040a
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ist	r1, r0
	ldi	r0, 0x01ffff9f
	ldi	r1, 0x00000068
	ist	r1, r0
	ldi	r0, 0x01ffffa0
	ldi	r1, 0x0000002c
	ist	r1, r0
	ldi	r0, 0x01ffffa1
	ldi	r1, 0x0000004f
	ist	r1, r0
	ldi	r0, 0x01ffffa2
	ofp	r14
	ldi	r15, 0x0000040e
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffa3
	ldi	r1, 0x00000051
	ist	r1, r0
	ldi	r0, 0x01ffffa4
	ldi	r1, 0x0000007e
	ist	r1, r0
	ldi	r0, 0x01ffffa5
	ldi	r1, 0x00000046
	ist	r1, r0
	ldi	r0, 0x01ffffa6
	ofp	r14
	ldi	r15, 0x0000040d
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffa7
	ldi	r1, 0x00000036
	ist	r1, r0
	ldi	r0, 0x01ffffa8
	ldi	r1, 0x0000007d
	ist	r1, r0
	ldi	r0, 0x01ffffa9
	ldi	r1, 0x0000002d
	ist	r1, r0
	ldi	r0, 0x01ffffaa
	ldi	r1, 0x00000022
	ist	r1, r0
	ldi	r0, 0x01ffffab
	ldi	r1, 0x0000003d
	ist	r1, r0
	ldi	r0, 0x01ffffac
	ldi	r1, 0x00000028
	ist	r1, r0
	ldi	r0, 0x01ffffad
	ldi	r1, 0x00000042
	ist	r1, r0
	ldi	r0, 0x01ffffae
	ldi	r1, 0x00000025
	ist	r1, r0
	ldi	r0, 0x01ffffaf
	ldi	r1, 0x0000002a
	ist	r1, r0
	ldi	r0, 0x01ffffb0
	ldi	r1, 0x00000077
	ist	r1, r0
	ldi	r0, 0x01ffffb1
	ldi	r1, 0x0000004b
	ist	r1, r0
	ldi	r0, 0x01ffffb2
	ofp	r14
	ldi	r15, 0x00000407
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffb3
	ldi	r1, 0x00000079
	ist	r1, r0
	ldi	r0, 0x01ffffb4
	ldi	r1, 0x00000044
	ist	r1, r0
	ldi	r0, 0x01ffffb5
	ldi	r1, 0x00000027
	ist	r1, r0
	ldi	r0, 0x01ffffb6
	ldi	r1, 0x00000043
	ist	r1, r0
	ldi	r0, 0x01ffffb7
	ldi	r1, 0x0000005b
	ist	r1, r0
	ldi	r0, 0x01ffffb8
	ofp	r14
	ldi	r15, 0x0000040b
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffb9
	ldi	r1, 0x00000054
	ist	r1, r0
	ldi	r0, 0x01ffffba
	ofp	r14
	ldi	r15, 0x00000409
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffbb
	ofp	r14
	ldi	r15, 0x0000040c
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffffbc
	ldi	r1, 0x0000006a
	ist	r1, r0
	ldi	r0, 0x01ffffbd
	ldi	r1, 0x0000002b
	ist	r1, r0
	ldi	r0, 0x01fffb61
	ldi	r1, 0x00000400
	ofp	r14
	ldi	r15, 0x0000040d
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	call	__aurora_zero_words
	call	__aurora_set_up_stack_pointer
	call	__aurora_disable_interrupt
	ldi	r0, 0x03000007
	lda	r1, handle_timer
	call	__aurora_register_interrupt
	ofp	r14
	ldi	r15, 0x0000040c
	add	r14, r15, r14
	ist	r6, r14                         ; 4-byte Folded Spill
	ist	r7, r6
	ofp	r14
	ldi	r15, 0x00000406
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	call	__aurora_put_string
	;APP
	ei

	;NO_APP
.LBB1_1:                                ; =>This Inner Loop Header: Depth=1
	ild	r0, r4
	cmp	r0, r7
	jcc	z, .LBB1_1+0
; %bb.2:
	ldi	r0, 0x0000000a
	call	__aurora_put_char
	ofp	r14
	ldi	r15, 0x0000040c
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ild	r0, r0
	ldi	r9, 0x00000001
	cmp	r0, r9
	jcc	lt, .LBB1_17+0
; %bb.3:
	ofp	r5
	ldi	r15, 0x00000006
	add	r5, r15, r5
	ldi	r0, 0x0000000a
	ofp	r14
	ldi	r15, 0x00000409
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x00000100
	ofp	r14
	ldi	r15, 0x0000040b
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	mov	r7, r8
	jmp	.LBB1_4+0
.LBB1_15:                               ;   in Loop: Header=BB1_4 Depth=1
	ofp	r14
	ldi	r15, 0x00000407
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	mov	r4, r0
	call	__aurora_put_char
	add	r5, r8, r0
	ist	r4, r0
	ofp	r14
	ldi	r15, 0x00000409
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	call	__aurora_put_char
.LBB1_16:                               ;   in Loop: Header=BB1_4 Depth=1
	add	r8, r9, r8
	ofp	r14
	ldi	r15, 0x0000040c
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ild	r0, r0
	cmp	r8, r0
	jcc	ge, .LBB1_17+0
.LBB1_4:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_5 Depth 2
                                        ;       Child Loop BB1_8 Depth 3
                                        ;       Child Loop BB1_10 Depth 3
                                        ;     Child Loop BB1_14 Depth 2
	ofp	r14
	ldi	r15, 0x0000040d
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	add	r8, r0, r0
	ild	r0, r0
	ofp	r14
	ldi	r15, 0x00000407
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	mov	r7, r1
.LBB1_5:                                ;   Parent Loop BB1_4 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB1_8 Depth 3
                                        ;       Child Loop BB1_10 Depth 3
	mov	r7, r4
	ofp	r14
	ldi	r15, 0x00000408
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	add	r1, r0, r0
	ild	r7, r0
	ofp	r14
	ldi	r15, 0x00000407
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	cmp	r0, r7
	jcc	z, .LBB1_12+0
; %bb.6:                                ;   in Loop: Header=BB1_5 Depth=2
	ofp	r14
	ldi	r15, 0x00000406
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	cmp	r8, r4
	jcc	z, .LBB1_9+0
; %bb.7:                                ;   in Loop: Header=BB1_5 Depth=2
	mov	r4, r6
.LBB1_8:                                ;   Parent Loop BB1_4 Depth=1
                                        ;     Parent Loop BB1_5 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	r5, r6, r0
	ild	r0, r0
	call	__aurora_put_char
	add	r6, r9, r6
	cmp	r8, r6
	jcc	nz, .LBB1_8+0
.LBB1_9:                                ;   in Loop: Header=BB1_5 Depth=2
	mov	r7, r0
	call	__aurora_put_char
	ofp	r14
	ldi	r15, 0x00000409
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	call	__aurora_put_char
	ofp	r14
	ldi	r15, 0x0000040b
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	mov	r4, r7
.LBB1_10:                               ;   Parent Loop BB1_4 Depth=1
                                        ;     Parent Loop BB1_5 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	sub	r6, r9, r6
	call	__aurora_nop
	cmp	r6, r7
	jcc	nz, .LBB1_10+0
; %bb.11:                               ;   in Loop: Header=BB1_5 Depth=2
	ofp	r14
	ldi	r15, 0x00000406
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	add	r2, r9, r0
	ofp	r14
	ldi	r15, 0x0000040a
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	cmp	r2, r1
	mov	r0, r1
	jcc	c, .LBB1_5+0
	jmp	.LBB1_16+0
.LBB1_12:                               ;   in Loop: Header=BB1_4 Depth=1
	mov	r4, r7
	cmp	r8, r7
	jcc	z, .LBB1_15+0
; %bb.13:                               ;   in Loop: Header=BB1_4 Depth=1
	mov	r7, r4
.LBB1_14:                               ;   Parent Loop BB1_4 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	r5, r4, r0
	ild	r0, r0
	call	__aurora_put_char
	add	r4, r9, r4
	cmp	r8, r4
	jcc	nz, .LBB1_14+0
	jmp	.LBB1_15+0
.LBB1_17:
	call	__aurora_halt
.Lfunc_end1:
	.size	_start, .Lfunc_end1-_start
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
	pushfp
	stf
	osp	r14
	ldi	r15, 0x00000001
	add	r14, r15, r14
	lsp	r14
	ldi	r5, 0x01ffffbe
	ild	r0, r5
	ldi	r1, 0x00000000
	cmp	r0, r1
	jcc	nz, .LBB2_5+0
; %bb.1:
	ofp	r4
	mov	r4, r0
	call	__aurora_get_char
	ild	r0, r4
	ldi	r1, 0x0000000d
	cmp	r0, r1
	jcc	z, .LBB2_3+0
; %bb.2:
	ldi	r1, 0x0000000a
	cmp	r0, r1
	jcc	nz, .LBB2_4+0
.LBB2_3:
	ldi	r0, 0x00000001
	ist	r0, r5
	jmp	.LBB2_5+0
.LBB2_4:
	ldi	r1, 0x01ffffbf
	ild	r2, r1
	ldi	r3, 0x00000001
	add	r2, r3, r3
	ist	r3, r1
	ldi	r1, 0x01fffb61
	add	r2, r1, r1
	ist	r0, r1
	call	__aurora_put_char
.LBB2_5:
	fts
	popfp
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
.Lfunc_end2:
	.size	handle_input, .Lfunc_end2-handle_input
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_enable_interrupt
	.type	__aurora_enable_interrupt,@function
__aurora_enable_interrupt:              ; @__aurora_enable_interrupt
; %bb.0:
	;APP
	ei

	;NO_APP
	ret
.Lfunc_end3:
	.size	__aurora_enable_interrupt, .Lfunc_end3-__aurora_enable_interrupt
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_disable_interrupt
	.type	__aurora_disable_interrupt,@function
__aurora_disable_interrupt:             ; @__aurora_disable_interrupt
; %bb.0:
	;APP
	di

	;NO_APP
	ret
.Lfunc_end4:
	.size	__aurora_disable_interrupt, .Lfunc_end4-__aurora_disable_interrupt
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_set_up_stack_pointer
	.type	__aurora_set_up_stack_pointer,@function
__aurora_set_up_stack_pointer:          ; @__aurora_set_up_stack_pointer
; %bb.0:
	ret
.Lfunc_end5:
	.size	__aurora_set_up_stack_pointer, .Lfunc_end5-__aurora_set_up_stack_pointer
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_set_stack_pointer
	.type	__aurora_set_stack_pointer,@function
__aurora_set_stack_pointer:             ; @__aurora_set_stack_pointer
; %bb.0:
	;APP
	lsp	r0

	;NO_APP
	ret
.Lfunc_end6:
	.size	__aurora_set_stack_pointer, .Lfunc_end6-__aurora_set_stack_pointer
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_get_stack_pointer
	.type	__aurora_get_stack_pointer,@function
__aurora_get_stack_pointer:             ; @__aurora_get_stack_pointer
; %bb.0:
	;APP
	osp	r0

	;NO_APP
	ret
.Lfunc_end7:
	.size	__aurora_get_stack_pointer, .Lfunc_end7-__aurora_get_stack_pointer
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_halt
	.type	__aurora_halt,@function
__aurora_halt:                          ; @__aurora_halt
; %bb.0:
	;APP
	halt

	;NO_APP
.Lfunc_end8:
	.size	__aurora_halt, .Lfunc_end8-__aurora_halt
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_nop
	.type	__aurora_nop,@function
__aurora_nop:                           ; @__aurora_nop
; %bb.0:
	;APP
	nop

	;NO_APP
	ret
.Lfunc_end9:
	.size	__aurora_nop, .Lfunc_end9-__aurora_nop
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_read
	.type	__aurora_read,@function
__aurora_read:                          ; @__aurora_read
; %bb.0:
	ild	r0, r0
	ret
.Lfunc_end10:
	.size	__aurora_read, .Lfunc_end10-__aurora_read
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_write
	.type	__aurora_write,@function
__aurora_write:                         ; @__aurora_write
; %bb.0:
	ist	r1, r0
	ret
.Lfunc_end11:
	.size	__aurora_write, .Lfunc_end11-__aurora_write
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_register_interrupt
	.type	__aurora_register_interrupt,@function
__aurora_register_interrupt:            ; @__aurora_register_interrupt
; %bb.0:
	ist	r1, r0
	ret
.Lfunc_end12:
	.size	__aurora_register_interrupt, .Lfunc_end12-__aurora_register_interrupt
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_push
	.type	__aurora_push,@function
__aurora_push:                          ; @__aurora_push
; %bb.0:
	;APP
	push	r0

	;NO_APP
	ret
.Lfunc_end13:
	.size	__aurora_push, .Lfunc_end13-__aurora_push
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_pop
	.type	__aurora_pop,@function
__aurora_pop:                           ; @__aurora_pop
; %bb.0:
	;APP
	pop	r0

	;NO_APP
	ret
.Lfunc_end14:
	.size	__aurora_pop, .Lfunc_end14-__aurora_pop
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_get_char
	.type	__aurora_get_char,@function
__aurora_get_char:                      ; @__aurora_get_char
; %bb.0:
	ldi	r0, 0x04000000
	ild	r0, r0
	ret
.Lfunc_end15:
	.size	__aurora_get_char, .Lfunc_end15-__aurora_get_char
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_put_char
	.type	__aurora_put_char,@function
__aurora_put_char:                      ; @__aurora_put_char
; %bb.0:
	ldi	r1, 0x05000000
	ist	r0, r1
	ret
.Lfunc_end16:
	.size	__aurora_put_char, .Lfunc_end16-__aurora_put_char
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_put_int
	.type	__aurora_put_int,@function
__aurora_put_int:                       ; @__aurora_put_int
; %bb.0:
	ldi	r1, 0xffffffff
	cmp	r0, r1
	jcc	le, .LBB17_1+0
; %bb.7:
	ldi	r1, 0x00000000
	cmp	r0, r1
	jcc	nz, .LBB17_2+0
; %bb.8:
	ldi	r0, 0x05000000
	ldi	r1, 0x00000030
	ist	r1, r0
	ret
.LBB17_1:
	ldi	r1, 0x05000000
	ldi	r2, 0x0000002d
	ist	r2, r1
	ldi	r1, 0x00000000
	sub	r1, r0, r0
.LBB17_2:
	;APP
	osp	r1

	;NO_APP
	ldi	r2, 0x0000000a
	ldi	r3, 0x00000030
.LBB17_3:                               ; =>This Inner Loop Header: Depth=1
	rem	r0, r2, r10
	add	r10, r3, r10
	;APP
	push	r10

	;NO_APP
	div	r0, r2, r10
	cmp	r0, r2
	mov	r10, r0
	jcc	nc, .LBB17_3+0
; %bb.4:
	;APP
	osp	r0

	;NO_APP
	cmp	r0, r1
	jcc	nb, .LBB17_9+0
; %bb.5:
	ldi	r0, 0x05000000
.LBB17_6:                               ; =>This Inner Loop Header: Depth=1
	;APP
	pop	r2

	;NO_APP
	ist	r2, r0
	;APP
	osp	r2

	;NO_APP
	cmp	r2, r1
	jcc	b, .LBB17_6+0
.LBB17_9:
	ret
.Lfunc_end17:
	.size	__aurora_put_int, .Lfunc_end17-__aurora_put_int
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
	jcc	z, .LBB18_3+0
; %bb.1:
	ldi	r10, 0x00000001
	add	r0, r10, r11
	ldi	r0, 0x05000000
.LBB18_2:                               ; =>This Inner Loop Header: Depth=1
	ist	r3, r0
	add	r11, r10, r12
	ild	r3, r11
	and	r3, r1, r3
	cmp	r3, r2
	mov	r12, r11
	jcc	nz, .LBB18_2+0
.LBB18_3:
	ret
.Lfunc_end18:
	.size	__aurora_put_string, .Lfunc_end18-__aurora_put_string
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_zero_words
	.type	__aurora_zero_words,@function
__aurora_zero_words:                    ; @__aurora_zero_words
; %bb.0:
	jmp	.LBB19_1+0
.LBB19_1:                               ; =>This Inner Loop Header: Depth=1
	ldi	r2, 0x00000000
	ist	r2, r0
	ldi	r3, 0x00000001
	add	r0, r3, r0
	sub	r1, r3, r1
	cmp	r1, r2
	jcc	nz, .LBB19_1+0
	jmp	.LBB19_2+0
.LBB19_2:
	ret
.Lfunc_end19:
	.size	__aurora_zero_words, .Lfunc_end19-__aurora_zero_words
                                        ; -- End function
	.type	__aurora_program_mode,@object   ; @__aurora_program_mode
	.section	.rodata,"a",@progbits
	.p2align	2, 0x0
__aurora_program_mode:
	.long	0                               ; 0x0
	.size	__aurora_program_mode, 4

	.ident	"clang version 24.0.0git"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym handle_timer
	.addrsig_sym __aurora_enable_interrupt
	.addrsig_sym __aurora_disable_interrupt
	.addrsig_sym __aurora_set_up_stack_pointer
	.addrsig_sym __aurora_set_stack_pointer
	.addrsig_sym __aurora_get_stack_pointer
	.addrsig_sym __aurora_halt
	.addrsig_sym __aurora_nop
	.addrsig_sym __aurora_read
	.addrsig_sym __aurora_write
	.addrsig_sym __aurora_register_interrupt
	.addrsig_sym __aurora_push
	.addrsig_sym __aurora_pop
	.addrsig_sym __aurora_get_char
	.addrsig_sym __aurora_put_char
	.addrsig_sym __aurora_put_int
	.addrsig_sym __aurora_put_string
	.addrsig_sym __aurora_zero_words
	.addrsig_sym __aurora_program_mode
