;-------------------------------------------------------------------------------
psg_frequency_table:
	dw	$06AE,$064E,$05F4,$059E
	dw 	$054E,$0501,$04B9,$0476
	dw	$0436,$03F9,$03C0,$038A
	dw	$0000,$07f2,$0780,$0714

;再生周波数 = 1.79MHz / ((n+1)*16)　で算出
;0800h以上の音は出ません。従って(07F2h)o1 aより下の音は出ません
;0008h以下の音は出ません。従って(0008h)o9f+より上の音は出ません
;音程が上がるほど音痴になります
;-------------------------------------------------------------------------------
;実は使ってないです。上と同じテーブル使っている．．．直さねばなぁ
;tri_frequency_table:
;	dw	$06AE,$064E,$05F4,$059E	;0,1,2,3
;	dw 	$054E,$0501,$04B9,$0476	;4,5,6,7
;	dw	$0436,$03F9,$03C0,$038A	;8,9,a,b
;	dw	$0000,$07f2,$0780,$0714	;c,d,e,f
;再生周波数 = 1.79MHz / ((n+1)*32)　で算出
;0800h以上の音は出ません。従って(07F2h)o0 aより下の音は出ません。
;0008h以下の音は出ません。従って(0008h)o8f+より上の音は出ません。
;音程が上がるほど音痴になります
;-------------------------------------------------------------------------------
noise_frequency_table:

;		 +-------- ランダム数型生成
;		 l   ++++- 演奏サンプル速度
;		 l   llll  +++++---- 発音長カウンタロードレジスタ（要はハードエンベ）
;		 lxxxllll  lllllxxx
	db	%00000000,%00000000	;0
	db	%00000001,%00000000	;1
	db	%00000010,%00000000	;2
	db	%00000011,%00000000	;3

	db	%00000100,%00000000	;4
	db	%00000101,%00000000	;5
	db	%00000110,%00000000	;6
	db	%00000111,%00000000	;7

	db	%00001000,%00000000	;8
	db	%00001001,%00000000	;9
	db	%00001010,%00000000	;a
	db	%00001011,%00000000	;b

	db	%00001100,%00000000	;c
	db	%00001101,%00000000	;d
	db	%00001110,%00000000	;e
	db	%00001111,%00000000	;f
;-------------------------------------------------------------------------------
fds_frequency_table:
	dw	$0995,$0a26,$0ac0,$0b64	;c ,c+,d ,d+
	dw	$0c11,$0cc9,$0d8c,$0e5a	;e ,f ,f+,g
	dw	$0f35,$101c,$1110,$1214	;g+,a ,a+,b
	dw	$0000,$0000,$0000,$0000

;平均律（に近い数値）を基準にしています
;式は以下な感じで
;              111860.8 Hz  
;再生周波数 = ------------- x n(周波数用データ)
;	  	64 x 4096
;o6aより上の音はでません（テーブルはo6のモノ）
;音程が下がるほど音痴になります？

;        ユニゾン：	x1.0000
;        短２度：	x1.0595
;        長２度：	x1.1225
;        短３度：	x1.1892
;        長３度：	x1.2599
;        完全４度：	x1.3348
;        増４度(減５度):x1.4142
;        完全５度：	x1.4983
;        増５度(短６度):x1.5874 
;        長６度：	x1.6818
;        減７度：	x1.7818
;        長７度：	x1.8877
;        オクターブ ：	x2.0000 
;-------------------------------------------------------------------------------
vrc7_freq_table:
	dw	$00AC	; o4 :   C : 3
	dw	$00B6	; o4 :  C# : 4
	dw	$00C1	; o4 :   D : 5
	dw	$00CD	; o4 :  D# : 6
	dw	$00D9	; o4 :   E : 7
	dw	$00E6	; o4 :   F : 8
	dw	$00F3	; o4 :  F# : 9
	dw	$0102	; o4 :   G : 10
	dw	$0111	; o4 :  G# : 11
	dw	$0121	; o4 :   A : 12
	dw	$0133	; o4 :  A# : 13
	dw	$0145	; o4 :   B : 14


n106_frequency_table:
	db	$cc,$3e,$02,$00	; c	$023ECB
	db	$fa,$60,$02,$00	; c+	$0260F7
	db	$30,$85,$02,$00	; d	$02852B
	db	$8d,$ab,$02,$00	; d+	$02AB8B
	db	$33,$d4,$02,$00	; e	$02D432
	db	$43,$ff,$02,$00	; f	$02FF42
	db	$e2,$2c,$03,$00	; f+	$032CE3
	db	$39,$5d,$03,$00	; g	$035D39
	db	$6e,$90,$03,$00	; g+	$039068
	db	$b0,$c6,$03,$00	; a	$03C6B0
	db	$34,$00,$04,$00	; a+	$040034
	db	$1b,$3d,$04,$00	; b	$043D1B
;
;              n(周波数用データ) * 440 * (2-F)              4
; 再生周波数 = -------------------------------   *  ----------------
;                           15467                   ch(チャンネル数)
;
; n : 再生周波数用データは18bitで構成される$0-$3FFFF
; F : オクターブ( $44 , $4c ,... の第4ビット)0で１オクターブ上がる
;ch : 使用チャンネル数 1-8
;
;o1a =   1933 =   78Dh = 000000011110001101
;o4a =  15467 =  3C6Bh = 000011110001101011
;o8a = 247472 = 3C6B0h = 111100011010110000
;
;o8a より高い音は出ません（テーブルはo8のモノ）
;ピッチベンドもLFOも　ｘ　オクターブにすれば大体115を指定すると次の音だなぁ
;良い具合になるな。やっぱそうするかなぁ〜
