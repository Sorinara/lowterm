#include "lowterm.h"

int main(int argc,char *argv[])
{
    LowTerm lowterm;
	int i;

	if(Config_Get(&lowterm) != 0){
        fprintf(stderr, "Total Terminal Count : %d\n", lowterm.terminal_count);
        return -1;
    }

    Stack_New(&lowterm.visible_list, lowterm.terminal_count);

	gtk_init(&argc, &argv);

    for(i = 0;i < lowterm.terminal_count; i++){
        /* fprintf(stderr, "Terminal Address : %p\n", &(lowterm.terminal[i])); */
        lowterm.terminal[i].visible_list_pointer  = &(lowterm.visible_list);
        Terminal_New(i, &(lowterm.terminal[i]));
        Terminal_Set(lowterm.terminal[i]);
        Terminal_Show_Hide(lowterm.terminal[i].window, &(lowterm.terminal[i]));
    }

    /* set focus main window */
    gtk_widget_grab_focus(lowterm.terminal[0].vte);
    gtk_window_present(GTK_WINDOW(lowterm.terminal[0].window));

	gtk_main();

	free(lowterm.terminal);
    Stack_Delete(lowterm.visible_list);

	return 0;
}
