#include "Functions.h"

int Functions::language;
string Functions::fatalError;

/*
the function is to translate text into voice message with
the cache system and interface with playbackAcapela
this function is called implicited by readKey and playbackText
@parameters:
	char* _text: text to translate
@return:
	char* : the file name without extension like .au
**/
string Functions::text2Voice(string _text)
{
  // file name without extension like au
  string fileName="/usr/share/asterisk/agi-bin/fileName";
  cout<<"Do voice file: "<<fileName+"au";
  PlaybackAcapela::mainFunc(_text,fileName+".au");
  return fileName;
}

/*
this function is to read a keyboard input in max length with background vocal of the text message given
@parameters:
	AGI_TOOLS*: agi handle
	AGI_CMD_RESULT*: agi result handle
	int*: defined autorised menu key
	int: how many menu key
	int: each key size, normally is 1 as 1 touche
	string: menu text
@return:
	int: the keyboard dtmf input
		if 0: goto operator
		if -1: input incorrect
		others: correct input
**/
int Functions::readKey(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res,int* _menuKey, int _nMenuKey, int _nKey, string _menu)
{
	char temp[4096];
	string option;
	string inputKey;  // take temp like string var
	int n=0;
	bool inputCorrect=false;
	int inputNo;
	
	string fileName=text2Voice(_menu);
	
	if(_menuKey==NULL || _nMenuKey==0)
	{
	
		while( (n<3) && (!inputCorrect) )
		{
		
			sprintf(temp,"usrInput,%s,%d,,1,%d",fileName.c_str(),_nKey,_nKey*5);
			AGITool_exec(_agi, _res, "Read", temp);
			if(AGITool_get_variable2(_agi, _res, "usrInput", temp, _nKey)==0)
			{
				inputKey=temp;
				cout<<"inputKey: "<<temp;

				if(inputKey.empty()||inputKey.compare("0"))  // if input #,then return null, so goto manuel
				{
					return 0;
				}
				
				istringstream iss(inputKey,istringstream::in);
				iss >> inputNo;
				
				int i=0;
				while( (inputNo!=_menuKey[i]) && (i<=_nMenuKey) ) i++;
				if(i<=_nMenuKey)	inputCorrect=true;	// input correct finded in menuKey
				
				n++;
			}
			else exit(-1);
		}
	}
	else  // no need the key verif
	{
		sprintf(temp,"usrInput,%s,%d,,1,%d",fileName.c_str(),_nKey,(int)(_nKey*1.5));
		AGITool_exec(_agi, _res, "read", temp);
		AGITool_get_variable2(_agi, _res, "usrInput", temp, sizeof(_nKey));
		
		inputKey=temp;
		istringstream iss(inputKey,istringstream::in);
		iss >> inputNo;
		inputCorrect=true;
	}

	if(inputCorrect) return inputNo;
	else return -1;
}

/*
	the function is to play the forground text message vocal
	@parameters:
		*AGI_TOOLS: the agi instance handle,
		*AGI_CMD_RESULT: the result handle,
		string: the message to playback
	@return:
		int: 0 normal, 1 anormal
**/
int Functions::playbackText(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res, string _msg)
{
	return AGITool_exec(_agi, _res, "Playback", const_cast<char *>(text2Voice(_msg).c_str()));
}

/*
	the function is called when the dtmf error producted
	@parameters: void
	@return: void	
**/
void Functions::exptMsgDtmf()
{
	cout<<"dtmf malformed error";
	// voice warning pass to the operator
	// write to log later with callerid
}
/*
	the function is called when the absolute timeout exceeded
	@parameters: void
	@return: void
**/
void Functions::exptMsgTimeout()
{
	cout<<"timeout exceeded";
	// voice warning, set fatalerror to cut system
	// write to log later with callerid
}

/*
	the function is called when the remote server connection failed,
	as network connection problem or the server is shutdown
	@parameters: void
	@return: void
**/
void Functions::exptMsgRmtFailed()
{
	cout<<"remote server failed";
	// voice warning, set fatalerror to cut system
	// write to log later with callerid 
}


void Functions::setFatalError(string _fatalError)
{
	fatalError=_fatalError;
}

string Functions::getFatalError()
{
	return fatalError;
}

void Functions::setLanguage(int _lang)
{
	language=_lang;
}

int Functions::getLanguage()
{
	return language;
}

void Functions::translateExpt(int _n)
{
	string temp;
	switch(_n)
	{
		case 0: temp="normal termine";
				return;

		/*the pair numbers are for the serious error*/
		case 2:	// for manuel reception
				setFatalError("warning: pass to operator");
				break;
		case 4: // remote connection failed
				setFatalError("error: remote server or connection error");
				exptMsgRmtFailed();
				break;
				
		case 6: // dtmf malformed
				setFatalError("error: dtmf malformed");
				exptMsgDtmf();
		
				break;
		case 8: // absolut timeout error
				setFatalError("error: absolut timeout exceeded");
				exptMsgTimeout();
		
				break;
				
		default: // unknown error
				cout<<"unknown error raised as Nr. "<<_n;
				exit(-1);
				break;
	}
	
}

/*
the function is to delivery the user to central
@parameter:
	AGI_TOOLS* : agi handle
	AGI_CMD_RESULT* : agi result handle
@return:
	int : system return 0 is correct
**/
int Functions::passToManuel(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res, char* callId)
{
	// no need to listen the promo one more time, so jump to 4
	char *ext="75";
	char *pri="4";
	AGITool_exec_goto(_agi, _res, "tad", ext, pri);
	fatalError="Warning: custumer pass to ";
	return 0;
}



















