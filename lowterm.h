#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <confuse.h>
#include <assert.h>
#include <unistd.h>

#include "data.h"

#define  EVENT_NAME_MAX         32

#define  KEY_CODE_MASK_EMPTY    0x00
#define  KEY_MASK_SHIFT         0x01
#define  KEY_MASK_CONTROL       0x02
#define  KEY_MASK_CAPS_LOCK     0x04
#define  KEY_MASK_ALT           0x08
#define  KEY_MASK_NUM_LOCK      0x18
#define  KEY_MASK_HYPER         0x20
#define  KEY_MASK_SUPER         0x40
#define  KEY_MASK_SCROLL_LOCK   0x80

#define  KEY_SYMBOL_EMPTY       0x01
#define  KEY_SYMBOL_UNKNOWN     0x02
#define  KEY_CODE_UNKNOWN       0x04
#define  KEY_CODE_MASK_UNKNOWN  0x08
#define  KEY_CODE_MASK_ERROR    0x10
#define  KEY_CODE_GRAB_ERROR    0x20
#define  KEY_CODE_FILTER_ERROR  0x40

#define  BOOL                   int

/* TODO: for user - config 파일 내부에 설정이 가능하도록 할것 */
#define  PROCESS_NAME           "lowterm"
#define  HTTP_REGEXP            "(ftp|(htt(p|ps)))://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*"
//#define  BROWSER_NAME           "google-chrome"
#define  BROWSER_NAME           "firefox"
#define  IMAGEVIEWER_NAME       "display"
#define  MOVE_UNIT_SIZE         25
#define  WINDOW_DELAY           8000
#define  CONFIG_FILE_NAME       ".mytermcfg"
#define  CONFIG_FILE_PATH_MAX   100
#define  CONFIG_TITLE           "Terminal"

#define  CONFIG_DEFAULT_NAME    "Terminal"
typedef struct _Config
{/*{{{*/
	char *name,
	     *execute,
         *win_bind_key,
         *win_bind_key_mask;
	int   win_visible,
          win_pos_x,
	      win_pos_y,
          win_layer;
	BOOL  win_focus,
          win_show_pager,
	      win_show_taskbar,
	      win_show_all_workspace;
    int   win_animation_delay;
    char *win_animation_move_start,
         *win_animation_move_end;
	int   term_size_height,
	      term_size_width;
	char *term_font;
    int   term_font_bold;
	char *term_locale;
    int   term_cursor_shape;
	BOOL  term_cursor_blink,
	      term_audio_bell;
	double term_back_color_red,
	       term_back_color_green,
	       term_back_color_blue,
           term_back_color_alpha,
           term_text_color_red,
	       term_text_color_green,
	       term_text_color_blue,
           term_text_color_alpha,
           term_cursor_color_red,
           term_cursor_color_green,
           term_cursor_color_blue,
           term_cursor_color_alpha;
    /* for new hot key module */
} Config;/*}}}*/

typedef struct _KeyEvent{/*{{{*/
    GtkWidget   *widget;
    guint        signal_id;

    KeyCode      key_code;
    unsigned int key_mask;

    void *handler_parameter;
} KeyEvent;/*}}}*/

typedef struct _Terminal {/*{{{*/
    GtkWidget *window,
              *vte;
    int id,
        visible;

    KeyEvent key;
    Stack *visible_list_pointer;

    Config config;
    GError *exit_code;
} Terminal;/*}}}*/

typedef struct _LowTerm{/*{{{*/
    Terminal *terminal;
    int terminal_count;

    Stack visible_list;
} LowTerm;/*}}}*/

/*  config.c */
int Config_Get(int argc, char *argv[], LowTerm *lowterm);

/* key_event.c */
int  Terminal_Key_Event_Register(Terminal *terminal, const char *event_name, void event_handler(GtkWidget *, gpointer));

/* terminal.c */
void Terminal_New(int terminal_index, Terminal *terminal);
void Terminal_Set(Terminal terminal);

/* callback.c */
int      Terminal_Mouse(GtkWidget *widget, GdkEventButton *event);
void     Terminal_Show_Hide(GtkWidget *nouse, gpointer user_data_param);
gboolean Terminal_Focus_In(GtkWidget *widget, GdkEvent *event, gpointer user_data_param);
gboolean Terminal_Focus_Out(GtkWidget *widget, GdkEvent *event, gpointer user_data_param);
void     Terminal_Exit(GtkWidget *widget, gpointer user_data_param);
