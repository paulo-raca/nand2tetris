// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

//While (1) {
(Loop)

//	fillValue=0
D=0
@fillValue
M=D

//	if (kbd) {
@KBD
D=M
@BeginFill
D;JEQ

//		fillValue=0xFFFF

D=-1
@fillValue
M=D

//	for (PTR=SCREEN; PTR<KBD; PTR++) {
(BeginFill)
//PTR=SCREEN
@SCREEN
D=A
@PTR
M=D

(LoopFill)
//	PTR<KBD
@PTR
D=M
@KBD
D=D-A
@Loop
D; JEQ

//		*PTR=fillValue
@fillValue
D=M
@PTR
A=M
M=D

//		PTR++
@PTR
M=M+1

//	} for (PTR=SCREEN; PTR<KBD; PTR++)
@LoopFill
0;JMP