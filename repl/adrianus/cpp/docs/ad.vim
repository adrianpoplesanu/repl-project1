autocmd BufRead,BufNewFile *.ad set filetype=ad

if exists("b:current_syntax")
    finish
endif

hi Boolean ctermfg=Red
hi Let ctermfg=Magenta
hi Comment ctermfg=DarkGray

syntax match adComment "\v\/\*.*\*\/$"
highlight link adComment Comment
syntax region potionString start=/\v"/ skip=/\v\\./ end=/\v"/

syntax keyword adFunction function len print
syntax keyword adKeyword if else return while class
syntax keyword adLet let, def
syntax keyword adBoolean true false potionString

highlight link adKeyword Keyword
highlight link adFunction Function
highlight link adLet Let
highlight link adBoolean Boolean
highlight link potionString String

let b:current_syntax = "ad"
