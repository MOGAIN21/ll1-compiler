# cocoo-compiler

The front end of a compiler, written in C, for a small statically typed teaching language.
It has a hand-written lexer, an LL(1) predictive parser built from automatically computed
FIRST/FOLLOW sets, and a parse-tree printer.

## Features

- **Lexer:** a DFA-style scanner that turns source text into tokens (keywords, identifiers,
  numbers, operators). Comments start with `%` and are skipped.
- **Grammar to parse table:** computes FIRST and FOLLOW sets from the grammar in `utils.c`
  and builds an LL(1) parse table.
- **Predictive parser:** a stack-based parser that builds a full parse tree and recovers from
  errors in panic mode (it reports the line and keeps going).
- **Parse-tree output:** writes every node (lexeme, line, token, value, parent, leaf flag) to
  a file.

## Building

You need `gcc` and `make` (on Windows, MinGW's `mingw32-make` works).

```sh
make            # builds ./compiler (compiler.exe on Windows)
make clean
```

Or without make:

```sh
gcc -Wall -Wextra -g -o compiler driver.c Parser.c utils.c lexer.c
```

## Usage

```sh
./compiler <source-file> <parse-tree-output-file>
```

You'll then see a menu:

| Option | Action |
|---|---|
| 0 | Exit |
| 1 | Remove comments *(not implemented yet: a stub)* |
| 2 | Print the token list (line, token id, lexeme) |
| 3 | Parse the source and write the parse tree to the output file |
| 4 | Parse the source and print the time taken |

Example:

```sh
./compiler testcase4.txt out.txt
# choose 3 → "COMPILATION SUCCESS!" and the parse tree in out.txt
```

## The language at a glance

```text
% functions come first, then _main
_sumN  input parameter list [int d5cc34]
output parameter list [real d3];
    type int : b5b567;
    b5b567 <--- 1;
    d3 <--- 0.00;
    while (b5b567 <= d5cc34)
        read(c3bd);
        d3 <--- d3 + c3bd;
        b5b567 <--- b5b567 + 1;
    endwhile
    return [d3];
end

_main
    type int : b3;
    type real : c4bd56;
    b3 <--- 7;
    [c4bd56] <--- call _sumN with parameters [b3];
    write(c4bd56);
    return;
end
```

| Element | Form |
|---|---|
| Variable identifier | starts with `b`–`d`, then a digit `2`–`7` (e.g. `b3`, `d5cb34567`) |
| Function identifier | `_name` (the entry point is `_main`) |
| Record/union type name | `#name` |
| Field name | a lowercase word that isn't a keyword |
| Integer / real | `42` / `12.34`, `1.25E+03` (exactly two digits after the point) |
| Assignment | `<---` |
| Arithmetic | `+ - * /`, brackets `( )` |
| Relational | `< <= == > >= !=` |
| Logical | `&&&` (and), `@@@` (or), `~` (not) |
| Types | `int`, `real`, `record #r … endrecord`, `union #u … endunion`, `definetype record #a as #b` |
| Statements | assignment, `while … endwhile`, `if (…) then … else … endif`, `read(…)`, `write(…)`, `call … with parameters […]`, `return [ … ];` |
| Comment | `%` to end of line |

## Project layout

| File | Purpose |
|---|---|
| `driver.c` | Entry point and menu |
| `lexer.c` | Tokenizer (`getNextToken`) |
| `Parser.c` | FIRST/FOLLOW computation, parse table, parser, tree printing |
| `utils.c` | Grammar rules (`initializeGrammar`) and non-terminal names |
| `ParserHeader.h` | Token/non-terminal enums and core data structures |
| `utils.h` | Shared declarations |
| `Grammar.txt` | Grammar reference (note: `utils.c` is the authoritative version) |
| `testcase1.txt`, `testcase4.txt`, `t2.txt` | Valid sample programs |
| `t1.txt` | Lexer stress test with deliberate lexical errors (don't parse it) |
| `output.txt`, `parse_output.txt` | Sample parse-tree outputs |
| `string.c`, `string.h` | Unused helper (not part of the build) |

## Known limitations

- The parser stops after 10,000 steps, so large valid programs are reported as failed.
- Errors at the end of the file show line 1, because the lexer's line counter resets at EOF.
- Comment removal (menu option 1) is a stub.
- The token listing prints numeric token IDs rather than names.
- Parse-table conflicts aren't detected (the grammar is assumed to be LL(1)).
- Memory for the parse tree and the grammar is never freed.
