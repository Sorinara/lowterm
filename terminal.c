#include "lowterm.h"
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#define PALETTE_SIZE 16

static void child_callback(VteTerminal *vte, GPid pid, GError *error, gpointer user_data)
{/*{{{*/
	Terminal *terminal = (Terminal *)user_data;

    if(!vte){
        return;
    }
    
    if(pid == -1){
		gtk_widget_destroy(GTK_WIDGET(terminal->window));
        g_error_free(error);
    }

    vte_terminal_watch_child(VTE_TERMINAL(vte), pid);
}/*}}}*/

void Terminal_New(int terminal_index, Terminal *terminal)
{/*{{{*/
    GdkScreen *screen;
    GdkVisual *visual;
    char **arg,
         event_name[EVENT_NAME_MAX];

    if(terminal->config.execute == NULL){
        terminal->config.execute = getenv("SHELL");
    }

    terminal->window    = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    terminal->vte       = vte_terminal_new();
    terminal->exit_code = NULL;

    screen = gtk_widget_get_screen(terminal->window);
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual == NULL) {
        visual = gdk_screen_get_system_visual(screen);
    }

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(terminal->window, visual);
        gtk_widget_set_visual(terminal->vte, visual);
    }

    g_shell_parse_argv(terminal->config.execute, NULL, &arg, NULL);
    vte_terminal_spawn_async(VTE_TERMINAL(terminal->vte), VTE_PTY_DEFAULT, getenv("HOME"), arg, NULL, 0, NULL, NULL, NULL, -1, NULL, child_callback, terminal);
    g_strfreev(arg);

    terminal->id        = terminal_index;
    terminal->visible   = terminal->config.win_visible;
    snprintf(event_name, sizeof(event_name), "HotKey_Event%d", terminal_index);

    /* TODO: destroy, child-exited */
    g_signal_connect(terminal->vte, "button-press-event",   G_CALLBACK(Terminal_Mouse),     terminal);
    g_signal_connect(terminal->vte, "child-exited",         G_CALLBACK(Terminal_Exit),      terminal);

    /* if allow focus, register callback function */
    if(terminal->config.win_focus == 1){
        g_signal_connect(terminal->vte, "focus-in-event",   G_CALLBACK(Terminal_Focus_In),  terminal);
        g_signal_connect(terminal->vte, "focus-out-event",  G_CALLBACK(Terminal_Focus_Out), terminal);
    }

    gtk_container_add(GTK_CONTAINER(terminal->window), terminal->vte);

    if(strcmp(terminal->config.win_bind_key, "") != 0){
        if(Terminal_Key_Event_Register(terminal, event_name, Terminal_Show_Hide) != 0){
            terminal->config.win_bind_key="";
        }
    }
}/*}}}*/

void Terminal_Set(Terminal terminal)
{/*{{{*/
	GdkRGBA background_color,
            text_color,
            cursor_color,
            cursor_color_text;

	/* show hide status */
	if(terminal.config.win_visible == FALSE){
		terminal.visible = FALSE;
    }

	/* position */
    if(terminal.config.win_pos_x >= 0 && terminal.config.win_pos_y >= 0){
	    gtk_window_move(GTK_WINDOW(terminal.window), terminal.config.win_pos_x, terminal.config.win_pos_y);
    }

    /* size */
    vte_terminal_set_size(VTE_TERMINAL(terminal.vte), terminal.config.term_size_width, terminal.config.term_size_height);

    /* window proterty */
    gtk_window_set_title(GTK_WINDOW(terminal.window), terminal.config.name);
    gtk_window_set_role(GTK_WINDOW(terminal.window), terminal.config.name);
    gtk_window_set_decorated(GTK_WINDOW(terminal.window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(terminal.window), FALSE);

	/* set font */
	if(strcmp(terminal.config.term_font, "") != 0){
		vte_terminal_set_font(VTE_TERMINAL(terminal.vte), pango_font_description_from_string(terminal.config.term_font));
	}

	/* terminal backgound color */
    vte_terminal_set_default_colors(VTE_TERMINAL(terminal.vte));

	background_color.red    = terminal.config.term_back_color_red;
	background_color.green  = terminal.config.term_back_color_green;
	background_color.blue   = terminal.config.term_back_color_blue;
    background_color.alpha  = terminal.config.term_back_color_alpha;
	vte_terminal_set_color_background(VTE_TERMINAL(terminal.vte), &background_color);

	/* terminal text color */
	text_color.red 	    = terminal.config.term_text_color_red;
	text_color.green    = terminal.config.term_text_color_green;
	text_color.blue 	= terminal.config.term_text_color_blue;
    text_color.alpha    = terminal.config.term_text_color_alpha;
	vte_terminal_set_color_foreground(VTE_TERMINAL(terminal.vte), &text_color);

	/* antialias, background image, transparent, double-buffer, encoding, font bold  - dropped */

	/* default window manager */
	if(terminal.config.win_layer == 1){
 		/* create above window (단 윈도 매니져가 우선) */
		gtk_window_set_keep_above(GTK_WINDOW(terminal.window), TRUE);
    }else if(terminal.config.win_layer == 0){
 		/* create below window */
		gtk_window_set_keep_below(GTK_WINDOW(terminal.window), TRUE);
		gtk_window_set_resizable(GTK_WINDOW(terminal.window),  FALSE);
	}

	/* forcus */
	if(terminal.config.win_focus == FALSE){
		gtk_window_set_accept_focus(GTK_WINDOW(terminal.window), FALSE);
        gtk_window_set_urgency_hint(GTK_WINDOW(terminal.window), FALSE);
        gtk_window_set_focus_on_map(GTK_WINDOW(terminal.window), FALSE);
	}

	/* cusor */
    if(terminal.config.term_cursor_blink == TRUE){
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal.vte), VTE_CURSOR_BLINK_ON);
    }else if(terminal.config.term_cursor_blink == TRUE){
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal.vte), VTE_CURSOR_BLINK_OFF);
    }else{
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal.vte), VTE_CURSOR_BLINK_SYSTEM);
    }

    vte_terminal_set_cursor_shape(VTE_TERMINAL(terminal.vte), terminal.config.term_cursor_shape);

	cursor_color.red 	= terminal.config.term_cursor_color_red;
	cursor_color.green  = terminal.config.term_cursor_color_green;
	cursor_color.blue 	= terminal.config.term_cursor_color_blue;
    cursor_color.alpha  = terminal.config.term_cursor_color_alpha;
	vte_terminal_set_color_cursor(VTE_TERMINAL(terminal.vte), &cursor_color);

    cursor_color_text.red   = 1.0 - terminal.config.term_cursor_color_red;
    cursor_color_text.green = 1.0 - terminal.config.term_cursor_color_green;
    cursor_color_text.blue  = 1.0 - terminal.config.term_cursor_color_blue;
    cursor_color_text.alpha = 1.0 - terminal.config.term_cursor_color_alpha;
	vte_terminal_set_color_cursor_foreground(VTE_TERMINAL(terminal.vte), &cursor_color_text);

	/* bell */
	vte_terminal_set_audible_bell(VTE_TERMINAL(terminal.vte), terminal.config.term_audio_bell);

	/* hide parger */
	if(terminal.config.win_show_pager == TRUE){
		gtk_window_set_skip_pager_hint(GTK_WINDOW(terminal.window), TRUE);
    }

	/* hide taskbar (default TRUE) */
	if(terminal.config.win_show_taskbar == FALSE){
		gtk_window_set_skip_taskbar_hint(GTK_WINDOW(terminal.window), TRUE);
    }

    /* workspace view */
	if(terminal.config.win_show_all_workspace == TRUE){
		/* print all workspace */
		gtk_window_stick(GTK_WINDOW(terminal.window));
    }

	/* mouse hide (No set) */
	vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal.vte), TRUE);

	/* backspace, delete binding */
	vte_terminal_set_backspace_binding(VTE_TERMINAL(terminal.vte), VTE_ERASE_ASCII_DELETE);
	vte_terminal_set_delete_binding(VTE_TERMINAL(terminal.vte), VTE_ERASE_DELETE_SEQUENCE);

    /* hyperlink */
    VteRegex* regex = vte_regex_new_for_match(HTTP_REGEXP, -1, PCRE2_UTF | PCRE2_MULTILINE | PCRE2_CASELESS, NULL);
	vte_terminal_match_add_regex(VTE_TERMINAL(terminal.vte), regex, 0);
    
    /* 일단 터미널과 윈도우 위젯들을 보여준다 */
    gtk_widget_show_all(terminal.window);

    /* 감추게 FALSE 이면 감추게 한다 
     * 걍 gtk_widget_show를 쓰면 안되는 이유가 끊기기 때문에 따로 gtk_widget_hide를 써야 */
    if(terminal.config.win_visible == FALSE){
        gtk_widget_hide(terminal.window);
    }
}/*}}}*/
