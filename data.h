/**
 *       @file      data.h
 *
 *       @section   
 *       @remark
 *
 *       @version   1.0
 *       @date      2014년 06월 01일 02시 29분 53초
 *
 *       @author    YOUR NAME (), 
 *
 *       @see       none
 *       @warning   none
 *       @todo      none
 *       @bug       none
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define  STACK_MEMORY_ALLOCATED_FAILED      -0x01
#define  STACK_PUSH_UNDERFLOW               -0x02
#define  STACK_PUSH_OVERFLOW                -0x03
#define  STACK_POP_UNDERFLOW                -0x04
#define  STACK_POP_OVERFLOW                 -0x05
#define  STACK_CLEAR_NOT_MATCH              -0x06
#define  STACK_LAST_UNDERFLOW               -0x07
#define  STACK_LAST_OVERFLOW                -0x08

typedef struct _Stack{
    void *space;
    
    int sp,
        max;
} Stack;
