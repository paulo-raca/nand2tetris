// Define a grammar called HackAssembler
grammar HackAssembly;
@header {
    package nand2tetris.parser;
}
file: 
	line * ;
line: 
	command? ENDLINE ;
command:
	a_command | 
	c_command | 
	label_command ;
a_command:
	'@' name_or_number ; 
label_command:
	'(' name ')' ;
c_command:
	( c_dest '=' )? c_expr ( ';' c_jump )? ;
c_dest:
	name? ;
c_expr:
	name_or_number |
	operator name_or_number |
	name operator name_or_number ;
c_jump:
	name;
name_or_number:
	name |
	number ;
name:
	NAME ;
number:
	NUMBER ;
operator:
	OPERATOR ;

OPERATOR: 
	'+' | '-' | '&' | '|' | '!';
NAME : 
	[A-Za-z][_a-zA-Z0-9.$]* ;
NUMBER: 
	[0-9]+ ;
ENDLINE: 
	[\r\n]+;
WS: 
	[ \t]+ -> skip ;
BLOCK_COMMENT:
	'/*' .*? '*/' -> skip;
LINE_COMMENT:
	'//' ~[\r\n]* -> skip;
