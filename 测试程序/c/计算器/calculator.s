	.file	"calculator.c"
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
	mov	r1, r9
	ldi	r1, 0x01ffffd9
	ldi	r0, 0x0000006e
	ist	r0, r1
	mov	r0, r4
	ldi	r1, 0x01ffffda
	ldi	r0, 0x00000074
	ist	r0, r1
	mov	r0, r1
	ldi	r2, 0x01ffffdb
	ldi	r6, 0x00000065
	ist	r6, r2
	ldi	r10, 0x01ffffdc
	ldi	r12, 0x00000072
	ist	r12, r10
	ldi	r11, 0x01ffffdd
	ldi	r13, 0x00000020
	ist	r13, r11
	ldi	r11, 0x01ffffde
	ist	r6, r11
	ldi	r11, 0x01ffffdf
	ldi	r0, 0x00000078
	ofp	r14
	ldi	r15, 0x0000000f
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffe0
	ldi	r0, 0x00000070
	ist	r0, r11
	mov	r0, r8
	ldi	r11, 0x01ffffe1
	ist	r12, r11
	ldi	r11, 0x01ffffe2
	ist	r6, r11
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
	ldi	r0, 0x00000069
	ist	r0, r11
	mov	r0, r3
	ofp	r14
	ldi	r15, 0x00000007
	add	r14, r15, r14
	ist	r3, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01ffffe6
	ldi	r7, 0x0000006f
	ist	r7, r11
	ldi	r11, 0x01ffffe7
	ist	r4, r11
	ldi	r11, 0x01ffffe8
	ldi	r0, 0x0000002c
	ofp	r14
	ldi	r15, 0x00000015
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffe9
	ist	r13, r11
	ldi	r11, 0x01ffffea
	ist	r6, r11
	ldi	r11, 0x01ffffeb
	ist	r4, r11
	mov	r4, r2
	ldi	r11, 0x01ffffec
	ldi	r0, 0x00000064
	ofp	r14
	ldi	r15, 0x00000011
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffed
	ist	r13, r11
	ldi	r11, 0x01ffffee
	ldi	r0, 0x00000077
	ofp	r14
	ldi	r15, 0x0000000d
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffef
	ist	r3, r11
	ldi	r11, 0x01fffff0
	ist	r1, r11
	ldi	r11, 0x01fffff1
	ldi	r0, 0x00000068
	ofp	r14
	ldi	r15, 0x00000006
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01fffff2
	ist	r13, r11
	ldi	r11, 0x01fffff3
	ldi	r4, 0x0000003c
	ist	r4, r11
	ldi	r11, 0x01fffff4
	ist	r9, r11
	mov	r9, r3
	ofp	r14
	ldi	r15, 0x0000001c
	add	r14, r15, r14
	ist	r3, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01fffff5
	ist	r2, r11
	mov	r2, r10
	ldi	r11, 0x01fffff6
	ist	r1, r11
	mov	r1, r2
	ldi	r11, 0x01fffff7
	ist	r6, r11
	ldi	r11, 0x01fffff8
	ist	r12, r11
	ldi	r11, 0x01fffff9
	ldi	r4, 0x0000003e
	ist	r4, r11
	ldi	r11, 0x01fffffa
	ldi	r1, 0x0000003a
	ist	r1, r11
	ofp	r14
	ldi	r15, 0x0000000b
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01fffffb
	ldi	r5, 0x0000000a
	ist	r5, r11
	ldi	r11, 0x01fffffc
	ist	r4, r11
	ldi	r11, 0x01fffffd
	ist	r4, r11
	ldi	r11, 0x01fffffe
	ist	r13, r11
	ldi	r11, 0x01ffffff
	ldi	r9, 0x00000000
	ist	r9, r11
	ldi	r11, 0x01ffffb8
	ldi	r4, 0x00000049
	ist	r4, r11
	ldi	r11, 0x01ffffb9
	ist	r10, r11
	ldi	r11, 0x01ffffba
	ist	r8, r11
	ldi	r11, 0x01ffffbb
	ldi	r0, 0x00000075
	ist	r0, r11
	ofp	r14
	ldi	r15, 0x00000016
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01ffffbc
	ist	r2, r11
	ldi	r11, 0x01ffffbd
	ist	r3, r11
	ldi	r11, 0x01ffffbe
	ist	r12, r11
	ldi	r11, 0x01ffffbf
	ist	r12, r11
	ldi	r11, 0x01ffffc0
	mov	r7, r3
	ist	r3, r11
	ldi	r11, 0x01ffffc1
	ist	r12, r11
	ldi	r11, 0x01ffffc2
	ist	r1, r11
	ldi	r11, 0x01ffffc3
	ist	r13, r11
	ldi	r11, 0x01ffffc4
	ldi	r4, 0x0000004e
	ist	r4, r11
	ldi	r11, 0x01ffffc5
	ist	r3, r11
	ofp	r14
	ldi	r15, 0x00000008
	add	r14, r15, r14
	ist	r3, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01ffffc6
	ist	r13, r11
	ldi	r11, 0x01ffffc7
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r11
	ldi	r11, 0x01ffffc8
	mov	r10, r4
	ist	r4, r11
	ldi	r11, 0x01ffffc9
	ist	r8, r11
	mov	r8, r10
	ofp	r14
	ldi	r15, 0x00000012
	add	r14, r15, r14
	ist	r10, r14                        ; 4-byte Folded Spill
	ldi	r11, 0x01ffffca
	ist	r0, r11
	ldi	r11, 0x01ffffcb
	ist	r2, r11
	ldi	r11, 0x01ffffcc
	ist	r13, r11
	ldi	r11, 0x01ffffcd
	ist	r12, r11
	ldi	r11, 0x01ffffce
	ist	r6, r11
	ldi	r11, 0x01ffffcf
	ldi	r0, 0x00000063
	ofp	r14
	ldi	r15, 0x00000010
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffd0
	ist	r6, r11
	ldi	r11, 0x01ffffd1
	ist	r1, r11
	ldi	r11, 0x01ffffd2
	ldi	r0, 0x00000076
	ofp	r14
	ldi	r15, 0x0000001b
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffd3
	ist	r6, r11
	ldi	r11, 0x01ffffd4
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r8, r11
	ldi	r11, 0x01ffffd5
	ldi	r0, 0x0000002e
	ofp	r14
	ldi	r15, 0x00000009
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffd6
	ofp	r14
	ldi	r15, 0x0000001a
	add	r14, r15, r14
	ist	r5, r14                         ; 4-byte Folded Spill
	ist	r5, r11
	ldi	r11, 0x01ffffd7
	ist	r9, r11
	ldi	r11, 0x01ffffb6
	ist	r5, r11
	ldi	r11, 0x01ffffb7
	ist	r9, r11
	ldi	r11, 0x01ffff98
	ldi	r0, 0x00000053
	ofp	r14
	ldi	r15, 0x00000018
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
	ist	r4, r11
	ofp	r14
	ldi	r15, 0x00000014
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ldi	r11, 0x01ffff9b
	ist	r2, r11
	mov	r2, r1
	ldi	r11, 0x01ffff9c
	ldi	r0, 0x00000061
	ofp	r14
	ldi	r15, 0x0000000a
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffff9d
	ofp	r14
	ldi	r15, 0x0000000f
	ldo	r14, r15, r7                    ; 4-byte Folded Reload
	ist	r7, r11
	ldi	r11, 0x01ffff9e
	ofp	r14
	ldi	r15, 0x0000001c
	ldo	r14, r15, r5                    ; 4-byte Folded Reload
	ist	r5, r11
	ldi	r11, 0x01ffff9f
	ist	r12, r11
	ldi	r11, 0x01ffffa0
	ist	r12, r11
	ldi	r11, 0x01ffffa1
	ist	r3, r11
	ldi	r11, 0x01ffffa2
	ist	r12, r11
	ldi	r11, 0x01ffffa3
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r11
	ldi	r11, 0x01ffffa4
	ist	r13, r11
	ldi	r11, 0x01ffffa5
	ldi	r0, 0x00000055
	ofp	r14
	ldi	r15, 0x00000017
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r11
	ldi	r11, 0x01ffffa6
	ist	r4, r11
	ldi	r11, 0x01ffffa7
	ist	r6, r11
	ldi	r11, 0x01ffffa8
	ist	r7, r11
	ldi	r11, 0x01ffffa9
	ist	r10, r11
	ldi	r11, 0x01ffffaa
	ist	r6, r11
	ldi	r11, 0x01ffffab
	ofp	r14
	ldi	r15, 0x00000010
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r0, r11
	ldi	r11, 0x01ffffac
	mov	r1, r4
	ofp	r14
	ldi	r15, 0x0000001e
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ist	r4, r11
	ldi	r11, 0x01ffffad
	ist	r6, r11
	ldi	r11, 0x01ffffae
	ist	r8, r11
	ldi	r11, 0x01ffffaf
	ist	r13, r11
	ldi	r8, 0x01ffffb0
	ldi	r3, 0x00000027
	ofp	r14
	ldi	r15, 0x00000013
	add	r14, r15, r14
	ist	r3, r14                         ; 4-byte Folded Spill
	ist	r3, r8
	ldi	r8, 0x01ffffb1
	ldi	r0, 0x00000029
	ofp	r14
	ldi	r15, 0x0000001d
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r0, r8
	ldi	r8, 0x01ffffb2
	ist	r3, r8
	ldi	r8, 0x01ffffb3
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r8
	ldi	r8, 0x01ffffb4
	ofp	r14
	ldi	r15, 0x0000001a
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r8
	ldi	r8, 0x01ffffb5
	ist	r9, r8
	ldi	r0, 0x01ffff7a
	ldi	r10, 0x0000004d
	ofp	r14
	ldi	r15, 0x00000019
	add	r14, r15, r14
	ist	r10, r14                        ; 4-byte Folded Spill
	ist	r10, r0
	ldi	r0, 0x01ffff7b
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r10, r0
	ldi	r0, 0x01ffff7c
	ist	r4, r0
	ldi	r0, 0x01ffff7d
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ist	r11, r0
	ldi	r0, 0x01ffff7e
	mov	r5, r7
	ist	r7, r0
	ldi	r0, 0x01ffff7f
	ist	r12, r0
	ldi	r0, 0x01ffff80
	ist	r12, r0
	ldi	r0, 0x01ffff81
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r5                    ; 4-byte Folded Reload
	ist	r5, r0
	ldi	r0, 0x01ffff82
	ist	r12, r0
	ldi	r0, 0x01ffff83
	ist	r2, r0
	ldi	r0, 0x01ffff84
	ist	r13, r0
	ldi	r0, 0x01ffff85
	ldi	r8, 0x00000044
	ist	r8, r0
	ldi	r0, 0x01ffff86
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff87
	ofp	r14
	ldi	r15, 0x0000001b
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r10, r0
	ldi	r0, 0x01ffff88
	ist	r2, r0
	ldi	r0, 0x01ffff89
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	ist	r4, r0
	ldi	r0, 0x01ffff8a
	ist	r2, r0
	ldi	r0, 0x01ffff8b
	mov	r5, r2
	ist	r2, r0
	ldi	r0, 0x01ffff8c
	ofp	r14
	ldi	r15, 0x00000014
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r10, r0
	ldi	r0, 0x01ffff8d
	ist	r13, r0
	ldi	r0, 0x01ffff8e
	ldi	r4, 0x00000062
	ist	r4, r0
	ldi	r0, 0x01ffff8f
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r5                    ; 4-byte Folded Reload
	ist	r5, r0
	ldi	r0, 0x01ffff90
	ist	r13, r0
	ldi	r0, 0x01ffff91
	ldi	r4, 0x0000007a
	ist	r4, r0
	ldi	r0, 0x01ffff92
	ist	r6, r0
	ldi	r0, 0x01ffff93
	ist	r12, r0
	ldi	r0, 0x01ffff94
	ist	r2, r0
	ldi	r0, 0x01ffff95
	mov	r3, r4
	ist	r4, r0
	ldi	r0, 0x01ffff96
	ist	r1, r0
	mov	r1, r11
	ldi	r0, 0x01ffff97
	ist	r9, r0
	ldi	r0, 0x01ffff5e
	ofp	r14
	ldi	r15, 0x00000018
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff5f
	ist	r5, r0
	ldi	r0, 0x01ffff60
	ist	r10, r0
	ldi	r0, 0x01ffff61
	ofp	r14
	ldi	r15, 0x0000001e
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r10, r0
	ldi	r0, 0x01ffff62
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r8, r0
	ldi	r0, 0x01ffff63
	ofp	r14
	ldi	r15, 0x0000000f
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff64
	ist	r7, r0
	ldi	r0, 0x01ffff65
	ist	r12, r0
	ldi	r0, 0x01ffff66
	ist	r12, r0
	ldi	r0, 0x01ffff67
	ist	r2, r0
	ldi	r0, 0x01ffff68
	ist	r12, r0
	ldi	r0, 0x01ffff69
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r5                    ; 4-byte Folded Reload
	ist	r5, r0
	ldi	r0, 0x01ffff6a
	ist	r13, r0
	ldi	r0, 0x01ffff6b
	ist	r7, r0
	mov	r7, r1
	ldi	r0, 0x01ffff6c
	ist	r3, r0
	ldi	r0, 0x01ffff6d
	ofp	r14
	ldi	r15, 0x00000012
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff6e
	ist	r6, r0
	ldi	r0, 0x01ffff6f
	ofp	r14
	ldi	r15, 0x00000010
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff70
	ist	r10, r0
	mov	r10, r7
	ldi	r0, 0x01ffff71
	ist	r6, r0
	ldi	r0, 0x01ffff72
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff73
	ist	r13, r0
	ldi	r0, 0x01ffff74
	ofp	r14
	ldi	r15, 0x00000013
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r10, r0
	ldi	r0, 0x01ffff75
	ofp	r14
	ldi	r15, 0x0000001d
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff76
	ist	r10, r0
	ldi	r0, 0x01ffff77
	ist	r4, r0
	ldi	r0, 0x01ffff78
	ist	r11, r0
	ldi	r0, 0x01ffff79
	ist	r9, r0
	ldi	r0, 0x01ffff3f
	ofp	r14
	ldi	r15, 0x00000019
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff40
	ist	r8, r0
	ldi	r0, 0x01ffff41
	ist	r7, r0
	ldi	r0, 0x01ffff42
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff43
	ist	r1, r0
	ldi	r0, 0x01ffff44
	ist	r12, r0
	ldi	r0, 0x01ffff45
	ist	r12, r0
	ldi	r0, 0x01ffff46
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff47
	ist	r12, r0
	ldi	r0, 0x01ffff48
	ist	r5, r0
	ldi	r0, 0x01ffff49
	ist	r13, r0
	ldi	r0, 0x01ffff4a
	ofp	r14
	ldi	r15, 0x00000017
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff4b
	ofp	r14
	ldi	r15, 0x00000014
	ldo	r14, r15, r5                    ; 4-byte Folded Reload
	ist	r5, r0
	ldi	r0, 0x01ffff4c
	ldi	r4, 0x0000006b
	ist	r4, r0
	ldi	r0, 0x01ffff4d
	ist	r5, r0
	ldi	r0, 0x01ffff4e
	ist	r2, r0
	ldi	r0, 0x01ffff4f
	ofp	r14
	ldi	r15, 0x0000000d
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	ist	r4, r0
	ldi	r0, 0x01ffff50
	ist	r5, r0
	ldi	r0, 0x01ffff51
	ist	r13, r0
	ldi	r0, 0x01ffff52
	mov	r2, r1
	ist	r1, r0
	ldi	r0, 0x01ffff53
	ofp	r14
	ldi	r15, 0x00000012
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff54
	ist	r6, r0
	ldi	r0, 0x01ffff55
	ist	r12, r0
	ldi	r0, 0x01ffff56
	ist	r8, r0
	ldi	r0, 0x01ffff57
	ist	r7, r0
	ldi	r0, 0x01ffff58
	ist	r1, r0
	ldi	r0, 0x01ffff59
	ist	r12, r0
	ldi	r0, 0x01ffff5a
	ist	r13, r0
	ldi	r0, 0x01ffff5b
	ldi	r4, 0x00000028
	ofp	r14
	ldi	r15, 0x00000017
	add	r14, r15, r14
	ist	r4, r14                         ; 4-byte Folded Spill
	ist	r4, r0
	ldi	r0, 0x01ffff5c
	ist	r10, r0
	ldi	r0, 0x01ffff5d
	ist	r9, r0
	ldi	r0, 0x01ffff3a
	ist	r10, r0
	ldi	r0, 0x01ffff3b
	ofp	r14
	ldi	r15, 0x0000001d
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff3c
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff3d
	mov	r11, r10
	ist	r10, r0
	ldi	r0, 0x01ffff3e
	ist	r9, r0
	ldi	r0, 0x01ffff31
	ldi	r4, 0x00000052
	ist	r4, r0
	ldi	r0, 0x01ffff32
	ist	r6, r0
	ldi	r0, 0x01ffff33
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ist	r11, r0
	ldi	r0, 0x01ffff34
	ofp	r14
	ldi	r15, 0x00000016
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ldi	r0, 0x01ffff35
	ldi	r4, 0x0000006c
	ist	r4, r0
	ldi	r0, 0x01ffff36
	ist	r7, r0
	ldi	r0, 0x01ffff37
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ist	r8, r0
	ldi	r0, 0x01ffff38
	ist	r13, r0
	ldi	r0, 0x01ffff39
	ist	r9, r0
	ldi	r0, 0x01ffff2b
	ldi	r4, 0x00000051
	ist	r4, r0
	ldi	r0, 0x01ffff2c
	ist	r1, r0
	ldi	r0, 0x01ffff2d
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff2e
	ist	r7, r0
	ldi	r0, 0x01ffff2f
	ist	r3, r0
	ldi	r0, 0x01ffff30
	ist	r9, r0
	ldi	r0, 0x01ffff03
	ist	r10, r0
	mov	r10, r1
	ldi	r0, 0x01ffff04
	ldi	r4, 0x00000057
	ist	r4, r0
	ldi	r0, 0x01ffff05
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff06
	ist	r12, r0
	ldi	r0, 0x01ffff07
	ist	r5, r0
	ldi	r0, 0x01ffff08
	ist	r2, r0
	ldi	r0, 0x01ffff09
	ist	r5, r0
	ldi	r0, 0x01ffff0a
	ldi	r4, 0x00000067
	ist	r4, r0
	ldi	r0, 0x01ffff0b
	ist	r8, r0
	ldi	r0, 0x01ffff0c
	ist	r13, r0
	ldi	r0, 0x01ffff0d
	ldi	r4, 0x00000054
	ist	r4, r0
	ldi	r0, 0x01ffff0e
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r4                    ; 4-byte Folded Reload
	ist	r4, r0
	ldi	r0, 0x01ffff0f
	ist	r6, r0
	ldi	r0, 0x01ffff10
	ist	r12, r0
	ldi	r0, 0x01ffff11
	ist	r6, r0
	ldi	r0, 0x01ffff12
	ofp	r14
	ldi	r15, 0x00000013
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r10, r0
	ldi	r0, 0x01ffff13
	ist	r12, r0
	ldi	r0, 0x01ffff14
	ist	r6, r0
	ldi	r0, 0x01ffff15
	ist	r13, r0
	ldi	r0, 0x01ffff16
	ist	r5, r0
	ldi	r0, 0x01ffff17
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r10, r0
	ldi	r0, 0x01ffff18
	ist	r13, r0
	ldi	r0, 0x01ffff19
	ist	r4, r0
	ldi	r0, 0x01ffff1a
	ist	r2, r0
	ldi	r0, 0x01ffff1b
	ist	r11, r0
	ldi	r0, 0x01ffff1c
	ist	r7, r0
	ldi	r0, 0x01ffff1d
	ist	r10, r0
	ldi	r0, 0x01ffff1e
	ist	r12, r0
	ldi	r0, 0x01ffff1f
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff20
	ist	r13, r0
	ldi	r0, 0x01ffff21
	ist	r3, r0
	ldi	r0, 0x01ffff22
	ist	r5, r0
	ldi	r0, 0x01ffff23
	ist	r11, r0
	ldi	r0, 0x01ffff24
	ofp	r14
	ldi	r15, 0x0000000d
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	ist	r3, r0
	ldi	r0, 0x01ffff25
	ist	r6, r0
	ldi	r0, 0x01ffff26
	ist	r12, r0
	ldi	r0, 0x01ffff27
	ist	r11, r0
	ldi	r0, 0x01ffff28
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r2, r0
	ldi	r0, 0x01ffff29
	ist	r1, r0
	ldi	r0, 0x01ffff2a
	ist	r9, r0
	ldi	r0, 0x01ffff02
	ofp	r14
	ldi	r15, 0x00000018
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r9, r0
	ldi	r0, 0x01ffff01
	ofp	r14
	ldi	r15, 0x0000000e
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r9, r0
	ldi	r0, 0x01ffff00
	ofp	r14
	ldi	r15, 0x0000001b
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r9, r0
	ldi	r8, 0x01fffeff
	ist	r9, r8
	ldi	r4, 0x01fffefe
	ist	r9, r4
	ldi	r0, 0x01fffefd
	ofp	r14
	ldi	r15, 0x0000001c
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ist	r9, r0
	ldi	r0, 0x01fffafd
	ofp	r14
	ldi	r15, 0x0000000a
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r7, 0x00000400
	mov	r7, r1
	call	__aurora_zero_words
	ldi	r5, 0x01fffafc
	ist	r9, r5
	ldi	r6, 0x01fff6fc
	mov	r6, r0
	mov	r7, r1
	ofp	r14
	ldi	r15, 0x00000018
	ldo	r14, r15, r7                    ; 4-byte Folded Reload
	call	__aurora_zero_words
	mov	r6, r11
	mov	r5, r2
                                        ; implicit-def: $r0
                                        ; kill: killed $r0
	ofp	r14
	ldi	r15, 0x00000019
	add	r14, r15, r14
	ist	r8, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000006
	add	r14, r15, r14
	ist	r2, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x0000000b
	add	r14, r15, r14
	ist	r11, r14                        ; 4-byte Folded Spill
.LBB0_1:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_2 Depth 2
                                        ;       Child Loop BB0_3 Depth 3
                                        ;     Child Loop BB0_8 Depth 2
                                        ;       Child Loop BB0_31 Depth 3
                                        ;       Child Loop BB0_23 Depth 3
                                        ;     Child Loop BB0_39 Depth 2
                                        ;     Child Loop BB0_44 Depth 2
                                        ;     Child Loop BB0_49 Depth 2
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
.LBB0_2:                                ;   Parent Loop BB0_1 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB0_3 Depth 3
	;APP
	di

	;NO_APP
	ldi	r5, 0xffffffff
	ist	r5, r2
	ist	r9, r4
	ist	r9, r8
	ist	r9, r7
	ist	r9, r6
	ldi	r0, 0x01ffffd8
	call	__aurora_put_string
	;APP
	ei

	;NO_APP
.LBB0_3:                                ;   Parent Loop BB0_1 Depth=1
                                        ;     Parent Loop BB0_2 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ild	r0, r4
	cmp	r0, r9
	jcc	z, .LBB0_3
; %bb.4:                                ;   in Loop: Header=BB0_2 Depth=2
	;APP
	di

	;NO_APP
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ild	r0, r1
	cmp	r0, r5
	ldi	r5, 0x00000001
	jcc	gt, .LBB0_6
; %bb.5:                                ;   in Loop: Header=BB0_2 Depth=2
	ldi	r0, 0x01ffffb8
	call	__aurora_put_string
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ist	r5, r6
	jmp	.LBB0_2
.LBB0_6:                                ;   in Loop: Header=BB0_1 Depth=1
	;APP
	osp	r8

	;NO_APP
	ild	r0, r1
	cmp	r0, r9
	ldi	r12, 0x00000002
	ldi	r0, 0x0000002d
	ofp	r14
	ldi	r15, 0x00000012
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x0000002f
	ofp	r14
	ldi	r15, 0x00000016
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x0000002a
	ofp	r14
	ldi	r15, 0x00000014
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ldi	r0, 0x0000002b
	ofp	r14
	ldi	r15, 0x00000011
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	mov	r9, r7
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	add	r14, r15, r14
	ist	r12, r14                        ; 4-byte Folded Spill
	jcc	lt, .LBB0_37
; %bb.7:                                ;   in Loop: Header=BB0_1 Depth=1
	ldi	r0, 0x00000000
	mov	r0, r7
	ofp	r14
	ldi	r15, 0x00000010
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	ofp	r14
	ldi	r15, 0x00000007
	add	r14, r15, r14
	ist	r8, r14                         ; 4-byte Folded Spill
	jmp	.LBB0_8
.LBB0_10:                               ;   in Loop: Header=BB0_8 Depth=2
	sft	r7, r5, r1
	add	r1, r10, r2
	ist	r5, r2
	ldi	r2, 0x01fffafe
	add	r1, r2, r1
	ldi	r2, 0x01fff6fd
	add	r0, r2, r0
	ild	r0, r0
	ist	r0, r1
	add	r7, r5, r7
.LBB0_36:                               ;   in Loop: Header=BB0_8 Depth=2
	ofp	r14
	ldi	r15, 0x00000008
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	add	r2, r5, r0
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ild	r1, r1
	cmp	r2, r1
	jcc	ge, .LBB0_37
.LBB0_8:                                ;   Parent Loop BB0_1 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB0_31 Depth 3
                                        ;       Child Loop BB0_23 Depth 3
	mov	r0, r2
	ild	r0, r6
	ofp	r14
	ldi	r15, 0x00000010
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	cmp	r0, r1
	jcc	nz, .LBB0_37
; %bb.9:                                ;   in Loop: Header=BB0_8 Depth=2
	ofp	r14
	ldi	r15, 0x00000008
	add	r14, r15, r14
	ist	r2, r14                         ; 4-byte Folded Spill
	sft	r2, r5, r0
	add	r0, r11, r1
	ild	r2, r1
	cmp	r2, r5
	jcc	z, .LBB0_10
; %bb.11:                               ;   in Loop: Header=BB0_8 Depth=2
	ild	r1, r1
	cmp	r1, r12
	jcc	nz, .LBB0_36
; %bb.12:                               ;   in Loop: Header=BB0_8 Depth=2
	ldi	r1, 0x01fff6fd
	add	r0, r1, r0
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB0_18
; %bb.13:                               ;   in Loop: Header=BB0_8 Depth=2
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x00000012
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB0_18
; %bb.14:                               ;   in Loop: Header=BB0_8 Depth=2
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x00000014
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB0_18
; %bb.15:                               ;   in Loop: Header=BB0_8 Depth=2
	ild	r1, r0
	ofp	r14
	ldi	r15, 0x00000016
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	z, .LBB0_18
; %bb.16:                               ;   in Loop: Header=BB0_8 Depth=2
	ild	r1, r0
	ldi	r2, 0x0000007e
	cmp	r1, r2
	jcc	z, .LBB0_18
; %bb.17:                               ;   in Loop: Header=BB0_8 Depth=2
	ild	r1, r0
	ldi	r2, 0x0000007f
	cmp	r1, r2
	jcc	nz, .LBB0_26
.LBB0_18:                               ;   in Loop: Header=BB0_8 Depth=2
	ild	r0, r0
	ofp	r14
	ldi	r15, 0x00000009
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	call	priority
	mov	r0, r1
	;APP
	osp	r0

	;NO_APP
	cmp	r0, r8
	jcc	nz, .LBB0_20
; %bb.19:                               ;   in Loop: Header=BB0_8 Depth=2
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	;APP
	push	r0

	;NO_APP
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r12                   ; 4-byte Folded Reload
	jmp	.LBB0_36
.LBB0_20:                               ;   in Loop: Header=BB0_8 Depth=2
	mov	r8, r6
	;APP
	pop	r8

	;NO_APP
	;APP
	push	r8

	;NO_APP
	mov	r8, r0
	ofp	r14
	ldi	r15, 0x0000000d
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	call	priority
	mov	r0, r2
	;APP
	osp	r0

	;NO_APP
	sub	r0, r6, r0
	mov	r8, r1
	ofp	r14
	ldi	r15, 0x0000000d
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	call	should_pop_operator
	ldi	r1, 0x00000000
	ofp	r14
	ldi	r15, 0x0000000f
	add	r14, r15, r14
	ist	r1, r14                         ; 4-byte Folded Spill
	cmp	r0, r1
	jcc	z, .LBB0_21
; %bb.22:                               ;   in Loop: Header=BB0_8 Depth=2
	add	r7, r5, r1
	sft	r7, r5, r0
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	add	r0, r10, r6
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r12                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
.LBB0_23:                               ;   Parent Loop BB0_1 Depth=1
                                        ;     Parent Loop BB0_8 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	mov	r1, r7
	ist	r12, r6
	add	r6, r5, r0
	;APP
	pop	r1

	;NO_APP
	ist	r1, r0
	;APP
	osp	r0

	;NO_APP
	cmp	r0, r8
	jcc	z, .LBB0_25
; %bb.24:                               ;   in Loop: Header=BB0_23 Depth=3
	;APP
	pop	r8

	;NO_APP
	;APP
	push	r8

	;NO_APP
	mov	r8, r0
	call	priority
	mov	r0, r2
	;APP
	osp	r0

	;NO_APP
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	sub	r0, r1, r0
	mov	r8, r1
	ofp	r14
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000d
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	call	should_pop_operator
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r12                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	add	r6, r12, r6
	add	r7, r5, r1
	ofp	r14
	ldi	r15, 0x0000000f
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	cmp	r0, r3
	jcc	nz, .LBB0_23
	jmp	.LBB0_25
.LBB0_21:                               ;   in Loop: Header=BB0_8 Depth=2
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r12                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000009
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
.LBB0_25:                               ;   in Loop: Header=BB0_8 Depth=2
	;APP
	push	r2

	;NO_APP
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r6                    ; 4-byte Folded Reload
	jmp	.LBB0_36
.LBB0_26:                               ;   in Loop: Header=BB0_8 Depth=2
	ild	r1, r0
	ild	r0, r0
	ofp	r14
	ldi	r15, 0x00000017
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r1, r2
	jcc	nz, .LBB0_28
; %bb.27:                               ;   in Loop: Header=BB0_8 Depth=2
	;APP
	push	r0

	;NO_APP
	jmp	.LBB0_36
.LBB0_28:                               ;   in Loop: Header=BB0_8 Depth=2
	ofp	r14
	ldi	r15, 0x0000001d
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	cmp	r0, r1
	jcc	nz, .LBB0_36
; %bb.29:                               ;   in Loop: Header=BB0_8 Depth=2
	;APP
	pop	r0

	;NO_APP
	ofp	r14
	ldi	r15, 0x00000017
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	cmp	r0, r1
	jcc	z, .LBB0_36
; %bb.30:                               ;   in Loop: Header=BB0_8 Depth=2
	sft	r7, r5, r1
	add	r1, r10, r1
.LBB0_31:                               ;   Parent Loop BB0_1 Depth=1
                                        ;     Parent Loop BB0_8 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	;APP
	osp	r2

	;NO_APP
	cmp	r2, r8
	jcc	nb, .LBB0_34
; %bb.32:                               ;   in Loop: Header=BB0_31 Depth=3
	;APP
	osp	r2

	;NO_APP
	cmp	r2, r8
	jcc	z, .LBB0_33
; %bb.35:                               ;   in Loop: Header=BB0_31 Depth=3
	ist	r12, r1
	add	r1, r5, r2
	ist	r0, r2
	add	r1, r12, r1
	add	r7, r5, r7
	;APP
	pop	r0

	;NO_APP
	ofp	r14
	ldi	r15, 0x00000017
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	cmp	r0, r2
	jcc	nz, .LBB0_31
	jmp	.LBB0_36
.LBB0_33:                               ;   in Loop: Header=BB0_1 Depth=1
	ldi	r0, 0x01ffff98
	call	__aurora_put_string
	ist	r5, r6
.LBB0_34:                               ;   in Loop: Header=BB0_1 Depth=1
	ldi	r0, 0x01ffff98
	call	__aurora_put_string
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r12                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ist	r5, r6
.LBB0_37:                               ;   in Loop: Header=BB0_1 Depth=1
	;APP
	osp	r0

	;NO_APP
	cmp	r0, r8
	jcc	nb, .LBB0_41
; %bb.38:                               ;   in Loop: Header=BB0_1 Depth=1
	sft	r7, r5, r0
	ldi	r1, 0x01fffafd
	add	r0, r1, r0
.LBB0_39:                               ;   Parent Loop BB0_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ild	r1, r6
	ldi	r2, 0x00000000
	cmp	r1, r2
	jcc	nz, .LBB0_41
; %bb.40:                               ;   in Loop: Header=BB0_39 Depth=2
	;APP
	pop	r1

	;NO_APP
	ist	r12, r0
	add	r0, r5, r2
	ist	r1, r2
	add	r0, r12, r0
	add	r7, r5, r7
	;APP
	osp	r1

	;NO_APP
	cmp	r1, r8
	jcc	b, .LBB0_39
.LBB0_41:                               ;   in Loop: Header=BB0_1 Depth=1
	cmp	r7, r5
	jcc	lt, .LBB0_42
; %bb.43:                               ;   in Loop: Header=BB0_1 Depth=1
	mov	r10, r6
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	jmp	.LBB0_44
.LBB0_51:                               ;   in Loop: Header=BB0_44 Depth=2
	add	r6, r5, r0
	ild	r0, r0
	;APP
	push	r0

	;NO_APP
.LBB0_73:                               ;   in Loop: Header=BB0_44 Depth=2
	add	r6, r12, r6
	sub	r7, r5, r7
	cmp	r7, r8
	jcc	z, .LBB0_45
.LBB0_44:                               ;   Parent Loop BB0_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ild	r0, r0
	ldi	r8, 0x00000000
	cmp	r0, r8
	jcc	nz, .LBB0_45
; %bb.50:                               ;   in Loop: Header=BB0_44 Depth=2
	ild	r0, r6
	cmp	r0, r5
	jcc	z, .LBB0_51
; %bb.52:                               ;   in Loop: Header=BB0_44 Depth=2
	ild	r0, r6
	cmp	r0, r12
	jcc	nz, .LBB0_73
; %bb.53:                               ;   in Loop: Header=BB0_44 Depth=2
	add	r6, r5, r13
	;APP
	pop	r0

	;NO_APP
	;APP
	pop	r1

	;NO_APP
	ild	r2, r13
	ofp	r14
	ldi	r15, 0x00000015
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	cmp	r2, r3
	jcc	gt, .LBB0_59
; %bb.54:                               ;   in Loop: Header=BB0_44 Depth=2
	ofp	r14
	ldi	r15, 0x00000017
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	cmp	r2, r3
	jcc	z, .LBB0_68
; %bb.55:                               ;   in Loop: Header=BB0_44 Depth=2
	ofp	r14
	ldi	r15, 0x00000014
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	cmp	r2, r3
	jcc	z, .LBB0_64
; %bb.56:                               ;   in Loop: Header=BB0_44 Depth=2
	ofp	r14
	ldi	r15, 0x00000011
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	cmp	r2, r3
	jcc	nz, .LBB0_69
; %bb.57:                               ;   in Loop: Header=BB0_44 Depth=2
	add	r1, r0, r0
	jmp	.LBB0_58
.LBB0_59:                               ;   in Loop: Header=BB0_44 Depth=2
	ofp	r14
	ldi	r15, 0x00000012
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	cmp	r2, r3
	jcc	z, .LBB0_63
; %bb.60:                               ;   in Loop: Header=BB0_44 Depth=2
	ofp	r14
	ldi	r15, 0x00000016
	ldo	r14, r15, r3                    ; 4-byte Folded Reload
	cmp	r2, r3
	jcc	z, .LBB0_65
; %bb.61:                               ;   in Loop: Header=BB0_44 Depth=2
	ldi	r3, 0x0000007f
	cmp	r2, r3
	jcc	nz, .LBB0_69
; %bb.62:                               ;   in Loop: Header=BB0_44 Depth=2
	;APP
	push	r1

	;NO_APP
	sub	r8, r0, r0
	jmp	.LBB0_58
.LBB0_68:                               ;   in Loop: Header=BB0_44 Depth=2
	ldi	r0, 0x01ffff5e
	jmp	.LBB0_70
.LBB0_63:                               ;   in Loop: Header=BB0_44 Depth=2
	sub	r1, r0, r0
	jmp	.LBB0_58
.LBB0_69:                               ;   in Loop: Header=BB0_44 Depth=2
	ldi	r0, 0x01ffff3f
	ofp	r14
	ldi	r15, 0x00000007
	add	r14, r15, r14
	ist	r13, r14                        ; 4-byte Folded Spill
	call	__aurora_put_string
	ofp	r14
	ldi	r15, 0x00000007
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ild	r0, r0
	ldi	r1, 0x05000000
	ist	r0, r1
	ldi	r0, 0x01ffff3a
.LBB0_70:                               ;   in Loop: Header=BB0_44 Depth=2
	call	__aurora_put_string
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r12                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r5, r0
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
.LBB0_71:                               ;   in Loop: Header=BB0_44 Depth=2
	ofp	r14
	ldi	r15, 0x00000013
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	jmp	.LBB0_72
.LBB0_64:                               ;   in Loop: Header=BB0_44 Depth=2
	mul	r1, r0, r0
.LBB0_58:                               ;   in Loop: Header=BB0_44 Depth=2
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
.LBB0_72:                               ;   in Loop: Header=BB0_44 Depth=2
	ofp	r14
	ldi	r15, 0x00000013
	add	r14, r15, r14
	ist	r0, r14                         ; 4-byte Folded Spill
	;APP
	push	r0

	;NO_APP
	jmp	.LBB0_73
.LBB0_65:                               ;   in Loop: Header=BB0_44 Depth=2
	cmp	r0, r8
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	jcc	z, .LBB0_66
; %bb.67:                               ;   in Loop: Header=BB0_44 Depth=2
	sdiv	r1, r0, r0
	jmp	.LBB0_72
.LBB0_66:                               ;   in Loop: Header=BB0_44 Depth=2
	ldi	r0, 0x01ffff7a
	call	__aurora_put_string
	ofp	r14
	ldi	r15, 0x0000000c
	ldo	r14, r15, r12                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r5, r0
	jmp	.LBB0_71
.LBB0_42:                               ;   in Loop: Header=BB0_1 Depth=1
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
.LBB0_45:                               ;   in Loop: Header=BB0_1 Depth=1
	ofp	r14
	ldi	r15, 0x0000000e
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ild	r0, r0
	ldi	r6, 0x00000000
	cmp	r0, r6
	jcc	nz, .LBB0_47
; %bb.46:                               ;   in Loop: Header=BB0_1 Depth=1
	ldi	r0, 0x01ffff31
	call	__aurora_put_string
	;APP
	pop	r0

	;NO_APP
	ofp	r14
	ldi	r15, 0x0000001c
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r0, r1
	ild	r0, r1
	call	__aurora_put_int
	ofp	r14
	ldi	r15, 0x0000000b
	ldo	r14, r15, r11                   ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000006
	ldo	r14, r15, r2                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x0000000a
	ldo	r14, r15, r10                   ; 4-byte Folded Reload
	ldi	r0, 0x05000000
	ofp	r14
	ldi	r15, 0x0000001a
	ldo	r14, r15, r1                    ; 4-byte Folded Reload
	ist	r1, r0
	ofp	r14
	ldi	r15, 0x0000001b
	ldo	r14, r15, r0                    ; 4-byte Folded Reload
	ist	r5, r0
.LBB0_47:                               ;   in Loop: Header=BB0_1 Depth=1
	ild	r0, r2
	cmp	r0, r5
	ofp	r14
	ldi	r15, 0x00000018
	ldo	r14, r15, r7                    ; 4-byte Folded Reload
	ofp	r14
	ldi	r15, 0x00000019
	ldo	r14, r15, r8                    ; 4-byte Folded Reload
	jcc	lt, .LBB0_1
; %bb.48:                               ;   in Loop: Header=BB0_1 Depth=1
	mov	r6, r0
.LBB0_49:                               ;   Parent Loop BB0_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	r0, r11, r1
	ist	r6, r1
	add	r0, r10, r1
	ist	r6, r1
	add	r0, r5, r0
	ild	r1, r2
	cmp	r0, r1
	jcc	lt, .LBB0_49
	jmp	.LBB0_1
.Lfunc_end0:
	.size	_start, .Lfunc_end0-_start
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
	jcc	z, .LBB1_2
	jmp	.LBB1_1
.LBB1_1:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000002d
	cmp	r0, r1
	jcc	nz, .LBB1_3
	jmp	.LBB1_2
.LBB1_2:
	ofp	r0
	ldi	r1, 0x00000001
	ist	r1, r0
	jmp	.LBB1_10
.LBB1_3:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000002a
	cmp	r0, r1
	jcc	z, .LBB1_5
	jmp	.LBB1_4
.LBB1_4:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000002f
	cmp	r0, r1
	jcc	nz, .LBB1_6
	jmp	.LBB1_5
.LBB1_5:
	ofp	r0
	ldi	r1, 0x00000002
	ist	r1, r0
	jmp	.LBB1_10
.LBB1_6:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007f
	cmp	r0, r1
	jcc	z, .LBB1_8
	jmp	.LBB1_7
.LBB1_7:
	ofp	r0
	ldi	r15, 0x00000001
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007e
	cmp	r0, r1
	jcc	nz, .LBB1_9
	jmp	.LBB1_8
.LBB1_8:
	ofp	r0
	ldi	r1, 0x00000003
	ist	r1, r0
	jmp	.LBB1_10
.LBB1_9:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB1_10
.LBB1_10:
	ofp	r0
	ild	r0, r0
	fts
	popfp
	ret
.Lfunc_end1:
	.size	priority, .Lfunc_end1-priority
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
	jcc	gt, .LBB2_2
	jmp	.LBB2_1
.LBB2_1:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB2_13
.LBB2_2:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x00000028
	cmp	r0, r1
	jcc	nz, .LBB2_4
	jmp	.LBB2_3
.LBB2_3:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB2_13
.LBB2_4:
	ofp	r0
	ldi	r15, 0x00000003
	add	r0, r15, r0
	ild	r0, r0
	ofp	r1
	ldi	r15, 0x00000004
	add	r1, r15, r1
	ild	r1, r1
	cmp	r0, r1
	jcc	gt, .LBB2_8
	jmp	.LBB2_5
.LBB2_5:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007f
	cmp	r0, r1
	jcc	z, .LBB2_8
	jmp	.LBB2_6
.LBB2_6:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007e
	cmp	r0, r1
	jcc	z, .LBB2_8
	jmp	.LBB2_7
.LBB2_7:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB2_13
.LBB2_8:
	ofp	r0
	ldi	r15, 0x00000003
	add	r0, r15, r0
	ild	r0, r0
	ofp	r1
	ldi	r15, 0x00000004
	add	r1, r15, r1
	ild	r1, r1
	cmp	r0, r1
	jcc	ge, .LBB2_12
	jmp	.LBB2_9
.LBB2_9:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007f
	cmp	r0, r1
	jcc	z, .LBB2_11
	jmp	.LBB2_10
.LBB2_10:
	ofp	r0
	ldi	r15, 0x00000002
	add	r0, r15, r0
	ild	r0, r0
	ldi	r1, 0x0000007e
	cmp	r0, r1
	jcc	nz, .LBB2_12
	jmp	.LBB2_11
.LBB2_11:
	ofp	r0
	ldi	r1, 0x00000000
	ist	r1, r0
	jmp	.LBB2_13
.LBB2_12:
	ofp	r0
	ldi	r1, 0x00000001
	ist	r1, r0
	jmp	.LBB2_13
.LBB2_13:
	ofp	r0
	ild	r0, r0
	fts
	popfp
	ret
.Lfunc_end2:
	.size	should_pop_operator, .Lfunc_end2-should_pop_operator
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
	ldi	r3, 0x01fffefe
	ild	r1, r3
	ldi	r0, 0x00000000
	cmp	r1, r0
	jcc	z, .LBB3_2
.LBB3_1:
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
.LBB3_2:
	ldi	r1, 0x04000000
	ild	r1, r1
	ldi	r2, 0x000000ff
	and	r1, r2, r1
	ldi	r2, 0x00000029
	cmp	r1, r2
	jcc	le, .LBB3_11
; %bb.3:
	ldi	r3, 0x00000050
	cmp	r1, r3
	jcc	gt, .LBB3_18
; %bb.4:
	ldi	r3, 0x0000002a
	cmp	r1, r3
	jcc	z, .LBB3_6
; %bb.5:
	ldi	r3, 0x0000002f
	cmp	r1, r3
	jcc	nz, .LBB3_26
.LBB3_6:
	ldi	r3, 0x01fffafc
	ild	r4, r3
	ild	r5, r3
	ldi	r6, 0xffffffff
	cmp	r4, r6
	jcc	le, .LBB3_22
; %bb.7:
	ldi	r4, 0x00000001
	sft	r5, r4, r6
	ldi	r5, 0x01fff6fc
	add	r6, r5, r6
	ild	r6, r6
	ldi	r7, 0x00000002
	cmp	r6, r7
	ldi	r6, 0x01fff6fd
	jcc	nz, .LBB3_9
; %bb.8:
	ild	r8, r3
	sft	r8, r4, r8
	add	r8, r6, r8
	ild	r8, r8
	cmp	r8, r2
	jcc	z, .LBB3_10
.LBB3_9:
	ild	r2, r3
	add	r2, r4, r2
	ist	r2, r3
	ild	r2, r3
	sft	r2, r4, r2
	add	r2, r5, r2
	ist	r4, r2
	ldi	r2, 0x01ffff02
	ild	r2, r2
	ild	r8, r3
	sft	r8, r4, r8
	add	r8, r6, r8
	ist	r2, r8
.LBB3_10:
	ild	r2, r3
	add	r2, r4, r2
	ist	r2, r3
	ild	r2, r3
	sft	r2, r4, r2
	add	r2, r5, r2
	ist	r7, r2
	ild	r2, r3
	sft	r2, r4, r2
	add	r2, r6, r2
	ist	r1, r2
	ldi	r2, 0x01ffff02
	ist	r0, r2
	jmp	.LBB3_23
.LBB3_11:
	ldi	r4, 0x0000000a
	cmp	r1, r4
	jcc	z, .LBB3_15
; %bb.12:
	ldi	r4, 0x0000000d
	cmp	r1, r4
	jcc	z, .LBB3_15
; %bb.13:
	ldi	r3, 0x00000020
	cmp	r1, r3
	jcc	nz, .LBB3_26
.LBB3_14:
	ldi	r0, 0x05000000
	ist	r3, r0
	jmp	.LBB3_1
.LBB3_15:
	ldi	r1, 0x01fffeff
	ild	r1, r1
	cmp	r1, r0
	jcc	z, .LBB3_17
; %bb.16:
	ldi	r0, 0x01fffafc
	ild	r1, r0
	ldi	r2, 0x00000001
	add	r1, r2, r1
	ist	r1, r0
	ild	r1, r0
	sft	r1, r2, r1
	ldi	r4, 0x01fff6fc
	add	r1, r4, r1
	ist	r2, r1
	ldi	r1, 0x01ffff02
	ild	r1, r1
	ild	r0, r0
	sft	r0, r2, r0
	ldi	r2, 0x01fff6fd
	add	r0, r2, r0
	ist	r1, r0
.LBB3_17:
	ldi	r0, 0x00000001
	ist	r0, r3
	ldi	r0, 0x01ffffb6
	call	__aurora_put_string
	jmp	.LBB3_1
.LBB3_18:
	ldi	r4, 0x0000007e
	cmp	r1, r4
	jcc	nz, .LBB3_25
; %bb.19:
	ldi	r1, 0x01ffff00
	ild	r1, r1
	cmp	r1, r0
	jcc	nz, .LBB3_21
; %bb.20:
	ldi	r0, 0x01ffff03
	call	__aurora_put_string
.LBB3_21:
	ldi	r0, 0x01fffefd
	ild	r0, r0
	ldi	r1, 0x01ffff02
	ist	r0, r1
	ldi	r0, 0x01fffeff
	ldi	r1, 0x00000001
	ist	r1, r0
	ldi	r0, 0x05000000
	ist	r4, r0
	jmp	.LBB3_1
.LBB3_22:
	ldi	r2, 0x00000001
	add	r5, r2, r4
	ist	r4, r3
	ild	r4, r3
	sft	r4, r2, r4
	ldi	r5, 0x01fff6fc
	add	r4, r5, r4
	ist	r2, r4
	ldi	r4, 0x01ffff02
	ild	r6, r4
	ild	r7, r3
	sft	r7, r2, r7
	ldi	r8, 0x01fff6fd
	add	r7, r8, r7
	ist	r6, r7
	ild	r6, r3
	add	r6, r2, r6
	ist	r6, r3
	ild	r6, r3
	sft	r6, r2, r6
	add	r6, r5, r5
	ldi	r6, 0x00000002
	ist	r6, r5
	ild	r3, r3
	sft	r3, r2, r2
	add	r2, r8, r2
	ist	r1, r2
	ist	r0, r4
.LBB3_23:
	ldi	r2, 0x01fffeff
	ist	r0, r2
.LBB3_24:
	ldi	r0, 0x05000000
	ist	r1, r0
	jmp	.LBB3_1
.LBB3_25:
	ldi	r3, 0x00000051
	cmp	r1, r3
	jcc	z, .LBB3_51
.LBB3_26:
	ldi	r3, 0x0000002a
	cmp	r1, r3
	jcc	gt, .LBB3_33
; %bb.27:
	ldi	r3, 0x00000028
	cmp	r1, r3
	jcc	z, .LBB3_39
; %bb.28:
	cmp	r1, r2
	jcc	nz, .LBB3_37
; %bb.29:
	ldi	r1, 0x01fffafc
	ild	r4, r1
	ldi	r3, 0x00000001
	sft	r4, r3, r5
	ldi	r4, 0x01fff6fc
	add	r5, r4, r5
	ild	r6, r5
	ldi	r5, 0x00000002
	cmp	r6, r5
	jcc	nz, .LBB3_31
; %bb.30:
	ild	r6, r1
	sft	r6, r3, r6
	ldi	r7, 0x01fff6fd
	add	r6, r7, r6
	ild	r6, r6
	cmp	r6, r2
	jcc	z, .LBB3_32
.LBB3_31:
	ild	r6, r1
	add	r6, r3, r6
	ist	r6, r1
	ild	r6, r1
	sft	r6, r3, r6
	add	r6, r4, r6
	ist	r3, r6
	ldi	r6, 0x01ffff02
	ild	r6, r6
	ild	r7, r1
	sft	r7, r3, r7
	ldi	r8, 0x01fff6fd
	add	r7, r8, r7
	ist	r6, r7
.LBB3_32:
	ild	r6, r1
	add	r6, r3, r6
	ist	r6, r1
	ild	r6, r1
	sft	r6, r3, r6
	add	r6, r4, r4
	ist	r5, r4
	ild	r1, r1
	sft	r1, r3, r1
	ldi	r3, 0x01fff6fd
	add	r1, r3, r1
	ist	r2, r1
	ldi	r1, 0x01ffff02
	ist	r0, r1
	ldi	r1, 0x01fffeff
	ist	r0, r1
	ldi	r0, 0x05000000
	ist	r2, r0
	jmp	.LBB3_1
.LBB3_33:
	ldi	r5, 0x0000002d
	cmp	r1, r5
	jcc	z, .LBB3_35
; %bb.34:
	ldi	r3, 0x0000002b
	cmp	r1, r3
	jcc	nz, .LBB3_37
.LBB3_35:
	ldi	r4, 0x01fffeff
	ild	r7, r4
	ldi	r3, 0x01fffafc
	ild	r6, r3
	cmp	r7, r0
	jcc	z, .LBB3_40
; %bb.36:
	ldi	r2, 0x00000001
	add	r6, r2, r5
	ist	r5, r3
	ild	r5, r3
	sft	r5, r2, r5
	ldi	r6, 0x01fff6fc
	add	r5, r6, r5
	ist	r2, r5
	ldi	r5, 0x01ffff02
	ild	r7, r5
	ild	r8, r3
	sft	r8, r2, r8
	ldi	r9, 0x01fff6fd
	add	r8, r9, r8
	ist	r7, r8
	ild	r7, r3
	add	r7, r2, r7
	ist	r7, r3
	ild	r7, r3
	sft	r7, r2, r7
	add	r7, r6, r6
	ldi	r7, 0x00000002
	ist	r7, r6
	ild	r3, r3
	sft	r3, r2, r2
	add	r2, r9, r2
	ist	r1, r2
	ist	r0, r5
	ist	r0, r4
	jmp	.LBB3_24
.LBB3_37:
	ldi	r0, 0x00000030
	sub	r1, r0, r0
	ldi	r2, 0x00000009
	cmp	r0, r2
	jcc	b, .LBB3_1
; %bb.38:
	ldi	r2, 0x01ffff02
	ild	r3, r2
	ldi	r4, 0x0000000a
	mul	r3, r4, r3
	add	r3, r0, r0
	ist	r0, r2
	ldi	r0, 0x01fffeff
	ldi	r2, 0x00000001
	jmp	.LBB3_50
.LBB3_39:
	ldi	r1, 0x01fffafc
	ild	r2, r1
	ldi	r4, 0x00000001
	add	r2, r4, r2
	ist	r2, r1
	ild	r2, r1
	sft	r2, r4, r2
	ldi	r5, 0x01fff6fc
	add	r2, r5, r2
	ldi	r5, 0x00000002
	ist	r5, r2
	ild	r1, r1
	sft	r1, r4, r1
	ldi	r2, 0x01fff6fd
	add	r1, r2, r1
	ist	r3, r1
	ldi	r1, 0x01ffff02
	ist	r0, r1
	ldi	r1, 0x01fffeff
	ist	r0, r1
	jmp	.LBB3_14
.LBB3_40:
	ild	r7, r3
	ldi	r8, 0xffffffff
	cmp	r6, r8
	jcc	le, .LBB3_46
; %bb.41:
	ldi	r6, 0x00000001
	sft	r7, r6, r8
	ldi	r7, 0x01fff6fd
	add	r8, r7, r8
	ild	r8, r8
	cmp	r8, r2
	jcc	nz, .LBB3_44
; %bb.42:
	ild	r2, r3
	sft	r2, r6, r8
	ldi	r2, 0x01fff6fc
	add	r8, r2, r8
	ild	r9, r8
	ldi	r8, 0x00000002
	cmp	r9, r8
	jcc	nz, .LBB3_44
; %bb.43:
	ild	r5, r3
	add	r5, r6, r5
	ist	r5, r3
	ild	r5, r3
	sft	r5, r6, r5
	add	r5, r2, r2
	ist	r8, r2
	ild	r2, r3
	sft	r2, r6, r2
	add	r2, r7, r2
	ist	r1, r2
	ldi	r2, 0x01ffff02
	ist	r0, r2
	ist	r0, r4
	jmp	.LBB3_24
.LBB3_44:
	ild	r0, r3
	add	r0, r6, r0
	ist	r0, r3
	ild	r0, r3
	sft	r0, r6, r0
	ldi	r2, 0x01fff6fc
	add	r0, r2, r0
	ldi	r2, 0x00000002
	ist	r2, r0
	ild	r0, r3
	sft	r0, r6, r0
	add	r0, r7, r0
	cmp	r1, r5
	jcc	nz, .LBB3_49
.LBB3_47:
	ldi	r2, 0x0000007f
	jmp	.LBB3_50
.LBB3_46:
	ldi	r0, 0x00000001
	add	r7, r0, r2
	ist	r2, r3
	ild	r2, r3
	sft	r2, r0, r2
	ldi	r4, 0x01fff6fc
	add	r2, r4, r2
	ldi	r4, 0x00000002
	ist	r4, r2
	ild	r2, r3
	sft	r2, r0, r0
	ldi	r2, 0x01fff6fd
	add	r0, r2, r0
	cmp	r1, r5
	jcc	z, .LBB3_47
.LBB3_49:
	ldi	r2, 0x0000007e
.LBB3_50:
	ist	r2, r0
	jmp	.LBB3_24
.LBB3_51:
	ldi	r0, 0x01ffff2b
	call	__aurora_put_string
	;APP
	halt

	;NO_APP
.Lfunc_end3:
	.size	handle_input, .Lfunc_end3-handle_input
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_put_int
	.type	__aurora_put_int,@function
__aurora_put_int:                       ; @__aurora_put_int
; %bb.0:
	ldi	r1, 0xffffffff
	cmp	r0, r1
	jcc	le, .LBB4_1
; %bb.7:
	ldi	r1, 0x00000000
	cmp	r0, r1
	jcc	nz, .LBB4_2
; %bb.8:
	ldi	r0, 0x05000000
	ldi	r1, 0x00000030
	ist	r1, r0
	ret
.LBB4_1:
	ldi	r1, 0x05000000
	ldi	r2, 0x0000002d
	ist	r2, r1
	ldi	r1, 0x00000000
	sub	r1, r0, r0
.LBB4_2:
	;APP
	osp	r1

	;NO_APP
	ldi	r2, 0x0000000a
	ldi	r3, 0x00000030
.LBB4_3:                                ; =>This Inner Loop Header: Depth=1
	rem	r0, r2, r10
	add	r10, r3, r10
	;APP
	push	r10

	;NO_APP
	div	r0, r2, r10
	cmp	r0, r2
	mov	r10, r0
	jcc	nc, .LBB4_3
; %bb.4:
	;APP
	osp	r0

	;NO_APP
	cmp	r0, r1
	jcc	nb, .LBB4_9
; %bb.5:
	ldi	r0, 0x05000000
.LBB4_6:                                ; =>This Inner Loop Header: Depth=1
	;APP
	pop	r2

	;NO_APP
	ist	r2, r0
	;APP
	osp	r2

	;NO_APP
	cmp	r2, r1
	jcc	b, .LBB4_6
.LBB4_9:
	ret
.Lfunc_end4:
	.size	__aurora_put_int, .Lfunc_end4-__aurora_put_int
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
	jcc	z, .LBB5_3
; %bb.1:
	ldi	r10, 0x00000001
	add	r0, r10, r11
	ldi	r0, 0x05000000
.LBB5_2:                                ; =>This Inner Loop Header: Depth=1
	ist	r3, r0
	add	r11, r10, r12
	ild	r3, r11
	and	r3, r1, r3
	cmp	r3, r2
	mov	r12, r11
	jcc	nz, .LBB5_2
.LBB5_3:
	ret
.Lfunc_end5:
	.size	__aurora_put_string, .Lfunc_end5-__aurora_put_string
                                        ; -- End function
	.p2align	2                               ; -- Begin function __aurora_zero_words
	.type	__aurora_zero_words,@function
__aurora_zero_words:                    ; @__aurora_zero_words
; %bb.0:
	jmp	.LBB6_1
.LBB6_1:                                ; =>This Inner Loop Header: Depth=1
	ldi	r2, 0x00000000
	ist	r2, r0
	ldi	r3, 0x00000001
	add	r0, r3, r0
	sub	r1, r3, r1
	cmp	r1, r2
	jcc	nz, .LBB6_1
	jmp	.LBB6_2
.LBB6_2:
	ret
.Lfunc_end6:
	.size	__aurora_zero_words, .Lfunc_end6-__aurora_zero_words
                                        ; -- End function
	.ident	"clang version 24.0.0git"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __aurora_zero_words
