#include <stdio.h>
#include <string.h>
extern void yyparse();
extern struct Node* root;
extern int err;
extern int yylineno;
char* optional(char* arg){
	if(strcmp(arg,"-t1") == 0)
		return "../Test/test1.cmm";
	else if(strcmp(arg,"-t2") == 0){
		return "../Test/test2.cmm";
	}
	else if(strcmp(arg,"-t3") == 0){
		return "../Test/test3.cmm";
	}
	else if(strcmp(arg,"-t4") == 0){
		return "../Test/test4.cmm";
	}
	else if(strcmp(arg,"-p1") == 0){
		return "../Test/optional1.cmm";
	}
	else if(strcmp(arg,"-p2") == 0){
		return "../Test/optional2.cmm";
	}
	else if(strcmp(arg,"-p3") == 0){
		return "../Test/optional3.cmm";
	}
	else if(strcmp(arg,"-p4") == 0){
		return "../Test/optional4.cmm";
	}
	else if(strcmp(arg,"-p5") == 0){
		return "../Test/optional5.cmm";
	}
	else if(strcmp(arg,"-p6") == 0){
		return "../Test/optional6.cmm";
	}
	else 
		return arg;

}
int main(int argc,char**argv){
	if(argc <= 1){
		return 1;
	}
	char* filename = optional(argv[1]);
	FILE* f = fopen(filename,"r");
	if(!f){
		perror(filename);
		return 1;
	}
	yylineno = 1;
	yyrestart(f);
	yyparse();
	if(!err)
		printTree(root,0);
	return 0;
}
