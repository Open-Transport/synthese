#include "Search.h"

Search::Search(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res)
{
	agi=_agi;
	res=_res;
}

Search::~Search()
{
	

}
/*
	this function is the principal to begin the procedure of the initinairy search
	@parameter:
		string: fatalError
	@return:	
		int:  to signe the next step,
			1: Reservation
			-1: if fatalError raised
			else to operator
			notice: if the custumer want to finish,
			the function will set fatalError to skip other procedures
**/
int Search::start(SessionReturnType *_session)
{
	session=_session;
	if(_session->sessionId.empty()) Functions::setFatalError("without session id in Search processus");
	
	if(Functions::getFatalError().empty())
	{
		try
		{
			Functions::playbackText(agi,res,"veuillez patienter.");
			int menuKey[12];
			for(int i=0;i<=session->favoris.size();i++) menuKey[i]=i;
			choicedFavorisTrajet=Functions::readKey(agi,res,menuKey,session->favoris.size()+1,1,session->favorisSentence);
		}
		catch(int e)
		{
			Functions::translateExpt(e);
			return -1;
		}
		
		if(session->favoris.size()!=0)	// with favoris
		{

			// request to the synthese for the time table of the favoris trip choiced
			try
			{
				sentence=searchFromSynthese(choicedFavorisTrajet);
			}
			catch(int e)
			{
				Functions::translateExpt(e);
				return -1;
			}

			// take the choice];
			int menuKey[12];
			for(int i=0;i<=session->solutionVector.size();i++) menuKey[i]=i;
			if(session->solutionVector.size()==0)  // no solution
			{
				Functions::playbackText(agi,res,"d�sol�, aucune solution trouv�e, veuiller verifier la validation des donn�es entr�es.");
				return start(_session);
			}
			else
			{
				do
				{
					Functions::playbackText(agi,res,"veuillez patienter.");
					choicedFavorisTrajet=Functions::readKey(agi,res,menuKey,session->solutionVector.size()+1,1,sentence);
				}
				while(!session->solutionVector.at(choicedFavorisTrajet-1).reservation);
			}
			
			
			return 1;		// Ok

		}
		// without favoris, ask origne & dest first
		// T9 input, will be realized after Nov.
		else  
		{
			// do nothing before Nov.
			return 0;
		}
	
	}
	else
	{
		// do noting normally bcz fatalError raised
		cerr<<Functions::getFatalError()<<endl;
		return -1;
	}
}

int Search::getChoicedFavorisTrajet()
{
	return choicedFavorisTrajet;
}
int Search::getRankOfChoicedTrajet()
{
	return session->solutionVector.at(choicedFavorisTrajet-1).rank;
}


/*
	function is to get how many favoris for this session from Synthese
	@parameters:
	@return:
		int: 0 ok, else error
**/
int Search::getFavorisFromSynthese() throw (int)
{
	// update favorisTrip
}

string Search::readDateTime()
{
	synthese::time::Date *dateTime=new synthese::time::Date(synthese::time::TIME_CURRENT);
	// change da=yyyy-mm-dd hh:mm:ss 2007-10-20 18:50, by default: A means Actuel
	//string dt="2007-10-20 18:50";
	int menuKey[]={1,2,3};
	dtmfInput=Functions::readKey(agi,res,menuKey,3,1,"veuillez entrer, 1, pour une recherche en temps actuel. presser 2 pour modifier l\'heure du trajet cherch�. presser 3 pour modifier la date et l\'heure du trajet cherch�.");
	if(dtmfInput==1) return "A";
	if(dtmfInput==2)
	{
		dtmfInput=Functions::readKey(agi,res,menuKey,0,4,"veuillez entrer l\'heure de recheche en 4 chiffres. par exemple: 1 2 3 5, signifient 12:35. ou presser 4 fois 0 pour revenir au menu sup�rieur.");
		if(dtmfInput==0) return readDateTime();
		//string dt="2007-10-20 18:50";
		else
		{
			string dt=synthese::util::Conversion::ToString(dtmfInput);
			
			if(dt.size()<4)
			{
				string zeros;
				for(int i=0;i<4-dt.size();i++) zeros+="0";
				cerr<<"no enough number detected, "<<zeros<<" will be added"<<endl;
				dt.insert(0,zeros);
			}
			
			// dt format orignal: 08350310 , il faut 03-10 08:35
			string hour=dt.substr(0,2); cerr<<"hour input: "<<hour<<", ";
			string min=dt.substr(2,2); cerr<<"min input: "<<min<<", ";
			string date=synthese::util::Conversion::ToString(dateTime->getDay()); cerr<<"date input: "<<date<<", ";
			string month=synthese::util::Conversion::ToString(dateTime->getMonth()); cerr<<"month input: "<<month<<", "; cerr<<endl;
			
			string year=synthese::util::Conversion::ToString(dateTime->getYear());
			dt=year+"-"+month+"-"+date+" "+hour+":"+min;
			cerr<<"time organised: "<<dt<<endl;
			return dt;
		}
	}
	if(dtmfInput==3)
	{
		dtmfInput=Functions::readKey(agi,res,menuKey,0,8,"veuillez entrer la date et l\'heure de recheche en 8 chiffres. par exemple: 0 8 3 5 et 0 3 1 0, signifient 8:35 du 3 octobre. ou presser 8 fois 0 pour revenir au menu sup�rieur.");
		if(dtmfInput==0) return readDateTime();
		//string dt="2007-10-20 18:50"; the input format 08350310 so hhmmDDMM
		else
		{
			string dt=synthese::util::Conversion::ToString(dtmfInput);
			
			if(dt.size()<8)
			{
				string zeros;
				for(int i=0;i<8-dt.size();i++) zeros+="0";
				cerr<<"no enough number detected, "<<zeros<<" will be added"<<endl;
				dt.insert(0,zeros);
			}
			
			// dt format orignal: 08350310 , il faut 03-10 08:35
			string hour=dt.substr(0,2); cerr<<"hour input: "<<hour<<", ";
			string min=dt.substr(2,2); cerr<<"min input: "<<min<<", ";
			string date=dt.substr(4,2); cerr<<"date input: "<<date<<", ";
			string month=dt.substr(6,2); cerr<<"month input: "<<month<<", "; cerr<<endl;
			string year=synthese::util::Conversion::ToString(dateTime->getYear());
			dt=year+"-"+month+"-"+date+" "+hour+":"+min;
			cerr<<"time organised: "<<dt<<endl;
			return dt;
		}
	}
}

/*
	function to call Synthese and fill in the currrentSearchText and currentSearch
	@parameters:
		int: a favoris given
		tm: actual date and time
			tm structure as fellows:
				int tm_sec;
				int tm_min;
				int tm_hour;
				int tm_mday;
				int tm_mon;
				int tm_year;
				int tm_wday;
				int tm_yday;
				int tm_isdst;

	@return:
		int: 1 ok, 0 failed and raison saved in fatalError
**/
string Search::searchFromSynthese(int _favoris) throw (int)
{
		_favoris--;
		
		string dt=readDateTime();
		string req="fonction=rp&si=3&da="+dt+"&msn=3&dct="+session->favoris.at(_favoris).origin_city+"&dpt="+session->favoris.at(_favoris).origin_place+"&act="+session->favoris.at(_favoris).destination_city+"&apt="+session->favoris.at(_favoris).destination_place+"&ac=0";
				
		// valeur de retour � reflechir
		string xml=Functions::makeRequest(req);
		
		/*
		<solution
			rank="1"
			date="2007-11-03 19:38:00" 
			reservation="1"
			sentence="D?tail de votre trajet : Vous partez de City68 93.A 19:38, prenez la ligne 92 ? destination de City12 99.Descendez ? l\'arr?tCity12 97.A 20:11, prenez la ligne 94 ? destination de City68 93.Descendez ? l\'arr?tCity6 95.Vous arrivez ? City6 95 ? 20:15.Attention ! La r?servation est obligatoire pour pouvoir emprunter cette relation.Taper 1 pour r?server votre place." />
		*/
		session->solutionVector.clear();
		// prepare solutions
		int n=0;
		XMLNode xmlNode=synthese::util::XmlToolkit::ParseString(xml, "solutions");
		XMLNode xmlNodeChild=synthese::util::XmlToolkit::GetChildNode(xmlNode,"solution",0);
		
		bool stillValue=true;
		SessionReturnType::SolutionSt solutionSt;
		string place;
		
		while(!xmlNodeChild.isEmpty())
		{
			
			place="rank";
			solutionSt.rank=synthese::util::XmlToolkit::GetIntAttr(xmlNodeChild,place);;
			place="date";
			solutionSt.date=synthese::util::XmlToolkit::GetStringAttr(xmlNodeChild,place);
			place="reservation";
			solutionSt.reservation=synthese::util::XmlToolkit::GetIntAttr(xmlNodeChild,place);
			place="sentence";
			solutionSt.sentence=synthese::util::XmlToolkit::GetStringAttr(xmlNodeChild,place);
			
			cerr<<endl;
			cerr<<"solution rank: "<<solutionSt.rank<<endl;
			cerr<<"date: "<<solutionSt.date<<endl;
			cerr<<"reservation: "<<solutionSt.reservation<<endl;
			cerr<<"sentence: "<<solutionSt.sentence<<endl;
				
			session->solutionVector.push_back(solutionSt);
			xmlNodeChild=synthese::util::XmlToolkit::GetChildNode(xmlNode,"solution",++n);
		}
		
		return Functions::smartXmlParser(xml,"sentence");
	
	
}

