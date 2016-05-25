%{
#include "node.h"
#include "syntax.tab.h"

extern int err;

int yycolumn = 1;
#define YY_USER_ACTION {\
	yylloc.first_line = yylloc.last_line = yylineno;\
	yylloc.first_column = yycolumn;\
	yylloc.last_column = yycolumn + yyleng - 1;\
	yycolumn += yyleng;\
}

%}

%option yylineno
WS [ \t\r]+
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
INT [0-9]+
SFLOAT ([0-9]+)?(\.[0-9]+)[Ee][+-]?[0-9]+
FLOAT {SFLOAT}|([0-9]+\.[0-9]+)
ID (_|[a-zA-Z])([0-9]|_|[a-zA-Z])*

%%

{WS}        {}
\n          {yycolumn = 1;}
{SEMI}      {yylval.node = initNode("SEMI",yytext,0);return SEMI;}
{COMMA}     {yylval.node = initNode("COMMA",yytext,0);return COMMA;}
{ASSIGNOP}  {yylval.node = initNode("ASSIGNOP",yytext,0);return ASSIGNOP;}
{RELOP}     {yylval.node = initNode("RELOP",yytext,0);return RELOP;}
{PLUS}      {yylval.node = initNode("PLUS",yytext,0);return PLUS;}
{MINUS}     {yylval.node = initNode("MINUS",yytext,0);return MINUS;}
{STAR}      {yylval.node = initNode("STAR",yytext,0);return STAR;}
{DIV}       {yylval.node = initNode("DIV",yytext,0);return DIV;}
{AND}       {yylval.node = initNode("AND",yytext,0);return AND;}
{OR}        {yylval.node = initNode("OR",yytext,0);return OR;}
{DOT}       {yylval.node = initNode("DOT",yytext,0);return DOT;}
{NOT}       {yylval.node = initNode("NOT",yytext,0);return NOT;}
{TYPE}      {yylval.node = initNode("TYPE",yytext,0);return TYPE;}
{LP}        {yylval.node = initNode("LP",yytext,0);return LP;}
{RP}        {yylval.node = initNode("RP",yytext,0);return RP;}
{LB}        {yylval.node = initNode("LB",yytext,0);return LB;}
{RB}        {yylval.node = initNode("RB",yytext,0);return RB;}
{LC}        {yylval.node = initNode("LC",yytext,0);return LC;}
{RC}        {yylval.node = initNode("RC",yytext,0);return RC;}
{STRUCT}    {yylval.node = initNode("STRUCT",yytext,0);return STRUCT;}
{RETURN}    {yylval.node = initNode("RETURN",yytext,0);return RETURN;}
{IF}        {yylval.node = initNode("IF",yytext,0);return IF;}
{ELSE}      {yylval.node = initNode("ELSE",yytext,0);return ELSE;}
{WHILE}     {yylval.node = initNode("WHILE",yytext,0);return WHILE;}
{INT}       {yylval.node = initNode("INT",yytext,0);return INT;}
{FLOAT}     {yylval.node = initNode("FLOAT",yytext,0);return FLOAT;}
{ID}        {yylval.node = initNode("ID",yytext,0);return ID;}
.           {err = 1;printf("Error Type A at line %d:%s is missing match\n",yylineno,yytext);}

%%

int yywrap(){
	return 1;
}
