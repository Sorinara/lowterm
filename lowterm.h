#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <vte-0.0/vte/vte.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <glib-object.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <X11/Xlib.h>
#include <X11/XF86keysym.h>

#include <confuse.h>

#define PROCESS_NAME            "lowterm"
#define HTTP_REGEXP             "(ftp|(htt(p|ps)))://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*"

#define ON                      1
#define OFF                     0

#define TYPE_KEYS               widget_register()
#define KEYS(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_KEYS, BindKey))

typedef struct _BindKey      BindKey;
typedef struct _BindKeyClass BindKeyClass;

struct _BindKeyClass
{
	GObjectClass parent_class;
};

struct st_config
{/*{{{*/
	char *name;
	char *scr_opt;
	char *bd_key;
	int win_start_x;
	int win_start_y;
	int win_height;
	int win_width;
	char *font;
	int font_bold;
	int tx_red;	
	int tx_green;
	int tx_blue;
	int bg_red;
	int bg_blue;
	int bg_green;
	int blink_curser;
	int audio_bell;
	int accept_focus;
	int skip_pager;
	int antialias;
	int double_buffer;
	int show_hide;
	char *locale;
	char *bg_image;
	double bg_transparency;
	int layer;
	int taskbar_view;
	int all_workspace_view;
    int ani_sleep_sec;
    char *ani_start_place;
    char *ani_end_place;
};/*}}}*/

struct st_terminal
{
    // index는 0부터 시작한다
    int index;
    struct st_config config;
    BindKey *object;
};

struct _BindKey
{
	GObject parent;
    guint signal;
	GtkWidget *window;
	GtkWidget *terminal;
    KeyCode keycode;
	int onoff;
    struct st_terminal *mother;
};

/* config.c */
int Config_Get(struct st_terminal **term_cfg);

/* widget.c */
BindKey* Terminal_Widget_Create(struct st_terminal *get_term);

/* terminal.c */
int Terminal_Set(struct st_terminal term);

/* action.c */
int  Terminal_Show_Hide(BindKey *object,struct st_terminal *term_head);

/* callback.c */
void Terminal_Copy(GtkWidget *get_data,void *data); 	
void Terminal_Paste(GtkWidget *get_data,void *data);
int  Terminal_Mouse_Click(GtkWidget *widget,GdkEventButton *event);
void Terminal_Exit(GtkWidget *g_widget,struct st_terminal *get_value);
