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
    GdkScreen *gscr;
    int scr_size_x,
        scr_size_y;
    int i;

    if(direction == NULL){
        return -1;
    }

    if(status == NULL){
        return -1;
    }

    /* get screen size */
    gscr        = gdk_screen_get_default();
    scr_size_x  = gdk_screen_get_width(gscr);
    scr_size_y  = gdk_screen_get_height(gscr);

    //printf("%s %s\n", status, direction);
    if(strcmp(status, "Show") == 0){
        /* from down to up */
        /* if(strcmp(direction, "up") == 0){ */
        /*     for(i = scr_size_y - pos_y;i >= pos_y; i-= MOVE_UNIT_SIZE){ */
        /*         gtk_window_move(GTK_WINDOW(window), pos_x, i); */
        /*         gdk_flush(); */
        /*     } */
        /* } */
        /* from up to down */
        /* else if(strcmp(direction, "down") == 0){ */
        /*     for(i = 0;i <= pos_y; i+= MOVE_UNIT_SIZE){ */
        /*         gtk_window_move(GTK_WINDOW(window), pos_x, i); */
        /*         gdk_flush(); */
        /*     } */
        /* } */
        /* from right to left */
        /* else if(strcmp(direction, "left") == 0){ */
        /*     for(i = scr_size_x - pos_x;i >= pos_x;i -= MOVE_UNIT_SIZE){ */
        /*         gtk_window_move(GTK_WINDOW(window), i, pos_y); */
        /*         gdk_flush(); */
        /*     } */
        /* } */
        /* from right to left */
        /* else if(strcmp(direction, "right") == 0){ */
        /*     for(i=0;i<=pos_x;i += MOVE_UNIT_SIZE){ */
        /*         gtk_window_move(GTK_WINDOW(window), i, pos_y); */
        /*         gdk_flush(); */
        /*     } */
        /* } */

        /* 원위치로 이동 */
        gtk_window_move(GTK_WINDOW(window), pos_x, pos_y);
    }else if(strcmp(status, "Hide") == 0){
        /* from down to up */
        /* if(strcmp(direction, "up") == 0){ */
        /*     for(i = pos_y;i >= 0; i-= MOVE_UNIT_SIZE){ */
        /*         gtk_window_move(GTK_WINDOW(window), pos_x, i); */
        /*         gdk_flush(); */
        /*         usleep(WINDOW_DELAY); */
        /*     } */
        /* } */
        /* from up to down */
        /* else if(strcmp(direction, "down") == 0){ */
        /*     for(i = pos_y;i <= scr_size_y;i += MOVE_UNIT_SIZE){ */
        /*         gtk_window_move(GTK_WINDOW(window), pos_x, i); */
        /*         gdk_flush(); */
        /*         usleep(WINDOW_DELAY); */
        /*     } */
        /* } */
        /* from right to left */
        /* else if(strcmp(direction, "left") == 0){ */
        /*     for(i=pos_x;i>=0;i -= MOVE_UNIT_SIZE){ */
        /*         gtk_window_move(GTK_WINDOW(window), i, pos_y); */
        /*         gdk_flush(); */
        /*         usleep(WINDOW_DELAY); */
        /*     } */
        /* } */
        /* from right to left */
        /* else if(strcmp(direction, "right") == 0){ */
        /*     for(i=pos_x;i<=scr_size_x;i += MOVE_UNIT_SIZE){ */
        /*         gtk_window_move(GTK_WINDOW(window), i, pos_y); */
        /*         gdk_flush(); */
        /*         usleep(WINDOW_DELAY); */
        /*     } */
        /* } */
    }

    return 0;
}/*}}}*/

int  Terminal_Mouse(GtkWidget *widget, GdkEventButton *event)
{/*{{{*/
    int terminal_x,
        terminal_y,
        tag;
    char *url_match,
         *cmd_run;
    GtkBorder *style_border;

    switch(event->button){
        case 1:     // left mouse button press
            /* vte_terminal_get_padding((VteTerminal *)widget, &terminal_x, &terminal_y); */
            /* printf("OX: %d OY: %d\n", terminal_x, terminal_y); */
            gtk_widget_style_get(widget, "inner-border", &style_border, NULL);
            terminal_x = style_border->left + style_border->right;
            terminal_y = style_border->top  + style_border->bottom;
            /* printf("NX: %d NY: %d\n", terminal_x, terminal_y); */
            url_match = vte_terminal_match_check((VteTerminal *)widget,
                    (event->x - terminal_y) /((VteTerminal *)widget)->char_width,
                    (event->y - terminal_y) /((VteTerminal *)widget)->char_height,
                    &tag);

            if(url_match == NULL)
                terminal_mouse_copy(widget);
            else{
                if((cmd_run = (char *)calloc(strlen(url_match) + strlen(BROWSER_NAME) + 5, sizeof(char))) == NULL){
                    perror("Memroy Alloc Failed");
                    exit(1);
                }

                if(!(strcasecmp(url_match + strlen(url_match) - 3, "jpg"))  || \
                   !(strcasecmp(url_match + strlen(url_match) - 4, "jpeg")) || \
                   !(strcasecmp(url_match + strlen(url_match) - 3, "gif"))  || \
                   !(strcasecmp(url_match + strlen(url_match) - 3, "png"))     ){
                    sprintf(cmd_run, "%s '%s'&", IMAGEVIEWER_NAME, url_match);
                }
                else{
                    sprintf(cmd_run, "%s '%s'&", BROWSER_NAME, url_match);
                }

                system(cmd_run);
                free(cmd_run);
            }

            free(url_match);
            break;
        case 2:     // mouse wheel press
            break;
        case 3:     // right mouse button press
            terminal_mouse_paste(widget);
            break;
    }

    /* 꼭 FALSE를 적어야 터미널 select가 된다 */
    return FALSE;
}/*}}}*/

void Terminal_Show_Hide(GtkWidget *nouse, gpointer user_data_param)
{/*{{{*/
    Terminal *terminal,
             *terminal_focus = NULL;

    terminal = (Terminal *)user_data_param;

    if(terminal->visible == TRUE){
        gtk_window_set_default_size(GTK_WINDOW(terminal->window), terminal->config.win_width, terminal->config.win_height);
        terminal_window_move(terminal->window, terminal->config.win_start_x, terminal->config.win_start_y,
                             "Show", terminal->config.ani_start_place);

        gtk_widget_show_all(terminal->window);
        gtk_window_present(GTK_WINDOW(terminal->window));
        if(terminal->config.accept_focus == 1){
            Stack_Push(terminal->visible_list_pointer, terminal);
        }

        terminal->visible = FALSE;
    }else{
        terminal_window_move(terminal->window, terminal->config.win_start_x, terminal->config.win_start_y,
                             "Hide", terminal->config.ani_end_place);

        gtk_widget_hide_all(terminal->window);

        /* now terminal address clear, last terminal window pop */
        Stack_Clear(terminal->visible_list_pointer, terminal);
        Stack_Last(terminal->visible_list_pointer, &terminal_focus);
        /* fprintf(stderr, "Pop Get Address : %p\n", terminal_focus); */

        if(terminal_focus != NULL){
            gtk_window_present(GTK_WINDOW(terminal_focus->window));
        }

        terminal->visible = TRUE;
    }
}/*}}}*/

void Terminal_Focus(GtkWidget *nouse, GdkEvent *event, gpointer user_data_param)
{/*{{{*/
    Terminal *terminal,
             *terminal_focus = NULL;

    terminal = (Terminal *)user_data_param;

    Stack_Last(terminal->visible_list_pointer, &terminal_focus);

    gtk_window_present(GTK_WINDOW(terminal_focus->window));
}/*}}}*/

void Terminal_Exit(GtkWidget *widget, gpointer user_data_param)
{/*{{{*/
    Terminal *terminal;

    terminal = (Terminal *)user_data_param;

    gtk_widget_destroy(terminal->vte);
    gtk_widget_destroy(terminal->window);

    /* key bind, malloc free 하는거 나중에 추가하셈 */
    //g_free(terminal);
    //gtk_main_quit();
}/*}}}*/
