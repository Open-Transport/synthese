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
			if(tryTime>2)
			{
				Functions::setFatalError("usr login failed");


			}
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
			if(!session->message.empty()) Functions::playbackText(agi,res,session->message);
			/*
			switch(session->type)
			{
				case 0:
					// driver, so 2 possibilities, resa for self or resa by custumers
					// so do choice before say how many reservations
					menuKey[0]=8;
					menuKey[1]=9;
					dtmfInput=Functions::readKey(agi,res,menuKey,2,1,Functions::getMenu(1,4));
					break;
				default:
					menuKey[0]=7;
					menuKey[1]=9;
					dtmfInput=Functions::readKey(agi,res,menuKey,2,1,Functions::getMenu(1,6));
					break;
			}
			return dtmfInput;
			*/
			return 9;
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

	usr=Functions::readKey(agi,res,menuKey,0,4,"Veuillez entrer le numero d\'utilisateur.");

	psw=Functions::readKey(agi,res,menuKey,0,4,"Veuillez entrer votre mots de passe.");

	session->psw=synthese::util::Conversion::ToString(usr);
	session->psw=synthese::util::Conversion::ToString(psw);

	session->loginRequest.getAction()->setLogin(Conversion::ToString(usr));
	session->loginRequest.getAction()->setPassword(Conversion::ToString(psw));

	try
	{
		stringstream req;

		// interface 4
		req<<"ipaddr=0.0.0.0&a=login&fonction=page&page=login_response&i=4&actionParamlogin="<<usr<<"&actionParampwd="<<psw<<"&nr=1";


		// valeur de retour Ã  reflechir
		string xml=Functions::makeRequest(req.str());
		//cerr<<"xml return"<<xml<<endl;

		// do xml parser
		session->sessionId=Functions::smartXmlParser(xml,"session");
		cerr<<"sessionId: "<<session->sessionId<<endl;

		if(!session->sessionId.empty())
		{

			session->name=Functions::smartXmlParser(xml,"name");
			cerr<<"name: "<<session->name<<endl;

			session->userId=Functions::smartXmlParser(xml,"user_id");
			cerr<<"userId: "<<session->userId<<endl;
			//session->registredPhone=Functions::smartXmlParser(xml,"phone");
			//cerr<<"registred Phone: "<<session->registredPhone<<endl;
			session->favorisSentence=Functions::smartXmlParser(xml,"sentence");
			cerr<<"sentence: "<<session->favorisSentence<<endl;

			session->favoris.clear();
			/*
			<favorite rank="1" origin_city="TOULOUSE" origin_place="Capitole" destination_city="FLOURENS" destination_place="Mairie" />
			<favorite rank="2" origin_city="QUINT-FONSEGRIVES" origin_place="Mairie" destination_city="PIN-BALMA" destination_place="Pastoureau" />
			*/

			XMLNode xmlNode=synthese::util::XmlToolkit::ParseString(xml, "login");
			XMLNode xmlNodeChild;

			bool stillValue=true;
			SessionReturnType::FavorisVectorStruct favorisSt;
			string place;

			cerr<<xmlNode.nChildNode("favorite")<<" favoris finded"<<endl;

			for(int i=0;i<xmlNode.nChildNode("favorite");i++)
			{

				xmlNodeChild=synthese::util::XmlToolkit::GetChildNode(xmlNode,"favorite",i);
				place="rank";
				favorisSt.rank=synthese::util::XmlToolkit::GetIntAttr(xmlNodeChild,place);;
				place="origin_city";
				favorisSt.origin_city=synthese::util::XmlToolkit::GetStringAttr(xmlNodeChild,place);
				place="origin_place";
				favorisSt.origin_place=synthese::util::XmlToolkit::GetStringAttr(xmlNodeChild,place);
				place="destination_city";
				favorisSt.destination_city=synthese::util::XmlToolkit::GetStringAttr(xmlNodeChild,place);
				place="destination_place";
				favorisSt.destination_place=synthese::util::XmlToolkit::GetStringAttr(xmlNodeChild,place);

				cerr<<endl;
				cerr<<"favoris rank: "<<favorisSt.rank<<endl;
				cerr<<"origin_city: "<<favorisSt.origin_city<<endl;
				cerr<<"origin_place: "<<favorisSt.origin_place<<endl;
				cerr<<"destination_city: "<<favorisSt.destination_city<<endl;
				cerr<<"destination_place: "<<favorisSt.destination_place<<endl<<endl;

				session->favoris.push_back(favorisSt);
			}

			return true;
		}
		else
		{
				cerr<<"session id null"<<endl;
				return false;
		}

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

}

SessionReturnType* Login::getSession()
{
	return session;
}


