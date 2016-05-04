#include "node.h"

extern int yylineno;

struct Node *initNode(char *name,char *value,int lineno){

	assert(lineno >= 0);
    //ensure lineno >= 0
	struct Node *node = malloc(sizeof(struct Node));
	node->lineno = yylineno;
	strcpy(node->name,name);
	strcpy(node->value,value);
	node->children = NULL;
	node->children_tail = NULL;
	node->next = NULL;
	return node;
}

void addChild(struct Node* father,struct Node* child){

	assert(father != NULL && child != NULL);
    //ensure father and child are not NULL
	if(father->children == NULL){
		father->children = child;
		father->children_tail = child;
	}
	else{
		father->children_tail->next = child;
		father->children_tail = child;
	}
}

void addChildList(struct Node* father, ...){
	va_list pp;
	va_start(pp,father);
	struct Node* index = va_arg(pp,struct Node*);
	while(index != NULL){
		addChild(father,index);
		index = va_arg(pp,struct Node*);
	}
}

void printNode(struct Node* root){
	struct Node *child = root->children;
	if(child==NULL)
	{
		printf(" %s",root->value);
		return;
	}
	while(child!=NULL)
	{
		printNode(child);
		child=child->next;
	}
}

void printTree(struct Node* r,int count){
	if(r == NULL) return;
	if(r->children == NULL){
		int i = 0;
		for(i = 0;i < count;i++){
			printf("  ");
		}
		if(strcmp(r->name,"TYPE") == 0||strcmp(r->name,"INT") == 0 \
		   || strcmp(r->name,"FLOAT") == 0||strcmp(r->name,"ID") == 0){
			if(strcmp(r->name,"INT") == 0)
				printf("%s:%d\n",r->name,atoi(r->value));
			else if(strcmp(r->name,"FLOAT") == 0)
				printf("%s:%f\n",r->name,atof(r->value));
			else
				printf("%s:%s\n",r->name,r->value);
		}
		else{
			printf("%s\n",r->name);
		}
	}
	else{
		int i = 0;
		for(;i < count;i++){
			printf("  ");
		}
		printf("%s(%d)\n",r->name,r->lineno);
		struct Node *p = r->children;
		while(p !=NULL){
			printTree(p,count+1);
			struct Node* temp = p;
			p = p->next;
			free(temp);
			temp = NULL;
		}
	}
}
