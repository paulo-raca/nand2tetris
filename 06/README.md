Nand 2 Tetris - Hack Assembler
==============================

My implementation of a Hack Assembler is way more complicated than it had to be.

But I decided it was a good opportunity to finally learn about [ANTLR](http://www.antlr.org/).

The file [HackAssembly.g4](assembler/src/nand2tetris/parser/HackAssembly.g4) defines the (not very strict) grammar, and [HackAssembler.java](assembler/src/nand2tetris/parser/HackAssembler.java) uses callbacks to visit the tree and translate it into the binary instructions.

Unfortunately, IMO it is a pain in the to hardcode Maps in Java, so the builtin symbols, C-destinations, C-expressions and C-jumps are stored in Properties files and readed back through the [SymbolMap.java](assembler/src/nand2tetris/parser/SymbolMap.java) class.
