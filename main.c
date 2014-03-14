#include "lowterm.h"

int main(int argc,char *argv[])
{
    static Terminal *terminal;
	int terminal_number,
        i;

	if((terminal_number = Config_Get(&terminal)) <= 0){
        fprintf(stderr, "Total Terminal Count : %d\n", terminal_number);
        return -1;
    }

	gtk_init(&argc, &argv);

    for(i = 0;i < terminal_number; i++){
        Terminal_New(i, &(terminal[i]));
        Terminal_Set(terminal[i]);
        Terminal_Show_Hide(terminal[i].window, &(terminal[i]));
    }

    /* set focus main window */
    gtk_widget_grab_focus(terminal[0].vte);
    gtk_window_present(GTK_WINDOW(terminal[0].window));

	gtk_main();

	free(terminal);

	return 0;
}
