#include "lowterm.h"

void Terminal_New(int terminal_index, Terminal *terminal)
{/*{{{*/
    char **arg,
         event_name[EVENT_NAME_MAX];
    int error_code;

    terminal->window    = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    terminal->exit_code = NULL;

    if(terminal->config.execute == NULL){
        terminal->config.execute = getenv("SHELL");
    }

    g_shell_parse_argv(terminal->config.execute, NULL, &arg, NULL);
    terminal->vte = vte_terminal_new();

    /* argc argv 초기화 불량! */
    vte_terminal_fork_command_full(VTE_TERMINAL(terminal->vte),
                                   0,               /* default tty */
                                   getenv("HOME"),  /* start directory */
                                   arg,             /* argv */
                                   NULL,            /* enviroment value */
                                   G_SPAWN_SEARCH_PATH,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &(terminal->exit_code));
    g_strfreev(arg);

    terminal->id                    = terminal_index;
    terminal->visible               = terminal->config.win_visible;
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
        if(error_code = Terminal_Key_Event_Register(terminal, event_name, Terminal_Show_Hide) != 0){
            terminal->config.win_bind_key="";
        }
    }
}/*}}}*/

void Terminal_Set(Terminal terminal)
{/*{{{*/
	VteTerminalAntiAlias antialias = VTE_ANTI_ALIAS_USE_DEFAULT;
   	PangoFontDescription *p_font_fd;
	GdkColor terminal_fg_color,
             terminal_bg_color;

	/* show hide status */
	if(terminal.config.win_visible == FALSE){
		terminal.visible = FALSE;
    }

	/* position */
    if(terminal.config.win_pos_x >= 0 && terminal.config.win_pos_y >= 0){
	    gtk_window_move(GTK_WINDOW(terminal.window), terminal.config.win_pos_x, terminal.config.win_pos_y);
    }

    /* privent brake terminal */
	gtk_widget_set_size_request(terminal.window, terminal.config.win_size_width, terminal.config.win_size_height);
	gtk_widget_set_size_request(terminal.vte, terminal.config.win_size_width, terminal.config.win_size_height);

    /* window proterty */
    gtk_window_set_title(GTK_WINDOW(terminal.window), terminal.config.name);
    gtk_window_set_role(GTK_WINDOW(terminal.window), terminal.config.name);
    gtk_window_set_decorated(GTK_WINDOW(terminal.window), FALSE);

	/* set font/antialias */
	if(strcmp(terminal.config.term_font, "") != 0){
		p_font_fd = pango_font_description_from_string(terminal.config.term_font);

		if(terminal.config.term_antialias == TRUE){
			vte_terminal_set_font(VTE_TERMINAL(terminal.vte), p_font_fd);
            /* vte_terminal_set_font_full(VTE_TERMINAL(terminal.vte), p_font_fd, antialias); */
        }else{
			vte_terminal_set_font(VTE_TERMINAL(terminal.vte), p_font_fd);
        }
	}

	/* locale */
	if(strcmp(terminal.config.term_locale,"")!=0){
		vte_terminal_set_encoding(VTE_TERMINAL(terminal.vte), terminal.config.term_locale);
    }

	/* font bold (default TRUE) */
	vte_terminal_set_allow_bold(VTE_TERMINAL(terminal.vte), terminal.config.term_font_bold);

	/* terminal color backgound,foreground */
	terminal_fg_color.red 	= terminal.config.term_textcolor_red;
	terminal_fg_color.green = terminal.config.term_textcolor_green;
	terminal_fg_color.blue 	= terminal.config.term_textcolor_blue;
	terminal_bg_color.red 	= terminal.config.term_backcolor_red;
	terminal_bg_color.green = terminal.config.term_backcolor_green;
	terminal_bg_color.blue 	= terminal.config.term_backcolor_blue;
	vte_terminal_set_colors(VTE_TERMINAL(terminal.vte), &terminal_fg_color, &terminal_bg_color, NULL, 0);

	/*background image */
	if(strcmp(terminal.config.term_image, "") != 0){
	    vte_terminal_set_background_image_file(VTE_TERMINAL(terminal.vte), terminal.config.term_image);
    }

	/* transparent */
	vte_terminal_set_background_transparent(VTE_TERMINAL(terminal.vte), TRUE);

	/* saturation (contrast) */
	vte_terminal_set_background_saturation(VTE_TERMINAL(terminal.vte), terminal.config.term_transparent);
    
	/* default window manager */
	if(terminal.config.win_layer == 1){
 		/* create above window (단 윈도 매니져가 우선) */
		gtk_window_set_keep_above(GTK_WINDOW(terminal.window), TRUE);
    }else if(terminal.config.win_layer == 0){
 		/* create below window */
		gtk_window_set_keep_below(GTK_WINDOW(terminal.window), TRUE);
		gtk_window_set_resizable(GTK_WINDOW(terminal.window),  FALSE);
	}

	/* set forcus (TODO: modify TRUE FALSE ) */
	if(terminal.config.win_focus == FALSE){
		gtk_window_set_accept_focus(GTK_WINDOW(terminal.window), FALSE);
        gtk_window_set_urgency_hint(GTK_WINDOW(terminal.window), FALSE);
        gtk_window_set_focus_on_map(GTK_WINDOW(terminal.window), FALSE);
	}

	/* cusor blink (default FALSE) */
    if(terminal.config.term_blink_curser == TRUE){
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal.vte), VTE_CURSOR_BLINK_ON);
    }else if(terminal.config.term_blink_curser == TRUE){
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal.vte), VTE_CURSOR_BLINK_OFF);
    }else{
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal.vte), VTE_CURSOR_BLINK_SYSTEM);
    }

	/* double-buffer (anti font fold) */
	gtk_widget_set_double_buffered(terminal.vte, terminal.config.term_double_buffer);
	
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
    vte_terminal_match_add_gregex(VTE_TERMINAL(terminal.vte), g_regex_new(HTTP_REGEXP, 0, 0, NULL), 0);

    /* usleep(250000); */

    /* 일단 터미널과 윈도우 위젯들을 보여준다 */
    gtk_widget_show_all(terminal.window);

    /* 감추게 FALSE 이면 감추게 한다 
     * 걍 gtk_widget_show를 쓰면 안되는 이유가 끊기기 때문에 따로 gtk_widget_hide를 써야 */
    if(terminal.config.win_visible == FALSE){
        gtk_widget_hide_all(terminal.window);
    }
}/*}}}*/
