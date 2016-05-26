#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include "node.h"
#include "hash_table.h"
#include "IR.h"
#include "debug.h"

void checkFunc();

void Program(struct Node* root);
void ExtDefList(struct Node* root);
void ExtDef(struct Node* root);
void ExtDecList(struct Node* root,Type* type);

Type* Specifier(struct Node* root);
Type* StructSpecifier(struct Node* root);

FieldList* VarDec(struct Node* root,Type* type,int from);
Fundef* FunDec(struct Node* root,Type* type);
FieldList* VarList(struct Node* root);
FieldList* ParamDec(struct Node* root);

void CompSt(struct Node* root,Type* retype);
void StmtList(struct Node* root,Type* retype);
void Stmt(struct Node* root,Type* retype);

FieldList* DefList(struct Node* root,int from);
FieldList* Def(struct Node* root,int from);
FieldList* DecList(struct Node* root,Type* type,int from);
FieldList* Dec(struct Node* root,Type* type,int from);

Type* Exp(struct Node *n,Operand* place);
bool Args(struct Node* n,FieldList* f,Operand* arg_list);

Type* Exp_Cond(struct Node *n,Operand* label_true,Operand* label_false);

#endif
