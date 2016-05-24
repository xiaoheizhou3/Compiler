#include "IR.h"

InterCodes *code_head = NULL,*code_tail = NULL;
int varCount=1,labCount=1;

void insertCode(InterCodes* code){
    if(code_head == NULL){
        code_head = code;
        code_tail = code_head;
    }
    else{
        code->next = NULL;
        code->prev = code_tail;
        code_tail->next = code;
        code_tail = code_tail->next;
    }
}

void deleteCode(InterCodes* code) {
	if(code == code_head && code == code_tail){
		code_head = NULL;
		code_tail = NULL;
	}
	else if(code == code_head) {
		if(code->next != NULL)
			code->next->prev = NULL;
		code_head = code->next;
	}
	else if(code == code_tail) {
		if(code->prev != NULL)
			code->prev->next = NULL;
		code_tail = code->prev;
	}
	else {
		if(code->next != NULL)
			code->next->prev = code->prev;
		if(code->prev != NULL)
			code->prev->next = code->next;
	}
}

void printCode(char* filename) {
	FILE *fp=fopen(filename,"w");
	if(fp == NULL) {
		printf("open file error\n");
		return;
	}
	InterCodes* index = code_head;
	while(index != NULL)	{
		switch(index->code->kind) {
			case ASSIGN_K:
				printOp(index->code->u.assign.left,fp);
				fputs(":=	",fp);
				printOp(index->code->u.assign.right,fp);
				break;
			case ADD_K:
				printOp(index->code->u.binop.result,fp);
				fputs(":=	",fp);
				printOp(index->code->u.binop.op1,fp);
				fputs("+	",fp);
				printOp(index->code->u.binop.op2,fp);
				break;
			case SUB_K:
				printOp(index->code->u.binop.result,fp);
				fputs(":=	",fp);
				printOp(index->code->u.binop.op1,fp);
				fputs("-	",fp);
				printOp(index->code->u.binop.op2,fp);
				break;
			case MUL_K:
				printOp(index->code->u.binop.result,fp);
				fputs(":=	",fp);
				printOp(index->code->u.binop.op1,fp);
				fputs("*	",fp);
				printOp(index->code->u.binop.op2,fp);
				break;
			case DIV_K:
				printOp(index->code->u.binop.result,fp);
				fputs(":=	",fp);
				printOp(index->code->u.binop.op1,fp);
				fputs("/	",fp);
				printOp(index->code->u.binop.op2,fp);
				break;
			case RETURN_K:
				fputs("RETURN	",fp);
				printOp(index->code->u.one.op,fp);
				break;
			case LABEL_K:
				fputs("LABEL	",fp);
				printOp(index->code->u.one.op,fp);
				fputs(":	",fp);
				break;
			case GOTO_K:
				fputs("GOTO	",fp);
				printOp(index->code->u.one.op,fp);
				break;
			case IFGOTO_K:
				fputs("IF	",fp);
				printOp(index->code->u.triop.t1,fp);
				fputs(index->code->u.triop.op,fp);
				fputs("	",fp);
				printOp(index->code->u.triop.t2,fp);
				fputs("GOTO	",fp);
				printOp(index->code->u.triop.label,fp);
				break;
			case READ_K:
				fputs("READ	",fp);
				printOp(index->code->u.one.op,fp);
				break;
			case WRITE_K:
				fputs("WRITE	",fp);
				printOp(index->code->u.one.op,fp);
				break;
			case CALL_K:
				printOp(index->code->u.assign.left,fp);
				fputs(":=	CALL	",fp);
				printOp(index->code->u.assign.right,fp);
				break;
			case ARG_K:
				fputs("ARG	",fp);
				printOp(index->code->u.one.op,fp);
				break;
			case FUNCTION_K:
				fputs("FUNCTION	",fp);
				printOp(index->code->u.one.op,fp);
				fputs(":",fp);
				break;
			case PARAM_K:
				fputs("PARAM	",fp);
				printOp(index->code->u.one.op,fp);
				break;
			case RIGHTAT_K:
				printOp(index->code->u.assign.left,fp);
				fputs(":=	&",fp);
				printOp(index->code->u.assign.right,fp);
				break;
			case DEC_K:
				fputs("DEC	",fp);
				printOp(index->code->u.dec.op,fp);
				char size[32];
				sprintf(size,"%d",index->code->u.dec.size);
				fputs(size,fp);
				break;
		}
		fputs("\n",fp);
		index = index->next;
	}
	fclose(fp);
}

void printOp(Operand* op,FILE* fp){
	if(op == NULL){
		fputs("t0	",fp);
		return;
	}

	char var[32];
	memset(var,0,sizeof(var));
	switch(op->kind){
		case TEMPVAR:
			fputs("t",fp);
			sprintf(var,"%d",op->u.var_no);
			fputs(var,fp);
			break;
		case VARIABLE:
			fputs(op->u.value,fp);
			break;
		case CONSTANT:
			fputs("#",fp);
			fputs(op->u.value,fp);
			break;
		case LABEL:
			fputs("label",fp);
			sprintf(var,"%d",op->u.var_no);
			fputs(var,fp);
			break;
		case FUNCTION:
			fputs(op->u.value,fp);
			break;
		case TADDRESS: fputs("*t",fp);
			sprintf(var,"%d",op->u.name->u.var_no);
			fputs(var,fp);
			break;
		case VADDRESS:
			fputs("*",fp);
			fputs(op->u.name->u.value,fp);
			break;
	}
	fputs("	",fp);
}

/*optimize code*/
void optIF(){
	InterCodes* c = code_head;
	while(c!=NULL){
		if(c->code->kind == IFGOTO_K){
			InterCode* c1=c->code;
			InterCode* c2=c->next->code;
			if(c2==NULL)continue;
			InterCode* c3=c->next->next->code;
			if(c3==NULL)continue;
			if(c2->kind==GOTO_K&&c3->kind==LABEL_K&&c1->u.triop.label==c3->u.one.op){
				c1->u.triop.label=c2->u.one.op;
				deleteCode(c->next);
				if(strcmp(c1->u.triop.op,"==")==0){
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,"!=");
				}
				else if(strcmp(c1->u.triop.op,"!=")==0){
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,"==");
				}
				else if(strcmp(c1->u.triop.op,">=")==0){
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,"<");
				}
				else if(strcmp(c1->u.triop.op,"<")==0){
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,">=");
				}
				else if(strcmp(c1->u.triop.op,">")==0){
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,"<=");
				}
				else if(strcmp(c1->u.triop.op,"<=")==0){
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,">");
				}
			}
		}
		else if(c->code->kind==GOTO_K){
			InterCode* c1=c->code;
			InterCode* c2=c->next->code;
			if(c2!=NULL&&c2->kind==LABEL_K&&c1->u.one.op==c2->u.one.op){
				c=c->next;
				deleteCode(c);
			}
		}
		c=c->next;
	}
}

//remove useless label
void rmLabel(){
	Label_No* head = malloc(sizeof(struct Label_No_));
	head->no = -1;
	head->next=NULL;
	Label_No* tail=head;
	InterCodes* c = code_head;
	//count label no.
	while(c!=NULL){
		if(c->code->kind==GOTO_K){
			Label_No* temp=malloc(sizeof(struct Label_No_));
			temp->no=c->code->u.one.op->u.var_no;
			temp->next=NULL;
			tail->next=temp;
			tail=temp;
		}
		else if(c->code->kind==IFGOTO_K){
			Label_No* temp=malloc(sizeof(struct Label_No_));
			temp->no=c->code->u.triop.label->u.var_no;
			temp->next=NULL;
			tail->next=temp;
			tail=temp;
		}
		c=c->next;
	}

	c=code_head;
	while(c!=NULL){
		if(c->code->kind==LABEL_K){
			int no=c->code->u.one.op->u.var_no;
			Label_No* ln=head;
			while(ln!=NULL){
				if(ln->no==no)break;
				ln=ln->next;
			}
			InterCodes* c1=c;
			c=c->next;
			if(ln==NULL)
				deleteCode(c1);
		}
		else
			c=c->next;
	}

	tail=NULL;
	while(head!=NULL){
		Label_No* ln=head;
		head=head->next;
		free(ln);
	}
}

int opEqual(Operand* op1,Operand* op2){
	if(op1==NULL||op2==NULL)
        return 0;
	if(op1->kind==TEMPVAR||op1->kind==VARIABLE||op1->kind==CONSTANT);
	else
        return 0;
	if(op1==op2)
        return 1;
	if(op1->kind==TEMPVAR&&op2->kind==TEMPVAR&&op1->u.var_no==op2->u.var_no)
		return 1;
	if(op1->kind==VARIABLE&&op2->kind==VARIABLE&&strcmp(op1->u.value,op2->u.value)==0)
		return 1;
	if(op1->kind==CONSTANT&&op2->kind==CONSTANT){
		int p1=atoi(op1->u.value);
		int p2=atoi(op2->u.value);
		if(p1==p2)
			return 1;
	}
	return 0;
}

void rddCode(){
	InterCodes* top=code_tail;
	InterCodes* bottom=code_tail;
	//from buttom to top
	while(1){
		//find a block
		bottom=top->prev;
		if(bottom==NULL)
            break;
		top=bottom->prev;
		if(top==NULL)break;
		while(top!=NULL){
			if(top->code->kind==RETURN_K||top->code->kind==GOTO_K||top->code->kind==IFGOTO_K||top->code->kind==CALL_K){
				top=top->next;
				break;
			}
			else if(top->code->kind==LABEL_K||top->code->kind==FUNCTION_K)
				break;
			top=top->prev;
		}
		if(top==NULL)top=code_head;
		if(bottom==top)continue;

		//deal with this block
		while(bottom!=NULL&&bottom!=top->prev){
			Operand* noAct=NULL;
			//InterCode p=bottom;
            InterCodes* p=bottom;
			int flag=0;
			if(p->code->kind==ASSIGN_K){
				if(p->code->u.assign.left->kind==VARIABLE&&!opEqual(p->code->u.assign.left,p->code->u.assign.right)){
					noAct=p->code->u.assign.left;
					flag=1;
				}
			}
			else if(p->code->kind==ADD_K||p->code->kind==SUB_K||p->code->kind==MUL_K||p->code->kind==DIV_K){
				if(p->code->u.binop.result->kind==VARIABLE&&!opEqual(p->code->u.binop.result,p->code->u.binop.op1)&&!opEqual(p->code->u.binop.result,p->code->u.binop.op2)){
					noAct=p->code->u.binop.result;
					flag=1;
				}
			}

			if(flag){
				p=p->prev;
				while(p!=NULL&&p!=top->prev){
					if(p->code->kind==ASSIGN_K||p->code->kind==CALL_K){
						if(opEqual(noAct,p->code->u.assign.right)||p->code->u.assign.left->kind==VADDRESS)break;
						if(p->code->u.assign.right->kind==VADDRESS||p->code->u.assign.right->kind==TADDRESS)break;
						if(opEqual(noAct,p->code->u.assign.left)){
							InterCodes* temp=p;
							p=p->prev;
							deleteCode(temp);
							continue;
						}
					}
					else if(p->code->kind==ADD_K||p->code->kind==SUB_K||p->code->kind==MUL_K||p->code->kind==DIV_K){
						if(opEqual(noAct,p->code->u.binop.op1)||opEqual(noAct,p->code->u.binop.op2))
							break;
						if(p->code->u.binop.result->kind==VADDRESS)break;
						if(p->code->u.binop.op1->kind==VADDRESS||p->code->u.binop.op1->kind==TADDRESS)break;
						if(p->code->u.binop.op2->kind==VADDRESS||p->code->u.binop.op2->kind==TADDRESS)break;
						if(opEqual(noAct,p->code->u.binop.result)){
							InterCodes* temp=p;
							p=p->prev;
							deleteCode(temp);
							continue;
						}
					}
					else if(p->code->kind==READ_K){
						if(p->code->u.one.op->kind==VADDRESS)break;
						if(opEqual(noAct,p->code->u.one.op)){
							InterCodes* temp=p;
							p=p->prev;
							deleteCode(temp);
							continue;
						}
					}
					else if(p->code->kind==WRITE_K||p->code->kind==ARG_K||p->code->kind==PARAM_K){
						if(opEqual(noAct,p->code->u.one.op)||p->code->u.one.op->kind==VADDRESS||p->code->u.one.op->kind==TADDRESS)
							break;
					}
					else if(p->code->kind==RIGHTAT_K){
						if(opEqual(noAct,p->code->u.assign.right)||p->code->u.assign.left->kind==VADDRESS)break;
						if(p->code->u.assign.right->kind==VADDRESS||p->code->u.assign.right->kind==TADDRESS)break;
						if(opEqual(noAct,p->code->u.assign.left)){
							InterCodes* temp=p;
							p=p->prev;
							deleteCode(temp);
							continue;
						}
					}
					p=p->prev;
				}
			}
			bottom=bottom->prev;
		}
	}
}

void lookCon(){
	InterCodes* h=code_head;
	while(h!=NULL){
		if(h->code->kind==ADD_K||h->code->kind==SUB_K||h->code->kind==MUL_K||h->code->kind==DIV_K){
			if(h->code->u.binop.result->kind==TEMPVAR && h->code->u.binop.op1->kind==CONSTANT && h->code->u.binop.op2->kind==CONSTANT){
				int c1=atoi(h->code->u.binop.op1->u.value);
				int c2=atoi(h->code->u.binop.op2->u.value);
				int r=0;
				switch(h->code->kind){
					case ADD_K:r=c1+c2;break;
					case SUB_K:r=c1-c2;break;
					case MUL_K:r=c1*c2;break;
					case DIV_K:r=c1/c2;break;
					default:break;
				}
				h->code->u.binop.result->kind=CONSTANT;
				h->code->u.binop.result->u.value=malloc(32);
				sprintf(h->code->u.binop.result->u.value,"%d",r);
				InterCodes* temp=h;
				h=h->next;
				deleteCode(temp);
				continue;
			}
		}
		h=h->next;
	}
}

void sameRight(){
	InterCodes* h=code_head;
	while(h!=NULL){
		if(h->code->kind==ADD_K||h->code->kind==SUB_K||h->code->kind==MUL_K||h->code->kind==DIV_K){
			if(h->code->u.binop.result->kind==TEMPVAR){
				Operand* r=h->code->u.binop.result;
				Operand* op1=h->code->u.binop.op1;
				Operand* op2=h->code->u.binop.op2;
				InterCodes* p = h->next;
				while(p!=NULL&&p->code->kind!=RETURN_K&&p->code->kind!=GOTO_K&&p->code->kind!=IFGOTO_K&&p->code->kind!=CALL_K&&p->code->kind!=LABEL_K&&p->code->kind!=FUNCTION_K){
					if(p->code->kind==READ_K&&(p->code->u.one.op->kind==TADDRESS||p->code->u.one.op->kind==VADDRESS||opEqual(p->code->u.one.op,op1)||opEqual(p->code->u.one.op,op2)))
                        break;
					if((p->code->kind==CALL_K||p->code->kind==ASSIGN_K||p->code->kind==RIGHTAT_K)&&(p->code->u.assign.left->kind==TADDRESS||p->code->u.assign.left->kind==VADDRESS||opEqual(p->code->u.assign.left,op1)||opEqual(p->code->u.assign.left,op2)))
                        break;
					if((p->code->kind==ADD_K||p->code->kind==SUB_K||p->code->kind==MUL_K||p->code->kind==DIV_K)&&(p->code->u.binop.result->kind==TADDRESS||p->code->u.binop.result->kind==VADDRESS||opEqual(p->code->u.binop.result,op1)||opEqual(p->code->u.binop.result,op2)))
                        break;
					if(p->code->kind==h->code->kind&&p->code->u.binop.result->kind==TEMPVAR&&opEqual(p->code->u.binop.op1,op1)&&opEqual(p->code->u.binop.op2,op2)){
						p->code->u.binop.result->u.var_no=r->u.var_no;
						InterCodes* temp=p;
						p=p->next;
						deleteCode(temp);
						continue;
					}
					p=p->next;
				}
			}
		}
		h=h->next;
	}
}
