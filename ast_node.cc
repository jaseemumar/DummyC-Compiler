#include<string>
#include<list>
using namespace std;
long long nextLabel=0;
string BinaryOpStrs[]={
	"Or",
	"And",
	"EQ_OP",
	"NE_OP",
	"LT_FLOAT",
	"LT_INT",
	"GT_FLOAT",
	"GT_INT",
	"LE_FLOAT",
	"LE_INT",
	"GE_FLOAT",
	"GE_INT",
	"Plus_Float",
	"Plus_Int",
	"Minus_Int",
	"Minus_Float",
	"Mult",
	"Div",
	"Assign"
};
string UnaryOpStrs[]={
	"UMinus", "Not","PP", "TO_FLOAT", "TO_INT"
};

const int reg_count= 4;
int find_width(string type);

void swap_top(stack<string> &rstack) {
	if (rstack.empty()) {
		return;
	} else {
		string top = rstack.top();
		rstack.pop();
		if (rstack.empty()) {
			rstack.push(top);
			return;
		} else {
			string top2 = rstack.top();
			rstack.pop();
			rstack.push(top);
			rstack.push(top2);
		}
	}
}

// arrayref_astnode::arrayref_astnode(){}
// exp_astnode::exp_astnode(){};
// 		exp_astnode::~exp_astnode(){};
void block_astnode::print(){
	cout<<"(Block [";
		for(list<stmt_astnode*>::iterator iter=stmt_list->begin();iter!=stmt_list->end();iter++){
			(*iter)->print();
			cout<<" ";
		}
		cout<<"])";
}

string block_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	string code = "";
	for(list<stmt_astnode*>::iterator iter=stmt_list->begin();iter!=stmt_list->end();iter++){
		code += (*iter)->generate_code(rstack, lst,gst);
	}
	return code;
}

block_astnode::block_astnode(list<stmt_astnode*> * _stmt_list){
	stmt_list =_stmt_list;
}

ass_astnode::ass_astnode(exp_astnode *_lhs, exp_astnode* _rhs){
	lhs=_lhs;
	rhs=_rhs;
}

//string ass_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
string ass_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	
	stringstream code;
	vector<string> regs;
	
	
	bool stack_used = false;
	if (rstack.empty()) {
		cout<<"Stack is dry"<<endl;
	} else if (lhs->label >= rhs->label) {
		string r1 , r2;
		
		if(rhs->label >= reg_count) { // Need to push in the stack
			code<<"exceeded reg count"<<endl;
			code<< lhs->generate_code(rstack, lst,gst);
			if(lhs->type == "float") {
				code <<"pushf("+rstack.top()+");\n";
			} else {
				code <<"pushi("+rstack.top()+");\n";
			}
			stack_used = true;
		} else {
			//cout<<"LOL What ?"<<lhs->label<<" " <<rhs->label<<endl;
			swap_top(rstack);
			code<< lhs->generate_code(rstack, lst,gst);
			 //As left node evaluates to offset, keep the result of right on top
			r1 = rstack.top();
			rstack.pop();
		}

		
		code<<rhs->generate_code(rstack, lst,gst);
		
		r2 = rstack.top(); // rhs value in r2
		
		if (stack_used) {
			rstack.pop(); // rhs value in r2
			if(lhs->type == "int"){
				code << "loadi(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg
				//code<<"addi(ebp, "<<rstack.top()<<");"<<endl;
				code<<"storei("<<r2<<", ind(ebp,"<<rstack.top()<<"));"<<endl;
			} else {
				code << "loadf(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg
				//code<<"addf(ebp, "<<rstack.top()<<");"<<endl;
				code<<"storef("<<r2<<", ind(ebp,"<<rstack.top()<<"));"<<endl;
			}
			rstack.push(r2);
		} else {
			if(lhs->type == "int"){
				//code<<"addi(ebp, "<<r1<<");"<<endl;
				code<<"storei("<<r2<<", ind(ebp,"<<r1<<"));"<<endl;
				rstack.push(r1);
				swap_top(rstack);
			} else {
				//code<<"addf(ebp, "<<r1<<");"<<endl;
				code<<"storef("<<r2<<", ind(ebp,"<<r1<<"));"<<endl;
				rstack.push(r1);
				swap_top(rstack);
			}
		}
	} else {
		string r1, r2;
		if( lhs->label >= reg_count) { // Need to push in the stack
			swap_top(rstack);
			code << rhs->generate_code(rstack, lst,gst);
			if(lhs->type == "float"	) {
				code <<"pushf("+rstack.top()+");\n";
			} else {
				code <<"pushi("+rstack.top()+");\n";
			}
			stack_used = true;
		} else {
			code << rhs->generate_code(rstack, lst,gst);
			r2 = rstack.top();
			rstack.pop();
		}
		
		code << lhs->generate_code(rstack, lst,gst);
		r1 = rstack.top();
		if (stack_used) {
			rstack.pop(); // lhs value in r1
			if(lhs->type == "int") {
				code << "loadi(ind(esp), "+rstack.top()+");\n";
				code<< "popi(1);\n";
				//code<<"addi(ebp, "<<r1<<");"<<endl;
				code<<"storei("<<rstack.top()<<", ind(ebp,"<<r1<<"));"<<endl;
				rstack.push(r1);
				swap_top(rstack);
			} else {
				code << "loadf(ind(esp), "+rstack.top()+");\n";
				code<< "popf(1);\n";
				//code<<"addi(ebp, "<<r1<<");"<<endl;
				code<<"storef("<<rstack.top()<<", ind(ebp,"<<r1<<"));"<<endl;
				rstack.push(r1);
				swap_top(rstack);
			}
		} else {
			//code<<"addi(ebp , "<<r1<<");"<<endl;
			code<<"storef("<<r2<<", ind(ebp,"<<r1<<"));"<<endl;
			rstack.push(r2);
		}
	}
	
	//cout<<code.str()<<endl;
	return code.str() ;
}

void ass_astnode::print(){
	cout<<"(Ass ";
		lhs->print();
		cout <<" ";
		rhs->print();
		cout<<")";
}

return_astnode::return_astnode(exp_astnode * _return_value){
	return_value=_return_value;
}
void return_astnode::print(){
	cout<<"(Return ";
		return_value->print();
		cout<<")";
}

string return_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;
	code<<return_value->generate_code(rstack,lst,gst);
	string top_reg=rstack.top();
	string ret_type=return_value->type;
	int param_count=0;;
	for(list<lst_entry*>::iterator lst_it=lst->begin();lst_it!=lst->end();lst_it++){
		if(((*lst_it)->offset)>0) param_count++;
	}
	if(ret_type=="int")
		code<<"storei("<<top_reg<<", ind(ebp, "<<(param_count+1)*4<<"));"<<endl;
	else if (ret_type=="float")
		code<<"storef("<<top_reg<<", ind(ebp, "<<(param_count+1)*4<<"));"<<endl;
	//cout<<code.str();
	return code.str();
}

if_astnode::if_astnode(exp_astnode* _predicate, stmt_astnode* _if_body,stmt_astnode* _else_body){
	predicate=_predicate;
	if_body=_if_body;
	else_body=_else_body;
}
void if_astnode::print(){
	cout<<"(If ";
		predicate->print();
		if_body->print();
		else_body->print();
		cout<<")";

}

string if_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;
	int else_label=nextLabel++;
	int next_code_label=nextLabel++;
	code<<predicate->generate_code(rstack, lst,gst);
	string top_reg=rstack.top();
	code<<"cmpi(0,"<<top_reg<<");"<<endl;
	code<<"je(L_"<<else_label<<");"<<endl;
	code<<if_body->generate_code(rstack, lst,gst);
	code<<"j(L_"<<next_code_label<<");"<<endl;
	code<<"L_"<<else_label<<":"<<endl;
	
		
	code<<else_body->generate_code(rstack, lst,gst);
	
	code<<"L_"<<next_code_label<<":"<<endl;
	
			
	//cout<<code.str();
	return code.str();
}

while_astnode::while_astnode(exp_astnode *_guard, stmt_astnode* _body){
	guard=_guard;
	body=_body;
}

void while_astnode::print(){
	cout<<"(While ";
		guard->print();
		body->print();
		cout<<")";
}

string while_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;
	int loop_label=nextLabel++;
	int next_code_label=nextLabel++;

	code<<"L_"<<loop_label<<":"<<endl;
	code<<guard->generate_code(rstack, lst,gst);
	string top_reg=rstack.top();
	code<<"cmpi(0,"<<top_reg<<");"<<endl;
	code<<"je(L_"<<next_code_label<<");"<<endl;
	code<<body->generate_code(rstack, lst,gst);
	code<<"j(L_"<<loop_label<<");"<<endl;
	code<<"L_"<<next_code_label<<":"<<endl;
	//cout<<code.str();
	return code.str();
}


for_astnode::for_astnode(exp_astnode* _initialiser, exp_astnode* _guard, exp_astnode* _step, stmt_astnode* _body){
	initialiser=_initialiser;
	guard=_guard;
	step=_step;
	body=_body;
}

void for_astnode::print(){
	cout<<"(For";
		initialiser->print();
		guard->print();
		step->print();
		body->print();
		cout<<")";
}

string for_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;

	int loop_label=nextLabel++;
	int next_code_label=nextLabel++;

	
	code<<initialiser->generate_code(rstack, lst,gst);
	
	

	code<<"L_"<<loop_label<<":"<<endl;
	code<<guard->generate_code(rstack, lst,gst);
	string top_reg=rstack.top();
	code<<"cmpi(0,"<<top_reg<<");"<<endl;
	code<<"je(L_"<<next_code_label<<");"<<endl;
	code<<body->generate_code(rstack, lst,gst);
	code<<step->generate_code(rstack, lst,gst);
	code<<"j(L_"<<loop_label<<");"<<endl;
	code<<"L_"<<next_code_label<<":"<<endl;
	//cout<<code.str();
	return code.str();

}

void empty_astnode::print(){
	cout<<"(Empty)";
}

string empty_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	return "";
}

binaryop_astnode::binaryop_astnode(exp_astnode* _lhs, exp_astnode* _rhs, BinaryOp _op){
	lhs=_lhs;
	rhs=_rhs;
	op=_op;
	if(op==OR||op==AND){
		label= max(lhs->label, rhs->label);
	}
	else if (lhs->label == rhs->label) {
		label = lhs->label+1;
	} else {
		label = max(lhs->label, rhs->label);
	}
}

void binaryop_astnode::print(){
	cout<<"("<<BinaryOpStrs[op]<<" ";
		lhs->print();
		rhs->print();
		cout<<" )";
}

string binaryop_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;
	
	bool stack_used = false;

	if (rstack.empty()) {
		cout<<"Stack is dry"<<endl;
	} else if(op==OR) {
		long long success_label=nextLabel++;
		long long next_code_label=nextLabel++;
		 // Need to push in the stack
		code<< lhs->generate_code(rstack, lst,gst);

		// if lhs returned 1 go to success_label
		if(lhs->type == "float") 
			code<<"cmpf(0,"<<rstack.top()<<");"<<endl;
		else if(lhs->type == "int")
			code<<"cmpi(0,"<<rstack.top()<<");"<<endl;
		code<<"jne(L_"<<success_label<<");"<<endl;

		//lhs didn't return 1. evaluate rhs
		code<<rhs->generate_code(rstack, lst,gst);

		//if lhs returned 0 go to success_label
		if(rhs->type == "float") 
			code<<"cmpf(0,"<<rstack.top()<<");"<<endl;
		else if(lhs->type == "int")
			code<<"cmpi(0,"<<rstack.top()<<");"<<endl;

		code<<"jne(L_"<<success_label<<");"<<endl;
		code<<"move(0,"<<rstack.top()<<");"<<endl; //fail: set value of expression to 0
		code<<"j(L_"<<next_code_label<<");"<<endl;

		code<<"L_"<<success_label<<":"<<endl;
		code<<"move(1,"<<rstack.top()<<");"<<endl; //success: set value of expression to 1

		code<<"L_"<<next_code_label<<":"<<endl;
	} else if(op==AND){
		long long fail_label=nextLabel++;
		long long next_code_label=nextLabel++;
		code<< lhs->generate_code(rstack, lst,gst);

		// if lhs evaluated to 0, go to fail_label
		if(lhs->type == "float") 
			code<<"cmpf(0,"<<rstack.top()<<");"<<endl;
		else if(lhs->type == "int")
			code<<"cmpi(0,"<<rstack.top()<<");"<<endl;

		code<<"je(L_"<<fail_label<<");"<<endl;


		code<<rhs->generate_code(rstack, lst,gst);

		//if rhs evaluated to 0 go to fail_label
		if(lhs->type == "float") 
			code<<"cmpf(0,"<<rstack.top()<<");"<<endl;
		else if(lhs->type == "int")
			code<<"cmpi(0,"<<rstack.top()<<");"<<endl;

		code<<"je(L_"<<fail_label<<");"<<endl;

		code<<"move(1,"<<rstack.top()<<");"<<endl;
		code<<"j(L_"<<next_code_label<<");"<<endl; //success: set value of expression to 1
		code<<"L_"<<fail_label<<":"<<endl;
		code<<"move(0,"<<rstack.top()<<");"<<endl; //fail: set value of expression to 0
		code<<"L_"<<next_code_label<<":"<<endl;
	} else if (lhs->label >= rhs->label) {
		
		string r1 , r2;
		
		if(rhs->label >= reg_count) { // Need to push in the stack

			code<< lhs->generate_code(rstack, lst,gst);
			if(lhs->type == "float") {
				code <<"pushf("+rstack.top()+");\n";
			} else {
				code <<"pushi("+rstack.top()+");\n";
			}
			stack_used = true;
			
			if (op != ASSIGN) {
				swap_top(rstack); // To account for loading in again which has to be on top
			}
		} else {
			
			code<< lhs->generate_code(rstack, lst,gst);

			r1 = rstack.top();
			

			rstack.pop();

		}
		code<<rhs->generate_code(rstack, lst,gst);
		r2 = rstack.top();
		
		if (op == PLUS_FLOAT || op == MULT_FLOAT || op == MINUS_FLOAT || op == DIV_FLOAT) {
			if(stack_used) {
				rstack.pop(); // rhs value in r2
				if(lhs->type == "float") {
					code << "loadf(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg
					code<< "popf(1);\n";
					if (op == PLUS_FLOAT) {
						code << "addf("+r2+", "+rstack.top()+");\n";
					} else if (op == MULT_FLOAT) {
						code << "mulf("+r2+", "+rstack.top()+");\n";
					} else if (op == MINUS_FLOAT) {
						code<< "mulf(-1.0, "+r2+");\n";
						code << "addf("+r2+", "+rstack.top()+");\n";
					} else if (op == DIV_FLOAT) {
						code << "divf("+r2+", "+rstack.top()+");\n";
					}
					rstack.push(r2);
					swap_top(rstack); /// value on top again and restoring order of regs
				} else {
					cout<<"Impossible print"<<endl;
				}
			} else {

				if ( op == PLUS_FLOAT) {
					code<< "addf("+r2+", "+r1+");\n";
				} else if (op == MULT_FLOAT) {
					code<< "mulf("+r2+", "+r1+");\n";
				} else if (op == MINUS_FLOAT) {
					code<< "mulf(-1.0, "+r2+");\n";
					code<< "addf("+r2+", "+r1+");\n";
				} else if (op == DIV_FLOAT) {
					code << "divf("+r2+", "+r1+");\n";
				}
				rstack.push(r1);
			}
		} else if (op == PLUS_INT || op == MULT_INT || op == MINUS_INT || op == DIV_INT) {

			if(stack_used) {
				rstack.pop(); // rhs value in r2
				if(lhs->type == "int") {
					code << "loadi(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg
					code<< "popi(1);\n";
					if (op == PLUS_FLOAT) {
						code << "addi("+r2+", "+rstack.top()+");\n";
					} else if (op == MULT_FLOAT) {
						code << "muli("+r2+", "+rstack.top()+");\n";
					} else if (op == MINUS_FLOAT) {
						code<< "muli(-1.0, "+r2+");\n";
						code << "addi("+r2+", "+rstack.top()+");\n";
					} else if (op == DIV_FLOAT) {
						code << "divi"+r2+", "+rstack.top()+");\n";
					}
					rstack.push(r2);
					swap_top(rstack); /// value on top again and restoring order of regs
				} else {
					cout<<"Impossible print"<<endl;
				}
			} else {
				if (op == PLUS_INT) {
					code<< "addi("+r2+", "+r1+");\n";
				} else if (op == MULT_INT) {
					code<< "muli("+r2+", "+r1+");\n";
				} else if (op == MINUS_INT) {
					code<< "muli(-1.0, "+r2+");\n";
					code<< "addi("+r2+", "+r1+");\n";
				} else if (op == DIV_INT) {
					code << "divi("+r2+", "+r1+");\n";
				}
				rstack.push(r1);
			}
			
		} else if (op == ASSIGN) {
			if (stack_used) {
				rstack.pop(); // rhs value in r2
				if(lhs->type == "int"){
					code << "loadi(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg
					//code<<"addi(ebp, "<<rstack.top()<<");"<<endl;
					code<<"storei("<<r2<<", ind(ebp,"<<rstack.top()<<"));"<<endl;
				} else {
					code << "loadi(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg
					//code<<"addf( ebp"<<rstack.top()<<");"<<endl;
					code<<"storef("<<r2<<", ind(ebp,"<<rstack.top()<<"));"<<endl;
				}
				rstack.push(r2);
			} else {
				if(lhs->type == "int"){
					//code<<"addi(ebp," <<r1<<");"<<endl;
					code<<"storei("<<r2<<", ind(ebp,"<<r1<<"));"<<endl;
					rstack.push(r1);
					swap_top(rstack);
				} else {
					//code<<"addf(ebp, "<<r1<<");"<<endl;
					code<<"storef("<<r2<<", ind(ebp,"<<r1<<"));"<<endl;
					rstack.push(r1);
					swap_top(rstack);
				}
			}
		} else if (op ==LT_FLOAT||op == GT_FLOAT||op == LE_FLOAT||op == GE_FLOAT){
			if(stack_used) {
				rstack.pop(); // rhs value in r2
				if(lhs->type == "float") {
					code << "loadf(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg
					code<< "popf(1);\n";
					int true_label=nextLabel++;
					int next_code_label=nextLabel++;
					code << "cmpf("+r2+", "+rstack.top()+");\n";
					if (op == LT_FLOAT) {
						code << "jl(L_"<<true_label<<");"<<endl;
					} else if (op == GT_FLOAT) {
						code << "jg(L_"<<true_label<<");"<<endl;
					} else if(op==LE_FLOAT){
						code << "jle(L_"<<true_label<<");"<<endl;
					} else if(op==GE_FLOAT){
						code << "jge(L_"<<true_label<<");"<<endl;
					}
					code << "move(0,"<<rstack.top()<<");"<<endl;
					code<<"j(L_"<<next_code_label<<");"<<endl;
					code << "L_"<<true_label<<":"<<endl;
					code << "move(1,"<<rstack.top()<<");"<<endl;
					code << "L_"<<next_code_label<<":"<<endl;
					
					rstack.push(r2);
					swap_top(rstack); /// value on top again and restoring order of regs
				} else {
					cout<<"Impossible print"<<endl;
				}
			} else {
				int true_label=nextLabel++;
				int next_code_label=nextLabel++;
				code << "cmpf("+r2+", "+r1+");\n";
				if (op == LT_FLOAT) {
					code << "jl(L_"<<true_label<<");"<<endl;
				} else if (op == GT_FLOAT) {
					code << "jg(L_"<<true_label<<");"<<endl;
				} else if(op==LE_FLOAT){
					code << "jle(L_"<<true_label<<");"<<endl;
				} else if(op==GE_FLOAT){
					code << "jge(L_"<<true_label<<");"<<endl;
				}
				code << "move(0,"<<r1<<");"<<endl;
				code<<"j(L_"<<next_code_label<<");"<<endl;
				code << "L_"<<true_label<<":"<<endl;
				code << "move(1,"<<r1<<");"<<endl;
				code << "L_"<<next_code_label<<":"<<endl;

				rstack.push(r1);
			}
		} else if (op ==LT_INT||op == GT_INT||op == LE_INT||op == GE_INT){
			if(stack_used) {
				rstack.pop(); // rhs value in r2
				if(lhs->type == "int") {
					code << "loadi(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg
					code<< "popi(1);\n";
					int true_label=nextLabel++;
					int next_code_label=nextLabel++;
					code << "cmpi("+r2+", "+rstack.top()+");\n";
					if (op == LT_INT) {
						code << "jl(L_"<<true_label<<");"<<endl;
					} else if (op == GT_INT) {
						code << "jg(L_"<<true_label<<");"<<endl;
					} else if(op==LE_INT){
						code << "jle(L_"<<true_label<<");"<<endl;
					} else if(op==GE_INT){
						code << "jge(L_"<<true_label<<");"<<endl;
					}
					code << "move(0,"<<rstack.top()<<");"<<endl;
					code<<"j(L_"<<next_code_label<<");"<<endl;
					code << "L_"<<true_label<<":"<<endl;
					code << "move(1,"<<rstack.top()<<");"<<endl;
					code << "L_"<<next_code_label<<":"<<endl;
					
					rstack.push(r2);
					swap_top(rstack); /// value on top again and restoring order of regs
				} else {
					cout<<"Impossible print"<<endl;
				}
			} else {
				int true_label=nextLabel++;
				int next_code_label=nextLabel++;
				code << "cmpi("+r1+", "+r2+");\n";
				if (op == LT_INT) {
					code << "jl(L_"<<true_label<<");"<<endl;
				} else if (op == GT_INT) {
					code << "jg(L_"<<true_label<<");"<<endl;
				} else if(op==LE_INT){
					code << "jle(L_"<<true_label<<");"<<endl;
				} else if(op==GE_INT){
					code << "jge(L_"<<true_label<<");"<<endl;
				}
				code << "move(0,"<<r1<<");"<<endl;
				code<<"j(L_"<<next_code_label<<");"<<endl;
				code << "L_"<<true_label<<":"<<endl;
				code << "move(1,"<<r1<<");"<<endl;
				code << "L_"<<next_code_label<<":"<<endl;
				rstack.push(r1);
			}

		}
	} else if (lhs->label < rhs->label) {
		
		string r1, r2;
		if( lhs->label >= reg_count) { // Need to push in the stack
			if(op == ASSIGN) {
				swap_top(rstack);
			}
			code << rhs->generate_code(rstack, lst,gst);
			if(lhs->type == "float"	) {
				code <<"pushf("+rstack.top()+");\n";
			} else {
				code <<"pushi("+rstack.top()+");\n";
			}
			stack_used = true;
		} else {
			if(op != ASSIGN) {

				swap_top(rstack);
			}

			code << rhs->generate_code(rstack, lst,gst);
			r2 = rstack.top();
			rstack.pop();
		}
		
		code << lhs->generate_code(rstack, lst,gst);
		r1 = rstack.top();

		if(op == PLUS_FLOAT || op == MINUS_FLOAT || op == MULT_FLOAT || op == DIV_FLOAT) {
			if(stack_used) {
				if(rhs->type == "float") {
					rstack.pop(); // lhs value in r1
					code << "loadf(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg	
					code<< "popf(1);\n";
					if (op == PLUS_FLOAT) {
						code << "addf("+rstack.top()+", "+r1+");\n";
					} else if (op == MINUS_FLOAT) {
						code<< "mulf(-1.0, "+rstack.top()+");\n";
						code << "addf("+rstack.top()+", "+r1+");\n";
					} else if (op == MULT_FLOAT) {
						code << "mulf("+rstack.top()+", "+r1+");\n";
					} else if (op == DIV_FLOAT) {
						code << "divf("+rstack.top()+", "+r1+");\n";
					}

					rstack.push(r1);
				} else {
					cout<<"Impossible print"<<endl;
				}
			} else {
				if (op == PLUS_FLOAT) {
					code << "addf("+r2+", "+r1+");\n";
				} else if (op == MINUS_FLOAT) {
					code<< "mulf(-1.0, "+r2+");\n";
					code << "addf("+r2+", "+r1+");\n";
				} else if (op == MULT_FLOAT) {
					code << "mulf("+r2+", "+r1+");\n";
				} else if (op == DIV_FLOAT) {
					code << "divf("+r2+", "+r1+");\n";
				}
				rstack.push(r2);
				swap_top(rstack);
			}	


		} else if (op == PLUS_INT || op == MINUS_INT || op == MULT_INT || op == DIV_INT) {
			

			if(stack_used) {
				if(rhs->type == "int") {
					rstack.pop(); // lhs value in r1
					code << "loadi(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg	
					code<< "popi(1);\n";
					if (op == PLUS_FLOAT) {
						code << "addi("+rstack.top()+", "+r1+");\n";
					} else if (op == MINUS_FLOAT) {
						code<< "muli(-1.0, "+rstack.top()+");\n";
						code << "addi("+rstack.top()+", "+r1+");\n";
					} else if (op == MULT_FLOAT) {
						code << "muli("+rstack.top()+", "+r1+");\n";
					} else if (op == DIV_FLOAT) {
						code << "divi("+rstack.top()+", "+r1+");\n";
					}
					rstack.push(r1);
				} else {
					cout<<"Impossible print"<<endl;
				}
			} else {
				if (op == PLUS_INT) {
					code << "addi("+r2+", "+r1+");\n";
				} else if (op == MINUS_INT) {
					code<< "muli(-1.0, "+r2+");\n";
					code << "addi("+r2+", "+r1+");\n";
				} else if (op == MULT_INT) {
					code << "muli("+r2+", "+r1+");\n";					
				} else if (op == DIV_INT) {
					code << "divi("+r2+", "+r1+");\n";
				}
				rstack.push(r2);
				swap_top(rstack);
			}
			
		} else if (op == ASSIGN) {
			if (stack_used) {
				rstack.pop(); // lhs value in r1
				if(lhs->type == "int") {
					code << "loadi(ind(esp), "+rstack.top()+");\n";
					code<< "popi(1);\n";
					//code<<"addi(ebp, "<<r1<<");"<<endl;
					code<<"storei("<<rstack.top()<<", ind(ebp,"<<r1<<"));"<<endl;
					rstack.push(r1);
					swap_top(rstack);
				} else {
					code << "loadf(ind(esp), "+rstack.top()+");\n";
					code<< "popf(1);\n";
					//code<<"addi(ebp, "<<r1<<");"<<endl;
					code<<"storef("<<rstack.top()<<", ind(ebp,"<<r1<<"));"<<endl;
					rstack.push(r1);
					swap_top(rstack);
				}
			} else {
				//code<<"addi(ebp, "<<r1<<");"<<endl;
				code<<"storef("<<r2<<", ind(ebp,"<<r1<<"));"<<endl;
				rstack.push(r2);
			}
		} else if (op ==LT_FLOAT||op == GT_FLOAT||op == LE_FLOAT||op == GE_FLOAT){
			if(stack_used) {
				if(rhs->type == "float") {
					rstack.pop(); // lhs value in r1
					code << "loadf(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg	
					code<< "popf(1);\n";
					int true_label=nextLabel++;
					int next_code_label=nextLabel++;
					code << "cmpf("+r1+", "+rstack.top()+");\n";
					if (op == LT_FLOAT) {
						code << "jl(L_"<<true_label<<");"<<endl;
					} else if (op == GT_FLOAT) {
						code << "jg(L_"<<true_label<<");"<<endl;
					} else if(op==LE_FLOAT){
						code << "jle(L_"<<true_label<<");"<<endl;
					} else if(op==GE_FLOAT){
						code << "jge(L_"<<true_label<<");"<<endl;
					}
					code << "move(0,"<<r1<<");"<<endl;
					code<<"j(L_"<<next_code_label<<");"<<endl;
					code << "L_"<<true_label<<":"<<endl;
					code << "move(1,"<<r1<<");"<<endl; // valid change from rstack.top() -> r1 ?
					code << "L_"<<next_code_label<<":"<<endl;
					
					rstack.push(r1);
				} else {
					cout<<"Impossible print"<<endl;
				}
			} else {
				int true_label=nextLabel++;
				int next_code_label=nextLabel++;
				code << "cmpf("+r1+", "+r2+");\n";
				if (op == LT_FLOAT) {
					code << "jl(L_"<<true_label<<");"<<endl;
				} else if (op == GT_FLOAT) {
					code << "jg(L_"<<true_label<<");"<<endl;
				} else if(op==LE_FLOAT){
					code << "jle(L_"<<true_label<<");"<<endl;
				} else if(op==GE_FLOAT){
					code << "jge(L_"<<true_label<<");"<<endl;
				}
				code << "move(0,"<<r1<<");"<<endl;
				code<<"j(L_"<<next_code_label<<");"<<endl;
				code << "L_"<<true_label<<":"<<endl;
				code << "move(1,"<<r1<<");"<<endl;
				code << "L_"<<next_code_label<<":"<<endl;

				rstack.push(r2);
				swap_top(rstack);
			}	
		} else if (op ==LT_INT||op == GT_INT||op == LE_INT||op == GE_INT){
			if(stack_used) {
				if(rhs->type == "float") {
					rstack.pop(); // lhs value in r1
					code << "loadi(ind(esp), "+rstack.top()+");\n"; //  load value from stack to reg	
					code<< "popi(1);\n";
					int true_label=nextLabel++;
					int next_code_label=nextLabel++;
					code << "cmpi("+r1+", "+rstack.top()+");\n";
					if (op == LT_INT) {
						code << "jl(L_"<<true_label<<");"<<endl;
					} else if (op == GT_INT) {
						code << "jg(L_"<<true_label<<");"<<endl;
					} else if(op==LE_INT){
						code << "jle(L_"<<true_label<<");"<<endl;
					} else if(op==GE_INT){
						code << "jge(L_"<<true_label<<");"<<endl;
					}
					code << "move(0,"<<r1<<");"<<endl;
					code<<"j(L_"<<next_code_label<<");"<<endl;
					code << "L_"<<true_label<<":"<<endl;
					code << "move(1,"<<r1<<");"<<endl;
					code << "L_"<<next_code_label<<":"<<endl;
					
					rstack.push(r1);
				} else {
					cout<<"Impossible print"<<endl;
				}
			} else {
				int true_label=nextLabel++;
				int next_code_label=nextLabel++;
				code << "cmpi("+r1+", "+r2+");\n";
				if (op == LT_INT) {
					code << "jl(L_"<<true_label<<");"<<endl;
				} else if (op == GT_INT) {
					code << "jg(L_"<<true_label<<");"<<endl;
				} else if(op==LE_INT){
					code << "jle(L_"<<true_label<<");"<<endl;
				} else if(op==GE_INT){
					code << "jge(L_"<<true_label<<");"<<endl;
				}
				code << "move(0,"<<r1<<");"<<endl;
				code<<"j(L_"<<next_code_label<<");"<<endl;
				code << "L_"<<true_label<<":"<<endl;
				code << "move(1,"<<r1<<");"<<endl;
				code << "L_"<<next_code_label<<":"<<endl;
				
				rstack.push(r2);
				swap_top(rstack);
			}	
		} else {
			cout<<"Unreachable code"<<endl;
		}
		

		
	}

	
	return code.str();

}

unaryop_astnode::unaryop_astnode(exp_astnode* _operand, UnaryOp _op){
	operand=_operand;
	op=_op;
	if (op == TO_FLOAT) {
		type = "float";
	} else if (op == TO_INT || op == NOT) {
		type = "int";
	} else if (op == PP || op == UMINUS){
		type= operand->type;
	}
	if(op == PP) {
		label = max(operand->label, 2);
	} else {
		label = operand->label;
	}
}
void unaryop_astnode::print(){
	cout<<"("<<UnaryOpStrs[op]<<" ";
		operand->print();
		cout<<")";
}

string unaryop_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code ;
	if(op == TO_FLOAT) {
		code<<operand->generate_code(rstack, lst,gst);
		code<<"intTofloat("<<rstack.top()<<");\n";
	} else if (op == TO_INT) {
		code<<operand->generate_code(rstack, lst,gst);
		code<<"floatToint("<<rstack.top()<<");\n";
	} else if (op == PP) {
		swap_top(rstack);
		code<<operand->generate_code(rstack, lst, gst);
		//int offset = find_offset(((arrayref_astnode*) operand)->name, lst);
		// Because PP operand is an lvalue expect offset instead of 
		
		string offset_reg = rstack.top();
		rstack.pop(); // 
		if (operand->type == "float") {
			//code<<"addi(ebp, "<<offset_reg<<");"<<endl;
			code << "loadf(ind(ebp,"<<offset_reg<<"), "<<rstack.top()<<");\n";
			code<<"addf("<<1.0<<", "<<rstack.top()<<");\n";
			code<<"storef("<<rstack.top()<<", ind(ebp,"<<offset_reg<<"));\n";
			code<<"addf("<<-1.0<<", "<<rstack.top()<<");\n";
		} else {
			//code<<"addi(ebp, "<<offset_reg<<");"<<endl;
			code << "loadi(ind(ebp,"<<offset_reg<<"), "<<rstack.top()<<");\n";
			code<<"addi("<<1<<", "<<rstack.top()<<");\n";
			code<<"storei("<<rstack.top()<<", ind(ebp,"<<offset_reg<<"));\n";
			code<<"addi("<<-1<<", "<<rstack.top()<<");\n";
		}
		rstack.push(offset_reg);
		swap_top(rstack);
		//cout<<"Code here is "<<code.str()<<endl;
	} else if (op == NOT) {
		int true_label=nextLabel++;
		int next_code_label=nextLabel++;
		code<<operand->generate_code(rstack, lst,gst);
		if (operand->type == "float") {
			code<<"cmpf(0.0, "<<rstack.top()<<");\n" ;
			code<<"jne(L_"<<true_label<<");"<<endl;

 			// TODO
 			// code<<"je("
		} else {
			code<<"cmpi(0, "<<rstack.top()<<");\n" ;
			code<<"jne(L_"<<true_label<<");"<<endl;
		}
		code<<"move(1,"<<rstack.top()<<");"<<endl;
		code<<"j(L_"<<next_code_label<<");"<<endl;
		code<<"L_"<<true_label<<":"<<endl;
		code<<"move(0,"<<rstack.top()<<");"<<endl;
		code<<"L_"<<next_code_label<<":"<<endl;
 		//TODO
	} else if (op == UMINUS) {
		int offset = find_offset(((arrayref_astnode*) operand)->name, lst);
		if (operand->type == "float") {
			code << "loadf(ind(ebp, "<<offset<<"), "<<rstack.top()<<");\n";
			code<<"mulf("<<-1.0<<", "<<rstack.top()<<");\n";
		} else {
			code << "loadi(ind(ebp), "<<offset<<"), "<<rstack.top()<<");\n";
			code<<"muli("<<1<<", "<<rstack.top()<<");\n";
		}
	}
	return code.str();
}

funcallstmt_astnode::funcallstmt_astnode(string _name, list<exp_astnode*> * _args) {
	name=_name;
	args=_args;
}

string funcallstmt_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	
	stringstream code;
	string ret_type=get_ret_type(name, gst);
	if(name=="printf"){
		if(args->front()->type=="string"){
			code<<"print_string(\""<<((stringconst_astnode*)args->front())->value<<"\");"<<endl;
			return code.str();
		}
		else{
			code<<(args->front())->generate_code(rstack, lst,gst);
			if(args->front()->type=="int"){
				code<<"print_int("<<rstack.top()<<");"<<endl;
			}
			else if(args->front()->type=="float"){
				code<<"print_float("<<rstack.top()<<");"<<endl;
			}
		}
		return code.str();
	}
	// Getting space for return value
	if(ret_type=="int")
		code<<"pushi(0);"<<endl;
	else if(ret_type=="float")
		code<<"pushf(0);"<<endl;

	// TODO : Push according to size
	
	for(list<exp_astnode*>::iterator args_it=args->begin();args_it!=args->end();args_it++){
		code<<(*args_it)->generate_code(rstack, lst,gst);
		
		if((*args_it)->type=="int")
			code<<"pushi("<<rstack.top()<<");"<<endl;
		else if((*args_it)->type=="float")
			code<<"pushf("<<rstack.top()<<");"<<endl;
	}
	code<<name<<"();"<<endl;
	for(list<exp_astnode*>::reverse_iterator args_it=args->rbegin();args_it!=args->rend();args_it++){
		//(*args_it)->generate_code(rstack, lst,gst);
		if((*args_it)->type=="int")
			code<<"popi(1);"<<endl;
		else if((*args_it)->type=="float")
			code<<"popf(1);"<<endl;
	}
	

	if(ret_type=="int")
		code<<"popi(1);"<<endl;
	else if(ret_type=="float")
		code<<"popf(1);"<<endl;

	//cout<<code.str();
	return code.str();
}


void funcallstmt_astnode::print(){
	cout<<"(FunCallStmt ";
		cout<<name;
		cout<<" ";
		for(list<exp_astnode*>::iterator iter=args->begin();iter!=args->end();iter++) {
			(*iter)->print();
			cout <<" ";
		}
		cout<<")";
}

funcall_astnode::funcall_astnode(string _name, list<exp_astnode*> * _args){
	name=_name;
	args=_args;
	label = 1; // Of course incorrect !
	for(list<exp_astnode*>::iterator args_it=args->begin();args_it!=args->end();args_it++){
		label = max(label, (*args_it)->label); 
	} // Assume callee or caller saving so 

}

string funcall_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;
	string ret_type=get_ret_type(name, gst);
	
	// Getting space for return value
	if(ret_type=="int")
		code<<"pushi(0);"<<endl;
	else if(ret_type=="float")
		code<<"pushf(0);"<<endl;

	// TODO : Push according to size
	
	for(list<exp_astnode*>::iterator args_it=args->begin();args_it!=args->end();args_it++){
		code<<(*args_it)->generate_code(rstack, lst,gst);
		
		if((*args_it)->type=="int")
			code<<"pushi("<<rstack.top()<<");"<<endl;
		else if((*args_it)->type=="float")
			code<<"pushf("<<rstack.top()<<");"<<endl;
	}
	code<<name<<"();"<<endl;
	for(list<exp_astnode*>::reverse_iterator args_it=args->rbegin();args_it!=args->rend();args_it++){
		//(*args_it)->generate_code(rstack, lst,gst);
		if((*args_it)->type=="int")
			code<<"popi(1);"<<endl;
		else if((*args_it)->type=="float")
			code<<"popf(1);"<<endl;
	}
	
	string top_reg=rstack.top();

	code<<"loadi(ind(esp),"<<top_reg<<");"<<endl;

	if(ret_type=="int")
		code<<"popi(1);"<<endl;
	else if(ret_type=="float")
		code<<"popf(1);"<<endl;

	//cout<<code.str();
	return code.str();
}

void funcall_astnode::print(){
	cout<<"(FunCall ";
		cout<<name;
		cout<<" ";
		for(list<exp_astnode*>::iterator iter=args->begin();iter!=args->end();iter++) {
			(*iter)->print();
			cout <<" ";
		}
		cout<<")";
}


floatconst_astnode::floatconst_astnode(float _value){
	value=_value;
	label = 1;
	is_leaf = true;
}

void floatconst_astnode::print(){
	cout<<"(FloatConst "<<value<<")";
}

string floatconst_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;
	if (rstack.empty()) {
		cout<<"TODO : STACK EMPTY"<<endl;
	} else {
		string reg = rstack.top();
		code<<"move("<<value<<", "<<reg<<");\n"; // Put it on the top of stack
		//cout<<code.str()<<endl;
	}
	return code.str();
}

intconst_astnode::intconst_astnode(int _value){
	value=_value;
	label = 1;
	is_leaf = true;
}

void intconst_astnode::print(){
	cout<<"(IntConst "<<value<<")";
}

string intconst_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;
	if (rstack.empty()) {
		cout<<"TODO : STACK EMTPY"<<endl;
	} else {
		string reg = rstack.top();
		code<<"move("<<value<<", "<<reg<<");\n";
		//cout<<code.str()<<endl;
	}
	return code.str();
}

stringconst_astnode::stringconst_astnode(string _value){
	value=_value;
	is_leaf = true;
	// label ? 
	// NOTE : Usable only in function call printf
}

void stringconst_astnode::print(){
	cout<<"(StringConst \""<<value<<"\")";
}

string stringconst_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	return "";
}

identifier_astnode::identifier_astnode(string _value){
	value=_value;
	label = 1;
	is_leaf = true;
}
void identifier_astnode::print(){
	cout<<"(Id \""<<value<<"\")";
}

string identifier_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	string code = "";
	if (rstack.empty()) {
		cout<<"Empty stack."<<endl;
	} else {
		//cout<<"This is reached "<<endl;
	}
	return "";
}

arrayid_astnode::arrayid_astnode(string _name){
	name = _name;
	// TODO ? 
	label = 1;
	is_leaf = true;
}

string arrayid_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;
	if (rstack.empty()) {
		cout<<"Stack is dry"<<endl;
	} else {
		string top = rstack.top();
		int offset = find_offset(name, lst);
		string type = find_type(name, lst);
		//cout<<"Reached here "<<endl;
		if (is_lvalue) {
			code << "move("<<offset<<","<<top<<");\n"; //TODO
		} else {
			if (type == "int") {
				code << "loadi(ind(ebp"<<", "<<offset<<"), "<<top<<");\n";
			} else if(type == "float") {
				code << "loadf(ind(ebp"<<", "<<offset<<"), "<<top<<");\n";
			}
		}
	}

	return code.str();
}

arrayid_astnode::arrayid_astnode(string _name, string _type, unsigned int _width){
	name=_name;
	type = _type;
	width = _width;
	label=1;
}
void arrayid_astnode::print(){
	cout<<"(Id \""<<name<<"\")";
}

index_astnode::index_astnode(arrayref_astnode * _arrayid, exp_astnode * _index) {
	arrayid=_arrayid;
	index=_index;
	name=_arrayid->name;
	if (_arrayid->label == _index->label) {
		label = _arrayid->label+1;
	} else {
		label = max(_arrayid->label, _index->label);
	}
	// TODO ?
}

index_astnode::index_astnode(arrayref_astnode * _arrayref, exp_astnode * _index, string _type, unsigned int _width){
	type = _type;
	arrayid=_arrayref;
	index=_index;
	width = _width;
	name=_arrayref->name;
	if (_arrayref->label == _index->label) {
		label = _arrayref->label+1;
	} else {
		label = max(_arrayref->label, _index->label);
	}
}

void index_astnode::print(){
	cout<<"(Index ";
		arrayid->print();
		index->print();
		cout<<")";
}

string index_astnode::generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) {
	stringstream code;

	string atype = arrayid->type;
	if (atype.find("array") == string::npos) {
		cout<<"Impossible statement reached "<<endl;
	} else {
		string r1,r2;
		if (rstack.empty()) {
			cout<<"Stack is dry"<<endl;
		}

		else if(arrayid->label>=reg_count&&index->label>=reg_count){

			code << arrayid->generate_code(rstack, lst,gst);
			code <<"pushi("+rstack.top()+");\n";
			code<<index->generate_code(rstack, lst,gst);

			r2 = rstack.top();

			rstack.pop();

			code << "loadf(ind(esp), "+rstack.top()+");"<<endl;
			code<< "popf(1);"<<endl;

			code<<"muli("<<find_width(type)<<",r2);"<<endl;
			code<<"addi(r2,"<<rstack.top()<<");"<<endl;


			rstack.push(r2);
			swap_top(rstack);
		}
		else {
			if(arrayid->label>=index->label){
				code<< arrayid->generate_code(rstack, lst,gst);
				r1 = rstack.top();
				rstack.pop();
				code<< index->generate_code(rstack, lst,gst);
				r2 = rstack.top();
			}
			else{
				code<< index->generate_code(rstack, lst,gst);
				r2 = rstack.top();
				rstack.pop();
				code<< arrayid->generate_code(rstack, lst,gst);
				r1 = rstack.top();
			}


			code<<"muli("<<-find_width(type)<<","<<r2<<");"<<endl;
			code<<"addi("<<r2<<","<<r1<<");"<<endl;

			if(arrayid->label>=index->label){
				rstack.push(r1);
			} else{
				rstack.push(r2);
				swap_top(rstack);
			}
		}

		if(!is_lvalue){
			if(type=="int")
				code<<"loadi(ind(ebp,"<<rstack.top()<<"),"<<rstack.top()<<");"<<endl;
			else if(type=="float")
				code<<"loadi(ind(ebp,"<<rstack.top()<<"),"<<rstack.top()<<");"<<endl;
			else{
				cout<<"Unreachable code in index_astnode::generate_code"<<endl;
				exit(-1);
			}

		}	
	}
	
	return code.str();
}

int find_width(string type){
	if(type=="int"||type=="float") return 4;
	
	int a=stoi(type.substr(type.find('(')+1,type.find(',')-type.find('(')-1));
	
	return a*find_width(type.substr(type.find(',')+1, type.length()-type.find(',')-2));
}