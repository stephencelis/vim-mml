dutyenve_table:
	dw	dutyenve_000
dutyenve_lp_table:
	dw	dutyenve_lp_000

dutyenve_000:
dutyenve_lp_000:
	db	$00,$00,$00,$00,$02,$02,$02,$02
	db	$ff

softenve_table:
	dw	softenve_000
	dw	softenve_001
	dw	softenve_002
	dw	softenve_003
	dw	softenve_004
	dw	softenve_005
	dw	softenve_006
	dw	softenve_007
	dw	softenve_008
	dw	softenve_009
softenve_lp_table:
	dw	softenve_lp_000
	dw	softenve_lp_001
	dw	softenve_lp_002
	dw	softenve_lp_003
	dw	softenve_lp_004
	dw	softenve_lp_005
	dw	softenve_lp_006
	dw	softenve_lp_007
	dw	softenve_lp_008
	dw	softenve_lp_009

softenve_000:
	db	$03,$02
softenve_lp_000:
	db	$01,$ff
softenve_001:
	db	$05
softenve_lp_001:
	db	$04,$ff
softenve_002:
	db	$04,$03,$03,$03,$03,$03,$03,$02
	db	$02,$02,$02,$02,$02,$02,$02,$01
	db	$01,$01,$01,$01,$01,$01,$01,$01
	db	$01
softenve_lp_002:
	db	$00,$ff
softenve_003:
	db	$00,$01,$01,$01,$01,$02,$02,$02
	db	$02,$03,$03,$03,$03,$05,$05,$05
	db	$06,$05
softenve_lp_003:
	db	$04,$ff
softenve_004:
	db	$06,$06,$06,$06,$05,$05,$05,$04
	db	$04,$03,$02,$01
softenve_lp_004:
	db	$00,$ff
softenve_005:
softenve_lp_005:
	db	$02,$01,$ff
softenve_006:
	db	$08,$05,$03,$01
softenve_lp_006:
	db	$00,$ff
softenve_007:
	db	$08,$06,$05,$04,$04,$03,$03,$03
	db	$02,$02,$02,$02,$01,$01,$01,$01
	db	$01
softenve_lp_007:
	db	$00,$ff
softenve_008:
	db	$0f,$0e,$0d,$0c,$0a,$09,$08,$08
	db	$07,$07,$07,$06,$06,$06,$05,$05
	db	$05,$04,$04,$04,$04,$03,$03,$03
	db	$03,$03,$02,$02,$02,$02,$02,$02
	db	$01,$01,$01,$01,$01,$01,$01
softenve_lp_008:
	db	$00,$ff
softenve_009:
	db	$03,$01
softenve_lp_009:
	db	$00,$ff

pitchenve_table:
	dw	pitchenve_000
	dw	pitchenve_001
pitchenve_lp_table:
	dw	pitchenve_lp_000
	dw	pitchenve_lp_001

pitchenve_000:
	db	$50,$8a,$8a,$8a,$8a,$8a,$8a,$8a
	db	$8a
pitchenve_lp_000:
	db	$00,$ff
pitchenve_001:
	db	$00
pitchenve_lp_001:
	db	$0a,$ff

arpeggio_table:
arpeggio_lp_table:


lfo_data:
	db	$08,$02,$06,$00
	db	$18,$03,$04,$00
	db	$18,$03,$02,$00

fds_data_table:
fds_effect_select:
fds_4088_data:


n106_channel:
	db	0
n106_wave_init:
n106_wave_table:


vrc7_data_table:


dpcm_data:

;DPCM samples

	.include	"/Users/shauninman/Desktop/mimeo/jaunt.h"
