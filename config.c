#include "lowterm.h"
#include <malloc.h>

int Config_Get(LowTerm *lowterm)
{
	char  config_file[CONFIG_FILE_PATH_MAX],
	      *home_dirtory;
	cfg_t *cfg,
          *cfg_terminal;
	int   i,
          table_count;
    Terminal *terminal;

	cfg_opt_t terminal_opts[]={		//장기적으로 파싱 함수 내장할것/*{{{*/
		CFG_STR("session_name", 				"temp", 	CFGF_NONE),
		CFG_STR("screen_options", 				"", 		CFGF_NONE),
		CFG_STR("binding_key", 					"", 		CFGF_NONE),
		CFG_STR("binding_key_mask", 		    "", 		CFGF_NONE),
        CFG_STR("ani_start_place",              "left",     CFGF_NONE),
        CFG_STR("ani_end_place",                "left",     CFGF_NONE),
		CFG_INT("start_x", 						0, 			CFGF_NONE),
		CFG_INT("start_y", 						0, 			CFGF_NONE),
		CFG_INT("height", 						500, 		CFGF_NONE), /*1021이하여야함*/
		CFG_INT("width", 						500, 		CFGF_NONE),
		CFG_STR("font", 						"", 		CFGF_NONE),
		CFG_BOOL("font_bold", 					FALSE, 		CFGF_NONE),
		CFG_STR("locale", 						"",  		CFGF_NONE),
		CFG_STR("background_image", 			"", 		CFGF_NONE),
		CFG_FLOAT("background_transparency", 	0, 			CFGF_NONE),
		CFG_INT("background_color_red", 		0x0000, 	CFGF_NONE),
		CFG_INT("background_color_green", 		0x0000, 	CFGF_NONE),
		CFG_INT("background_color_blue", 		0x0000, 	CFGF_NONE),
		CFG_INT("text_color_red", 				0xffff, 	CFGF_NONE),
		CFG_INT("text_color_green", 			0xffff, 	CFGF_NONE),
		CFG_INT("text_color_blue", 				0xffff, 	CFGF_NONE),
        CFG_INT("ani_sleep_sec",                5000,       CFGF_NONE),
		CFG_BOOL("blink_curser", 				TRUE, 		CFGF_NONE),
		CFG_INT("layer", 						0, 			CFGF_NONE),
		CFG_BOOL("taskbar_view", 				FALSE, 		CFGF_NONE),
		CFG_BOOL("all_workspace_view", 			TRUE, 		CFGF_NONE),
		CFG_BOOL("audio_bell", 					TRUE, 		CFGF_NONE),
		CFG_BOOL("antialias", 					FALSE, 		CFGF_NONE),
		CFG_BOOL("double_buffer", 				TRUE, 		CFGF_NONE),
		CFG_BOOL("accept_focus",				TRUE,		CFGF_NONE),
		CFG_BOOL("skip_pager",					FALSE,		CFGF_NONE),
		CFG_BOOL("show_hide",					TRUE,		CFGF_NONE),
		CFG_END()
	};/*}}}*/

	cfg_opt_t opts[] = {
		CFG_SEC(CONFIG_TITLE, terminal_opts, CFGF_TITLE | CFGF_MULTI),
		CFG_END()
	};

	memset(config_file, 0, sizeof(config_file));
	home_dirtory = getenv("HOME");

    if((strlen(home_dirtory) + strlen(CONFIG_FILE_NAME)) > CONFIG_FILE_PATH_MAX){
        fprintf(stderr,"파일 이름이 너무 깁니다");
        return -1;
    }

	sprintf(config_file, "%s/"CONFIG_FILE_NAME, home_dirtory);
#ifdef DEBUG
	sprintf(stderr, "file_name: %s\n", config_file);
#endif
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
        terminal->config.name 			    = strdup(cfg_getstr(cfg_terminal,"session_name"));
        terminal->config.scr_opt            = strdup(cfg_getstr(cfg_terminal,"screen_options"));
        terminal->config.font 			    = strdup(cfg_getstr(cfg_terminal,"font"));
        terminal->config.bd_key 		    = strdup(cfg_getstr(cfg_terminal,"binding_key"));
        terminal->config.bd_key_mask        = strdup(cfg_getstr(cfg_terminal,"binding_key_mask"));
        terminal->config.locale             = strdup(cfg_getstr(cfg_terminal,"locale"));
        terminal->config.bg_image           = strdup(cfg_getstr(cfg_terminal,"background_image"));
        terminal->config.ani_start_place    = strdup(cfg_getstr(cfg_terminal,"ani_start_place"));
        terminal->config.ani_end_place      = strdup(cfg_getstr(cfg_terminal,"ani_end_place"));
		
		/* FLOAT */	
		terminal->config.bg_transparency 	= cfg_getfloat(cfg_terminal,"background_transparency");

		/* INTEGER */
		terminal->config.win_start_x 		= cfg_getint(cfg_terminal,"start_x"); 
		terminal->config.win_start_y 		= cfg_getint(cfg_terminal,"start_y");
		terminal->config.win_height 		= cfg_getint(cfg_terminal,"height");
		terminal->config.win_width 		    = cfg_getint(cfg_terminal,"width");
		terminal->config.tx_red 			= cfg_getint(cfg_terminal,"text_color_red");
		terminal->config.tx_green 		    = cfg_getint(cfg_terminal,"text_color_green");
		terminal->config.tx_blue 			= cfg_getint(cfg_terminal,"text_color_blue");
		terminal->config.bg_red 			= cfg_getint(cfg_terminal,"background_color_red");
		terminal->config.bg_blue 			= cfg_getint(cfg_terminal,"background_color_blue");
		terminal->config.bg_green 		    = cfg_getint(cfg_terminal,"background_color_green");
		terminal->config.layer 			    = cfg_getint(cfg_terminal,"layer");
        terminal->config.ani_sleep_sec      = cfg_getint(cfg_terminal,"ani_sleep_sec");
		
		/* BOOL */
		terminal->config.font_bold 			= cfg_getbool(cfg_terminal,"font_bold");
		terminal->config.blink_curser 		= cfg_getbool(cfg_terminal,"blink_curser");
		terminal->config.audio_bell 		= cfg_getbool(cfg_terminal,"audio_bell");
		terminal->config.antialias 			= cfg_getbool(cfg_terminal,"antialias");
		terminal->config.double_buffer 		= cfg_getbool(cfg_terminal,"double_buffer");
		terminal->config.taskbar_view 		= cfg_getbool(cfg_terminal,"taskbar_view");
		terminal->config.all_workspace_view = cfg_getbool(cfg_terminal,"all_workspace_view");
		terminal->config.accept_focus		= cfg_getbool(cfg_terminal,"accept_focus");
		terminal->config.skip_pager			= cfg_getbool(cfg_terminal,"skip_pager");
		terminal->config.show_hide			= cfg_getbool(cfg_terminal,"show_hide");
	} /*}}}*/

    lowterm->terminal_count = table_count;

	cfg_free(cfg);

	return 0;
}
