# VM Parity Matrix

Legend:

- `Y` = implemented/covered
- `N` = missing
- `P` = partial

| Feature | AST Type | Evaluator | Compiler | VM Runtime | Parity Tests | Notes |
|---|---|---:|---:|---:|---:|---|
| Program / expression statements | `ST_PROGRAM`, `ST_EXPRESSION_STATEMENT` | Y | Y | Y | N | Baseline runner needed |
| Integers | `ST_INTEGER` | Y | Y | Y | P | Add differential fixtures |
| Floats | `ST_FLOAT` | Y | N | N | N | Core phase gap |
| Booleans | `ST_BOOLEAN` | Y | Y | Y | P | Add edge-case fixtures |
| Null | `ST_NULL_EXPRESSION` | Y | Y | Y | P | Verify truthiness parity |
| Prefix ops (`!`, `-`) | `ST_PREFIX_EXPRESSION` | Y | Y | Y | P | Check mixed numeric behavior |
| Infix ops | `ST_INFIX_EXPRESSION` | Y | Y | Y | P | Check evaluator-vm exact semantics |
| If/else | `ST_IF_EXPRESSION` | Y | Y | Y | P | Jump semantics present |
| While | `ST_WHILE_EXPRESSION` | Y | P | P | N | Compiler emits special-case but parity unknown |
| For | `ST_FOR_EXPRESSION` | Y | N | N | N | Core phase gap |
| Break / continue | `ST_BREAK_STATEMENT`, `ST_CONTINUE_STATEMENT` | Y | N | N | N | Core phase gap |
| Let bindings | `ST_LET_STATEMENT` | Y | Y | Y | P | Add differential fixtures |
| Assignment (`=`) | `ST_ASSIGN_STATEMENT` | Y | Y | P | N | Member/index assignment parity missing |
| Plus/minus equals | `ST_PLUS_EQUALS`, `ST_MINUS_EQUALS` | Y | N | N | N | Core phase gap |
| Strings | `ST_STRING_LITERAL` | Y | Y | Y | P | Add concatenation/index checks |
| Lists | `ST_LIST_LITERAL` | Y | Y | Y | P | Add nested structures checks |
| Hashes | `ST_HASH_LITERAL` | Y | Y | Y | P | Add key/error edge tests |
| Index access | `ST_INDEX_EXPRESSION` | Y | Y | Y | P | Slicing variants still missing |
| Function literals | `ST_FUNCTION_LITERAL` | Y | Y | Y | P | Need recursive/free-var parity cases |
| Calls | `ST_CALL_EXPRESSION` | Y | Y | Y | P | Builtin and arity parity needed |
| Return | `ST_RETURN_STATEMENT` | Y | Y | Y | P | Confirm nested control-flow parity |
| Def statements | `ST_DEF_STATEMENT` | Y | Y | Y | P | Validate name binding recursion |
| Classes | `ST_CLASS_STATEMENT` | Y | N | P | N | OOP phase major gap |
| Member access | `ST_MEMBER_ACCESS` | Y | N | P | N | OOP phase major gap |
| `this` / `super` | `ST_THIS_EXPRESSION`, `ST_SUPER_EXPRESSION` | Y | N | N | N | OOP phase gap |
| Spawn / await | `ST_SPAWN_EXPRESSION`, `ST_AWAIT_EXPRESSION` | Y | N | N | N | Async phase gap |

## Near-Term Updates

As implementation progresses:

1. Update `Compiler` and `VM Runtime` columns first.
2. Add at least 2 parity fixtures per completed row.
3. Move `Parity Tests` to `Y` only when runner is green in CI.
