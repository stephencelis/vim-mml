" Vim syntax file
" Language:    Music Macro Language (MML)
" Maintainer:  Stephen Celis <me@stephencelis.com>
" Filenames:   *.mml
" Last Change: 2012 January 21

if exists("b:current_syntax")
  finish
endif

" Comments:
"
syntax match   mmlComment          "\v(;.*|//.*)$"
syntax region  mmlComment          start="\v/\*" end="\v\*/"

" Compiler Instructions: E.g.,
"
"   #TITLE Super Mario Bros. Main Theme
"   #COMPOSER Koji Kondo
"   #MAKER 1985 Koji Kondo
"
syntax region  mmlCompilerLine     start="\v^#" end="\v$"
      \                            contains=mmlCompilerPreProc
syntax match   mmlCompilerPreProc  "\v^#[0-9A-Z-]*"
      \                            contained
      \                            contains=mmlCompilerError,mmlCompilerSpecial
      \                            nextgroup=mmlCompilerArgument
syntax match   mmlCompilerSpecial  "\vAUTO-BANKSWITCH" contained
syntax match   mmlCompilerSpecial  "\vBANK-CHANGE"     contained
syntax match   mmlCompilerSpecial  "\vCOMPOSER"        contained
syntax match   mmlCompilerSpecial  "\vDPCM-RESTSTOP"   contained
syntax match   mmlCompilerSpecial  "\vEFFECT-INCLUDE"  contained
syntax match   mmlCompilerSpecial  "\vEX-DISKFM"       contained
syntax match   mmlCompilerSpecial  "\vEX-NAMCO106"     contained
syntax match   mmlCompilerSpecial  "\vEX-VRC6"         contained
syntax match   mmlCompilerSpecial  "\vEX-VRC7"         contained
syntax match   mmlCompilerSpecial  "\vEX-MMC5"         contained
syntax match   mmlCompilerSpecial  "\vEX-FME7"         contained
syntax match   mmlCompilerSpecial  "\vMAKER"           contained
syntax match   mmlCompilerSpecial  "\vNO-BANKSWITCH"   contained
syntax match   mmlCompilerSpecial  "\vOCTAVE-REV"      contained
syntax match   mmlCompilerSpecial  "\vPROGRAMER"       contained
syntax match   mmlCompilerSpecial  "\vSETBANK"         contained
syntax match   mmlCompilerSpecial  "\vTITLE"           contained
syntax keyword mmlCompilerError    PROGRAMMER
      \                            contained
syntax match   mmlCompilerArgument "\v .{1,31}"
      \                            contained
      \                            nextgroup=mmlCompilerOverflow
syntax match   mmlCompilerOverflow "\v.*"
      \                            contained

" Macros:
"
syntax match   mmlMacroVariable
      \ "\v\@((\@|v|EP|EN|MP|DPCM|FM|N|MW|MH)?\d+|OP0)"
      \                            nextgroup=mmlMacroAssignment
syntax match   mmlMacroAssignment  "\v\="
syntax region  mmlMacroBlock       start="\v\{" end="\v\}"
      \                            contains=mmlMacroNumber,mmlMacroString
syntax match   mmlMacroNumber      "\v\$([0-9A-F]{2}|[0-9A-F]{4})|\d+"
      \                            contained
syntax region  mmlMacroString      start='\v"' end='\v"' skip='\v\\\"'
      \                            contained
syntax region  mmlMacroString      start="\v'" end="\v'" skip="\v\\\'"
      \                            contained

" Tracks:
"
syntax match   mmlTracks           "\v^[A-Zab]+"

" Tempo:
"
syntax match   mmlTempoLine        "\v^[A-Zab]+ \@?t\d{1,3}"
      \                            contains=mmlTempo,mmlTracks
      \                            transparent
syntax match   mmlTempo            "\v\@?t\d+"
      \                            contained

" Notes:
"
syntax match   mmlNote             "\v[a-g+-]+"
      \                            nextgroup=mmlNoteLength
syntax match   mmlNoteLength       "\v\d+"

" Types:
"
syntax match   mmlType
      \ "\v(r\d*|[Kkloqv]\d+|\@[nq]\d*|\@vr?\d*|EH\d+,\d+|(MP|SD)(\d|(OF?)))"
syntax match   mmlOperator         "\v[<>&^]"

" Loops:
"
syntax region  mmlLoop             start="\v\[" end="\v\]\d+"
      \                            transparent
syntax match   mmlLoopBracket      "\v\[|\]\d+"
syntax region  mmlLoop             start="\v\|:" end="\v:\|\d*"
      \                            transparent
syntax match   mmlLoopBracket      "\v\|:|:\|\d*"
syntax match   mmlLoopBracket      "\vL"

" Numbers:
"
syntax match   mmlNumber           "\v\$[0-9A-F]{4}"

" Highlighting:
"
highlight default link mmlComment          Comment
highlight default link mmlCompilerError    Error
highlight default link mmlCompilerOverflow Error
highlight default link mmlCompilerPreProc  PreProc
highlight default link mmlCompilerSpecial  Special
highlight default link mmlLength           Type
highlight default link mmlLoopBracket      Repeat
highlight default link mmlMacroAssignment  Operator
highlight default link mmlMacroNumber      Number
highlight default link mmlMacroString      String
highlight default link mmlMacroVariable    Identifier
highlight default link mmlNote             Character
highlight default link mmlNoteLength       Number
highlight default link mmlNumber           Number
highlight default link mmlOperator         Operator
highlight default link mmlTempo            Number
highlight default link mmlTracks           Constant
highlight default link mmlType             Type
highlight default link mmlVolume           Type

let b:current_syntax = "mml"
