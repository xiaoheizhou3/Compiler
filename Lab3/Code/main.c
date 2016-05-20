// #include <stdio.h>
// #include <string.h>
//
// extern struct Node* root;
// extern int err;
// extern int yylineno;
//
// extern void yyparse();
// extern yyerror(char* msg);
// extern void yyrestart(FILE* f);
// extern void Program(struct Node* root);
//
// int main(int argc,char**argv){
// 	if(argc <= 1){
// 		return 1;
// 	}
// 	FILE* f = fopen(argv[1],"r");
// 	if(!f){
// 		perror(argv[1]);
// 		return 1;
// 	}
// 	yylineno = 1;
// 	yyrestart(f);
// 	yyparse();
// 	// printTree(root,0);
// 	if(!err){
// 		// printTree(root,0);
// 		Program(root);
// 	}
// 	return 0;
// }
