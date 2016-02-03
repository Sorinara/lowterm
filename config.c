#include "lowterm.h"
#include <malloc.h>

int Config_Get(int argc, char *argv[], LowTerm *lowterm)
{
	char  config_file[CONFIG_FILE_PATH_MAX] = {0},
          *optarg_c                         = NULL,
	      *home_dirtory;
	cfg_t *cfg,
          *cfg_terminal;
	int   c,
          i,
          table_count;
    Terminal *terminal;

    while((c = getopt(argc, argv, "c:")) != -1){
        switch(c){
            case 'c':
                optarg_c = optarg;
                goto EXIT_OPTION;
        }
    }EXIT_OPTION:;

    if(optarg_c != NULL){
        memcpy(config_file, optarg_c, strlen(optarg_c));
    }else{
        home_dirtory = getenv("HOME");

        if((strlen(home_dirtory) + strlen(CONFIG_FILE_NAME)) > CONFIG_FILE_PATH_MAX){
            fprintf(stderr,"파일 이름이 너무 깁니다");
            return -1;
        }

        // 설정파일을 능동적?으로 읽게하기
        sprintf(config_file, "%s/"CONFIG_FILE_NAME, home_dirtory);
    }

//#ifdef DEBUG
    fprintf(stderr, "file_name: %s\n", config_file);
//#endif

	cfg_opt_t terminal_opts[]={		//장기적으로 파싱 함수 내장할것/*{{{*/
		CFG_STR("name", 				    "temp", 	CFGF_NONE),
		CFG_STR("execute", 				    "/bin/sh", 	CFGF_NONE),
		CFG_STR("bind_key", 				"", 		CFGF_NONE),
		CFG_STR("bind_key_mask", 		    "", 		CFGF_NONE),
		CFG_BOOL("win_visible",				TRUE,		CFGF_NONE),
		CFG_INT("win_pos_x", 				0, 			CFGF_NONE),
		CFG_INT("win_pos_y", 				0, 			CFGF_NONE),
		CFG_INT("win_height", 				500, 		CFGF_NONE), /*1021이하여야함*/
		CFG_INT("win_width", 				500, 		CFGF_NONE),
		CFG_INT("win_layer", 				0, 			CFGF_NONE),
		CFG_BOOL("win_focus",				TRUE,		CFGF_NONE),
		CFG_BOOL("win_show_pager",			FALSE,		CFGF_NONE),
		CFG_BOOL("win_show_taskbar", 		FALSE, 		CFGF_NONE),
		CFG_BOOL("win_show_all_workspace", 	TRUE, 		CFGF_NONE),
        CFG_INT("win_move_sleep_sec",       5000,       CFGF_NONE),
        CFG_STR("win_move_start",           "left",     CFGF_NONE),
        CFG_STR("win_move_end",             "left",     CFGF_NONE),
		CFG_STR("term_font", 				"", 		CFGF_NONE),
		CFG_BOOL("term_font_bold", 			FALSE, 		CFGF_NONE),
		CFG_BOOL("term_antialias", 			FALSE, 		CFGF_NONE),
		CFG_STR("term_locale", 				"",  		CFGF_NONE),
		CFG_BOOL("term_blink_curser", 		TRUE, 		CFGF_NONE),
		CFG_BOOL("term_double_buffer", 		TRUE, 		CFGF_NONE),
		CFG_BOOL("term_audio_bell", 		TRUE, 		CFGF_NONE),
		CFG_STR("term_image", 			    "",         CFGF_NONE),
		CFG_FLOAT("term_transparency", 	    0, 	        CFGF_NONE),
		CFG_INT("term_backcolor_red", 		0x0000, 	CFGF_NONE),
		CFG_INT("term_backcolor_green", 	0x0000, 	CFGF_NONE),
		CFG_INT("term_backcolor_blue", 		0x0000, 	CFGF_NONE),
		CFG_INT("term_textcolor_red", 		0xffff, 	CFGF_NONE),
		CFG_INT("term_textcolor_green", 	0xffff, 	CFGF_NONE),
		CFG_INT("term_textcolor_blue", 		0xffff, 	CFGF_NONE),
		CFG_END()
	};/*}}}*/

	cfg_opt_t opts[] = {
		CFG_SEC(CONFIG_TITLE, terminal_opts, CFGF_TITLE | CFGF_MULTI),
		CFG_END()
	};

	cfg = cfg_init(opts, 0);
	
	if(cfg_parse(cfg, config_file) == CFG_PARSE_ERROR){
		fprintf(stderr, "기본적인 내용이 %s 파일에 저장되었습니다!", config_file);
		return -2;
	}

	/* table_count 가지고 calloc 하기 */
	table_count = cfg_size(cfg, CONFIG_TITLE);

	if((lowterm->terminal = (Terminal *)calloc(table_count, sizeof(Terminal))) == NULL){
		perror("Memory Allocation Failed");
		return -3;
	}

	for(i = 0; i < table_count; i++){/*{{{*/
        terminal = lowterm->terminal + i;
        /* printf("XX: %p %d/%d +%x\n", terminal, i, table_count, sizeof(Terminal) * i); */
		if((cfg_terminal = cfg_getnsec(cfg, CONFIG_TITLE, i)) == NULL){
            return -4;
        }
#ifdef DEBUG
		fprintf(stderr, "session title %s\n", cfg_title(cfg_terminal));
#endif
        terminal->id           			    = i;

        /* STRING  문자열 증발 방지 위해 strdup함수 사용 */
        terminal->config.name 			        = strdup(cfg_getstr(cfg_terminal,"name"));
        terminal->config.execute                = strdup(cfg_getstr(cfg_terminal,"command"));
        terminal->config.bind_key 		        = strdup(cfg_getstr(cfg_terminal,"bind_key"));
        terminal->config.bind_key_mask          = strdup(cfg_getstr(cfg_terminal,"bind_key_mask"));
        terminal->config.win_move_start         = strdup(cfg_getstr(cfg_terminal,"win_move_start"));
        terminal->config.win_move_end           = strdup(cfg_getstr(cfg_terminal,"win_move_end"));
        terminal->config.term_font 			    = strdup(cfg_getstr(cfg_terminal,"term_font"));
        terminal->config.term_locale            = strdup(cfg_getstr(cfg_terminal,"term_locale"));
        terminal->config.term_image             = strdup(cfg_getstr(cfg_terminal,"term_image"));
		
		/* FLOAT */	
		terminal->config.term_transparency 	    = cfg_getfloat(cfg_terminal,"term_transparency");

		/* INTEGER */
		terminal->config.win_pos_x 		        = cfg_getint(cfg_terminal,"win_pos_x"); 
		terminal->config.win_pos_y 		        = cfg_getint(cfg_terminal,"win_pos_y");
		terminal->config.win_height 		    = cfg_getint(cfg_terminal,"win_height");
		terminal->config.win_width 		        = cfg_getint(cfg_terminal,"win_width");
		terminal->config.win_layer 			    = cfg_getint(cfg_terminal,"win_layer");
        terminal->config.win_move_sleep_sec     = cfg_getint(cfg_terminal,"win_move_sleep_sec");
		terminal->config.term_backcolor_red 	= cfg_getint(cfg_terminal,"term_backcolor_red");
		terminal->config.term_backcolor_blue 	= cfg_getint(cfg_terminal,"term_backcolor_blue");
		terminal->config.term_backcolor_green 	= cfg_getint(cfg_terminal,"term_backcolor_green");
		terminal->config.term_textcolor_red 	= cfg_getint(cfg_terminal,"term_textcolor_red");
		terminal->config.term_textcolor_green 	= cfg_getint(cfg_terminal,"term_textcolor_green");
		terminal->config.term_textcolor_blue 	= cfg_getint(cfg_terminal,"term_textcolor_blue");
		
		/* BOOL */
		terminal->config.win_visible            = cfg_getbool(cfg_terminal,"win_visible");
		terminal->config.win_focus              = cfg_getbool(cfg_terminal,"win_focus");
		terminal->config.win_show_pager		    = cfg_getbool(cfg_terminal,"win_show_pager");
		terminal->config.win_show_taskbar       = cfg_getbool(cfg_terminal,"win_show_taskbar");
		terminal->config.win_show_all_workspace = cfg_getbool(cfg_terminal,"win_show_all_workspace");
		terminal->config.term_font_bold 	    = cfg_getbool(cfg_terminal,"term_font_bold");
		terminal->config.term_antialias 	    = cfg_getbool(cfg_terminal,"term_antialias");
		terminal->config.term_blink_curser 	    = cfg_getbool(cfg_terminal,"term_blink_curser");
		terminal->config.term_double_buffer     = cfg_getbool(cfg_terminal,"term_double_buffer");
		terminal->config.term_audio_bell 	    = cfg_getbool(cfg_terminal,"term_audio_bell");
	} /*}}}*/

    lowterm->terminal_count = table_count;

	cfg_free(cfg);

	return 0;
}
