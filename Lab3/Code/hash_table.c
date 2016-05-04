#include "hash_table.h"

unsigned int hash(char* name){
	unsigned int val = 0,i;
	for(;*name;++name){
		val = (val<<2) + *name;
		if(i = val & ~SYMBOL_TABLE_SIZE)
			val = (val^(i >> 12)) & SYMBOL_TABLE_SIZE;
	}
	return val;
}
/*
int read(){
	int input;
	scanf("%d",input);
	return input;
}

int write(int output){
	printf("%d\n",output);
	return 0;
}
*/
void initTables(){
	Fundef* read_func = malloc(sizeof(struct Fundef_));
	strcpy(read_func->name,"read");
	Type* read_ret = malloc(sizeof(struct Type_));
	read_ret->kind = BASIC;
	read_ret->u.basic = int_type;
	read_func->ret = read_ret;
	read_func->args_list = NULL;
	read_func->row = 0;
	read_func->samehash = NULL;
	read_func->hasDefined = true;
	Fundef* write_func = malloc(sizeof(struct Fundef_));
	strcpy(write_func->name,"write");
	Type* write_ret = malloc(sizeof(struct Type_));
	write_ret->kind = BASIC;
	write_ret->u.basic = int_type;
	write_func->ret = write_ret;
	FieldList* args_list = malloc(sizeof(struct FieldList_));
	args_list->next = NULL;
	args_list->samehash = NULL;
	strcpy(args_list->name,"output");
	Type* args_list_type = malloc(sizeof(struct Type_));
	args_list_type->kind = BASIC;
	args_list_type->u.basic = int_type;
	args_list->type = args_list_type;
	write_func->args_list = args_list;
	write_func->row = 0;
	write_func->samehash = NULL;
	write_func->hasDefined = true;
	int i = 0;
	for(;i < SYMBOL_TABLE_SIZE;i++){
		VarTable[i] = NULL;
		FuncTable[i] = NULL;
	}

	insertFunc(read_func,1);
	insertFunc(write_func,1);

}

FieldList* getVarByName(char* name){
	unsigned int index = hash(name);
	if(VarTable[index] != NULL){
		FieldList* temp = VarTable[index];
		while(temp != NULL){
			if(strcmp(temp->name,name) == 0){
				return temp;
			}
			temp = temp->samehash;
		}
	}
	return NULL;
}

Fundef* getFuncByName(char* name){
	unsigned int index = hash(name);
	if(FuncTable[index] != NULL){
		Fundef* temp = FuncTable[index];
		while(temp != NULL){
			if(strcmp(temp->name,name) == 0){
				return temp;
			}
			temp = temp->samehash;
		}
	}
	return NULL;
}

int insertVar(FieldList* var){
	FieldList* temp = getVarByName(var->name);
	if(temp == NULL){// the var not exist
		if(VarTable[hash(var->name)] == NULL){
			VarTable[hash(var->name)] = var;
		}
		else{
			var->samehash = VarTable[hash(var->name)]->samehash;
			VarTable[hash(var->name)]->samehash = var;
		}
		return 0;
	}
	return 1;
}

bool typeEqual(Type* t1,Type* t2){
	if(t1->kind != t2->kind){
		return false;
	}
	else{
		if(t1->kind == BASIC){
			if(t1->u.basic != t2->u.basic){
				return false;
			}
		}
		else if(t1->kind == ARRAY){
			return typeEqual(t1->u.array.elem,t2->u.array.elem);
		}
		else{
			if(t1->u.structure->name == NULL || t2->u.structure->name == NULL){
				return paramEqual(t1->u.structure->next,t2->u.structure->next);
			}
			if(strcmp(t1->u.structure->name,t2->u.structure->name) != 0){
				return false;
			}
		}
	}
	return true;
}

bool paramEqual(FieldList* f1,FieldList* f2){
	if(f1 == f2 && f1 == NULL){
		return true;
	}
	if(f1 == NULL || f2 == NULL){
		return false;
	}
	if(typeEqual(f1->type,f2->type)){
		return paramEqual(f1->next,f2->next);
	}
	else{
		return false;
	}
}

int insertFunc(Fundef* func,int hasDefined){
	if(func->name == NULL){
		return 0;
	}
	unsigned int index = hash(func->name);
	Fundef* temp = getFuncByName(func->name);
	if(temp == NULL){//the func not exist
		if(FuncTable[index] == NULL){
			FuncTable[index] = func;
			//deal the param of function
			FieldList* args_list = func->args_list;
			int i = 0;
			while(args_list != NULL){
				i = insertVar(args_list);
				if(i == 1){
					printf("Error type 3 at line %d: Redefined variable '%s'\n",func->row,args_list->name);
				}
				args_list = args_list->next;
			}
		}
		else{
			temp->samehash = FuncTable[index]->samehash;
			FuncTable[index]->samehash = temp;
		//	deal the param of function
		FieldList* args_list = func->args_list;
		int i = 0;
		while(args_list != NULL){
			i = insertVar(args_list);
			if(i == 1){
				printf("Error type 3 at line %d: Redefined variable '%s'\n",func->row,args_list->name);
			}
			args_list = args_list->next;
		}
		}
	}
	else{
		if(temp->hasDefined == 1){
			return 1;
		}
		else{
			if(!typeEqual(temp->ret,func->ret) || !paramEqual(temp->args_list,func->args_list)){
				return 2;
			}
			temp->hasDefined = hasDefined;
			return 0;
		}
	}
}
