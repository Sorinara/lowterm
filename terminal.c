#include "lowterm.h"

int Terminal_Set(struct st_terminal term)
{
	VteTerminalAntiAlias antialias = VTE_ANTI_ALIAS_USE_DEFAULT;
   	PangoFontDescription *p_font_fd;
	GdkColor terminal_fg_color,
             terminal_bg_color;
	GtkAccelGroup *accel_group;
	GClosure *copy,
             *paste;
	/* TODO: break data(?) GtkAccelGroup, PangoFontDescription */

    /* proterty */
    gtk_window_set_title(GTK_WINDOW(term.object->window), term.config.name);
    gtk_window_set_role(GTK_WINDOW(term.object->window), term.config.name);
    gtk_window_set_decorated(GTK_WINDOW(term.object->window), FALSE);

	/* window position init */
	gtk_window_move(GTK_WINDOW(term.object->window),term.config.win_start_x, term.config.win_start_y);

    /* privent brake terminal */
	gtk_widget_set_size_request(term.object->window,term.config.win_width, term.config.win_height);
	gtk_widget_set_size_request(term.object->terminal,term.config.win_width, term.config.win_height);

    /* window role->widget.c */
	/* window layer */
	if(term.config.all_workspace_view == TRUE){
		/* print all workspace */
		gtk_window_stick(GTK_WINDOW(term.object->window));
    }

	/* hide taskbar (default TRUE) */
	if(term.config.taskbar_view == FALSE){
		gtk_window_set_skip_taskbar_hint(GTK_WINDOW(term.object->window), TRUE);
    }

	/* default window manager */
	if(term.config.layer == 1){
 		/* create above window (단 윈도 매니져가 우선) */
		gtk_window_set_keep_above(GTK_WINDOW(term.object->window), TRUE);
    }else if(term.config.layer==0){
 		/* create below window */
		gtk_window_set_keep_below(GTK_WINDOW(term.object->window), TRUE);
		gtk_window_set_resizable(GTK_WINDOW(term.object->window), FALSE);
	}

	/* show hide status */
	if(term.config.show_hide == FALSE){
		term.object->onoff = OFF;
    }

	/* set font/antialias */
	if(strcmp(term.config.font, "") != 0){
		p_font_fd = pango_font_description_from_string(term.config.font);

		if(term.config.antialias == TRUE){
			vte_terminal_set_font(VTE_TERMINAL(term.object->terminal), p_font_fd);
            /* vte_terminal_set_font_full(VTE_TERMINAL(term.object->terminal), p_font_fd, antialias); */
        }else{
			vte_terminal_set_font(VTE_TERMINAL(term.object->terminal), p_font_fd);
        }
	}

	/* set forcus (TODO: modify TRUE FALSE ) */
	if(term.config.accept_focus == FALSE){
		gtk_window_set_accept_focus(GTK_WINDOW(term.object->window), FALSE);
        gtk_window_set_urgency_hint(GTK_WINDOW(term.object->window), FALSE);
        gtk_window_set_focus_on_map(GTK_WINDOW(term.object->window), FALSE);
	}

	/* hide parger */
	if(term.config.skip_pager == TRUE){
		gtk_window_set_skip_pager_hint(GTK_WINDOW(term.object->window), TRUE);
    }

	/* font bold (default TRUE) */
	vte_terminal_set_allow_bold(VTE_TERMINAL(term.object->terminal), term.config.font_bold);

	/* locale */
	if(strcmp(term.config.locale,"")!=0){
		vte_terminal_set_encoding(VTE_TERMINAL(term.object->terminal), term.config.locale);
    }

	/* bell */
	vte_terminal_set_audible_bell(VTE_TERMINAL(term.object->terminal), term.config.audio_bell);

	/* mouse hide (No set) */
	vte_terminal_set_mouse_autohide(VTE_TERMINAL(term.object->terminal), TRUE);

	/* cusor blink (default FALSE) */
    if(term.config.blink_curser == TRUE){
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(term.object->terminal), VTE_CURSOR_BLINK_ON);
    }else if(term.config.blink_curser == TRUE){
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(term.object->terminal), VTE_CURSOR_BLINK_OFF);
    }else{
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(term.object->terminal), VTE_CURSOR_BLINK_SYSTEM);
    }

	/*background image */
	/* if(strcmp(term.config . bg_image,"")!=0)
	    vte_terminal_set_background_image_file(VTE_TERMINAL(term.config.terminal), */

	/* transparent */
	vte_terminal_set_background_transparent(VTE_TERMINAL(term.object->terminal), TRUE);

	/* saturation (contrast) */
	vte_terminal_set_background_saturation(VTE_TERMINAL(term.object->terminal), term.config.bg_transparency);
    
	/* backspace, delete binding */
	vte_terminal_set_backspace_binding(VTE_TERMINAL(term.object->terminal), VTE_ERASE_ASCII_DELETE);
	vte_terminal_set_delete_binding(VTE_TERMINAL(term.object->terminal), VTE_ERASE_DELETE_SEQUENCE);

	/* terminal color backgound,foreground */
	terminal_fg_color . red 	= term.config.tx_red;
	terminal_fg_color . green 	= term.config.tx_green;
	terminal_fg_color . blue 	= term.config.tx_blue;
	terminal_bg_color . red 	= term.config.bg_red;
	terminal_bg_color . green 	= term.config.bg_green;
	terminal_bg_color . blue 	= term.config.bg_blue;
	vte_terminal_set_colors(VTE_TERMINAL(term.object->terminal), &terminal_fg_color, &terminal_bg_color, NULL, 0);

	/* double-buffer (anti font fold) */
	gtk_widget_set_double_buffered(term.object->terminal, term.config.double_buffer);
	
    /* hyperlink */
    vte_terminal_match_add_gregex(VTE_TERMINAL(term.object->terminal), g_regex_new(HTTP_REGEXP, 0, 0, NULL), 0);

	/* copy & paste */
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(term.object->window), accel_group);
	
	paste = g_cclosure_new_swap((GCallback) Terminal_Paste, term.object->terminal, NULL);
    gtk_accel_group_connect(accel_group, 'v', GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE,paste);
	
	copy = g_cclosure_new_swap((GCallback) Terminal_Copy, term.object->terminal, NULL);
 	gtk_accel_group_connect(accel_group, 'c', GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE,copy);
    /*
    GClosure *quit;
	quit = g_cclosure_new_swap((GCallback) gtk_main_quit,NULL,NULL);
	gtk_accel_group_connect(accel_group,'q' , GDK_CONTROL_MASK | GDK_SHIFT_MASK ,GTK_ACCEL_VISIBLE,quit);
    */
	return 0;
}
