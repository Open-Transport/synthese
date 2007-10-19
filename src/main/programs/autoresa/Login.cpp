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
		// menu bienvenu, waiting for usr and psw
		Functions::playbackText(agi,res,Functions::getMenu(1,1));
		
		int usr=Functions::readKey(agi,res,menuKey,0,9,Functions::getMenu(1,2));
		
		int psw=Functions::readKey(agi,res,menuKey,0,6,Functions::getMenu(1,3));
		
		try
		{
			session=identifyUser(usr, psw);
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
			menuKey[0]=0;
			dtmfInput=Functions::readKey(agi,res,menuKey,1,1,Functions::getMenu(1,7));
			if(dtmfInput==-1)
			{
				Functions::text2Voice(Functions::getMenu(1,8));
				return -1;
			}
			else
			{
				return dtmfInput;
			}
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
SessionReturnType* Login::identifyUser(int _usr, int _psw) throw (int)
{
	
	if((_usr==1234) && (_psw==1234))
	{
		SessionReturnType *session=new SessionReturnType;
		session->sessionId="123456789";
		session->type=1;
		session->name="Dupont";
		session->totalResa=2;
		session->driverTotalResa=0;
		session->message="hello";
		session->callerId="0225480668";
	}
	
	return session;
}

SessionReturnType* Login::getSession()
{
	return session;
}






