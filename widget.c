#include "lowterm.h"

/* Dep Tree
 * Terminal_Widget_Create()
 *      widget_register()
 *          widget_class()
 *              widget_class_finalize()
 *          widget_instance()
 *              widget_key_grab()
 *              widget_key_handler()
 *  */

static GObjectClass *parent_class;
static GType type = 0;
static struct st_terminal *terminal;
static int class_signal = -1;

static void widget_class_finalize(GObject *object)
{/*{{{*/
	parent_class->finalize(G_OBJECT(object));
}/*}}}*/

/* register user define key */
static void widget_key_grab(int key_code, GdkWindow *root)
{/*{{{*/
	gdk_error_trap_push();
	XGrabKey(GDK_DISPLAY(), key_code, 0,                                GDK_WINDOW_XID(root), True, GrabModeAsync, GrabModeAsync);
	XGrabKey(GDK_DISPLAY(), key_code, Mod2Mask,                         GDK_WINDOW_XID(root), True, GrabModeAsync, GrabModeAsync);
	XGrabKey(GDK_DISPLAY(), key_code, Mod5Mask,                         GDK_WINDOW_XID(root), True, GrabModeAsync, GrabModeAsync);
	XGrabKey(GDK_DISPLAY(), key_code, LockMask,                         GDK_WINDOW_XID(root), True, GrabModeAsync, GrabModeAsync);
	XGrabKey(GDK_DISPLAY(), key_code, Mod2Mask | Mod5Mask,              GDK_WINDOW_XID(root), True, GrabModeAsync, GrabModeAsync);
	XGrabKey(GDK_DISPLAY(), key_code, Mod2Mask | LockMask,              GDK_WINDOW_XID(root), True, GrabModeAsync, GrabModeAsync);
	XGrabKey(GDK_DISPLAY(), key_code, Mod5Mask | LockMask,              GDK_WINDOW_XID(root), True, GrabModeAsync, GrabModeAsync); 
    XGrabKey(GDK_DISPLAY(), key_code, Mod2Mask | Mod5Mask | LockMask,   GDK_WINDOW_XID(root), True, GrabModeAsync, GrabModeAsync);
	gdk_flush();

	if(gdk_error_trap_pop()){
		fprintf(stderr, "Error grabbing key %d, %p\n", key_code, root);
	}
}/*}}}*/

/* key event handler */
static GdkFilterReturn widget_key_handler(GdkXEvent *xevent, GdkEvent *event, gpointer data)
{/*{{{*/
	XEvent *xev;
	XKeyEvent *key;

    /* fprintf(stderr, "Key (%d) %d\n", xev->type, ((BindKey *)data)->keycode); */
	xev = (XEvent *)xevent;
	if(xev->type != KeyPress){
		return GDK_FILTER_CONTINUE;
	}

	key = (XKeyEvent *)xevent;

	if(key->keycode == ((BindKey *)data)->keycode){
        g_signal_emit(data,((BindKey *)data)->signal, 0, 0);
		return GDK_FILTER_REMOVE;
	}

    return GDK_FILTER_CONTINUE;
}/*}}}*/

/* register user key event */
static void widget_class(BindKeyClass *klass)
{/*{{{*/
	GObjectClass *object_class;

	parent_class            = g_type_class_peek_parent(klass);
	object_class            =(GObjectClass*)klass;
	object_class->finalize  = widget_class_finalize;

	class_signal = g_signal_new("BindKey_Press",
			                   G_TYPE_FROM_CLASS(klass),
			                   G_SIGNAL_RUN_LAST,
			                   0,
			                   NULL, NULL,
			                   g_cclosure_marshal_VOID__INT,
			                   G_TYPE_NONE, 1, G_TYPE_INT);
}/*}}}*/

static void widget_instance(BindKey *object)
{/*{{{*/
	GdkDisplay *display;
	GdkScreen *screen;
	GdkWindow *root;
	guint screen_index;
    char **arg;

    object->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if(*(terminal->config.scr_opt)== 0){
        terminal->config.scr_opt  = getenv("SHELL");
    }

    g_shell_parse_argv(terminal->config.scr_opt,NULL,&arg,NULL);
    object->terminal = vte_terminal_new();
    /* argc argv 초기화 불량! */
    vte_terminal_fork_command_full(VTE_TERMINAL(object->terminal),
                                   0,               /* default tty */
                                   getenv("HOME"),  /* start directory */
                                   arg,             /* argv */
                                   NULL,            /* enviroment value */
                                   G_SPAWN_SEARCH_PATH,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL);
    g_strfreev(arg);

    /* TODO: destroy, child-exited*/
    g_signal_connect(object, "BindKey_Press", G_CALLBACK(Terminal_Show_Hide), terminal);
    g_signal_connect(object->terminal, "button-press-event", G_CALLBACK(Terminal_Mouse_Click), terminal);
    g_signal_connect(object->terminal, "child-exited", G_CALLBACK(Terminal_Exit), terminal);
    gtk_container_add(GTK_CONTAINER(object->window), object->terminal);

	object->keycode = XKeysymToKeycode(GDK_DISPLAY(), XStringToKeysym(terminal->config.bd_key));
    object->signal  = class_signal;
    object->onoff   = terminal->config.show_hide;

	display = gdk_display_get_default();

    for(screen_index = 0; screen_index < gdk_display_get_n_screens(display); screen_index ++){
        if((screen = gdk_display_get_screen(display, screen_index)) != NULL){
            root = gdk_screen_get_root_window(screen);

            if(object->keycode > 0){
                widget_key_grab(object->keycode, root);
                gdk_window_add_filter(root, widget_key_handler, object);
            }
        }
    }
}/*}}}*/

GType widget_register(void)
{/*{{{*/
	if(!type){
		static const GTypeInfo info = {
				sizeof(BindKeyClass),
				NULL,           /* base_init */
				NULL,           /* base_finalize */
				(GClassInitFunc)widget_class,
				NULL,           /* class_finalize */
				NULL,           /* class_data */
				sizeof(BindKey),
				0,
				(GInstanceInitFunc)widget_instance,
			};

		type = g_type_register_static(G_TYPE_OBJECT, "BindKey", &info, 0);
	}

	return type;
}/*}}}*/

BindKey* Terminal_Widget_Create(struct st_terminal *get_term)
{
    terminal = get_term;
	return KEYS(g_object_new(widget_register(), NULL));
}
