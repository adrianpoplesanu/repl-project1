autocmd BufRead,BufNewFile *.ad set filetype=ad

if exists("b:current_syntax")
    finish
endif

hi Boolean ctermfg=Red
hi Let ctermfg=Magenta

syntax keyword adFunction fn len
syntax keyword adKeyword if else return
syntax keyword adLet let
syntax keyword adBoolean true false

highlight link adKeyword Keyword
highlight link adFunction Function
highlight link adLet Let
highlight link adBoolean Boolean

let b:current_syntax = "ad"
