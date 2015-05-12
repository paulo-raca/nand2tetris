// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

//Pseudo-code:

//A=R0
//B=R1
//mask=1
//while (A) {
//  if (mask & A) {
//    ret += B
//    A = A & ~mask
//  }
//  B *= 2
//  mask *= 2
//}
//R2=ret

//ret=0
@ret
M=0

//A=R0
@R0
D=M
@A
M=D

//B=R1
@R1
D=M
@B
M=D

//mask=1
@mask
M=1

//while (A) {
(Loop)
@A
D=M
@EndLoop
D;JEQ

//	if (mask & A) {
@A
D=M
@mask
D=M&D
@skipSum
D;JEQ

//		ret += B
@B
D=M
@ret
M=M+D

//		A = A & ~mask
@mask
D=!M
@A
M=M&D

//	B *= 2
(skipSum)
@B
D=M
M=D+M


//	mask *= 2
@mask
D=M
M=D+M

//} while (mask <= A) {
@Loop
0;JMP

//R2 = ret
(EndLoop)
@ret
D=M
@R2
M=D

(End)
@End
0;JMP
