#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SYMBOL_TABLE_SIZE 0x3fff //size of symboltable
#define int_type 0
#define float_type 1

typedef enum{false,true} bool;
typedef enum{var,func} Item_Type;
typedef struct Type_ Type;
typedef struct FieldList_ FieldList;
typedef struct Fundef_ Fundef;

struct Type_{
	enum {BASIC,ARRAY,STRUCTURE,CONSTANT} kind;
	union {
		int basic;
		struct {Type* elem;int size;} array;
		FieldList* structure;
	} u;
};

struct FieldList_{
	char* name;
	Type* type;
	FieldList* next;
	FieldList* samehash;
};

struct Fundef_{
	Type* ret;
	char* name;
	FieldList* args_list;
	int row;
	Fundef* samehash;
	bool hasDefined;
};


FieldList*  VarTable[SYMBOL_TABLE_SIZE];//basic/array/structure
Fundef*     FuncTable[SYMBOL_TABLE_SIZE];//function

unsigned int hash(char* name);
void initTables();
FieldList* getVarByName(char* name);
Fundef* getFuncByName(char* name);
int insertVar(FieldList* var);
int insertFunc(Fundef* func,int hasDefined);

bool typeEqual(Type* f1,Type* f2);
bool paramEqual(FieldList* f1,FieldList* f2);
int typeSize(Type* type);

#endif
