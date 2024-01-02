autocmd BufRead,BufNewFile *.ad set filetype=ad

if exists("b:current_syntax")
    finish
endif

hi Boolean ctermfg=Red
hi Let ctermfg=Magenta
hi Comment ctermfg=DarkGray
hi FuncName ctermfg=Yellow

syntax match adComment "\v\/\*.*\*\/$"
syntax match lineComment "\v\/\/.*$"
highlight link adComment Comment
highlight link lineComment Comment
syntax region potionString start=/\v"/ skip=/\v\\./ end=/\v"/
" syntax region potionFuncName start=/\vdef/ skip=/\v\\./ end=/\v\(/

syntax keyword adFunction len print println func list
syntax keyword adKeyword if else return while class for
syntax keyword adLet let def function fun
syntax keyword adBoolean true false potionString

highlight link adKeyword Keyword
highlight link adFunction Function
highlight link adLet Let
highlight link adBoolean Boolean
highlight link potionString String
" highlight link potionFuncName FuncName


" Highlight Class and Function names
syn match    cCustomParen    "(" contains=cParen,cCppParen
syn match    cCustomFunc     "\w\+\s*(" contains=cCustomParen
syn match    cCustomScope    "::"
syn match    cCustomClass    "\w\+\s*::" contains=cCustomScope

hi def link cCustomFunc  FuncName
hi def link cCustomClass FuncName


let b:current_syntax = "ad"
