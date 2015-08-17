#include <iostream>
#include "Scanner.h"
#include "Parser.h"
#include <fstream>
using namespace std;
int main (int argc, char** arg)
{
  Parser parser;
  parser.rstack.push("eax");
  parser.rstack.push("ebx");
  parser.rstack.push("ecx");
  parser.rstack.push("edx");
  parser.parse();
  list<gst_entry*>::iterator iter = parser.gst.begin();
  cout<<endl;

  bool contains_main= false;
  cout <<"Global Symbol Table" <<endl;
  cout << "-------------------------"<<endl;
  while (iter != parser.gst.end()) {
   	cout<<(*iter)->func_name<<"\t"<<(*iter)->ret_type<<"\t"<<(*iter)->lst<<endl;
	if((*iter)->func_name == "main") {
		contains_main = true;
		
   	 //cout <<"--------------------------"<<endl;


	}
   	iter++;
	//cout << "-------------------------"<<endl;
  }
  cout<<endl;
  if(!contains_main) {
	cout <<"Error: does not contain 'main' function"<<endl;
	}
  else{ 
  	ofstream output_file("code.asm");
    output_file<<parser.code.str();
  }

}


