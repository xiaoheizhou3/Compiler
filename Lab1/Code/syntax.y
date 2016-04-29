%{
#include "lex.yy.c"
#include "node.h"
struct Node *root;
int err = 0;
%}

%union{
	int type_int;
	float type_float;
	char *type_str;
    struct Node *node;
}

%token <node> SEMI COMMA
%token <node> ASSIGNOP RELOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR DOT NOT
%token <node> LP RP LB RB LC RC
%token <node> STRUCT RETURN IF ELSE WHILE
%token <node> TYPE
%token <node> INT 
%token <node> FLOAT
%token <node> ID

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%nonassoc WHILE RETURN STRUCT

%type <node> Program
%type <node> ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag 
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt DefList Def DecList Dec
%type <node> Exp Args

%start Program
%%
/*High-level Definitions*/
Program : ExtDefList {$$ = initNode("Program","",@$.first_line);addChildList($$,$1,NULL);root = $$;}
		;
ExtDefList : ExtDef ExtDefList {$$ = initNode("ExtDefList","",@$.first_line);addChildList($$,$1,$2,NULL);}
		   |/*empty*/          {$$ = NULL;}
		   ; 
ExtDef : Specifier ExtDecList SEMI {$$ = initNode("ExtDef","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	   | Specifier SEMI            {$$ = initNode("ExtDef","",@$.first_line);addChildList($$,$1,$2,NULL);}
	   | Specifier FunDec CompSt   {$$ = initNode("ExtDef","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	   ;
ExtDecList : VarDec                  {$$ = initNode("ExtDecList","",@$.first_line);addChildList($$,$1,NULL);}
		   |VarDec COMMA ExtDecList  {$$ = initNode("ExtDecList","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
           ;

/*Specifier*/
Specifier : TYPE                 {$$ = initNode("Specifier","",@$.first_line);addChildList($$,$1,NULL);}
		  |StructSpecifier       {$$ = initNode("Specifier","",@$.first_line);addChildList($$,$1,NULL);}
          ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$ = initNode("StructSpecifier","",@$.first_line);addChildList($$,$1,$2,$3,$4,$5,NULL);}
				|STRUCT Tag {$$ = initNode("StructSpecifier","",@$.first_line);addChildList($$,$1,$2,NULL);}
				;
OptTag : ID         {$$ = initNode("OptTag","",@$.first_line);addChildList($$,$1,NULL);} 
	   |/*empty*/   {$$ = NULL;}
	   ;
Tag : ID            {$$ = initNode("Tag","",@$.first_line);addChildList($$,$1,NULL);}
    ;

/*Declarators*/
VarDec : ID                   {$$ = initNode("VarDec","",@$.first_line);addChildList($$,$1,NULL);}
	   |VarDec LB INT RB      {$$ = initNode("VarDec","",@$.first_line);addChildList($$,$1,$2,$3,$4,NULL);}
	   ;
FunDec : ID LP VarList RP     {$$ = initNode("FunDec","",@$.first_line);addChildList($$,$1,$2,$3,$4,NULL);}
	   | ID LP RP              {$$ = initNode("FunDec","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	   ;
VarList : ParamDec COMMA VarList  {$$ = initNode("VarList","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
		|ParamDec                 {$$ = initNode("VarList","",@$.first_line);addChildList($$,$1,NULL);}
		;
ParamDec : Specifier VarDec       {$$ = initNode("ParamDec","",@$.first_line);addChildList($$,$1,$2,NULL);}
		 ;

/*Statements*/
CompSt : LC DefList StmtList RC   {$$ = initNode("CompSt","",@$.first_line);addChildList($$,$1,$2,$3,$4,NULL);}
	   ;
StmtList : Stmt StmtList          {$$ = initNode("StmtList","",@$.first_line);addChildList($$,$1,$2,NULL);}
		 |/*empty*/               {$$ = NULL;}
		 ;
Stmt : Exp SEMI                                 {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,NULL);}
	 |CompSt									{$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,NULL);}
	 |RETURN Exp SEMI							{$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	 |IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,$3,$4,$5,NULL);}
	 |IF LP Exp RP Stmt ELSE Stmt               {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,$3,$4,$5,$6,$7,NULL);}
	 |WHILE LP Exp RP Stmt                      {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,$3,$4,$5,NULL);}
 ;

/*Local Defnitions*/
DefList : Def DefList           {$$ = initNode("DefList","",@$.first_line);addChildList($$,$1,$2,NULL);}
		|/*empty*/              {$$ = NULL;}
		;
Def : Specifier DecList SEMI    {$$ = initNode("Def","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	| Specifier DecList error   {err = 1;}   
	 ;
DecList : Dec                   {$$ = initNode("DecList","",@$.first_line);addChildList($$,$1,NULL);}
		|Dec COMMA DecList      {$$ = initNode("DecList","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
		;
Dec : VarDec                    {$$ = initNode("Dec","",@$.first_line);addChildList($$,$1,NULL);}
	|VarDec ASSIGNOP Exp        {$$ = initNode("Dec","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	;

/*Expressions*/
Exp : Exp ASSIGNOP Exp			{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|Exp AND Exp				{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|Exp OR Exp					{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|Exp RELOP Exp				{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|Exp PLUS Exp				{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|Exp MINUS Exp				{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|Exp STAR Exp				{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|Exp DIV Exp				{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|LP Exp RP					{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|MINUS Exp				    {$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,NULL);}
	|NOT Exp					{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,NULL);}
	|ID LP Args RP				{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,$4,NULL);}
	|ID LP RP					{$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|Exp LB Exp RB              {$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,$4,NULL);}
	|Exp DOT ID                 {$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	|ID                         {$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,NULL);}
	|INT                        {$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,NULL);}
	|FLOAT                      {$$ = initNode("Exp","",@$.first_line);addChildList($$,$1,NULL);}
	;
Args : Exp COMMA Args           {$$ = initNode("Args","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	 |Exp                       {$$ = initNode("Args","",@$.first_line);addChildList($$,$1,NULL);}
	 ;
/*Error*/
Stmt : error SEMI               {err = 1;}
	 ;
CompSt : error RC               {err = 1;}
	   ;
Exp : error RP                  {err = 1;}
	| error Exp                 {err = 1;}
	;
VarDec : error SEMI             {err = 1;}
	   | error RB               {err = 1;}
	   ;
ExtDef : error SEMI             {err = 1;}
	   ;
Def : error SEMI				{err = 1;}
	;
%%

void yyerror(char const* msg){
	fprintf(stderr,"Error type B at Line %d ,wrong input:  %s ?\n",yylineno,yylval.node->value);
}
