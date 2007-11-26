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
string Functions::text2Voice(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res,string _text)
{
	/*
	cerr<<"do vocal message: "<<_text<<endl<<endl;

	string fileName="/usr/share/asterisk/agi-bin/";
	string temp;
	
	for(int i=0;i<_text.size()%10;)
	{
		do
		{
			temp=_text.at(i);
			i++;
		}
		while((temp.c_str()<="a")||(temp.c_str()>="z"));
		fileName+=temp;
	}
	
	FILE *pFile;
	pFile = fopen (fileName.c_str(),"r");
	if (pFile!=NULL)
	{
		
		cerr<<fileName+".au"<<" exist already"<<endl;
		return fileName;
	}
	else
	{
		// file name without extension like au

		cerr<<"do voice file: "<<fileName+".au"<<endl<<endl;
		PlaybackAcapela::mainFunc(_text,fileName+".au","10.12.155.36","claire22s");
		 return fileName;
	}
	*/
	
	cerr<<"do vocal message: "<<_text<<endl<<endl;

	string fileName="/usr/share/asterisk/agi-bin/fileName";
	
	// file name without extension like au

	cerr<<"do voice file: "<<fileName+".raw"<<endl<<endl;
	PlaybackAcapela::mainFunc(_text,fileName+".raw","192.168.100.1","claire22s");
	cerr<<system("sox -r 22060 -s -w /usr/share/asterisk/agi-bin/fileName.raw /usr/share/asterisk/agi-bin/wavFileName.wav")<<endl;
	cerr<<system("sox /usr/share/asterisk/agi-bin/wavFileName.wav -r 8000 /usr/share/asterisk/agi-bin/gsmFileName.gsm resample -ql")<<endl;
	
	fileName="/usr/share/asterisk/agi-bin/gsmFileName";
	return fileName;

}

/*
	 to verify the input validate, called by readKey
**/
bool Functions::validateInput(int *_menuKey,int _nMenuKey, int _inputKey)
{
	int i=0;
	while(i<_nMenuKey)
	{
		if(_menuKey[i]==_inputKey)
		{
			cerr<<"input key is valable"<<endl;
			return true;
		}
		else i++;
	}
	cerr<<"input key is NOT valable"<<endl;
	return false;
}

/*
this function is to play a background message, to read the keyboard dtmf input and to return this input as one int
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
int Functions::readKey(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res,int* _menuKey, int _nMenuKey, int _nKey, string _menu, int tryTime=0)
{
	if(tryTime>2)
	{
		playbackText(_agi,_res,"désolé, veuillez renouveler votre appel. Au revoir.");
		return -1;
	}
	else if(tryTime>0)
	{
		playbackText(_agi,_res,"l\'entrée invalide, veuillez reessayer.");
	}
	
	cerr<<"MenuKey autorised: ";
	for(int i=0;i<_nMenuKey;i++) cerr<<_menuKey[i]<<" ";
	cerr<<endl;
	
	int inputKey=0;
	// to play background message
	string fileName=text2Voice(_agi,_res,_menu);

	// clic * to stop input
	int timeout=0;
	if(_nMenuKey==1) timeout=8000;
	else timeout=(int)(ceil(_nKey*1.5))*1000;
	
	inputKey=AGITool_get_data(_agi,_res,fileName.c_str(),timeout, _nKey);
	
	if((_nKey==1)&&(inputKey==0))
		//Functions::passToManuel(_agi,_res,getCallerId(_agi,_res));
		 return readKey(_agi,_res,_menuKey,_nMenuKey,_nKey,_menu,++tryTime);
	
	cerr<<"inputKey: "<<inputKey<<endl;
	
	if(_nMenuKey==0)
		return inputKey;
	else if(validateInput(_menuKey,_nMenuKey,inputKey)) return inputKey;
		else
			return readKey(_agi,_res,_menuKey,_nMenuKey,_nKey,_menu,++tryTime);

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
	cerr<<"playbackText called for "<<_msg<<endl;
	return AGITool_stream_file(_agi, _res, const_cast<char *>(text2Voice(_agi,_res,_msg).c_str()), "", 0);
}

/*
	the function is called when the dtmf error producted
	@parameters: void
	@return: void	
**/
void Functions::exptMsgDtmf()
{
	cerr<<"dtmf malformed error";
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
	cerr<<"timeout exceeded";
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
	cerr<<"remote server failed";
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
	cerr<<"Exception cached"<<endl;
	switch(_n)
	{
		case 0: temp="normal termine";
				return;

		/*the pair numbers are for the serious error*/
		case 2:	// for manuel reception
				setFatalError("warning: pass to operator");
				cerr<<fatalError<<endl;
				break;
		case 4: // remote connection failed
				setFatalError("error: remote server or connection error");
				cerr<<fatalError<<endl;
				exptMsgRmtFailed();
				break;
				
		case 6: // dtmf malformed
				setFatalError("error: dtmf malformed");
				cerr<<fatalError<<endl;
				exptMsgDtmf();
				break;
				
		case 8: // absolut timeout error
				setFatalError("error: absolut timeout exceeded");
				cerr<<fatalError<<endl;
				exptMsgTimeout();
		
				break;
				
		default: // unknown error
				cerr<<"unknown error raised as Nr. "<<_n<<endl;
				//exit(-1);
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
int Functions::passToManuel(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res, string callId)
{
	cerr<<"pass to manuel called"<<endl;
	// no need to listen the promo one more time, so jump to 4
	playbackText(_agi,_res,"Veuillez patienter. Le système vous transfert au centre de reservation");
	char *ext="75";
	char *pri="4";
	AGITool_exec_goto(_agi, _res, "tad", ext, pri);
	fatalError="Warning: custumer pass to centre";
	exit(1);
}

/*

**/
string Functions::makeRequest(string _request) throw (int)
{
	cerr<<"REQUEST: "<<_request<<endl;
	synthese::server::BasicClient *basicClient=new synthese::server::BasicClient("localhost",3591);
	//synthese::server::BasicClient *basicClient=new synthese::server::BasicClient("81.63.140.157",3593);
	
	std::stringstream out;
	basicClient->request(out,_request);
	
	delete basicClient;
	
	cerr<<"RETURN: "<<out.str() <<endl;
	
	return out.str();
	
}

string Functions::getCallerId(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res)
{
	char number[20];
	AGITool_get_variable2(_agi,_res, "CALLERID(NUMBER)", number, 20);
	cerr<<"caller Id: "<<number<<endl;
	return number;
}


/*

**/
static string Functions::smartXmlParser(string xml, string nodeName)
{
	// do xml parser
	XMLNode xmlNode=synthese::util::XmlToolkit::ParseString(xml, nodeName);
	cerr<<"xml Node: "<<xmlNode.getName()<<endl;
	xmlNode=synthese::util::XmlToolkit::ParseString(xml, xmlNode.getName());
		
	string msg;
	int i=0;
	
	try
	{
		xmlNode=xmlNode.getChildNode(nodeName.c_str());
		if(xmlNode.nText()>0)
		{
			msg=xmlNode.getText();
		}
	}
	catch (std::exception e)
	{
		//cerr<<e;
	}
	
	return msg;
}

