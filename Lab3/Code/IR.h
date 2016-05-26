#ifndef _IR_H
#define _IR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONSTANT 2

typedef struct Operand_ Operand;
typedef struct InterCode_ InterCode;
typedef struct Label_No_ Label_No;
typedef struct InterCodes_ InterCodes;

struct Operand_ {
    enum {TEMPVAR,VARIABLE,CONS,VADDRESS,LABEL,FUNCTION,TADDRESS} kind;
    union {
        int var_no;
        char* value;
        Operand* name;
    }u;
    Operand* next;
};

struct InterCode_ {
    enum {ASSIGN_K,ADD_K,SUB_K,MUL_K,DIV_K,RETURN_K,LABEL_K,GOTO_K,IFGOTO_K,\
        READ_K,WRITE_K,CALL_K,ARG_K,FUNCTION_K,PARAM_K,DEC_K,RIGHTAT_K} kind;
    union {
        struct{			//return ,label,goto,read,write,arg, function
			Operand* op;		//param
		} one;
		struct{
			Operand *left,*right;	//call
		} assign;	//assign	rightat
		struct{
			Operand *result,*op1,*op2;
		} binop;			//add sub mul div
		struct{
			Operand *t1;
			char *op;
			Operand *t2,*label;
		} triop;			//if_goto
		struct{
			Operand *op;
			int size;
		} dec; //dec
    }u;
};

struct InterCodes_ {
    InterCode* code;
    InterCodes* prev;
    InterCodes* next;
};

extern InterCodes *code_head,*code_tail;
extern int varCount,labCount;

void insertCode(InterCodes* code);
void deleteCode(InterCodes* code);
void printCode(char* filename);
void printOp(Operand* op,FILE* fp);

void insertCodeIntoIR(InterCode* code);
#endif
