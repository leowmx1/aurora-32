	.file	"calculator.c"
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
	ldi	r15, 0x0000001f
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
	ldi	r0, 0x01ffffd8
	ldi	r1, 0x00000045
	ist	r1, r0
	mov	r1, r5
	ldi	r1, 0x01ffffd9
	ldi	r6, 0x0000006e
	ist	r6, r1
	ldi	r1, 0x01ffffda
	ldi	r7, 0x00000074
	ist	r7, r1
	ldi	r2, 0x01ffffdb
	ldi	r12, 0x00000065
	ist	r12, r2
	ldi	r10, 0x01ffffdc
	ldi	r2, 0x00000072
	ist	r2, r10
	ldi	r11, 0x01ffffdd
	ldi	r10, 0x00000020
	ist	r10, r11
	ldi	r11, 0x01ffffde
	ist	r12, r11
	ldi	r11, 0x01ffffdf
	ldi	r0, 0x00000078
	ofp	r14
	ldi	r15, 0x00000010
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffe0
	ldi	r0, 0x00000070
	ist	r0, r11
	mov	r0, r8
	ldi	r11, 0x01ffffe1
	ist	r2, r11
	ldi	r11, 0x01ffffe2
	ist	r12, r11
	ldi	r11, 0x01ffffe3
	ldi	r0, 0x00000073
	ofp	r14
	ldi	r15, 0x0000000c
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffe4
	ist	r0, r11
	ldi	r11, 0x01ffffe5
	ldi	r9, 0x00000069
	ist	r9, r11
	ldi	r11, 0x01ffffe6
	ldi	r0, 0x0000006f
	ist	r0, r11
	mov	r0, r3
	ldi	r11, 0x01ffffe7
	ist	r6, r11
	ldi	r11, 0x01ffffe8
	ldi	r0, 0x0000002c
	ofp	r14
	ldi	r15, 0x00000017
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffe9
	ist	r10, r11
	ldi	r11, 0x01ffffea
	ist	r12, r11
	ldi	r11, 0x01ffffeb
	ist	r6, r11
	ldi	r11, 0x01ffffec
	ldi	r0, 0x00000064
	ofp	r14
	ldi	r15, 0x00000012
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffed
	ist	r10, r11
	ldi	r11, 0x01ffffee
	ldi	r0, 0x00000077
	ofp	r14
	ldi	r15, 0x0000000d
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffef
	ist	r9, r11
	ldi	r11, 0x01fffff0
	ist	r7, r11
	ldi	r11, 0x01fffff1
	ldi	r0, 0x00000068
	ist	r0, r11
	ofp	r14
	ldi	r15, 0x00000007
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01fffff2
	ist	r10, r11
	ldi	r11, 0x01fffff3
	ldi	r4, 0x0000003c
	ist	r4, r11
	ldi	r11, 0x01fffff4
	mov	r5, r1
	ist	r1, r11
	ldi	r11, 0x01fffff5
	ist	r6, r11
	ldi	r11, 0x01fffff6
	ist	r7, r11
	ldi	r11, 0x01fffff7
	ist	r12, r11
	ldi	r11, 0x01fffff8
	ist	r2, r11
	ldi	r11, 0x01fffff9
	ldi	r4, 0x0000003e
	ist	r4, r11
	ldi	r11, 0x01fffffa
	ldi	r0, 0x0000003a
	ist	r0, r11
	mov	r0, r13
	ofp	r14
	ldi	r15, 0x0000001a
	add	r14, r15, r14
	ist	r13, r14                        ; 4-byte Folded Spill
	ldi	r11, 0x01fffffb
	ldi	r0, 0x0000000a
	ofp	r14
	ldi	r15, 0x00000006
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01fffffc
	ist	r4, r11
	ldi	r11, 0x01fffffd
	ist	r4, r11
	ldi	r11, 0x01fffffe
	ist	r10, r11
	ldi	r11, 0x01ffffff
	ldi	r5, 0x00000000
	ist	r5, r11
	ldi	r11, 0x01ffffb8
	ldi	r4, 0x00000049
	ist	r4, r11
	ldi	r11, 0x01ffffb9
	ist	r6, r11
	ldi	r11, 0x01ffffba
	ofp	r14
	ldi	r15, 0x00000011
	add	r14, r15, r14
	ist	r8, r14                         ; 4-byte Folded Spill
	ist	r8, r11
	ldi	r11, 0x01ffffbb
	ldi	r0, 0x00000075
	ist	r0, r11
	ofp	r14
	ldi	r15, 0x00000013
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01ffffbc
	ist	r7, r11
	ldi	r11, 0x01ffffbd
	ist	r1, r11
	ldi	r11, 0x01ffffbe
	ist	r2, r11
	ldi	r11, 0x01ffffbf
	ist	r2, r11
	ldi	r11, 0x01ffffc0
	ist	r3, r11
	ldi	r11, 0x01ffffc1
	ist	r2, r11
	ldi	r11, 0x01ffffc2
	ist	r13, r11
	ldi	r11, 0x01ffffc3
	ist	r10, r11
	ldi	r11, 0x01ffffc4
	ldi	r4, 0x0000004e
	ist	r4, r11
	ldi	r11, 0x01ffffc5
	ist	r3, r11
	mov	r3, r13
	ofp	r14
	ldi	r15, 0x00000009
	add	r14, r15, r14
	ist	r13, r14                        ; 4-byte Folded Spill
	ldi	r11, 0x01ffffc6
	ist	r10, r11
	ldi	r11, 0x01ffffc7
	ofp	r14
	ldi	r15, 0x0000000f
	add	r14, r15, r14
	ist	r9, r14                         ; 4-byte Folded Spill
	ist	r9, r11
	ldi	r11, 0x01ffffc8
	ist	r6, r11
	ldi	r11, 0x01ffffc9
	ist	r8, r11
	ldi	r11, 0x01ffffca
	ist	r0, r11
	ldi	r11, 0x01ffffcb
	ist	r7, r11
	ldi	r11, 0x01ffffcc
	ist	r10, r11
	ldi	r11, 0x01ffffcd
	ist	r2, r11
	ldi	r11, 0x01ffffce
	ist	r12, r11
	ldi	r11, 0x01ffffcf
	ldi	r4, 0x00000063
	ist	r4, r11
	ofp	r14
	ldi	r15, 0x00000016
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01ffffd0
	ist	r12, r11
	ldi	r11, 0x01ffffd1
	ist	r9, r11
	ldi	r11, 0x01ffffd2
	ldi	r0, 0x00000076
	ofp	r14
	ldi	r15, 0x00000019
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffd3
	ist	r12, r11
	ldi	r11, 0x01ffffd4
	ofp	r14
	ldi	r15, 0x00000012
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r11
	ldi	r11, 0x01ffffd5
	ldi	r0, 0x0000002e
	ofp	r14
	ldi	r15, 0x0000000a
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffd6
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r0, r11
	ldi	r11, 0x01ffffd7
	ist	r5, r11
	ldi	r11, 0x01ffffb6
	ist	r0, r11
	ldi	r11, 0x01ffffb7
	ist	r5, r11
	ldi	r11, 0x01ffff98
	ldi	r0, 0x00000053
	ofp	r14
	ldi	r15, 0x00000015
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffff99
	ldi	r0, 0x00000079
	ofp	r14
	ldi	r15, 0x0000000e
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffff9a
	ofp	r14
	ldi	r15, 0x0000000b
	add	r14, r15, r14
	ist	r6, r14                         ; 4-byte Folded Spill
	ist	r6, r11
	ldi	r11, 0x01ffff9b
	ist	r7, r11
	ldi	r11, 0x01ffff9c
	ldi	r0, 0x00000061
	ist	r0, r11
	mov	r0, r9
	ofp	r14
	ldi	r15, 0x0000001c
	add	r14, r15, r14
	ist	r9, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01ffff9d
	ofp	r14
	ldi	r15, 0x00000010
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r0, r11
	ldi	r11, 0x01ffff9e
	ist	r1, r11
	ldi	r11, 0x01ffff9f
	ist	r2, r11
	ldi	r11, 0x01ffffa0
	ist	r2, r11
	ldi	r11, 0x01ffffa1
	ist	r13, r11
	ldi	r11, 0x01ffffa2
	ist	r2, r11
	ldi	r11, 0x01ffffa3
	ofp	r14
	ldi	r15, 0x0000001a
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r11
	ldi	r11, 0x01ffffa4
	ist	r10, r11
	ldi	r11, 0x01ffffa5
	ldi	r8, 0x00000055
	ofp	r14
	ldi	r15, 0x00000014
	add	r14, r15, r14
	ist	r8, r14                         ; 4-byte Folded Spill
	ist	r8, r11
	ldi	r11, 0x01ffffa6
	ist	r6, r11
	ldi	r11, 0x01ffffa7
	ist	r12, r11
	ldi	r11, 0x01ffffa8
	ist	r0, r11
	ldi	r11, 0x01ffffa9
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r0, r11
	ldi	r11, 0x01ffffaa
	ist	r12, r11
	ldi	r11, 0x01ffffab
	ist	r4, r11
	ldi	r11, 0x01ffffac
	ist	r7, r11
	ofp	r14
	ldi	r15, 0x0000001d
	add	r14, r15, r14
	ist	r7, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01ffffad
	ist	r12, r11
	ldi	r11, 0x01ffffae
	ist	r3, r11
	ldi	r11, 0x01ffffaf
	ist	r10, r11
	ldi	r0, 0x01ffffb0
	ldi	r11, 0x00000027
	ofp	r14
	ldi	r15, 0x00000008
	add	r14, r15, r14
	ist	r11, r14                        ; 4-byte Folded Spill
	ist	r11, r0
	ldi	r0, 0x01ffffb1
	ldi	r3, 0x00000029
	ofp	r14
	ldi	r15, 0x0000001e
	add	r14, r15, r14
	ist	r3, r14                         ; 4-byte Folded Spill
	ist	r3, r0
	ldi	r0, 0x01ffffb2
	ist	r11, r0
	ldi	r0, 0x01ffffb3
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffffb4
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r8, r0
	ldi	r0, 0x01ffffb5
	ist	r5, r0
	ldi	r0, 0x01ffff7a
	ldi	r11, 0x0000004d
	ofp	r14
	ldi	r15, 0x00000018
	add	r14, r15, r14
	ist	r11, r14                        ; 4-byte Folded Spill
	ist	r11, r0
	ldi	r0, 0x01ffff7b
	ist	r9, r0
	ldi	r0, 0x01ffff7c
	ist	r7, r0
	ldi	r0, 0x01ffff7d
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ist	r11, r0
	ldi	r0, 0x01ffff7e
	ist	r1, r0
	mov	r1, r7
	ldi	r0, 0x01ffff7f
	ist	r2, r0
	ldi	r0, 0x01ffff80
	ist	r2, r0
	ldi	r0, 0x01ffff81
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	ist	r6, r0
	ldi	r0, 0x01ffff82
	ist	r2, r0
	ldi	r0, 0x01ffff83
	mov	r13, r1
	ist	r1, r0
	ldi	r0, 0x01ffff84
	ist	r10, r0
	ldi	r0, 0x01ffff85
	ldi	r9, 0x00000044
	ist	r9, r0
	ldi	r0, 0x01ffff86
	ofp	r14
	ldi	r15, 0x0000000f
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ist	r11, r0
	ldi	r0, 0x01ffff87
	ofp	r14
	ldi	r15, 0x00000019
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r0
	ldi	r0, 0x01ffff88
	ist	r11, r0
	ldi	r0, 0x01ffff89
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	ist	r4, r0
	ldi	r0, 0x01ffff8a
	ist	r11, r0
	ldi	r0, 0x01ffff8b
	ist	r6, r0
	ldi	r0, 0x01ffff8c
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r0
	ldi	r0, 0x01ffff8d
	ist	r10, r0
	ldi	r0, 0x01ffff8e
	ldi	r4, 0x00000062
	ist	r4, r0
	ldi	r0, 0x01ffff8f
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ist	r11, r0
	ldi	r0, 0x01ffff90
	ist	r10, r0
	ldi	r0, 0x01ffff91
	ldi	r4, 0x0000007a
	ist	r4, r0
	ldi	r0, 0x01ffff92
	ist	r12, r0
	ldi	r0, 0x01ffff93
	ist	r2, r0
	ldi	r0, 0x01ffff94
	ist	r6, r0
	ldi	r0, 0x01ffff95
	mov	r3, r9
	ist	r9, r0
	ldi	r0, 0x01ffff96
	mov	r8, r4
	ist	r4, r0
	ldi	r0, 0x01ffff97
	ist	r5, r0
	ldi	r0, 0x01ffff5e
	ofp	r14
	ldi	r15, 0x00000015
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff5f
	ist	r11, r0
	ldi	r0, 0x01ffff60
	ist	r13, r0
	ldi	r0, 0x01ffff61
	ofp	r14
	ldi	r15, 0x0000001d
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r0
	ldi	r0, 0x01ffff62
	ofp	r14
	ldi	r15, 0x0000001c
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r8, r0
	ldi	r0, 0x01ffff63
	ofp	r14
	ldi	r15, 0x00000010
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff64
	mov	r7, r11
	ist	r11, r0
	ldi	r0, 0x01ffff65
	ist	r2, r0
	ldi	r0, 0x01ffff66
	ist	r2, r0
	ldi	r0, 0x01ffff67
	ist	r6, r0
	ldi	r0, 0x01ffff68
	ist	r2, r0
	ldi	r0, 0x01ffff69
	mov	r1, r7
	ist	r7, r0
	ldi	r0, 0x01ffff6a
	ist	r10, r0
	ldi	r0, 0x01ffff6b
	ist	r11, r0
	ldi	r0, 0x01ffff6c
	ist	r3, r0
	ldi	r0, 0x01ffff6d
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff6e
	ist	r12, r0
	ldi	r0, 0x01ffff6f
	ofp	r14
	ldi	r15, 0x00000016
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff70
	mov	r13, r3
	ist	r3, r0
	ldi	r0, 0x01ffff71
	ist	r12, r0
	ldi	r0, 0x01ffff72
	ofp	r14
	ldi	r15, 0x00000012
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r0
	ldi	r0, 0x01ffff73
	ist	r10, r0
	ldi	r0, 0x01ffff74
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r0
	ldi	r0, 0x01ffff75
	ofp	r14
	ldi	r15, 0x0000001e
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	ist	r6, r0
	ldi	r0, 0x01ffff76
	ist	r13, r0
	ldi	r0, 0x01ffff77
	ist	r9, r0
	ldi	r0, 0x01ffff78
	ist	r4, r0
	ldi	r0, 0x01ffff79
	ist	r5, r0
	ldi	r0, 0x01ffff3f
	ofp	r14
	ldi	r15, 0x00000018
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r0
	ldi	r0, 0x01ffff40
	mov	r8, r9
	ist	r9, r0
	ldi	r0, 0x01ffff41
	ist	r3, r0
	mov	r3, r8
	ldi	r0, 0x01ffff42
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	ist	r4, r0
	ldi	r0, 0x01ffff43
	ist	r11, r0
	ldi	r0, 0x01ffff44
	ist	r2, r0
	ldi	r0, 0x01ffff45
	ist	r2, r0
	ldi	r0, 0x01ffff46
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	ist	r6, r0
	ldi	r0, 0x01ffff47
	ist	r2, r0
	ldi	r0, 0x01ffff48
	ist	r7, r0
	ldi	r0, 0x01ffff49
	ist	r10, r0
	ldi	r0, 0x01ffff4a
	ofp	r14
	ldi	r15, 0x00000014
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff4b
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff4c
	ldi	r4, 0x0000006b
	ist	r4, r0
	ldi	r0, 0x01ffff4d
	ist	r3, r0
	ldi	r0, 0x01ffff4e
	ist	r6, r0
	ldi	r0, 0x01ffff4f
	ofp	r14
	ldi	r15, 0x0000000d
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	ist	r4, r0
	ldi	r0, 0x01ffff50
	ist	r3, r0
	ldi	r0, 0x01ffff51
	ist	r10, r0
	ldi	r0, 0x01ffff52
	mov	r6, r3
	ist	r3, r0
	ldi	r0, 0x01ffff53
	ist	r1, r0
	ldi	r0, 0x01ffff54
	ist	r12, r0
	ldi	r0, 0x01ffff55
	ist	r2, r0
	ldi	r0, 0x01ffff56
	ist	r9, r0
	ldi	r0, 0x01ffff57
	ist	r8, r0
	mov	r8, r11
	ldi	r0, 0x01ffff58
	ist	r3, r0
	ldi	r0, 0x01ffff59
	ist	r2, r0
	ldi	r0, 0x01ffff5a
	ist	r10, r0
	ldi	r0, 0x01ffff5b
	ldi	r4, 0x00000028
	ofp	r14
	ldi	r15, 0x0000001b
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ist	r4, r0
	ldi	r0, 0x01ffff5c
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff5d
	ist	r5, r0
	ldi	r0, 0x01ffff3a
	ist	r1, r0
	ldi	r0, 0x01ffff3b
	ofp	r14
	ldi	r15, 0x0000001e
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff3c
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff3d
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff3e
	ist	r5, r0
	ldi	r0, 0x01ffff31
	ldi	r4, 0x00000052
	ist	r4, r0
	ldi	r0, 0x01ffff32
	ist	r12, r0
	ldi	r0, 0x01ffff33
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	ist	r13, r0
	ldi	r0, 0x01ffff34
	ofp	r14
	ldi	r15, 0x00000013
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r8, r0
	ldi	r0, 0x01ffff35
	ldi	r4, 0x0000006c
	ist	r4, r0
	ldi	r0, 0x01ffff36
	ist	r11, r0
	ldi	r0, 0x01ffff37
	ist	r7, r0
	ldi	r0, 0x01ffff38
	ist	r10, r0
	ldi	r0, 0x01ffff39
	ist	r5, r0
	ldi	r0, 0x01ffff2b
	ldi	r4, 0x00000051
	ist	r4, r0
	ldi	r0, 0x01ffff2c
	ist	r8, r0
	ldi	r0, 0x01ffff2d
	ofp	r14
	ldi	r15, 0x0000000f
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r8, r0
	ldi	r0, 0x01ffff2e
	ist	r11, r0
	ldi	r0, 0x01ffff2f
	ist	r3, r0
	ldi	r0, 0x01ffff30
	ist	r5, r0
	ldi	r0, 0x01ffff03
	ist	r1, r0
	ldi	r0, 0x01ffff04
	ldi	r4, 0x00000057
	ist	r4, r0
	ldi	r0, 0x01ffff05
	ist	r9, r0
	ldi	r0, 0x01ffff06
	ist	r2, r0
	ldi	r0, 0x01ffff07
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff08
	ist	r8, r0
	ldi	r0, 0x01ffff09
	ist	r3, r0
	ldi	r0, 0x01ffff0a
	ldi	r4, 0x00000067
	ist	r4, r0
	ldi	r0, 0x01ffff0b
	ist	r7, r0
	ldi	r0, 0x01ffff0c
	ist	r10, r0
	ldi	r0, 0x01ffff0d
	ldi	r4, 0x00000054
	ist	r4, r0
	ldi	r0, 0x01ffff0e
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	ist	r4, r0
	ldi	r0, 0x01ffff0f
	ist	r12, r0
	ldi	r0, 0x01ffff10
	ist	r2, r0
	ldi	r0, 0x01ffff11
	ist	r12, r0
	ldi	r0, 0x01ffff12
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff13
	ist	r2, r0
	ldi	r0, 0x01ffff14
	ist	r12, r0
	ldi	r0, 0x01ffff15
	ist	r10, r0
	ldi	r0, 0x01ffff16
	ist	r3, r0
	ldi	r0, 0x01ffff17
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff18
	ist	r10, r0
	ldi	r0, 0x01ffff19
	ist	r4, r0
	ldi	r0, 0x01ffff1a
	ist	r8, r0
	ldi	r0, 0x01ffff1b
	ist	r13, r0
	ldi	r0, 0x01ffff1c
	ist	r11, r0
	ldi	r0, 0x01ffff1d
	ist	r1, r0
	ldi	r0, 0x01ffff1e
	ist	r2, r0
	ldi	r0, 0x01ffff1f
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff20
	ist	r10, r0
	ldi	r0, 0x01ffff21
	ist	r9, r0
	ldi	r0, 0x01ffff22
	ist	r3, r0
	ldi	r0, 0x01ffff23
	ist	r13, r0
	ldi	r0, 0x01ffff24
	ofp	r14
	ldi	r15, 0x0000000d
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff25
	ist	r12, r0
	ldi	r0, 0x01ffff26
	ist	r2, r0
	ldi	r0, 0x01ffff27
	ist	r13, r0
	ldi	r0, 0x01ffff28
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff29
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff2a
	ist	r5, r0
	ldi	r0, 0x01fffee2
	lda	r1, __aurora_enable_interrupt
	ist	r1, r0
	ldi	r0, 0x01fffee3
	lda	r2, __aurora_disable_interrupt
	ist	r2, r0
	ldi	r0, 0x01fffee4
	lda	r3, __aurora_set_up_stack_pointer
	ist	r3, r0
	ldi	r0, 0x01fffee5
	lda	r3, __aurora_set_stack_pointer
	ist	r3, r0
	ldi	r0, 0x01fffee6
	lda	r10, __aurora_get_stack_pointer
	ist	r10, r0
	ldi	r0, 0x01fffee7
	lda	r11, __aurora_halt
	ist	r11, r0
	ldi	r0, 0x01fffee8
	lda	r11, __aurora_nop
	ist	r11, r0
	ldi	r0, 0x01fffee9
	lda	r11, __aurora_read
	ist	r11, r0
	ldi	r0, 0x01fffeea
	lda	r11, __aurora_write
	ist	r11, r0
	ldi	r0, 0x01fffeeb
	ist	r1, r0
	ldi	r0, 0x01fffeec
	ist	r2, r0
	ldi	r0, 0x01fffeed
	lda	r1, __aurora_register_interrupt
	ist	r1, r0
	ldi	r0, 0x01fffeee
	ist	r3, r0
	ldi	r0, 0x01fffeef
	ist	r10, r0
	ldi	r0, 0x01fffef0
	lda	r1, __aurora_push
	ist	r1, r0
	ldi	r0, 0x01fffef1
	lda	r1, __aurora_pop
	ist	r1, r0
	ldi	r0, 0x01fffef2
	lda	r1, __aurora_get_char
	ist	r1, r0
	ldi	r0, 0x01fffef3
	lda	r1, __aurora_put_char
	ist	r1, r0
	ldi	r0, 0x01fffef4
	lda	r1, __aurora_put_int
	ist	r1, r0
	ldi	r0, 0x01fffef5
	lda	r1, __aurora_put_string
	ist	r1, r0
	ldi	r0, 0x01fffef6
	ist	r5, r0
	ldi	r0, 0x01fffef7
	ldi	r1, 0x01000000
	ist	r1, r0
	ldi	r0, 0x01fffef8
	ldi	r2, 0x02000000
	ist	r2, r0
	ldi	r0, 0x01fffef9
	ldi	r2, 0x03000000
	ist	r2, r0
	ldi	r0, 0x01fffefa
	ldi	r3, 0x04000000
	ist	r3, r0
	ldi	r0, 0x01fffefb
	ldi	r10, 0x05000000
	ist	r10, r0
	ldi	r0, 0x01fffefc
	ldi	r11, 0x06000000
	ist	r11, r0
	ldi	r0, 0x01fffefd
	ist	r5, r0
	ldi	r0, 0x01fffefe
	ist	r1, r0
	ldi	r0, 0x01fffeff
	ist	r2, r0
	ldi	r0, 0x01ffff00
	ist	r3, r0
	ldi	r0, 0x01ffff01
	ist	r10, r0
	ldi	r0, 0x01ffff02
	ist	r11, r0
	ldi	r9, 0x01fffee1
	ist	r5, r9
	ldi	r0, 0x01fffee0
	ofp	r14
	ldi	r15, 0x0000000a
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r5, r0
	ldi	r0, 0x01fffedf
	ofp	r14
	ldi	r15, 0x0000001c
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r5, r0
	ldi	r0, 0x01fffede
	ofp	r14
	ldi	r15, 0x00000008
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r5, r0
	ldi	r7, 0x01fffedd
	ist	r5, r7
	ldi	r0, 0x01fffedc
	ofp	r14
	ldi	r15, 0x0000001d
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r5, r0
	ldi	r8, 0x01fffadc
	ldi	r6, 0x00000400
	mov	r8, r0
	mov	r6, r1
	call	__aurora_zero_words
	ldi	r0, 0x01fffadb
	ofp	r14
	ldi	r15, 0x00000006
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r5, r0
	ldi	r4, 0x01fff6db
	mov	r4, r0
	mov	r6, r1
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	call	__aurora_zero_words
	mov	r4, r10
	mov	r8, r2
                                        ; implicit-def: $r0
                                        ; kill: killed $r0
	ofp	r14
	ldi	r15, 0x00000019
	add	r14, r15, r14
	ist	r9, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x0000000b
	add	r14, r15, r14
	ist	r2, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000011
	add	r14, r15, r14
	ist	r10, r14                        ; 4-byte Folded Spill
	jmp	.LBB1_1+0
.LBB1_4:                                ;   in Loop: Header=BB1_1 Depth=1
	ldi	r0, 0x01ffffb8
	call	__aurora_put_string
	ist	r8, r6
.LBB1_1:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_2 Depth 2
                                        ;     Child Loop BB1_7 Depth 2
                                        ;       Child Loop BB1_34 Depth 3
                                        ;       Child Loop BB1_22 Depth 3
                                        ;     Child Loop BB1_43 Depth 2
                                        ;     Child Loop BB1_48 Depth 2
                                        ;     Child Loop BB1_53 Depth 2
	call	__aurora_set_up_stack_pointer
	call	__aurora_disable_interrupt
	ldi	r0, 0x03000007
	lda	r1, handle_timer
	call	__aurora_register_interrupt
	ldi	r4, 0xffffffff
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r4, r8
	ist	r5, r7
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r5, r0
	ist	r5, r9
	ist	r5, r6
	ldi	r0, 0x01ffffd8
	call	__aurora_put_string
	;APP
	ei

	;NO_APP
.LBB1_2:                                ;   Parent Loop BB1_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ild	r0, r7
	cmp	r0, r5
	jcc	z, .LBB1_2+0
; %bb.3:                                ;   in Loop: Header=BB1_1 Depth=1
	call	__aurora_disable_interrupt
	ild	r0, r8
	cmp	r0, r4
	ldi	r8, 0x00000001
	jcc	le, .LBB1_4+0
; %bb.5:                                ;   in Loop: Header=BB1_1 Depth=1
	call	__aurora_get_stack_pointer
	ofp	r14
	ldi	r15, 0x00000007
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ild	r0, r0
	cmp	r0, r5
	ldi	r11, 0x00000002
	ldi	r0, 0x0000002d
	ofp	r14
	ldi	r15, 0x00000013
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x0000002f
	ofp	r14
	ldi	r15, 0x0000001a
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x0000002a
	ofp	r14
	ldi	r15, 0x00000016
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x0000002b
	ofp	r14
	ldi	r15, 0x00000012
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	mov	r5, r12
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	add	r14, r15, r14
	ist	r11, r14                        ; 4-byte Folded Spill
	jcc	lt, .LBB1_40+0
; %bb.6:                                ;   in Loop: Header=BB1_1 Depth=1
	ldi	r13, 0x00000000
	mov	r13, r12
	mov	r13, r0
	ofp	r14
	ldi	r15, 0x00000015
	add	r14, r15, r14
	ist	r13, r14                        ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000010
	add	r14, r15, r14
	ist	r8, r14                         ; 4-byte Folded Spill
	jmp	.LBB1_7+0
.LBB1_9:                                ;   in Loop: Header=BB1_7 Depth=2
	sft	r12, r8, r1
	add	r1, r3, r2
	ist	r8, r2
	ldi	r2, 0x01fffadd
	add	r1, r2, r1
	ldi	r2, 0x01fff6dc
	add	r0, r2, r0
	ild	r0, r0
	ist	r0, r1
	add	r12, r8, r12
.LBB1_39:                               ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x0000000d
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	add	r2, r8, r0
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ild	r1, r1
	cmp	r2, r1
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	jcc	ge, .LBB1_40+0
.LBB1_7:                                ;   Parent Loop BB1_1 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB1_34 Depth 3
                                        ;       Child Loop BB1_22 Depth 3
	mov	r0, r1
	ild	r0, r6
	cmp	r0, r13
	jcc	nz, .LBB1_40+0
; %bb.8:                                ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x0000000d
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	sft	r1, r8, r0
	add	r0, r10, r1
	ild	r2, r1
	cmp	r2, r8
	jcc	z, .LBB1_9+0
; %bb.10:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r1, r1
	cmp	r1, r11
	jcc	nz, .LBB1_39+0
; %bb.11:                               ;   in Loop: Header=BB1_7 Depth=2
	ldi	r1, 0x01fff6dc
	add	r0, r1, r0
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x00000012
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB1_17+0
; %bb.12:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x00000013
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB1_17+0
; %bb.13:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x00000016
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB1_17+0
; %bb.14:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x0000001a
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB1_17+0
; %bb.15:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r1, r0
	ldi	r2, 0x0000007e
	cmp	r1, r2
	jcc	z, .LBB1_17+0
; %bb.16:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r1, r0
	ldi	r2, 0x0000007f
	cmp	r1, r2
	jcc	nz, .LBB1_26+0
.LBB1_17:                               ;   in Loop: Header=BB1_7 Depth=2
	mov	r9, r4
	ofp	r14
	ldi	r15, 0x00000009
	add	r14, r15, r14
	ist	r12, r14                        ; 4-byte Folded Spill
	ild	r6, r0
	mov	r6, r0
	call	priority
	ofp	r14
	ldi	r15, 0x0000000e
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	call	__aurora_get_stack_pointer
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r9                    ; 4-byte Folded Reload
	cmp	r0, r9
	jcc	nz, .LBB1_19+0
; %bb.18:                               ;   in Loop: Header=BB1_7 Depth=2
	mov	r6, r0
	call	__aurora_push
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r12                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000015
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	mov	r4, r9
	jmp	.LBB1_39+0
.LBB1_19:                               ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x00000018
	add	r14, r15, r14
	ist	r6, r14                         ; 4-byte Folded Spill
	call	__aurora_pop
	mov	r0, r6
	call	__aurora_push
	mov	r6, r0
	call	priority
	mov	r0, r4
	call	__aurora_get_stack_pointer
	sub	r0, r9, r0
	mov	r6, r1
	mov	r4, r2
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	mov	r4, r3
	call	should_pop_operator
	ldi	r1, 0x00000000
	ofp	r14
	ldi	r15, 0x0000000f
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	cmp	r0, r1
	jcc	z, .LBB1_20+0
; %bb.21:                               ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	add	r0, r8, r1
	sft	r0, r8, r0
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	add	r0, r2, r9
	mov	r8, r2
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
.LBB1_22:                               ;   Parent Loop BB1_1 Depth=1
                                        ;     Parent Loop BB1_7 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	mov	r1, r6
	ist	r8, r9
	add	r9, r2, r4
	call	__aurora_pop
	ist	r0, r4
	call	__aurora_get_stack_pointer
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	cmp	r0, r1
	jcc	z, .LBB1_23+0
; %bb.24:                               ;   in Loop: Header=BB1_22 Depth=3
	call	__aurora_pop
	mov	r0, r4
	call	__aurora_push
	mov	r4, r0
	call	priority
	ofp	r14
	ldi	r15, 0x00000009
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	call	__aurora_get_stack_pointer
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	sub	r0, r1, r0
	mov	r4, r1
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	mov	r4, r3
	call	should_pop_operator
	add	r9, r8, r9
	ofp	r14
	ldi	r15, 0x00000010
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	add	r6, r3, r1
	ofp	r14
	ldi	r15, 0x0000000f
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r0, r2
	mov	r3, r2
	jcc	nz, .LBB1_22+0
	jmp	.LBB1_25+0
.LBB1_20:                               ;   in Loop: Header=BB1_7 Depth=2
	mov	r8, r2
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	jmp	.LBB1_25+0
.LBB1_23:                               ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x00000010
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
.LBB1_25:                               ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x00000018
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	mov	r2, r8
	call	__aurora_push
	mov	r6, r12
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000019
	ldo	r14, r15, r9                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
.LBB1_32:                               ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x00000015
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	jmp	.LBB1_39+0
.LBB1_26:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x0000001b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	nz, .LBB1_28+0
; %bb.27:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r0, r0
	mov	r12, r4
	call	__aurora_push
	ofp	r14
	ldi	r15, 0x00000015
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	mov	r4, r12
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	jmp	.LBB1_29+0
.LBB1_28:                               ;   in Loop: Header=BB1_7 Depth=2
	ild	r0, r0
	ofp	r14
	ldi	r15, 0x0000001e
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	cmp	r0, r1
	jcc	nz, .LBB1_29+0
; %bb.30:                               ;   in Loop: Header=BB1_7 Depth=2
	mov	r12, r4
	call	__aurora_pop
	mov	r0, r2
	ofp	r14
	ldi	r15, 0x0000001b
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	cmp	r2, r0
	jcc	nz, .LBB1_33+0
; %bb.31:                               ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	mov	r4, r12
	jmp	.LBB1_32+0
.LBB1_29:                               ;   in Loop: Header=BB1_7 Depth=2
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	jmp	.LBB1_39+0
.LBB1_33:                               ;   in Loop: Header=BB1_7 Depth=2
	mov	r4, r12
	sft	r12, r8, r0
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	add	r0, r1, r6
.LBB1_34:                               ;   Parent Loop BB1_1 Depth=1
                                        ;     Parent Loop BB1_7 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ofp	r14
	ldi	r15, 0x00000009
	add	r14, r15, r14
	ist	r2, r14                         ; 4-byte Folded Spill
	mov	r12, r9
	call	__aurora_get_stack_pointer
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	cmp	r0, r4
	jcc	nb, .LBB1_37+0
; %bb.35:                               ;   in Loop: Header=BB1_34 Depth=3
	call	__aurora_get_stack_pointer
	cmp	r0, r4
	jcc	z, .LBB1_36+0
; %bb.38:                               ;   in Loop: Header=BB1_34 Depth=3
	mov	r8, r4
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r8, r6
	add	r6, r4, r0
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	add	r6, r8, r6
	add	r9, r4, r9
	call	__aurora_pop
	mov	r9, r12
	mov	r8, r11
	mov	r4, r8
	mov	r0, r2
	ofp	r14
	ldi	r15, 0x0000001b
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	cmp	r2, r0
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000019
	ldo	r14, r15, r9                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000015
	ldo	r14, r15, r13                   ; 4-byte Folded Reload
	jcc	nz, .LBB1_34+0
	jmp	.LBB1_39+0
.LBB1_36:                               ;   in Loop: Header=BB1_1 Depth=1
	ldi	r0, 0x01ffff98
	call	__aurora_put_string
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r8, r0
.LBB1_37:                               ;   in Loop: Header=BB1_1 Depth=1
	ldi	r0, 0x01ffff98
	call	__aurora_put_string
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	ist	r8, r6
	mov	r9, r12
.LBB1_40:                               ;   in Loop: Header=BB1_1 Depth=1
	mov	r12, r9
	call	__aurora_get_stack_pointer
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	cmp	r0, r1
	jcc	nb, .LBB1_41+0
; %bb.42:                               ;   in Loop: Header=BB1_1 Depth=1
	sft	r9, r8, r0
	ldi	r1, 0x01fffadc
	add	r0, r1, r4
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
.LBB1_43:                               ;   Parent Loop BB1_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ild	r0, r6
	ldi	r1, 0x00000000
	cmp	r0, r1
	jcc	nz, .LBB1_45+0
; %bb.44:                               ;   in Loop: Header=BB1_43 Depth=2
	call	__aurora_pop
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r4
	add	r4, r8, r1
	ist	r0, r1
	add	r9, r8, r9
	add	r4, r2, r4
	call	__aurora_get_stack_pointer
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	cmp	r0, r1
	jcc	b, .LBB1_43+0
.LBB1_45:                               ;   in Loop: Header=BB1_1 Depth=1
	cmp	r9, r8
	jcc	lt, .LBB1_46+0
.LBB1_47:                               ;   in Loop: Header=BB1_1 Depth=1
	mov	r2, r6
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	jmp	.LBB1_48+0
.LBB1_55:                               ;   in Loop: Header=BB1_48 Depth=2
	add	r6, r8, r0
	ild	r0, r0
	call	__aurora_push
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
.LBB1_75:                               ;   in Loop: Header=BB1_48 Depth=2
	add	r6, r1, r6
	sub	r9, r8, r9
	cmp	r9, r4
	jcc	z, .LBB1_49+0
.LBB1_48:                               ;   Parent Loop BB1_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ild	r0, r0
	ldi	r4, 0x00000000
	cmp	r0, r4
	jcc	nz, .LBB1_49+0
; %bb.54:                               ;   in Loop: Header=BB1_48 Depth=2
	ild	r0, r6
	cmp	r0, r8
	jcc	z, .LBB1_55+0
; %bb.56:                               ;   in Loop: Header=BB1_48 Depth=2
	ild	r0, r6
	cmp	r0, r1
	jcc	nz, .LBB1_75+0
; %bb.57:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000009
	add	r14, r15, r14
	ist	r9, r14                         ; 4-byte Folded Spill
	add	r6, r8, r9
	call	__aurora_pop
	ofp	r14
	ldi	r15, 0x00000007
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	call	__aurora_pop
	ild	r1, r9
	ofp	r14
	ldi	r15, 0x00000017
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	gt, .LBB1_62+0
; %bb.58:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x0000001b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB1_71+0
; %bb.59:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000016
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB1_67+0
; %bb.60:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000012
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	nz, .LBB1_72+0
; %bb.61:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	add	r0, r1, r0
	jmp	.LBB1_74+0
.LBB1_62:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000013
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB1_66+0
; %bb.63:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x0000001a
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB1_68+0
; %bb.64:                               ;   in Loop: Header=BB1_48 Depth=2
	ldi	r2, 0x0000007f
	cmp	r1, r2
	jcc	nz, .LBB1_72+0
; %bb.65:                               ;   in Loop: Header=BB1_48 Depth=2
	call	__aurora_push
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	sub	r4, r0, r0
	jmp	.LBB1_74+0
.LBB1_71:                               ;   in Loop: Header=BB1_48 Depth=2
	ldi	r0, 0x01ffff5e
	jmp	.LBB1_73+0
.LBB1_66:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	sub	r0, r1, r0
	jmp	.LBB1_74+0
.LBB1_72:                               ;   in Loop: Header=BB1_48 Depth=2
	ldi	r0, 0x01ffff3f
	call	__aurora_put_string
	ild	r0, r9
	call	__aurora_put_char
	ldi	r0, 0x01ffff3a
	jmp	.LBB1_73+0
.LBB1_67:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	mul	r0, r1, r0
	jmp	.LBB1_74+0
.LBB1_68:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r2, r4
	jcc	z, .LBB1_69+0
; %bb.70:                               ;   in Loop: Header=BB1_48 Depth=2
	sdiv	r0, r2, r0
	jmp	.LBB1_74+0
.LBB1_69:                               ;   in Loop: Header=BB1_48 Depth=2
	ldi	r0, 0x01ffff7a
.LBB1_73:                               ;   in Loop: Header=BB1_48 Depth=2
	call	__aurora_put_string
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r8, r0
	ofp	r14
	ldi	r15, 0x00000014
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
.LBB1_74:                               ;   in Loop: Header=BB1_48 Depth=2
	ofp	r14
	ldi	r15, 0x00000014
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	call	__aurora_push
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r9                    ; 4-byte Folded Reload
	jmp	.LBB1_75+0
.LBB1_41:                               ;   in Loop: Header=BB1_1 Depth=1
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r9, r8
	jcc	ge, .LBB1_47+0
.LBB1_46:                               ;   in Loop: Header=BB1_1 Depth=1
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
.LBB1_49:                               ;   in Loop: Header=BB1_1 Depth=1
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	ild	r0, r6
	ldi	r4, 0x00000000
	cmp	r0, r4
	jcc	nz, .LBB1_51+0
; %bb.50:                               ;   in Loop: Header=BB1_1 Depth=1
	ldi	r0, 0x01ffff31
	call	__aurora_put_string
	call	__aurora_pop
	ofp	r14
	ldi	r15, 0x0000001d
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r0, r1
	ild	r0, r1
	call	__aurora_put_int
	ldi	r0, 0x0000000a
	call	__aurora_put_char
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000001c
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r8, r0
.LBB1_51:                               ;   in Loop: Header=BB1_1 Depth=1
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ild	r0, r0
	cmp	r0, r8
	ofp	r14
	ldi	r15, 0x00000019
	ldo	r14, r15, r9                    ; 4-byte Folded Reload
	jcc	lt, .LBB1_1+0
; %bb.52:                               ;   in Loop: Header=BB1_1 Depth=1
	mov	r4, r0
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
.LBB1_53:                               ;   Parent Loop BB1_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	r0, r10, r1
	ist	r4, r1
	add	r0, r2, r1
	ist	r4, r1
	add	r0, r8, r0
	ild	r1, r3
	cmp	r0, r1
	jcc	lt, .LBB1_53+0
	jmp	.LBB1_1+0
.Lfunc_end1:
	.size	_start, .Lfunc_end1-_start
                                        ; -- End function
	.p2align	2                               ; -- Begin function priority
	.type	priority,@function
priority:                               ; @priority
; %bb.0:
	pushfp
	stf
	osp	r14
	ldi	r15, 0x00000002
	add	r14, r15, r14
	lsp	r14
	ofp	r1
	ldi	r15, 0x00000001
	add	r1, r15, r1
	ist	r0, r1
	ild	r0, r1
	ldi	r1, 0x0000002b
	cmp	r0, r1
	jcc	z, .LBB2_2+0
	jmp	.LBB2_1+0
.LBB2_1:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000002d
	cmp	r0, r1
	jcc	nz, .LBB2_3+0
	jmp	.LBB2_2+0
.LBB2_2:
	ofp	r0
	ldi	r1, 0x00000001
	ist	r1, r0
	jmp	.LBB2_10+0
.LBB2_3:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000002a
	cmp	r0, r1
	jcc	z, .LBB2_5+0
	jmp	.LBB2_4+0
.LBB2_4:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000002f
	cmp	r0, r1
	jcc	nz, .LBB2_6+0
	jmp	.LBB2_5+0
.LBB2_5:
	ofp	r0
	ldi	r1, 0x00000002
	ist	r1, r0
	jmp	.LBB2_10+0
.LBB2_6:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007f
	cmp	r0, r1
	jcc	z, .LBB2_8+0
	jmp	.LBB2_7+0
.LBB2_7:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007e
	cmp	r0, r1
	jcc	nz, .LBB2_9+0
	jmp	.LBB2_8+0
.LBB2_8:
	ofp	r0
	ldi	r1, 0x00000003
	ist	r1, r0
	jmp	.LBB2_10+0
.LBB2_9:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB2_10+0
.LBB2_10:
	ofp	r0
	ild	r0, r0
	fts
	popfp
	ret
.Lfunc_end2:
	.size	priority, .Lfunc_end2-priority
                                        ; -- End function
	.p2align	2                               ; -- Begin function should_pop_operator
	.type	should_pop_operator,@function
should_pop_operator:                    ; @should_pop_operator
; %bb.0:
	pushfp
	stf
	osp	r14
	ldi	r15, 0x00000005
	add	r14, r15, r14
	lsp	r14
	ofp	r10
	ldi	r15, 0x00000001
	add	r10, r15, r10
	ist	r0, r10
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ist	r1, r0
	ofp	r0
	ldi	r15, 0x00000003
	add	r0, r15, r0
	ist	r2, r0
	ofp	r0
	ldi	r15, 0x00000004
	add	r0, r15, r0
	ist	r3, r0
	ild	r0, r10
	ldi	r1, 0x00000000
	cmp	r0, r1
	jcc	gt, .LBB3_2+0
	jmp	.LBB3_1+0
.LBB3_1:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB3_13+0
.LBB3_2:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x00000028
	cmp	r0, r1
	jcc	nz, .LBB3_4+0
	jmp	.LBB3_3+0
.LBB3_3:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB3_13+0
.LBB3_4:
	ofp	r0
	ldi	r15, 0x00000003
	add	r0, r15, r0
	ild	r0, r0
	ofp	r1
	ldi	r15, 0x00000004
	add	r1, r15, r1
	ild	r1, r1
	cmp	r0, r1
	jcc	gt, .LBB3_8+0
	jmp	.LBB3_5+0
.LBB3_5:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007f
	cmp	r0, r1
	jcc	z, .LBB3_8+0
	jmp	.LBB3_6+0
.LBB3_6:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007e
	cmp	r0, r1
	jcc	z, .LBB3_8+0
	jmp	.LBB3_7+0
.LBB3_7:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB3_13+0
.LBB3_8:
	ofp	r0
	ldi	r15, 0x00000003
	add	r0, r15, r0
	ild	r0, r0
	ofp	r1
	ldi	r15, 0x00000004
	add	r1, r15, r1
	ild	r1, r1
	cmp	r0, r1
	jcc	ge, .LBB3_12+0
	jmp	.LBB3_9+0
.LBB3_9:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007f
	cmp	r0, r1
	jcc	z, .LBB3_11+0
	jmp	.LBB3_10+0
.LBB3_10:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007e
	cmp	r0, r1
	jcc	nz, .LBB3_12+0
	jmp	.LBB3_11+0
.LBB3_11:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB3_13+0
.LBB3_12:
	ofp	r0
	ldi	r1, 0x00000001
	ist	r1, r0
	jmp	.LBB3_13+0
.LBB3_13:
	ofp	r0
	ild	r0, r0
	fts
	popfp
	ret
.Lfunc_end3:
	.size	should_pop_operator, .Lfunc_end3-should_pop_operator
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
	ldi	r6, 0x01fffedd
	ild	r0, r6
	ldi	r5, 0x00000000
	cmp	r0, r5
	jcc	nz, .LBB4_24+0
; %bb.1:
	ofp	r4
	mov	r4, r0
	call	__aurora_get_char
	ild	r0, r4
	ldi	r1, 0x000000ff
	and	r0, r1, r0
	ldi	r1, 0x00000029
	cmp	r0, r1
	jcc	le, .LBB4_10+0
; %bb.2:
	ldi	r2, 0x00000050
	cmp	r0, r2
	jcc	gt, .LBB4_17+0
; %bb.3:
	ldi	r2, 0x0000002a
	cmp	r0, r2
	jcc	z, .LBB4_5+0
; %bb.4:
	ldi	r2, 0x0000002f
	cmp	r0, r2
	jcc	nz, .LBB4_26+0
.LBB4_5:
	ldi	r2, 0x01fffadb
	ild	r3, r2
	ild	r4, r2
	ldi	r6, 0xffffffff
	cmp	r3, r6
	jcc	le, .LBB4_21+0
; %bb.6:
	ldi	r3, 0x00000001
	sft	r4, r3, r6
	ldi	r4, 0x01fff6db
	add	r6, r4, r6
	ild	r6, r6
	ldi	r7, 0x00000002
	cmp	r6, r7
	ldi	r6, 0x01fff6dc
	jcc	nz, .LBB4_8+0
; %bb.7:
	ild	r8, r2
	sft	r8, r3, r8
	add	r8, r6, r8
	ild	r8, r8
	cmp	r8, r1
	jcc	z, .LBB4_9+0
.LBB4_8:
	ild	r1, r2
	add	r1, r3, r1
	ist	r1, r2
	ild	r1, r2
	sft	r1, r3, r1
	add	r1, r4, r1
	ist	r3, r1
	ldi	r1, 0x01fffee1
	ild	r1, r1
	ild	r8, r2
	sft	r8, r3, r8
	add	r8, r6, r8
	ist	r1, r8
.LBB4_9:
	ild	r1, r2
	add	r1, r3, r1
	ist	r1, r2
	ild	r1, r2
	sft	r1, r3, r1
	add	r1, r4, r1
	ist	r7, r1
	ild	r1, r2
	sft	r1, r3, r1
	add	r1, r6, r1
	ist	r0, r1
	ldi	r1, 0x01fffee1
	ist	r5, r1
	jmp	.LBB4_22+0
.LBB4_10:
	ldi	r2, 0x0000000a
	cmp	r0, r2
	jcc	z, .LBB4_14+0
; %bb.11:
	ldi	r2, 0x0000000d
	cmp	r0, r2
	jcc	z, .LBB4_14+0
; %bb.12:
	ldi	r2, 0x00000020
	cmp	r0, r2
	jcc	nz, .LBB4_26+0
; %bb.13:
	ldi	r0, 0x00000020
	jmp	.LBB4_23+0
.LBB4_14:
	ldi	r0, 0x01fffede
	ild	r0, r0
	cmp	r0, r5
	jcc	z, .LBB4_16+0
; %bb.15:
	ldi	r0, 0x01fffadb
	ild	r1, r0
	ldi	r2, 0x00000001
	add	r1, r2, r1
	ist	r1, r0
	ild	r1, r0
	sft	r1, r2, r1
	ldi	r3, 0x01fff6db
	add	r1, r3, r1
	ist	r2, r1
	ldi	r1, 0x01fffee1
	ild	r1, r1
	ild	r0, r0
	sft	r0, r2, r0
	ldi	r2, 0x01fff6dc
	add	r0, r2, r0
	ist	r1, r0
.LBB4_16:
	ldi	r0, 0x00000001
	ist	r0, r6
	ldi	r0, 0x01ffffb6
	call	__aurora_put_string
	jmp	.LBB4_24+0
.LBB4_17:
	ldi	r2, 0x0000007e
	cmp	r0, r2
	jcc	nz, .LBB4_25+0
; %bb.18:
	ldi	r0, 0x01fffedf
	ild	r0, r0
	cmp	r0, r5
	jcc	nz, .LBB4_20+0
; %bb.19:
	ldi	r0, 0x01ffff03
	call	__aurora_put_string
.LBB4_20:
	ldi	r0, 0x01fffedc
	ild	r0, r0
	ldi	r1, 0x01fffee1
	ist	r0, r1
	ldi	r0, 0x01fffede
	ldi	r1, 0x00000001
	ist	r1, r0
	ldi	r0, 0x0000007e
	jmp	.LBB4_23+0
.LBB4_21:
	ldi	r1, 0x00000001
	add	r4, r1, r3
	ist	r3, r2
	ild	r3, r2
	sft	r3, r1, r3
	ldi	r4, 0x01fff6db
	add	r3, r4, r3
	ist	r1, r3
	ldi	r3, 0x01fffee1
	ild	r6, r3
	ild	r7, r2
	sft	r7, r1, r7
	ldi	r8, 0x01fff6dc
	add	r7, r8, r7
	ist	r6, r7
	ild	r6, r2
	add	r6, r1, r6
	ist	r6, r2
	ild	r6, r2
	sft	r6, r1, r6
	add	r6, r4, r4
	ldi	r6, 0x00000002
	ist	r6, r4
	ild	r2, r2
	sft	r2, r1, r1
	add	r1, r8, r1
	ist	r0, r1
	ist	r5, r3
.LBB4_22:
	ldi	r1, 0x01fffede
	ist	r5, r1
.LBB4_23:
	call	__aurora_put_char
.LBB4_24:
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
.LBB4_25:
	ldi	r2, 0x00000051
	cmp	r0, r2
	jcc	z, .LBB4_51+0
.LBB4_26:
	ldi	r2, 0x0000002a
	cmp	r0, r2
	jcc	gt, .LBB4_33+0
; %bb.27:
	ldi	r2, 0x00000028
	cmp	r0, r2
	jcc	z, .LBB4_39+0
; %bb.28:
	cmp	r0, r1
	jcc	nz, .LBB4_37+0
; %bb.29:
	ldi	r0, 0x01fffadb
	ild	r3, r0
	ldi	r2, 0x00000001
	sft	r3, r2, r4
	ldi	r3, 0x01fff6db
	add	r4, r3, r4
	ild	r6, r4
	ldi	r4, 0x00000002
	cmp	r6, r4
	jcc	nz, .LBB4_31+0
; %bb.30:
	ild	r6, r0
	sft	r6, r2, r6
	ldi	r7, 0x01fff6dc
	add	r6, r7, r6
	ild	r6, r6
	cmp	r6, r1
	jcc	z, .LBB4_32+0
.LBB4_31:
	ild	r6, r0
	add	r6, r2, r6
	ist	r6, r0
	ild	r6, r0
	sft	r6, r2, r6
	add	r6, r3, r6
	ist	r2, r6
	ldi	r6, 0x01fffee1
	ild	r6, r6
	ild	r7, r0
	sft	r7, r2, r7
	ldi	r8, 0x01fff6dc
	add	r7, r8, r7
	ist	r6, r7
.LBB4_32:
	ild	r6, r0
	add	r6, r2, r6
	ist	r6, r0
	ild	r6, r0
	sft	r6, r2, r6
	add	r6, r3, r3
	ist	r4, r3
	ild	r0, r0
	sft	r0, r2, r0
	ldi	r2, 0x01fff6dc
	add	r0, r2, r0
	ist	r1, r0
	ldi	r0, 0x01fffee1
	ist	r5, r0
	ldi	r0, 0x01fffede
	ist	r5, r0
	mov	r1, r0
	jmp	.LBB4_23+0
.LBB4_33:
	ldi	r4, 0x0000002d
	cmp	r0, r4
	jcc	z, .LBB4_35+0
; %bb.34:
	ldi	r2, 0x0000002b
	cmp	r0, r2
	jcc	nz, .LBB4_37+0
.LBB4_35:
	ldi	r3, 0x01fffede
	ild	r7, r3
	ldi	r2, 0x01fffadb
	ild	r6, r2
	cmp	r7, r5
	jcc	z, .LBB4_40+0
; %bb.36:
	ldi	r1, 0x00000001
	add	r6, r1, r4
	ist	r4, r2
	ild	r4, r2
	sft	r4, r1, r4
	ldi	r6, 0x01fff6db
	add	r4, r6, r4
	ist	r1, r4
	ldi	r4, 0x01fffee1
	ild	r7, r4
	ild	r8, r2
	sft	r8, r1, r8
	ldi	r9, 0x01fff6dc
	add	r8, r9, r8
	ist	r7, r8
	ild	r7, r2
	add	r7, r1, r7
	ist	r7, r2
	ild	r7, r2
	sft	r7, r1, r7
	add	r7, r6, r6
	ldi	r7, 0x00000002
	ist	r7, r6
	ild	r2, r2
	sft	r2, r1, r1
	add	r1, r9, r1
	ist	r0, r1
	ist	r5, r4
	ist	r5, r3
	jmp	.LBB4_23+0
.LBB4_37:
	ldi	r1, 0x00000030
	sub	r0, r1, r1
	ldi	r2, 0x00000009
	cmp	r1, r2
	jcc	b, .LBB4_24+0
; %bb.38:
	ldi	r2, 0x01fffee1
	ild	r3, r2
	ldi	r4, 0x0000000a
	mul	r3, r4, r3
	add	r3, r1, r1
	ist	r1, r2
	ldi	r1, 0x01fffede
	ldi	r2, 0x00000001
	jmp	.LBB4_50+0
.LBB4_39:
	ldi	r0, 0x01fffadb
	ild	r1, r0
	ldi	r3, 0x00000001
	add	r1, r3, r1
	ist	r1, r0
	ild	r1, r0
	sft	r1, r3, r1
	ldi	r4, 0x01fff6db
	add	r1, r4, r1
	ldi	r4, 0x00000002
	ist	r4, r1
	ild	r0, r0
	sft	r0, r3, r0
	ldi	r1, 0x01fff6dc
	add	r0, r1, r0
	ist	r2, r0
	ldi	r0, 0x01fffee1
	ist	r5, r0
	ldi	r0, 0x01fffede
	ist	r5, r0
	mov	r2, r0
	jmp	.LBB4_23+0
.LBB4_40:
	ild	r7, r2
	ldi	r8, 0xffffffff
	cmp	r6, r8
	jcc	le, .LBB4_46+0
; %bb.41:
	ldi	r6, 0x00000001
	sft	r7, r6, r8
	ldi	r7, 0x01fff6dc
	add	r8, r7, r8
	ild	r8, r8
	cmp	r8, r1
	jcc	nz, .LBB4_44+0
; %bb.42:
	ild	r1, r2
	sft	r1, r6, r8
	ldi	r1, 0x01fff6db
	add	r8, r1, r8
	ild	r9, r8
	ldi	r8, 0x00000002
	cmp	r9, r8
	jcc	nz, .LBB4_44+0
; %bb.43:
	ild	r4, r2
	add	r4, r6, r4
	ist	r4, r2
	ild	r4, r2
	sft	r4, r6, r4
	add	r4, r1, r1
	ist	r8, r1
	ild	r1, r2
	sft	r1, r6, r1
	add	r1, r7, r1
	ist	r0, r1
	ldi	r1, 0x01fffee1
	ist	r5, r1
	ist	r5, r3
	jmp	.LBB4_23+0
.LBB4_44:
	ild	r1, r2
	add	r1, r6, r1
	ist	r1, r2
	ild	r1, r2
	sft	r1, r6, r1
	ldi	r3, 0x01fff6db
	add	r1, r3, r1
	ldi	r3, 0x00000002
	ist	r3, r1
	ild	r1, r2
	sft	r1, r6, r1
	add	r1, r7, r1
	cmp	r0, r4
	jcc	nz, .LBB4_49+0
.LBB4_47:
	ldi	r2, 0x0000007f
	jmp	.LBB4_50+0
.LBB4_46:
	ldi	r1, 0x00000001
	add	r7, r1, r3
	ist	r3, r2
	ild	r3, r2
	sft	r3, r1, r3
	ldi	r5, 0x01fff6db
	add	r3, r5, r3
	ldi	r5, 0x00000002
	ist	r5, r3
	ild	r2, r2
	sft	r2, r1, r1
	ldi	r2, 0x01fff6dc
	add	r1, r2, r1
	cmp	r0, r4
	jcc	z, .LBB4_47+0
.LBB4_49:
	ldi	r2, 0x0000007e
.LBB4_50:
	ist	r2, r1
	jmp	.LBB4_23+0
.LBB4_51:
	ldi	r0, 0x01ffff2b
	call	__aurora_put_string
	call	__aurora_halt
.Lfunc_end4:
	.size	handle_input, .Lfunc_end4-handle_input
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_enable_interrupt
	.type	__aurora_enable_interrupt,@function
__aurora_enable_interrupt:              ; @__aurora_enable_interrupt
; %bb.0:
	;APP
	ei

	;NO_APP
	ret
.Lfunc_end5:
	.size	__aurora_enable_interrupt, .Lfunc_end5-__aurora_enable_interrupt
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_disable_interrupt
	.type	__aurora_disable_interrupt,@function
__aurora_disable_interrupt:             ; @__aurora_disable_interrupt
; %bb.0:
	;APP
	di

	;NO_APP
	ret
.Lfunc_end6:
	.size	__aurora_disable_interrupt, .Lfunc_end6-__aurora_disable_interrupt
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_set_up_stack_pointer
	.type	__aurora_set_up_stack_pointer,@function
__aurora_set_up_stack_pointer:          ; @__aurora_set_up_stack_pointer
; %bb.0:
	ret
.Lfunc_end7:
	.size	__aurora_set_up_stack_pointer, .Lfunc_end7-__aurora_set_up_stack_pointer
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_set_stack_pointer
	.type	__aurora_set_stack_pointer,@function
__aurora_set_stack_pointer:             ; @__aurora_set_stack_pointer
; %bb.0:
	;APP
	lsp	r0

	;NO_APP
	ret
.Lfunc_end8:
	.size	__aurora_set_stack_pointer, .Lfunc_end8-__aurora_set_stack_pointer
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_get_stack_pointer
	.type	__aurora_get_stack_pointer,@function
__aurora_get_stack_pointer:             ; @__aurora_get_stack_pointer
; %bb.0:
	;APP
	osp	r0

	;NO_APP
	ret
.Lfunc_end9:
	.size	__aurora_get_stack_pointer, .Lfunc_end9-__aurora_get_stack_pointer
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_halt
	.type	__aurora_halt,@function
__aurora_halt:                          ; @__aurora_halt
; %bb.0:
	;APP
	halt

	;NO_APP
.Lfunc_end10:
	.size	__aurora_halt, .Lfunc_end10-__aurora_halt
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_nop
	.type	__aurora_nop,@function
__aurora_nop:                           ; @__aurora_nop
; %bb.0:
	;APP
	nop

	;NO_APP
	ret
.Lfunc_end11:
	.size	__aurora_nop, .Lfunc_end11-__aurora_nop
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_read
	.type	__aurora_read,@function
__aurora_read:                          ; @__aurora_read
; %bb.0:
	ild	r0, r0
	ret
.Lfunc_end12:
	.size	__aurora_read, .Lfunc_end12-__aurora_read
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_write
	.type	__aurora_write,@function
__aurora_write:                         ; @__aurora_write
; %bb.0:
	ist	r1, r0
	ret
.Lfunc_end13:
	.size	__aurora_write, .Lfunc_end13-__aurora_write
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_register_interrupt
	.type	__aurora_register_interrupt,@function
__aurora_register_interrupt:            ; @__aurora_register_interrupt
; %bb.0:
	ist	r1, r0
	ret
.Lfunc_end14:
	.size	__aurora_register_interrupt, .Lfunc_end14-__aurora_register_interrupt
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_push
	.type	__aurora_push,@function
__aurora_push:                          ; @__aurora_push
; %bb.0:
	;APP
	push	r0

	;NO_APP
	ret
.Lfunc_end15:
	.size	__aurora_push, .Lfunc_end15-__aurora_push
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_pop
	.type	__aurora_pop,@function
__aurora_pop:                           ; @__aurora_pop
; %bb.0:
	;APP
	pop	r0

	;NO_APP
	ret
.Lfunc_end16:
	.size	__aurora_pop, .Lfunc_end16-__aurora_pop
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_get_char
	.type	__aurora_get_char,@function
__aurora_get_char:                      ; @__aurora_get_char
; %bb.0:
	ldi	r0, 0x04000000
	ild	r0, r0
	ret
.Lfunc_end17:
	.size	__aurora_get_char, .Lfunc_end17-__aurora_get_char
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_put_char
	.type	__aurora_put_char,@function
__aurora_put_char:                      ; @__aurora_put_char
; %bb.0:
	ldi	r1, 0x05000000
	ist	r0, r1
	ret
.Lfunc_end18:
	.size	__aurora_put_char, .Lfunc_end18-__aurora_put_char
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_put_int
	.type	__aurora_put_int,@function
__aurora_put_int:                       ; @__aurora_put_int
; %bb.0:
	ldi	r1, 0xffffffff
	cmp	r0, r1
	jcc	le, .LBB19_1+0
; %bb.7:
	ldi	r1, 0x00000000
	cmp	r0, r1
	jcc	nz, .LBB19_2+0
; %bb.8:
	ldi	r0, 0x05000000
	ldi	r1, 0x00000030
	ist	r1, r0
	ret
.LBB19_1:
	ldi	r1, 0x05000000
	ldi	r2, 0x0000002d
	ist	r2, r1
	ldi	r1, 0x00000000
	sub	r1, r0, r0
.LBB19_2:
	;APP
	osp	r1

	;NO_APP
	ldi	r2, 0x0000000a
	ldi	r3, 0x00000030
.LBB19_3:                               ; =>This Inner Loop Header: Depth=1
	rem	r0, r2, r10
	add	r10, r3, r10
	;APP
	push	r10

	;NO_APP
	div	r0, r2, r10
	cmp	r0, r2
	mov	r10, r0
	jcc	nc, .LBB19_3+0
; %bb.4:
	;APP
	osp	r0

	;NO_APP
	cmp	r0, r1
	jcc	nb, .LBB19_9+0
; %bb.5:
	ldi	r0, 0x05000000
.LBB19_6:                               ; =>This Inner Loop Header: Depth=1
	;APP
	pop	r2

	;NO_APP
	ist	r2, r0
	;APP
	osp	r2

	;NO_APP
	cmp	r2, r1
	jcc	b, .LBB19_6+0
.LBB19_9:
	ret
.Lfunc_end19:
	.size	__aurora_put_int, .Lfunc_end19-__aurora_put_int
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
	jcc	z, .LBB20_3+0
; %bb.1:
	ldi	r10, 0x00000001
	add	r0, r10, r11
	ldi	r0, 0x05000000
.LBB20_2:                               ; =>This Inner Loop Header: Depth=1
	ist	r3, r0
	add	r11, r10, r12
	ild	r3, r11
	and	r3, r1, r3
	cmp	r3, r2
	mov	r12, r11
	jcc	nz, .LBB20_2+0
.LBB20_3:
	ret
.Lfunc_end20:
	.size	__aurora_put_string, .Lfunc_end20-__aurora_put_string
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_zero_words
	.type	__aurora_zero_words,@function
__aurora_zero_words:                    ; @__aurora_zero_words
; %bb.0:
	jmp	.LBB21_1+0
.LBB21_1:                               ; =>This Inner Loop Header: Depth=1
	ldi	r2, 0x00000000
	ist	r2, r0
	ldi	r3, 0x00000001
	add	r0, r3, r0
	sub	r1, r3, r1
	cmp	r1, r2
	jcc	nz, .LBB21_1+0
	jmp	.LBB21_2+0
.LBB21_2:
	ret
.Lfunc_end21:
	.size	__aurora_zero_words, .Lfunc_end21-__aurora_zero_words
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
