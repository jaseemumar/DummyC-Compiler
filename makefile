all: cc asm a.out
	
cc:  clean  lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih 
	./sedscript
	g++  -g --std=c++0x lex.cc parse.cc main.cc -o cc;

asm: code
	./cc < code

a.out: machine.cc code.asm
	g++ -g3 -O0 machine.cc        

lex.cc: lex.l Scanner.ih 
	rm -f Scanner.ih; 
	flexc++ lex.l; 
#	sed -i '/include/a #include "Parserbase.h"' Scanner.ih; 

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	rm -f Parser.h
	bisonc++  parse.y ;
#	sed -i 's/private:/\0\n\t int identifier=0;/' Parser.h
#	bisonc++   --construction -V parse.y; 
#	sed -i '/ifndef/a #include "tree_util.hh"' Parserbase.h;
#	sed -i '/ifndef/a #include "tree.hh"' Parserbase.h;
#	./sedscript
     
clean:
	rm -f *.h *.ih lex.cc parse.cc *~;
#	rm -f a.out
	
Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l
