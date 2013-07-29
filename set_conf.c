#include "lowterm.h"

int Set_Config(struct st_terminal term)
{
   	PangoFontDescription *p_font_fd;

	//데이터 망가질 위험이 있음??(아래 2줄)
	GtkAccelGroup *accel_group;
	GClosure *copy,
             *paste,
             *mouse_paste,
             *quit;
	GdkColor terminal_fg_color,terminal_bg_color;
	VteTerminalAntiAlias antialias = VTE_ANTI_ALIAS_USE_DEFAULT;
	
    // window proterty
    gtk_window_set_title(GTK_WINDOW(term.object -> window),term.config.name);

    // openbox면 아래를 주석처리, 플빡이면 주석해제 
    gtk_window_set_role(GTK_WINDOW(term.object -> window),term.config.name);
    gtk_window_set_decorated(GTK_WINDOW(term.object -> window),FALSE);

	//window (place) init	
	gtk_window_move(GTK_WINDOW(term.object->window),term.config.win_start_x,
												   term.config.win_start_y);

    // 터미널 일그러짐 방지
	gtk_widget_set_size_request(term.object->window,term.config.win_width,
													term.config.win_height);
	gtk_widget_set_size_request(term.object->terminal,term.config.win_width,
													term.config.win_height);

    // window role -> keybind.c

	//window layer
	if(term.config.all_workspace_view == TRUE)
		gtk_window_stick(GTK_WINDOW(term.object->window));						//모든 워크스페이스에 출력

	//taskbar hide
	if(term.config.taskbar_view == FALSE)	//default TRUE
		gtk_window_set_skip_taskbar_hint(GTK_WINDOW(term.object->window), TRUE);	//테스크바에 안보이게 하기

	//default window manager
	if(term.config.layer == 1)	
		gtk_window_set_keep_above(GTK_WINDOW(term.object->window), TRUE); 		//최상위 윈도우로 만들기 (단 윈도 매니져가 우선)
	else if(term.config.layer==0){
		gtk_window_set_keep_below(GTK_WINDOW(term.object->window), TRUE); 		//최하위 윈도우 로 만들기
		gtk_window_set_resizable(GTK_WINDOW(term.object->window),FALSE);
	}

	//show hide 상태 맞춤
	if(term.config.show_hide == FALSE)
		term.object->onoff = OFF;

	//font,antialias
	if(strcmp(term.config.font, "") != 0){
		p_font_fd=pango_font_description_from_string (term.config.font);

		if(term.config.antialias == TRUE){
			vte_terminal_set_font(VTE_TERMINAL(term.object->terminal), p_font_fd);
            /*
			vte_terminal_set_font_full(VTE_TERMINAL(term.object->terminal),
					p_font_fd, antialias); 
            */
        }else{
			vte_terminal_set_font(VTE_TERMINAL(term.object->terminal), p_font_fd);
        }
	}

	//포커스 설정(TRUE FALSE 수정할것)
	if(term.config.accept_focus == FALSE){
		gtk_window_set_accept_focus(GTK_WINDOW(term.object->window),FALSE);
        gtk_window_set_urgency_hint(GTK_WINDOW(term.object->window),FALSE);
        gtk_window_set_focus_on_map(GTK_WINDOW(term.object->window),FALSE);
	}

	//파저에 안보이게 설정
	if(term.config.skip_pager == TRUE)
		gtk_window_set_skip_pager_hint(GTK_WINDOW(term.object->window),TRUE);

	//font bold (default TRUE)
	vte_terminal_set_allow_bold(VTE_TERMINAL(term.object->terminal),
											 term.config.font_bold);

	//locale
	if(strcmp(term.config.locale,"")!=0)
		vte_terminal_set_encoding(VTE_TERMINAL(term.object->terminal),
											   term.config.locale);	

	//bell (??)
	vte_terminal_set_audible_bell(VTE_TERMINAL(term.object->terminal),
											   term.config.audio_bell);

	//mouse hide (No set)
	vte_terminal_set_mouse_autohide(VTE_TERMINAL(term.object->terminal),TRUE);

	//cusor blink (default FALSE)
    if(term.config.blink_curser == TRUE)
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(term.object->terminal),VTE_CURSOR_BLINK_ON);
    else if(term.config.blink_curser == TRUE)
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(term.object->terminal),VTE_CURSOR_BLINK_OFF);
    else
	    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(term.object->terminal),VTE_CURSOR_BLINK_SYSTEM);

	//background image
	//if(strcmp(term.config . bg_image,"")!=0)
	//	vte_terminal_set_background_image_file(VTE_TERMINAL(term.config.terminal),

	//transparent
	vte_terminal_set_background_transparent(VTE_TERMINAL(term.object->terminal),TRUE);	//투명여부

	//saturation
	vte_terminal_set_background_saturation(VTE_TERMINAL(term.object->terminal),
														term.config.bg_transparency);	//투명도..이녀석이 문제다 위에건상관 없다
																//같은값으로 고정하면 하나도 안끈김
	
	//backspace,delete binding(ADD??)
	vte_terminal_set_backspace_binding(VTE_TERMINAL(term.object->terminal),
													VTE_ERASE_ASCII_DELETE);

	vte_terminal_set_delete_binding(VTE_TERMINAL(term.object->terminal),
												 VTE_ERASE_DELETE_SEQUENCE);

    // http 링크
    vte_terminal_match_add_gregex(VTE_TERMINAL(term.object->terminal), g_regex_new(HTTP_REGEXP, 0, 0, NULL), 0);

	//terminal color backgound,foreground......
	terminal_fg_color . red 	= term.config.tx_red;
	terminal_fg_color . green 	= term.config.tx_green;
	terminal_fg_color . blue 	= term.config.tx_blue;
	terminal_bg_color . red 	= term.config.bg_red;
	terminal_bg_color . green 	= term.config.bg_green;
	terminal_bg_color . blue 	= term.config.bg_blue;

	vte_terminal_set_colors(VTE_TERMINAL(term.object->terminal),&terminal_fg_color,&terminal_bg_color,NULL,0);

	//double-buffer (anti font fold)
	gtk_widget_set_double_buffered(term.object->terminal,term.config.double_buffer);
	
	//copy and paste
	accel_group = gtk_accel_group_new ();
	gtk_window_add_accel_group (GTK_WINDOW(term.object->window), accel_group);
	
	paste = g_cclosure_new_swap ((GCallback) paste_clipboard, term.object->terminal, NULL);
    gtk_accel_group_connect (accel_group, 'v', GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE,paste);
	
	copy = g_cclosure_new_swap ((GCallback) copy_clipboard,term.object->terminal, NULL);
 	gtk_accel_group_connect (accel_group, 'c', GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE,copy);

    /*
	quit = g_cclosure_new_swap ((GCallback) gtk_main_quit,NULL,NULL);
	gtk_accel_group_connect (accel_group,'q' , GDK_CONTROL_MASK | GDK_SHIFT_MASK ,GTK_ACCEL_VISIBLE,quit);
    */

	return 0;
}
