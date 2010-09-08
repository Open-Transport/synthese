#ifndef AGI_CTOOL_H
#define AGI_CTOOL_H

#define AGIRES_OK 200

#define AST_STATE_DOWN 0
#define AST_STATE_RESERVED 1
#define AST_STATE_OFFHOOK 2
#define AST_STATE_DIALING 3
#define AST_STATE_RING 4
#define AST_STATE_RINGING 5
#define AST_STATE_UP 6
#define AST_STATE_BUSY 7
#define AST_STATE_DIALING_OFFHOOK 8
#define AST_STATE_PRERING 9

#define AST_CONFIG_DIR "/etc/asterisk"
#define DEFAULT_PHPAGI_CONFIG "/phpagi.conf"
#define AST_SPOOL_DIR "/var/spool/asterisk/"
#define AST_TMP_DIR AST_SPOOL_DIR "/tmp/"

#ifdef WIN32  // would that happen?  Just in case.
#define DIRECTORY_SEPARATOR '\\'
#else
#define DIRECTORY_SEPARATOR '/'
#endif

#ifndef atoul
#define atoul(str) strtoul(str,NULL,10)
#endif // atoul

#ifndef strlcpy
#define strlcpy(dest, src, len); strncpy(dest,src,len);dest[len-1]=0;
#endif // strlcpy

#ifndef MAXPATH
#define MAXPATH 1024
#endif // MAXPATH

typedef struct __agi_val_list_ {
	char *field;
	char *value;
	struct __agi_val_list_ *next;
} AGI_VAL_LIST;

typedef struct _asterisk_cmd_result_ {
	int code;
	char result[100];
	unsigned int endpos;
	char data[2048];
	char raw[2048];
} AGI_CMD_RESULT;


typedef struct _asterisk_tools_ {
	FILE *out;
	FILE *in;
	AGI_VAL_LIST *agi_vars;
	AGI_VAL_LIST *settings;
	int dtmf_timeout;
} AGI_TOOLS;

AGI_VAL_LIST *AGITool_ListAddItem(AGI_VAL_LIST *l, char *field, char *value);
void AGITool_ListDestroy(AGI_VAL_LIST *l);
char *AGITool_ListGetVal(AGI_VAL_LIST *l, char *field);


int AGITool_Init(AGI_TOOLS *tool);
void AGITool_Destroy(AGI_TOOLS *tool);

int AGITool_sendcmd(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *command, ...);

// commands
int AGITool_answer(AGI_TOOLS *tool, AGI_CMD_RESULT *res);
int AGITool_autohangup(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int time);
int AGITool_channel_status(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *channel);
int AGITool_exec(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *application, char *options);
int AGITool_get_data(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *filename, int timeout, int max_digits);
int AGITool_get_variable(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *variable);
int AGITool_get_variable2(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *variable, char *dest, int len);
int AGITool_hangup(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *channel);
int AGITool_receive_char(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int timeout);
int AGITool_record_file(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *file, char *format, char *escape_digits, int timeout, int beep, int silence, int offset);
int AGITool_say_digits(AGI_TOOLS *tool, AGI_CMD_RESULT *res,char *digits, char *escape_digits);
int AGITool_say_phonetic(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *text, char *escape_digits);
int AGITool_say_number(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int number, char *escape_digits);
int AGITool_send_image(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *image);
int AGITool_send_text(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *text);
int AGITool_set_callerid(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *cid);
int AGITool_set_context(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *context);
int AGITool_set_extension(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *extension);
int AGITool_set_priority(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int priority);
int AGITool_set_variable(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *variable, char *value);
int AGITool_stream_file(AGI_TOOLS *tool, AGI_CMD_RESULT *res,char *filename, char *escape_digits, int offset);
int AGITool_tdd_mode(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *setting);
int AGITool_verbose(AGI_TOOLS *tool, AGI_CMD_RESULT *res,char *message, int level);
int AGITool_wait_for_digit(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int timeout);
int AGITool_database_put(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *family, char *key, char *value);
int AGITool_database_get(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *family, char *key);
int AGITool_database_del(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *family, char *key);
int AGITool_database_deltree(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *family, char *keytree);
int AGITool_say_time(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int time, char *escape_digits);
int AGITool_noop(AGI_TOOLS *tool, AGI_CMD_RESULT *res);
//int AGITool_set_music(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int enabled, char *class);

int AGITool_text2wav(AGI_TOOLS *agi, AGI_CMD_RESULT *res, char *text, char *escape_digits, int frequency);


int AGITool_exec_absolutetimeout(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int seconds);
int AGITool_exec_setlanguage(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *language);
int AGITool_exec_dial(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *type, char *destnum, char *gateway, int dialtimeout, int sessiontimeout);
int AGITool_exec_goto(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *context, char *extension, char *priority);

#endif //AGI_CTOOL
