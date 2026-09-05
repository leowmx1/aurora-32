	.file	"sort.c"
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
	ldi	r15, 0x00000009
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
	ldi	r0, 0x01ffffcb
	ofp	r14
	ldi	r15, 0x00000006
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r6, 0x00000045
	ist	r6, r0
	ldi	r0, 0x01ffffcc
	ldi	r11, 0x0000006e
	ist	r11, r0
	ldi	r0, 0x01ffffcd
	ldi	r10, 0x00000074
	ist	r10, r0
	ldi	r0, 0x01ffffce
	ldi	r3, 0x00000065
	ist	r3, r0
	ldi	r0, 0x01ffffcf
	ldi	r12, 0x00000072
	ist	r12, r0
	ldi	r0, 0x01ffffd0
	ldi	r2, 0x00000020
	ist	r2, r0
	ldi	r0, 0x01ffffd1
	ldi	r13, 0x00000069
	ist	r13, r0
	ldi	r0, 0x01ffffd2
	ist	r11, r0
	ldi	r0, 0x01ffffd3
	ist	r10, r0
	ldi	r0, 0x01ffffd4
	ist	r3, r0
	ldi	r0, 0x01ffffd5
	ldi	r1, 0x00000067
	ofp	r14
	ldi	r15, 0x00000008
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ist	r1, r0
	ldi	r0, 0x01ffffd6
	ist	r3, r0
	ldi	r0, 0x01ffffd7
	ist	r12, r0
	ldi	r0, 0x01ffffd8
	ldi	r1, 0x00000073
	ist	r1, r0
	ofp	r14
	ldi	r15, 0x00000007
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x01ffffd9
	ist	r2, r0
	ldi	r0, 0x01ffffda
	ist	r10, r0
	ldi	r0, 0x01ffffdb
	ldi	r5, 0x0000006f
	ist	r5, r0
	ldi	r0, 0x01ffffdc
	ist	r2, r0
	ldi	r0, 0x01ffffdd
	ist	r1, r0
	ldi	r0, 0x01ffffde
	ist	r5, r0
	ldi	r0, 0x01ffffdf
	ist	r12, r0
	ldi	r0, 0x01ffffe0
	ist	r10, r0
	ldi	r0, 0x01ffffe1
	ldi	r7, 0x0000002c
	ist	r7, r0
	ldi	r0, 0x01ffffe2
	ist	r2, r0
	ldi	r0, 0x01ffffe3
	ist	r5, r0
	ldi	r0, 0x01ffffe4
	ist	r11, r0
	ldi	r0, 0x01ffffe5
	ist	r3, r0
	ldi	r0, 0x01ffffe6
	ist	r2, r0
	ldi	r0, 0x01ffffe7
	ldi	r7, 0x00000070
	ist	r7, r0
	ldi	r0, 0x01ffffe8
	ist	r3, r0
	ldi	r0, 0x01ffffe9
	ist	r12, r0
	ldi	r0, 0x01ffffea
	ist	r2, r0
	ldi	r0, 0x01ffffeb
	ldi	r7, 0x0000006c
	ist	r7, r0
	ldi	r0, 0x01ffffec
	ist	r13, r0
	ldi	r0, 0x01ffffed
	ist	r11, r0
	ldi	r0, 0x01ffffee
	ist	r3, r0
	ldi	r8, 0x01ffffef
	ldi	r0, 0x0000002e
	ist	r0, r8
	ldi	r8, 0x01fffff0
	ist	r2, r8
	ldi	r8, 0x01fffff1
	ist	r6, r8
	ldi	r6, 0x01fffff2
	ist	r11, r6
	ldi	r6, 0x01fffff3
	ldi	r8, 0x00000064
	ist	r8, r6
	ldi	r6, 0x01fffff4
	ist	r2, r6
	ldi	r6, 0x01fffff5
	ldi	r9, 0x00000077
	ist	r9, r6
	ldi	r6, 0x01fffff6
	ist	r13, r6
	ldi	r6, 0x01fffff7
	ist	r10, r6
	ldi	r6, 0x01fffff8
	ldi	r9, 0x00000068
	ist	r9, r6
	ldi	r6, 0x01fffff9
	ist	r2, r6
	ldi	r6, 0x01fffffa
	ldi	r9, 0x00000027
	ist	r9, r6
	ldi	r6, 0x01fffffb
	ldi	r4, 0x0000005d
	ist	r4, r6
	ldi	r4, 0x01fffffc
	ist	r9, r4
	ldi	r4, 0x01fffffd
	ist	r0, r4
	ldi	r4, 0x01fffffe
	ldi	r9, 0x0000000a
	ist	r9, r4
	ldi	r4, 0x01ffffff
	ldi	r6, 0x00000000
	ist	r6, r4
	ldi	r4, 0x01ffffbf
	ldi	r1, 0x00000053
	ist	r1, r4
	ldi	r4, 0x01ffffc0
	ist	r5, r4
	ldi	r4, 0x01ffffc1
	ist	r12, r4
	ldi	r4, 0x01ffffc2
	ist	r10, r4
	ldi	r4, 0x01ffffc3
	ist	r13, r4
	ldi	r13, 0x01ffffc4
	ist	r11, r13
	ldi	r11, 0x01ffffc5
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r11
	ldi	r11, 0x01ffffc6
	ist	r0, r11
	ldi	r11, 0x01ffffc7
	ist	r0, r11
	ldi	r11, 0x01ffffc8
	ist	r0, r11
	ldi	r0, 0x01ffffc9
	ist	r9, r0
	ldi	r0, 0x01ffffca
	ist	r6, r0
	ldi	r0, 0x01ffffaf
	ist	r1, r0
	ldi	r0, 0x01ffffb0
	ist	r5, r0
	ldi	r0, 0x01ffffb1
	ist	r12, r0
	ldi	r0, 0x01ffffb2
	ist	r10, r0
	ldi	r0, 0x01ffffb3
	ist	r3, r0
	ldi	r0, 0x01ffffb4
	ist	r8, r0
	ldi	r0, 0x01ffffb5
	ist	r2, r0
	ldi	r0, 0x01ffffb6
	ldi	r1, 0x00000076
	ist	r1, r0
	ldi	r0, 0x01ffffb7
	ldi	r1, 0x00000061
	ist	r1, r0
	ldi	r0, 0x01ffffb8
	ist	r7, r0
	ldi	r0, 0x01ffffb9
	ldi	r1, 0x00000075
	ist	r1, r0
	ldi	r0, 0x01ffffba
	ist	r3, r0
	ldi	r0, 0x01ffffbb
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r10, r0
	ldi	r10, 0x01ffffbc
	ldi	r0, 0x0000003a
	ist	r0, r10
	ldi	r10, 0x01ffffbd
	ist	r9, r10
	ldi	r10, 0x01ffffbe
	ist	r6, r10
	ldi	r10, 0x01ffffad
	ist	r9, r10
	ldi	r10, 0x01ffffae
	ist	r6, r10
	ldi	r10, 0x01ffffa5
	ldi	r11, 0x0000004e
	ist	r11, r10
	ldi	r10, 0x01ffffa6
	ist	r1, r10
	ldi	r1, 0x01ffffa7
	ldi	r10, 0x0000006d
	ist	r10, r1
	ldi	r1, 0x01ffffa8
	ldi	r10, 0x00000062
	ist	r10, r1
	ldi	r1, 0x01ffffa9
	ist	r3, r1
	ldi	r1, 0x01ffffaa
	ist	r12, r1
	ldi	r1, 0x01ffffab
	ist	r2, r1
	ldi	r1, 0x01ffffac
	ist	r6, r1
	ldi	r1, 0x01ffffa2
	ist	r0, r1
	ldi	r0, 0x01ffffa3
	ist	r2, r0
	ldi	r0, 0x01ffffa4
	ist	r6, r0
	ldi	r0, 0x01ffff81
	lda	r1, __aurora_enable_interrupt
	ist	r1, r0
	ldi	r0, 0x01ffff82
	lda	r2, __aurora_disable_interrupt
	ist	r2, r0
	ldi	r0, 0x01ffff83
	lda	r3, __aurora_set_up_stack_pointer
	ist	r3, r0
	ldi	r0, 0x01ffff84
	lda	r3, __aurora_set_stack_pointer
	ist	r3, r0
	ldi	r0, 0x01ffff85
	lda	r10, __aurora_get_stack_pointer
	ist	r10, r0
	ldi	r0, 0x01ffff86
	lda	r11, __aurora_halt
	ist	r11, r0
	ldi	r0, 0x01ffff87
	lda	r11, __aurora_nop
	ist	r11, r0
	ldi	r0, 0x01ffff88
	lda	r11, __aurora_read
	ist	r11, r0
	ldi	r0, 0x01ffff89
	lda	r11, __aurora_write
	ist	r11, r0
	ldi	r0, 0x01ffff8a
	ist	r1, r0
	ldi	r0, 0x01ffff8b
	ist	r2, r0
	ldi	r0, 0x01ffff8c
	lda	r1, __aurora_register_interrupt
	ist	r1, r0
	ldi	r0, 0x01ffff8d
	ist	r3, r0
	ldi	r0, 0x01ffff8e
	ist	r10, r0
	ldi	r0, 0x01ffff8f
	lda	r1, __aurora_push
	ist	r1, r0
	ldi	r0, 0x01ffff90
	lda	r1, __aurora_pop
	ist	r1, r0
	ldi	r0, 0x01ffff91
	lda	r1, __aurora_get_char
	ist	r1, r0
	ldi	r0, 0x01ffff92
	lda	r1, __aurora_put_char
	ist	r1, r0
	ldi	r0, 0x01ffff93
	lda	r1, __aurora_put_int
	ist	r1, r0
	ldi	r0, 0x01ffff94
	lda	r1, __aurora_put_string
	ist	r1, r0
	ldi	r0, 0x01ffff95
	ist	r6, r0
	ldi	r0, 0x01ffff96
	ldi	r1, 0x01000000
	ist	r1, r0
	ldi	r0, 0x01ffff97
	ldi	r2, 0x02000000
	ist	r2, r0
	ldi	r0, 0x01ffff98
	ldi	r2, 0x03000000
	ist	r2, r0
	ldi	r0, 0x01ffff99
	ldi	r3, 0x04000000
	ist	r3, r0
	ldi	r0, 0x01ffff9a
	ldi	r10, 0x05000000
	ist	r10, r0
	ldi	r0, 0x01ffff9b
	ldi	r11, 0x06000000
	ist	r11, r0
	ldi	r0, 0x01ffff9c
	ist	r6, r0
	ldi	r0, 0x01ffff9d
	ist	r1, r0
	ldi	r0, 0x01ffff9e
	ist	r2, r0
	ldi	r0, 0x01ffff9f
	ist	r3, r0
	ldi	r0, 0x01ffffa0
	ist	r10, r0
	ldi	r0, 0x01ffffa1
	ist	r11, r0
	ldi	r0, 0x01ffff80
	ofp	r14
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r6, r0
	ldi	r9, 0x01ffff7f
	ist	r6, r9
	ldi	r4, 0x01ffff7e
	ist	r6, r4
	ldi	r7, 0x01ffff7d
	ist	r6, r7
	ldi	r8, 0x01ffff7c
	ist	r6, r8
	ldi	r5, 0x01fffb7c
	ldi	r1, 0x00000400
	mov	r5, r0
	call	__aurora_zero_words
	call	__aurora_set_up_stack_pointer
	call	__aurora_disable_interrupt
	ldi	r0, 0x03000007
	lda	r1, handle_timer
	call	__aurora_register_interrupt
	ist	r6, r4
	ist	r6, r7
	ist	r6, r9
	ist	r6, r8
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r6, r0
	ist	r6, r5
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	call	__aurora_put_string
	;APP
	ei

	;NO_APP
.LBB1_1:                                ; =>This Inner Loop Header: Depth=1
	ild	r0, r7
	cmp	r0, r6
	jcc	z, .LBB1_1+0
; %bb.2:
	call	__aurora_disable_interrupt
	ldi	r0, 0x01ffffbf
	call	__aurora_put_string
	ild	r4, r4
	ldi	r0, 0x00000002
	cmp	r4, r0
	jcc	lt, .LBB1_10+0
; %bb.3:
	ldi	r0, 0x00000001
	ldi	r1, 0x00000000
	ldi	r2, 0x00000003
	mov	r4, r3
.LBB1_5:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_6 Depth 2
	mov	r3, r10
	sub	r10, r0, r3
	mov	r1, r11
	mov	r1, r12
	jmp	.LBB1_6+0
.LBB1_8:                                ;   in Loop: Header=BB1_6 Depth=2
	add	r11, r0, r11
	cmp	r3, r11
	jcc	z, .LBB1_9+0
.LBB1_6:                                ;   Parent Loop BB1_5 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	r11, r5, r13
	add	r13, r0, r7
	ild	r8, r13
	ild	r9, r7
	cmp	r8, r9
	jcc	le, .LBB1_8+0
; %bb.7:                                ;   in Loop: Header=BB1_6 Depth=2
	ist	r9, r13
	ist	r8, r7
	mov	r0, r12
	jmp	.LBB1_8+0
.LBB1_9:                                ;   in Loop: Header=BB1_5 Depth=1
	cmp	r12, r1
	jcc	z, .LBB1_10+0
; %bb.4:                                ;   in Loop: Header=BB1_5 Depth=1
	cmp	r10, r2
	jcc	ge, .LBB1_5+0
.LBB1_10:
	ldi	r0, 0x01ffffaf
	call	__aurora_put_string
	ldi	r7, 0x00000001
	cmp	r4, r7
	jcc	lt, .LBB1_17+0
; %bb.11:
	ldi	r8, 0x0000003d
	ldi	r0, 0x0000003c
	ofp	r14
	ldi	r15, 0x00000006
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	jmp	.LBB1_12+0
.LBB1_15:                               ;   in Loop: Header=BB1_12 Depth=1
	call	__aurora_put_char
.LBB1_16:                               ;   in Loop: Header=BB1_12 Depth=1
	cmp	r4, r6
	jcc	z, .LBB1_17+0
.LBB1_12:                               ; =>This Inner Loop Header: Depth=1
	add	r6, r5, r9
	ild	r0, r9
	call	__aurora_put_int
	add	r6, r7, r6
	cmp	r6, r4
	jcc	ge, .LBB1_16+0
; %bb.13:                               ;   in Loop: Header=BB1_12 Depth=1
	add	r9, r7, r0
	ild	r1, r9
	ild	r0, r0
	cmp	r1, r0
	mov	r8, r0
	jcc	z, .LBB1_15+0
; %bb.14:                               ;   in Loop: Header=BB1_12 Depth=1
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	jmp	.LBB1_15+0
.LBB1_17:
	ldi	r0, 0x01ffffad
	call	__aurora_put_string
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
	ldi	r15, 0x00000002
	add	r14, r15, r14
	lsp	r14
	ofp	r4
	mov	r4, r0
	call	__aurora_get_char
	ild	r0, r4
	ldi	r5, 0x01ffff7d
	ild	r1, r5
	ldi	r8, 0x00000000
	cmp	r1, r8
	jcc	z, .LBB2_1+0
.LBB2_32:
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
.LBB2_1:
	ldi	r1, 0x000000ff
	and	r0, r1, r0
	ldi	r1, 0x0000002c
	cmp	r0, r1
	jcc	gt, .LBB2_9+0
; %bb.2:
	ldi	r1, 0x0000000a
	cmp	r0, r1
	jcc	z, .LBB2_20+0
; %bb.3:
	ldi	r1, 0x0000000d
	cmp	r0, r1
	jcc	nz, .LBB2_4+0
.LBB2_20:
	ldi	r6, 0x01ffff7f
	ild	r0, r6
	ldi	r5, 0x00000000
	cmp	r0, r5
	jcc	z, .LBB2_34+0
; %bb.21:
	ldi	r7, 0x01ffff80
	ild	r0, r7
	ldi	r8, 0x00000001
	cmp	r0, r8
	jcc	lt, .LBB2_24+0
; %bb.22:
	ldi	r4, 0x00000008
	mov	r5, r9
.LBB2_23:                               ; =>This Inner Loop Header: Depth=1
	mov	r4, r0
	call	__aurora_put_char
	add	r9, r8, r9
	ild	r0, r7
	cmp	r9, r0
	jcc	lt, .LBB2_23+0
.LBB2_24:
	ldi	r0, 0x01ffffa5
	call	__aurora_put_string
	ldi	r4, 0x01ffff7e
	ild	r0, r4
	add	r0, r8, r0
	call	__aurora_put_int
	ldi	r0, 0x01ffffa2
	call	__aurora_put_string
	ldi	r9, 0x01fffb7c
	ild	r0, r4
	add	r0, r9, r0
	ild	r0, r0
	call	__aurora_put_int
	ldi	r0, 0x01ffffad
	call	__aurora_put_string
	ild	r0, r4
	ldi	r1, 0x00000400
	cmp	r0, r1
	jcc	ge, .LBB2_26+0
; %bb.25:
	ild	r0, r4
	add	r0, r8, r0
	ist	r0, r4
.LBB2_26:
	ist	r5, r6
	ldi	r0, 0x01ffff7c
	ist	r5, r0
	ist	r5, r7
	ild	r0, r4
	ldi	r1, 0x000003ff
	cmp	r0, r1
	jcc	gt, .LBB2_32+0
; %bb.27:
	ild	r0, r4
	add	r0, r9, r0
	ist	r5, r0
	jmp	.LBB2_32+0
.LBB2_9:
	ldi	r1, 0x0000002d
	cmp	r0, r1
	jcc	z, .LBB2_28+0
; %bb.10:
	ldi	r1, 0x0000005d
	cmp	r0, r1
	jcc	nz, .LBB2_4+0
; %bb.11:
	ldi	r1, 0x01ffff7f
	ild	r0, r1
	cmp	r0, r8
	ldi	r7, 0x00000001
	jcc	z, .LBB2_33+0
; %bb.12:
	ofp	r14
	ldi	r15, 0x00000001
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ldi	r9, 0x01ffff80
	ild	r0, r9
	cmp	r0, r7
	jcc	lt, .LBB2_15+0
; %bb.13:
	ldi	r4, 0x00000008
	mov	r8, r6
.LBB2_14:                               ; =>This Inner Loop Header: Depth=1
	mov	r4, r0
	call	__aurora_put_char
	add	r6, r7, r6
	ild	r0, r9
	cmp	r6, r0
	jcc	lt, .LBB2_14+0
.LBB2_15:
	ldi	r0, 0x01ffffa5
	call	__aurora_put_string
	ldi	r4, 0x01ffff7e
	ild	r0, r4
	add	r0, r7, r0
	call	__aurora_put_int
	ldi	r0, 0x01ffffa2
	call	__aurora_put_string
	ldi	r6, 0x01fffb7c
	ild	r0, r4
	add	r0, r6, r0
	ild	r0, r0
	call	__aurora_put_int
	ldi	r0, 0x01ffffad
	call	__aurora_put_string
	ild	r0, r4
	ldi	r1, 0x00000400
	cmp	r0, r1
	jcc	ge, .LBB2_17+0
; %bb.16:
	ild	r0, r4
	add	r0, r7, r0
	ist	r0, r4
.LBB2_17:
	ofp	r14
	ldi	r15, 0x00000001
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r8, r0
	ldi	r0, 0x01ffff7c
	ist	r8, r0
	ist	r8, r9
	ild	r0, r4
	ldi	r1, 0x000003ff
	cmp	r0, r1
	jcc	gt, .LBB2_19+0
; %bb.18:
	ild	r0, r4
	add	r0, r6, r0
	ist	r8, r0
.LBB2_19:
	ist	r7, r5
	jmp	.LBB2_32+0
.LBB2_4:
	ldi	r2, 0x00000030
	sub	r0, r2, r1
	ldi	r3, 0x00000009
	cmp	r1, r3
	jcc	b, .LBB2_32+0
; %bb.5:
	ldi	r4, 0x01ffff7e
	ild	r3, r4
	ldi	r5, 0x000003ff
	cmp	r3, r5
	jcc	gt, .LBB2_32+0
; %bb.6:
	ild	r5, r4
	ldi	r3, 0x01fffb7c
	add	r5, r3, r5
	ild	r5, r5
	ldi	r6, 0x0000000a
	mul	r5, r6, r5
	ldi	r6, 0x01ffff7c
	ild	r6, r6
	ild	r4, r4
	ldi	r7, 0x00000000
	cmp	r6, r7
	jcc	z, .LBB2_8+0
; %bb.7:
	sub	r2, r0, r1
.LBB2_8:
	add	r4, r3, r2
	add	r5, r1, r1
	ist	r1, r2
	ldi	r1, 0x01ffff7f
	ldi	r2, 0x00000001
	ist	r2, r1
	ldi	r1, 0x01ffff80
	ild	r3, r1
	add	r3, r2, r2
	ist	r2, r1
	jmp	.LBB2_31+0
.LBB2_28:
	ldi	r0, 0x01ffff7f
	ild	r1, r0
	ldi	r0, 0x00000000
	cmp	r1, r0
	jcc	nz, .LBB2_32+0
; %bb.29:
	ldi	r1, 0x01ffff7c
	ild	r2, r1
	cmp	r2, r0
	jcc	nz, .LBB2_32+0
; %bb.30:
	ldi	r0, 0x00000001
	ist	r0, r1
	ldi	r1, 0x01ffff80
	ild	r2, r1
	add	r2, r0, r0
	ist	r0, r1
	ldi	r0, 0x0000002d
.LBB2_31:
	call	__aurora_put_char
	jmp	.LBB2_32+0
.LBB2_34:
	ldi	r0, 0x01ffff7c
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB2_32+0
.LBB2_33:
	ldi	r0, 0x01ffff7c
	ldi	r1, 0x00000000
	ist	r1, r0
	ist	r7, r5
	jmp	.LBB2_32+0
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
