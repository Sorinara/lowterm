#include "lowterm.h"
#include <malloc.h>
#include <getopt.h>

typedef struct _Param
{/*{{{*/
    char *data;
    int   length;
} Param;/*}}}*/

void Config_Print_Param_Debug(Config config)
{/*{{{*/
    if(config.name != NULL){
        fprintf(stderr, "--name                   : %s\n", config.name);
    }

    if(config.execute != NULL){
        fprintf(stderr, "--execute                : %s\n", config.execute);
    }

    if(config.win_bind_key != NULL){
        fprintf(stderr, "--win-bind-key [key]     : %s\n", config.win_bind_key);
    }

    if(config.win_bind_key_mask != NULL){
        fprintf(stderr, "--win-bind-key [mask]    : %s\n", config.win_bind_key_mask);
    }

    if(config.win_visible != 0){
        fprintf(stderr, "--win-visible            : %s\n", config.win_visible ? "True" : "False");
    }

    if(config.win_pos_x != 0){
        fprintf(stderr, "--win-pos [x]            : %d\n", config.win_pos_x);
    }

    if(config.win_pos_y != 0){
        fprintf(stderr, "--win-pos [y]            : %d\n", config.win_pos_y);
    }

    if(config.win_layer != 0){
        fprintf(stderr, "--win-layer              : %d\n", config.win_layer);
    }

    if(config.win_focus != 0){
        fprintf(stderr, "--win-focus              : %s\n", config.win_focus ? "True" : "False");
    }

    if(config.win_show_pager != 0){
        fprintf(stderr, "--win-show-pager         : %s\n", config.win_show_pager ? "True" : "False");
    }

    if(config.win_show_taskbar != 0){
        fprintf(stderr, "--win-show-taskbar       : %s\n", config.win_show_taskbar ? "True" : "False");
    }

    if(config.win_show_all_workspace != 0){
        fprintf(stderr, "--win-show-all-workspace : %s\n", config.win_show_all_workspace ? "True" : "False");
    }    

    if(config.win_animation_delay != 0){
        fprintf(stderr, "--win-move-delay-second  : %d\n", config.win_animation_delay);
    }

    if(config.win_animation_move_start != NULL){
        fprintf(stderr, "--win-move-start         : %s\n", config.win_animation_move_start);
    }

    if(config.win_animation_move_end != NULL){
        fprintf(stderr, "--win-move-end           : %s\n", config.win_animation_move_end);
    }

    if(config.term_size_width != 0){
        fprintf(stderr, "--term-size [width]       : %d\n", config.term_size_width);
    }
    
    if(config.term_size_height != 0){
        fprintf(stderr, "--term-size [height]      : %d\n", config.term_size_height);
    }

    if(config.term_font != NULL){
        fprintf(stderr, "--term-font              : %s\n", config.term_font);
    }

    if(config.term_font_bold != 0){
        fprintf(stderr, "--term-font-bold         : %s\n", config.term_font_bold ? "True" : "False");
    }

    if(config.term_locale != NULL){
        fprintf(stderr, "--term-locale            : %s\n", config.term_locale);
    }

    if(config.term_cursor_blink != 0){
        fprintf(stderr, "--term-blink-curser      : %d\n", config.term_cursor_blink);
    }

    if(config.term_cursor_shape != 0){
        fprintf(stderr, "--term-blink-curser      : %s\n", config.term_cursor_shape ? "True" : "False");
    }

    if(config.term_audio_bell != 0){
        fprintf(stderr, "--term-audio-bell        : %s\n", config.term_audio_bell ? "True" : "False");
    }

    fprintf(stderr, "--term-backcolor [red]   : %f\n", config.term_back_color_red);
    fprintf(stderr, "--term-backcolor [green] : %f\n", config.term_back_color_green);
    fprintf(stderr, "--term-backcolor [blue]  : %f\n", config.term_back_color_blue);
    fprintf(stderr, "--term-backcolor [alpha] : %f\n", config.term_back_color_alpha);
    fprintf(stderr, "--term-textcolor [red]   : %f\n", config.term_text_color_red);
    fprintf(stderr, "--term-textcolor [green] : %f\n", config.term_text_color_green);
    fprintf(stderr, "--term-textcolor [blue]  : %f\n", config.term_text_color_blue);
    fprintf(stderr, "--term-textcolor [alpha] : %f\n", config.term_text_color_alpha);
    fprintf(stderr, "--term-cursor-color [green] : %f\n", config.term_cursor_color_green);
    fprintf(stderr, "--term-cursor-color [blue]  : %f\n", config.term_cursor_color_blue);
    fprintf(stderr, "--term-cursor-color [alpha] : %f\n", config.term_cursor_color_alpha);
}/*}}}*/

int Config_Parameter_Parser(char *execute, char delimiter, Param *buffer, int param_count)
{/*{{{*/
    char *now_pos,
         *delimiter_pos;
    int i;

    i       = 0;
    now_pos = execute;

    for(i = 0;i < param_count;i ++){
        if((delimiter_pos = strchr(now_pos, delimiter)) == NULL){
            buffer[i].data   = now_pos;
            buffer[i].length = execute + strlen(execute) - now_pos;

            //printf("Test %s %d\n", buffer[i].data, buffer[i].length);
            break;
        }

        buffer[i].data   = now_pos;
        buffer[i].length = delimiter_pos - now_pos;

        now_pos = delimiter_pos + 1;

        //printf("Test %s %d\n", buffer[i].data, buffer[i].length);
    }

    return i + 1;
}/*}}}*/

int Config_Parameter_Get(int argc, char *argv[], char **filepath, Config *config)
{/*{{{*/
    int    c,
           option_index = 0;
    Param  param_list[3];
    int    param_count;
    struct option long_options[] = {/*{{{*/
        {"rc-file",                     required_argument,  0, 'r'},
        {"name",                        required_argument,  0, 'n'},
        {"execute",                     required_argument,  0, 'e'},
        {"win-bind-key",                required_argument,  0, 'k'},
        {"win-visible",                 no_argument,        0, 'v'},
        {"win-pos",                     required_argument,  0, 'p'},
        {"win-layer",                   required_argument,  0, 'L'},
        {"win-focus",                   no_argument,        0, 'F'},
        {"win-show-pager",              no_argument,        0, 'P'},
        {"win-show-taskbar",            no_argument,        0, 'T'},
        {"win-show-all-workspace",      no_argument,        0, 'w'},
        {"win-animation-delay",         required_argument,  0, 'd'},
        {"win-animation-move-start",    required_argument,  0, 'S'},
        {"win-animation-move-end",      required_argument,  0, 'E'},
        {"term-size",                   required_argument,  0, 's'},
        {"term-font",                   required_argument,  0, 'f'},
        {"term-font-bold",              no_argument,        0, 'b'},
        {"term-locale",                 required_argument,  0, 'l'},
        {"term-blink-curser",           no_argument,        0, 'B'},
        {"term-double-buffer",          no_argument,        0, 'D'},
        {"term-audio-bell",             no_argument,        0, 'A'},
        {"term-image",                  required_argument,  0, 'i'},
        {"term-backcolor",              required_argument,  0, 'C'},
        {"term-textcolor",              required_argument,  0, 'c'},
        {0, 0, 0, 0}
    };/*}}}*/
    Config options;

    *filepath                           = NULL;

    options.name                        = "Terminal";
    options.execute                     = "/bin/sh";
    options.win_bind_key                = "";
    options.win_bind_key_mask           = "";
    options.win_visible                 = TRUE;
	options.win_pos_x                   = -1;
	options.win_pos_y                   = -1;
	options.win_layer                   = 0;
	options.win_focus                   = TRUE;
    options.win_show_pager              = TRUE;
	options.win_show_taskbar            = TRUE;
	options.win_show_all_workspace      = TRUE;
    options.win_animation_delay         = 1000;
    options.win_animation_move_start    = "left";
    options.win_animation_move_end      = "left";
	options.term_size_height            = 500;
	options.term_size_width             = 500;
	options.term_font                   = "";
	options.term_font_bold              = TRUE;
	options.term_locale                 = "";
	options.term_cursor_blink           = TRUE;
	options.term_cursor_shape           = 0;
	options.term_audio_bell             = TRUE;
	options.term_back_color_red         = 0.0;
	options.term_back_color_green       = 0.0;
	options.term_back_color_blue 	    = 0.0;
	options.term_back_color_alpha       = 1.0;
	options.term_text_color_red 	    = 1.0;
	options.term_text_color_green 	    = 1.0;
	options.term_text_color_blue 	    = 1.0;
	options.term_text_color_alpha       = 0.0;
	options.term_cursor_color_red       = 0.0;
	options.term_cursor_color_green 	= 0.0;
	options.term_cursor_color_blue 	    = 0.0;
	options.term_cursor_color_alpha     = 0.0;

    while (1){
        c = getopt_long(argc, argv, "r:n:e:c:l:f:BC:i:t:k:d:S:E:s:p:L:HFaUDAPTwuo", long_options, &option_index);

        if (c == -1)
            break;

        switch (c){
            case 'r':
                if(strcmp(options.win_bind_key, "") != 0){
                    free(options.win_bind_key);
                }

                if(strcmp(options.win_bind_key_mask, "") != 0){
                    free(options.win_bind_key_mask);
                }

                *filepath = strdup(optarg);

                return 1;
            case 'n':
                options.name = strdup(optarg);
                break;
            case 'e':
                options.execute = strdup(optarg);
                break;
            case 'k':
                memset(param_list, 0x00, sizeof(param_list));

                param_count = Config_Parameter_Parser(optarg, '|', param_list, 2);
                options.win_bind_key = strndup(param_list[0].data, param_list[0].length);

                if(param_count >= 2){
                    options.win_bind_key_mask = strndup(param_list[1].data, param_list[1].length);
                }
                break;
            case 'v':
                options.win_visible = TRUE;
                break;
            case 'p':
                memset(param_list, 0x00, sizeof(param_list));

                if(Config_Parameter_Parser(optarg, 'x', param_list, 2) == 2){
                    options.win_pos_x = atoi(param_list[0].data);
                    options.win_pos_y = atoi(param_list[1].data);
                }
                break;
            case 's':
                memset(param_list, 0x00, sizeof(param_list));

                if(Config_Parameter_Parser(optarg, 'x', param_list, 2) == 2){
                    options.term_size_width  = atoi(param_list[0].data);
                    options.term_size_height = atoi(param_list[1].data);
                }
                break;
            case 'L':
                options.win_layer = atoi(optarg);
                break;
            case 'F':
                options.win_focus = FALSE;
                break;
            case 'P':
                options.win_show_pager = FALSE;
                break;
            case 'T':
                options.win_show_taskbar = FALSE;
                break;
            case 'w':
                options.win_show_all_workspace = FALSE;
                break;
            case 'd':
                options.win_animation_delay = atoi(optarg);
                break;
            case 'S':
                options.win_animation_move_start = strdup(optarg);
                break;
            case 'E':
                options.win_animation_move_end = strdup(optarg);
                break;
            case 'f':
                options.term_font = strdup(optarg);
                break;
            case 'b':
                options.term_font_bold = FALSE;
                break;
            case 'a':
                break;
            case 'l':
                options.term_locale = strdup(optarg);
                break;
            case 'B':
                options.term_cursor_blink = FALSE;
                break;
            case 'u':
                options.term_cursor_shape = FALSE;
                break;
            case 'D':
                break;
            case 'A':
                options.term_audio_bell = FALSE;
                break;
            case 'i':
                break;
            case 't':
                break;
            case 'C':
                memset(param_list, 0x00, sizeof(param_list));

                if(Config_Parameter_Parser(optarg, ',', param_list, 4) == 4){
                    options.term_back_color_red   = strtod(param_list[0].data, NULL);
                    options.term_back_color_green = strtod(param_list[1].data, NULL);
                    options.term_back_color_blue  = strtod(param_list[2].data, NULL);
                    options.term_back_color_alpha = strtod(param_list[3].data, NULL);
                }
                break;
            case 'c':
                memset(param_list, 0x00, sizeof(param_list));

                if(Config_Parameter_Parser(optarg, ',', param_list, 4) == 4){
                    options.term_text_color_red   = strtod(param_list[0].data, NULL);
                    options.term_text_color_green = strtod(param_list[1].data, NULL);
                    options.term_text_color_blue  = strtod(param_list[2].data, NULL);
                    options.term_text_color_alpha = strtod(param_list[3].data, NULL);
                }
                break;
            case 'o':
                memset(param_list, 0x00, sizeof(param_list));

                if(Config_Parameter_Parser(optarg, ',', param_list, 4) == 4){
                    options.term_cursor_color_red   = strtod(param_list[0].data, NULL);
                    options.term_cursor_color_green = strtod(param_list[1].data, NULL);
                    options.term_cursor_color_blue  = strtod(param_list[2].data, NULL);
                    options.term_cursor_color_alpha = strtod(param_list[3].data, NULL);
                }
                break;
        }
    }

    memcpy(config, &options, sizeof(Config));

    return 0;
/*}}}*/}

int Config_File_Table_Size(cfg_t **cfg, const char *config_path)
{/*{{{*/
	int table_count;

    if(config_path == NULL){
        return -1;
    }

	cfg_opt_t terminal_opts[]={ /*{{{*/
		CFG_STR("name", 				    "temp", 	            CFGF_NONE),
		CFG_STR("execute", 				    "/bin/sh", 	            CFGF_NONE),
		CFG_STR_LIST("win_bind_key", 		"{}", 		            CFGF_NONE),
		CFG_BOOL("win_visible",				TRUE,		            CFGF_NONE),
		CFG_INT_LIST("win_pos", 			"{0, 0}", 			    CFGF_NONE),
		CFG_INT("win_layer", 				0, 			            CFGF_NONE),
		CFG_BOOL("win_focus",				TRUE,		            CFGF_NONE),
		CFG_BOOL("win_show_pager",			TRUE,		            CFGF_NONE),
		CFG_BOOL("win_show_taskbar", 		TRUE, 		            CFGF_NONE),
		CFG_BOOL("win_show_all_workspace", 	FALSE, 		            CFGF_NONE),
        CFG_INT("win_animation_delay",      1000,                   CFGF_NONE),
        CFG_STR_LIST("win_animation_move",  "{left, left}",         CFGF_NONE),
		CFG_INT_LIST("term_size", 			"{500, 500}", 		    CFGF_NONE), /*1021이하여야함*/
		CFG_STR("term_font", 				"", 		            CFGF_NONE),
		CFG_BOOL("term_font_bold", 			TRUE, 		            CFGF_NONE),
		CFG_STR("term_locale", 				"",  		            CFGF_NONE),
		CFG_BOOL("term_cursor_blink", 		TRUE, 		            CFGF_NONE),
		CFG_INT("term_cursor_shape", 		TRUE, 		            CFGF_NONE),
		CFG_BOOL("term_audio_bell", 		TRUE, 		            CFGF_NONE),
		CFG_FLOAT_LIST("term_back_color",   "{1.0, 1.0, 1.0, 1.0}", CFGF_NONE),
		CFG_FLOAT_LIST("term_text_color",   "{0.0, 0.0, 0.0, 0.0}", CFGF_NONE),
		CFG_FLOAT_LIST("term_cursor_color", "{0.0, 0.0, 0.0, 0.0}", CFGF_NONE),
		CFG_END()
	}; /*}}}*/

	cfg_opt_t opts[] = {
		CFG_SEC(CONFIG_TITLE, terminal_opts, CFGF_TITLE | CFGF_MULTI),
		CFG_END()
	};

	*cfg = cfg_init(opts, CFGF_NONE);
	
	if(cfg_parse(*cfg, config_path) == CFG_PARSE_ERROR){
		fprintf(stderr, "설정파일(%s)에 오류가 있습니다.", config_path);
		return -2;
	}

	table_count = cfg_size(*cfg, CONFIG_TITLE);

	return table_count;
/*}}}*/}

int Config_File_Table_Data(cfg_t *cfg, int table_index, Config *config)
{/*{{{*/
    cfg_t *cfg_table;

    if((cfg_table = cfg_getnsec(cfg, CONFIG_TITLE, table_index)) == NULL){
        return -1;
    }

    /* STRING  문자열 증발 방지 위해 strdup함수 사용 */
    config->name 			            = strdup(cfg_getstr(cfg_table, "name"));
    config->execute                     = strdup(cfg_getstr(cfg_table, "execute"));
    config->term_font 			        = strdup(cfg_getstr(cfg_table, "term_font"));
    config->win_bind_key_mask           = strdup(cfg_getnstr(cfg_table, "win_bind_key", 0));
    config->win_bind_key 		        = strdup(cfg_getnstr(cfg_table, "win_bind_key", 1));
    config->term_locale                 = strdup(cfg_getstr(cfg_table, "term_locale"));
    config->win_animation_move_start    = strdup(cfg_getnstr(cfg_table, "win_animation_move", 0));
    config->win_animation_move_end      = strdup(cfg_getnstr(cfg_table, "win_animation_move", 1));

    /* INTEGER */
    config->win_pos_x 		            = cfg_getnint(cfg_table, "win_pos", 0); 
    config->win_pos_y 		            = cfg_getnint(cfg_table, "win_pos", 1);
    config->term_size_width 		    = cfg_getnint(cfg_table, "term_size", 0);
    config->term_size_height 		    = cfg_getnint(cfg_table, "term_size", 1);
    config->win_layer 			        = cfg_getint(cfg_table, "win_layer");
    config->win_animation_delay         = cfg_getint(cfg_table, "win_animation_delay");
    config->term_cursor_shape           = cfg_getint(cfg_table, "term_cursor_shape");

    /* FLOAT(DOUBLE) */
    config->term_back_color_red         = cfg_getnfloat(cfg_table, "term_back_color", 0);
    config->term_back_color_green 	    = cfg_getnfloat(cfg_table, "term_back_color", 1);
    config->term_back_color_blue 	    = cfg_getnfloat(cfg_table, "term_back_color", 2);
    config->term_back_color_alpha       = cfg_getnfloat(cfg_table, "term_back_color", 3);
    config->term_text_color_red         = cfg_getnfloat(cfg_table, "term_text_color", 0);
    config->term_text_color_green       = cfg_getnfloat(cfg_table, "term_text_color", 1);
    config->term_text_color_blue 	    = cfg_getnfloat(cfg_table, "term_text_color", 2);
    config->term_text_color_alpha       = cfg_getnfloat(cfg_table, "term_text_color", 3);
    config->term_cursor_color_red       = cfg_getnfloat(cfg_table, "term_cursor_color", 0);
    config->term_cursor_color_green     = cfg_getnfloat(cfg_table, "term_cursor_color", 1);
    config->term_cursor_color_blue 	    = cfg_getnfloat(cfg_table, "term_cursor_color", 2);
    config->term_cursor_color_alpha     = cfg_getnfloat(cfg_table, "term_cursor_color", 3);

    /* BOOL */
    config->term_font_bold 		        = cfg_getbool(cfg_table, "term_font_bold");
    config->term_cursor_blink 		    = cfg_getbool(cfg_table, "term_cursor_blink");
    config->term_audio_bell 		    = cfg_getbool(cfg_table, "term_audio_bell");
    config->win_show_taskbar            = cfg_getbool(cfg_table, "win_show_taskbar");
    config->win_show_all_workspace      = cfg_getbool(cfg_table, "win_show_all_workspace");
    config->win_focus		            = cfg_getbool(cfg_table, "win_focus");
    config->win_show_pager			    = cfg_getbool(cfg_table, "win_show_pager");
    config->win_visible                 = cfg_getbool(cfg_table, "win_visible");

	return 0;
/*}}}*/}

void Config_Free(LowTerm lowterm)
{ /*{{{*/
    int i;

    for(i = 0;i < lowterm.terminal_count;i++){
        /* Config_Print_Param_Debug((lowterm.terminal)[i].config); */

        free(lowterm.terminal[i].config.name);
        free(lowterm.terminal[i].config.execute);
        free(lowterm.terminal[i].config.term_font);
        free(lowterm.terminal[i].config.term_locale);
        free(lowterm.terminal[i].config.win_animation_move_start);
        free(lowterm.terminal[i].config.win_animation_move_end);
        free(lowterm.terminal[i].config.win_bind_key);
        free(lowterm.terminal[i].config.win_bind_key_mask);
    }
    
    free(lowterm.terminal);
} /*}}}*/

int Config_Get(int argc, char *argv[], LowTerm *lowterm)
{ /*{{{*/
    int param_status;
    char *filepath;
    int table_count,
        i;
    cfg_t *cfg;

	if((lowterm->terminal = (Terminal *)calloc(1, sizeof(Terminal))) == NULL){
		return -1;
	}

    if((param_status = Config_Parameter_Get(argc, argv, &filepath, &(lowterm->terminal[0].config))) < 0){
        return -2;
    }

    if(param_status == 0){
        table_count = 1;
    }else{
        table_count = Config_File_Table_Size(&cfg, filepath);
        free(filepath);

        if((lowterm->terminal = (Terminal *)realloc(lowterm->terminal, table_count * sizeof(Terminal))) == NULL){
            cfg_free(cfg);
            free(lowterm->terminal);
            return -3;
        }

        for(i = 0;i < table_count;i ++){
            if(Config_File_Table_Data(cfg, i, &((lowterm->terminal)[i].config)) < 0){
                cfg_free(cfg);
                free(lowterm->terminal);
                return -4;
            }
        }

	    cfg_free(cfg);
    }

    lowterm->terminal_count = table_count;

    return 0;
} /*}}}*/
