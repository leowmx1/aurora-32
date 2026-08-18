	.file	"sort.c"
	.text
	.globl	_start                          ; -- Begin function _start
	.p2align	2
	.type	_start,@function
_start:                                 ; @_start
; %bb.0:
	osp	r15
	ldi	r14, 0x00000007
	add	r15, r14, r15
	lsp	r15
	osp	r14
	ldi	r15, 0x00000002
	sub	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	osp	r14
	ldi	r15, 0x00000003
	sub	r14, r15, r14
	ist	r5, r14                         ; 4-byte Folded Spill
	osp	r14
	ldi	r15, 0x00000004
	sub	r14, r15, r14
	ist	r6, r14                         ; 4-byte Folded Spill
	osp	r14
	ldi	r15, 0x00000005
	sub	r14, r15, r14
	ist	r7, r14                         ; 4-byte Folded Spill
	osp	r14
	ldi	r15, 0x00000006
	sub	r14, r15, r14
	ist	r8, r14                         ; 4-byte Folded Spill
	osp	r14
	ldi	r15, 0x00000007
	sub	r14, r15, r14
	ist	r9, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x03000000
	lda	r1, handle_input
	ist	r1, r0
	ldi	r0, 0x02000000
	;APP
	lsp	r0

	;NO_APP
	;APP
	di

	;NO_APP
	ldi	r1, 0x01fffffd
	ldi	r3, 0x00000000
	ist	r3, r1
	ldi	r2, 0x01fffffe
	ist	r3, r2
	ldi	r0, 0x01ffffff
	ist	r3, r0
	ldi	r0, 0x01000000
	ist	r3, r0
	;APP
	ei

	;NO_APP
.LBB0_1:                                ; =>This Inner Loop Header: Depth=1
	ild	r10, r2
	cmp	r10, r3
	jcc	z, .LBB0_1
; %bb.2:
	;APP
	di

	;NO_APP
	ild	r1, r1
	ldi	r10, 0x00000001
	sub	r1, r10, r11
	ldi	r12, 0x00000002
	ldi	r2, 0x01000001
	jmp	.LBB0_3
.LBB0_8:                                ;   in Loop: Header=BB0_3 Depth=1
	cmp	r4, r3
	jcc	z, .LBB0_9
.LBB0_3:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_5 Depth 2
	cmp	r1, r12
	jcc	c, .LBB0_10
; %bb.4:                                ;   in Loop: Header=BB0_3 Depth=1
	mov	r3, r13
	mov	r3, r4
	jmp	.LBB0_5
.LBB0_7:                                ;   in Loop: Header=BB0_5 Depth=2
	add	r13, r10, r13
	cmp	r11, r13
	jcc	z, .LBB0_8
.LBB0_5:                                ;   Parent Loop BB0_3 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	r13, r2, r5
	add	r13, r0, r6
	ild	r7, r6
	ild	r8, r5
	cmp	r7, r8
	jcc	nb, .LBB0_7
; %bb.6:                                ;   in Loop: Header=BB0_5 Depth=2
	ild	r4, r6
	ild	r7, r5
	ist	r7, r6
	ist	r4, r5
	mov	r10, r4
	jmp	.LBB0_7
.LBB0_10:
	ldi	r3, 0x05000000
	ldi	r10, 0x0000000a
	ist	r10, r3
	ldi	r3, 0x00000000
	cmp	r1, r3
	jcc	nz, .LBB0_11
.LBB0_22:
	;APP
	halt

	;NO_APP
.LBB0_9:
	ldi	r3, 0x05000000
	ldi	r10, 0x0000000a
	ist	r10, r3
.LBB0_11:
	ldi	r3, 0x00000000
	ldi	r10, 0x05000000
	ldi	r11, 0x00000030
	ldi	r12, 0x00000001
	ldi	r13, 0x0000003d
	ldi	r4, 0x0000003c
	osp	r14
	ldi	r15, 0x00000001
	sub	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ldi	r5, 0x0000000a
	mov	r3, r6
	jmp	.LBB0_12
.LBB0_20:                               ;   in Loop: Header=BB0_12 Depth=1
	ist	r6, r10
.LBB0_21:                               ;   in Loop: Header=BB0_12 Depth=1
	cmp	r8, r1
	mov	r8, r6
	jcc	z, .LBB0_22
.LBB0_12:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_23 Depth 2
                                        ;     Child Loop BB0_14 Depth 2
	add	r6, r0, r7
	ild	r9, r7
	;APP
	osp	r8

	;NO_APP
	cmp	r9, r3
	jcc	z, .LBB0_13
.LBB0_23:                               ;   Parent Loop BB0_12 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	rem	r9, r5, r4
	or	r4, r11, r4
	;APP
	push	r4

	;NO_APP
	div	r9, r5, r4
	cmp	r9, r5
	mov	r4, r9
	jcc	nc, .LBB0_23
.LBB0_13:                               ;   in Loop: Header=BB0_12 Depth=1
	;APP
	osp	r9

	;NO_APP
	cmp	r9, r8
	jcc	nb, .LBB0_15
.LBB0_14:                               ;   Parent Loop BB0_12 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	;APP
	pop	r4

	;NO_APP
	ist	r4, r10
	;APP
	osp	r4

	;NO_APP
	cmp	r4, r8
	jcc	b, .LBB0_14
.LBB0_15:                               ;   in Loop: Header=BB0_12 Depth=1
	ild	r4, r7
	cmp	r4, r3
	jcc	nz, .LBB0_17
; %bb.16:                               ;   in Loop: Header=BB0_12 Depth=1
	ist	r11, r10
.LBB0_17:                               ;   in Loop: Header=BB0_12 Depth=1
	add	r6, r12, r8
	cmp	r8, r1
	jcc	nc, .LBB0_21
; %bb.18:                               ;   in Loop: Header=BB0_12 Depth=1
	add	r6, r2, r4
	ild	r6, r7
	ild	r4, r4
	cmp	r6, r4
	mov	r13, r6
	jcc	z, .LBB0_20
; %bb.19:                               ;   in Loop: Header=BB0_12 Depth=1
	osp	r14
	ldi	r15, 0x00000001
	sub	r14, r15, r14
	ild	r6, r14                         ; 4-byte Folded Reload
	jmp	.LBB0_20
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
	ldi	r0, 0x04000000
	ild	r0, r0
	ldi	r2, 0x01fffffe
	ild	r3, r2
	ldi	r1, 0x00000000
	cmp	r3, r1
	jcc	nz, .LBB1_12
; %bb.1:
	ldi	r3, 0x000000ff
	and	r0, r3, r0
	ldi	r3, 0x0000000a
	cmp	r0, r3
	jcc	z, .LBB1_4
; %bb.2:
	ldi	r4, 0x0000005d
	cmp	r0, r4
	jcc	z, .LBB1_7
; %bb.3:
	ldi	r2, 0x0000000d
	cmp	r0, r2
	jcc	nz, .LBB1_10
.LBB1_4:
	ldi	r2, 0x01ffffff
	ild	r3, r2
	cmp	r3, r1
	jcc	z, .LBB1_6
; %bb.5:
	ldi	r3, 0x01fffffd
	ild	r4, r3
	ldi	r5, 0x00000001
	add	r4, r5, r4
	ist	r4, r3
	ist	r1, r2
	ild	r2, r3
	ldi	r3, 0x01000000
	add	r2, r3, r2
	ist	r1, r2
.LBB1_6:
	ldi	r1, 0x05000000
	ist	r0, r1
	jmp	.LBB1_12
.LBB1_7:
	ldi	r0, 0x01ffffff
	ild	r3, r0
	cmp	r3, r1
	jcc	z, .LBB1_9
; %bb.8:
	ldi	r3, 0x01fffffd
	ild	r4, r3
	ldi	r5, 0x00000001
	add	r4, r5, r4
	ist	r4, r3
	ist	r1, r0
.LBB1_9:
	ldi	r0, 0x00000001
	ist	r0, r2
	jmp	.LBB1_12
.LBB1_10:
	ldi	r1, 0x00000030
	sub	r0, r1, r1
	ldi	r2, 0x00000009
	cmp	r1, r2
	jcc	b, .LBB1_12
; %bb.11:
	ldi	r2, 0x01fffffd
	ild	r2, r2
	ldi	r4, 0x01000000
	add	r2, r4, r2
	ild	r4, r2
	mul	r4, r3, r3
	add	r3, r1, r1
	ist	r1, r2
	ldi	r1, 0x05000000
	ist	r0, r1
	ldi	r0, 0x01ffffff
	ldi	r1, 0x00000001
	ist	r1, r0
.LBB1_12:
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
	.ident	"clang version 24.0.0git"
	.section	".note.GNU-stack","",@progbits
	.addrsig
