/**
 *      @file      key_filter_map.c
 *
 *      @section   
 *      @remark
 *
 *      @version   1.0
 *      @date      2014년 03월 19일 16시 57분 44초
 *
 *      @author    YOUR NAME (), 
 *
 *      @see       none
 *      @warning   none
 *      @todo      none
 *      @bug       none
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

/* MUST BE USED ONLY LIST POINTER */
/* pointer unit size is not 1(byte) but 4(sizeof(void *)) */
/* Parameter Type    : void*, void* */
#define LIST_DATA_POINTER_GET(list, saved_pointer)          ( saved_pointer = *(void **)list    )
/* Parameter Type    : void*, void* */
#define LIST_DATA_POINTER_SET(list, set_address)            ( *(void **)list = set_address      )

/* Parameter Type    : void*, int */
#define LIST_DISPLACEMENT_ADDRESS_GET(address, index)       ( (address) + ((sizeof(void *)) * (index))  )
/* Parameter Type    : void*, int, void* */
#define LIST_DISPLACEMENT_DATA_GET(list, index, data)       ( LIST_DATA_POINTER_GET(LIST_DISPLACEMENT_ADDRESS_GET(list, index), data) )

#define ALLOCATOR_VALID_ERROR_POINTER_NULL                  01
#define ALLOCATOR_VALID_ERROR_ELEMENT_COUNT_MINUS           02
#define ALLOCATOR_VALID_ERROR_ELEMENT_COUNT_ZERO            03
#define ALLOCATOR_VALID_ERROR_ELEMENT_SIZE_MINUS            04
#define ALLOCATOR_VALID_ERROR_ELEMENT_SIZE_ZERO             05
#define ALLOCATOR_VALID_ERROR                               010
#define ALLOCATOR_ERROR_CALLOC                              020
#define ALLOCATOR_ERROR_MASK                                070
#define ALLOCATOR_ERROR_SUB_VALUE                           007

/* 마지막자리는 ALLOCATOR_ERROR_*가 오는 위치라서 마지막을 비워둬야 한다.
 * 하위 3바이트 차지 */
#define LIST_VALID_PARAMETER_EMPTY_INT                      0x00

#define LIST_VALID_ERROR_NULL                               0100
#define LIST_VALID_ERROR_INDEX_MAX_MINUS                    0200
#define LIST_VALID_ERROR_INDEX_MAX_ZERO                     0300
#define LIST_VALID_ERROR_INDEX_RANGE_MINUS                  0400
#define LIST_VALID_ERROR_INDEX_RANGE_OVER                   0500
#define LIST_VALID_TYPE_MASK                                0700
#define LIST_VALID_SUB_VALUE                                0077
#define LIST_NEW_ERROR_VALID                                011000
#define LIST_NEW_ERROR_ALLOCATION                           012000
#define LIST_GET_ADDRESS_ERROR_VALID                        031000
#define LIST_GET_ADDRESS_ERROR_DATA_POINTER_NULL            032000
#define LIST_TYPE_MASK                                      077000
#define LIST_SUB_VALUE                                      000777
#define LINK_GET_ADDRESS_START_NULL                         01100000
#define LINK_GET_ADDRESS_DATA_NULL                          01200000
#define LINK_GET_ADDRESS_ERROR_GET                          01300000
#define LINK_SET_ADDRESS_START_NULL                         02100000
#define LINK_SET_ADDRESS_DATA_NOT_NULL                      02200000
#define LINK_SET_ADDRESS_ERROR_NEW                          02300000
#define LINK_SET_ADDRESS_ERROR_GET                          02400000
#define LINK_TYPE_MASK                                      07700000
#define LINK_SUB_VALUE                                      00077777
#define LINK_ROUNDS_ERROR_START_ADDRESS_NULL                0010000000
#define LINK_ROUNDS_ERROR_DEPTH_START_MINUS                 0020000000
#define LINK_ROUNDS_ERROR_DEPTH_START_MAX_MINUS             0030000000
#define LINK_ROUNDS_ERROR_DEPTH_START_MAX_ZERO              0040000000
#define LINK_ROUNDS_ERROR_DEPTH_OVER                        0050000000
#define LINK_ROUNDS_ERROR_ROUTE_LIST_NULL                   0060000000
#define LINK_ROUNDS_ERROR_INDEX_MAX_ARRAY_NULL              0070000000
#define LINK_ROUNDS_ERROR_ROUTINE_NULL                      0100000000
#define LINK_ROUNDS_ERROR_INFORMATION_DATA_NULL             0200000000
#define LINK_ROUNDS_ERROR_INDEX_OVERFLOW                    0300000000
#define LINK_ROUNDS_ERROR_ROUTINE_FAILD                     0400000000
#define LINK_ROUNDS_TYPE_MASK                               0770000000
#define LINK_ROUNDS_SUB_VALUE                               0007777777
#define NODE_POINTER_GET_ERROR_LINK_ROUNDS                  011000000000
#define NODE_POINTER_GET_ERROR_LIST_NULL                    012000000000
#define NODE_POINTER_SET_ERROR_LINK_ROUNDS                  021000000000
#define NODE_POINTER_SET_ERROR_LIST_NULL                    022000000000
#define NODE_POINTER_TYPE_MASK                              077000000000
#define NODE_POINTER_SUB_VALUE                              000777777777

/* 하위 1바이트 차지 */
#define PAGE_VALID_ERROR_PAGE_POINTER_NULL                  01
#define PAGE_VALID_ERROR_DATA_NULL                          02
#define PAGE_VALID_ERROR_DATA_SIZE_MINUS_ZERO               03
#define PAGE_VALID_ERROR_PARAMETER_COUNT_MINUS_ZERO         04
#define PAGE_VALID_ERROR_PARAMETER_COUNT_OVER               05
#define PAGE_EMTPY_ERROR_ADDRESS_NULL                       010
#define PAGE_EMTPY_ERROR_DEPTH_MINUS                        020
#define PAGE_EMPTY_ERROR_LIST_EMPTY                         030
#define PAGE_ERROR_MASK                                     070
#define PAGE_SUB_VALUE                                      007

#define PAGE_VALID_PARAMETER_EMPTY_POINTER                  (void *)0x1
#define PAGE_VALID_PARAMETER_EMPTY_INT                      0x1

/* -상위 1바이트
 * 첫째 8비트 : 콜된 함수
 * 둘째 8비트 : 콜된 함수타입 이걸 3바이트쪽으로 넘기면, node, parameter는 콜된 위치를 알기 애매
 * -다음 3바이트는 page/paramter/node의 오류값이 들어감. */
#define PAGE_INIT_ERROR                                     0x10000000
#define PAGE_GET_ERROR                                      0x20000000
#define PAGE_SET_ERROR                                      0x30000000
#define PAGE_CLEAR_ERROR                                    0x40000000
#define PAGE_DELETE_ERROR                                   0x50000000
#define PAGE_ERROR_TYPE_PAGE                                0x01000000
#define PAGE_ERROR_TYPE_PARAMETER                           0x02000000
#define PAGE_ERROR_TYPE_NODE                                0x03000000
#define PAGE_ERROR_TYPE_OPERATION_MASK                      0xF0000000
#define PAGE_ERROR_TYPE_MASK                                0x0F000000
#define PAGE_ERROR_TYPE_SUB_VALUE                           0x00FFFFFF

#define PAGE_INIT_ERROR_VALID                               PAGE_INIT_ERROR     | PAGE_ERROR_TYPE_PAGE
#define PAGE_INIT_ERROR_PARAMETER                           PAGE_INIT_ERROR     | PAGE_ERROR_TYPE_PARAMETER
#define PAGE_GET_ERROR_VALID                                PAGE_GET_ERROR      | PAGE_ERROR_TYPE_PAGE
#define PAGE_GET_ERROR_PARAMETER                            PAGE_GET_ERROR      | PAGE_ERROR_TYPE_PARAMETER
#define PAGE_GET_ERROR_NODE_POINTER_GET                     PAGE_GET_ERROR      | PAGE_ERROR_TYPE_NODE
#define PAGE_GET_ERROR_NODE_POINTER_DATA_NULL               PAGE_GET_ERROR      | PAGE_ERROR_TYPE_PAGE
#define PAGE_SET_ERROR_VALID                                PAGE_SET_ERROR      | PAGE_ERROR_TYPE_PAGE
#define PAGE_SET_ERROR_PARAMETER                            PAGE_SET_ERROR      | PAGE_ERROR_TYPE_PARAMETER
#define PAGE_SET_ERROR_NODE_POINTER_SET                     PAGE_SET_ERROR      | PAGE_ERROR_TYPE_NODE
#define PAGE_CLEAR_ERROR_VALID                              PAGE_CLEAR_ERROR    | PAGE_ERROR_TYPE_PAGE
#define PAGE_CLEAR_ERROR_PARAMETER                          PAGE_CLEAR_ERROR    | PAGE_ERROR_TYPE_PARAMETER
#define PAGE_CLEAR_ERROR_NODE_POINTER_GET                   PAGE_CLEAR_ERROR    | PAGE_ERROR_TYPE_NODE
#define PAGE_DELETE_ERROR_VALID                             PAGE_DELETE_ERROR   | PAGE_ERROR_TYPE_PAGE
#define PAGE_DELETE_ERROR_PARAMETER                         PAGE_DELETE_ERROR   | PAGE_ERROR_TYPE_PARAMETER
#define PAGE_DELETE_ERROR_NODE_POINTER_GET                  PAGE_DELETE_ERROR   | PAGE_ERROR_TYPE_NODE
#define PAGE_DELETE_ERROR_NODE_EMPTY                        PAGE_DELETE_ERROR   | PAGE_ERROR_TYPE_PAGE

typedef struct _List_Information{
    void *list_address;
    int  list_index,
         list_index_max,
         depth;
} List_Information;

typedef struct _Page{
    void *list_header;
    int  *list_max_array,
         depth_max;
} Page;

void memory_pointer_dump(char *data, const int size)
{/*{{{*/
    int i;
    void **data_void;

    data_void = (void **)data;

    fprintf(stderr, "Dump Start Address   : %10p\n{\n", data);
    for(i = 0;i < size;i ++){
        fprintf(stderr, "%9p ", *(data_void + i));
        if((i % 16) == 15){
            fprintf(stderr, "\n");
        }
    }

    if(i < 15){
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "}\n");
}/*}}}*/

int allocator_valid(void **allocated_pointer, const int element_count, const int element_size)
{/*{{{*/
    if(allocated_pointer == NULL){
        return ALLOCATOR_VALID_ERROR_POINTER_NULL;
    }
    
    if(element_count < 0){
        return ALLOCATOR_VALID_ERROR_ELEMENT_COUNT_MINUS;
    }

    if(element_count == 0){
        return ALLOCATOR_VALID_ERROR_ELEMENT_COUNT_ZERO;
    }

    if(element_size < 0){
        return ALLOCATOR_VALID_ERROR_ELEMENT_SIZE_MINUS;
    }

    if(element_size == 0){
        return ALLOCATOR_VALID_ERROR_ELEMENT_SIZE_ZERO;
    }

    return 0;
}/*}}}*/

int allocator(void **allocated_pointer, const int element_count, const int element_size)
{/*{{{*/
    int error_code;

    if((error_code = allocator_valid(allocated_pointer, element_count, element_size)) != 0){
        return ALLOCATOR_VALID_ERROR | error_code;
    }

    if((*allocated_pointer = calloc(element_count, element_size)) == NULL){
        return ALLOCATOR_ERROR_CALLOC;
    }

    fprintf(stderr, "Memory Allocated     : %10p\n", *allocated_pointer);

    return 0;
}/*}}}*/

void allocator_debug_valid(const int error_code)
{/*{{{*/
    switch(error_code){
        case ALLOCATOR_VALID_ERROR_POINTER_NULL:
            fprintf(stderr, "[%20s(%5d)] Memory allocation, Result saved address is NULL\n", __FUNCTION__, __LINE__);
            break;
        case ALLOCATOR_VALID_ERROR_ELEMENT_COUNT_MINUS:
            fprintf(stderr, "[%20s(%5d)] Memory allocation, Element count is minus\n", __FUNCTION__, __LINE__);
            break;
        case ALLOCATOR_VALID_ERROR_ELEMENT_COUNT_ZERO:
            fprintf(stderr, "[%20s(%5d)] Memory allocation, Element count is zero\n", __FUNCTION__, __LINE__);
            break;
        case ALLOCATOR_VALID_ERROR_ELEMENT_SIZE_MINUS:
            fprintf(stderr, "[%20s(%5d)] Memory allocation, Element size is minus\n", __FUNCTION__, __LINE__);
            break;
        case ALLOCATOR_VALID_ERROR_ELEMENT_SIZE_ZERO:
            fprintf(stderr, "[%20s(%5d)] Memory allocation, Element size is zero\n", __FUNCTION__, __LINE__);
            break;
    }
}/*}}}*/

void allocator_debug(const int error_code)
{/*{{{*/
    int error_code_type,
        error_code_sub;

    error_code_type = error_code & ALLOCATOR_ERROR_MASK;
    error_code_sub  = error_code & ALLOCATOR_ERROR_SUB_VALUE;

    switch(error_code_type){
        case ALLOCATOR_VALID_ERROR:
            fprintf(stderr, "[%20s(%5d)] Memory allocation, valid error\n", __FUNCTION__, __LINE__);
            break;
        case ALLOCATOR_ERROR_CALLOC:
            fprintf(stderr, "[%20s(%5d)] Memory allocation, calloc Failed\n", __FUNCTION__, __LINE__);
            break;
    }

    if(error_code_sub != 0){
        allocator_debug_valid(error_code_sub);
    }
}/*}}}*/

void deleter(void *delete_pointer)
{/*{{{*/
    free(delete_pointer);
    /* fprintf(stderr, "Memory Free          : %10p\n", delete_pointer); */
}/*}}}*/

int va_parse_int(va_list parameter_va, const int parameter_count, int **parameter)
{/*{{{*/
    int error_code,
        i;
    
    if((error_code = allocator((void **)parameter, parameter_count, sizeof(int))) != 0){
        return error_code;
    }

    for(i = 0;i < parameter_count;i ++){
        (*parameter)[i] = va_arg(parameter_va, int); 
    }
    
    return 0;
}/*}}}*/

void va_parse_delete(void *parameter)
{/*{{{*/
    fprintf(stderr, "Parameter Free       : %10p\n", parameter);
    deleter(parameter);
}/*}}}*/

void va_parse_int_debug(const int error_code)
{/*{{{*/
    if(error_code != 0){
        fprintf(stderr, "[%20s(%5d)] Integer Parameter List Get Failed\n", __FUNCTION__, __LINE__);
        allocator_debug(error_code);
    }
}/*}}}*/

int list_valid(void *list, const int index, const int index_max)
{/*{{{*/
    if(list == NULL){
        return LIST_VALID_ERROR_NULL;
    }

    if(index_max < 0){
        return LIST_VALID_ERROR_INDEX_MAX_MINUS;
    }

    if(index_max == 0){
        return LIST_VALID_ERROR_INDEX_MAX_ZERO;
    }

    if(index < 0){
        return LIST_VALID_ERROR_INDEX_RANGE_MINUS;
    }

    if(index >= index_max){
        return LIST_VALID_ERROR_INDEX_RANGE_OVER;
    }

    return 0;
}/*}}}*/

int list_new(void *list_saved_address, const int index_max, void **new_space)
{/*{{{*/
    int error_code;
    void *list_new_space_pointer;

    /* list_valid()의 두번째 인자값은 의미가 없다. */
    if((error_code = list_valid(list_saved_address, LIST_VALID_PARAMETER_EMPTY_INT, index_max)) != 0){
        return LIST_NEW_ERROR_VALID | error_code;
    }

    if((error_code = allocator(list_saved_address, index_max, sizeof(void *))) != 0){
        return LIST_NEW_ERROR_ALLOCATION | error_code;
    }

    if(new_space != NULL){
        /* *new_space = *(void **)list_saved_address; */
        LIST_DATA_POINTER_GET(list_saved_address, list_new_space_pointer);
        *new_space = list_new_space_pointer;
    }

    return 0;
}/*}}}*/

int list_get_address(void *address, const int index, const int index_max, void **next)
{/*{{{*/
    int error_code;

    if((error_code = list_valid(address, index, index_max)) != 0){
        return LIST_GET_ADDRESS_ERROR_VALID | error_code;
    }

    if(next == NULL){
        return LIST_GET_ADDRESS_ERROR_DATA_POINTER_NULL;
    }

    /* DO NOT USE 'LIST_DISPLACEMENT_DATA_GET()' 
     * MUST BE GET 'List address', NOT 'List_data' */
    *next = LIST_DISPLACEMENT_ADDRESS_GET(address, index);

    return 0;
}/*}}}*/

void list_delete(void *list)
{/*{{{*/
    fprintf(stderr, "List    Free         : %10p\n", list);
    deleter(list);
}/*}}}*/

void list_debug_valid(const int error_code)
{/*{{{*/
    int error_type,
        error_code_sub;

    error_type      = error_code & LIST_VALID_TYPE_MASK;
    error_code_sub  = error_code & LIST_VALID_SUB_VALUE;

    switch(error_type){
        case LIST_VALID_ERROR_NULL:
            fprintf(stderr, "[%20s(%5d)] list is NULL\n", __FUNCTION__, __LINE__);
            break;
        case LIST_VALID_ERROR_INDEX_MAX_MINUS:
            fprintf(stderr, "[%20s(%5d)] list index is minus\n", __FUNCTION__, __LINE__);
            break;
        case LIST_VALID_ERROR_INDEX_MAX_ZERO:
            fprintf(stderr, "[%20s(%5d)] list index is zero\n", __FUNCTION__, __LINE__);
            break;
        case LIST_VALID_ERROR_INDEX_RANGE_MINUS:
            fprintf(stderr, "[%20s(%5d)] list index minus\n", __FUNCTION__, __LINE__);
            break;
        case LIST_VALID_ERROR_INDEX_RANGE_OVER:
            fprintf(stderr, "[%20s(%5d)] list index overflow\n", __FUNCTION__, __LINE__);
            break;
    }

    if(error_code_sub != 0){
        allocator_debug(error_code_sub);
    }
}/*}}}*/

void list_debug(const int error_code)
{/*{{{*/
    int error_type,
        error_code_sub;

    error_type      = error_code & LIST_TYPE_MASK;
    error_code_sub  = error_code & LIST_SUB_VALUE;

    switch(error_type){
        case LIST_NEW_ERROR_VALID:
            fprintf(stderr, "[%20s(%5d)] list pointer new valid error \n", __FUNCTION__, __LINE__);
            break;
        case LIST_NEW_ERROR_ALLOCATION:
            fprintf(stderr, "[%20s(%5d)] list pointer new allocation error\n", __FUNCTION__, __LINE__);
            break;
        case LIST_GET_ADDRESS_ERROR_VALID:
            fprintf(stderr, "[%20s(%5d)] list pointer address get valid problem\n", __FUNCTION__, __LINE__);
            break;
        case LIST_GET_ADDRESS_ERROR_DATA_POINTER_NULL:
            fprintf(stderr, "[%20s(%5d)] list pointer address get saved pointer is NULL\n", __FUNCTION__, __LINE__);
            break;
    }
        
    if(error_code_sub != 0){
        list_debug_valid(error_code_sub);
    }
}/*}}}*/

int link_get_address(void *list_address, const int index, const int index_max, void **next)
{/*{{{*/
    void *data_pointer;
    int error_code;

    /* 1, reference list */
    if(list_address == NULL){
        return LINK_GET_ADDRESS_START_NULL;
    }
    
    LIST_DATA_POINTER_GET(list_address, data_pointer);

    /* 2, check EMTPY data */
    if(data_pointer == NULL){
        return LINK_GET_ADDRESS_DATA_NULL; 
    }
    
    /* 3, get next position */
    if((error_code = list_get_address(data_pointer, index, index_max, next)) != 0){
        return LINK_GET_ADDRESS_ERROR_GET | error_code;
    }

    return 0;
}/*}}}*/

int link_set_new(void *list_address, const int index, const int index_max, void **next)
{/*{{{*/
    void *data_pointer,
         *new_space;
    int error_code;

    /* 1, reference list */
    if(list_address == NULL){
        return LINK_SET_ADDRESS_START_NULL; 
    }

    LIST_DATA_POINTER_GET(list_address, data_pointer);

    /* 2, check EXIST data */
    if(data_pointer != NULL){
        return LINK_SET_ADDRESS_DATA_NOT_NULL; 
    }

    /* 3, create space */
    if((error_code = list_new(list_address, index_max, &new_space)) != 0){
        return LINK_SET_ADDRESS_ERROR_NEW | error_code;
    }

    /* 4, get next position */
    if((error_code = list_get_address(new_space, index, index_max, next)) != 0){
        return LINK_SET_ADDRESS_ERROR_GET | error_code;
    }

    fprintf(stderr, "                                          %9p[%02d] = %9p\n", new_space, index, *next);
    return 0;
}/*}}}*/

void link_debug(const int error_code)
{/*{{{*/
    int error_type,
        error_code_sub;

    error_type      = error_code & LINK_TYPE_MASK;
    error_code_sub  = error_code & LINK_SUB_VALUE;

    switch(error_type){
        case LINK_GET_ADDRESS_START_NULL:
            fprintf(stderr, "[%20s(%5d)] link 'address get' start pointer is NULL\n", __FUNCTION__, __LINE__);
            break;
        case LINK_GET_ADDRESS_DATA_NULL:
            fprintf(stderr, "[%20s(%5d)] link 'address get' data pointer is NULL\n", __FUNCTION__, __LINE__);
            break;
        case LINK_GET_ADDRESS_ERROR_GET:
            fprintf(stderr, "[%20s(%5d)] link 'address get' get pointer failed\n", __FUNCTION__, __LINE__);
            break;
        case LINK_SET_ADDRESS_START_NULL:
            fprintf(stderr, "[%20s(%5d)] link 'address set' start pointer is NULL\n", __FUNCTION__, __LINE__);
            break;
        case LINK_SET_ADDRESS_DATA_NOT_NULL:
            fprintf(stderr, "[%20s(%5d)] link 'address set' data pointer is not NULL\n", __FUNCTION__, __LINE__);
            break;
        case LINK_SET_ADDRESS_ERROR_NEW:
            fprintf(stderr, "[%20s(%5d)] link 'address set' new pointer failed\n", __FUNCTION__, __LINE__);
            break;
        case LINK_SET_ADDRESS_ERROR_GET:
            fprintf(stderr, "[%20s(%5d)] link 'address set' get pointer failed\n", __FUNCTION__, __LINE__);
            break;
    }

    if(error_code_sub != 0){
        list_debug(error_code_sub);
    }
}/*}}}*/

int link_rounds_valid(void *start_address, const int depth_start, int *index_max_array, const int depth_max, int *route_list, int routine(void *, const int, const int, void**), List_Information *information_data)
{/*{{{*/
    if(start_address == NULL){
        return LINK_ROUNDS_ERROR_START_ADDRESS_NULL;
    }

    if(depth_start < 0){
        return LINK_ROUNDS_ERROR_DEPTH_START_MINUS;
    }

    if(depth_max < 0){
        return LINK_ROUNDS_ERROR_DEPTH_START_MAX_MINUS;
    }
    
    if(depth_max == 0){
        return LINK_ROUNDS_ERROR_DEPTH_START_MAX_ZERO;
    }

    if(depth_start > depth_max){
        return LINK_ROUNDS_ERROR_DEPTH_OVER;
    }
    
    if(route_list == NULL){
        return LINK_ROUNDS_ERROR_ROUTE_LIST_NULL;
    }

    if(index_max_array == NULL){
        return LINK_ROUNDS_ERROR_INDEX_MAX_ARRAY_NULL;
    }
    
    if(routine == NULL){
        return LINK_ROUNDS_ERROR_ROUTINE_NULL;
    }

    if(information_data == NULL){
        return LINK_ROUNDS_ERROR_INFORMATION_DATA_NULL;
    }

    return 0;
}/*}}}*/

/* 시작깊이를 정할수 있게하고, 또한 최종적으로 이동된 깊이를 외부로 리턴할수 있어야 함 */
int link_rounds(void *start_address, const int depth_start, int *index_max_array, const int depth_max, int *route_list, int routine(void *, const int, const int, void**), List_Information *position_last)
{/*{{{*/
    int i,
        list_index,
        list_index_max,
        routine_status,
        error_code = 0;
    void *list_address,
         *list_index_address;

    if((error_code = link_rounds_valid(start_address, depth_start, index_max_array, depth_max, route_list, routine, position_last)) != 0){
        return error_code;
    }

    /* 처음부터 실패하더라도 address값을 넘겨줘야 */
    list_address = list_index_address = start_address;

    for(i = depth_start;i < depth_max;i ++){
        list_index      = route_list[i];
        list_index_max  = index_max_array[i];

        if(list_index >= list_index_max){
            error_code = LINK_ROUNDS_ERROR_INDEX_OVERFLOW;
            goto END_OF_ROUTINE_LOOP;
        }

        /* fprintf(stderr, "SP:%p, d: %d, i: %d, m: %d\n", list_address, i, list_index, list_index_max); */
        if((routine_status = routine(list_address, list_index, list_index_max, &list_index_address)) != 0){
            error_code = LINK_ROUNDS_ERROR_ROUTINE_FAILD | routine_status;
            goto END_OF_ROUTINE_LOOP;
        }

        fprintf(stderr, "[%03d](%04d/%04d)     : %10p (Head) -------------------------- %9p (Index)\n", i, list_index, list_index_max, list_address, list_index_address);
        list_address = (void **)list_index_address;
    } END_OF_ROUTINE_LOOP:;

    /* 오류난다고 해서 주소값이나 깊이를 리턴하지 않는경우는 없어야 한다
     * (이렇게 해야 get하고 난뒤에 이어서 new를 할 수 있다!) */
    position_last->list_address     = list_index_address;
    position_last->list_index       = list_index;
    position_last->list_index_max   = list_index_max;
    position_last->depth            = i;

    return error_code;
}/*}}}*/

void link_rounds_debug(const int error_code)
{/*{{{*/
    int error_type,
        error_code_sub;

    error_type      = error_code & LINK_ROUNDS_TYPE_MASK;
    error_code_sub  = error_code & LINK_ROUNDS_SUB_VALUE;

    switch(error_type){
        case LINK_ROUNDS_ERROR_START_ADDRESS_NULL:
            fprintf(stderr, "[%20s(%5d)] link rounds start address NULL \n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_DEPTH_START_MINUS:
            fprintf(stderr, "[%20s(%5d)] link rounds start depth position is minus\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_DEPTH_START_MAX_ZERO:
            fprintf(stderr, "[%20s(%5d)] link rounds start depth position is zero\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_DEPTH_START_MAX_MINUS:
            fprintf(stderr, "[%20s(%5d)] link rounds start depth position is minus\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_DEPTH_OVER:
            fprintf(stderr, "[%20s(%5d)] link rounds overflow index\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_ROUTE_LIST_NULL:
            fprintf(stderr, "[%20s(%5d)] link rounds route list NULL\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_INDEX_MAX_ARRAY_NULL:
            fprintf(stderr, "[%20s(%5d)] link rounds route index max array NULL (empty)\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_ROUTINE_NULL:
            fprintf(stderr, "[%20s(%5d)] link rounds route routine function is not exist\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_INFORMATION_DATA_NULL:
            fprintf(stderr, "[%20s(%5d)] link rounds save pointer is NULL\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_INDEX_OVERFLOW:
            fprintf(stderr, "[%20s(%5d)] link rounds list index over\n", __FUNCTION__, __LINE__);
            break;
        case LINK_ROUNDS_ERROR_ROUTINE_FAILD:
            fprintf(stderr, "[%20s(%5d)] link rounds routine return value is not zero (failed)\n", __FUNCTION__, __LINE__);
            break;
    }

    if(error_code_sub != 0){
        link_debug(error_code_sub);
    }
}/*}}}*/

int node_pointer_get(void *start_address, const int depth_start, int *list_index_max_array, const int depth_max, int *route, void **data_save, List_Information *list_information)
{/*{{{*/
    int error_code;
    void *data_pointer;

    if((error_code = link_rounds(start_address, depth_start, list_index_max_array, depth_max, route, link_get_address, list_information)) != 0){
        return NODE_POINTER_GET_ERROR_LINK_ROUNDS | error_code;
    }

    if(list_information->list_address == NULL){
        return NODE_POINTER_GET_ERROR_LIST_NULL; 
    }
    
    LIST_DATA_POINTER_GET(list_information->list_address, data_pointer);

    /* 단지 위치 정보를 알고싶을때는 이 부분이 NULL이 될 수 있다.  */
    if(data_save != NULL){
        *data_save = data_pointer;
        fprintf(stderr, "Load  Pointer Result : %10p (Data) <------------------------- %9p(List)\n", data_pointer, list_information->list_address);
    }
    
    return 0;
}/*}}}*/

int node_pointer_set(void *start_address, const int depth_start, int *list_index_max_array, const int depth_max, int *route, void *data_set, List_Information *list_information)
{/*{{{*/
    int error_code;

    if((error_code = link_rounds(start_address, depth_start, list_index_max_array, depth_max, route, link_set_new, list_information)) != 0){
        return NODE_POINTER_SET_ERROR_LINK_ROUNDS | error_code;
    }

    if(list_information->list_address == NULL){
        return NODE_POINTER_SET_ERROR_LIST_NULL; 
    }
    
    LIST_DATA_POINTER_SET(list_information->list_address, data_set);

    fprintf(stderr, "Set Pointer Result   : %10p (List) <------------------------- %9p(Data)\n", list_information->list_address, data_set);

    return 0;
}/*}}}*/

void node_pointer_debug(int error_code)
{/*{{{*/
    int error_type,
        error_code_sub;

    error_type      = error_code & NODE_POINTER_TYPE_MASK;
    error_code_sub  = error_code & NODE_POINTER_SUB_VALUE;

    switch(error_type){
        case NODE_POINTER_GET_ERROR_LINK_ROUNDS:
            fprintf(stderr, "[%20s(%5d)] node pointer get routine failed\n", __FUNCTION__, __LINE__);
            break;
        case NODE_POINTER_GET_ERROR_LIST_NULL:
            fprintf(stderr, "[%20s(%5d)] node pointer get data failed\n", __FUNCTION__, __LINE__);
            break;
        case NODE_POINTER_SET_ERROR_LINK_ROUNDS:
            fprintf(stderr, "[%20s(%5d)] node pointer set routine failed\n", __FUNCTION__, __LINE__);
            break;
        case NODE_POINTER_SET_ERROR_LIST_NULL:
            fprintf(stderr, "[%20s(%5d)] node pointer set data failed\n", __FUNCTION__, __LINE__);
            break;
    }

    if(error_code_sub != 0){
        link_rounds_debug(error_code_sub);
    }
}/*}}}*/

void page_debug_valid(const int error_code)
{/*{{{*/
    switch(error_code){
        case PAGE_VALID_ERROR_PAGE_POINTER_NULL:
            fprintf(stderr, "[%20s(%5d)] page pointer NULL\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_VALID_ERROR_DATA_NULL:
            fprintf(stderr, "[%20s(%5d)] page data NULL\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_VALID_ERROR_DATA_SIZE_MINUS_ZERO:
            fprintf(stderr, "[%20s(%5d)] page size is minus or zero\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_VALID_ERROR_PARAMETER_COUNT_MINUS_ZERO:
            fprintf(stderr, "[%20s(%5d)] page parameter count is minus or zero\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_VALID_ERROR_PARAMETER_COUNT_OVER:
            fprintf(stderr, "[%20s(%5d)] page parameter count over\n", __FUNCTION__, __LINE__);
            break;
    }
}/*}}}*/

void page_debug_empty(const int error_code)
{/*{{{*/
    switch(error_code){
        case PAGE_EMTPY_ERROR_ADDRESS_NULL:
            fprintf(stderr, "[%20s(%5d)] list empty, list start pointer address is NULL\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_EMTPY_ERROR_DEPTH_MINUS:
            fprintf(stderr, "[%20s(%5d)] list empty, list depth is minus\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_EMPTY_ERROR_LIST_EMPTY:
            fprintf(stderr, "[%20s(%5d)] list empty (all pointer's clear)\n", __FUNCTION__, __LINE__);
            break;
    }
}/*}}}*/

void page_debug(const int error_code)
{/*{{{*/
    int error_code_type,
        error_code_sub;

    error_code_type = error_code & PAGE_ERROR_MASK;
    error_code_sub  = error_code & PAGE_SUB_VALUE;

    if(error_code_type != 0){
        page_debug_valid(error_code_type);
    }

    if(error_code_sub != 0){
        page_debug_empty(error_code_sub);
    }
}/*}}}*/

void page_error_print(const int error_code)
{/*{{{*/
    int error_function,
        error_type,
        error_code_sub;

    error_function  = error_code & PAGE_ERROR_TYPE_OPERATION_MASK;
    error_type      = error_code & PAGE_ERROR_TYPE_MASK;
    error_code_sub  = error_code & PAGE_ERROR_TYPE_SUB_VALUE;

    switch(error_function){
        case PAGE_INIT_ERROR:
            fprintf(stderr, "[%20s(%5d)] page init   error\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_GET_ERROR:
            fprintf(stderr, "[%20s(%5d)] page get    error\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_SET_ERROR:
            fprintf(stderr, "[%20s(%5d)] page set    error\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_CLEAR_ERROR:
            fprintf(stderr, "[%20s(%5d)] page clear  error\n", __FUNCTION__, __LINE__);
            break;
        case PAGE_DELETE_ERROR:
            fprintf(stderr, "[%20s(%5d)] page delete error\n", __FUNCTION__, __LINE__);
            break;
    }

    switch(error_type){
        case PAGE_ERROR_TYPE_PAGE:
            page_debug(error_code_sub);
            break;
        case PAGE_ERROR_TYPE_PARAMETER:
            va_parse_int_debug(error_code_sub);
            break;
        case PAGE_ERROR_TYPE_NODE:
            node_pointer_debug(error_code_sub);
            break;
    }
}/*}}}*/

int page_valid(Page *page, void *data, const int data_size, int parameter_count)
{/*{{{*/
    if(page == NULL){
        return PAGE_VALID_ERROR_PAGE_POINTER_NULL;
    }

    if(data == NULL){
        return PAGE_VALID_ERROR_DATA_NULL;
    }

    if(data_size <= 0){
        return PAGE_VALID_ERROR_DATA_SIZE_MINUS_ZERO;
    }

    if(parameter_count <= 0){
        return PAGE_VALID_ERROR_PARAMETER_COUNT_MINUS_ZERO;
    }

    if(parameter_count > page->depth_max){
        return PAGE_VALID_ERROR_PARAMETER_COUNT_OVER;
    }

    return 0;
}/*}}}*/

int page_empty(void *list_start_address, const int page_count)
{/*{{{*/
    void *null_pointer = NULL;
    int i,
        null_check_count;

    if(list_start_address == NULL){
        return PAGE_EMTPY_ERROR_ADDRESS_NULL;
    }
    
    if(page_count < 0){
        return PAGE_EMTPY_ERROR_DEPTH_MINUS;
    }

    /* 모든 공간이 NULL로 이뤄졌음을 확인함 (중간에 자료가 하나라도 있으면 안되! )
     * -1을 하는 이유 : 예를들어 2로 들어왔다고 하면, 실제 주소가 이동하는 범위는 4(sizeof(void *)) 만큼 이동. 이하 비교하는 구문도 마찬가지 */
    for(i = 0, null_check_count = 0;i < page_count - 1;i++){
        if(memcmp(list_start_address + (sizeof(void *) * i), &null_pointer, sizeof(void *)) == 0){
            null_check_count ++;
        }
    }
    
    if(null_check_count != page_count - 1){
        memory_pointer_dump(list_start_address, page_count);
        return PAGE_EMPTY_ERROR_LIST_EMPTY;
    }

    return 0;
}/*}}}*/

int page_init(Page *page, const int parameter_count, ...)
{/*{{{*/
    va_list parameter_va;
    int error_code;

    /* valid검사를 하기전에, 먼저 초기값을 넣어놔야 한다. */
    page->list_header       = NULL;
    page->list_max_array    = NULL;
    page->depth_max         = parameter_count;

    if((error_code = page_valid(page, PAGE_VALID_PARAMETER_EMPTY_POINTER, PAGE_VALID_PARAMETER_EMPTY_INT, parameter_count)) != 0){
        return PAGE_INIT_ERROR_VALID | error_code;
    }

    va_start(parameter_va, parameter_count);

    /* 가변인자는 각 Depth의 최대 index값을 나타냄.  */
    if((error_code = va_parse_int(parameter_va, parameter_count, &(page->list_max_array))) != 0){
        return PAGE_INIT_ERROR_PARAMETER | error_code;
    }

    va_end(parameter_va);

    return 0;
}/*}}}*/

int page_get(Page *page, void *data, const int data_size, int parameter_count, ...)
{/*{{{*/
    va_list parameter_va;
    int error_code,
        *parameter;
    List_Information list_information;
    void *list_data_pointer = NULL;

    fprintf(stderr, "[%s] DEBUG '%s' - (%s, Line:%d) %p\n", __TIME__, __FUNCTION__, __FILE__, __LINE__, page->list_header);

    if((error_code = page_valid(page, data, data_size, parameter_count)) != 0){
        return PAGE_GET_ERROR_VALID | error_code;
    }

    va_start(parameter_va, parameter_count);

    if((error_code = va_parse_int(parameter_va, parameter_count, &parameter)) != 0){
        return PAGE_GET_ERROR_PARAMETER | error_code;
    }

    va_end(parameter_va);

    if((error_code = node_pointer_get(&(page->list_header), 0, page->list_max_array, page->depth_max, parameter, &list_data_pointer, &list_information)) != 0){
        va_parse_delete(parameter);
        return PAGE_GET_ERROR_NODE_POINTER_GET | error_code;
    }

    /* GET TODO
     * 1, check NULL (쉬운 예외처리를 위해서, 바로 NULL 체크 안함)
     * 2, copy pointer from node, to data*/
    if((error_code = page_empty(&list_data_pointer, 1)) != 0){
        va_parse_delete(parameter);
        return PAGE_GET_ERROR_NODE_POINTER_DATA_NULL;
    }

    memcpy(data, list_data_pointer, data_size);
    va_parse_delete(parameter);

    return 0;
}/*}}}*/

int page_set(Page *page, void *data, int parameter_count, ...)
{/*{{{*/
    va_list parameter_va;
    int error_code,
        *parameter;
    List_Information list_information_get,
                     list_information_set;

    fprintf(stderr, "[%s] DEBUG '%s'                  : (%s, Line:%d)\n", __TIME__, __FUNCTION__, __FILE__, __LINE__);

    if((error_code = page_valid(page, data, PAGE_VALID_PARAMETER_EMPTY_INT, parameter_count)) != 0){
        return PAGE_SET_ERROR_VALID | error_code;
    }

    va_start(parameter_va, parameter_count);

    if((error_code = va_parse_int(parameter_va, parameter_count, &parameter)) != 0){
        return PAGE_SET_ERROR_PARAMETER | error_code;
    }

    va_end(parameter_va);

    /* Set
     * 1, Rounds Route (Search)
     * 2, if not exist, create NODE 
     * 3, set pointer in NODE */
    if(node_pointer_get(&(page->list_header), 0, page->list_max_array, page->depth_max, parameter, data, &list_information_get) != 0){
        /* 밑으로는 리스트가 없으므로 생성해주기 위해 여태까지 찾은 위치를 초기값으로 다시 넣어줘서 set */
        if((error_code = node_pointer_set(list_information_get.list_address, list_information_get.depth, page->list_max_array, page->depth_max, parameter, data, &list_information_set)) != 0){
            va_parse_delete(parameter);
            return PAGE_SET_ERROR_NODE_POINTER_SET | error_code;
        }
    }

    va_parse_delete(parameter);

    return 0;
}/*}}}*/

int page_clear(Page *page, int parameter_count, ...)
{/*{{{*/
    va_list parameter_va;
    int error_code,
        *parameter;
    List_Information list_information;

    fprintf(stderr, "[%s] DEBUG '%s' - (%s, Line:%d)\n", __TIME__, __FUNCTION__, __FILE__, __LINE__);

    if((error_code = page_valid(page, PAGE_VALID_PARAMETER_EMPTY_POINTER, PAGE_VALID_PARAMETER_EMPTY_INT, parameter_count)) != 0){
        return PAGE_CLEAR_ERROR_VALID | error_code;
    }
    va_start(parameter_va, parameter_count);

    if((error_code = va_parse_int(parameter_va, parameter_count, &parameter)) != 0){
        return PAGE_CLEAR_ERROR_PARAMETER | error_code;
    }

    va_end(parameter_va);

    /* list에 저장되어 있는 '값'이 아니라 '위치'(주소)를 가지고 온다 */
    if((error_code = node_pointer_get(&(page->list_header), 0, page->list_max_array, page->depth_max, parameter, NULL, &list_information)) != 0){
        va_parse_delete(parameter);
        return PAGE_CLEAR_ERROR_NODE_POINTER_GET | error_code;
    }

    /* Clear
     * 1, insert NULL in target pointer */
    memset(list_information.list_address, 0, sizeof(void *));

    fprintf(stderr, "Clear Pointer        : %10p (List) <------------------------- NULL}\n", list_information.list_address);
    va_parse_delete(parameter);

    return 0;
}/*}}}*/

/* 포인터 라인(list)한것만 지움. 저장된 최종 포인터를 쓰는게 아님.  */
int page_delete(Page *page, int parameter_count, ...)
{/*{{{*/
    va_list parameter_va;
    int error_code,
        list_pointer_depth,
        list_index_max,
        *parameter;
    List_Information list_information;
    void *list_index_pointer = NULL;

    fprintf(stderr, "[%s] DEBUG '%s' - (%s, Line:%d)\n", __TIME__, __FUNCTION__, __FILE__, __LINE__);

    if((error_code = page_valid(page, PAGE_VALID_PARAMETER_EMPTY_POINTER, PAGE_VALID_PARAMETER_EMPTY_INT, parameter_count)) != 0){
        return PAGE_DELETE_ERROR_VALID | error_code;
    }

    va_start(parameter_va, parameter_count);

    if((error_code = va_parse_int(parameter_va, parameter_count, &parameter)) != 0){
        return PAGE_DELETE_ERROR_PARAMETER | error_code;
    }

    va_end(parameter_va);

    if((error_code = node_pointer_get(&(page->list_header), 0, page->list_max_array, parameter_count, parameter, &list_index_pointer, &list_information)) != 0){
        va_parse_delete(parameter);
        return PAGE_DELETE_ERROR_NODE_POINTER_GET | error_code;
    }

    /* Delete
     * 1, check empty in list
     * 2, free list */
    list_pointer_depth  = list_information.depth;
    list_index_max      = page->list_max_array[list_pointer_depth];

    if((error_code = page_empty(list_index_pointer, list_index_max)) != 0){
        va_parse_delete(parameter);
        return PAGE_DELETE_ERROR_NODE_EMPTY | error_code;
    }

    list_delete(list_index_pointer);
    va_parse_delete(parameter);

    return 0;
}/*}}}*/

void page_terminte(Page *page)
{/*{{{*/
    va_parse_delete(page->list_max_array);
    list_delete(page->list_header);

    page->list_max_array    = NULL;
    page->list_header       = NULL;
    page->depth_max         = 0;
}/*}}}*/

int main(int argc, const char *argv[])
{
    Page page;
    int error_code;
    int data1     = 1823,
        data1_get = -1;
    /* int data2 = 72301, */
        /* data2_get = 999; */

    if((error_code = page_init(&page, 2, 256, 256)) != 0){
        page_error_print(error_code);
        return -1;
    }

    fprintf(stderr, "List start Pointer address (saved here!)     : %p\n", &(page.list_header));
    fprintf(stderr, "Set Data                                     : %d (%p)\n\n", data1, &data1);

    /* TODO: depth가부족하면 에러나게 할것, 재할당 금지...*/
    if((error_code = page_set(&page, &data1, 2, 20, 48)) != 0){
        page_error_print(error_code);
        return -2;
    }
    fprintf(stderr, "\n");

    if((error_code = page_get(&page, &data1_get, sizeof(data1), 2, 20, 48)) != 0){
        page_error_print(error_code);
        return -3;
    }

    fprintf(stderr, "Get Data                                     : %d\n\n", data1_get);

    if((error_code = page_clear(&page, 2, 20, 48)) != 0){
        page_error_print(error_code);
        return -4;
    }
    fprintf(stderr, "\n");

    if((error_code = page_delete(&page, 1, 20)) != 0){
        page_error_print(error_code);
        return -5;;
    }

    /* fprintf(stderr, "Set data    : %d\n", data2); */
    /* page_set(&page, &data2, 2, 92, 28); */
    /* page_get(&page, &data2_get, sizeof(data2), 2, 92, 28); */
    /* fprintf(stderr, "\n=>Get Data  : %d\n", data2_get); */
    /* page_delete(&page, 1, 92); */
    page_terminte(&page);

    return 0;
}
