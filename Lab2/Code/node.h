#ifndef _HEADERNAME_H
#define _HEADERNAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
struct Node{
	int lineno;
	char name[16];
	char value[32];
	struct Node *children;//the head of children list
	struct Node *children_tail;//the tail of children list
	struct Node *next;
};

struct Node *initNode(char* name,char* value,int row);//initial node
void addChild(struct Node* f,struct Node* c);
void addChildList(struct Node* father,...);
void printNode(struct Node* root);
void printTree(struct Node* r,int count);

#endif
