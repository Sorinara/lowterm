#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <vte-0.0/vte/vte.h>
#include <confuse.h>
#include <assert.h>

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

typedef struct _Config
{/*{{{*/
	char *name,
	     *scr_opt,
	     *bd_key,
         *bd_key_mask;

	int show_hide,
	    win_start_x,
	    win_start_y,
	    win_height,
	    win_width;

	char *font,
	     *locale;
	int antialias,
        font_bold;

	int tx_red,
	    tx_green,
	    tx_blue,
	    bg_red,
	    bg_green,
	    bg_blue;

	char *bg_image; /* no use */
	double bg_transparency;

	BOOL layer,
	     accept_focus,
	     blink_curser,
	     double_buffer,
	     audio_bell,
	     skip_pager,
	     taskbar_view,
	     all_workspace_view;

    int ani_sleep_sec;
    char *ani_start_place,
         *ani_end_place;

    /* for new hot key module */
} Config;/*}}}*/

typedef struct _KeyEvent{
    GtkWidget   *widget;
    guint        signal_id;

    KeyCode      key_code;
    unsigned int key_mask;

    void *handler_parameter;
} KeyEvent;

typedef struct _Terminal
{
    GtkWidget *window,
              *vte;
    int id,
        visible;

    KeyEvent key;
    Stack *visible_list_pointer;

    Config config;
} Terminal;

typedef struct _LowTerm{
    Terminal *terminal;
    int terminal_count;

    Stack visible_list;
} LowTerm;

/*  config.c */
int  Config_Get(LowTerm *lowterm);

/* key_event.c */
void Key_Filter_Debug(int error_code);
int  Key_Filter(GtkWidget *widget, const char *event_name, char *mask, char *key_symbol_string, void event_handler(GtkWidget *, gpointer), KeyEvent *key_data, void *user_data);

/* terminal.c */
void Terminal_New(int terminal_index, Terminal *terminal);
void Terminal_Set(Terminal terminal);

/* callback.c */
int  Terminal_Mouse(GtkWidget *widget, GdkEventButton *event);
void Terminal_Show_Hide(GtkWidget *nouse, gpointer user_data_param);
void Terminal_Exit(GtkWidget *g_widget, Terminal *get_value);
