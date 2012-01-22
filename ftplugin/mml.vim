" Vim filetype plugin
" Language:    MML
" Maintainer:  Stephen Celis <me@stephencelis.com>
" Last Change: 2012 January 21

if exists("b:loaded_ftplugin")
  finish
endif

let b:mml_base_dir = shellescape(expand('<sfile>:p:h:h'))
let b:mml_binaries_dir = b:mml_base_dir . '/bin/'
let b:mml_includes_dir = b:mml_base_dir . '/include/'

" Add `ppmckc' errorformat.
set errorformat+=Error\ %#:\ %#%f\ %#\ %l:\ %#%m

function! s:MmlMake()
  echo 'Building "' . expand('%:t') . '"...'

  " `bin/ppmckc -i $1`
  let s:filename = shellescape(expand('%:p'))
  cgetexpr system(b:mml_binaries_dir . 'ppmckc -i ' . s:filename)
  if v:shell_error ==# 1
    copen
    return
  endif

  " `NES_INCLUDE=include bin/asmnes -s -raw ppmck.asm`
  let s:include = 'NES_INCLUDE=' . b:mml_includes_dir
  let s:command = b:mml_binaries_dir . 'nesasm -s -raw ppmck.asm'
  caddexpr system(s:include . ' ' . s:command)

  " `mv ppmck.nes $(basename $1 .mml).nsf`
  let s:basename = shellescape(expand('%:p:r'))
  call system('mv ppmck.nes ' . s:basename . '.nsf')

  " `open $(basename $1 .mml).nsf`
  call system('[[ -x $(which open) ]] && open ' . s:basename . '.nsf')

  " `rm define.inc effect.h ppmck.sym $(basename $1 .mml).h`
  caddexpr system('rm define.inc effect.h ppmck.sym ' . s:basename . '.h')

  cclose
  redraw! " Clear message.
endfunction

command! -buffer MmlMake :call <SID>MmlMake()

let b:loaded_ftplugin = 1
