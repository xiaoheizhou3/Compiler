#include "IR.h"

InterCodes *code_head = NULL,*code_tail = NULL;

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

void printOp(Operand* op,FILE* fp)
{
	if(op == NULL)
	{
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
