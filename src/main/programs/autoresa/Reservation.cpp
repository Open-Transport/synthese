#include "Reservation.h"

Reservation::Reservation(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res)
{
	agi=_agi;
	res=_res;
	menuKey=new int[12];
}

Reservation::~Reservation()
{

}

int Reservation::start(SessionReturnType *_session, int _tripChoiced, int _timeChoiced)
{
	session=_session;
	tripChoiced=_tripChoiced;
	timeChoiced=_timeChoiced;
	if(_session->sessionId.empty()) Functions::setFatalError("without session id in Reservation processus");
	
	if(Functions::getFatalError().empty())
	{
		menuKey[0]=1;
		menuKey[1]=3;
		dtmfInput=Functions::readKey(agi,res,menuKey,2,1,Functions::getMenu(3,1));
		
		switch(dtmfInput)
		{
			case 1:	// yes
				try
				{
					requestReservationToSynthese();
					return 1;
				}
				catch(int e)
				{
					Functions::translateExpt(e);
					return -1;
				}
				Functions::playbackText(agi,res,Functions::getMenu(3,2));
				break;
			case 3:
				return 0;	// will redo search
				break;
			default:
				return 2;	// will transfer to operator
				break;
		}
	
	}
	else
	{
		// do noting bcz fatalError raised
		cerr<<Functions::getFatalError()<<endl;
		return -1;
	}

}

/*
	this function is to request the synthese a text string of the line given
	@parameters:
		string: sessionId
	@return:
		string: line info
		
**/
int Reservation::requestReservationToSynthese() throw (int)
{
	return 0;
}
