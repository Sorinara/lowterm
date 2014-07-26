/**
 *      @file      data.c
 *
 *      @section   
 *      @remark
 *
 *      @version   1.0
 *      @date      2014년 05월 31일 23시 44분 54초
 *
 *      @author    YOUR NAME (), 
 *
 *      @see       none
 *      @warning   none
 *      @todo      none
 *      @bug       none
 */
#include "data.h"

int Stack_New(Stack *stack, const int stack_size)
{/*{{{*/
    if((stack->space = calloc(stack_size, sizeof(void *))) == NULL){
        /* assert(0); */
        return STACK_MEMORY_ALLOCATED_FAILED;
    }

    stack->max  = stack_size;
    stack->sp   = 0;

    return 0;
}/*}}}*/

void Stack_Delete(Stack *stack)
{/*{{{*/
    stack->sp   = 0;
    stack->max  = -1;

    free(stack->space);
}/*}}}*/

int Stack_Push(Stack *stack, void *data)
{/*{{{*/
    /* PUSH: sp가 0이상, max 미만일때는 OK */
    if(stack->sp < 0){
        /* assert(0); */
        return STACK_PUSH_UNDERFLOW;
    }
    if(stack->sp >= stack->max){
        /* assert(0); */
        return STACK_PUSH_OVERFLOW;
    }

    memcpy(stack->space + (stack->sp * sizeof(void *)), &data, sizeof(void *));
    stack->sp ++;

    return 0;
}/*}}}*/

int Stack_Clear(Stack *stack, void *data)
{/*{{{*/
    int i;

    for(i = 0;i < stack->max;i++){
        /* fprintf(stderr, "Compare Address : %p\n", stack->space + (i * sizeof(void *))); */
        if(memcmp(stack->space + (i * sizeof(void *)), &data, sizeof(void *)) == 0){
            // shift
            for(;i + 1 < stack->max;i ++){
                memcpy(stack->space + (i * sizeof(void *)), stack->space + ((i + 1) * sizeof(void *)) , sizeof(void *));
            }

            memset(stack->space + (i * sizeof(void *)), 0x00, sizeof(void *));
            /* fprintf(stderr, "Match Index     : %d\n", i); */
            stack->sp --;
            return i;
        }
    }

    return STACK_CLEAR_NOT_MATCH;
}/*}}}*/

int Stack_Last(Stack *stack, void **data_pointer)
{/*{{{*/
    /* POP과 같다 */
    if(stack->sp <= 0){
        /* assert(0); */
        return STACK_LAST_UNDERFLOW;
    }

    if(stack->sp > stack->max){
        /* assert(0); */
        return STACK_LAST_OVERFLOW;
    }

    if(data_pointer != NULL){
        *data_pointer = *(void **)(stack->space + ((stack->sp - 1) * sizeof(void *)));
    }

    return 0;
}/*}}}*/

void Stack_Print(Stack stack)
{/*{{{*/
    int i;

    for(i = 0;i < stack.max;i++){
         fprintf(stderr, "%p : %p\n", (stack.space + (i * sizeof(void *))), *(void **)(stack.space + (i * sizeof(void *))));
    }
}/*}}}*/

/*
int Stack_Pop(Stack *stack, void **data_pointer)
{
    // POP  : sp가 0초과, max이하일때는 OK
    if(stack->sp <= 0){
        // assert(0);
        return STACK_POP_UNDERFLOW;
    }

    if(stack->sp > stack->max){
        // assert(0);
        return STACK_POP_OVERFLOW;
    }

    if(data_pointer != NULL){
        *data_pointer = *(void **)(stack->space + (stack->sp * sizeof(void *)));
    }

    // sp 순서가 앞에 와야해.
    stack->sp --;
    memset(stack->space + (stack->sp * sizeof(void *)), 0x00, sizeof(void *));

    return 0;
}

int Stack_Test(void)
{
    Stack stack;
    int a,
        b,
        c;

    a = 1;
    b = 2;
    c = 3;

    Stack_New(&stack, 3);
    fprintf(stderr, "List %p = {%p, %p %p}\n", stack.space, &a, &b, &c);
    // underflow
    // Stack_Pop(&stack, NULL);

    Stack_Push(&stack, &a);
    Stack_Push(&stack, &b);
    Stack_Push(&stack, &c);
    // overflow
    // Stack_Push(&stack, &c);
    Stack_Print(stack);

    fprintf(stderr, "2---------------------------\n");
    Stack_Pop(&stack, NULL);
    Stack_Print(stack);
    fprintf(stderr, "3---------------------------\n");
    Stack_Push(&stack, &c);
    Stack_Print(stack);
    fprintf(stderr, "4---------------------------\n");
    Stack_Clear_Data(&stack, &b);
    Stack_Print(stack);
    fprintf(stderr, "5---------------------------\n");
    Stack_Clear_Data(&stack, &a);
    Stack_Print(stack);
    fprintf(stderr, "----------------------------\n");

    Stack_Delete(&stack);

    return 0;
}*/

/*
int main(int argc, const char *argv[])
{
    Stack_Test();
} */
