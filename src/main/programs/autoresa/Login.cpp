#include "Login.h"

Login::Login(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res)
{
	agi=_agi;
	res=_res;
	session=new SessionReturnType;
	menuKey=new int[12];
}

Login::~Login()
{
	delete session;
	// agi and res donot need to be deleted here, will be done in main
}

/*
	this is a principal functione to start login procedure
	@parameters:
		string: the fatalError
	@return:
		int:  to signe the next step,
			7: FeedbackCst
			8: FeedbackDrv
			9: Search
			-1: interruption, stop system
			else to operator
**/
int Login::start(string _fatalError)
{
	if(_fatalError.empty())
	{
		int tryTime=0;

		try
		{
			while((!identifyUser())&&(tryTime<2))
			{
				Functions::playbackText(agi,res,Functions::getMenu(1,7));
				cerr<<"identifyUser, try "<<tryTime<<" times"<<endl;
				tryTime++;
			}
		}
		catch(int e)
		{
			Functions::translateExpt(e);
			return -1;
		}

		if(!session->sessionId.empty())
		{
			if(!session->message.empty()) Functions::text2Voice(session->message);
			
			switch(session->type)
			{
				case 0:
					// driver, so 2 possibilities, resa for self or resa by custumers
					// so do choice before say how many reservations
					menuKey[0]=8;
					menuKey[1]=9;
					dtmfInput=Functions::readKey(agi,res,menuKey,2,1,Functions::getMenu(1,4));
					break;
				case 1:
					menuKey[0]=7;
					menuKey[1]=9;
					dtmfInput=Functions::readKey(agi,res,menuKey,2,1,Functions::getMenu(1,6));
					break;
			}
			return dtmfInput;
		}
		else  // session id null
		{
			Functions::setFatalError("seesion id null in Login");
			cerr<<Functions::getFatalError()<<endl;
			return -1;
		}
	}
	else  // if fatalError producted
	{
		// do nothing for the moment
		return 0;
	}
	
}



/*
this function is to identify the user name and passwor near the Synthese
@parameters:
	char* _usr: username
	char* _psw: password
@return:
	sessionReturnType: the session return struct
**/
bool Login::identifyUser() throw (int)
{
	
	int usr=Functions::readKey(agi,res,menuKey,0,4,Functions::getMenu(1,2));
		
	int psw=Functions::readKey(agi,res,menuKey,0,4,Functions::getMenu(1,3));

	session->loginRequest.getAction()->setLogin(Conversion::ToString(usr));
	session->loginRequest.getAction()->setPassword(Conversion::ToString(psw));

	try
	{
		string req;
		//string req=session->loginRequest.getQueryString().getContent();
		if(req.empty()) req="a=login&fonction=admin&i=2&roid=-1&rub=home&sid=&actionParamlogin=1234&actionParampwd=1234";
		cerr<<"request: "<<req<<endl;
		
		/*
		// valeur de retour à reflechir
		string xml=Functions::makeRequest(req);
		XMLNode xmlNode=synthese::util::XmlToolkit::ParseString(xml, "session");
		cerr<<"xmlNode: "<<synthese::util::XmlToolkit::CheckForRequiredAttr(xmlNode,"session")<<endl;
		session->sessionId=synthese::util::XmlToolkit::GetStringAttr(xmlNode, "");
		cout<<"Noop sessionId: "<<session->sessionId<<endl;
		
		*/
	}
	catch (int e)
	{
		Functions::translateExpt(e);
		return false;
	}
	
	session->sessionId="1234";
	
	return true;
}

SessionReturnType* Login::getSession()
{
	return session;
}

