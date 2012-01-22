;-------------------------------------------------------------------------------
;macros and misc sub routines
;-------------------------------------------------------------------------------
;indirect_lda
;statement
;	indirect_lda	hoge_add_low	;hoge_add_low is not zero page address
;is same as:
;	lda	[hoge_add_low,x]
indirect_lda	.macro
	lda	\1,x		;hoge_add_low
	sta	<zero_add_low
	lda	\1+1,x		;hoge_add_high
	sta	<zero_add_high
	stx	<x_escape
	ldx	#$00
	lda	[zero_add_low,x]
	ldx	<x_escape
	.endm
;--------------------------------------	
reg400x:	; X = 現在のch<<1 , Y = X<<1
	lda	channel_sel
	asl	a
	tax
	asl	a
	tay
	rts

;-------------------------------------------------------------------------------
;initialize routine
;-------------------------------------------------------------------------------
INITIAL_WAIT_FRM = $00 ;最初にこのフレーム数だけウェイト
;初期化ルーチン
sound_init:
	.if TOTAL_SONGS > 1
	pha
	.endif
	lda	#$00
	ldx	#$00
.memclear
	sta	$0000,x
	sta	$0200,x
	sta	$0300,x
	sta	$0400,x
	sta	$0500,x
	sta	$0600,x
	sta	$0700,x
	inx
	bne	.memclear

	lda	#INITIAL_WAIT_FRM
	sta	initial_wait

	lda	#$0f		;内蔵音源初期化
	sta	$4015		;チャンネル使用フラグ
	lda	#$08		
	sta	$4001		;矩形波o2a以下対策
	sta	$4005

	.if	SOUND_GENERATOR & __FME7
	jsr	fme7_sound_init
	.endif

	.if	SOUND_GENERATOR & __MMC5
	jsr	mmc5_sound_init
	.endif

	.if	SOUND_GENERATOR & __FDS
	jsr	fds_sound_init
	.endif
	
	.if	SOUND_GENERATOR & __N106
	jsr	n106_sound_init
	.endif

	.if	SOUND_GENERATOR & __VRC6
	jsr	vrc6_sound_init
	.endif

	.if TOTAL_SONGS > 1
		pla
		asl	a
		tax
		
		lda	song_addr_table,x
		sta	<start_add_lsb
		lda	song_addr_table+1,x
		sta	<start_add_lsb+1
		
		.if (ALLOW_BANK_SWITCH)
			lda	song_bank_table,x
			sta	<start_bank
			lda	song_bank_table+1,x
			sta	<start_bank+1
		.endif
	
	.endif
	
	lda	#$00
	sta	channel_sel
sound_channel_set:
	lda	channel_sel
	cmp	#PTR_TRACK_END		;終わり？
	beq	sound_init_end
	
	lda	channel_sel
	
	
	.if TOTAL_SONGS > 1
		.if (ALLOW_BANK_SWITCH)
			tay				; y = ch; x = ch<<1;
			asl	a
			tax
			lda	[start_bank],y
			sta	sound_bank,x
			
			txa				; x = y = ch<<1;
			tay
		.else
			asl	a			; x = y = ch<<1;
			tax
			tay
		.endif
				
		lda	[start_add_lsb],y
		sta	<sound_add_low,x	;データ開始位置書き込み
		iny
		lda	[start_add_lsb],y
		sta	<sound_add_low+1,x	;データ開始位置書き込み
	.else
	
		tay				; x = ch<<1; y = ch;
		asl	a
		tax
		

		.if (ALLOW_BANK_SWITCH)
			lda	song_000_bank_table,y
			sta	sound_bank,x
		.endif
		
		lda	song_000_track_table,x
		sta	<sound_add_low,x	;データ開始位置書き込み
		lda	song_000_track_table+1,x
		sta	<sound_add_low+1,x	;データ開始位置書き込み

	.endif
	; x = ch<<1; y = ?
	
	lda	#$00
	sta	effect_flag,x
	lda	#$01
	sta	sound_counter,x
	
	inc	channel_sel
	jmp	sound_channel_set
sound_init_end:
	rts

;-------------------------------------------------------------------------------
;main routine
;-------------------------------------------------------------------------------
sound_driver_start:

	lda	initial_wait
	beq	.gogo
	dec	initial_wait
	rts
.gogo

	lda	#$00
	sta	channel_sel

internal_return:
	jsr	sound_internal
	inc	channel_sel		;次のチャンネルを設定して
	lda	channel_sel
	cmp	#$04
	bne	internal_return		;戻す

;	.if	DPCMON
sound_dpcm_part:
	jsr	sound_dpcm
;	.endif
	inc	channel_sel		;次のチャンネルを設定して

	.if	SOUND_GENERATOR & __FDS
	jsr	sound_fds		;FDS行ってこい
	inc	channel_sel		;次のチャンネルを設定して
	.endif

	.if	SOUND_GENERATOR & __VRC7
vrc7_return:
	jsr	sound_vrc7		;vrc7行ってこい
	inc	channel_sel		;次のチャンネルを設定して
	lda	channel_sel
	sec
	sbc	#PTRVRC7
	cmp	#$06			;vrc7は終わりか？
	bne	vrc7_return		;まだなら戻れ
	.endif

	.if	SOUND_GENERATOR & __VRC6
vrc6_return:
	jsr	sound_vrc6		;vrc6行ってこい
	inc	channel_sel		;次のチャンネルを設定して
	lda	channel_sel
	sec
	sbc	#PTRVRC6
	cmp	#$03			;vrc6は終わりか？
	bne	vrc6_return		;まだなら戻れ
	.endif

	.if	SOUND_GENERATOR & __N106
.rept:
	jsr	sound_n106		;n106行ってこい
	inc	channel_sel		;次のチャンネルを設定して
	lda	channel_sel
	sec
	sbc	#PTRN106
	cmp	n106_channel		;n106は終わりか？
	bne	.rept			;まだなら戻れ
	.endif

	.if	SOUND_GENERATOR & __FME7
fme7_return:
	jsr	sound_fme7		;fme7行ってこい
	inc	channel_sel		;次のチャンネルを設定して
	lda	channel_sel
	sec
	sbc	#PTRFME7
	cmp	#$03			;fme7は終わりか？
	bne	fme7_return		;まだなら戻れ
	.endif

	.if	SOUND_GENERATOR & __MMC5
mmc5_return:
	jsr	sound_mmc5		;mmc5行ってこい
	inc	channel_sel		;次のチャンネルを設定して
	lda	channel_sel
	sec
	sbc	#PTRMMC5
	cmp	#$02			;mmc5は終わりか？
	bne	mmc5_return		;まだなら戻れ
	.endif

	rts

;------------------------------------------------------------------------------
;command read sub routines
;------------------------------------------------------------------------------
sound_data_address:
	inc	<sound_add_low,x	;データアドレス＋１
	bne	return2			;位が上がったら
sound_data_address_inc_high
	inc	<sound_add_high,x	;データアドレス百の位（違）＋１
return2:
	rts

sound_data_address_add_a:
	clc
	adc	<sound_add_low,x
	sta	<sound_add_low,x
	bcs	sound_data_address_inc_high
	rts
;-------------------------------------------------------------------------------
change_bank:
;バンクをReg.Aに変えます～
;変更されるバンクアドレスはバンクコントローラによる
;現在はNSFのみ。
	if (ALLOW_BANK_SWITCH)
;バンク切り替えできるcondition: A <= BANK_MAX_IN_4KB
;i.e. A < BANK_MAX_IN_4KB + 1
;i.e. A - (BANK_MAX_IN_4KB+1) < 0
;i.e. NOT ( A - (BANK_MAX_IN_4KB+1) >= 0 )
;skipするcondition: A - (BANK_MAX_IN_4KB+1) >= 0
	cmp	#BANK_MAX_IN_4KB+1
	bcs	.avoidbankswitch
	sta	$5ffa ; A000h-AFFFh
	clc
	adc	#$01
	cmp	#BANK_MAX_IN_4KB+1
	bcs	.avoidbankswitch
	sta	$5ffb ; B000h-BFFFh
.avoidbankswitch
	endif
	rts

;-------------------------------------------------------------------------------
; リピート終了コマンド
;
; channel_loop++;
; if (channel_loop == <num>) {
;   channel_loop = 0;
;   残りのパラメータ無視してadrを次に進める;
; } else {
;   0xeeコマンドと同じ処理;
; }
loop_sub:
	jsr	sound_data_address
	inc	channel_loop,x
	lda	channel_loop,x
	cmp	[sound_add_low,x]	;繰り返し回数
	beq	loop_end
	jsr	sound_data_address
	jmp	bank_address_change
loop_end:
	lda	#$00
	sta	channel_loop,x
loop_esc_through			;loop_sub2から飛んでくる
	lda	#$04
	jsr	sound_data_address_add_a
	rts				;おちまい
;-----------
; リピート途中抜け
;
; channel_loop++;
; if (channel_loop == <num>) {
;   channel_loop = 0;
;   0xeeコマンドと同じ処理;
; } else {
;   残りのパラメータ無視してadrを次に進める;
; }

loop_sub2:
	jsr	sound_data_address
	inc	channel_loop,x
	lda	channel_loop,x
	cmp	[sound_add_low,x]	;繰り返し回数
	bne	loop_esc_through
	lda	#$00
	sta	channel_loop,x
	jsr	sound_data_address
	jmp	bank_address_change
;-------------------------------------------------------------------------------
;バンクセット (gotoコマンド。bank, adr_low, adr_high)
data_bank_addr:
	jsr	sound_data_address
bank_address_change:
	if (ALLOW_BANK_SWITCH)
	lda	[sound_add_low,x]
	sta	sound_bank,x
	endif

	jsr	sound_data_address
	lda	[sound_add_low,x]
	pha
	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	<sound_add_high,x
	pla
	sta	<sound_add_low,x	;新しいアドレス書込み

	rts
;-------------------------------------------------------------------------------
;data_end_sub:
;	ldy	channel_sel
;	
;	if (ALLOW_BANK_SWITCH)
;	lda	loop_point_bank,y
;	sta	sound_bank,x
;	endif
;	
;	lda	loop_point_table,x
;	sta	<sound_add_low,x	;ループ開始位置書き込み Low
;	inx
;	lda	loop_point_table,x
;	sta	<sound_add_low,x	;ループ開始位置書き込み High
;	rts
;-------------------------------------------------------------------------------
volume_sub:
	lda	effect_flag,x
	ora	#%00000001
	sta	effect_flag,x		;ソフトエンベ有効指定

;	lda	#$00
;	sta	register_low,x		;通常ボリュームをゼロに

	lda	temporary
	sta	softenve_sel,x
	asl	a
	tay
	lda	softenve_table,y	;ソフトエンベデータアドレス設定
	sta	soft_add_low,x
	iny
	lda	softenve_table,y
	sta	soft_add_high,x
	jsr	sound_data_address
	rts
;-------------------------------------------------------------------------------
lfo_set_sub:
	jsr	sound_data_address
	lda	[sound_add_low,x]
	cmp	#$ff
	bne	lfo_data_set

	jsr	reg400x
	lda	effect_flag,x
	and	#%10001111		;LFO無効処理
	sta	effect_flag,x
	jsr	sound_data_address
	rts
lfo_data_set:
	asl	a
	asl	a
	sta	lfo_data873,x

	tay
	lda	channel_sel
	asl	a
	tax
	lda	lfo_data,y
	sta	lfo_count,x		;ディレイセット
	sta	lfo_start_count,x
	iny
	lda	lfo_data,y
	sta	lfo_revers,x		;スピードセット
	sta	lfo_revers_count,x
	iny
	lda	lfo_data,y
	sta	lfo_depth,x		;デプスセット
	iny
	lda	lfo_data,y
	sta	lfo_harf_time,x
	sta	lfo_harf_count,x		;1/2カウンタセット

	jsr	warizan_start

	lda	channel_sel		;なぜこの処理を入れているかというと
	sec				;内蔵音源と拡張音源で+-が逆だからである
	sbc	#$05
	bcc	urararara2


	lda	effect_flag,x
	ora	#%00110000
	sta	effect_flag,x
	jmp	ittoke2
urararara2:
	lda	effect_flag,x
	and	#%11011111		;波形－処理
	ora	#%00010000		;LFO有効フラグセット
	sta	effect_flag,x
ittoke2:
	jsr	sound_data_address
	rts
;-------------------------------------------------------------------------------
detune_sub:
	jsr	sound_data_address
	lda	[sound_add_low,x]
	cmp	#$ff
	bne	detune_data_set

	jsr	reg400x
	lda	effect_flag,x
	and	#%01111111		;detune無効処理
	sta	effect_flag,x
	jsr	sound_data_address
	rts
detune_data_set:
	pha
	jsr	reg400x
	pla
	tay
	sta	detune_dat,x
	lda	effect_flag,x
	ora	#%10000000		;detune有効処理
	sta	effect_flag,x
	jsr	sound_data_address
	rts
;-------------------------------------------------------------------------------
pitch_set_sub:
	jsr	sound_data_address
	lda	[sound_add_low,x]
	cmp	#$ff
	bne	pitch_enverope_part
	lda	effect_flag,x
	and	#%11111101
	sta	effect_flag,x
	jsr	sound_data_address
	rts

pitch_enverope_part:
	sta	pitch_sel,x
	asl	a
	tay
	lda	pitchenve_table,y
	sta	pitch_add_low,x
	iny
	lda	pitchenve_table,y
	sta	pitch_add_high,x
	lda	effect_flag,x
	ora	#%00000010
	sta	effect_flag,x
	jsr	sound_data_address
	rts
;-------------------------------------------------------------------------------
arpeggio_set_sub:
	jsr	sound_data_address
	lda	[sound_add_low,x]
	cmp	#$ff
	bne	arpeggio_part

	lda	effect_flag,x
	and	#%11110111
	sta	effect_flag,x
	jsr	sound_data_address
	rts

arpeggio_part:
	sta	arpeggio_sel,x
	asl	a
	tay
	lda	arpeggio_table,y
	sta	arpe_add_low,x
	iny
	lda	arpeggio_table,y
	sta	arpe_add_high,x

	lda	effect_flag,x
	ora	#%00001000
	sta	effect_flag,x
	jsr	sound_data_address
	rts
;-------------------------------------------------------------------------------
direct_freq_sub:
	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	sound_freq_low,x		;Low
	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	sound_freq_high,x		;High
	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	sound_counter,x			;Counter
	jsr	sound_data_address
	jsr	effect_init
	rts
;-------------------------------------------------------------------------------
y_sub:
	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	<reg_add_low
	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	<reg_add_high
	jsr	sound_data_address
	lda	[sound_add_low,x]
	ldx	#$00
	sta	[reg_add_low,x]
	jsr	reg400x
	jsr	sound_data_address
	rts
;-------------------------------------------------------------------------------
wait_sub:
	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	sound_counter,x
	jsr	sound_data_address

	rts
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
;effect sub routines
;-------------------------------------------------------------------------------
detune_write_sub:
	lda	effect_flag,x
	and	#%10000000
	bne	detune_part
	rts

detune_part:
	lda	detune_dat,x
	and	#%10000000
	bne	detune_minus

detune_plus:
	lda	detune_dat,x
	clc
	adc	sound_freq_low,x
	sta	sound_freq_low,x
	bcs	mid_plus
	rts
mid_plus:
	inc	sound_freq_high,x
	bne	n106_high_through
	inc	sound_freq_n106,x
n106_high_through:
	rts

detune_minus:
	lda	detune_dat,x
	and	#%01111111
	sta	detune_tmp
	lda	sound_freq_low,x
	sec
	sbc	detune_tmp
	sta	sound_freq_low,x
	bcc	mid_minus
	rts
mid_minus:
	lda	sound_freq_high,x
	beq	.borrow
.no_borrow
	dec	sound_freq_high,x
	rts
.borrow
	dec	sound_freq_high,x
	dec	sound_freq_n106,x
	rts
;----------------------------------------------
sound_software_enverope:
	jsr	volume_enve_sub
	sta	register_low,x
	ora	register_high,x		;音色データ（上位4bit）と下位4bitで足し算
	sta	$4000,y			;書き込み～
	jsr	enverope_address	;アドレス一個増やして
	rts				;おしまい

volume_enve_sub:
	jsr	reg400x

	indirect_lda	soft_add_low		;エンベロープデータ読み込み
	cmp	#$ff			;最後かどーか
	beq	return3			;最後ならループ処理へ
	rts

return3:
	lda	softenve_sel,x
	asl	a
	tay
	lda	softenve_lp_table,y
	sta	soft_add_low,x
	iny
	lda	softenve_lp_table,y
	sta	soft_add_high,x
	jmp	volume_enve_sub
;-------------------------------------------------------------------------------
enverope_address:
	inc	soft_add_low,x
	bne	return5
	inc	soft_add_high,x
return5:
	rts
;-------------------------------------------------------------------------------
sound_duty_enverope:
	jsr	reg400x

	lda	channel_sel
	cmp	#$02
	beq	return21		;三角波なら飛ばし～

	indirect_lda	duty_add_low		;エンベロープデータ読み込み
	cmp	#$ff			;最後かどーか
	beq	return22		;最後ならそのままおしまい
	asl	a
	asl	a
	asl	a
	asl	a
	asl	a
	asl	a
	ora	#%00110000		;hardware envelope & ... disable
	sta	register_high,x
	ora	register_low,x		;音色データ（上位4bit）と下位4bitで足し算
	sta	$4000,y			;書き込み～
	jsr	duty_enverope_address	;アドレス一個増やして
return21:
	rts				;おしまい

return22:
	lda	duty_sel,x
	asl	a
	tay
	lda	dutyenve_lp_table,y
	sta	duty_add_low,x
	iny
	lda	dutyenve_lp_table,y
	sta	duty_add_high,x
	jmp	sound_duty_enverope

;-------------------------------------------------------------------------------
duty_enverope_address:
	inc	duty_add_low,x
	bne	return23
	inc	duty_add_high,x
return23:
	rts
;--------------------------------------	
sound_lfo:
	lda	sound_freq_high,x
	sta	temporary

	jsr	lfo_sub

	lda	sound_freq_low,x
	sta	$4002,y			;　　現在値をレジスタにセット
	lda	sound_freq_high,x
	cmp	temporary
	beq	end4
	sta	$4003,y
end4:
	rts				;ここまで
;-------------------------------------------------------------------------------
lfo_sub:
	jsr	reg400x
	lda	lfo_start_count,x
	beq	lfo_start
	dec	lfo_start_count,x
	rts
lfo_start:

	lda	effect_flag,x
	and	#%01000000
	bne	hanbun

	lda	lfo_harf_count,x
	beq	lfo_start_2

	dec	lfo_harf_count,x
	bne	lfo_start_2

	lda	effect_flag,x
	ora	#%01000000
	sta	effect_flag,x
hanbun:
	jsr	lfo_set_2
	lsr	lfo_revers,x
	lsr	lfo_revers_count,x
	jsr	warizan

lfo_start_2:
	asl	lfo_revers,x
	lda	lfo_revers_count,x	;現在の値を読み込んで
	cmp	lfo_revers,x
	beq	lfo_revers_set		;規定数に達していたら反転セット
	jmp	lfo_depth_set		;達していなければデプス処理へ

lfo_revers_set:
	lda	#$00			;
	sta	lfo_revers_count,x	;反転カウンタ初期化
	lda	effect_flag,x		;エフェクト読み込んで
	pha				;一時退避
	and	#%00100000
	bne	lfo_revers_p

	pla
	ora	#%00100000
	sta	effect_flag,x
	jmp	lfo_depth_set
lfo_revers_p:
	pla
	and	#%11011111
	sta	effect_flag,x

lfo_depth_set:
	lsr	lfo_revers,x
	lda	lfo_depth_count,x	;時間読み込み
	cmp	lfo_adc_sbc_time,x	;現在のカウンタと比較
	beq	lfo_depth_work		;一致していればデプス処理へ
	jmp	lfo_count_plus		;まだならカウンタプラスへ

lfo_depth_work:
	lda	#$00			;
	sta	lfo_depth_count,x	;デプスカウンタ初期化
	lda	effect_flag,x		;＋か－か
	and	#%00100000
	bne	lfo_depth_plus

lfo_depth_minus:
	lda	sound_freq_low,x	;デプス読み込み
	sec
	sbc	lfo_depth,x		;引く
	sta	sound_freq_low,x
	bcc	lfo_high_minus		;桁処理へ
	jmp	lfo_count_plus
lfo_high_minus:
	lda	sound_freq_high,x
	beq	.borrow			;1を引く前にゼロだったらその上の桁にも桁借り発生
.no_borrow
	dec	sound_freq_high,x
	jmp	lfo_count_plus
.borrow
	dec	sound_freq_high,x
	dec	sound_freq_n106,x
	jmp	lfo_count_plus

lfo_depth_plus:
	lda	sound_freq_low,x	;デプス読み込み
	clc
	adc	lfo_depth,x		;足す
	sta	sound_freq_low,x
	bcs	lfo_high_plus		;桁処理へ
	jmp	lfo_count_plus
lfo_high_plus:
	inc	sound_freq_high,x	;桁処理
	bne	lfo_count_plus
	inc	sound_freq_n106,x

lfo_count_plus:
	inc	lfo_revers_count,x	;カウンタ足してお終い
	inc	lfo_depth_count,x
	lda	effect_flag,x
	and	#%01000000
	beq	endend
	jsr	lfo_set_2
	jsr	warizan
endend:
	rts

;-------------------------------------------------------------------------------
lfo_set_2:
	lda	lfo_data873,x
	tay
	lda	channel_sel
	asl	a
	tax
	iny
	lda	lfo_data,y
	sta	lfo_revers,x		;スピードセット
	sta	lfo_revers_count,x
	iny
	lda	lfo_data,y
	sta	lfo_depth,x		;デプスセット
	rts
;-------
warizan_start:
	lda	#$00
	sta	kotae
	lda	lfo_revers,x
	cmp	lfo_depth,x
	beq	plus_one
	bmi	depth_wari
revers_wari:
	lda	lfo_depth,x
	sta	waru
	lda	lfo_revers,x
	jsr	warizan
	lda	kotae
	sta	lfo_adc_sbc_time,x
	sta	lfo_depth_count,x
	lda	#$01
	sta	lfo_depth,x
	rts
depth_wari:
	lda	lfo_revers,x
	sta	waru
	lda	lfo_depth,x
	jsr	warizan
	lda	kotae
	sta	lfo_depth,x
	lda	#$01
	sta	lfo_adc_sbc_time,x
	sta	lfo_depth_count,x
	rts
plus_one:
	lda	#$01
	sta	lfo_depth,x
	sta	lfo_adc_sbc_time,x
	sta	lfo_depth_count,x
	rts
warizan:
	inc	kotae
	sec
	sbc	waru
	beq	warizan_end
	bcc	warizan_end
	jmp	warizan
warizan_end:
	rts

;-------------------------------------------------------------------------------
sound_pitch_enverope:
	lda	sound_freq_high,x
	sta	temporary
	jsr	pitch_sub
pitch_write:
	lda	sound_freq_low,x
	sta	$4002,y
	lda	sound_freq_high,x
	cmp	temporary
	beq	end3
	sta	$4003,y
end3:
	jsr	pitch_enverope_address
	rts
;-------------------------------------------------------------------------------
pitch_sub:
	jsr	reg400x
	indirect_lda	pitch_add_low	
	cmp	#$ff
	beq	return62

	and	#%10000000
;	cmp	#%10000000
	bne	pitch_plus

	indirect_lda	pitch_add_low	
	clc
	adc	sound_freq_low,x
	sta	sound_freq_low,x	;low側書き込み
	bcs	freq_high_plus_2
	rts
freq_high_plus_2:
	inc	sound_freq_high,x	;high側＋１
	bne	rreturn
	inc	sound_freq_n106,x
rreturn
	rts				;レジスタ書き込みへGO!
pitch_plus:
	indirect_lda	pitch_add_low	
	and	#%01111111
	sta	pitch_tmp
	sec
	lda	sound_freq_low,x
	sbc	pitch_tmp
	sta	sound_freq_low,x
	bcc	freq_high_minus_2
	rts
freq_high_minus_2:
	lda	sound_freq_high,x
	beq	.borrow
.no_borrow
	dec	sound_freq_high,x
	rts
.borrow
	dec	sound_freq_high,x
	dec	sound_freq_n106,x
	rts
;-------------------------------------------------------------------------------
return62:
	indirect_lda	pitch_add_low	
	lda	pitch_sel,x
	asl	a
	tay
	lda	pitchenve_lp_table,y
	sta	pitch_add_low,x
	iny
	lda	pitchenve_lp_table,y
	sta	pitch_add_high,x
	jmp	pitch_sub
;-------------------------------------------------------------------------------
pitch_enverope_address:
	inc	pitch_add_low,x
	bne	return63
	inc	pitch_add_high,x
return63:
	rts
;-------------------------------------------------------------------------------
sound_high_speed_arpeggio:		;note enverope
;	lda	sound_freq_high,x
;	sta	temporary2
	jsr	note_enve_sub
	bcs	.end4			;0なので書かなくてよし
	jsr	frequency_set
;.note_freq_write:
	jsr	reg400x
	lda	sound_freq_low,x
	sta	$4002,y
	lda	sound_freq_high,x
;	cmp	temporary2		;ここに間違って,yがついてたの直してみたけどそれでも変だった
;	cmp	$4003,y
;	beq	.end2
	sta	$4003,y
;.end2:
	jsr	arpeggio_address
	rts
.end4
;	jsr	frequency_set
	jsr	arpeggio_address
	rts
;-------------------------------------------------------------------------------
note_add_set:
	lda	arpeggio_sel,x
	asl	a
	tay
	lda	arpeggio_lp_table,y
	sta	arpe_add_low,x
	iny
	lda	arpeggio_lp_table,y
	sta	arpe_add_high,x
	jmp	note_enve_sub
;-------------------------------------------------------------------------------
arpeggio_address:
	inc	arpe_add_low,x
	bne	return83
	inc	arpe_add_high,x
return83:
	rts
;-------------------------------------------------------------------------------
;----------------
;Output 
;	C=0(読み込んだ値は0じゃないので発音処理しろ)
;	C=1(読み込んだ値は0なので発音処理しなくていいよ)
;
note_enve_sub:

	jsr	reg400x
	indirect_lda	arpe_add_low		;ノートエンベデータ読み出し
	cmp	#$ff			;$ff（お終い）か？
	beq	note_add_set
	cmp	#$00			;ゼロか？
	beq	.note_enve_zero_end	;ゼロならC立ててお終い
	cmp	#$80
	beq	.note_enve_zero_end	;ゼロならC立ててお終い
	jmp	.arpeggio_sign_check
.note_enve_zero_end
	sec				;発音処理は不要
	rts
.arpeggio_sign_check
	and	#%10000000
;	cmp	#%10000000		;－か？
	bne	arpeggio_minus		;－処理へ

arpeggio_plus:
	indirect_lda	arpe_add_low		;ノートエンベデータを読み出して
	sta	arpeggio_tmp		;テンポラリに置く（ループ回数）
arpeggio_plus2:
	lda	sound_sel,x		;音階データ読み出し
	and	#$0f			;下位4bit抽出
	cmp	#$0b			;もしbなら
	beq	oct_plus		;オクターブ＋処理へ
	inc	sound_sel,x		;でなければ音階＋１
	jmp	loop_1			;ループ処理１へ
oct_plus:
	lda	sound_sel,x		;音階データ読み出し
	and	#$f0			;上位4bit取り出し＆下位4bitゼロ
	clc
	adc	#$10			;オクターブ＋１
	sta	sound_sel,x		;音階データ書き出し
loop_1:
	dec	arpeggio_tmp		;ループ回数－１
	lda	arpeggio_tmp		;んで読み出し
;	cmp	#$00			;ゼロか？
	beq	note_enve_end		;ならループ処理終わり
	jmp	arpeggio_plus2		;でなければまだ続く

arpeggio_minus:
	indirect_lda	arpe_add_low	
	and	#%01111111
	sta	arpeggio_tmp
arpeggio_minus2:
	lda	sound_sel,x		;音階データ読み出し
	and	#$0f			;下位4bit抽出
;	cmp	#$00			;ゼロか？
	beq	oct_minus		;ゼロなら－処理へ
	dec	sound_sel,x		;でなければ音階－１
	jmp	loop_2			;ループ処理２へ
oct_minus:
	lda	sound_sel,x		;音階データ読み出し
	clc
	adc	#$0b			;+b
	sec
	sbc	#$10			;-10
	sta	sound_sel,x		;音階データ書き出し
loop_2:
	dec	arpeggio_tmp		;ループ回数－１
	lda	arpeggio_tmp		;んで読み出し
	cmp	#$00			;ゼロか？
	bne	arpeggio_minus2		;ならループ処理終わり
note_enve_end:
	clc				;発音処理は必要
	rts				;
;-------------------------------------------------------------------------------
;oto_setで呼ばれる
effect_init:
;ソフトウェアエンベロープ読み込みアドレス初期化
	lda	softenve_sel,x
	asl	a
	tay
	lda	softenve_table,y
	sta	soft_add_low,x
	iny
	lda	softenve_table,y
	sta	soft_add_high,x

;ピッチエンベロープ読み込みアドレス初期化
	lda	pitch_sel,x
	asl	a
	tay
	lda	pitchenve_table,y
	sta	pitch_add_low,x
	iny
	lda	pitchenve_table,y
	sta	pitch_add_high,x

;デューティエンベロープ読み込みアドレス初期化
	lda	duty_sel,x
	asl	a
	tay
	lda	dutyenve_table,y
	sta	duty_add_low,x
	iny
	lda	dutyenve_table,y
	sta	duty_add_high,x

;ノートエンベロープ読み込みアドレス初期化
	lda	arpeggio_sel,x
	asl	a
	tay
	lda	arpeggio_table,y
	sta	arpe_add_low,x
	iny
	lda	arpeggio_table,y
	sta	arpe_add_high,x
;ソフトウェアLFO初期化
	lda	lfo_count,x
	sta	lfo_start_count,x
	lda	lfo_adc_sbc_time,x
	sta	lfo_depth_count,x
	lda	lfo_harf_time,x
	sta	lfo_harf_count,x
	lda	lfo_revers,x
	sta	lfo_revers_count,x

	lda	channel_sel
	sec
	sbc	#$04
	bmi	urararara

	lda	effect_flag,x
	and	#%10111111
	ora	#%00100000
	sta	effect_flag,x
	jmp	ittoke
urararara:
	lda	effect_flag,x
	and	#%10011111
	sta	effect_flag,x
	
ittoke:
;休符フラグクリア&Key Onフラグ書き込み
	lda	#%00000010
	sta	rest_flag,x
	rts


;-------------------------------------------------------------------------------
;internal 4 dpcm 1 fds 1 vrc7 6 vrc6 3 n106 8 fme7 3 mmc5 3(?)
;4+1+1+6+3+8+3+3=29ch
MAX_CH		=	$20		;32ch
;-------------------------------------------------------------------------------
;memory definition
;-------------------------------------------------------------------------------
;ゼロページメモリ定義
sound_add_low	=	$10		;command address
sound_add_high	=	$10+1		;

start_add_lsb	=	$f9
start_add_lsb_hi=	$fa
start_bank	=	$fe
start_bank_hi	=	$ff
reg_add_low	=	$f9	;temp
reg_add_high	=	$fa
x_escape	=	$fb
zero_add_low	=	$fc
zero_add_high	=	$fd
fds_wave_address=	$fe
fds_wave_addhigh=	$ff
vrc7_wave_add	=	$fe
vrc7_wave_add_hi=	$ff
VRC6_DST_REG_LOW=	$fe
VRC6_DST_REG_HIGH=	$ff
n106_wave_add	=	$fe
n106_wave_add_hi=	$ff
;それ以外に必要なメモリ
;	jsr	reg400x
;	してから
;	lda	memory,x
;	するので1バイトおきにデータがならぶ

;_add_lowと_add_highは近接している必要がある
soft_add_low	=	$0200+MAX_CH* 0  	;software envelope(@v) address
soft_add_high	=	$0200+MAX_CH* 0+1	;
pitch_add_low	=	$0200+MAX_CH* 2  	;pitch envelope (EP) address
pitch_add_high	=	$0200+MAX_CH* 2+1	;
duty_add_low	=	$0200+MAX_CH* 4  	;duty envelope (@@) address
duty_add_high	=	$0200+MAX_CH* 4+1	;
arpe_add_low	=	$0200+MAX_CH* 6  	;note envelope (EN) address
arpe_add_high	=	$0200+MAX_CH* 6+1	;
lfo_revers_count=	$0200+MAX_CH* 8  	;
lfo_depth_count	=	$0200+MAX_CH* 8+1	;
lfo_start_count	=	$0200+MAX_CH*10  	;
lfo_count	=	$0200+MAX_CH*10+1	;
lfo_adc_sbc_time=	$0200+MAX_CH*12  	;
lfo_depth	=	$0200+MAX_CH*12+1	;
lfo_revers	=	$0200+MAX_CH*14  	;
lfo_harf_time	=	$0200+MAX_CH*14+1	;未使用
lfo_data873	=	$0200+MAX_CH*16  	;vibrato (MP) no
lfo_harf_count	=	$0200+MAX_CH*16+1	;?
detune_dat	=	$0200+MAX_CH*18  	;detune value
register_high	=	$0200+MAX_CH*18+1	;
register_low	=	$0200+MAX_CH*20  	;
duty_sel	=	$0200+MAX_CH*20+1	;duty envelope no
channel_loop	=	$0200+MAX_CH*22  	;|: :| loop counter
rest_flag	=	$0200+MAX_CH*22+1	;
softenve_sel	=	$0200+MAX_CH*24  	;software envelope(@v) no
pitch_sel	=	$0200+MAX_CH*24+1	;pitch envelope (EP) no
arpeggio_sel	=	$0200+MAX_CH*26  	;note envelope (EN) no
effect_flag	=	$0200+MAX_CH*26+1	;
sound_sel	=	$0200+MAX_CH*28  	;note no.
sound_counter	=	$0200+MAX_CH*28+1	;wait counter
sound_freq_low	=	$0200+MAX_CH*30  	;
sound_freq_high	=	$0200+MAX_CH*30+1	;
sound_freq_n106	=	$0200+MAX_CH*32  	;n106じゃないchでも使ってる
sound_bank	=	$0200+MAX_CH*32+1	;
extra_mem0	=	$0200+MAX_CH*34  	;ここから先はchごとに用途が違う
extra_mem1	=	$0200+MAX_CH*34+1	;
extra_mem2	=	$0200+MAX_CH*36  	;

n106_volume	=	extra_mem0
n106_7c		=	extra_mem1

vrc7_key_stat	=	extra_mem0
vrc7_volume	=	extra_mem1

	.if	$0200+MAX_CH*38 > $0790
	.fail	"memory out of range"
	.endif
;その他
waru		=	$079c	;1byte
kotae		=	$079d	;1byte

detune_tmp	=	$07a1	;1byte
pitch_tmp	=	$07a3	;1byte
arpeggio_tmp	=	$07a5	;1byte
temporary	=	$07a7	;1byte
channel_sel	=	$07a9	;1byte
temporary2	=	$07ab

fds_hard_select	=	$07af
fds_volume	=	$07b1

n106_7f		=	$07b5

vrc7_temp	=	$07b8
n106_temp	=	$07b9
n106_temp_2	=	$07bb

fds_hard_count_1=	$07c0
fds_hard_count_2=	$07c1

initial_wait	=	$07c3

fme7_ch_sel	=	$07c5
fme7_ch_selx2	=	$07c6
fme7_data	=	$07c7
fme7_adr	=	$07c8


;effect_flag: DLLLadpv
;+------ detune flag
;l+----- software LFOスピード可変フラグ（予約）
;ll+---- software LFO方向フラグ0=- 1=+
;lll+--- software LFO flag
;llll+---- note enverope flag
;lllll+--- duty enverope flag / FDS hardware effect flag
;llllll+-- pitch enverope flag
;lllllll+- software enverope flag
;llllllll
;DLLLadpv


;rest_flag
;xxxxxxor
;|||||||+- rest
;||||||+-- key on (if set, must do sound_data_write)
;
