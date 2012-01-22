n106_sound_init:
	ldx	#$00
	lda	#$80
	sta	$f800
	lda	#$00
.soresore
	sta	$4800
	inx
	cpx	#$00
	beq	.owari
	jmp	.soresore
.owari
	lda	#$7f
	sta	$f800
	lda	n106_channel
	sec
	sbc	#$01
	asl	a
	asl	a
	asl	a
	asl	a
	sta	$4800
	sta	n106_7f
	rts


sound_n106:
	lda	channel_sel
	asl	a
	tax
	dec	sound_counter,x		;カウンタいっこ減らし
	beq	.sound_read_go		;ゼロならサウンド読み込み
	jsr	n106_do_effect		;ゼロ以外ならエフェクトして
	rts				;おわり
.sound_read_go
	jsr	sound_n106_read
	jsr	n106_do_effect
	lda	rest_flag,x
	and	#%00000010		;キーオンフラグ
	beq	.end1			
	jsr	sound_n106_write	;立っていたらデータ書き出し
	lda	rest_flag,x
	and	#%11111101		;キーオンフラグオフ
	sta	rest_flag,x
.end1
	rts

;-------
n106_do_effect:
	lda	rest_flag,x
	and	#%00000001
	beq	.duty_write2
	rts				;休符なら終わり

.duty_write2:

.enve_write2:
	lda	effect_flag,x
	and	#%00000001
	beq	.lfo_write2
	jsr	sound_n106_softenve

.lfo_write2:
	lda	effect_flag,x
	and	#%00010000
	beq	.pitchenve_write2
	jsr	sound_n106_lfo

.pitchenve_write2:
	lda	effect_flag,x
	and	#%00000010
	beq	.arpeggio_write2
	jsr	sound_n106_pitch_enve

.arpeggio_write2:
	lda	effect_flag,x
	and	#%00001000
	beq	.return7
	lda	rest_flag,x		;キーオンのときとそうでないときでアルペジオの挙動はちがう
	and	#%00000010		;キーオンフラグ
	bne	.arpe_key_on
	jsr	sound_n106_note_enve	;キーオンじゃないとき通常はこれ
	jmp	.return7
.arpe_key_on				;キーオンも同時の場合
	jsr	note_enve_sub		;メモリ調整だけで、ここでは書き込みはしない
	jsr	n106_freq_set
	jsr	arpeggio_address
.return7:
	rts
;------------------------------------------------
n106_freq_set:
	lda	channel_sel
	asl	a
	tax
	lda	sound_sel,x		;音階データ読み出し
	and	#%00001111		;下位4bitを取り出して
	asl	a
	asl	a
	tay

	lda	n106_frequency_table,y	;n106周波数テーブルからLowを読み出す
	sta	sound_freq_low,x	;書き込み
	iny				;yを１増やして
	lda	n106_frequency_table,y	;n106周波数テーブルからMidleを読み出す
	sta	sound_freq_high,x	;書き込み
	iny				;yを１増やして
	lda	n106_frequency_table,y	;n106周波数テーブルからHighを読み出す
	sta	sound_freq_n106,x	;書き込み

n106_oct_set1:

	lda	sound_sel,x		;音階データ読み出し
	lsr	a			;上位4bitを取り出し
	lsr	a			;
	lsr	a			;
	lsr	a			;
	sta	temporary
	cmp	#$08
	beq	n106_freq_end		;ゼロならそのまま終わり
	tay

n106_oct_set2:

	lsr	sound_freq_n106,x	;右シフト　末尾はCへ
	ror	sound_freq_high,x	;Cから持ってくるでよ　右ローテイト
	ror	sound_freq_low,x	;Cから持ってくるでよ　右ローテイト
	iny				;
	cpy	#$08
	bne	n106_oct_set2		;オクターブ分繰り返す

n106_freq_end:
	lda	temporary
	tay
n106_detune_loop:
	jsr	detune_write_sub
	dey
	bne	n106_detune_loop
	rts
;---------------------------------------------------------------
sound_n106_read:
	jsr	reg400x

	lda	sound_bank,x
	jsr	change_bank

	lda	[sound_add_low,x]
;----------
;ループ処理1
n106_loop_program
	cmp	#$a0
	bne	n106_loop_program2
	jsr	loop_sub
	jmp	sound_n106_read
;----------
;ループ処理2(分岐)
n106_loop_program2
	cmp	#$a1
	bne	n106_bank_command
	jsr	loop_sub2
	jmp	sound_n106_read
;----------
;バンク切り替え
n106_bank_command
	cmp	#$ee
	bne	n106_wave_set
	jsr	data_bank_addr
	jmp	sound_n106_read
;----------
;データエンド設定
;n106_data_end:
;	cmp	#$ff
;	bne	n106_wave_set
;	jsr	data_end_sub
;	jmp	sound_n106_read
;----------
;音色設定
n106_wave_set:
	cmp	#$fe
	bne	n106_volume_set
	jsr	sound_data_address
	lda	[sound_add_low,x]

	asl	a
	tax				;何番目の波形を使うかの設定開始

	lda	n106_wave_init,x	;;波形データ長リード
	asl	a
	asl	a
	sta	temporary

	lda	n106_wave_table,x
	sta	<n106_wave_add
	inx
	lda	n106_wave_table,x
	sta	<n106_wave_add_hi	;波形データ開始アドレスセット

	lda	n106_wave_init,x	;波形データオフセットアドレスリード
	sta	n106_temp_2

	lda	#$7c
	jsr	n106_write_sub
	jsr	reg400x
	lda	temporary
	ora	#%10000000
	sta	n106_7c,x
	sta	$4800			;波形データ長セット
	lsr	temporary
	lda	#$10
	sec
	sbc	temporary
	sta	temporary		;波形データ長算出

	lda	#$7e
	jsr	n106_write_sub
	lda	n106_temp_2
	sta	$4800			;波形データオフセットアドレスセット

	lsr	a
	ora	#%10000000		;自動インクリメントオン
	sta	$f800

	ldy	#$00
n106_wave_data_set:
	lda	[fds_wave_address],y
	sta	$4800			;波形書き込み（wave data write)
	iny
	cpy	temporary
	bmi	n106_wave_data_set

	jsr	reg400x
	jsr	sound_data_address
	jmp	sound_n106_read
;----------
;音量設定
n106_volume_set:
	cmp	#$fd
	bne	n106_rest_set
	jsr	sound_data_address
	lda	[sound_add_low,x]

	sta	temporary
	and	#%10000000		;bit7
	cmp	#%10000000
	bne	n106_softenve_part	;ソフトエンベ処理へ

n106_volume_part:
	lda	effect_flag,x
	and	#%11111110
	sta	effect_flag,x		;ソフトエンベ無効指定

	lda	temporary
	and	#%00001111
	sta	n106_volume,x
	lda	#$7f

	jsr	n106_write_sub
	lda	n106_7f
	ora	n106_volume,x
	sta	$4800

	jsr	sound_data_address
	jmp	sound_n106_read

n106_softenve_part:
	jsr	volume_sub
	jmp	sound_n106_read
;----------
n106_rest_set:
	cmp	#$fc
	bne	n106_lfo_set

	lda	rest_flag,x
	ora	#%00000001
	sta	rest_flag,x

	jsr	sound_data_address
	lda	[sound_add_low,x]
	sta	sound_counter,x

	lda	#$7f
	jsr	n106_write_sub
	lda	n106_7f
	sta	$4800

	jsr	sound_data_address
	rts
;----------
n106_lfo_set:
	cmp	#$fb
	bne	n106_detune_set
	jsr	lfo_set_sub
	jmp	sound_n106_read
;----------
n106_detune_set:
	cmp	#$fa
	bne	n106_pitch_set
	jsr	detune_sub
	jmp	sound_n106_read
;----------
;ピッチエンベロープ設定
n106_pitch_set:
	cmp	#$f8
	bne	n106_arpeggio_set
	jsr	pitch_set_sub
	jmp	sound_n106_read
;----------
;ノートエンベロープ設定
n106_arpeggio_set:
	cmp	#$f7
	bne	n106_freq_direct_set
	jsr	arpeggio_set_sub
	jmp	sound_n106_read
;----------
;再生周波数直接設定
n106_freq_direct_set:
	cmp	#$f6
	bne	n106_y_command_set
	jsr	direct_freq_sub
	rts
;----------
;ｙコマンド設定
n106_y_command_set:
	cmp	#$f5
	bne	n106_wait_set
	jsr	y_sub
	jmp	sound_n106_read
;----------
;ウェイト設定
n106_wait_set:
	cmp	#$f4
	bne	n106_oto_set
	jsr	wait_sub
	rts
;----------
n106_oto_set:
	sta	sound_sel,x		;処理はまた後で
	jsr	sound_data_address
	lda	[sound_add_low,x]	;音長読み出し
	sta	sound_counter,x		;実際のカウント値となります
	jsr	sound_data_address
	jsr	n106_freq_set		;周波数セットへ
;volume
	lda	#$7f
	jsr	n106_write_sub
	lda	n106_7f
	ora	n106_volume,x
	sta	$4800
	jsr	effect_init
	rts
;-------------------------------------------------------------------------------
sound_n106_write:
	jsr	reg400x

	lda	#$78
	jsr	n106_write_sub
	lda	sound_freq_low,x
	sta	$4800

	lda	#$7a
	jsr	n106_write_sub
	lda	sound_freq_high,x
	sta	$4800

	lda	#$7c
	jsr	n106_write_sub
	lda	n106_7c,x
	ora	sound_freq_n106,x
	sta	$4800
	rts
;-------------------------------------------------------------------------------
n106_write_sub
	sta	n106_temp
	lda	channel_sel
	sec
	sbc	#PTRN106
	asl	a
	asl	a
	asl	a
	eor	#$ff
	sec
	adc	n106_temp
	sta	$f800
	rts
;-----------------------------------------------------
sound_n106_softenve:
	jsr	volume_enve_sub
	sta	temporary
	lda	#$7f
	jsr	n106_write_sub
	lda	n106_7f
	ora	temporary
	sta	$4800
	jmp	enverope_address
;-------------------------------------------------------------------------------
sound_n106_lfo:
	jsr	lfo_sub
	jmp	sound_n106_write
;-------------------------------------------------------------------------------
sound_n106_pitch_enve:
	jsr	pitch_sub
	jsr	sound_n106_write
	jmp	pitch_enverope_address
;-------------------------------------------------------------------------------
sound_n106_note_enve
	jsr	note_enve_sub
	bcs	.end4			;0なので書かなくてよし
	jsr	n106_freq_set
	jsr	sound_n106_write
	jsr	arpeggio_address
	rts
.end4
;	jsr	n106_freq_set
	jsr	arpeggio_address
	rts
;-------------------------------------------------------------------------------
