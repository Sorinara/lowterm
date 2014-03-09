#include "lowterm.h"

int main(int argc,char *argv[])
{
    static struct st_terminal *terminal;
	int i,
        terminal_number;

	/* get config file data */
	if((terminal_number = Config_Get(&terminal)) <= 0){
        //fprintf(stderr, "Total Terminal Count : %d\n", terminal_number);
        return -1;
    }

	gtk_init(&argc,&argv);

    for(i = 0;i < terminal_number; i++){
        terminal[i].object          = Terminal_Widget_Create(&terminal[i]);
        terminal[i].object->mother  = &terminal[i];
        Terminal_Set(terminal[i]);
        Terminal_Show_Hide(NULL, &terminal[i]);
    }

    /* set focus main window */
    gtk_widget_grab_focus(terminal[0].object->terminal);
    gtk_window_present(GTK_WINDOW(terminal[0].object->window));

	gtk_main();

	free(terminal);

	return 0;
}
