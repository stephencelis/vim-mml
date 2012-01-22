sound_dpcm:
	lda	channel_sel
	asl	a
	tax
	dec	sound_counter,x		;カウンタいっこ減らし
	bne	.dpcm_end
	jsr	sound_dpcm_play
.dpcm_end
	rts

sound_dpcm_play:
	jsr	reg400x

	lda	sound_bank,x
	jsr	change_bank

	lda	[sound_add_low,x]
;----------
;ループ処理1
	cmp	#$a0
	bne	dmc_loop_program2
	jsr	loop_sub
	jmp	sound_dpcm_play
;----------
;ループ処理2(分岐)
dmc_loop_program2
	cmp	#$a1
	bne	dmc_bank_command
	jsr	loop_sub2
	jmp	sound_dpcm_play
;----------
;バンク切り替え
dmc_bank_command
	cmp	#$ee
	bne	no_dpcm
	jsr	data_bank_addr
	jmp	sound_dpcm_play
;----------
;dmc_data_end:
;	cmp	#$ff
;	bne	no_dpcm
;	jsr	data_end_sub
;	jmp	sound_dpcm_play
;----------
no_dpcm:
	cmp	#$fc
	bne	dmc_y_command_set
	.if	DPCM_RESTSTOP
	lda	#$0F		;この2行を有効にすると
	sta	$4015		;rでDPCM停止
	.endif
	jmp	ontyou2
;----------
;ｙコマンド設定
dmc_y_command_set:
	cmp	#$f5
	bne	wait_set2
	jsr	y_sub
	jmp	sound_dpcm_play

wait_set2:
	cmp	#$f4
	bne	dpcm_set
	jsr	wait_sub
	rts

dpcm_set:
	pha
	lda	#$0F
	sta	$4015		;DPCM stop
;;;;;;;;;; 無理矢理改造 "MCK virtual keyboard" by Norix
;	lda	#$FF		; 鳴らすぜ！
;	sta	$07A0+4*2	; 休符フラグ使っちゃうよー
;;;;;;;;;;
	pla

	if (DPCM_BANKSWITCH)
		pha
		tax
		if (!ALLOW_BANK_SWITCH)
		error "ALLOW_BANK_SWITCH and DPCM_BANKSWITCH conflicted!"
		endif
		lda	dpcm_data_bank,x		;4KB bank
		cmp	#BANK_MAX_IN_4KB+1
		bcs	.avoidbankswitch
		sta	$5ffc ; C000h-CFFFh
		clc
		adc	#1
		cmp	#BANK_MAX_IN_4KB+1
		bcs	.avoidbankswitch
		sta	$5ffd ; D000h-DFFFh
		clc
		adc	#1
		cmp	#BANK_MAX_IN_4KB+1
		bcs	.avoidbankswitch
		sta	$5ffe ; E000h-EFFFh
		clc
		adc	#1
		cmp	#BANK_MAX_IN_4KB+1
		bcs	.avoidbankswitch
		sta	$5fff ; F000h-FFFFh
.avoidbankswitch
		pla
	endif
	
	asl	a
	asl	a
	tax

	lda	dpcm_data,x		;DPCM control
	sta	$4010
	inx	
	lda	dpcm_data,x		;DPCM delta counter initialize
	cmp	#$FF
	beq	.skip
	sta	$4011
.skip
	inx	
	lda	dpcm_data,x		;DPCM address set
	sta	$4012
	inx	
	lda	dpcm_data,x		;DPCM length set
	sta	$4013

	lda	#$1F
	sta	$4015

ontyou2:
	jsr	reg400x
	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	sound_counter,x
	jsr	sound_data_address
	rts
