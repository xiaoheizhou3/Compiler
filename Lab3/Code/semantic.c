#include "semantic.h"
#include "IR.h"

void checkFunc(){
	int i;
	for(i = 0;i < SYMBOL_TABLE_SIZE;i++){
		if(FuncTable[i] != NULL){
			Fundef* temp = FuncTable[i];
			while(temp != NULL){
				if(!temp->hasDefined){
					printf("Error type 18 at line %d: undefined function:%s\n",temp->row,temp->name);
				}
				temp = temp->samehash;
			}
		}
	}
}

void Program(struct Node* root){
	// printf("Enter Program\n");
	initTables();
	ExtDefList(root->children);
	checkFunc();
}

void ExtDefList(struct Node* root){
	// printf("Enter ExtDefList\n");
	struct Node* child = root->children;
	if(child != NULL){
		ExtDef(child);
		child = child->next;
		ExtDefList(child);
	}
}

void ExtDef(struct Node* root){
	// printf("Enter ExtDef\n");
	struct Node* child = root->children;
	Type* type = Specifier(child);
	child = child->next;
	if(strcmp(child->name,"SEMI") == 0){
		return ;
	}
	else if(strcmp(child->name,"ExtDecList") == 0){
		ExtDecList(child,type);
	}
	else if(strcmp(child->name,"FunDec") == 0){
		Fundef* temp = FunDec(child,type);
		child = child->next;
		int i = 0;
		if(strcmp(child->name,"SEMI") == 0){
			if(temp == NULL)
				return;
			temp->hasDefined = 0;
			i = insertFunc(temp,0);
				if(i == 1){
					printf("Error type 4 at line %d: Redefined function '%s'\n",temp->row,temp->name);
				}
				else if(i == 2){
					printf("Error type 19 at line %d: Inconsistent declaration of function '%s'\n",temp->row,temp->name);
				}
		}
		else{
			if(temp != NULL){
				temp->hasDefined = 1;
				i = insertFunc(temp,1);
				if(i == 1){
					printf("Error type 4 at line %d: Redefined function '%s'\n",temp->row,temp->name);
				}
				else if(i == 2){
					printf("Error type 19 at line %d: Inconsistent declaration of function '%s'\n",temp->row,temp->name);
				}
				else{
					InterCodes* tempNodeOfFunction = malloc(sizeof(struct InterCodes_));
					tempNodeOfFunction->code = malloc(sizeof(struct InterCode_));
					tempNodeOfFunction->code->kind = FUNCTION_K;
					tempNodeOfFunction->code->u.one.op = malloc(sizeof(struct Operand_));
					tempNodeOfFunction->code->u.one.op->kind = FUNCTION;
					tempNodeOfFunction->code->u.one.op->u.value = temp->name;
					insertCode(tempNodeOfFunction);
					FieldList* args_list = malloc(sizeof(struct FieldList_));
					args_list = temp->args_list;
					while(args_list != NULL){
						InterCodes* tempNodeOfArgs = malloc(sizeof(struct InterCodes_));
						tempNodeOfArgs->code = malloc(sizeof(struct InterCode_));
						tempNodeOfArgs->code->kind = PARAM_K;
						tempNodeOfArgs->code->u.one.op = malloc(sizeof(struct Operand_));
						tempNodeOfArgs->code->u.one.op->kind = VARIABLE;
						tempNodeOfArgs->code->u.one.op->u.value = args_list->name;
						insertCode(tempNodeOfArgs);
					}
				}
			}
			CompSt(child,type);
		}
	}
}

Type* Specifier(struct Node* root){
	Type* ret;
	struct Node* child = root->children;
	if(strcmp(child->name,"TYPE") == 0){
		ret = malloc(sizeof(struct Type_));
		ret->kind = BASIC;
		if(strcmp(child->value,"int") == 0){
			ret->u.basic = int_type;
		}
		else if(strcmp(child->value,"float") == 0){
			ret->u.basic = float_type;
		}
	}
	else/* if(strcmp(child->name,"StructSpecifier") == 0)*/{
		ret = StructSpecifier(child);
	}
	return ret;
}

Type* StructSpecifier(struct Node* root){
	// printf("Enter StructSpecifier\n");
	struct Node* child = root->children;
	Type* type = malloc(sizeof(struct Type_));
	type->kind = STRUCTURE;
	type->u.structure = malloc(sizeof(struct FieldList_));
	type->u.structure->next = NULL;

	FieldList* temp = malloc(sizeof(struct FieldList_));
	temp->type=type;
	struct Node* name_of_struct = NULL;
	while(child != NULL){
		if(strcmp(child->name,"OptTag") == 0){
			name_of_struct = child;
			if(child->children == NULL){
				type->u.structure->name = NULL;
			}
			else{
				type->u.structure->name = malloc(sizeof(child->children->value));
				strcpy(type->u.structure->name,child->children->value);
				temp->name=malloc(sizeof(child->children->value));
				strcpy(temp->name,child->children->value);
			}
		}
		else if(strcmp(child->name,"Tag") == 0){
			name_of_struct = child;
			FieldList* result = getVarByName(child->children->value);
			if(result == NULL || result->type->kind != STRUCTURE || strcmp(result->name,result->type->u.structure->name) != 0){
				printf("Error type 17 at line %d: Undefined structure ‘%s’\n",child->lineno,child->children->value);
				return NULL;
			}
			return result->type;
		}
		else if(strcmp(child->name,"DefList") == 0){
			type->u.structure->next=DefList(child,2);
			if(temp->type->u.structure->name==NULL){
				return type;
			}
			int i = insertVar(temp);
			if(i == 1)
			{
				printf("Error type 16 at line %d: Duplicated name ‘%s’\n",name_of_struct->lineno,temp->name);
				return NULL;
			}
			return type;
		}
		child = child->next;
	}
}

void ExtDecList(struct Node* root,Type* type){
	// printf("Enter ExtDecList\n");
	struct Node* child = root->children;
	VarDec(child,type,1);
	child = child->next;
	while(child != NULL){
		child->next;
		ExtDecList(child,type);
	}
}

FieldList* VarDec(struct Node* root,Type* type,int from){
	// printf("Enter VarDec\n");
	struct Node* child = root->children;
	FieldList* ret;
	if(strcmp(child->name,"ID") == 0){
		ret = malloc(sizeof(struct FieldList_));
		ret->name = malloc(sizeof(child->value));
		strcpy(ret->name,child->value);
		ret->type = type;
		ret->samehash = NULL;
		ret->next = NULL;
		if(from == 3){
			return ret;
		}
		int i = insertVar(ret);
		if(i == 1){
			if(from == 1)
				printf("Error type 3 at line %d: Redefined variable'%s'\n",child->lineno,ret->name);
			else
				printf("Error type 15 at line %d: Redefined field ‘%s’\n",child->lineno,ret->name);
			return NULL;
		}
		return ret;
	}
	else{
		ret = VarDec(child,type,from);
		if(ret == NULL){
			return NULL;
		}
		Type* ret_type_record = ret->type;
		child = child->next->next;
		Type* temp = malloc(sizeof(struct Type_));
		temp->kind = ARRAY;
		temp->u.array.size = atoi(child->value);
		temp->u.array.elem = type;
		if(ret_type_record->kind != ARRAY){

			ret->type = temp;
			return ret;
		}
		while(ret_type_record->u.array.elem->kind == ARRAY){
			ret_type_record = ret_type_record->u.array.elem;
		}
		ret_type_record->u.array.elem = temp;
		return ret;
	}


}

Fundef* FunDec(struct Node* root,Type* type){
	// printf("Enter FunDec\n");
	struct Node* child = root->children;
	Fundef* f = malloc(sizeof(struct Fundef_));
	f->name = malloc(sizeof(child->value));
	strcpy(f->name,child->value);
	f->hasDefined = 0;
	f->row = child->lineno;
	f->ret = type;
	f->samehash = NULL;
	f->args_list = NULL;
	child = child->next;
	child = child->next;
	if(strcmp(child->name,"VarList") ==  0)
	{
		f->args_list = VarList(child);
	}
	return f;
}

FieldList* VarList(struct Node* root){
	// printf("Enter VarList\n");
	struct Node* child = root->children;
	FieldList* f;
	f = ParamDec(child);
	child = child->next;
	if(child != NULL)
	{
		FieldList* p = f;
		child = child->next;
		if(p == NULL)
			f = VarList(child);
		else{
			while(p->next != NULL)
				p = p->next;
			p->next = VarList(child);
		}
	}
	return f;
}

FieldList* ParamDec(struct Node* root){
	// printf("Enter ParamDec\n");
	struct Node *child = root->children;
	FieldList* f;
	Type* type;
	type = Specifier(child);
	f = VarDec(child->next,type,3);
	return f;
}

void CompSt(struct Node* root,Type* retype){
	// printf("Enter CompSt\n");
	struct Node *child = root->children;
	child=child->next;
	DefList(child,1);
	child=child->next;
	StmtList(child,retype);
}

void StmtList(struct Node* root,Type* retype){
	// printf("Enter StmtList\n");
	struct Node *child = root->children;
	if(child == NULL)
		return;
	Stmt(child,retype);
	child = child->next;
	StmtList(child,retype);
}

void Stmt(struct Node* root,Type* retype){
	// printf("Enter Stmt\n");
	struct Node* child = root->children;
	while(child != NULL){
		if(strcmp(child->name,"RETURN")==0){
			child = child->next;
			Type* t = Exp(child);
			if(retype == NULL||t == NULL)
				return;
			if(!typeEqual(retype,t)){
				printf("Error type 8 at line %d: Type mismatched for return\n",child->lineno);
			}
			return;
		}
		else if(strcmp(child->name,"LP") == 0){
			child = child->next;
			Type* t = Exp(child);
			if(t != NULL && !(t->kind == 0 && t->u.basic == int_type)){
				printf("Error type ? conditional statement wrong type\n");
			}
		}
		else if(strcmp(child->name,"Exp") == 0){
			Exp(child);
		}
		else if(strcmp(child->name,"Stmt") == 0){
			Stmt(child,retype);
		}
		child = child->next;
	}
}

FieldList* DefList(struct Node* root,int from){
	// printf("Enter DefList\n");
	if(root->children == NULL){
		return NULL;
	}
	FieldList* f;
	struct Node *child = root->children;
	// printf("mean to enter Def\n");
	f = Def(child,from);
	// printf("go out from Def\n");
	FieldList* t = f;
	child = child->next;
	if(t != NULL){
		while(t->next!=NULL)	//find the last field
		{
			t=t->next;
		}
		t->next=DefList(child,from);
	}
	else
		f = DefList(child,from);
	return f;
}

FieldList* Def(struct Node* root,int from){
	// printf("Enter Def\n");
	struct Node* child = root->children;
	FieldList* f;
	Type* type = Specifier(child);
	child = child->next;
	f = DecList(child,type,from);	//inhre
	return f;
}

FieldList* DecList(struct Node* root,Type* type,int from){
	// printf("Enter DecList\n");
	struct Node *child = root->children;
	FieldList* f;
	f = Dec(child,type,from);
	child = child->next;
	if(child != NULL){
		child = child->next;
		FieldList* p = f;
		if(p != NULL)
		{
			while(p->next != NULL)
				p=p->next;
			p->next=DecList(child,type,from);
		}
		else
			f=DecList(child,type,from);
	}
	return f;
}

FieldList* Dec(struct Node* root,Type* type,int from){
	// printf("Enter Dec\n");
	struct Node *child = root->children;
	FieldList* f;
	f = VarDec(child,type,from);
	child = child->next;
	if(child != NULL){		//assignop =
		if(from == 2){	//struct cannot be initialized
			printf("Error type 15 at line %d: be initialized field ‘%s’\n",child->lineno,f->name);
			return f;
		}
		child = child->next;
		Type* t = Exp(child);
		if(t != NULL && type != NULL && !typeEqual(type,t)){
			printf("Error type 5 at line %d: Type mismatched for assignment\n",child->lineno);
		}
	}
	return f;
}

Type* Exp(struct Node* root){
	// printf("Enter Exp\n");
	struct Node *child = root->children;
	// printf("%s\n",child->value);
	Type* type;
	if(strcmp(child->name,"Exp") == 0){
		struct Node *child2 = child->next;
		if(strcmp(child2->name,"ASSIGNOP")==0)
		{
			//left value
			struct Node *leftChild = child->children;
			Type* leftType=NULL;
			//because of associative property and priority,it is right
			if(strcmp(leftChild->name,"ID") == 0 && leftChild->next == NULL)
				leftType = Exp(child);
			else if(strcmp(leftChild->name,"Exp") == 0&& leftChild->next != NULL &&strcmp(leftChild->next->name,"LB") == 0)	//array
				leftType = Exp(child);
			else if(strcmp(leftChild->name,"Exp") == 0&&leftChild->next != NULL&&strcmp(leftChild->next->name,"DOT") == 0){
				leftType = Exp(child);
			}
			else
				printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable\n",child->lineno);
			child2 = child2->next;
			Type* rightType = Exp(child2);
			if(leftType==NULL||rightType==NULL)
				return NULL;
			if(typeEqual(leftType,rightType))
				return leftType;
			else{
				printf("Error type 5 at line %d: Type mismatched for assignment\n",child->lineno);
				return NULL;
			}
		}
		else if(strcmp(child2->name,"PLUS")==0||strcmp(child2->name,"MINUS")==0||strcmp(child2->name,"STAR")==0||strcmp(child2->name,"DIV")==0||strcmp(child2->name,"RELOP")==0){
			Type* t = Exp(child);
			// printf("%s\n",child->children->name);
		    // printf("t`s type: %d,%d\n",t->kind,t->u.basic);
			child2 = child2->next;
			// printf("%s\n",child2->children->value);
			Type* t2 = Exp(child2);
			// printf("t2`s type: %d,%d\n",t2->kind,t2->u.basic);
			if(t == NULL||t2 == NULL)
				return NULL;
			else if(t->kind == BASIC && t2->kind == BASIC && t->u.basic == t2->u.basic){
				// printf("%d,%d\n",t->kind,t2->kind);
				return t;
			}
			else{
				printf("Error type 7 at line %d: Type mismatched for Operands\n",child->lineno);
					return NULL;
			}
		}
		else if(strcmp(child2->name,"LB")==0){
			Type* t1 = Exp(child);
			if(t1 == NULL)
				return NULL;
			if(t1->kind != 1){
				printf("Error type 10 at line %d: '",child->lineno);
				printNode(child);
				printf("' is not an array\n");
				return NULL;
			}
			child2 = child2->next;
			Type* t2 = Exp(child2);
			if(t2 == NULL)
				return NULL;
			if(!(t2->kind == BASIC && t2->u.basic == int_type)){
				printf("Error type 12 at line %d:",child2->lineno);
				printNode(child2);
				printf("' is not an integer.\n");
				return NULL;
			}
			return t1->u.array.elem;
		}
		else if(strcmp(child2->name,"DOT")==0){
			Type* t1 = Exp(child);
			if(t1 == NULL)
				return NULL;
			if(t1->kind != 2){
				printf("Error type 13 at line %d: Illegal use of '.'\n",child->lineno);
				return NULL;
			}
			FieldList* fl = t1->u.structure->next;
			child2 = child2->next;
			while(fl != NULL)
			{
				if(strcmp(fl->name,child2->value)==0)
					return fl->type;
				fl=fl->next;
			}
			printf("Error type 14 at line %d: Non-existent field '%s‘\n",child2->lineno,child2->value);
			return NULL;
		}

	}
	else if(strcmp(child->name,"LP")==0)	//()
	{
		child = child->next;
		return Exp(child);
	}
	else if(strcmp(child->name,"MINUS")==0)	//-
	{
		child = child->next;
		Type* t = Exp(child);
		if(t == NULL)
			return NULL;
		if(t->kind!=0){
			printf("Error type 7 at line %d: Operands type mismatched\n",child->lineno);
			return NULL;
		}
		return t;
	}
	else if(strcmp(child->name,"NOT")==0)	//not
	{
		child = child->next;
		Type* t = Exp(child);
		if(t == NULL)
			return NULL;
		if(t->kind == 0 && t->u.basic == int_type)
			return t;
		printf("Error type 7 at line %d: Operands type mismatched\n",child->lineno);
		return NULL;
	}
	else if(strcmp(child->name,"ID") ==0 && child->next != NULL){
		FieldList* f1 = getVarByName(child->value);
		Fundef* f = getFuncByName(child->value);
		if(f1 != NULL && f == NULL){
			printf("Error type 11 at line %d: '%s' is not a function\n",child->lineno,child->value);
			return NULL;
		}
		if(f == NULL||!f->hasDefined){
			printf("Error type 2 at line %d: Undefined function '%s'\n",child->lineno,child->value);
			return NULL;
		}
		FieldList* param = f->args_list;
		child = child->next->next;
		if(strcmp(child->name,"RP") == 0){
			if(param != NULL)
			{
				printf("Error type 9 at line %d: Function %s is not applicable for the arguments\n",child->lineno,f->name);
			}
		}
		else{
			if(!Args(child,param)){
				printf("Error type 9 at line %d: Function %s is not applicable for the arguments\n",child->lineno,f->name);
			}
		}
		return f->ret;
	}
	else if(strcmp(child->name,"ID") == 0)
	{
		// printf("enter ID\n");
		FieldList* f = getVarByName(child->value);
		// printf("quchulaide kind:%d,u.basic:%d\n",f->type->kind,f->type->u.basic);
	//	printf("ID:%s\n",f->name);
		if(f == NULL)
		{
			printf("Error type 1 at line %d: Undefined variable '%s'\n",child->lineno,child->value);
			return NULL;
		}
		return f->type;
	}
	else if(strcmp(child->name,"INT")==0)
	{
		// printf("enter int\n");
		Type* t = malloc(sizeof(struct Type_));
		t->kind = BASIC;
		t->u.basic = int_type;
		return t;
	}
	else if(strcmp(child->name,"FLOAT")==0)
	{
		Type* t = malloc(sizeof(struct Type_));
		t->kind = BASIC;
		t->u.basic=float_type;
		return t;
	}

}

bool Args(struct Node* root,FieldList* f){
	// printf("Enter Args\n");
	if(root == NULL&&f == NULL){
		return true;
	}
	else if(root == NULL||f == NULL){
		return false;
	}
	struct Node *child = root->children;
	Type* t = Exp(child);
	if(t == NULL){
		return true;
	}
	if(!typeEqual(t,f->type)){
		return false;
	}
	if(child->next == NULL && f->next == NULL){
		return true;
	}
	else if(child->next == NULL || f->next == NULL){
		return false;
	}
	return Args(child->next->next,f->next);
}
