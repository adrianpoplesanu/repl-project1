autocmd BufRead,BufNewFile *.ad set filetype=ad

if exists("b:current_syntax")
    finish
endif

hi BooleanTrue ctermfg=Green
hi BooleanFalse ctermfg=Red
hi LetReturn ctermfg=Magenta

syntax keyword adFunction fn
syntax keyword adKeyword if else
syntax keyword adLetReturn let return
syntax keyword adBooleanTrue true
syntax keyword adBooleanFalse false

highlight link adKeyword Keyword
highlight link adFunction Function
highlight link adLetReturn LetReturn
highlight link adBooleanTrue BooleanTrue
highlight link adBooleanFalse BooleanFalse

let b:current_syntax = "ad"
