#ifndef _IR_H
#define _IR_H
#include <stdio.h>
#include <stdlib.h>

typedef struct Operand_* Operand;
typedef struct InterCode_ InterCode;
typedef struct InterCodes_ InterCodes;
struct Operand_ {
    enum {varible,contsant,address,/*to add*/} kind;
    union {
        int var_no;
        int value;
        /*to add*/
    } u;
};

struct InterCode_ {
    enum {assign,add,sub,mul,/*to add*/} kind;
    union {
        struct { Operand right,left;} assign;
        struct { Operand result,op1,op2;} binop;
        /*to add */
    };
};

struct InterCodes_ {
    InterCode code;
    InterCodes* prev;
    InterCodes* next;
};

InterCodes *code_head = NULL,*code_tail = NULL;

void insertCode(InterCodes* code);
void deleteCode(InterCodes* code);



#endif
