#include "lowterm.h"

#define BROWSER_NAME        "google-chrome"
#define IMAGEVIEWER_NAME    "display"

void Terminal_Copy(GtkWidget *widget, void *data) 	
{/*{{{*/
    vte_terminal_copy_clipboard((VteTerminal *)widget);
}/*}}}*/

void Terminal_Paste(GtkWidget *widget, void *data)
{/*{{{*/
    vte_terminal_paste_clipboard((VteTerminal *)widget);
}/*}}}*/

int  Terminal_Mouse_Click(GtkWidget *widget, GdkEventButton *event)
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
                Terminal_Copy(widget, NULL);
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
                else
                    sprintf(cmd_run, "%s '%s'&", BROWSER_NAME, url_match);

                system(cmd_run);
                free(cmd_run);
            }

            free(url_match);
            break;
        case 2:     // mouse wheel press
            break;
        case 3:     // right mouse button press
            Terminal_Paste(widget, NULL);
            break;
    }

    /* 꼭 FALSE를 적어야 터미널 select가 된다 */
    return FALSE;
}/*}}}*/

void Terminal_Exit(GtkWidget *g_widget, struct st_terminal *get_value)
{/*{{{*/
    gtk_widget_destroy(get_value -> object -> terminal);
    gtk_widget_destroy(get_value -> object -> window);

    /* key bind, malloc free 하는거 나중에 추가하셈 */
    //g_free(get_value);
    //gtk_main_quit();
}/*}}}*/
