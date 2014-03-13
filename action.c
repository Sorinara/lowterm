#include "lowterm.h"

#define  MOVE_UNIT_SIZE 25
#define  WINDOW_DELAY   8000

int Terminal_Window_Move(GtkWidget *window, int pos_x, int pos_y, char *status, char *direction)
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

int Terminal_Show_Hide(BindKey *object, struct st_terminal *term_head)
{/*{{{*/
    int     i=0,
            keycode;
    struct  st_terminal term;

    /* 제일 처음 시작할때는 main 함수에서 NULL 로 시작 */
    if(object == NULL){
        usleep(250000);
        /* 일단 터미널과 윈도우 위젯들을 보여준다 */
        gtk_widget_show_all(term_head->object->window);
        //gtk_widget_show(term_head->object->window);
        //gtk_widget_show(term_head->object->terminal);

        /* 감추게 FALSE 이면 감추게 한다 
         * 걍 gtk_widget_show를 쓰면 안되는 이유가 끊기기 때문에 따로 gtk_widget_hide를 써야 */
        if(term_head->config.show_hide == FALSE){
            //gtk_widget_hide(term_head->object->terminal);
            //gtk_widget_hide(term_head->object->window);
            gtk_widget_hide_all(term_head->object->window);
        }

        return 2;
    }

    if(object->onoff == OFF){
        gtk_window_set_default_size(GTK_WINDOW(object->window), object->mother->config.win_width, object->mother->config.win_height);
        Terminal_Window_Move(object->window,
                             object->mother->config.win_start_x, object->mother->config.win_start_y,
                             "Show", object->mother->config.ani_start_place);

        //gdk_flush();
        /* 이동시 바로 커서가 이동하게 하기 위해서 */
        gtk_widget_show(object->terminal);
        gtk_widget_show(object->window);

        gtk_window_present(GTK_WINDOW(object->window));
        //gtk_widget_grab_focus(object->terminal);
        gtk_widget_grab_focus(object->window);
        object->onoff = ON;
    }else{
        Terminal_Window_Move(object->window,
                             object->mother->config.win_start_x, object->mother->config.win_start_y,
                             "Hide", object->mother->config.ani_end_place);
        gtk_widget_hide(object->window);
        gtk_widget_hide(object->terminal);

        object->onoff = OFF;
    }

    return 0;
}/*}}}*/
