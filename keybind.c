#include "lowterm.h"

#define DEBUG

static void widget_class_init (BindKeyClass *klass);
static void widget_init       (BindKey      *object);
static void widget_finalize   (GObject      *object);

static void grab_mmkey (int key_code, GdkWindow *root);

static GdkFilterReturn filter_key (GdkXEvent *xevent,
			              GdkEvent *event,
			              gpointer data);

static GObjectClass *parent_class;
static GType type = 0;
static struct st_terminal *terminal;
static int class_signal = -1;

GType
widget_get_type(void)
{
	if (!type) {
		static const GTypeInfo info = {
				sizeof (BindKeyClass),
				NULL,           /* base_init */
				NULL,           /* base_finalize */
				(GClassInitFunc) widget_class_init,
				NULL,           /* class_finalize */
				NULL,           /* class_data */
				sizeof (BindKey),
				0,
				(GInstanceInitFunc) widget_init,
			};

		type = g_type_register_static (G_TYPE_OBJECT, "BindKey",&info, 0);
	}

	return type;
}

static void
widget_class_init(BindKeyClass *klass)
{
	GObjectClass *object_class;

	parent_class = g_type_class_peek_parent (klass);
	object_class = (GObjectClass*) klass;

	object_class->finalize = widget_finalize;

	class_signal = g_signal_new ("BindKey_Press",
			                   G_TYPE_FROM_CLASS (klass),
			                   G_SIGNAL_RUN_LAST,
			                   0,
			                   NULL, NULL,
			                   g_cclosure_marshal_VOID__INT,
			                   G_TYPE_NONE, 1, G_TYPE_INT);
}

static void
widget_finalize (GObject *object)
{
	parent_class->finalize (G_OBJECT(object));
}

static void
widget_init (BindKey *object)
{
	GdkDisplay *display;
	GdkScreen *screen;
	GdkWindow *root;
	guint i;
    char **arg;

    //usleep(250000);

    object -> window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if(*(terminal->config.scr_opt) == 0)
        terminal->config.scr_opt  = getenv("SHELL");

    g_shell_parse_argv(terminal->config.scr_opt,NULL,&arg,NULL);
    object -> terminal = vte_terminal_new();
    /* argc argv 초기화 불량! */
    vte_terminal_fork_command_full(VTE_TERMINAL(object -> terminal),
                                   0,               /* default tty */
                                   getenv("HOME"),  /* start directory */
                                   arg,             /* argv */
                                   NULL,            /* enviroment value */
                                   G_SPAWN_SEARCH_PATH,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL);
    g_strfreev (arg);

    /*시그널 이것도 상당히 문제가 크다 destroy와 child-exited*/
    g_signal_connect(object,"BindKey_Press",G_CALLBACK(Terminal_Show_Hide),terminal);
    g_signal_connect(object->terminal,"button-press-event",G_CALLBACK(event_mouse_press),terminal);
    g_signal_connect(object->terminal,"drag-data-get", G_CALLBACK(event_mouse_drag), terminal);
    g_signal_connect(object -> terminal,"child-exited",G_CALLBACK(App_Quit),terminal);

    gtk_container_add(GTK_CONTAINER(object -> window),object -> terminal);

	object -> keycode = XKeysymToKeycode (GDK_DISPLAY (),XStringToKeysym(terminal->config.bd_key));
    object -> signal = class_signal;
    object -> onoff  = terminal->config.show_hide;

	display = gdk_display_get_default ();

    for (i = 0; i < gdk_display_get_n_screens (display); i++) {
        screen = gdk_display_get_screen (display, i);

        if (screen != NULL) {
            root = gdk_screen_get_root_window (screen);

            if (object -> keycode > 0){
                grab_mmkey (object -> keycode, root);
                gdk_window_add_filter (root, filter_key, object);
            }
        }
    }
}

BindKey*
Create_Terminal_Widget(struct st_terminal *get_term)
{
    terminal = get_term;
	return KEYS(g_object_new (TYPE_KEYS, NULL));
}

static void
grab_mmkey
(int key_code, GdkWindow *root)
{
	gdk_error_trap_push ();

	XGrabKey (GDK_DISPLAY (), key_code,
		  0,
		  GDK_WINDOW_XID (root), True,
		  GrabModeAsync, GrabModeAsync);
	XGrabKey (GDK_DISPLAY (), key_code,
		  Mod2Mask,
		  GDK_WINDOW_XID (root), True,
		  GrabModeAsync, GrabModeAsync);
	XGrabKey (GDK_DISPLAY (), key_code,
		  Mod5Mask,
		  GDK_WINDOW_XID (root), True,
		  GrabModeAsync, GrabModeAsync);
	XGrabKey (GDK_DISPLAY (), key_code,
		  LockMask,
		  GDK_WINDOW_XID (root), True,
		  GrabModeAsync, GrabModeAsync);
	XGrabKey (GDK_DISPLAY (), key_code,
		  Mod2Mask | Mod5Mask,
		  GDK_WINDOW_XID (root), True,
		  GrabModeAsync, GrabModeAsync);
	XGrabKey (GDK_DISPLAY (), key_code,
		  Mod2Mask | LockMask,
		  GDK_WINDOW_XID (root), True,
		  GrabModeAsync, GrabModeAsync);
	XGrabKey (GDK_DISPLAY (), key_code,
		  Mod5Mask | LockMask,
		  GDK_WINDOW_XID (root), True,
		  GrabModeAsync, GrabModeAsync);
	XGrabKey (GDK_DISPLAY (), key_code,
		  Mod2Mask | Mod5Mask | LockMask,
		  GDK_WINDOW_XID (root), True,
		  GrabModeAsync, GrabModeAsync);

	gdk_flush ();
	if (gdk_error_trap_pop ()) {
		fprintf (stderr, "Error grabbing key %d, %p\n", key_code, root);
	}
}

/* 위에꺼와 는 처음 객체 만들때만 상관있고
 * 아래는 계속 지키는 것으로 다르다 */
static GdkFilterReturn
filter_key
(GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
	XEvent *xev;
	XKeyEvent *key;

	xev = (XEvent *) xevent;
	if (xev->type != KeyPress) {
		return GDK_FILTER_CONTINUE;
	}

	key = (XKeyEvent *) xevent;

	if (key -> keycode == ((BindKey *)data)->keycode) {
        g_signal_emit (data,((BindKey *)data)->signal, 0, 0);
		return GDK_FILTER_REMOVE;
	}

    return GDK_FILTER_CONTINUE;
}
