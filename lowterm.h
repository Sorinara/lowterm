#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<ctype.h>

#include<vte-0.0/vte/vte.h>
#include<gtk/gtk.h>
#include<gdk/gdk.h>
#include<gdk/gdkx.h>
#include<glib-object.h>
#include<sys/types.h>
#include<sys/stat.h>

#include <X11/Xlib.h>
#include <X11/XF86keysym.h>

#include<confuse.h>

#define CMD_SIZ 512
#define PROCESS_NAME "lowterm"

#define ON 1
#define OFF 0

#define KEYS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_KEYS, BindKey))
#define KEYS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_KEYS, BindKeyClass))
#define IS_KEYS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_KEYS))
#define IS_KEYS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_KEYS))
#define KEYS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_KEYS, BindKeyClass))

#define HTTP_REGEXP "(ftp|(htt(p|ps)))://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*"

typedef struct _BindKey      BindKey;
typedef struct _BindKeyClass BindKeyClass;

struct _BindKeyClass
{
	GObjectClass parent_class;
};

struct st_config
{
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
};

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

/* bindkey.c */
GType   widget_get_type (void);
BindKey* Create_Terminal_Widget(struct st_terminal *get_term);

/* terminal.c */
int Terminal_Window_Move(GtkWidget *window,int pos_x,int pos_y,char *status,char *direction); 
int Terminal_Show_Hide(BindKey *object,struct st_terminal *term_head);

/* get_conf.c */
int Get_Config(struct st_terminal **term_cfg);

/* set_conf.c */
int Set_Config(struct st_terminal term);

/* callback.c */
int Get_Keycode(char *key);
void event_mouse_drag (GtkWidget *widget, 
                  GdkDragContext *context, 
                  int x,
                  int y,
                  GtkSelectionData *selection_data,
                  unsigned int info,
                  guint32 time,
                  gpointer data);
void copy_clipboard(GtkWidget *get_data,void *data); 	
void paste_clipboard(GtkWidget *get_data,void *data);
int event_mouse_press(GtkWidget *widget,GdkEventButton *event);
//void window_manager(GtkWidget *widget,void *data);
void App_Quit(GtkWidget *g_widget,struct st_terminal *get_value);
