#include "lowterm.h"

void memory_pointer_dump(void *data, const int size)
{/*{{{*/
    int i;
    void **data_void;

    data_void = (void **)data;

    fprintf(stderr, "Dump Start Address   : %10p\n{\n     ", data);
    for(i = 0;i < size;i ++){
        fprintf(stderr, "%9p ", *(data_void + i));
        if((i % 16) == 15){
            fprintf(stderr, "\n");
        }
    }

    if(i < 15){
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "}\n");
}/*}}}*/

void terminal_mouse_copy(GtkWidget *widget) 	
{/*{{{*/
    vte_terminal_copy_clipboard((VteTerminal *)widget);
}/*}}}*/

void terminal_mouse_paste(GtkWidget *widget)
{/*{{{*/
    vte_terminal_paste_clipboard((VteTerminal *)widget);
}/*}}}*/

int  terminal_window_move(GtkWidget *window, int pos_x, int pos_y, char *status, char *direction)
{/*{{{*/
	GdkDisplay   *dpy;
	GdkWindow    *win;
	GdkMonitor   *monitor;
	GdkRectangle  geometry;
    int screen_width,
        screen_height;
    int i;

    if(direction == NULL){
        return -1;
    }

    if(status == NULL){
        return -1;
    }

    /* get screen size */
	dpy     = gtk_widget_get_display(window);
	win     = gtk_widget_get_window(window);
	monitor = gdk_display_get_monitor_at_window(dpy, win);

	gdk_monitor_get_geometry(monitor, &geometry);

	screen_width  = geometry.width;
	screen_height = geometry.height;

    //printf("%s %s\n", status, direction);
    if(strcmp(status, "Show") == 0){
        /* from down to up */
        if(strcmp(direction, "up") == 0){
            for(i = screen_height - pos_y;i >= pos_y; i-= MOVE_UNIT_SIZE){
                gtk_window_move(GTK_WINDOW(window), pos_x, i);
                gdk_flush();
            }
        }
        /* from up to down */
        else if(strcmp(direction, "down") == 0){
            for(i = 0;i <= pos_y; i+= MOVE_UNIT_SIZE){
                gtk_window_move(GTK_WINDOW(window), pos_x, i);
                gdk_flush();
            }
        }
        /* from right to left */
        else if(strcmp(direction, "left") == 0){
            for(i = screen_width - pos_x;i >= pos_x;i -= MOVE_UNIT_SIZE){
                gtk_window_move(GTK_WINDOW(window), i, pos_y);
                gdk_flush();
            }
        }
        /* from right to left */
        else if(strcmp(direction, "right") == 0){
            for(i=0;i<=pos_x;i += MOVE_UNIT_SIZE){
                gtk_window_move(GTK_WINDOW(window), i, pos_y);
                gdk_flush();
            }
        }

        /* 원위치로 이동 */
        if(pos_x >= 0 && pos_y >= 0){
            gtk_window_move(GTK_WINDOW(window), pos_x, pos_y);
        }    
    }else if(strcmp(status, "Hide") == 0){
        /* from down to up */
        if(strcmp(direction, "up") == 0){
            for(i = pos_y;i >= 0; i-= MOVE_UNIT_SIZE){
                gtk_window_move(GTK_WINDOW(window), pos_x, i);
                gdk_flush();
                usleep(WINDOW_DELAY);
            }
        }
        /* from up to down */
        else if(strcmp(direction, "down") == 0){
            for(i = pos_y;i <= screen_height;i += MOVE_UNIT_SIZE){
                gtk_window_move(GTK_WINDOW(window), pos_x, i);
                gdk_flush();
                usleep(WINDOW_DELAY);
            }
        }
        /* from right to left */
        else if(strcmp(direction, "left") == 0){
            for(i=pos_x;i>=0;i -= MOVE_UNIT_SIZE){
                gtk_window_move(GTK_WINDOW(window), i, pos_y);
                gdk_flush();
                usleep(WINDOW_DELAY);
            }
        }
        /* from right to left */
        else if(strcmp(direction, "right") == 0){
            for(i=pos_x;i<=screen_width;i += MOVE_UNIT_SIZE){
                gtk_window_move(GTK_WINDOW(window), i, pos_y);
                gdk_flush();
                usleep(WINDOW_DELAY);
            }
        }
    }

    gtk_window_move(GTK_WINDOW(window), pos_x, pos_y);

    return 0;
}/*}}}*/

int  Terminal_Mouse(GtkWidget *widget, GdkEventButton *event)
{/*{{{*/
    int column,
        row,
        tag;
    char *url_match,
         *cmd_run;

    /* fprintf(stderr, "Event - : %d\n", event->button); */
    /* fprintf(stderr, "1P: %d,2P: %d, 3P: %d\n", GDK_BUTTON_PRESS, GDK_2BUTTON_PRESS, GDK_3BUTTON_PRESS); */
    /* fprintf(stderr, "%d \n", GDK_BUTTON_RELEASE); */

    switch(event->button){
        case 1: // left mouse button press 
            column     = event->x / vte_terminal_get_char_width(VTE_TERMINAL(widget));
            row        = event->y / vte_terminal_get_char_height(VTE_TERMINAL(widget));
            url_match  = vte_terminal_match_check((VteTerminal *)widget, column, row, &tag);

            if(url_match != NULL){
                if((cmd_run = (char *)calloc(strlen(url_match) + strlen(BROWSER_NAME) + 5, sizeof(char))) == NULL){
                    perror("Memroy Alloc Failed");
                    exit(1);
                }

                if(!(strcasecmp(url_match + strlen(url_match) - 3, "jpg"))  || \
                   !(strcasecmp(url_match + strlen(url_match) - 4, "jpeg")) || \
                   !(strcasecmp(url_match + strlen(url_match) - 3, "gif"))  || \
                   !(strcasecmp(url_match + strlen(url_match) - 3, "png"))  ){
                    sprintf(cmd_run, "%s '%s'&", IMAGEVIEWER_NAME, url_match);
                }
                else{
                    sprintf(cmd_run, "%s '%s'&", BROWSER_NAME, url_match);
                }

                system(cmd_run);
                free(cmd_run);
            }else{
                terminal_mouse_copy(widget);
            }

            free(url_match);
            break;
        case 2:
            break;
        case 3:
            terminal_mouse_paste(widget);
            break;
    }

    /* 꼭 FALSE를 적어야 터미널 select가 된다 */
    return FALSE;
}/*}}}*/

void Terminal_Show_Hide(GtkWidget *widget, gpointer user_data_param)
{/*{{{*/
    Terminal *terminal,
             *terminal_stack_last = NULL;

    terminal = (Terminal *)user_data_param;

    /* fprintf(stderr, "Terminal Changed : %d (%s)\n", terminal->id, terminal->visible ? "TRUE" : "FALSE"); */
    if(terminal->visible == TRUE){
        terminal_window_move(terminal->window, terminal->config.win_pos_x, terminal->config.win_pos_y,
                             "Show", terminal->config.win_animation_move_start);

        /* Hidden -> Show */
        gtk_widget_show_all(terminal->window);
        gtk_window_present(GTK_WINDOW(terminal->window));

        if(terminal->config.win_focus== 1){
            Stack_Push(terminal->visible_list_pointer, terminal);
            /* Stack_Print(*(terminal->visible_list_pointer)); */
        }

        terminal->visible = FALSE;
    }else{
        terminal_window_move(terminal->window, terminal->config.win_pos_x, terminal->config.win_pos_y,
                             "Hide", terminal->config.win_animation_move_end);

        /* Show -> Hidden */
        gtk_widget_hide(terminal->window);

        if(terminal->config.win_focus == 1){
            /* Restore before show window */
            Stack_Clear(terminal->visible_list_pointer, terminal);
            Stack_Last(terminal->visible_list_pointer, (void **)&terminal_stack_last);
            /* 비지블스택의 마지막(last)를 최상위 윈도우로 두면, Alt+Tab으로 창 이동이 안된다. */
            /* gtk_window_present(GTK_WINDOW(terminal_stack_last->window)); */
            /* Stack_Print(*(terminal->visible_list_pointer)); */
        }

        terminal->visible = TRUE;
    }
}/*}}}*/

/* if get focus from another window, do not run this callback */
gboolean Terminal_Focus_In(GtkWidget *widget, GdkEvent *event, gpointer user_data_param)
{/*{{{*/
    Terminal *terminal,
             *terminal_stack_first,
             *terminal_stack_last = NULL;

    if((terminal = (Terminal *)user_data_param) == NULL){
        return FALSE;
    }

    if(terminal->config.win_focus != 1){
        return FALSE;
    }

    /* Show Last Window */
    Stack_Get(terminal->visible_list_pointer, 0, (void **)&terminal_stack_first);
    Stack_Last(terminal->visible_list_pointer, (void **)&terminal_stack_last);

    if(terminal_stack_first == NULL){
        return FALSE;
    }

    if(terminal_stack_last == NULL){
        return FALSE;
    }

    if(widget != terminal_stack_first->window){
        return FALSE;
    }

    /* 포커스를 떠난 터미널과 들어온 터미널이 다르면
     * 떠난 터미널(stack의 last)과 같게 맞춰 준다 
     * TODO: 이렇게 하지말고, 터미널 전체의 포커스를 조사해  */
    if(widget != terminal_stack_last->vte){
        /* gtk_widget_grab_focus(terminal_stack_first->vte); */
        /* gtk_window_set_focus(GTK_WINDOW(terminal_stack_first->window),  terminal_stack_first->vte); */
        /* must be use once gtk_window_present() */
        gtk_window_present(GTK_WINDOW(terminal_stack_last->window));
        /* fprintf(stderr, "2: TP focus in  : %d,%d\n", gtk_widget_is_focus(widget), gtk_widget_is_focus(terminal_stack_last->vte)); */
    }else{
        /* fprintf(stderr, "2: OP focus in  : %d,%d\n", gtk_widget_is_focus(widget), gtk_widget_is_focus(terminal_stack_last->vte)); */
    }

    return FALSE;
}/*}}}*/

gboolean Terminal_Focus_Out(GtkWidget *widget, GdkEvent *event, gpointer user_data_param)
{/*{{{*/
    Terminal *terminal,
             *terminal_stack_last;

    if((terminal = (Terminal *)user_data_param) == NULL){
        return FALSE;
    }

    Stack_Last(terminal->visible_list_pointer, (void **)&terminal_stack_last);
    /* fprintf(stderr, "3: focus Out : %d %d\n\n", gtk_widget_is_focus(widget), gtk_widget_is_focus(terminal_stack_last->vte)); */

    return FALSE;
}/*}}}*/

void Terminal_Exit(GtkWidget *widget, gpointer user_data_param)
{/*{{{*/
    /* Terminal *terminal; */

    /* terminal = (Terminal *)user_data_param; */

    /* crash! gtk3.0 */
    /* gtk_widget_destroy(terminal->vte); */
    /* gtk_widget_destroy(terminal->window); */

    /* printf("[Terminal %d] Exit Code : %p\n", terminal->id, terminal->exit_code); */
    /* key bind, malloc free 하는거 나중에 추가하셈 */
    //g_free(terminal);
    gtk_main_quit();
}/*}}}*/
