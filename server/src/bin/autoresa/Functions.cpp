#include "Functions.h"

int Functions::language;
string Functions::fatalError;

string  Functions::ipAcapela="10.12.155.55";  // idefix
string  Functions::ipSynthese3="10.12.155.55";  // idefix
int  Functions::portSynthese3=3591;
string  Functions::voiceChoiced="claire8kmu";


unsigned int Functions::RSHash(const std::string& str)
{
   unsigned int b    = 378551;
   unsigned int a    = 63689;
   unsigned int hash = 0;

   for(std::size_t i = 0; i < str.length(); i++)
   {
      hash = hash * a + str[i];
      a    = a * b;
   }

   return hash;
}

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
	/* Cache Management Mecanism
	// struct in DB t800_cache_system
	// words : string (key) , fileName : string , filePath : string , creatDateTime : date , lastAcessDateTime : date , category : number (p.ex: 0 music, 1 file etc)
	DB* db = DBModule::GetDB();
	stringstream query;
	query
		<< "SELECT *"
		<< " FROM " << " t800_cache_system "
		<< " WHERE " << TABLE_COL_LOGIN << "=" << Conversion::ToDBString(login);
	try
	{
		db::DBResultSPtr rows = db->execQuery(query.str());
		if (rows->next () == false)
			throw UserTableSyncException("User "+ login + " not found in database.");

		shared_ptr<User> user (new User);
		load(user.get(), rows);
		link(user.get(), rows, GET_AUTO);
		return user;
	}
	catch (DBException e)
	{
		throw UserTableSyncException(e.getMessage());
	}
	*/
	cerr<<"do vocal message: "<<_text<<endl<<endl;

	string spath="/usr/share/asterisk/agi-bin/resaVoice/";
	string fileName=synthese::util::Conversion::ToString(RSHash(_text));
	//string fileTemp=spath+"temp/"+fileName;
	fileName=spath+fileName;

	//cerr<<" FileName: "<<fileName<<" and FileTemp: "<<fileTemp<<endl;
	cerr<<" FileName: "<<fileName<<endl;

	//string gsmFileName=fileName+".gsm";
	//cerr<<"gsmFileName: "<<gsmFileName<<endl;

	// search if file exist
	FILE * pFile;
	//pFile = fopen (gsmFileName.c_str(),"r");
	pFile = fopen((fileName+".au").c_str(),"r");
	if (pFile!=NULL)
	{
		// file exist
		fclose (pFile);
		cerr<<"file exist, no needs to remake"<<endl;
		return fileName;
	}
	else // without file, do it
	{
		// file name without extension like au
		//cerr<<"do voice file, bcz not exist: "<<fileTemp+".raw"<<endl<<endl;
		cerr<<"do voice file, bcz not exist: "<<fileName+".au"<<endl<<endl;
		//PlaybackAcapela::mainFunc(_text,fileTemp+".raw",ipAcapela,voiceChoiced);
		PlaybackAcapela::mainFunc(_text,fileName+".au",ipAcapela,voiceChoiced);

		/*
		string cmdSox="sox -r 22060 -s -w  "+fileTemp+".raw "+fileTemp+".wav";
		cerr<<"command: "<<cmdSox.c_str()<<" , return: ";
		int returnVal=system(cmdSox.c_str());
		cerr<<returnVal<<endl;
		if(returnVal==0)
		{
			//sox /usr/share/asterisk/agi-bin/wavFileName.wav -r 8000 /usr/share/asterisk/agi-bin/gsmFileName.gsm resample -ql
			cmdSox="sox "+fileTemp+".wav -r 8000 "+gsmFileName+" resample -ql";
			cerr<<"command: "<<cmdSox.c_str()<<" , return: ";
			cerr<<system(cmdSox.c_str())<<endl;
		}
		else
		{
			cerr<<"system error, voice file is not created: "<<returnVal<<endl;
		}
		*/

		return fileName+".au";
	}




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
	else timeout=(int)(ceil(_nKey*1.5))*1000+_menu.size()*1000;

	if(_nKey==1)	// if one input, call stream_file function to diff 0 and *,#
	{
		char allkey[]={'0','1','2','3','4','5','6','7','8','9'};
		inputKey=AGITool_stream_file(_agi, _res,fileName.c_str(),allkey, 0)-48;
		cerr<<"stream_file called for one dtmf: "<<inputKey<<endl;
		if(inputKey==-48)
		{
			// timeout in millesec
			inputKey=AGITool_wait_for_digit(_agi, _res, timeout)-48;
			cerr<<"wait_digit called for one dtmf: "<<inputKey<<endl;
		}
		if(inputKey==-49)  // -1-48 channel failed
		{
			setFatalError("dtmf channel failed");
			cerr<<"dtmf channel failed, system halt"<<endl;
			exit(-1);
		}

	}
	else	// if many input, call get_data, diff is no more important
	{
		inputKey=AGITool_get_data(_agi,_res,fileName.c_str(),timeout, _nKey);
		cerr<<"get_data called for mtl. dtmf: "<<inputKey<<endl;
	}

	if((_nKey==1)&&(inputKey==0))
		Functions::passToManuel(_agi,_res,getCallerId(_agi,_res));
		//return readKey(_agi,_res,_menuKey,_nMenuKey,_nKey,_menu,++tryTime);

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
	playbackText(_agi,_res,"Veuillez patienter, Le système vous transfert au centre de reservation.");
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
	synthese::server::BasicClient *basicClient=new synthese::server::BasicClient(ipSynthese3,portSynthese3);
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

