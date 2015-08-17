void add_to_table(list<lst_entry*> *lst, lst_entry * entry){
	for(list<lst_entry*>::iterator it=lst->begin();it!=lst->end();it++){
		if((*it)->name==entry->name){
			goto error;
		}
	
	}

	lst->push_back(entry);
	return;
	error:
		std::cout<<"Error: redeclaration of "<<entry->name<<endl; 
		exit(-1);
		//TODO print line no
}

string itos(int i){
	stringstream ss;
	ss<<i;
	string ret =ss.str();
	return ret;
}

string array_type_gen(string s, int index){
	int close_bracket_pos=s.find(")");
	int comma_pos=s.find_last_of(',');
	string type,new_type;
		if(comma_pos==string::npos){
			//cout<<"Error:didn't find a comma in "<<s<<endl;
			type=s;
			new_type="array("+itos(index)+","+type+")";
		}
		else {
			type=s.substr(comma_pos+1,close_bracket_pos-comma_pos-1);
			new_type=s.substr(0,comma_pos+1)+"array("+itos(index)+","+type+")"+s.substr(close_bracket_pos);
		}

	return new_type;
}

string find_type(string var_name, list<lst_entry*> * lst) {
	list<lst_entry*>::iterator iter = lst->begin();
	while (iter != lst->end()) {
		if ((*iter)->name == var_name) {
			return (*iter)->var_type;
		}
		iter++;
	}
	return "NOT_FOUND";
}

int find_offset(string var_name, list<lst_entry*> * lst) {
	list<lst_entry*>::iterator iter = lst->begin();
	while (iter != lst->end()) {
		if ((*iter)->name == var_name) {
			return (*iter)->offset;
		}
		iter++;
	}
	return 0;
}
bool used_name(string f, list<gst_entry*> *gst) {
	list<gst_entry*>::iterator iter= gst->begin();
	while(iter != gst->end()) {
		if ((*iter)->func_name == f) {
			return true;
		}
		iter++;
	} 
	return false;
}

string get_ret_type(string f, list<gst_entry*> *gst){
	list<gst_entry*>::iterator iter= gst->begin();
	while(iter != gst->end()) {
		if ((*iter)->func_name == f) {
			return (*iter)->ret_type;
		}
		iter++;
	} 
	return "NOT_FOUND";
}

list<lst_entry*>* get_lst (string f, list<gst_entry*> *gst) {
	list<gst_entry*>::iterator iter= gst->begin();
	while(iter != gst->end()) {
		if ((*iter)->func_name == f) {
			return (*iter)->lst;
		}
		iter++;
	} 
	return NULL;
}
