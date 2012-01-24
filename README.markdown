# mml.vim

Edit, compile, and play chiptune music written in the
[Music Macro Language][1-1] (see `:help mml` for more information).

As simple as `:MmlMake`. If you'd like to make it simpler, add the following
(or similar) to your `~/.vimrc`:

``` vim
autocmd FileType mml nnoremap <leader>m :MmlMake<cr>
```


## Installation

I recommend using either [pathogen.vim][2-1] or [Vundle][2-2].


### Pathogen

Once [pathogen.vim][2-1] is installed, simply run the following commands:

``` bash
% cd ~/.vim/bundle
% git clone git://github.com/stephencelis/vim-mml.git
```


### Vundle

If you're using [Vundle][2-2], add the following line to your `~/.vimrc`
(with the rest of your `Bundle` declarations):

``` vim
Bundle 'stephencelis/vim-mml'
```

Then, `:source $MYVIMRC` and `:BundleInstall`.


## Dependencies

Right now, automatically playing the compiled NES Sound Format (NSF) files
requires Mac OS X and either [Nestopia][3-1] or [Audio Overload][3-2].


## Getting Started

`vim secret.mml` and insert the following:

```
#TITLE Secret!

@v0 = { 8 12 15 14 12 10 8 6 }
@v1 = { 6 5 4 3 2 1 0 }

AB @t3,20 @2 @v0 l4 o5
A g d+ < g+ > @q9 @vr1 g+2^8
B r8 f+ < a > e > @q9 @vr1 c2
```

Then `:w`, `:MmlMake`, and...!


## Related Links

* [Shaun Inman][4-1] made a [TextMate Bundle][4-2] and mirrors an excellent
  [MML Reference][4-3].


## License

Copyright © 2012 Stephen Celis. Distributed under the same terms as Vim
itself (see `:help license`).


[1-1]: http://en.wikipedia.org/wiki/Music_Macro_Language
[2-1]: https://github.com/tpope/vim-pathogen
[2-2]: https://github.com/gmarik/vundle
[3-1]: http://bannister.org/software/nestopia.htm
[3-2]: http://bannister.org/software/ao.htm
[4-1]: http://shauninman.com
[4-2]: http://shauninman.com/archive/2010/02/13/an_mml_bundle_for_textmate
[4-3]: http://shauninman.com/assets/downloads/ppmck_guide.html
