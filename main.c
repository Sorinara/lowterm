#include "lowterm.h"

int main(int argc,char *argv[])
{
    LowTerm lowterm;
	int i;

	if(Config_Get(argc, argv, &lowterm) != 0){
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

    // 초기 윈도우 크기(width/height), 윈도우명, 실행명령을 파라미터로 받을수 있도록 한다.
    // urxvt를 밀어버리고 이걸로 쓸 예정, urxvt는 한글입력이 뭐같아서 싫어
	gtk_main();

	free(lowterm.terminal);
    Stack_Delete(&(lowterm.visible_list));

	return 0;
}
