FLOATCONST ([0-9]*\.[0-9]+([eE][\+\-]?[0-9]*)?)|([0-9]+\.[0-9]*([eE][\+\-]?[0-9]*)?)|([0-9]+([eE][\+\-]?[0-9]*))
INTCONST ([1-9][0-9]*)|(0[bB][01]+)|(0[0-7]*)|(0[xX][0-9a-fA-F]+)
IDENTIFIER [[:alpha:]_][[:alpha:][:digit:]_]*
%%
[ \t] 													;
\n														Parser::lineno++;
\"([^\\\"\n\t]|(\\[\\nt\"]))*\"							{Parser::d_val__=std::string(matched());
														return Parser::STRING_LITERAL;}
void 													return Parser::VOID;
int 													return Parser::INT;
float 													return Parser::FLOAT;
return 													return Parser::RETURN;
while 													return Parser::WHILE;
for														return Parser::FOR;
if														return Parser::IF;
else													return Parser::ELSE;
{FLOATCONST}										 	{ std::string::size_type sz;
														Parser::d_val__ =stof(matched(),&sz);
														return Parser::FLOAT_CONSTANT;}
{INTCONST} 												{std::string::size_type sz;
														Parser::d_val__ =stoi(matched(), &sz);
														return Parser::INT_CONSTANT;}
{IDENTIFIER}  											{Parser::d_val__ =std::string(matched());
														return Parser::IDENTIFIER;}
\&\& 														return Parser::AND_OP;
\|\| 														return Parser::OR_OP;
\=\= 														return Parser::EQ_OP;
\!\= 														return Parser::NE_OP;
\+\+														return Parser::INC_OP;
\(															return '(';
\,															return ',';
\)															return ')';
\{															return '{';
\}															return '}';
\[															return '[';
\]															return ']';
\; 															return ';';
\>\= 														return Parser::GE_OP;
\<\= 														return Parser::LE_OP;
\>															return '>';
\<															return '<';
\+															return '+';
\- 															return '-';														
\* 															return '*';
\/ 															return '/';
\=															return '=';
\!															return '!';
.															;
