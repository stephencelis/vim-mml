TOTAL_SONGS	equ	$04
SOUND_GENERATOR	equ	$00
PTRDPCM		equ	 4
PTRFDS		equ	 5
PTRVRC7		equ	 5
PTRVRC6		equ	 5
PTRN106		equ	 5
PTRFME7		equ	 5
PTRMMC5		equ	 5
PTR_TRACK_END		equ	 5
DPCM_RESTSTOP		equ	 0
DPCM_BANKSWITCH		equ	 0
DPCM_EXTRA_BANK_START		equ	 1
BANK_MAX_IN_4KB		equ	(0 + 0)*2+1
ALLOW_BANK_SWITCH		equ	0
TITLE	.macro
	db	$4b, $69, $6e, $67, $00, $00, $00, $00
	db	$00, $00, $00, $00, $00, $00, $00, $00
	db	$00, $00, $00, $00, $00, $00, $00, $00
	db	$00, $00, $00, $00, $00, $00, $00, $00
	.endm
COMPOSER	.macro
	db	$53, $68, $61, $75, $6e, $20, $49, $6e
	db	$6d, $61, $6e, $00, $00, $00, $00, $00
	db	$00, $00, $00, $00, $00, $00, $00, $00
	db	$00, $00, $00, $00, $00, $00, $00, $00
	.endm
MAKER	.macro
	db	$32, $30, $31, $30, $20, $53, $68, $61
	db	$75, $6e, $20, $49, $6e, $6d, $61, $6e
	db	$00, $00, $00, $00, $00, $00, $00, $00
	db	$00, $00, $00, $00, $00, $00, $00, $00
	.endm


