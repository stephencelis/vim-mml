;	.list
	.include	"define.inc"

    .bank	0
    .org	$8000
    .code

;NSF HEADER
	db	"NESM",$1A	;
	db	1		;Ver.
	db	TOTAL_SONGS	;Number of Songs
	db	1		;Start Song No.
	dw	LOAD		;Load
	dw	INIT		;Init
	dw	PLAY		;Play
  .org	$800E
	TITLE
;	db	"Title",$0
  .org	$802E
	COMPOSER
;	db	"Composer",$0
  .org	$804E
	MAKER
;	db	"Maker",$0
  .org	$806E
	dw	16666		;1000000 / (freq of NTSC) sec
	.if	(ALLOW_BANK_SWITCH)
	BANKSWITCH_INIT_MACRO
	.else
	db	0,0,0,0,0,0,0,0 ;Bankswitch Init Values
	.endif
	dw	20000		;1000000 / (freq of PAL)  sec
	db	%00
;                ||
;                |+-------------- PAL/NTSC
;                +--------------- dual PAL/NTSC tune or not
;	db	$02

__VRC6	=	%00000001
__VRC7	=	%00000010
__FDS	=	%00000100
__MMC5	=	%00001000
__N106	=	%00010000
__FME7	=	%00100000
	db	SOUND_GENERATOR
	db	0,0,0,0
LOAD:
INIT:
	jsr	sound_init
	rts
PLAY:
	jsr	sound_driver_start
	rts
;-------------------------------------------------------------------------------
	.include	"ppmck/sounddrv.h"
	.include	"ppmck/internal.h"
	.include	"ppmck/dpcm.h"
	.if	SOUND_GENERATOR & __FDS
	.include	"ppmck/fds.h"
	.endif
	.if	SOUND_GENERATOR & __VRC7
	.include	"ppmck/vrc7.h"
	.endif
	.if	SOUND_GENERATOR & __VRC6
	.include	"ppmck/vrc6.h"
	.endif
	.if	SOUND_GENERATOR & __N106
	.include	"ppmck/n106.h"
	.endif
	.if	SOUND_GENERATOR & __MMC5
	.include	"ppmck/mmc5.h"
	.endif
	.if	SOUND_GENERATOR & __FME7
	.include	"ppmck/fme7.h"
	.endif
	.include	"ppmck/freqdata.h"
	.include	"effect.h"

