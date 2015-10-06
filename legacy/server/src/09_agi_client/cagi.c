// 
//   Written by: David Cornewell (david.cornewell@gmail.com)
//         Date: 03-16-2005
// 
//   Purpose: 	This is a C library for interfacing with Asterisks' AGI.  It is based on PHPAGI. 
//				Actually, it is almost completely copied from it. I just changed the code to C.
//				The in depth documentation is the same.
//
//   License:	LGPL. 
//
//   MODS: 
//        $Id: cagi.c,v 1.14 2006/11/13 16:14:19 dcornewell Exp $
//        $Log: cagi.c,v $
//        Revision 1.14  2006/11/13 16:14:19  dcornewell
//        Fixed bug in AGITool_sendcmd(); when parsing data from response it was getting
//        the length of the string inside the (). it did -1. it should have.
//
//        Revision 1.13  2006/10/06 20:39:45  dcornewell
//        put checkes in Init for the return of fgets. Just in case the stdin pipe
//        gets lost, but app is blocking SIGPIPE
//
//        Revision 1.12  2006/10/04 18:49:17  dcornewell
//        Changed AGITool_ListAddItem() to malloc field and value rather than use
//        static buffers with them.  uses memory necessary and allows for more than 50
//        bytes each.
//
//        Revision 1.11  2006/03/10 19:20:32  dcornewell
//        Fixed a bug when trying to get the status of a dead channel. if program handles
//        the SIGHUP and tries to clean up, it was checking the status of a channel
//        hoping to find out that it was down. would get SIGPIPE cause stdin/out/err are
//        not gone and would try to parse a NULL from fgets(stdin). checking for that
//        null now.
//
//        also implemented some exec functions. basically wrappers that call exec with
//        something.
//
//        Revision 1.10  2006/02/21 19:20:53  dcornewell
//        Checking value for null before doing anything with it.
//
//        Revision 1.9  2005/10/04 16:36:27  dcornewell
//        Updated AGITool_exec_dial prototype. added check to make sure res->data isn't
//        over filled. would take 2048 bytes, but you never know
//
//        Revision 1.8  2005/10/04 16:19:18  dcornewell
//        Fixed bug with exec dial. needed a \n
//
//        Revision 1.7  2005/10/03 13:50:39  dcornewell
//        Added AGITool_get_variable2() that takes a dest variable. it will fill that
//        field with the variable. More self explainatory.
//
//        Revision 1.6  2005/10/03 13:36:46  dcornewell
//        Fixed bug in send_cmd. I was parsing data from asterisk and I bombed a zero
//        into my buffer rather than res->result. this stopped the parsing and I never
//        filled res->data. Also added a raw field to the result structure that will
//        contain the raw data from asterisk. This would have helped some in debugging.
//
//        Revision 1.5  2005/09/29 20:02:49  dcornewell
//        Added AGITool_exec_dial() will dial. Thanks to Raymond Chen for testing and
//        working on this.
//
//        Revision 1.4  2005/07/07 15:15:33  dcornewell
//        Added a check for null in strim. Anything that stops core dumps. not likely,
//        but good to check.
//
//        Revision 1.3  2005/06/05 23:31:09  dcornewell
//        Changed the command AUTOHANGUP to "SET AUTOHANGUP".  I am not sure if I messed
//        this up, or if it just changed.  hope it works
//
//        Revision 1.2  2005/04/14 00:13:29  dcornewell
//        Changed License to LGPL.  Not sure why, people just like it better
//
//        Revision 1.1.1.1  2005/03/22 15:45:15  dcornewell
//        initial checkin of CAGI. Most needed features are implemented.
//


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "cagi.h"

int FileExists(char *path)
{
	struct stat file_stat;

	if(stat(path, &file_stat))
		return 0;
	return 1;
}

int strim(char *src)
{
	int i=0;
	if (src) {
		i=strlen(src)-1;
		while (i>0 && isspace(src[i])) {
			src[i]=0;
			i--;
		}
	}
	return i;
}

AGI_VAL_LIST *AGITool_ListAddItem(AGI_VAL_LIST *l, char *field, char *value)
{
	AGI_VAL_LIST *newl;

	newl = (AGI_VAL_LIST *)malloc(sizeof(AGI_VAL_LIST));
	if (newl) {
		memset(newl,0,sizeof(AGI_VAL_LIST));

		if (field!=NULL) {
			newl->field = (char*)malloc(strlen(field)+1);
			strlcpy(newl->field,field,strlen(field)+1);
		}
		if (value!=NULL) {
			newl->value = (char*)malloc(strlen(value)+1);
			strlcpy(newl->value,value,strlen(value)+1);
		}
		newl->next=l;
	}
	return newl;
}

void AGITool_ListDestroy(AGI_VAL_LIST *l)
{
	AGI_VAL_LIST *l2;

	while (l) {
		l2=l->next;
		free(l->field);l->field=NULL;
		free(l->value);l->value=NULL;
		free(l);l=NULL;		
		l=l2;
	}
	return;
}

char *AGITool_ListGetVal(AGI_VAL_LIST *l, char *field)
{
	while (l) {
		if (!strcmp(l->field, field)) {
			return l->value;
		}
		l=l->next;
	}
	return "";
}

int AGITool_Init(AGI_TOOLS *tool)
{
	char buffer[1024], *field, *value;
	
	// open stdin
	tool->in = stdin;
	
	// open stdout
	tool->out = stdout;
	
	/*
    * Often contains any/all of the following:
    *   agi_network - value is yes if this is a fastagi
    *   agi_network_script - name of the script to execute
    *   agi_request - name of agi script
    *   agi_channel - current channel
    *   agi_language - current language
    *   agi_type - channel type (SIP, ZAP, IAX, ...)
    *   agi_uniqueid - unique id based on unix time
    *   agi_callerid - callerID string
    *   agi_dnid - dialed number id
    *   agi_rdnis - referring DNIS number
    *   agi_context - current context
    *   agi_extension - extension dialed
    *   agi_priority - current priority
    *   agi_enhanced - value is 1.0 if started as an EAGI script
    *   agi_accountcode - set by SetAccount in the dialplan
    */
	
	// read the request
	tool->agi_vars=tool->settings=NULL;
	
	if ( fgets(buffer,sizeof(buffer),stdin) ) {
		while(strcmp(buffer, "\n") != 0) {
			field=buffer;
			value=strchr(buffer,':');
			if (value) {
				value[0]=0;
				value+=2;

				strim(value);
				tool->agi_vars = AGITool_ListAddItem(tool->agi_vars, field,value);
			}
	
			if ( !fgets(buffer,sizeof(buffer),stdin) ) {
				// if failed, may have lost pipe, but sigpipe is blocked.
				break;
			}
		}
	}
	
	// These could be configured in an ini...
	tool->settings = AGITool_ListAddItem(tool->settings, "tmpdir","/tmp/");
	tool->settings = AGITool_ListAddItem(tool->settings, "festival_text2wave","text2wave");
	tool->settings = AGITool_ListAddItem(tool->settings, "cepstral_swift","swift");

	return 0;
}

void AGITool_Destroy(AGI_TOOLS *tool)
{
	AGITool_ListDestroy(tool->agi_vars);
	AGITool_ListDestroy(tool->settings);
}

int AGITool_sendcmd(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *command, ...)
{
    va_list ap;
	char buffer[4096], *str, *ptr;
	int count,ret;

	res->code=500;
	strlcpy(res->result,"-1",sizeof(res->result));
	res->data[0]=0;

	// write command
    va_start(ap, command);
    ret = vfprintf(tool->out, command, ap);
	va_end(ap);
	fflush(tool->out);
	if (ret<0) {
		return 0;
	}

	// Read result.  Occasionally, a command return a string followed by an extra new line.
	// When this happens, our script will ignore the new line, but it will still be in the
	// buffer.  So, if we get a blank line, it is probably the result of a previous
	// command.  We read until we get a valid result or asterisk hangs up.  One offending
	// command is SEND TEXT.
	buffer[0]=0;
	str=buffer;

	for (count=0; str && !strlen(str) && count<5; count++) {
		str = fgets(buffer,sizeof(buffer),tool->in);
	}
	if (!str) {
		return atoi(res->result);
	}

	if(count >= 5) {
		return 0;
	}

	//
	// Let's save what asterisk sent in case caller needs raw data string. Good for debug too.
	// This will only do us good with single line responses though.
	//
	strlcpy(res->raw,buffer,sizeof(res->raw));

	strim(buffer);

  // parse result
	res->code = atoi(str);
	str+=4;

	if(str[0] == '-') // we have a multiline response!  Like Usage
	{
		char junkit[2048];
		fgets(junkit,sizeof(junkit),tool->in);
		while (atoi(junkit) != res->code) {
			fgets(junkit,sizeof(junkit),tool->in);
		}
	}

	if(res->code != AGIRES_OK) // some sort of error
	{
		strlcpy(res->data, str,sizeof(res->data));
	} else {// normal AGIRES_OK response

		ptr=strstr(str,"result=");
		if (ptr) {
			strlcpy(res->result, ptr+7, sizeof(res->result));
			ptr=strchr(res->result,' ');
			if (ptr) {
				ptr[0]=0;
			}
//			res->result = atoi(strstr(str,"result=")+7);
		}
		if (strstr(str, "endpos=")) {
			res->endpos = atoul(strstr(str,"endpos=")+7);
		}
		if (strchr(str, '(')) {
			if (strchr(str, ')')) {
				int dlen=strchr(str, ')')-strchr(str,'(');
				if (dlen>sizeof(res->data)) dlen=sizeof(res->data);
				strlcpy(res->data, strchr(str,'(')+1, dlen);
			} else {
				strlcpy(res->data, strchr(str,'(')+1, sizeof(res->data));
			}
		}
	}

	return atoi(res->result);
}


//****************************
// Commands to be sent
//****************************

//
// Answer channel if not already in answer state.
//
int AGITool_answer(AGI_TOOLS *tool, AGI_CMD_RESULT *res)
{
	return AGITool_sendcmd(tool, res, "ANSWER\n");
}

//
// Cause the channel to automatically hangup at $time seconds in the future.
// If $time is 0 then the autohangup feature is disabled on this channel.
//
// If the channel is hungup prior to $time seconds, this setting has no effect.
//
// @param integer $time until automatic hangup
// @return array, see evaluate for return information.
//
int AGITool_autohangup(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int time)
{
	return AGITool_sendcmd(tool, res, "SET AUTOHANGUP %d\n", time);
}

//
// Get the status of the specified channel. If no channel name is specified, return the status of the current channel.
//
// @param string $channel
//
int AGITool_channel_status(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *channel)
{
	int ret = AGITool_sendcmd(tool, res, "CHANNEL STATUS %s\n", channel);
	switch(atoi(res->result))
	{
		case -1: snprintf(res->data, sizeof(res->data), "There is no channel that matches %s",channel); break;
		case AST_STATE_DOWN: snprintf(res->data, sizeof(res->data), "Channel is down and available"); break;
		case AST_STATE_RESERVED: snprintf(res->data, sizeof(res->data), "Channel is down, but reserved"); break;
		case AST_STATE_OFFHOOK: snprintf(res->data, sizeof(res->data), "Channel is off hook"); break;
		case AST_STATE_DIALING: snprintf(res->data, sizeof(res->data), "Digits (or equivalent) have been dialed"); break;
		case AST_STATE_RING: snprintf(res->data, sizeof(res->data), "JourneyPattern is ringing"); break;
		case AST_STATE_RINGING: snprintf(res->data, sizeof(res->data), "Remote end is ringing"); break;
		case AST_STATE_UP: snprintf(res->data, sizeof(res->data), "JourneyPattern is up"); break;
		case AST_STATE_BUSY: snprintf(res->data, sizeof(res->data), "JourneyPattern is busy"); break;
		case AST_STATE_DIALING_OFFHOOK: snprintf(res->data, sizeof(res->data), "Digits (or equivalent) have been dialed while offhook"); break;
		case AST_STATE_PRERING: snprintf(res->data, sizeof(res->data), "Channel has detected an incoming call and is waiting for ring"); break;
		default: snprintf(res->data, sizeof(res->data), "Unknown result: %s", res->result); break;
	}
	return ret;
}

//
// Executes the specified Asterisk application with given options
//
// @link http://www.voip-info.org/wiki-Asterisk+-+documentation+of+application+commands
//
int AGITool_exec(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *application, char *options)
{
	char temp[4096];
	sprintf(temp,"EXEC %s %s\n", application, options);	
	return AGITool_sendcmd(tool, res, temp);
}

//
// Plays the given file and receives DTMF data.
//
// This is similar to STREAM FILE, but this command can accept and return many DTMF digits,
//  while STREAM FILE returns immediately after the first DTMF digit is detected.
//
// Asterisk looks for the file to play in /var/lib/asterisk/sounds 
//
// If the user doesn't press any keys when the message plays, there is $timeout milliseconds
// of silence then the command ends. 
//
// The user has the opportunity to press a key at any time during the message or the
// post-message silence. If the user presses a key while the message is playing, the
// message stops playing. When the first key is pressed a timer starts counting for
// $timeout milliseconds. Every time the user presses another key the timer is restarted.
// The command ends when the counter goes to zero or the maximum number of digits is entered,
// whichever happens first. 
//
// If you don't specify a time out then a default timeout of 2000 is used following a pressed
// digit. If no digits are pressed then 6 seconds of silence follow the message. 
//
// If you don't specify $max_digits then the user can enter as many digits as they want. 
//
// Pressing the # key has the same effect as the timer running out: the command ends and
// any previously keyed digits are returned. A side effect of this is that there is no
// way to read a # key using this command.
//
// @link http://www.voip-info.org/wiki-get+data
// @param string $filename file to play. Do not include file extension.
// @param integer $timeout milliseconds
// @param integer $max_digits
//
// This differs from other commands with return DTMF as numbers representing ASCII characters.
//
int AGITool_get_data(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *filename, int timeout, int max_digits)
{
	return AGITool_sendcmd(tool, res, "GET DATA %s %d %d\n", filename,timeout,max_digits);
}

//
// Fetch the value of a variable.
//
// Does not work with global variables. Does not work with some variables that are generated by modules.
//
// @link http://www.voip-info.org/wiki-Asterisk+variables
// @param string $variable name
//
int AGITool_get_variable(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *variable)
{
	return AGITool_sendcmd(tool, res, "GET VARIABLE %s\n", variable);
}

//
// Added this to make it easier to get a variable. more self explainatory.
// 
int AGITool_get_variable2(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *variable, char *dest, int len)
{
	int ret=AGITool_sendcmd(tool, res, "GET VARIABLE %s\n", variable);
	strlcpy(dest,res->data,len);
	return ret;
}

//
// Hangup the specified channel. If no channel name is given, hang up the current channel.
//
// With power comes responsibility. Hanging up channels other than your own isn't something
// that is done routinely. If you are not sure why you are doing so, then don't.
//
// Most channels do not support the reception of text.
//
// @param string $channel
//
int AGITool_hangup(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *channel)
{
  return AGITool_sendcmd(tool, res, "HANGUP %s\n", channel);
}

//
// Receive a character of text from a connected channel. Waits up to $timeout milliseconds for
// a character to arrive, or infinitely if $timeout is zero.
//
// @param integer $timeout milliseconds
// res->result is 0 on timeout or not supported, -1 on failure. Otherwise 
// it is the decimal value of the DTMF tone. Use chr() to convert to ASCII.
//
int AGITool_receive_char(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int timeout)
{
	return AGITool_sendcmd(tool, res, "RECEIVE CHAR %d\n", timeout);
}

//
// Record sound to a file until an acceptable DTMF digit is received or a specified amount of
// time has passed. Optionally the file BEEP is played before recording begins.
//
// @link http://www.voip-info.org/wiki-record+file
// @param string $file to record, without extension, often created in /var/lib/asterisk/sounds
// @param string $format of the file. GSM and WAV are commonly used formats. MP3 is read-only and thus cannot be used.
// @param string $escape_digits
// @param integer $timeout is the maximum record time in milliseconds, or -1 for no timeout.
// @param boolean $beep
// @param integer $silence number of seconds of silence allowed before the function returns despite the 
// lack of dtmf digits or reaching timeout.
// res->result is -1 on error, 0 on hangup, otherwise a decimal value of the 
// DTMF tone. Use chr() to convert to ASCII.
//
int AGITool_record_file(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *file, char *format, char *escape_digits, int timeout, int beep, int silence, int offset)
{
	char buf[255];
	snprintf(buf,sizeof(buf),"RECORD FILE %s %s \"%s\" %d",file,format,escape_digits, timeout);
	if (offset>0) {
		snprintf(&buf[strlen(buf)], sizeof(buf)-strlen(buf)," %d",offset);
	}
	if(beep) {
		snprintf(&buf[strlen(buf)], sizeof(buf)-strlen(buf)," BEEP");
	}
	if(silence>0) {
		snprintf(&buf[strlen(buf)], sizeof(buf)-strlen(buf)," s=%d",silence);
	}
	strcat(buf,"\n");
	return AGITool_sendcmd(tool, res, buf);
}

//
// Say the given digit string, returning early if any of the given DTMF escape digits are received on the channel.
//
// @link http://www.voip-info.org/wiki-say+digits
// @param integer $digits
// @param string $escape_digits
// @res->result is -1 on hangup or error, 0 if playback completes with no 
// digit received, otherwise a decimal value of the DTMF tone.  Use chr() to convert to ASCII.
//
int AGITool_say_digits(AGI_TOOLS *tool, AGI_CMD_RESULT *res,char *digits, char *escape_digits)
{
	return AGITool_sendcmd(tool, res, "SAY DIGITS %s \"%s\"\n", digits, escape_digits);
}

//
// Say the given character string, returning early if any of the given DTMF escape digits are received on the channel.
//
// @param string $text
// @param string $escape_digits
// @res->result is -1 on hangup or error, 0 if playback completes with no 
// digit received, otherwise a decimal value of the DTMF tone.  Use chr() to convert to ASCII.
//
int AGITool_say_phonetic(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *text, char *escape_digits)
{
	return AGITool_sendcmd(tool, res, "SAY PHONETIC %s \"%s\"\n", text, escape_digits);
}

//
// Say the given number, returning early if any of the given DTMF escape digits are received on the channel.
//
// @link http://www.voip-info.org/wiki-say+number
// @param integer number
// @param string escape_digits
// @res->result is -1 on hangup or error, 0 if playback completes with no 
// digit received, otherwise a decimal value of the DTMF tone.  Use chr() to convert to ASCII.
//s
int AGITool_say_number(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int number, char *escape_digits)
{
	return AGITool_sendcmd(tool, res, "SAY NUMBER %d \"%s\"\n", number, escape_digits);
}

//
// Send the specified image on a channel.
//
// Most channels do not support the transmission of images.
//
// @param string $image without extension, often in /var/lib/asterisk/images
// @res->result is -1 on hangup or error, 0 if the image is sent or 
// channel does not support image transmission.
//
int AGITool_send_image(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *image)
{
	return AGITool_sendcmd(tool, res, "SEND IMAGE %s\n", image);
}

//
// Send the given text to the connected channel.
//
// Most channels do not support transmission of text.
//
// @param $text
// @res->result is -1 on hangup or error, 0 if the text is sent or 
// channel does not support text transmission.
//
int AGITool_send_text(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *text)
{
	return AGITool_sendcmd(tool, res, "SEND TEXT \"%s\"\n", text);
}

//
// Changes the caller ID of the current channel
//
// @param string $cid example: "John Smith"<1234567>
// This command will let you take liberties with the <caller ID specification> but the format shown in the example above works 
// well: the name enclosed in double quotes followed immediately by the number inside angle brackets. If there is no name then
// you can omit it. If the name contains no spaces you can omit the double quotes around it. The number must follow the name
// immediately; don't put a space between them. The angle brackets around the number are necessary; if you omit them the
// number will be considered to be part of the name.
//
int AGITool_set_callerid(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *cid)
{
	return AGITool_sendcmd(tool, res, "SET CALLERID %s\n", cid);
}

//
// Sets the context for continuation upon exiting the application.
//
// Setting the context does NOT automatically reset the extension and the priority; if you want to start at the top of the new 
// context you should set extension and priority yourself. 
//
// If you specify a non-existent context you receive no error indication (['result'] is still 0) but you do get a 
// warning message on the Asterisk console.
//
// @param string $context 
//
int AGITool_set_context(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *context)
{
  return AGITool_sendcmd(tool, res, "SET CONTEXT %s\n", context);
}

//
// Set the extension to be used for continuation upon exiting the application.
//
// Setting the extension does NOT automatically reset the priority. If you want to start with the first priority of the 
// extension you should set the priority yourself. 
//
// If you specify a non-existent extension you receive no error indication (['result'] is still 0) but you do 
// get a warning message on the Asterisk console.
//
// @param string $extension
//
int AGITool_set_extension(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *extension)
{
	return AGITool_sendcmd(tool, res, "SET EXTENSION %s\n", extension);
}

//
// Set the priority to be used for continuation upon exiting the application.
//
// If you specify a non-existent priority you receive no error indication (['result'] is still 0)
// and no warning is issued on the Asterisk console.
//
// @param integer $priority
//
int AGITool_set_priority(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int priority)
{
	return AGITool_sendcmd(tool, res, "SET PRIORITY %d\n", priority);
}

//
// Sets a variable to the specified value. The variables so created can later be used by later using ${<variablename>}
// in the dialplan.
//
// These variables live in the channel Asterisk creates when you pickup a phone and as such they are both local and temporary. 
// Variables created in one channel can not be accessed by another channel. When you hang up the phone, the channel is deleted 
// and any variables in that channel are deleted as well.
//
// @param string $variable is case sensitive
// @param string $value
//
int AGITool_set_variable(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *variable, char *value)
{
//  $value = str_replace("\n", '\n', addslashes($value));
	return AGITool_sendcmd(tool, res, "SET VARIABLE %s \"%s\"\n", variable, value);
}

//
// Play the given audio file, allowing playback to be interrupted by a DTMF digit. This command is similar to the GET DATA 
// command but this command returns after the first DTMF digit has been pressed while GET DATA can accumulated any number of 
// digits before returning.
//
// @example examples/ping.php Ping an IP address
//
// @param string $filename without extension, often in /var/lib/asterisk/sounds
// @param string $escape_digits
// @param integer $offset
// @res->result is -1 on hangup or error, 0 if playback completes with no 
// digit received, otherwise a decimal value of the DTMF tone.  Use chr() to convert to ASCII.
//
int AGITool_stream_file(AGI_TOOLS *tool, AGI_CMD_RESULT *res,char *filename, char *escape_digits, int offset)
{
	return AGITool_sendcmd(tool, res, "STREAM FILE %s \"%s\" %d\n", filename, escape_digits, offset);
}

//
// Enable or disable TDD transmission/reception on the current channel.
//
// @param string $setting can be on, off or mate
// @res->result is 1 on sucess, 0 if the channel is not TDD capable.
//
int AGITool_tdd_mode(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *setting)
{
	return AGITool_sendcmd(tool, res, "TDD MODE %s\n", setting);
}

//
// Sends $message to the Asterisk console via the 'verbose' message system.
//
// If the Asterisk verbosity level is $level or greater, send $message to the console.
//
// The Asterisk verbosity system works as follows. The Asterisk user gets to set the desired verbosity at startup time or later 
// using the console 'set verbose' command. Messages are displayed on the console if their verbose level is less than or equal 
// to desired verbosity set by the user. More important messages should have a low verbose level; less important messages 
// should have a high verbose level.
//
// @param string $message
// @param integer $level from 1 to 4
//
int AGITool_verbose(AGI_TOOLS *tool, AGI_CMD_RESULT *res,char *message, int level)
{
	return AGITool_sendcmd(tool, res, "VERBOSE \"%s\" %d\n", message, level);
}

//
// Waits up to $timeout milliseconds for channel to receive a DTMF digit
//
// @param integer $timeout in millisecons. Use -1 for the timeout value if you want the call to wait indefinitely.
// @return array, see evaluate for return information. ['result'] is 0 if wait completes with no 
// digit received, otherwise a decimal value of the DTMF tone.  
//
int AGITool_wait_for_digit(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int timeout)
{
	return AGITool_sendcmd(tool, res, "WAIT FOR DIGIT %d\n", timeout);
}

//
// Adds or updates an entry in the Asterisk database for a given family, key, and value.
//
// @param string $family
// @param string $key
// @param string $value
// @res->result is 1 on sucess, 0 otherwise
//
int AGITool_database_put(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *family, char *key, char *value)
{
	return AGITool_sendcmd(tool, res, "DATABASE PUT \"%s\" \"%s\" \"%s\"\n", family, key, value);
}

//
// Retrieves an entry in the Asterisk database for a given family and key.
//
// @param string $family
// @param string $key
// @res->result is 1 on sucess, 0 failure. res->data holds the value
//
int AGITool_database_get(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *family, char *key)
{
	return AGITool_sendcmd(tool, res, "DATABASE GET \"%s\" \"%s\"\n", family, key);
}

//
// Deletes an entry in the Asterisk database for a given family and key.
//
// @param string $family
// @param string $key
// @res->result is 1 on sucess, 0 otherwise.
//
int AGITool_database_del(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *family, char *key)
{
	return AGITool_sendcmd(tool, res, "DATABASE DEL \"%s\" \"%s\"\n", family, key);
}

//
// Deletes a family or specific keytree within a family in the Asterisk database.
//
// @param string $family
// @param string $keytree
// @res->result is 1 on sucess, 0 otherwise.
//
int AGITool_database_deltree(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *family, char *keytree)
{
	if (strlen(keytree)) {
		return AGITool_sendcmd(tool, res, "DATABASE DELTREE \"%s\" \"%s\"", family, keytree);
	} else {
		return AGITool_sendcmd(tool, res, "DATABASE DELTREE \"%s\"", family);
	}
}


//
// Say a given time, returning early if any of the given DTMF escape digits are received on the channel.
//
// @link http://www.voip-info.org/wiki-say+time
// @param integer $time
// @param string $escape_digits
// @res->result is -1 on hangup or error, 0 if playback completes with no 
// digit received, otherwise a decimal value of the DTMF tone.  Use chr() to convert to ASCII.
//
int AGITool_say_time(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int time, char *escape_digits)
{
	return AGITool_sendcmd(tool, res, "SAY TIME %d \"%s\"\n", time, escape_digits);
}

//
// Does nothing
//
int AGITool_noop(AGI_TOOLS *tool, AGI_CMD_RESULT *res)
{
	return AGITool_sendcmd(tool, res, "NOOP");
}

//
// Enable/Disable Music on hold generator
//
// @param boolean $enabled
// @param string $class
//
/*
int AGITool_set_music(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int enabled, char *class)
{
	return AGITool_sendcmd(tool, res, "SET MUSIC %s %s\n", enabled?"ON":"OFF", class);
}
*/
//
// Still need to translate some from PHP to C
//

// *********************************************************************************************************
// **                       APPLICATIONS                                                                  **
// *********************************************************************************************************

//
// Set absolute maximum time of call
//
// Note that the timeout is set from the current time forward, not counting the number of seconds the call has already been up. 
// Each time you call AbsoluteTimeout(), all previous absolute timeouts are cancelled. 
// Will return the call to the T extension so that you can playback an explanatory note to the calling party (the called party 
// will not hear that)
//
// @link http://www.voip-info.org/wiki-Asterisk+-+documentation+of+application+commands
// @link http://www.dynx.net/ASTERISK/AGI/ccard/agi-ccard.agi
// @param seconds allowed, 0 disables timeout
//
int AGITool_exec_absolutetimeout(AGI_TOOLS *tool, AGI_CMD_RESULT *res, int seconds)
{
	char secbuf[30];
	snprintf(secbuf,sizeof(secbuf),"%d",seconds);
	return AGITool_exec(tool, res, "AbsoluteTimeout", secbuf);
}

//   /**
//    * Executes an AGI compliant application
//    *
//    * @param string $command
//    * @return array, see evaluate for return information. ['result'] is -1 on hangup or if application requested hangup, or 0 on non-hangup exit.
//    * @param string $args
//    */
//    int AGITool_exec_agi(AGI_TOOLS *tool, AGI_CMD_RESULT *res,$command, $args)
//    {
//      return $this->exec("AGI $command", $args);
//    }
//
//   /**
//    * Set Language 
//    *
//    * @param string $language code
//    * @return array, see evaluate for return information.
//    */
int AGITool_exec_setlanguage(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *language)
{
	if (!language) {
		language="en";
	}
	return AGITool_exec(tool, res, "SetLanguage", language);
}

//   /**
//    * Do ENUM Lookup
//    *
//    * Note: to retrieve the result, use
//    *   get_variable('ENUM');
//    *
//    * @param $exten
//    * @return array, see evaluate for return information.
//    */
//    int AGITool_exec_enumlookup(AGI_TOOLS *tool, AGI_CMD_RESULT *res,$exten)
//    {
//      return $this->exec('EnumLookup', $exten);
//    }

/**
* Dial
*
* @link http://www.voip-info.org/wiki-Asterisk+cmd+Dial
* @param string type
* @param string destnum
* @param string gateway
* @param integer dialtimeout
* @param integer sessiontimeout
* @return result code
*/
int AGITool_exec_dial(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *type, char *destnum, char *gateway, int dialtimeout, int sessiontimeout)
{
	return AGITool_sendcmd(tool, res, "EXEC Dial %s/%s@%s|%d|L(%d:80000:20000)\n", type, destnum, gateway, dialtimeout, sessiontimeout);  
}


/**
* Goto
*
* This function takes three arguments: context,extension, and priority, but the leading arguments
* are optional, not the trailing arguments.  Thuse goto($z) sets the priority to $z.
*
* @param string a
* @param string b;
* @param string c;
* @return result code
*/
int AGITool_exec_goto(AGI_TOOLS *tool, AGI_CMD_RESULT *res, char *context, char *extension, char *priority)
{
	char buf[255],*ptr;

	snprintf(buf, sizeof(buf), "%s|%s|%s", context?context:"",extension?extension:"",priority);
	ptr=buf;
	while (ptr[0]=='|') {
		ptr++;
	}
	return AGITool_exec(tool, res, "Goto", ptr);
}

//   // *********************************************************************************************************
//   // **                       DERIVED                                                                       **
//   // *********************************************************************************************************
//
//   /**
//    * Goto - Set context, extension and priority
//    *
//    * @param string $context
//    * @param string $extension
//    * @param string $priority
//    */
//    int AGITool_goto(AGI_TOOLS *tool, AGI_CMD_RESULT *res,$context, $extension='s', $priority=1)
//    {
//      $this->set_context($context);
//      $this->set_extension($extension);
//      $this->set_priority($priority);
//    }
//
//   /**
//    * Parse caller id
//    *
//    * @example examples/dtmf.php Get DTMF tones from the user and say the digits
//    * @example examples/input.php Get text input from the user and say it back
//    *
//    * "name" <proto:user@server:port>
//    *
//    * @param string $callerid
//    * @return array('Name'=>$name, 'Number'=>$number)
//    */
//    int AGITool_parse_callerid(AGI_TOOLS *tool, AGI_CMD_RESULT *res,$callerid=NULL)
//    {
//      if(is_null($callerid))
//        $callerid = $this->request['agi_callerid'];
//
//      $ret = array('name'=>'', 'protocol'=>'', 'username'=>'', 'host'=>'', 'port'=>'');
//      $callerid = trim($callerid);
//
//      if($callerid{0} == '"' || $callerid{0} == "'")
//      {
//        $d = $callerid{0};
//        $callerid = explode($d, substr($callerid, 1));
//        $ret['name'] = array_shift($callerid);
//        $callerid = join($d, $callerid);
//      }
//
//      $callerid = explode('@', trim($callerid, '<> '));
//      $username  = explode(':', array_shift($callerid));
//      if(count($username) == 1)
//        $ret['username'] = $username[0];
//      else
//      {
//        $ret['protocol'] = array_shift($username);
//        $ret['username'] = join(':', $username);
//      }
//
//      $callerid = join('@', $callerid);
//      $host = explode(':', $callerid);
//      if(count($host) == 1)
//        $ret['host'] =  $host[0];
//      else
//      {
//        $ret['host'] = array_shift($host);
//        $ret['port'] = join(':', $host);
//      }
//
//      return $ret;
//    }
//
//
// Use festival to read text
//
// @example examples/dtmf.php Get DTMF tones from the user and say the digits
// @example examples/input.php Get text input from the user and say it back
// @example examples/ping.php Ping an IP address
//
// @link http://www.cstr.ed.ac.uk/projects/festival/
// @param string $text
// @param string $escape_digits
// @param integer $frequency - default 8000
//
int AGITool_text2wav(AGI_TOOLS *agi, AGI_CMD_RESULT *res, char *text, char *escape_digits, int frequency)
{
	int ret=-1;
	char fname[MAXPATH], command[MAXPATH*2];
	FILE *p;

	if(!strlen(text)) return 1;
	if (frequency <= 0) frequency=8000;
	
	// create the wave file
	snprintf(fname,sizeof(fname),"%s%ctxt2wav_%s", AGITool_ListGetVal(agi->settings,"tmpdir"), DIRECTORY_SEPARATOR, AGITool_ListGetVal(agi->agi_vars,"agi_uniqueid"));

	snprintf(command,sizeof(command),"%s -F %d -o %s.wav", AGITool_ListGetVal(agi->settings,"festival_text2wave"), frequency, fname);

	p = popen(command, "w");
	if (p) {
		fputs(text, p);
		pclose(p);

		// stream it
		ret = AGITool_stream_file(agi,res,fname, escape_digits,0);

		// destroy it
		strcat(fname,".wav");
		if(FileExists(fname))
			unlink(fname);
	}
	
	
	return ret;
}

//   /**
//    * Use Cepstral Swift to read text
//    *
//    * @link http://www.cepstral.com/
//    * @param string $text
//    * @param string $escape_digits
//    * @param integer $frequency
//    * @return array, see evaluate for return information.
//    */
//    int AGITool_swift(AGI_TOOLS *tool, AGI_CMD_RESULT *res,$text, $escape_digits='', $frequency=8000, $voice=NULL)
//    {
//      $text = trim($text);
//      if($text == '') return true;
//
//      if(!is_null($voice))
//        $voice = "-n $voice";
//      elseif(isset($this->config['cepstral']['voice']))
//        $voice = "-n {$this->config['cepstral']['voice']}";
//
//      // create the wave file
//      $fname = $this->config['phpagi']['tempdir'] . DIRECTORY_SEPARATOR;
//      $fname .= str_replace('.', '_', 'swift_' . $this->request['agi_uniqueid']);
//      $p = popen("{$this->config['cepstral']['swift']} -p audio/channels=1,audio/sampling-rate=$frequency $voice -o $fname.wav -f -", 'w');
//      fputs($p, $text);
//      pclose($p);
//
//      // stream it
//      $ret = $this->stream_file($fname, $escape_digits);
//
//      // destroy it
//      if(file_exists($fname . '.wav'))
//        unlink($fname . '.wav');
//
//      return $ret;
//    }
//
//   /**
//    * Text Input
//    *
//    * Based on ideas found at http://www.voip-info.org/wiki-Asterisk+cmd+DTMFToText
//    *
//    * Example:
//    *              UC   H     LC   i      ,     SP   h     o      w    SP   a    r      e     SP   y      o      u     ?
//    *   $string = '*8'.'44*'.'*5'.'444*'.'00*'.'0*'.'44*'.'666*'.'9*'.'0*'.'2*'.'777*'.'33*'.'0*'.'999*'.'666*'.'88*'.'0000*';
//    *
//    * @link http://www.voip-info.org/wiki-Asterisk+cmd+DTMFToText
//    * @example examples/input.php Get text input from the user and say it back
//    *
//    * @return string
//    */
//    int AGITool_text_input(AGI_TOOLS *tool, AGI_CMD_RESULT *res,$mode='NUMERIC')
//    {
//      $alpha = array( 'k0'=>' ', 'k00'=>',', 'k000'=>'.', 'k0000'=>'?', 'k00000'=>'0',
//                      'k1'=>'!', 'k11'=>':', 'k111'=>';', 'k1111'=>'#', 'k11111'=>'1',
//                      'k2'=>'A', 'k22'=>'B', 'k222'=>'C', 'k2222'=>'2',
//                      'k3'=>'D', 'k33'=>'E', 'k333'=>'F', 'k3333'=>'3',
//                      'k4'=>'G', 'k44'=>'H', 'k444'=>'I', 'k4444'=>'4',
//                      'k5'=>'J', 'k55'=>'K', 'k555'=>'L', 'k5555'=>'5',
//                      'k6'=>'M', 'k66'=>'N', 'k666'=>'O', 'k6666'=>'6',
//                      'k7'=>'P', 'k77'=>'Q', 'k777'=>'R', 'k7777'=>'S', 'k77777'=>'7',
//                      'k8'=>'T', 'k88'=>'U', 'k888'=>'V', 'k8888'=>'8',
//                      'k9'=>'W', 'k99'=>'X', 'k999'=>'Y', 'k9999'=>'Z', 'k99999'=>'9');
//      $symbol = array('k0'=>'=',
//                      'k1'=>'<', 'k11'=>'(', 'k111'=>'[', 'k1111'=>'{', 'k11111'=>'1',
//                      'k2'=>'@', 'k22'=>'$', 'k222'=>'&', 'k2222'=>'%', 'k22222'=>'2',
//                      'k3'=>'>', 'k33'=>')', 'k333'=>']', 'k3333'=>'}', 'k33333'=>'3',
//                      'k4'=>'+', 'k44'=>'-', 'k444'=>'*', 'k4444'=>'/', 'k44444'=>'4',
//                      'k5'=>"'", 'k55'=>'`', 'k555'=>'5',
//                      'k6'=>'"', 'k66'=>'6',
//                      'k7'=>'^', 'k77'=>'7',
//                      'k8'=>"\\",'k88'=>'|', 'k888'=>'8',
//                      'k9'=>'_', 'k99'=>'~', 'k999'=>'9');
//      $text = '';
//      do
//      {
//        $command = false;
//        $result = $this->get_data('beep');
//        foreach(explode('*', $result['result']) as $code)
//        {
//          if($command)
//          {
//            switch($code{0})
//            {
//              case '2': $text = substr($text, 0, strlen($text) - 1); break; // backspace
//              case '5': $mode = 'LOWERCASE'; break;
//              case '6': $mode = 'NUMERIC'; break;
//              case '7': $mode = 'SYMBOL'; break;
//              case '8': $mode = 'UPPERCASE'; break;
//              case '9': $text = explode(' ', $text); unset($text[count($text)-1]); $text = join(' ', $text); break; // backspace a word
//            }
//            $code = substr($code, 1);
//            $command = false;
//          }
//          if($code == '')
//            $command = true;
//          elseif($mode == 'NUMERIC')
//            $text .= $code;
//          elseif($mode == 'UPPERCASE' && isset($alpha['k'.$code]))
//            $text .= $alpha['k'.$code];
//          elseif($mode == 'LOWERCASE' && isset($alpha['k'.$code]))
//            $text .= strtolower($alpha['k'.$code]);
//          elseif($mode == 'SYMBOL' && isset($symbol['k'.$code]))
//            $text .= $symbol['k'.$code];
//        }
//        $this->say_punctuation($text);
//      } while(substr($result['result'], -2) == '**');
//      return $text;
//    }
//
//   /**
//    * Say Puncutation in a string
//    *
//    * @param string $text
//    * @param string $escape_digits
//    * @param integer $frequency
//    * @return array, see evaluate for return information.
//    */
//    int AGITool_say_punctuation(AGI_TOOLS *tool, AGI_CMD_RESULT *res,$text, $escape_digits='', $frequency=8000)
//    {
//      for($i = 0; $i < strlen($text); $i++)
//      {
//        switch($text{$i})
//        {
//          case ' ': $ret .= 'SPACE ';
//          case ',': $ret .= 'COMMA '; break;
//          case '.': $ret .= 'PERIOD '; break;
//          case '?': $ret .= 'QUESTION MARK '; break;
//          case '!': $ret .= 'EXPLANATION POINT '; break;
//          case ':': $ret .= 'COLON '; break;
//          case ';': $ret .= 'SEMICOLON '; break;
//          case '#': $ret .= 'POUND '; break;
//          case '=': $ret .= 'EQUALS '; break;
//          case '<': $ret .= 'LESS THAN '; break;
//          case '(': $ret .= 'LEFT PARENTHESIS '; break;
//          case '[': $ret .= 'LEFT BRACKET '; break;
//          case '{': $ret .= 'LEFT BRACE '; break;
//          case '@': $ret .= 'AT '; break;
//          case '$': $ret .= 'DOLLAR SIGN '; break;
//          case '&': $ret .= 'AMPERSAND '; break;
//          case '%': $ret .= 'PERCENT '; break;
//          case '>': $ret .= 'GREATER THAN '; break;
//          case ')': $ret .= 'RIGHT PARENTHESIS '; break;
//          case ']': $ret .= 'RIGHT BRACKET '; break;
//          case '}': $ret .= 'RIGHT BRACE '; break;
//          case '+': $ret .= 'PLUS '; break;
//          case '-': $ret .= 'MINUS '; break;
//          case '*': $ret .= 'ASTERISK '; break;
//          case '/': $ret .= 'SLASH '; break;
//          case "'": $ret .= 'SINGLE QUOTE '; break;
//          case '`': $ret .= 'BACK TICK '; break;
//          case '"': $ret .= 'QUOTE '; break;
//          case '^': $ret .= 'CAROT '; break;
//          case "\\": $ret .= 'BACK SLASH '; break;
//          case '|': $ret .= 'BAR '; break;
//          case '_': $ret .= 'UNDERSCORE '; break;
//          case '~': $ret .= 'TILDE '; break;
//          default: $ret .= $text{$i} . ' '; break;
//        }
//      }
//      return $this->text2wav($ret, $escape_digits, $frequency);
//    }



