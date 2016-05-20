#include "IR.h"

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

void deleteCode(InterCodes* code){
    
}
