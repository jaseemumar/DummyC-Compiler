struct type
{
	unsigned int width;
	string value;
	type() {

	}
	type(unsigned int _width, string _value) {
		width = _width;
		value = _value;
	}
};


struct lst_entry {
	string name;
	string var_type;
	bool param; // True if the entry is a parameter passed to the function
	int offset;
	unsigned int width;

	lst_entry(string _name, string _vtype, bool _param, int _offset, unsigned int _width){
		name=_name;
		var_type=_vtype;
		param=_param;
		offset=_offset;
		width=_width;
	}
};

// Assume no polymorphism
struct gst_entry {
	string func_name;
	string ret_type;
	list<lst_entry*>* lst;
	int offset;
	gst_entry() {
		lst = new list<lst_entry*>();
	}
};

struct fun_dec {
	list <lst_entry*>* lst;
	string name; 
	fun_dec() {

	}
};

