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
int Login::start()
{
	int temp=0;
	
	if(Functions::getFatalError().empty())
	{
		int tryTime=0;
		session->callerId=Functions::getCallerId(agi,res);		
		try
		{
			while((!identifyUser())&&(tryTime<2))
			{
				temp=Functions::playbackText(agi,res,Functions::getMenu(1,7));
				cerr<<"identifyUser, try "<<tryTime<<" times"<<endl;
				tryTime++;
			}
			if(tryTime>2) Functions::setFatalError("usr login failed");
		}
		catch(int e)
		{
			Functions::translateExpt(e);
			return -1;
		}
		catch(string e)
		{
			cerr<<"unknown Exception catched "<<e<<endl;
			return 0;
		}		
		
		if((!session->sessionId.empty())&&(Functions::getFatalError().empty()))
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
		cerr<<"fatal error found in Login"<<endl;
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
	
	int usr,psw;

	usr=Functions::readKey(agi,res,menuKey,0,4,Functions::getMenu(1,2));
		
	psw=Functions::readKey(agi,res,menuKey,0,4,Functions::getMenu(1,3));
	
	//usr=1234; psw=1234;

	session->loginRequest.getAction()->setLogin(Conversion::ToString(usr));
	session->loginRequest.getAction()->setPassword(Conversion::ToString(psw));

	try
	{
		stringstream req;

		// interface 4
		req<<"ipaddr=0.0.0.0&a=login&fonction=page&page=ajax_login_response&i=2&actionParamlogin="<<usr<<"&actionParampwd="<<psw<<"&nr=1";

		cerr<<"request: "<< req.str() <<endl;
		
		
		// valeur de retour à reflechir
		string xml=Functions::makeRequest(req.str());
		//cerr<<"xml return"<<xml<<endl;		

		// do xml parser
		session->name=smartXmlParser(xml,"name");
		cerr<<"name: "<<session->name<<endl;
		session->sessionId=smartXmlParser(xml,"session");
		cerr<<"sessionId: "<<session->sessionId<<endl;
		session->type=1;
		
		
	}
	catch (int e)
	{
		Functions::translateExpt(e);
		return false;
	}
	catch (string e)
	{
		cerr<<"unknown Exception cached: "<<e<<endl;
	}
	
	if(session->sessionId.empty())
	{
	    return false;
	}
	else
	{
	    return true;
	}
}

SessionReturnType* Login::getSession()
{
	return session;
}

/*

**/
static string Login::smartXmlParser(string xml, string nodeName)
{
	// do xml parser
	XMLNode xmlNode=synthese::util::XmlToolkit::ParseString(xml, nodeName);
	cerr<<"xml Node: "<<xmlNode.getName()<<endl;
	xmlNode=synthese::util::XmlToolkit::ParseString(xml, xmlNode.getName());
	
	int i=0;
	string msg;
	msg=xmlNode.getChildNode(nodeName.c_str()).getText();

	return msg;
}


