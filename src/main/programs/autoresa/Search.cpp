#include "Search.h"

Search::Search(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res)
{
	agi=_agi;
	res=_res;
	menuKey=new int[12];
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
			// get favoris trip from synthese
			nFavorisTrip=getFavorisFromSynthese();
		}
		catch(int e)
		{
			Functions::translateExpt(e);
			return -1;
		}
		
		if(nFavorisTrip!=0)	// with favoris
		{
			Functions::playbackText(agi,res,Functions::getMenu(2,1));
			
			for(int i=0;i<nFavorisTrip;i++) menuKey[i]=i;
			choicedFavorisTrajet=Functions::readKey(agi,res,menuKey,nFavorisTrip,1,favorisTrip);
			
			// request to the synthese for the time table of the favoris trip choiced
			try
			{
				nTimeOfChoicedTrip=searchFromSynthese(choicedFavorisTrajet,dateTime);
			}
			catch(int e)
			{
				Functions::translateExpt(e);
				return -1;
			}
			
			for(int i=0;i<nTimeOfChoicedTrip;i++) menuKey[i]=i;
			choicedTimeOfChoicedTrip=Functions::readKey(agi,res,menuKey,nTimeOfChoicedTrip,(int)(nTimeOfChoicedTrip/10)+1,timeOfChoicedTrip);
			
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
int Search::getChoicedTimeOfChoicedTrip()
{
	return choicedTimeOfChoicedTrip;
}


/*
	function is to get how many favoris for this session from Synthese
	@parameters:
	@return:
		int: quantity of favoris saved for this session
**/
int Search::getFavorisFromSynthese() throw (int)
{
	// ask with session->sessionId
	// update favorisTrip as favoris message
	// return quantity of favoris
	// for the moment
	return 5;
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
int Search::searchFromSynthese(int _favoris, tm _dataTime) throw (int)
{
	//(session->loginRequest);
	
	
	if(_dataTime.tm_year==0)
	{
		time_t rawtime;

		time ( &rawtime );
		//_dataTime = localtime ( &rawtime );
	}
	
	// update timeOfChoicedTrip for the favoris trajet text
	// return nTimeOfChoicedTrip max 3
	
	favorisTrip="favoris 1: Aigle pour Toulous, 2 Nyon pour Marseille.";
	nFavorisTrip=2;
	
	return 3;
	
}

int Search::searchFromSynthese(string _orignal, string _destination, tm _dataTime) throw (int)
{
	if(_dataTime.tm_year==0)
	{
		time_t rawtime;

		time ( &rawtime );
		//_dataTime = localtime ( &rawtime );
	}
	
	return 0;
}

