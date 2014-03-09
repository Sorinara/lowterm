#include "lowterm.h"
#include <malloc.h>

//#define OPT_DEBUG
#define FILE_NAME       ".mytermcfg"
#define FILE_LEN        100
#define CONFIG_TITLE    "Terminal"

int Config_Get(struct st_terminal **term_conf)		
{
	char  config_file[FILE_LEN];
	char  *home_dirtory;
	cfg_t *cfg,
          *cfg_terminal;
	int   i,
          term_num;

	cfg_opt_t terminal_opts[]={		//장기적으로 파싱 함수 내장할것/*{{{*/
		CFG_STR("session_name", 				"temp", 	CFGF_NONE),
		CFG_STR("screen_options", 				"", 		CFGF_NONE),
		CFG_STR("binding_key", 					"", 		CFGF_NONE),
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
		CFG_SEC(CONFIG_TITLE,terminal_opts,CFGF_TITLE | CFGF_MULTI),
		CFG_END()
	};

	memset(config_file, 0, sizeof(config_file));
	home_dirtory = getenv("HOME");

    if(FILE_LEN < strlen(home_dirtory) + strlen(FILE_NAME)){
        fprintf(stderr,"파일 이름이 너무 깁니다");
        return -1;
    }

	sprintf(config_file, "%s/"FILE_NAME, home_dirtory);
#ifdef OPT_DEBUG
	printf("file_name: %s\n", config_file);
#endif
	cfg = cfg_init(opts, 0);
	
	if(cfg_parse(cfg, config_file) == CFG_PARSE_ERROR){
		printf("기본적인 내용이 %s 파일에 저장되었습니다!", config_file);
		return -1;
	}

	/* term_num 가지고 Malloc 하기 */
	term_num = cfg_size(cfg, CONFIG_TITLE);

	if((*term_conf = (struct st_terminal *)malloc(sizeof(struct st_terminal) * term_num)) == NULL){
		perror("Memory Allocation Failed");
		exit(1);
	}

	for(i = 0; i < term_num ; i++){/*{{{*/
		cfg_terminal = cfg_getnsec(cfg, CONFIG_TITLE, i);
#ifdef OPT_DEBUG
		printf("session title %s\n", cfg_title(cfg_terminal));
#endif
        (*term_conf)[i].index 			            = i;

        /* STRING  문자열 증발 방지 위해 strdup함수 사용 */
        (*term_conf)[i].config.name 			    = strdup(cfg_getstr(cfg_terminal,"session_name"));
        (*term_conf)[i].config.scr_opt 			    = strdup(cfg_getstr(cfg_terminal,"screen_options"));
        (*term_conf)[i].config.font 			    = strdup(cfg_getstr(cfg_terminal,"font"));
        (*term_conf)[i].config.bd_key 			    = strdup(cfg_getstr(cfg_terminal,"binding_key"));
        (*term_conf)[i].config.locale 			    = strdup(cfg_getstr(cfg_terminal,"locale"));
        (*term_conf)[i].config.bg_image 		    = strdup(cfg_getstr(cfg_terminal,"background_image"));
        (*term_conf)[i].config.ani_start_place      = strdup(cfg_getstr(cfg_terminal,"ani_start_place"));
        (*term_conf)[i].config.ani_end_place        = strdup(cfg_getstr(cfg_terminal,"ani_end_place"));
		
		/* FLOAT */	
		(*term_conf)[i].config.bg_transparency 	    = cfg_getfloat(cfg_terminal,"background_transparency");

		/* INTEGER */
		(*term_conf)[i].config.win_start_x 		    = cfg_getint(cfg_terminal,"start_x"); 
		(*term_conf)[i].config.win_start_y 		    = cfg_getint(cfg_terminal,"start_y");
		(*term_conf)[i].config.win_height 		    = cfg_getint(cfg_terminal,"height");
		(*term_conf)[i].config.win_width 		    = cfg_getint(cfg_terminal,"width");
		(*term_conf)[i].config.tx_red 			    = cfg_getint(cfg_terminal,"text_color_red");
		(*term_conf)[i].config.tx_green 		    = cfg_getint(cfg_terminal,"text_color_green");
		(*term_conf)[i].config.tx_blue 			    = cfg_getint(cfg_terminal,"text_color_blue");
		(*term_conf)[i].config.bg_red 			    = cfg_getint(cfg_terminal,"background_color_red");
		(*term_conf)[i].config.bg_blue 			    = cfg_getint(cfg_terminal,"background_color_blue");
		(*term_conf)[i].config.bg_green 		    = cfg_getint(cfg_terminal,"background_color_green");
		(*term_conf)[i].config.layer 			    = cfg_getint(cfg_terminal,"layer");
        (*term_conf)[i].config.ani_sleep_sec        = cfg_getint(cfg_terminal,"ani_sleep_sec");
		
		/* BOOL */
		(*term_conf)[i].config.font_bold 			= cfg_getbool(cfg_terminal,"font_bold");
		(*term_conf)[i].config.blink_curser 		= cfg_getbool(cfg_terminal,"blink_curser");
		(*term_conf)[i].config.audio_bell 		    = cfg_getbool(cfg_terminal,"audio_bell");
		(*term_conf)[i].config.antialias 			= cfg_getbool(cfg_terminal,"antialias");
		(*term_conf)[i].config.double_buffer 		= cfg_getbool(cfg_terminal,"double_buffer");
		(*term_conf)[i].config.taskbar_view 		= cfg_getbool(cfg_terminal,"taskbar_view");
		(*term_conf)[i].config.all_workspace_view   = cfg_getbool(cfg_terminal,"all_workspace_view");
		(*term_conf)[i].config.accept_focus		    = cfg_getbool(cfg_terminal,"accept_focus");
		(*term_conf)[i].config.skip_pager			= cfg_getbool(cfg_terminal,"skip_pager");
		(*term_conf)[i].config.show_hide			= cfg_getbool(cfg_terminal,"show_hide");
	}
/*}}}*/

	cfg_free(cfg);

	return term_num;
}
