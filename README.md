# omc65
 Our Macro Crossassembler for 6502

Mac/65 compatible 6502 crossassembler

omc54 was originally coded for Atari ST in PURE-C around 1990-1991.
Later it was ported to win32.
It was used in several commercial games and a few demoscene prods.

The crossassembler has been modernized to work with the current C and C++ compilers and reformatted with clang-format-11 (LLVM style).

Compilation:
```gcc --x c OMC_MAIN.C  -o omc
gcc --std=gnu11 --x c OMC_MAIN.C  -o omc

g++ OMC_MAIN.C  -o omc
g++ --std=c++17 OMC_MAIN.C -o omc
g++ --std=c++13 OMC_MAIN.C -o omc
g++ --std=c++14 OMC_MAIN.C -o omc
g++ --std=c++11 OMC_MAIN.C -o omc
g++ --std=c++03 OMC_MAIN.C -o omc
g++ --std=c++98 OMC_MAIN.C -o omc
```

TODO

 - [ ] Translate Polish variable names to English
 - [x] Compile under Linux
 - [ ] Make output files compatible with Altirra debugger
 - [ ] Develop test suite
 - [ ] Fix known bugs
