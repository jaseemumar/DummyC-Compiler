%scanner Scanner.h
%scanner-token-function d_scanner.lex()


%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP EQ_OP NE_OP LE_OP GE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN IDENTIFIER INC_OP



%polymorphic exp:exp_astnode*; arrayref:arrayref_astnode*; stmt:stmt_astnode*; stmt_list: std::list<stmt_astnode*>*; exp_list:std::list<exp_astnode*>*; INT_T:int; FLOAT_T:float; STRING_T :std::string; UnaryOp_T:UnaryOp; type_t:type*;  lst_entry_ptr:lst_entry*; fun_dec_ptr:fun_dec*; lst_ptr:std::list<lst_entry*>*;

%type<exp> expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression primary_expression constant_expression postfix_expression
%type<arrayref> declarator l_expression 
%type<stmt> statement assignment_statement selection_statement iteration_statement	compound_statement
%type<type_t> type_specifier
%type<stmt_list> statement_list
%type<exp_list> expression_list
%type<INT_T> INT_CONSTANT
%type<UnaryOp_T> unary_operator
%type<FLOAT_T> FLOAT_CONSTANT
%type<STRING_T> STRING_LITERAL IDENTIFIER fun_dec_name
%type<lst_entry_ptr> parameter_declaration
%type<lst_ptr> parameter_list declarator_list
%type<fun_dec_ptr> fun_declarator
%%

translation_unit
	: function_definition 
	| translation_unit function_definition 
    ;

function_definition 
	: type_specifier fun_declarator compound_statement {
		//write out void function_name(){
		//pushi(ebp); // Setting dynamic link
		//move(esp,ebp); //not in case of main
		code<<"void "<<cur_fun<<"(){"<<endl;
		
			code<<"pushi(ebp);"<<endl; // Setting dynamic link
			code<<"move(esp,ebp);"<<endl;
		
		gst.back()->lst = $2->lst;
		// Getting offset to -4 now that one function is done
		o=-4;
		cout<<cur_fun<<endl;
		cout <<"--------------------------"<<endl;
		cout<<"AST:"<<endl;
		$3->print();
		cout<<endl;
		cout<<"Local Symbol Table:"<<endl;
		cout<<"Name\tType\tWidth\tOffset"<<endl;
		cout <<"--------------------------"<<endl;
			list <lst_entry*>::iterator liter;
			for(liter =cur_lst->begin(); liter!=cur_lst->end();liter++){
   				cout <<(*liter)->name<<"\t"<<(*liter)->var_type <<"\t"<< (*liter)->width<<"\t"<<(*liter)->offset<<endl;
   				 
   			}
   		cout <<"--------------------------"<<endl<<endl;	
   		code << $3->generate_code(rstack,$2->lst,&gst);
   		
   			code<<"loadi(ind(ebp), ebp);"<<endl; // restoring dynamic link
			code<<"popi(1);"<<endl;
		
   		code<<"}"<<endl;
	}
	;

type_specifier
	: VOID {
		t = "void";
		w = 0;
		$$ = new type(0, "void");
	}
    | INT {
    	t = "int";
    	w = 4;
    	$$ = new type(4, "int");
    }
	| FLOAT {
		t = "float";
		w = 4;
		$$ = new type(4, "float");
	}
    ;

fun_declarator
	: fun_dec_name '(' ')' {

    	if (used_name($1, &gst)) {
			cout <<"Error: Redefinition of function in line : "<<lineno<<endl;
			exit(-1);
		}
    	fun_dec* fd = new fun_dec();
    	gst_entry* g_entry=new gst_entry();
		g_entry->ret_type = t;
		g_entry->func_name = $1;
		//  to the gst but no lst attached
		gst.push_back(g_entry);
    	cur_fun = $1;
    	fd->name = $1;
    	fd->lst = new list<lst_entry*>();
    	cur_lst = fd->lst;
    	$$= fd;
    } 
    | fun_dec_name'(' parameter_list ')' {

		// Pass the lst containing parameters and the name of the function
		if (used_name($1, &gst)) {
			cout <<"Error: Redefinition of function in line : "<<lineno<<endl;
			exit(-1);
		}
		gst_entry* g_entry=new gst_entry();
		g_entry->ret_type = func_t;
		g_entry->func_name = $1;
		// add to the gst but no lst attached
		
		cur_fun = $1;

		fun_dec* fd = new fun_dec();
		fd->name = $1;
		fd->lst = $3;
		cur_lst = fd->lst;
		$$ = fd;
		g_entry->lst=$3;
		gst.push_back(g_entry);
	}
	;

fun_dec_name 
	: IDENTIFIER {func_t = t;}  {
		$$ = $1;
	}
	;

parameter_list
	: parameter_declaration {
		list<lst_entry*>* lst;
		lst  = new list<lst_entry*>(); // TODO valid ?
		$1->offset = 4; // Below old %ebp a word is left for return ?
		lst->push_back($1);
		$$ = lst;
	}
	| parameter_list ',' parameter_declaration {
		// correcting offset
		$3->offset = $1->back()->offset + $1->back()->width;
		add_to_table($1,$3);
		$$ = $1;
	}
	;

parameter_declaration
	: type_specifier declarator {
		unsigned int size;
		arrayid_astnode* id=(arrayid_astnode *)$2;
		lst_entry* l_entry = new lst_entry(id->name, $1->value, true, 0, $1->width); // Incorrect offset. Correct in above level
		$$ = l_entry ;
	}
    ;

declarator
	: IDENTIFIER {
		//cout<<"IDENTIFIER: "<<$1<<endl;
		if (t == "void") {
			cout <<"Error : Invalid variable in line : "<<lineno<<endl;
			exit(-1);
		}
		$$ = new arrayid_astnode($1, t, w);
	}
	| declarator '[' constant_expression ']' {
		int index = ((intconst_astnode*) $3)->value;
		if (((intconst_astnode*) $3)->type != "int" || index <0) {
			// TODO line number
			cout <<"Error : Invalid index in array at line : "<<lineno<<endl;
			exit(-1) ;
		}
		$$ = new index_astnode($1, $3, array_type_gen($1->type,index), index*($1->width));
	}
    ;

constant_expression 
    : INT_CONSTANT {
    	$$=new intconst_astnode($1);
    
    	$$->type = "int";
    }
    | FLOAT_CONSTANT {
    	$$=new floatconst_astnode($1);
    	$$->type = "float";
    }
    ;

compound_statement
	: '{' '}' {
		$$=new block_astnode(new list<stmt_astnode*>());
		//rootNode=$$;
	}
	| '{' statement_list '}' {
		$$=new block_astnode($2);
		//cout<<$$->generate_code(rstack, cur_lst, &gst);
		//rootNode=$$;
		
	}
    | '{' declaration_list statement_list '}' {
		$$=new block_astnode($3);
		//cout<<$$->generate_code(rstack, cur_lst, &gst);
	}
	;

statement_list
	: statement {
		$$=new std::list<stmt_astnode*>();
		$$->push_back($1);	
	}
	| statement_list statement	{
		$$=$1;
		$$->push_back($2);
	}
	;

statement
    : '{' statement_list '}' {
		$$ = new block_astnode($2);
		//rootNode=$$;
	} //a solution to the local decl problem
    | selection_statement {
		$$=$1;
	}
    | iteration_statement {
		$$=$1;
	}
	| assignment_statement {
		$$=$1;
	}
    | RETURN expression ';'{
		//cout<<"lol"<<endl;
    	string ret_type = $2->type;
    	string expected_ret_type = get_ret_type(cur_fun, &gst);
    	if (ret_type == expected_ret_type || (expected_ret_type=="void")) {
    		$$=new return_astnode($2);
    	} else if (ret_type == "int" && expected_ret_type == "float") {
			$$=new return_astnode(new unaryop_astnode($2, TO_FLOAT));
		} else if (ret_type == "float" && expected_ret_type == "int") {
			$$=new return_astnode(new unaryop_astnode($2, TO_INT));
		} else {
    		cout <<"Error: Invalid return type for function " <<cur_fun<<". Expected return type: "<<get_ret_type(cur_fun, &gst)<<" got "<<ret_type<<" in line : "<<lineno<<endl;
    		exit(-1);
    	}
	}
	|  IDENTIFIER '(' expression_list ')' ';' {
		list<lst_entry*>::iterator liter= cur_lst->begin();
		while(liter != cur_lst->end()) {
			if ((*liter)->name == $1) {
				cout<<"Invalid usage of variable '"<<$1<<"' as a function"<<endl;
				exit(-1);
			}
			liter++;
		}
		if($1=="printf"){
			if($3->size()>1){
				cout <<"Error: printf given more than one argument at : "<<lineno<<endl;
				exit(-1);
			}	
			$$ = new funcallstmt_astnode($1, $3);
		}
		else if (used_name($1, &gst)) {
			list<lst_entry*>* l = get_lst($1, &gst);
			list<lst_entry*>::iterator liter = l->begin();
			list<exp_astnode*>::iterator eiter = $3->begin();
			while (eiter != $3->end()) {
				if (liter == l->end() || (*liter)->offset < 0) {
					cout <<"Error: Function '"<<$1<<"' does not have required number of parameters in line number : "<<lineno<<endl;
					exit(-1);	
				} else {
					if ((*liter)->var_type == (*eiter)->type) {
						$$ = new funcallstmt_astnode($1, $3);
					} else {
						if((*liter)->var_type == "float" && (*eiter)->type == "int") {
							*eiter = new unaryop_astnode(*eiter, TO_FLOAT);
						} else if((*liter)->var_type == "int" && (*eiter)->type == "float"){
							*eiter = new unaryop_astnode(*eiter, TO_INT);
						} else {
							cout <<"Error: Function '"<<$1<<"' is used with incorrect parameter types in line : "<<lineno<<endl;
    						exit(-1);				
						}
					}
				}
				liter++;
				eiter++;
			}
			if( (liter != l->end()) && ((*liter)->offset > 0)) {
				cout <<"Error: Function '"<<$1<<"' has wrong number of parameters in line number : "<<lineno<<endl;
				exit(-1);
			}
			$$ = new funcallstmt_astnode($1, $3);
		} else {
    		cout <<"Error: Function '"<<$1<<"' is not declared in this scope in line : "<<lineno<<endl;
    		exit(-1);
    	}
	}
	| IDENTIFIER '(' ')' ';' {
		list<lst_entry*>::iterator liter= cur_lst->begin();
		while(liter != cur_lst->end()) {
			if ((*liter)->name == $1) {
				cout<<"Invalid usage of variable '"<<$1<<"' as a function"<<endl;
				exit(-1);
			}
			liter++;
		}
    		if (used_name($1, &gst)) {
    			// check arguements match
    			list<lst_entry*>* l = get_lst($1, &gst);
    			list<lst_entry*>::iterator iter = l->begin();
    			if ((l->size()==0) || ((*iter)->offset < 0) ) {
    				$$ = new funcallstmt_astnode($1, new std::list<exp_astnode*>());	
    			} else {
    				cout <<"Error: Function '"<<$1<<"' has wrong number of parameters in line number : "<<lineno<<endl;
    				exit(-1);
    			}
    		} else {
    			cout <<"Error: Function '"<<$1<<"' is not declared in this scope in line : "<<lineno<<endl;
    			exit(-1);
    		}
		
	}	
    ;

assignment_statement
	: ';' {
		$$=new empty_astnode();
	}								
	|  l_expression '=' expression ';' {
		string expected_type = $1->type;
		string got_type = $3->type;
		if (expected_type == got_type) {
    		$$=new ass_astnode($1,$3);
    	} else if (expected_type == "int" && got_type == "float") {
			$$=new ass_astnode($1, new unaryop_astnode($3, TO_INT));
		} else if (expected_type == "float" && got_type == "int") {
			$$=new ass_astnode($1, new unaryop_astnode($3, TO_FLOAT));
		} else {
    			cout << "Error: Invalid assigment of "+$3->type +" to variable of type "+ $1->type+" in line number: "<<lineno<<endl;
    			exit(-1);
    		}
	}	
	;

expression
    : logical_and_expression {
	$$=$1;
//	cout<<$$->generate_code(rstack, cur_lst,&gst)<<endl;
//	cout<<"Label is "<<endl;
    }
    | expression OR_OP logical_and_expression {
	$$=new binaryop_astnode($1,$3,OR);
	$$->type = "int";
	}
	;

logical_and_expression
    : equality_expression {
    		$$=$1;
    }
    | logical_and_expression AND_OP equality_expression {
		$$=new binaryop_astnode($1,$3,AND);
		$$->type = "int";
	}
	;

equality_expression
	: relational_expression {
		$$=$1;
	}
    | equality_expression EQ_OP relational_expression {
		if($1->type == "int" && $3->type == "int") {
			$$=new binaryop_astnode($1,$3,EQ);
			$$->type = "int";
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,EQ);
			$$->type ="int";
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3,EQ);
			$$->type ="int";
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3,TO_FLOAT),EQ);
			$$->type="int";
		} else {
			cout << "Error: Invalid comparison between "+$1->type +" and "+ $3->type+" in line number: "<<lineno<<endl;
			exit(-1);
		}
	}
	| equality_expression NE_OP relational_expression {
	    if($1->type == "int" && $3->type == "int") {
			$$=new binaryop_astnode($1,$3,NE);
			$$->type = "int";
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,NE);
			$$->type ="int";
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3,NE);
			$$->type ="int";
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3,TO_FLOAT),NE);
			$$->type="int";
		} else {
			cout << "Error: Invalid comparison between "+$1->type +" and "+ $3->type+" in line number: "<<lineno<<endl;
			exit(-1);
		}	
	}
	;

relational_expression
	: additive_expression {
		$$=$1;
	}
    | relational_expression '<' additive_expression {
    	if($1->type == "int" && $3->type == "int") {
			$$=new binaryop_astnode($1,$3,LT_INT);
			$$->type = "int";
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,LT_FLOAT);
			$$->type ="int";
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3,LT_FLOAT);
			$$->type ="int";
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3,TO_FLOAT),LT_FLOAT);
			$$->type="int";
		} else {
			cout <<"Error : Invalid operation in line :"<<lineno<<endl;
			exit(-1);
		}
	}
	| relational_expression '>' additive_expression {
		if($1->type == "int" && $3->type == "int") {
			$$=new binaryop_astnode($1,$3,GT_INT);
			$$->type = "int";
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,GT_FLOAT);
			$$->type ="int";
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3,GT_FLOAT);
			$$->type ="int";
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3,TO_FLOAT),GT_FLOAT);
			$$->type="int";
		} else {
			cout <<"Error : Invalid operation in line : "<<lineno<<endl;
			exit(-1);
		}
	}
	| relational_expression LE_OP additive_expression {
		if($1->type == "int" && $3->type == "int") {
			$$=new binaryop_astnode($1,$3,LE_INT);
			$$->type = "int";
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,LE_FLOAT);
			$$->type ="int";
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3,LE_FLOAT);
			$$->type ="int";
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3,TO_FLOAT),LE_FLOAT);
			$$->type="int";
		} else {
			cout <<"Error : Invalid operation in line : "<<lineno<<endl;
			exit(-1);
		}
	} 
    | relational_expression GE_OP additive_expression {
		if($1->type == "int" && $3->type == "int") {
			$$=new binaryop_astnode($1,$3,GE_INT);
			$$->type = "int";
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,GE_FLOAT);
			$$->type ="int";
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3,GE_FLOAT);
			$$->type ="int";
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3,TO_FLOAT),GE_FLOAT);
			$$->type="int";
		} else {
			cout <<"Error : Invalid operation in line : "<<lineno<<endl;
			exit(-1);
		}
	}
	;

additive_expression 
	: multiplicative_expression {
		$$=$1;
	}
	| additive_expression '+' multiplicative_expression {
		if($1->type == "int" && $3->type == "int") {
			$$=new binaryop_astnode($1,$3,PLUS_INT);
			$$->type = "int";
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,PLUS_FLOAT);
			$$->type ="float";
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3,PLUS_FLOAT);
			$$->type ="float";
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3,TO_FLOAT),PLUS_FLOAT);
			$$->type="float";
		} else {
			cout <<"Error : Invalid operation in line : "<<lineno<<endl;
			cout<<$1->type<<" "<<$3->type<<endl;
			exit(-1);
		}
	}
	| additive_expression '-' multiplicative_expression {
		if($1->type == "int" && $3->type == "int") {
			$$=new binaryop_astnode($1,$3,MINUS_INT);
			$$->type = "int";
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,MINUS_FLOAT);
			$$->type ="float";
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3,MINUS_FLOAT);
			$$->type ="float";
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3,TO_FLOAT),MINUS_FLOAT);
			$$->type="float";
		} else {
			cout <<"Error : Invalid operation in line :"<<lineno<<endl;
			exit(-1);
		}
	}
	;

multiplicative_expression
	: unary_expression {
		$$=$1;
		//cout<<"Reached mulitplicative expression of type "<<$$->type<<endl;
	}
	| multiplicative_expression '*' unary_expression {
		string ltype = $1->type;
		string rtype = $3->type;
		
		
		if ($1->type == "int" && $3->type== "int") {
			$$->type= "int";
			$$=new binaryop_astnode($1,$3,MULT_INT);
		} else if ($1->type == "float" && $3->type == "float") {
			$$->type= "float";
			$$=new binaryop_astnode($1,$3,MULT_FLOAT);
		} else if ($1->type == "float" && $3->type == "int") {
			$$->type= "float";
			$$=new binaryop_astnode($1,new unaryop_astnode($3, TO_FLOAT),MULT_FLOAT);
		} else if ($1->type == "int" && $3->type == "float") {
			$$->type = "float";
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3, MULT_FLOAT);
		} else {
			cout<<"Invalid operation in line : "<<lineno<<endl;
			exit(-1);
		}
		$1->type = ltype;
		$3->type = rtype;
	} 
	| multiplicative_expression '/' unary_expression {
		string ltype = $1->type;
		string rtype = $3->type;

		if ($1->type == "int" && $3->type== "int") {
			$$=new binaryop_astnode($1,$3,DIV_INT);
		} else if ($1->type == "float" && $3->type == "float") {
			$$=new binaryop_astnode($1,$3,DIV_FLOAT);
		} else if ($1->type == "float" && $3->type == "int") {
			$$=new binaryop_astnode($1,new unaryop_astnode($3, TO_FLOAT),DIV_FLOAT);
		} else if ($1->type == "int" && $3->type == "float") {
			$$=new binaryop_astnode(new unaryop_astnode($1, TO_FLOAT),$3, DIV_FLOAT);
		} else {
			cout<<"Invalid operation in line : "<<lineno<<endl;
			exit(-1);
		}
		$$->type= "float";
		$1->type = ltype;
		$3->type = rtype;
	} 
	;
unary_expression
	: postfix_expression  {
		$$=$1;
	}				
	| unary_operator postfix_expression {
		if ($2->type != "float" && $2->type != "int") {
			cout <<"Error: Invalid operator on type "+$2->type+" at line : "<<lineno<<endl;
			exit(-1);
		}
		$$=new unaryop_astnode($2,$1);
		$$->type = $2->type;
	}
	;

postfix_expression
	: primary_expression {
		$$=$1;
	}
    | IDENTIFIER '(' ')' {
    	list<lst_entry*>::iterator liter= cur_lst->begin();
		while(liter != cur_lst->end()) {
			if ((*liter)->name == $1) {
				cout<<"Invalid usage of variable '"<<$1<<"' as a function"<<endl;
				exit(-1);
			}
			liter++;
		}
    	if (used_name($1, &gst)) {
    		// check arguements match
    		list<lst_entry*>* l = get_lst($1, &gst);
    		list<lst_entry*>::iterator iter = l->begin();
    		if ((l->size() == 0) || ((*iter)->offset < 0)) {
    			$$ = new funcall_astnode($1, new std::list<exp_astnode*>());
    			$$->type= get_ret_type($1, &gst);
    		} else {
    			cout <<"Error: Function '"<<$1<<"' has wrong number of parameters in line number : "<<lineno<<endl;
    			exit(-1);
    		}
    	} else {
    		cout <<"Error: Function '"<<$1<<"' is not declared in this scope in line : "<<lineno<<endl;
    		exit(-1);
    	}
		
	}	
	| IDENTIFIER '(' expression_list ')' {
		list<lst_entry*>::iterator liter= cur_lst->begin();
		while(liter != cur_lst->end()) {
			if ((*liter)->name == $1) {
				cout<<"Invalid usage of variable '"<<$1<<"' as a function"<<endl;
				exit(-1);
			}
			liter++;
		}
		if (used_name($1, &gst)) {
			list<lst_entry*>* l = get_lst($1, &gst);
			list<lst_entry*>::iterator liter = l->begin();
			list<exp_astnode*>::iterator eiter = $3->begin();
			while (eiter != $3->end()) {
				if (liter == l->end() || (*liter)->offset < 0) {
					cout <<"Error: Function '"<<$1<<"' has wrong number of parameters in line number : "<<lineno<<endl;
					exit(-1);
				} else {
					if ((*liter)->var_type == (*eiter)->type) {
						$$ = new funcall_astnode($1, $3);
					} else {
						if((*liter)->var_type == "float" && (*eiter)->type == "int") {
							*eiter = new unaryop_astnode(*eiter, TO_FLOAT);
						} else if ((*liter)->var_type == "int" && (*eiter)->type == "float"){
							*eiter = new unaryop_astnode(*eiter, TO_INT);
						} else {
							cout <<"Error: Function '"<<$1<<"' is used with incorrect parameter types in line : "<<lineno<<endl;
    						exit(-1);				
						}
					}
				}
				liter++;
				eiter++;
			}
			if((liter != l->end()) && ((*liter)->offset > 0)){
				cout <<"Error: Function '"<<$1<<"' has wrong number of parameters in line number : "<<lineno<<endl;
				exit(-1);
			}
			$$ = new funcall_astnode($1, $3);
			$$->type= get_ret_type($1, &gst);
		} else {
    		cout <<"Error: Function '"<<$1<<"' is not declared in this scope in line : "<<lineno<<endl;
    		exit(-1);
    	}
		
	}
	| l_expression INC_OP {
		if ($1->type == "int" || $1->type == "float") {
			$$=new unaryop_astnode($1,PP);
			$$->type = $1->type;
		} else {
			cout<<"Error : Invalid increment operation in line : "<<lineno<<endl;
			exit(-1);
		}
	}
	;

primary_expression
	: l_expression {
		$1->is_lvalue = false;
		$$=$1;
		
	}
    | l_expression '=' expression {
    	string expected_type = $1->type;
		string got_type = $3->type;
		if (expected_type == got_type) {
    		$$=new binaryop_astnode($1,$3,ASSIGN);
			$$->type = $1->type;
    	} else if (expected_type == "int" && got_type == "float") {
    		$$=new binaryop_astnode($1, new unaryop_astnode($3, TO_INT),ASSIGN);
			$$->type = $1->type;
		} else if (expected_type == "float" && got_type == "int") {
			$$=new binaryop_astnode($1, new unaryop_astnode($3, TO_FLOAT),ASSIGN);
			$$->type = $1->type;
		} else {
    		cout << "Error: Invalid assigment of "+$3->type +" to variable of type "+ $1->type+" in line number: "<<lineno<<endl;
    		exit(-1);
    	}
	} // added this production
	| INT_CONSTANT {
    		$$=new intconst_astnode($1);
    		$$->type= "int";
			if (rstack.empty()) {
				cout<<"Empty stack"<<endl;
			}// else {
				
	}
	| FLOAT_CONSTANT {
		$$=new floatconst_astnode($1);
		$$->type = "float";
	}
	| STRING_LITERAL {
		std::string str=$1;
		$$=new stringconst_astnode(str.substr(1,str.length()-2));
		$$->type = "string";
	}
	| '(' expression ')'{
		$$=$2;
	} 	
	;

l_expression
	: IDENTIFIER {
		$$ = new arrayid_astnode($1);
		// Set type from symbol table
		string vartype= find_type($1, cur_lst);
		if (vartype == "NOT_FOUND") {
			cout<<"Error: Variable is neither declared nor a parameter in line : "<<lineno<<endl;
			exit(-1);
		}
		$$->type = vartype;
		$$->is_lvalue = true;
		int offset = find_offset($1, cur_lst);
	}
	| l_expression '[' expression ']' {
		// Index would be decreased 
		string vartype= $1->type;
		if (vartype == "NOT_FOUND") {
			cout<<"Error: Variable is neither declared nor a parameter in line : "<<lineno<<endl;
			exit(-1);
		} else if (vartype.find("array(") != 0) {
			cout <<"Invalid index opration on a non-array object in line : "<<lineno<<endl;
			exit(-1);
		}
		if ($3->type == "int") {
			// Removing a level of indexing from type
			string t =  vartype.substr(vartype.find(',')+1, vartype.length()-vartype.find(',')-2);
			$$ =new index_astnode($1, $3);
			$$->name = $1->name;
			$$->type=t;
			
		} else {
			cout<<"Error: Invalid type of index in line number : "<<lineno<<endl;
			exit(-1);
		}
		$$->is_lvalue = true;
	}
	;
	
expression_list
	: expression {
		$$=new std::list<exp_astnode*>();
		$$->push_back($1);
	}
	| expression_list ',' expression {
		$$=$1;
		$$->push_back($3);
		
	}
	;
	
unary_operator
    : '-' {
		$$=UMINUS;
	}	
	| '!' {
		$$ = NOT;
	} 
	;

selection_statement
    : IF '(' expression ')' statement ELSE statement {
		$$=new if_astnode($3,$5,$7);
	}
	;

iteration_statement
	: WHILE '(' expression ')' statement {
		$$=new while_astnode($3,$5);
	}	
    | FOR '(' expression ';' expression ';' expression ')' statement {
		$$=new for_astnode($3,$5,$7,$9);
	}
     //modified this production
    
    ;

declaration_list
	: declaration  {
	}				
	| declaration_list declaration {
	}
	;

declaration
	: type_specifier declarator_list ';'
	;

declarator_list
	: declarator {
		lst_entry *decl = new lst_entry($1->name, $1->type, false, o, $1->width);
		o = o - $1->width;
		add_to_table(cur_lst, decl);
	}
	| declarator_list ',' declarator  {
		
		lst_entry *decl = new lst_entry($3->name, $3->type, false, o, $3->width);
		o = o - $3->width;
		add_to_table(cur_lst, decl);
	}
	;