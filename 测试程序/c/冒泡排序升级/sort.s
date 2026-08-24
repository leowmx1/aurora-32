	.file	"sort.c"
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
	ldi	r0, 0x01ffffa1
	ofp	r14
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r6, r0
	ldi	r9, 0x01ffffa0
	ist	r6, r9
	ldi	r4, 0x01ffff9f
	ist	r6, r4
	ldi	r7, 0x01ffff9e
	ist	r6, r7
	ldi	r8, 0x01ffff9d
	ist	r6, r8
	ldi	r5, 0x01fffb9d
	ldi	r1, 0x00000400
	mov	r5, r0
	call	__aurora_zero_words
	;APP
	di

	;NO_APP
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
.LBB0_1:                                ; =>This Inner Loop Header: Depth=1
	ild	r0, r7
	cmp	r0, r6
	jcc	z, .LBB0_1
; %bb.2:
	;APP
	di

	;NO_APP
	ldi	r0, 0x01ffffbf
	call	__aurora_put_string
	ild	r4, r4
	ldi	r0, 0x00000002
	cmp	r4, r0
	jcc	lt, .LBB0_10
; %bb.3:
	ldi	r0, 0x00000001
	ldi	r1, 0x00000000
	ldi	r2, 0x00000003
	mov	r4, r3
.LBB0_5:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_6 Depth 2
	mov	r3, r10
	sub	r10, r0, r3
	mov	r1, r11
	mov	r1, r12
	jmp	.LBB0_6
.LBB0_8:                                ;   in Loop: Header=BB0_6 Depth=2
	add	r11, r0, r11
	cmp	r3, r11
	jcc	z, .LBB0_9
.LBB0_6:                                ;   Parent Loop BB0_5 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	r11, r5, r13
	add	r13, r0, r7
	ild	r8, r13
	ild	r9, r7
	cmp	r8, r9
	jcc	le, .LBB0_8
; %bb.7:                                ;   in Loop: Header=BB0_6 Depth=2
	ist	r9, r13
	ist	r8, r7
	mov	r0, r12
	jmp	.LBB0_8
.LBB0_9:                                ;   in Loop: Header=BB0_5 Depth=1
	cmp	r12, r1
	jcc	z, .LBB0_10
; %bb.4:                                ;   in Loop: Header=BB0_5 Depth=1
	cmp	r10, r2
	jcc	ge, .LBB0_5
.LBB0_10:
	ldi	r0, 0x01ffffaf
	call	__aurora_put_string
	ldi	r7, 0x00000001
	cmp	r4, r7
	jcc	lt, .LBB0_17
; %bb.11:
	ldi	r0, 0x0000003d
	ofp	r14
	ldi	r15, 0x00000006
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r9, 0x05000000
	ldi	r0, 0x0000003c
	ofp	r14
	ldi	r15, 0x00000007
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	jmp	.LBB0_12
.LBB0_15:                               ;   in Loop: Header=BB0_12 Depth=1
	ist	r0, r9
.LBB0_16:                               ;   in Loop: Header=BB0_12 Depth=1
	cmp	r4, r6
	jcc	z, .LBB0_17
.LBB0_12:                               ; =>This Inner Loop Header: Depth=1
	add	r6, r5, r8
	ild	r0, r8
	call	__aurora_put_int
	add	r6, r7, r6
	cmp	r6, r4
	jcc	ge, .LBB0_16
; %bb.13:                               ;   in Loop: Header=BB0_12 Depth=1
	add	r8, r7, r0
	ild	r1, r8
	ild	r0, r0
	cmp	r1, r0
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	jcc	z, .LBB0_15
; %bb.14:                               ;   in Loop: Header=BB0_12 Depth=1
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	jmp	.LBB0_15
.LBB0_17:
	ldi	r0, 0x01ffffad
	call	__aurora_put_string
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
	ldi	r0, 0x04000000
	ild	r0, r0
	ldi	r4, 0x01ffff9e
	ild	r2, r4
	ldi	r1, 0x00000000
	cmp	r2, r1
	jcc	z, .LBB1_1
.LBB1_16:
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
.LBB1_1:
	ldi	r2, 0x000000ff
	and	r0, r2, r0
	ldi	r2, 0x0000002c
	cmp	r0, r2
	jcc	gt, .LBB1_5
; %bb.2:
	ldi	r2, 0x0000000a
	cmp	r0, r2
	jcc	z, .LBB1_4
; %bb.3:
	ldi	r2, 0x0000000d
	cmp	r0, r2
	jcc	nz, .LBB1_11
.LBB1_4:
	call	finish_number
	jmp	.LBB1_16
.LBB1_5:
	ldi	r2, 0x0000002d
	cmp	r0, r2
	jcc	z, .LBB1_8
; %bb.6:
	ldi	r2, 0x0000005d
	cmp	r0, r2
	jcc	nz, .LBB1_11
; %bb.7:
	call	finish_number
	ldi	r0, 0x00000001
	ist	r0, r4
	jmp	.LBB1_16
.LBB1_11:
	ldi	r3, 0x00000030
	sub	r0, r3, r2
	ldi	r4, 0x00000009
	cmp	r2, r4
	jcc	b, .LBB1_16
; %bb.12:
	ldi	r5, 0x01ffff9f
	ild	r4, r5
	ldi	r6, 0x000003ff
	cmp	r4, r6
	jcc	gt, .LBB1_16
; %bb.13:
	ild	r6, r5
	ldi	r4, 0x01fffb9d
	add	r6, r4, r6
	ild	r6, r6
	ldi	r7, 0x0000000a
	mul	r6, r7, r6
	ldi	r7, 0x01ffff9d
	ild	r7, r7
	ild	r5, r5
	cmp	r7, r1
	jcc	z, .LBB1_15
; %bb.14:
	sub	r3, r0, r2
.LBB1_15:
	add	r5, r4, r1
	add	r6, r2, r2
	ist	r2, r1
	ldi	r1, 0x01ffffa0
	ldi	r2, 0x00000001
	ist	r2, r1
	ldi	r1, 0x01ffffa1
	ild	r3, r1
	add	r3, r2, r2
	ist	r2, r1
	ldi	r1, 0x05000000
	ist	r0, r1
	jmp	.LBB1_16
.LBB1_8:
	ldi	r0, 0x01ffffa0
	ild	r0, r0
	cmp	r0, r1
	jcc	nz, .LBB1_16
; %bb.9:
	ldi	r0, 0x01ffff9d
	ild	r3, r0
	cmp	r3, r1
	jcc	nz, .LBB1_16
; %bb.10:
	ldi	r1, 0x00000001
	ist	r1, r0
	ldi	r0, 0x01ffffa1
	ild	r3, r0
	add	r3, r1, r1
	ist	r1, r0
	ldi	r0, 0x05000000
	ist	r2, r0
	jmp	.LBB1_16
.Lfunc_end1:
	.size	handle_input, .Lfunc_end1-handle_input
                                        ; -- End function
	.p2align	2                               ; -- Begin function finish_number
	.type	finish_number,@function
finish_number:                          ; @finish_number
; %bb.0:
	pushfp
	stf
	osp	r14
	ldi	r15, 0x00000006
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
	ldi	r5, 0x01ffffa0
	ild	r0, r5
	ldi	r4, 0x00000000
	cmp	r0, r4
	jcc	z, .LBB2_9
; %bb.1:
	ldi	r6, 0x01ffffa1
	ild	r0, r6
	ldi	r7, 0x00000001
	cmp	r0, r7
	jcc	lt, .LBB2_4
; %bb.2:
	ldi	r0, 0x05000000
	ldi	r1, 0x00000008
	mov	r4, r2
.LBB2_3:                                ; =>This Inner Loop Header: Depth=1
	ist	r1, r0
	add	r2, r7, r2
	ild	r3, r6
	cmp	r2, r3
	jcc	lt, .LBB2_3
.LBB2_4:
	ldi	r0, 0x01ffffa5
	call	__aurora_put_string
	ldi	r8, 0x01ffff9f
	ild	r0, r8
	add	r0, r7, r0
	call	__aurora_put_int
	ldi	r0, 0x01ffffa2
	call	__aurora_put_string
	ldi	r9, 0x01fffb9d
	ild	r0, r8
	add	r0, r9, r0
	ild	r0, r0
	call	__aurora_put_int
	ldi	r0, 0x01ffffad
	call	__aurora_put_string
	ild	r0, r8
	ldi	r1, 0x00000400
	cmp	r0, r1
	jcc	ge, .LBB2_6
; %bb.5:
	ild	r0, r8
	add	r0, r7, r0
	ist	r0, r8
.LBB2_6:
	ist	r4, r5
	ldi	r0, 0x01ffff9d
	ist	r4, r0
	ist	r4, r6
	ild	r0, r8
	ldi	r1, 0x000003ff
	cmp	r0, r1
	jcc	gt, .LBB2_8
; %bb.7:
	ild	r0, r8
	add	r0, r9, r0
	ist	r4, r0
	jmp	.LBB2_8
.LBB2_9:
	ldi	r0, 0x01ffff9d
	ldi	r1, 0x00000000
	ist	r1, r0
.LBB2_8:
	ofp	r14
	ild	r9, r14                         ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000001
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000002
	ldo	r14, r15, r7                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000003
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000004
	ldo	r14, r15, r5                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000005
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	fts
	popfp
	ret
.Lfunc_end2:
	.size	finish_number, .Lfunc_end2-finish_number
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_put_int
	.type	__aurora_put_int,@function
__aurora_put_int:                       ; @__aurora_put_int
; %bb.0:
	ldi	r1, 0xffffffff
	cmp	r0, r1
	jcc	le, .LBB3_1
; %bb.7:
	ldi	r1, 0x00000000
	cmp	r0, r1
	jcc	nz, .LBB3_2
; %bb.8:
	ldi	r0, 0x05000000
	ldi	r1, 0x00000030
	ist	r1, r0
	ret
.LBB3_1:
	ldi	r1, 0x05000000
	ldi	r2, 0x0000002d
	ist	r2, r1
	ldi	r1, 0x00000000
	sub	r1, r0, r0
.LBB3_2:
	;APP
	osp	r1

	;NO_APP
	ldi	r2, 0x0000000a
	ldi	r3, 0x00000030
.LBB3_3:                                ; =>This Inner Loop Header: Depth=1
	rem	r0, r2, r10
	add	r10, r3, r10
	;APP
	push	r10

	;NO_APP
	div	r0, r2, r10
	cmp	r0, r2
	mov	r10, r0
	jcc	nc, .LBB3_3
; %bb.4:
	;APP
	osp	r0

	;NO_APP
	cmp	r0, r1
	jcc	nb, .LBB3_9
; %bb.5:
	ldi	r0, 0x05000000
.LBB3_6:                                ; =>This Inner Loop Header: Depth=1
	;APP
	pop	r2

	;NO_APP
	ist	r2, r0
	;APP
	osp	r2

	;NO_APP
	cmp	r2, r1
	jcc	b, .LBB3_6
.LBB3_9:
	ret
.Lfunc_end3:
	.size	__aurora_put_int, .Lfunc_end3-__aurora_put_int
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
	jcc	z, .LBB4_3
; %bb.1:
	ldi	r10, 0x00000001
	add	r0, r10, r11
	ldi	r0, 0x05000000
.LBB4_2:                                ; =>This Inner Loop Header: Depth=1
	ist	r3, r0
	add	r11, r10, r12
	ild	r3, r11
	and	r3, r1, r3
	cmp	r3, r2
	mov	r12, r11
	jcc	nz, .LBB4_2
.LBB4_3:
	ret
.Lfunc_end4:
	.size	__aurora_put_string, .Lfunc_end4-__aurora_put_string
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
