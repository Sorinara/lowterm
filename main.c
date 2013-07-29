#include "lowterm.h"

int main(int argc,char *argv[])
{
    static struct st_terminal *terminal;
	int i,terminal_number;

	/* 설정을 받아오는 함수 st_terminal 을 채워 온다 
     * 그리고 전체 터미널의 갯수를 리턴한다 */
	terminal_number = Get_Config(&terminal);
    //printf("%d\n", terminal_number);

	gtk_init(&argc,&argv);

    for(i=0;i<terminal_number;i++){
        terminal[i].object = Create_Terminal_Widget(&terminal[i]);
        terminal[i].object->mother =  &terminal[i];
        Set_Config(terminal[i]);
        Terminal_Show_Hide(NULL,&terminal[i]);
        /* 난 이렇게 하지 싫었지만... */
        //g_signal_connect(terminal[i].object->terminal,"hide",G_CALLBACK(window_manager),(void *)i);
    }

    /* 첫번째(main) 윈도우로 포커스를 맞춰준다 */
    gtk_widget_grab_focus(terminal[0].object->terminal);
    gtk_window_present(GTK_WINDOW (terminal[0].object->window));

	gtk_main();

	free(terminal);

	return 0;
}
