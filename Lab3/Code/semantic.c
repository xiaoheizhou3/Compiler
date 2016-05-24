#include "semantic.h"

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
	printf("enter program\n");
	initTables();
	printf("init succeed\n");
	ExtDefList(root->children);
	checkFunc();
}

void ExtDefList(struct Node* root){
	printf("enter extdeflist\n");
	struct Node* child = root->children;
	if(child != NULL){
		ExtDef(child);
		child = child->next;
		ExtDefList(child);
	}
}

void ExtDef(struct Node* root){
	printf("Enter ExtDef\n");
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
	printf("enter specifier\n");
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
	else{
		ret = StructSpecifier(child);
	}
	return ret;
}

Type* StructSpecifier(struct Node* root){
	printf("Enter StructSpecifier\n");
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
			if(i == 1){
				printf("Error type 16 at line %d: Duplicated name ‘%s’\n",name_of_struct->lineno,temp->name);
				return NULL;
			}
			return type;
		}
		child = child->next;
	}
	return NULL;
}

void ExtDecList(struct Node* root,Type* type){
	printf("Enter ExtDecList\n");
	struct Node* child = root->children;
	FieldList* f = VarDec(child,type,1);	//1:global variable
	if(f != NULL){
		if(f->type->kind==ARRAY){
			Operand* op = malloc(sizeof(struct Operand_));
			op->kind = TEMPVAR;
			op->u.var_no = varCount++;

			InterCode* deccode = malloc(sizeof(struct InterCode_));
			deccode->kind = DEC_K;
			deccode->u.dec.op = op;
			deccode->u.dec.size = typeSize(f->type);
			InterCodes* tempNodeOfdeccode = malloc(sizeof(struct InterCodes_));
			tempNodeOfdeccode->code = deccode;
			insertCode(tempNodeOfdeccode);

			Operand* v = malloc(sizeof(struct Operand_));
			v->kind = VARIABLE;
			v->u.value = f->name;
			InterCode* addrcode = malloc(sizeof(struct InterCode_));
			addrcode->kind = RIGHTAT_K;
			addrcode->u.assign.left = v;
			addrcode->u.assign.right = op;
			InterCodes* tempNodeOfAddr = malloc(sizeof(struct InterCodes_));
			tempNodeOfAddr->code = addrcode;
			insertCode(tempNodeOfAddr);
		}
	}
	child = child->next;
	if(child != NULL){
		child = child->next;
		ExtDecList(child,type);
	}
}

FieldList* VarDec(struct Node* root,Type* type,int from){
	printf("Enter VarDec\n");
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
	printf("Enter FunDec\n");
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
	if(strcmp(child->name,"VarList") ==  0){
		f->args_list = VarList(child);
	}
	return f;
}

FieldList* VarList(struct Node* root){
	printf("Enter VarList\n");
	struct Node* child = root->children;
	FieldList* f;
	f = ParamDec(child);
	child = child->next;
	if(child != NULL){
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
	printf("Enter ParamDec\n");
	struct Node *child = root->children;
	FieldList* f;
	Type* type;
	type = Specifier(child);
	f = VarDec(child->next,type,3);
	return f;
}

void CompSt(struct Node* root,Type* retype){
	printf("Enter CompSt\n");
	struct Node *child = root->children;
	child=child->next;
	DefList(child,1);
	child=child->next;
	StmtList(child,retype);
}

void StmtList(struct Node* root,Type* retype){
	printf("Enter StmtList\n");
	struct Node *child = root->children;
	if(child == NULL)
		return;
	Stmt(child,retype);
	child = child->next;
	StmtList(child,retype);
}

void Stmt(struct Node* n,Type* retype){
	printf("enter stmt\n");
	struct Node*child=n->children;
	if(child == NULL)
		return;
	if(strcmp(child->name,"Exp")==0){
		Exp(child,NULL);
		return;
	}
	else if(strcmp(child->name,"CompSt")==0){
		CompSt(child,retype);
		return;
	}
	else if(strcmp(child->name,"RETURN")==0){
		child=child->next;
		Operand* op=malloc(sizeof(struct Operand_));
		op->kind=TEMPVAR;
		op->u.var_no=varCount++;
		Type* t=Exp(child,op);
		if(retype==NULL||t==NULL)return;
		if(!typeEqual(retype,t)){
			printf("Error type 8 at line %d: The return type mismatched\n",child->lineno);
			return;
		}
		InterCode* code = malloc(sizeof(struct InterCode_));
		code->kind=RETURN_K;
		code->u.one.op=op;
		InterCodes* tempNodeOfcode = malloc(sizeof(struct InterCodes_));
		tempNodeOfcode->code = code;
		insertCode(tempNodeOfcode);
		return;
	}
	else if(strcmp(child->name,"IF")==0){
		child=child->next->next;
		//new temp
		Operand* lb1=malloc(sizeof(struct Operand_));
		lb1->kind=LABEL;
		lb1->u.var_no=labCount++;
		Operand* lb2=malloc(sizeof(struct Operand_));
		lb2->kind=LABEL;
		lb2->u.var_no=labCount++;
		Type* t=Exp_Cond(child,lb1,lb2);	//TODO:this function
		if(t!=NULL&&!((t->kind==0||t->kind==3)&&t->u.basic==int_type)){
			printf("Error type ? conditional statement wrong type\n");
		}
		//print label1
		InterCode* code1=malloc(sizeof(struct InterCode_));
		code1->kind=LABEL_K;
		code1->u.one.op=lb1;
		InterCodes* tempNodeOfcode1 = malloc(sizeof(struct InterCodes_));
		tempNodeOfcode1->code = code1;
		insertCode(tempNodeOfcode1);
		child=child->next->next;
		Stmt(child,retype);//code2

		InterCode* lb2code=malloc(sizeof(struct InterCode_));
		lb2code->kind=LABEL_K;
		lb2code->u.one.op=lb2;
		if(child->next!=NULL){
			Operand* lb3=malloc(sizeof(struct Operand_));
			lb3->kind=LABEL;
			lb3->u.var_no=labCount++;
			InterCode* code2=malloc(sizeof(struct InterCode_));
			code2->kind=GOTO_K;
			code2->u.one.op=lb3;
			InterCodes* tempNodeOfcode2 = malloc(sizeof(struct InterCodes_));
			tempNodeOfcode2->code = code2;
			insertCode(tempNodeOfcode2);			//goto label3
			InterCodes* tempNodeOflb2code = malloc(sizeof(struct InterCodes_));
			tempNodeOflb2code->code = lb2code;
			insertCode(tempNodeOflb2code);		//label2
			child=child->next->next;
			Stmt(child,retype);			//code3
			InterCode* lb3code=malloc(sizeof(struct InterCode_));
			lb3code->kind=LABEL_K;
			lb3code->u.one.op=lb3;
			InterCodes* tempNodeOflb3code = malloc(sizeof(struct InterCodes_));
			tempNodeOflb3code->code = lb3code;
			insertCode(tempNodeOflb3code);		//label3
		}
		else{
			InterCodes* tempNodeOflb2code = malloc(sizeof(struct InterCodes_));
			tempNodeOflb2code->code = lb2code;
			insertCode(tempNodeOflb2code);
		}
	}
	else if(strcmp(child->name,"WHILE")==0){
		Operand* lb1=malloc(sizeof(struct Operand_));
		lb1->kind=LABEL;
		lb1->u.var_no=labCount++;
		Operand* lb2=malloc(sizeof(struct Operand_));
		lb2->kind=LABEL;
		lb2->u.var_no=labCount++;
		Operand* lb3=malloc(sizeof(struct Operand_));
		lb3->kind=LABEL;
		lb3->u.var_no=labCount++;
		child=child->next->next;

		InterCode* lb1code=malloc(sizeof(struct InterCode_));
		lb1code->kind=LABEL_K;
		lb1code->u.one.op=lb1;
		InterCodes* tempNodeOflb1code = malloc(sizeof(struct InterCodes_));
		tempNodeOflb1code->code = lb1code;
		insertCode(tempNodeOflb1code);		//label 1
		Type* t=Exp_Cond(child,lb2,lb3);	//code1

		if(t!=NULL&&!((t->kind==0||t->kind==3)&&t->u.basic==int_type)){
			printf("Error type ? conditional statement wrong type\n");
		}

		InterCode* lb2code=malloc(sizeof(struct InterCode_));
		lb2code->kind=LABEL_K;
		lb2code->u.one.op=lb2;
		InterCodes* tempNodeOflb2code = malloc(sizeof(struct InterCodes_));
		tempNodeOflb2code->code = lb2code;
		insertCode(tempNodeOflb2code);
		child=child->next->next;
		Stmt(child,retype);			//code2
		InterCode* gotolb1=malloc(sizeof(struct InterCode_));
		gotolb1->kind=GOTO_K;
		gotolb1->u.one.op=lb1;
		InterCodes* tempNodeOfgotolb1 = malloc(sizeof(struct InterCodes_));
		tempNodeOfgotolb1->code = gotolb1;
		insertCode(tempNodeOfgotolb1);
		InterCode* lb3code=malloc(sizeof(struct InterCode_));
		lb3code->kind=LABEL_K;
		lb3code->u.one.op=lb3;
		InterCodes* tempNodeOflb3code = malloc(sizeof(struct InterCodes_));
		tempNodeOflb3code->code = lb3code;
		insertCode(tempNodeOflb3code);
	}
}

FieldList* DefList(struct Node* root,int from){
	printf("Enter DefList\n");
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
		while(t->next!=NULL){
			t=t->next;
		}
		t->next=DefList(child,from);
	}
	else
		f = DefList(child,from);
	return f;
}

FieldList* Def(struct Node* root,int from){
	printf("Enter Def\n");
	struct Node* child = root->children;
	FieldList* f;
	Type* type = Specifier(child);
	child = child->next;
	f = DecList(child,type,from);	//inhre
	return f;
}

FieldList* DecList(struct Node* root,Type* type,int from){
	printf("Enter DecList\n");
	struct Node *child = root->children;
	FieldList* f;
	f = Dec(child,type,from);
	child = child->next;
	if(child != NULL){
		child = child->next;
		FieldList* p = f;
		if(p != NULL){
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
	printf("Enter Dec\n");
	struct Node *child = root->children;
	FieldList* f;
	f = VarDec(child,type,from);

	if(f->type->kind==1&&from==1)
	{
		//array space
		Operand* op=malloc(sizeof(struct Operand_));
		op->kind=TEMPVAR;
		op->u.var_no=varCount++;

		InterCode* deccode=malloc(sizeof(struct InterCode_));
		deccode->kind=DEC_K;
		deccode->u.dec.op=op;
		deccode->u.dec.size=typeSize(f->type);
		InterCodes* tempNodeOfdeccode = malloc(sizeof(struct InterCodes_));
		tempNodeOfdeccode->code = deccode;
		insertCode(tempNodeOfdeccode);

		Operand* v=malloc(sizeof(struct Operand_));
		v->kind=VARIABLE;
		v->u.value=f->name;

		InterCode* addrcode=malloc(sizeof(struct InterCode_));
		addrcode->kind=RIGHTAT_K;
		addrcode->u.assign.left=v;
		addrcode->u.assign.right=op;
		InterCodes* tempNodeOfaddrcode = malloc(sizeof(struct InterCodes_));
		tempNodeOfaddrcode->code = deccode;
		insertCode(tempNodeOfaddrcode);
		// insertCode(addrcode);
	}
	if(f==NULL)
		return NULL;

	child = child->next;
	if(child != NULL){		//assignop =
		if(from == 2){	//struct cannot be initialized
			printf("Error type 15 at line %d: be initialized field ‘%s’\n",child->lineno,f->name);
			return f;
		}
		Operand* place=malloc(sizeof(struct Operand_));
		place->kind=VARIABLE;
		place->u.value=f->name;
		child = child->next;
		Type* t = Exp(child,place);
		if(t != NULL && type != NULL && !typeEqual(type,t)){
			printf("Error type 5 at line %d: Type mismatched for assignment\n",child->lineno);
		}
		if(place->kind!=VARIABLE||place->u.value!=f->name){
			Operand* left=malloc(sizeof(struct Operand_));
			left->kind=VARIABLE;
			left->u.value=f->name;

			InterCode* asscode=malloc(sizeof(struct InterCode_));
			asscode->kind=ASSIGN_K;
			asscode->u.assign.left=left;
			asscode->u.assign.right=place;
			InterCodes* tempNodeOfasscode = malloc(sizeof(struct InterCodes_));
			tempNodeOfasscode->code = asscode;
			insertCode(tempNodeOfasscode);
		}
	}
	return f;
}

Type* Exp(struct Node *n,Operand* place){
	printf("enter exp\n");
	struct Node *child = n->children;
	if(strcmp(child->name,"Exp")==0){
		struct Node *child2 = child->next;
		if(strcmp(child2->name,"ASSIGNOP") == 0){
			//left value
			struct Node *leftChild=child->children;
			Type* leftType=NULL;
			//new temp
			Operand* leftOp = malloc(sizeof(struct Operand_));
			leftOp->kind=TEMPVAR;
			leftOp->u.var_no=varCount++;
			//because of associative property and priority,it is right
			if(strcmp(leftChild->name,"ID") == 0 && leftChild->next == NULL)
				leftType = Exp(child,leftOp);
			else if(strcmp(leftChild->name,"Exp") == 0 && leftChild->next != NULL && strcmp(leftChild->next->name,"LB") == 0)	//array
				leftType = Exp(child,leftOp);
			else if(strcmp(leftChild->name,"Exp") == 0 && leftChild->next != NULL && strcmp(leftChild->next->name,"DOT") == 0)	//struct
				leftType = Exp(child,leftOp);
			else{
				printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable\n",child->lineno);
				return NULL;
			}

			child2 = child2->next;
			//new temp
			Operand* rightOp = malloc(sizeof(struct Operand_));
			rightOp->kind = TEMPVAR;
			rightOp->u.var_no = varCount++;
			int temp_no = rightOp->u.var_no;		//trick
			Type* rightType = Exp(child2,rightOp);

			if(leftType == NULL || rightType == NULL)
				return NULL;
			if(typeEqual(leftType,rightType)){

				if(rightOp->kind == TEMPVAR && rightOp->u.var_no == temp_no && (leftOp->kind == TEMPVAR || leftOp->kind == VARIABLE))
					memcpy(rightOp,leftOp,sizeof(struct Operand_));
				else{
					InterCode* code1 = malloc(sizeof(struct InterCode_));
					code1->kind = ASSIGN_K;
					code1->u.assign.left = leftOp;
					code1->u.assign.right = rightOp;
					InterCodes* tempNodeOfAssign = malloc(sizeof(struct InterCodes_));
					tempNodeOfAssign->code = code1;
					insertCode(tempNodeOfAssign);
				}
				InterCode* code2 = malloc(sizeof(struct InterCode_));
				code2->kind = ASSIGN_K;
				code2->u.assign.left = place;
				code2->u.assign.right = rightOp;
				if(place != NULL){
					InterCodes* tempNodeOfcode2 = malloc(sizeof(struct InterCodes_));
					tempNodeOfcode2->code = code2;
					insertCode(tempNodeOfcode2);
				}
				return leftType;
			}
			else{
				printf("Error type 5 at line %d: Type mismatched\n",child->lineno);
				return NULL;
			}
		}
		else if(strcmp(child2->name,"PLUS")==0||strcmp(child2->name,"MINUS")==0||strcmp(child2->name,"STAR")==0||strcmp(child2->name,"DIV")==0){
			//new temp
			Operand* op1 = malloc(sizeof(struct Operand_));
			op1->kind = TEMPVAR;
			op1->u.var_no = varCount++;
			Type* t = Exp(child,op1);

			//new temp
			Operand* op2 = malloc(sizeof(struct Operand_));
			op2->kind = TEMPVAR;
			op2->u.var_no = varCount++;
			struct Node *child3 = child2;
			child2 = child2->next;
			Type* t2 = Exp(child2,op2);
			if(t==NULL||t2==NULL)
				return NULL;
			else if((t->kind==0||t->kind==3)&&(t2->kind==0||t2->kind==3)&&t->u.basic==t2->u.basic){
				if(place==NULL)return t;
				InterCode* code = malloc(sizeof(struct InterCode_));
				code->u.binop.result = place;
				code->u.binop.op1 = op1;
				code->u.binop.op2 = op2;
				if(strcmp(child3->name,"PLUS")==0)
					code->kind=ADD_K;
				else if(strcmp(child3->name,"MINUS")==0)
					code->kind=SUB_K;
				else if(strcmp(child3->name,"STAR")==0)
					code->kind=MUL_K;
				else if(strcmp(child3->name,"DIV")==0)
					code->kind=DIV_K;
				if(place!=NULL){
					InterCodes* tempNodeOfcode = malloc(sizeof(struct InterCodes_));
					tempNodeOfcode->code = code;
					insertCode(tempNodeOfcode);
				}
				return t;
			}
			else{
				printf("Error type 7 at line %d: Operands type mismatched\n",child->lineno);
				return NULL;
			}
		}
		else if(strcmp(child2->name,"AND")==0||strcmp(child2->name,"OR")==0||strcmp(child2->name,"RELOP")==0){
			//new temp
			Operand* lb1=malloc(sizeof(struct Operand_));
			lb1->kind=LABEL;
			lb1->u.var_no=labCount++;
			Operand* lb2=malloc(sizeof(struct Operand_));
			lb2->kind=LABEL;
			lb2->u.var_no=labCount++;

			if(place!=NULL){
				InterCode* code0=malloc(sizeof(struct InterCode_));
				code0->kind=ASSIGN_K;
				code0->u.assign.left=place;
				Operand* c0=malloc(sizeof(struct Operand_));
				c0->kind=CONSTANT;
				c0->u.value=malloc(32);
				strcpy(c0->u.value,"0");
				code0->u.assign.right=c0;
				InterCodes* tempNodeOfAnd = malloc(sizeof(struct InterCodes_));
				tempNodeOfAnd->code = code0;
				insertCode(tempNodeOfAnd);	//code0
			}
			Type* t=Exp_Cond(n,lb1,lb2);	//code1

			InterCode* lb1code=malloc(sizeof(struct InterCode_));
			lb1code->kind=LABEL_K;
			lb1code->u.one.op=lb1;
			InterCodes* tempNodeOflb1code = malloc(sizeof(struct InterCodes_));
			tempNodeOflb1code->code = lb1code;
			insertCode(tempNodeOflb1code);	//label 1

			Operand* c1 = malloc(sizeof(struct Operand_));
			c1->kind = CONSTANT;
			c1->u.value = malloc(32);
			strcpy(c1->u.value,"1");
			InterCode* code2=malloc(sizeof(struct InterCode_));
			code2->kind = ASSIGN_K;
			code2->u.assign.left = place;
			code2->u.assign.right = c1;
			if(place!=NULL){
				InterCodes* tempNodeOfcode2 = malloc(sizeof(struct InterCodes_));
				tempNodeOfcode2->code = code2;
				insertCode(tempNodeOfcode2);		//code2
			}
			InterCode* lb2code=malloc(sizeof(struct InterCode_));
			lb2code->kind=LABEL_K;
			lb2code->u.one.op=lb2;
			InterCodes* tempNodeOflb2code = malloc(sizeof(struct InterCodes_));
			tempNodeOflb2code->code = lb2code;
			insertCode(tempNodeOflb2code);
			return t;
		}
		else if(strcmp(child2->name,"LB")==0){
			Operand* aop=malloc(sizeof(struct Operand_));
			aop->kind=TEMPVAR;
			aop->u.var_no=varCount++;
			Type* t1=Exp(child,aop);	//array
			//child's children must be a ID,
			if(t1==NULL)return NULL;
			if(t1->kind!=1){
				printf("Error type 10 at line %d: '",child->lineno);
				printNode(child);
				printf("' must be an array\n");
				return NULL;
			}

			Operand* subs = malloc(sizeof(struct Operand_));
			subs->kind = TEMPVAR;
			subs->u.var_no = varCount++;

			child2=child2->next;
			Type* t2=Exp(child2,subs);
			//printf("array back\n");
			if(t2==NULL)
				return NULL;
			if(!((t2->kind==0||t2->kind==3)&&t2->u.basic==int_type)){
				printf("Error type 12 at line %d: Operands type mistaken\n",child2->lineno);
				return NULL;
			}

			Operand* offset=malloc(sizeof(struct Operand_));
			offset->kind = TEMPVAR;
			offset->u.var_no = varCount++;

			Operand* intsize = malloc(sizeof(struct Operand_));
			intsize->kind=CONSTANT;
			intsize->u.value=malloc(32);
			memset(intsize->u.value,0,32);
			Type* subtype=t1->u.array.elem;
			sprintf(intsize->u.value,"%d",typeSize(subtype));

			InterCode* addrcode=malloc(sizeof(struct InterCode_));
			addrcode->kind=MUL_K;
			addrcode->u.binop.result=offset;
			addrcode->u.binop.op1=subs;
			addrcode->u.binop.op2=intsize;
			InterCodes* tempNodeOfaddrcode = malloc(sizeof(struct InterCodes_));
			tempNodeOfaddrcode->code = addrcode;
			insertCode(tempNodeOfaddrcode);

			Operand* temp=malloc(sizeof(struct Operand_));
			temp->kind=TEMPVAR;
			temp->u.var_no=varCount++;

			InterCode* code1=malloc(sizeof(struct InterCode_));
			code1->kind=ADD_K;

			if(subtype->kind==0){
				place->kind=TADDRESS;
				place->u.name=temp;
				code1->u.binop.result=temp;
			}
			else
				code1->u.binop.result=place;

			code1->u.binop.op1=aop;
			code1->u.binop.op2=offset;
			InterCodes* tempNodeOfCode1 = malloc(sizeof(struct InterCodes_));
			tempNodeOfCode1->code = code1;
			insertCode(tempNodeOfCode1);

			return t1->u.array.elem;
		}
		else if(strcmp(child2->name,"DOT")==0){
			Operand* op1 = malloc(sizeof(struct Operand_));
			op1->kind = TEMPVAR;
			op1->u.var_no = varCount++;
			Type* t1 = Exp(child,op1);
			if(t1 == NULL)return NULL;
			if(t1->kind!=2){
				printf("Error type 13 at line %d: Illegal use of '.'\n",child->lineno);
				return NULL;
			}
			FieldList* fl=t1->u.structure->next;
			child2=child2->next;
			int size=0;
			while(fl!=NULL){
				if(strcmp(fl->name,child2->value)==0){
					if(size==0){
						if(op1->kind==VARIABLE||op1->kind==VADDRESS){
							if(fl->type->kind==0){
								place->kind=VADDRESS;
								place->u.name=op1;
							}
							else{
								place->kind=VARIABLE;
								place->u.value=op1->u.value;
							}
						}
						else if(op1->kind==TEMPVAR||op1->kind==TADDRESS){
							if(fl->type->kind==0){
								place->kind=TADDRESS;
								place->u.name=op1;
							}
							else{
								place->kind=TEMPVAR;
								place->u.var_no=op1->u.var_no;
							}
						}
					}
					else{
						Operand* temp = malloc(sizeof(struct Operand_));
						temp->kind=TEMPVAR;
						temp->u.var_no = varCount++;

						Operand* conOp = malloc(sizeof(struct Operand_));
						conOp->kind=CONSTANT;
						conOp->u.value=malloc(32);
						memset(conOp->u.value,0,32);
						sprintf(conOp->u.value,"%d",size);

						InterCode* code1=malloc(sizeof(struct InterCode_));
						code1->kind=ADD_K;

						if(fl->type->kind==0){
							place->kind=TADDRESS;
							place->u.name=temp;
							code1->u.binop.result=temp;
						}
						else
							code1->u.binop.result=place;
						code1->u.binop.op1=op1;
						code1->u.binop.op2=conOp;
						InterCodes* tempNodeOfCode1 = malloc(sizeof(struct InterCodes_));
						tempNodeOfCode1->code = code1;
						insertCode(tempNodeOfCode1);
					}
					return fl->type;
				}
				size += typeSize(fl->type);
				fl=fl->next;
			}
			printf("Error type 14 at line %d: Un-existed field '%s‘\n",child2->lineno,child2->value);
			return NULL;
		}

	}
	else if(strcmp(child->name,"LP")==0){
		child=child->next;
		return Exp(child,place);
	}
	else if(strcmp(child->name,"MINUS")==0){
		//new temp
		child=child->next;
		Operand* op=malloc(sizeof(struct Operand_));
		op->kind=TEMPVAR;
		op->u.var_no=varCount++;
		Type* t=Exp(child,op);
		if(t==NULL)
			return NULL;
		if(t->kind!=0&&t->kind!=3){
			printf("Error type 7 at line %d: Operands type mismatched1\n",child->lineno);
			return NULL;
		}

		//print code here
		Operand* op2=malloc(sizeof(struct Operand_));
		op2->kind=CONSTANT;
		op2->u.value=malloc(4);
		strcpy(op2->u.value,"0");
		InterCode* code=malloc(sizeof(struct InterCode_));
		code->kind=SUB_K;
		code->u.binop.result=place;
		code->u.binop.op1=op2;
		code->u.binop.op2=op;
		if(place!=NULL){
			InterCodes* tempNodeOfcode = malloc(sizeof(struct InterCodes_));
			tempNodeOfcode->code = code;
			insertCode(tempNodeOfcode);
		}

		return t;
	}
	else if(strcmp(child->name,"NOT")==0){
		Operand* lb1=malloc(sizeof(struct Operand_));
		lb1->kind=LABEL;
		lb1->u.var_no=labCount++;
		Operand* lb2=malloc(sizeof(struct Operand_));
		lb2->kind=LABEL;
		lb2->u.var_no=labCount++;

		InterCode* code0=malloc(sizeof(struct InterCode_));
		code0->kind=ASSIGN_K;
		code0->u.assign.left=place;
		Operand* c0=malloc(sizeof(struct Operand_));
		c0->kind=CONSTANT;
		c0->u.value=malloc(32);
		strcpy(c0->u.value,"0");
		code0->u.assign.right=c0;
		if(place!=NULL){
			InterCodes* tempNodeOfcode0 = malloc(sizeof(struct InterCodes_));
			tempNodeOfcode0->code = code0;
			insertCode(tempNodeOfcode0);	//code0
		}
		Type* t=Exp_Cond(n,lb1,lb2);	//code1

		InterCode* lb1code=malloc(sizeof(struct InterCode_));
		lb1code->kind=LABEL_K;
		lb1code->u.one.op=lb1;
		InterCodes* tempNodeOflb1code = malloc(sizeof(struct InterCodes_));
		tempNodeOflb1code->code = lb1code;
		insertCode(tempNodeOflb1code);	//label 1

		Operand* c1 = malloc(sizeof(struct Operand_));
		c1->kind=CONSTANT;
		c1->u.value=malloc(32);
		strcpy(c1->u.value,"1");
		InterCode* code2 = malloc(sizeof(struct InterCode_));
		code2->kind=ASSIGN_K;
		code2->u.assign.left=place;
		code2->u.assign.right=c1;
		if(place!=NULL){
			InterCodes* tempNodeOfcode2 = malloc(sizeof(struct InterCodes_));
			tempNodeOfcode2->code = code2;
			insertCode(tempNodeOfcode2);		//code2
		}
		InterCode* lb2code=malloc(sizeof(struct InterCode_));
		lb2code->kind=LABEL_K;
		lb2code->u.one.op=lb2;
		InterCodes* tempNodeOflb2code = malloc(sizeof(struct InterCodes_));
		tempNodeOflb2code->code = lb2code;
		insertCode(tempNodeOflb2code);
		return t;
	}
	else if(strcmp(child->name,"ID")==0&&child->next!=NULL){

		FieldList* f1 = getVarByName(child->value);
		Fundef* f = getFuncByName(child->value);
		if(f1!=NULL&&f==NULL){
			printf("Error type 11 at line %d: '%s' must be a function\n",child->lineno,child->value);
			return NULL;
		}
		if(f==NULL||!f->hasDefined){
			printf("Error type 2 at line %d: Undefined function '%s'\n",child->lineno,child->value);
			return NULL;
		}
		FieldList* param = f->args_list;
		child=child->next;
		child=child->next;
		if(strcmp(child->name,"RP")==0){
			if(param!=NULL){
				printf("Error type 9 at line%d : The method '%s(",child->lineno,f->name);
				printparam(param);
				printf(")'is not applicable for the arguments '()'\n");
			}
			if(strcmp(f->name,"read")==0){
				InterCode* rpcode=malloc(sizeof(struct InterCode_));
				rpcode->kind=READ_K;
				rpcode->u.one.op=place;
				if(place!=NULL){
					InterCodes* tempNodeOfrpCode = malloc(sizeof(struct InterCodes_));
					tempNodeOfrpCode->code = rpcode;
					insertCode(tempNodeOfrpCode);		//read place
				}
			}
			else{
				Operand* fop=malloc(sizeof(struct Operand_));
				fop->kind=FUNCTION;
				fop->u.value=f->name;
				InterCode* cfcode=malloc(sizeof(struct InterCode_));
				cfcode->kind=CALL_K;
				cfcode->u.assign.left=place;//TODO:NULL?
				cfcode->u.assign.right=fop;
				InterCodes* tempNodeOfcfcode = malloc(sizeof(struct InterCodes_));
				tempNodeOfcfcode->code = cfcode;
				insertCode(tempNodeOfcfcode);			//call func
			}
		}
		else{
			Operand* arg_list_head=malloc(sizeof(struct Operand_));
			arg_list_head->next=NULL;
			if(!Args(child,param,arg_list_head)){
				printf("error type 9\n");
				printf("Error type 9 at line %d : The method '%s(",child->lineno,f->name);
				printparam(param);
				printf(")'is not applicable for the arguments '(");
				printf(")'\n");
			}
			else{
				if(strcmp(f->name,"write")==0){
					InterCode* wacode=malloc(sizeof(struct InterCode_));
					wacode->kind=WRITE_K;
					wacode->u.one.op=arg_list_head->next;
					InterCodes* tempNodeOfwacode = malloc(sizeof(struct InterCodes_));
					tempNodeOfwacode->code = wacode;
					insertCode(tempNodeOfwacode);			//write arg
				}
				else{
					arg_list_head = arg_list_head->next;
					while(arg_list_head!=NULL){
						InterCode* argcode=malloc(sizeof(struct InterCode_));
						argcode->kind=ARG_K;
						argcode->u.one.op=arg_list_head;
						InterCodes* tempNodeOfargcode = malloc(sizeof(struct InterCodes_));
						tempNodeOfargcode->code = argcode;
						insertCode(tempNodeOfargcode);		//Arg arg
						arg_list_head=arg_list_head->next;
					}
					Operand* fop = malloc(sizeof(struct Operand_));
					fop->kind=FUNCTION;
					fop->u.value=f->name;
					InterCode* cfcode = malloc(sizeof(struct InterCode_));
					cfcode->kind=CALL_K;
					cfcode->u.assign.left=place;
					cfcode->u.assign.right=fop;
					InterCodes* tempNodeOfcfcode = malloc(sizeof(struct InterCodes_));
					tempNodeOfcfcode->code = cfcode;
					insertCode(tempNodeOfcfcode);		//call func
				}
			}
		}
		return f->ret;
	}
	else if(strcmp(child->name,"ID")==0){
		FieldList* f=getVarByName(child->value);
		if(f==NULL){
			printf("Error type 1 at line %d: Undefined variable '%s'\n",child->lineno,child->value);
			return NULL;
		}
		//print code here
		place->kind=VARIABLE;
		place->u.value=child->value;

		return f->type;
	}
	else if(strcmp(child->name,"INT")==0){
		Type* t=malloc(sizeof(struct Type_));
		t->kind=3;
		t->u.basic=int_type;
		//print code here
		if(place!=NULL){
			place->kind=CONSTANT;
			place->u.value=child->value;
		}
		return t;
	}
	else if(strcmp(child->name,"FLOAT")==0){
		Type* t = malloc(sizeof(struct Type_));
		t->kind=3;
		t->u.basic=float_type;
		if(place!=NULL){
			place->kind=CONSTANT;
			place->u.value=child->value;
		}
		return t;
	}
	return NULL;
}

bool Args(struct Node* n,FieldList* f,Operand* arg_list){
	printf("enter args\n");
	if(n==NULL&&f==NULL)
		return true;
	else if(n==NULL||f==NULL)
		return false;
	Operand* t1=malloc(sizeof(struct Operand_));
	t1->kind=TEMPVAR;
	t1->u.var_no=varCount++;
	struct Node *child=n->children;
	Type* t=Exp(child,t1);			//code1
	t1->next=arg_list->next;
	arg_list->next=t1;
	if(t==NULL)
		return true;
	if(!typeEqual(t,f->type))
		return false;
	if(child->next==NULL&&f->next==NULL)
		return true;
	else if(child->next==NULL||f->next==NULL)
		return false;
	return Args(child->next->next,f->next,arg_list);	//code2
}

Type* Exp_Cond(struct Node *n,Operand* label_true,Operand* label_false){
	printf("enter exp_cond\n");
	struct Node *child=n->children;
	Type* type;
	if(strcmp(child->name,"Exp")==0){
		struct Node *child2=child->next;
		if(strcmp(child2->name,"RELOP")==0){
			//new temp
			Operand* t1=malloc(sizeof(struct Operand_));
			t1->kind=TEMPVAR;
			t1->u.var_no=varCount++;
			Operand* t2=malloc(sizeof(struct Operand_));
			t2->kind=TEMPVAR;
			t2->u.var_no=varCount++;

			struct Node* child3=child2;
			child2=child2->next;
			Type* tp=Exp(child,t1);	//code1
			Type* tp2=Exp(child2,t2);	//code2
			if(tp==NULL||tp2==NULL)
				return NULL;
			else if((tp->kind==0||tp->kind==3)&&(tp2->kind==0||tp2->kind==3)&&tp->u.basic==tp2->u.basic){
				InterCode* code3=malloc(sizeof(struct InterCode_));
				code3->kind=IFGOTO_K;
				code3->u.triop.t1=t1;
				code3->u.triop.op=child3->value;
				code3->u.triop.t2=t2;
				code3->u.triop.label=label_true;
				InterCodes* tempNodeOfcode3 = malloc(sizeof(struct InterCodes_));
				tempNodeOfcode3->code = code3;
				insertCode(tempNodeOfcode3);		//code3

				InterCode* gotolbf=malloc(sizeof(struct InterCode_));
				gotolbf->kind=GOTO_K;
				gotolbf->u.one.op=label_false;
				InterCodes* tempNodeOfgotolbf = malloc(sizeof(struct InterCodes_));
				tempNodeOfgotolbf->code = code3;
				insertCode(tempNodeOfgotolbf);
				return tp;
			}
			else{
				printf("Error type 7 at line %d: Operands type mismatched!\n",child->lineno);
				return NULL;
			}

		}
		else if(strcmp(child2->name,"AND")==0){
			//new temp
			Operand* lb1=malloc(sizeof(struct Operand_));
			lb1->kind=LABEL;
			lb1->u.var_no=labCount++;

			Type* t=Exp_Cond(child,lb1,label_false);	//code1

			InterCode* lb1code=malloc(sizeof(struct InterCode_));
			lb1code->kind=LABEL_K;
			lb1code->u.one.op=lb1;
			InterCodes* tempNodeOflb1code = malloc(sizeof(struct InterCodes_));
			tempNodeOflb1code->code = lb1code;
			insertCode(tempNodeOflb1code);		//label 1

			child2=child2->next;
			Type* t2=Exp_Cond(child2,label_true,label_false);	//code2
			if(t==NULL||t2==NULL)
				return NULL;
			else if((t->kind==0||t->kind==3)&&(t2->kind==0||t2->kind==3)&&t->u.basic==t2->u.basic)
								return t;
			else{
				printf("Error type 7 at line %d: Operands type mismatched!!\n",child->lineno);
				return NULL;
			}

		}
		else if(strcmp(child2->name,"OR")==0){
			//new temp
			Operand* lb1=malloc(sizeof(struct Operand_));
			lb1->kind=LABEL;
			lb1->u.var_no=labCount++;

			child2=child2->next;
			Type* t=Exp_Cond(child,label_true,lb1);	//code1

			InterCode* lb1code=malloc(sizeof(struct InterCode_));
			lb1code->kind=LABEL_K;
			lb1code->u.one.op=lb1;
			InterCodes* tempNodeOflb1code = malloc(sizeof(struct InterCodes_));
			tempNodeOflb1code->code = lb1code;
			insertCode(tempNodeOflb1code);		//label 1

			Type* t2=Exp_Cond(child2,label_true,label_false);	//code2
			if(t==NULL||t2==NULL)
				return NULL;
			else if((t->kind==0||t->kind==3)&&(t2->kind==0||t2->kind==3)&&t->u.basic==t2->u.basic)
				return t;
			else{
				printf("Error type 7 at line %d: Operands type mismatched!!!\n",child->lineno);
				return NULL;
			}

		}

	}
	if(strcmp(child->name,"NOT")==0){
		child=child->next;
		Type* t=Exp_Cond(child,label_false,label_true);
		if(t==NULL)return NULL;
		if(t->kind==0&&t->u.basic==int_type)return t;
		printf("Error type 7 at line %d: Operands type mismatched\n",child->lineno);
		return NULL;
	}
	Operand* t1=malloc(sizeof(struct Operand_));
	t1->kind=TEMPVAR;
	t1->u.var_no=varCount++;
	type=Exp(n,t1);		//code1
	InterCode* code2=malloc(sizeof(struct InterCode_));
	code2->kind = IFGOTO_K;
	code2->u.triop.t1=t1;
	code2->u.triop.op=malloc(2);
	strcpy(code2->u.triop.op,"!=");
	Operand* t2=malloc(sizeof(struct Operand_));
	t2->kind=CONSTANT;
	t2->u.value=malloc(32);
	strcpy(t2->u.value,"0");
	code2->u.triop.t2=t2;
	code2->u.triop.label=label_true;
	InterCodes* tempNodeOfcode2 = malloc(sizeof(struct InterCodes_));
	tempNodeOfcode2->code = code2;
	insertCode(tempNodeOfcode2);		//code2

	InterCode* gotolbf=malloc(sizeof(struct InterCode_));
	gotolbf->kind=GOTO_K;
	gotolbf->u.one.op=label_false;
	InterCodes* tempNodeOfgotolbf = malloc(sizeof(struct InterCodes_));
	tempNodeOfgotolbf->code = code2;
	insertCode(tempNodeOfgotolbf);
	return type;
}

void printparam(FieldList* f){
	while(f!=NULL){
		printtype(f->type);
		f=f->next;
	}
}

void printargs(struct Node *n){
	struct Node *child=n->children;
	Type* t=Exp(child,NULL);
	if(t==NULL)
		return;
	printtype(t);
	child=child->next;
	if(child==NULL)
		return;
	child=child->next;
	printargs(child);
}

void printtype(Type* t){
	if((t->kind==0||t->kind==3)&&t->u.basic==int_type)
		printf(" int ");
	else if((t->kind==0||t->kind==3)&&t->u.basic==float_type)
		printf(" float ");
	else if(t->kind==2)
		printf("struct %s ",t->u.structure->name);
	else if(t->kind==1){
		printtype(t->u.array.elem);
		printf("[]");
	}
}
