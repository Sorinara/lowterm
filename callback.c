#include "lowterm.h"
//#define DEBUG

//#define BROWSER_NAME "firefox"
#define BROWSER_NAME "google-chrome"
#define IMAGEVIEWER_NAME "display"

int Get_Keycode(char *key)
{
#ifdef DEBUG
	puts("Get_Keycode Func start");
#endif
	int key_code=0;
    KeySym  key_sym;
	
	/*예외처리 할것*/
	key_sym = XStringToKeysym(key);
	key_code = XKeysymToKeycode(GDK_DISPLAY(), key_sym);
#ifdef DEBUG
	puts("Get_Keycode Func End");
    printf("Keycode is %d\n, character : %s,%p\n",key_code,key,key_sym);
#endif
	return key_code;
}

void copy_clipboard(GtkWidget *widget,void *data) 	
{
    vte_terminal_copy_clipboard((VteTerminal *)widget);
}

void paste_clipboard(GtkWidget *widget,void *data)
{
    vte_terminal_paste_clipboard((VteTerminal *)widget);
}

    void
event_mouse_drag (GtkWidget *widget, 
                  GdkDragContext *context, 
                  int x,
                  int y,
                  GtkSelectionData *selection_data,
                  unsigned int info,
                  guint32 time,
                  gpointer data)
{
    if(selection_data->length > 0){
        puts("ping!!");
    }
    return ;
}		/* -----  end of function event_mouse_drag  ----- */

int event_mouse_press(GtkWidget *widget,GdkEventButton *event)
{
    int terminal_x,
        terminal_y,
        tag;
    char *url_match,
         *cmd_run;

    switch(event->button){
        case 1:     // left mouse button press
            vte_terminal_get_padding ((VteTerminal *)widget, &terminal_x, &terminal_y);
            url_match = vte_terminal_match_check ((VteTerminal *)widget,
                    (event->x - terminal_y) / ((VteTerminal *)widget)->char_width,
                    (event->y - terminal_y) / ((VteTerminal *)widget)->char_height,
                    &tag);

            if(url_match == NULL)
                copy_clipboard(widget,NULL);
            else{
                if((cmd_run = (char *)calloc(strlen(url_match) + strlen(BROWSER_NAME) + 5,sizeof(char))) == NULL){
                        perror("Memroy Alloc Failed");
                        exit(1);
                }

                if(!(strcasecmp(url_match + strlen(url_match) - 3,"jpg"))  || \
                   !(strcasecmp(url_match + strlen(url_match) - 4,"jpeg")) || \
                   !(strcasecmp(url_match + strlen(url_match) - 3,"gif"))  || \
                   !(strcasecmp(url_match + strlen(url_match) - 3,"png"))     ){
                    sprintf(cmd_run,"%s '%s'&",IMAGEVIEWER_NAME,url_match);
                }
                else
                    sprintf(cmd_run,"%s '%s'&",BROWSER_NAME,url_match);

                system(cmd_run);
                free(cmd_run);
            }

            free(url_match);
            break;
        case 2:     // mouse wheel press
            break;
        case 3:     // right mouse button press
            paste_clipboard(widget,NULL);
            break;
    }

    /* 꼭 FALSE를 적어야 터미널 select가 된다 */
    return FALSE;
}

void window_manager(GtkWidget *widget,void *data)
{
    //puts("window manager test");
}

void App_Quit(GtkWidget *g_widget,struct st_terminal *get_value)
{
    gtk_widget_destroy(get_value -> object -> terminal);
    gtk_widget_destroy(get_value -> object -> window);

    /* key bind, malloc free 하는거 나중에 추가하셈 */
    //g_free(get_value);
    //gtk_main_quit();
}
