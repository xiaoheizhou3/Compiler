%{
#include "lex.yy.c"
#include "node.h"
#include "semantic.h"
#define YYERROR_VERBOSE
struct Node *root;
int err = 0;

void yyerror(const char* msg){
	fprintf(stderr,"Error type B at Line %d ,wrong input:  %s ?\n",yylineno,yylval.node->value);
}

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
		   |/*empty*/          {$$ = initNode("ExtDefList","",@$.first_line);}
		   ;
ExtDef : Specifier ExtDecList SEMI {$$ = initNode("ExtDef","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	   | Specifier SEMI            {$$ = initNode("ExtDef","",@$.first_line);addChildList($$,$1,$2,NULL);}
	   | Specifier FunDec CompSt   {$$ = initNode("ExtDef","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	   | Specifier FunDec SEMI     {$$ = initNode("ExtDef","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	   | error SEMI                {err = 1;}
	   ;
ExtDecList : VarDec                   {$$ = initNode("ExtDecList","",@$.first_line);addChildList($$,$1,NULL);}
		   | VarDec COMMA ExtDecList  {$$ = initNode("ExtDecList","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
           ;

/*Specifier*/
Specifier : TYPE                  {$$ = initNode("Specifier","",@$.first_line);addChildList($$,$1,NULL);}
		  | StructSpecifier       {$$ = initNode("Specifier","",@$.first_line);addChildList($$,$1,NULL);}
          ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$ = initNode("StructSpecifier","",@$.first_line);addChildList($$,$1,$2,$3,$4,$5,NULL);}
				| STRUCT Tag                  {$$ = initNode("StructSpecifier","",@$.first_line);addChildList($$,$1,$2,NULL);}
				;
OptTag : ID         {$$ = initNode("OptTag","",@$.first_line);addChildList($$,$1,NULL);}
	   |/*empty*/   {$$ = initNode("OptTag","",@$.first_line);}
	   ;
Tag : ID            {$$ = initNode("Tag","",@$.first_line);addChildList($$,$1,NULL);}
    ;

/*Declarators*/
VarDec : ID                    {$$ = initNode("VarDec","",@$.first_line);addChildList($$,$1,NULL);}
	   | VarDec LB INT RB      {$$ = initNode("VarDec","",@$.first_line);addChildList($$,$1,$2,$3,$4,NULL);}
	   ;
FunDec : ID LP VarList RP      {$$ = initNode("FunDec","",@$.first_line);addChildList($$,$1,$2,$3,$4,NULL);}
	   | ID LP RP              {$$ = initNode("FunDec","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	   | error RP              {err = 1;}
	   ;
VarList : ParamDec COMMA VarList   {$$ = initNode("VarList","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
		| ParamDec                 {$$ = initNode("VarList","",@$.first_line);addChildList($$,$1,NULL);}
		;
ParamDec : Specifier VarDec       {$$ = initNode("ParamDec","",@$.first_line);addChildList($$,$1,$2,NULL);}
		 ;

/*Statements*/
CompSt : LC DefList StmtList RC   {$$ = initNode("CompSt","",@$.first_line);addChildList($$,$1,$2,$3,$4,NULL);}
	   | error RC                 {err = 1;}
	   ;
StmtList : Stmt StmtList          {$$ = initNode("StmtList","",@$.first_line);addChildList($$,$1,$2,NULL);}
		 |/*empty*/               {$$ = initNode("StmtList","",@$.first_line);}
		 ;
Stmt : Exp SEMI                                  {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,NULL);}
	 | CompSt								  	 {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,NULL);}
	 | RETURN Exp SEMI							 {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	 | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,$3,$4,$5,NULL);}
	 | IF LP Exp RP Stmt ELSE Stmt               {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,$3,$4,$5,$6,$7,NULL);}
	 | WHILE LP Exp RP Stmt                      {$$ = initNode("Stmt","",@$.first_line);addChildList($$,$1,$2,$3,$4,$5,NULL);}
	 | error RP                                  {err = 1;}
	 | error SEMI                                {err = 1;}
 	 ;

/*Local Defnitions*/
DefList : Def DefList           {$$ = initNode("DefList","",@$.first_line);addChildList($$,$1,$2,NULL);}
		|/*empty*/              {$$ = initNode("DefList","",@$.first_line);}
		;
Def : Specifier DecList SEMI    {$$ = initNode("Def","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
//	| Specifier DecList error   {err = 1;}
	 ;
DecList : Dec                    {$$ = initNode("DecList","",@$.first_line);addChildList($$,$1,NULL);}
		| Dec COMMA DecList      {$$ = initNode("DecList","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
		;
Dec : VarDec                     {$$ = initNode("Dec","",@$.first_line);addChildList($$,$1,NULL);}
	| VarDec ASSIGNOP Exp        {$$ = initNode("Dec","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
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
Args : Exp COMMA Args            {$$ = initNode("Args","",@$.first_line);addChildList($$,$1,$2,$3,NULL);}
	 | Exp                       {$$ = initNode("Args","",@$.first_line);addChildList($$,$1,NULL);}
	 ;
%%


int main(int argc,char**argv){
	if(argc <= 1){
		return 1;
	}
	FILE* f = fopen(argv[1],"r");
	if(!f){
		perror(argv[1]);
		return 1;
	}
	yylineno = 1;
	yyrestart(f);
	yyparse();
	// printTree(root,0);
	if(!err){
		// printTree(root,0);
		Program(root);
	}
	return 0;
}
