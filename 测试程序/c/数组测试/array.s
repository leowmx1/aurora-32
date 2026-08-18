	.file	"array.c"
	.text
	.globl	_start                          ; -- Begin function _start
	.p2align	2
	.type	_start,@function
_start:                                 ; @_start
; %bb.0:
	ldi	r14, 0x02000000
	lsp	r14
	lfp	r14
	ldi	r0, 0x03000000
	lda	r1, handle_input
	ist	r1, r0
	ldi	r0, 0x01ffff38
	ldi	r1, 0x000000c8
	call	__aurora_zero_words
	ldi	r0, 0x01ffff37
	ldi	r1, 0x00000000
	ist	r1, r0
	ldi	r0, 0x01ffff36
	ist	r1, r0
	;APP
	ei

	;NO_APP
	call	input
	;APP
	di

	;NO_APP
	call	sort
	call	output
	;APP
	halt

	;NO_APP
.Lfunc_end0:
	.size	_start, .Lfunc_end0-_start
                                        ; -- End function
	.globl	input                           ; -- Begin function input
	.p2align	2
	.type	input,@function
input:                                  ; @input
; %bb.0:
	ldi	r0, 0x01ffff36
	ild	r2, r0
	ldi	r1, 0x00000000
	cmp	r2, r1
	jcc	nz, .LBB1_3
.LBB1_1:                                ; =>This Inner Loop Header: Depth=1
	;APP
	ei

	;NO_APP
	ild	r2, r0
	cmp	r2, r1
	jcc	z, .LBB1_1
.LBB1_3:
	;APP
	di

	;NO_APP
	ret
.Lfunc_end1:
	.size	input, .Lfunc_end1-input
                                        ; -- End function
	.globl	sort                            ; -- Begin function sort
	.p2align	2
	.type	sort,@function
sort:                                   ; @sort
; %bb.0:
	pushfp
	stf
	osp	r14
	ldi	r15, 0x00000003
	add	r14, r15, r14
	lsp	r14
	ofp	r14
	ldi	r15, 0x00000002
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000001
	add	r14, r15, r14
	ist	r5, r14                         ; 4-byte Folded Spill
	ofp	r14
	ist	r6, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x00000000
	ldi	r1, 0x00000001
	ldi	r2, 0x01ffff38
	ldi	r3, 0x01ffff39
	ldi	r10, 0x000000c7
	mov	r1, r11
	mov	r0, r12
	jmp	.LBB2_2
.LBB2_4:                                ;   in Loop: Header=BB2_2 Depth=1
	add	r12, r1, r12
	cmp	r12, r10
	jcc	z, .LBB2_1
.LBB2_2:                                ; =>This Inner Loop Header: Depth=1
	add	r12, r2, r13
	add	r12, r3, r4
	ild	r6, r4
	ild	r5, r13
	cmp	r5, r6
	jcc	nb, .LBB2_4
; %bb.3:                                ;   in Loop: Header=BB2_2 Depth=1
	ist	r6, r13
	ist	r5, r4
	mov	r0, r11
	jmp	.LBB2_4
.LBB2_1:                                ;   in Loop: Header=BB2_2 Depth=1
	cmp	r11, r0
	mov	r1, r11
	mov	r0, r12
	jcc	z, .LBB2_2
; %bb.5:
	ofp	r14
	ild	r6, r14                         ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000001
	add	r14, r15, r14
	ild	r5, r14                         ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000002
	add	r14, r15, r14
	ild	r4, r14                         ; 4-byte Folded Reload
	fts
	popfp
	ret
.Lfunc_end2:
	.size	sort, .Lfunc_end2-sort
                                        ; -- End function
	.globl	output                          ; -- Begin function output
	.p2align	2
	.type	output,@function
output:                                 ; @output
; %bb.0:
	ldi	r0, 0x00000000
	ldi	r1, 0x01ffff38
	ldi	r2, 0x05000000
	ldi	r3, 0x00000001
	ldi	r10, 0x000000c8
.LBB3_1:                                ; =>This Inner Loop Header: Depth=1
	add	r0, r1, r11
	ild	r11, r11
	ist	r11, r2
	add	r0, r3, r0
	cmp	r0, r10
	jcc	nz, .LBB3_1
; %bb.2:
	ret
.Lfunc_end3:
	.size	output, .Lfunc_end3-output
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
	ldi	r1, 0x01ffff37
	ild	r2, r1
	ldi	r3, 0x01ffff38
	add	r2, r3, r2
	ist	r0, r2
	ild	r2, r1
	ldi	r0, 0x00000001
	add	r2, r0, r2
	ist	r2, r1
	ldi	r1, 0x000000c8
	cmp	r2, r1
	jcc	nz, .LBB4_2
; %bb.1:
	ldi	r1, 0x01ffff36
	ist	r0, r1
.LBB4_2:
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
.Lfunc_end4:
	.size	handle_input, .Lfunc_end4-handle_input
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_zero_words
	.type	__aurora_zero_words,@function
__aurora_zero_words:                    ; @__aurora_zero_words
; %bb.0:
	jmp	.LBB5_1
.LBB5_1:                                ; =>This Inner Loop Header: Depth=1
	ldi	r2, 0x00000000
	ist	r2, r0
	ldi	r3, 0x00000001
	add	r0, r3, r0
	sub	r1, r3, r1
	cmp	r1, r2
	jcc	nz, .LBB5_1
	jmp	.LBB5_2
.LBB5_2:
	ret
.Lfunc_end5:
	.size	__aurora_zero_words, .Lfunc_end5-__aurora_zero_words
                                        ; -- End function
	.ident	"clang version 24.0.0git"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __aurora_zero_words
