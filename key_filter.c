/**
 *      @file      key_test.c
 *
 *      @section   
 *      @remark
 *
 *      @version   1.0
 *      @date      2014년 03월 09일 20시 37분 17초
 *
 *      @author    YOUR NAME (), 
 *
 *      @see       none
 *      @warning   none
 *      @todo      none
 *      @bug       none
 */
#include "lowterm.h"

int key_filter_grab_code(char *key_symbol_string, KeyEvent *key_data)
{/*{{{*/
    KeySym key_symbol;

    if(strcmp(key_symbol_string, "") == 0){
        return KEY_SYMBOL_EMPTY;
    }

    if((key_symbol = XStringToKeysym(key_symbol_string)) == NoSymbol){
        return KEY_SYMBOL_UNKNOWN;
    }

    if((key_data->key_code = XKeysymToKeycode(GDK_DISPLAY(), key_symbol)) == 0){
        return KEY_CODE_UNKNOWN;
    }

    return 0;
}/*}}}*/

void key_filter_grab_mask_delmiter(char *string, int *string_length, char **next)
{/*{{{*/
    char *delimiter_pointer;

    if(*string == '\0'){
        return ;
    }

    /* nothing '+' */
    if((delimiter_pointer = strchr(string, (int)'+')) == NULL){
        *string_length  = strlen(string);
        *next           = string + *string_length;
        return ;
    }

    /* +1 is delminter(+) */
    *string_length  = delimiter_pointer - string;
    *next           = string + *string_length + 1;
}/*}}}*/

int key_filter_grab_mask_parse(char *key_chain, int key_string_length, KeyEvent *key_data)
{/*{{{*/
    unsigned int mask_old;
    int return_mask;

    mask_old        = key_data->key_mask;
    return_mask     = 0;

    if(*key_chain == '\0'){
        return 0;
    }

    switch(key_string_length){
        case 7:
            if(strncmp(key_chain, "AltMask", 7)         == 0 ||\
               strncmp(key_chain, "AltMask", 7)         == 0){
                key_data->key_mask  |= Mod1Mask;
                return_mask          = KEY_MASK_ALT;
                /* fprintf(stderr, "Alt Flag\n"); */
            }
            break;
        case 9:
            if(strncmp(key_chain, "ShiftMask", 9)       == 0 ||\
               strncmp(key_chain, "ShiftMask", 9)       == 0){
                key_data->key_mask  |= ShiftMask;
                return_mask          = KEY_MASK_SHIFT;
                /* fprintf(stderr, "Shift Flag\n"); */
            }

            if(strncmp(key_chain, "HyperMask", 9)       == 0 ||\
               strncmp(key_chain, "HyperMask", 9)       == 0){
                key_data->key_mask  |= Mod3Mask;
                return_mask          = KEY_MASK_HYPER;
                /* fprintf(stderr, "Hyper Flag\n"); */
            }

            if(strncmp(key_chain, "SuperMask", 9)       == 0 ||\
               strncmp(key_chain, "SuperMask", 9)       == 0){
                key_data->key_mask  |= Mod4Mask;
                return_mask          = KEY_MASK_SUPER;
                /* fprintf(stderr, "Super Flag\n"); */
            }
            break;
        case 10:
            if(strncmp(key_chain, "NumLockMask", 10)    == 0){
                key_data->key_mask  |= Mod2Mask;
                return_mask          = KEY_MASK_NUM_LOCK;
                /* fprintf(stderr, "Num_Lock Flag\n"); */
            }
            break;
        case 11:
            if(strncmp(key_chain, "ControlMask", 11)    == 0 ||\
               strncmp(key_chain, "ControlMask", 11)    == 0){
                key_data->key_mask  |= ControlMask;
                return_mask          = KEY_MASK_CONTROL;
                /* fprintf(stderr, "Control Flag\n"); */
            }
            break;
        case 12:
            if(strncmp(key_chain, "CapsLockMask", 12)   == 0){
                key_data->key_mask  |= LockMask;
                return_mask          = KEY_MASK_CAPS_LOCK;
                /* fprintf(stderr, "Caps_Lock Flag\n"); */
            }
            break;
        case 14:
            if(strncmp(key_chain, "ScrollLockMask", 14) == 0){
                key_data->key_mask  |= Mod5Mask;
                return_mask          = KEY_MASK_SCROLL_LOCK;
                /* fprintf(stderr, "Scroll_Lock Flag\n"); */
            }
            break;
    }

    if(key_data->key_mask == mask_old){
        return 1;
    }

    return 0;
}/*}}}*/

int key_filter_grab_mask(char *key_mask_chain, KeyEvent *key_data)
{/*{{{*/
    char *string_start,
         *string_next;
    int  key_name_length;

    string_start        = key_mask_chain;
    key_name_length     = 0;
    key_data->key_mask  = 0;

    while(*string_start != '\0'){
        key_filter_grab_mask_delmiter(string_start, &key_name_length, &string_next);
        /* fprintf(stderr, "Delimiter : Now[%s] Next[%s]\n", string_start, string_next); */
        if(key_filter_grab_mask_parse(string_start, key_name_length, key_data) == 1){
            return KEY_CODE_MASK_UNKNOWN;
        }

        string_start = string_next;
    }

    return 0;
}/*}}}*/

int key_filter_grab(GdkWindow *root_window, char *key_mask, char *key_symbol_string, KeyEvent *key_data)
{/*{{{*/
    int error_code;

    if((error_code = key_filter_grab_code(key_symbol_string, key_data)) != 0){
        return KEY_CODE_MASK_ERROR | error_code;
    }

    if((error_code = key_filter_grab_mask(key_mask, key_data)) != 0){
        return KEY_CODE_MASK_ERROR | error_code;
    }

    fprintf(stderr, "[%-25s][%-10s] => Mask : 0x%x, Keycode: %d \n", key_mask, key_symbol_string, key_data->key_mask, key_data->key_code);

	gdk_error_trap_push();
	XGrabKey(GDK_DISPLAY(), key_data->key_code, key_data->key_mask, GDK_WINDOW_XID(root_window), True, GrabModeAsync, GrabModeAsync);
	gdk_flush();

	if(gdk_error_trap_pop()){
        return KEY_CODE_GRAB_ERROR;
	}

    return 0;
}/*}}}*/

void key_filter_debug_print(int error_code, int error_bit, const char *error_message)
{/*{{{*/
    if((error_code & error_bit) == error_bit){
        fprintf(stderr, error_message);
    }
}/*}}}*/

void key_filter_debug(int error_code)
{/*{{{*/
    key_filter_debug_print(error_code, KEY_CODE_FILTER_ERROR, "Key Filter, ");
    key_filter_debug_print(error_code, KEY_CODE_GRAB_ERROR,   "Key Grab Error. another program grab key\n");
    key_filter_debug_print(error_code, KEY_CODE_MASK_ERROR ,  "Key Mask Error, ");
    key_filter_debug_print(error_code, KEY_CODE_MASK_UNKNOWN, "Unknown key mask\n");
    key_filter_debug_print(error_code, KEY_CODE_UNKNOWN,      "Unknown key code\n");
    key_filter_debug_print(error_code, KEY_SYMBOL_UNKNOWN,    "Unknown key symbol\n");
    key_filter_debug_print(error_code, KEY_SYMBOL_EMPTY,      "key symbol is Empty\n");
}/*}}}*/

static GdkFilterReturn key_event_emiter(GdkXEvent *xevent, GdkEvent *event, gpointer event_data)
{/*{{{*/
	XEvent *xev;
	XKeyEvent *key;
    KeyEvent *key_data;

	xev = (XEvent *)xevent;

    /* 주의> 윈도우 안에서는 Press만 잡힘 */
	if(xev->type != KeyPress){
		return GDK_FILTER_CONTINUE;
	}

	key      = (XKeyEvent *)xevent;
    key_data = (KeyEvent *)event_data;
    /* fprintf(stderr, "Key (Code: %u, Mask:%u) %s\n", key_data->key_code, key_data->key_mask, (xev->type - 1) ? "Release" : "Press"); */
    /* fprintf(stderr, "Key (Code: %u, Mask:%u)\n", key->keycode, key->state); */
    /* fprintf(stderr, "-      %p %p %p\n", key_data->widget, key_data->signal_id, key_data->handler_parameter); */

	if(key->keycode == key_data->key_code && \
       key->state   == key_data->key_mask){
        g_signal_emit(key_data->widget, key_data->signal_id, 0, key_data->handler_parameter);
		return GDK_FILTER_REMOVE;
	}

    return GDK_FILTER_CONTINUE;
}/*}}}*/

/* 새로운 터미널이 생성될때마다, show/hide 키 옵션으로 이게 실행됨. 
 * 핸들러로는 Terminal_Show_Hide(), handler_parameter로는 terminal이 사용된다. */
int Key_Filter(GtkWidget *widget, const char *event_name, char *mask, char *key_symbol_string, void event_handler(GtkWidget *, gpointer), KeyEvent *key_data, void *handler_parameter)
{/*{{{*/
    GdkWindow *root_window;
    int        error_code;
    guint      signal_id;

    root_window = gdk_get_default_root_window();

    /* save data for user */
    key_data->handler_parameter = handler_parameter;

    if((error_code = key_filter_grab(root_window, mask, key_symbol_string, key_data)) != 0){
        key_filter_debug(error_code);
        return KEY_CODE_FILTER_ERROR | error_code;
    }

    gdk_window_add_filter(root_window, key_event_emiter, key_data);

    /* register event and connect     1,             2,                 3, 4,    5,    6,    7,           8, 9,            10 */
    signal_id = g_signal_new(event_name, G_TYPE_OBJECT, G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);
    g_signal_connect(widget, event_name, G_CALLBACK(event_handler), handler_parameter);

    key_data->widget            = widget;
    key_data->signal_id         = signal_id;

    return 0;
}/*}}}*/

int Terminal_Key_Event_Register(Terminal *terminal, const char *event_name, void event_handler(GtkWidget *, gpointer))
{/*{{{*/
    // Key_Filter 함수의 키 유효성을 명확히 하기 위해서.
    return Key_Filter(terminal->window, event_name, terminal->config.bd_key_mask, terminal->config.bd_key, Terminal_Show_Hide, &(terminal->key), terminal);
}/*}}}*/
