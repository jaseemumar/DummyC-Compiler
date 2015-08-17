#ifndef __AST_DECLARE__
#define __AST_DECLARE__



enum BinaryOp{
	OR,
	AND,
	EQ,
	NE,
	LT_FLOAT,
	LT_INT,
	GT_FLOAT,
	GT_INT,
	LE_FLOAT,
	LE_INT,
	GE_FLOAT,
	GE_INT,
	PLUS_FLOAT,
	PLUS_INT,
	MINUS_INT,
	MINUS_FLOAT,
	MULT_INT,
	MULT_FLOAT,
	DIV_INT,
	DIV_FLOAT,
	ASSIGN
};




enum UnaryOp{
	UMINUS, NOT, PP, TO_FLOAT, TO_INT
};

//string UnaryOpStrs[];

class abstract_astnode
{
	public:
		virtual void print ()=0;

	 	virtual string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst) = 0;
	// 	virtual basic_types getType() = 0;
	// 	virtual bool checkTypeofAST() = 0;
	// protected:
	// 	virtual void setType(basic_types) = 0;
	// private:
	// 	typeExp astnode_type;
};

class stmt_astnode : public abstract_astnode
{

};	

class exp_astnode : public abstract_astnode
{	
	public:
		string type;
		int label;
		bool is_leaf;
};

class arrayref_astnode : public exp_astnode
{	
	public:
		string name;
		unsigned int width;
		bool is_lvalue;
};

class block_astnode : public stmt_astnode
{
	public :
		list<stmt_astnode*> * stmt_list;
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);

		block_astnode(list<stmt_astnode*> * _stmt_list);
};

class ass_astnode : public stmt_astnode
{
	public :
		exp_astnode* lhs;
		exp_astnode* rhs;
		ass_astnode(exp_astnode *_lhs, exp_astnode* _rhs);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class return_astnode : public stmt_astnode
{
	public :
		exp_astnode* return_value;
		return_astnode(exp_astnode * _return_value);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class if_astnode : public stmt_astnode
{
	public :
		exp_astnode* predicate;
		stmt_astnode* if_body;
		stmt_astnode* else_body;

		if_astnode(exp_astnode* _predicate, stmt_astnode* _if_body,stmt_astnode* _else_body);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);		
};


class while_astnode : public stmt_astnode
{
	public :
		exp_astnode* guard;
		stmt_astnode* body;

		while_astnode(exp_astnode *_guard, stmt_astnode* _body);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};


class for_astnode : public stmt_astnode
{
	public :
		exp_astnode* initialiser;
		exp_astnode* guard;
		exp_astnode* step;
		stmt_astnode* body;
		
		for_astnode(exp_astnode* _initialiser, exp_astnode* _guard, exp_astnode* _step, stmt_astnode* _body);
        
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class empty_astnode : public stmt_astnode{
	public:
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class funcallstmt_astnode : public stmt_astnode {
public:
	list<exp_astnode*> * args;
	string name;
	funcallstmt_astnode(string name, list<exp_astnode*> * _args);
	void print();
	string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};
class binaryop_astnode : public exp_astnode{
	public:
		exp_astnode* lhs;
		exp_astnode* rhs;
		BinaryOp op;
		bool castlhs;
		bool castrhs;
		binaryop_astnode(exp_astnode* _lhs, exp_astnode* _rhs, BinaryOp _op);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class unaryop_astnode : public exp_astnode
{
	public:
		exp_astnode* operand;
		UnaryOp op;

		unaryop_astnode(exp_astnode* _operand, UnaryOp _op);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class funcall_astnode : public exp_astnode
{
	public:
		string name;
		list<exp_astnode*> * args;

		funcall_astnode(string name, list<exp_astnode*> * _args);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class floatconst_astnode : public exp_astnode{
	public:
		float value;
		floatconst_astnode(float _value);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class intconst_astnode : public exp_astnode{
	public:
		int value;
		intconst_astnode(int _value);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class stringconst_astnode : public exp_astnode{
	public :
		string value;
		stringconst_astnode(string _value);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class identifier_astnode : public exp_astnode{
	public :
		string value;
		identifier_astnode(string _value);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class arrayid_astnode : public arrayref_astnode{
	public :
		arrayid_astnode(string _name);
		arrayid_astnode(string _name, string _type, unsigned int _size);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

class index_astnode : public arrayref_astnode{
	public :
		arrayref_astnode * arrayid;
		exp_astnode * index;
		index_astnode(arrayref_astnode * _arrayid, exp_astnode * _index);
		index_astnode(arrayref_astnode * _arrayid, exp_astnode * _index, string _type, unsigned int _width);
		void print();
		string generate_code(stack<string> &rstack, list<lst_entry*> *lst,list<gst_entry*> *gst);
};

#endif
