#include "Reservation.h"

Reservation::Reservation(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res)
{
	agi=_agi;
	res=_res;
}

Reservation::~Reservation()
{

}

int Reservation::start(SessionReturnType *_session, int _tripChoiced, int _rankChoiced)
{
	session=_session;
	tripChoiced=_tripChoiced;
	rankChoiced=_rankChoiced;
	if(_session->sessionId.empty()) Functions::setFatalError("without session id in Reservation processus");

	if(Functions::getFatalError().empty())
	{
		Functions::playbackText(agi,res,"veuillez patienter.");
		int menuKey[]={0,1,3};
		dtmfInput=Functions::readKey(agi,res,menuKey,3,1,session->solutionVector.at(rankChoiced-1).sentence);

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
				return 3;	// will redo search
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
		string req="a=bra&fonction=page&i=4&nr=1&sid="
							+session->sessionId+
							"&page=resa_response&actionParamcuid="
							+session->userId+
							"&actionParamsit=3&actionParamacc=0&actionParamdct="
							+session->favoris.at(tripChoiced-1).origin_city+
							"&actionParamdpt="
							+session->favoris.at(tripChoiced-1).origin_place+
							"&actionParamact="
							+session->favoris.at(tripChoiced-1).destination_city+
							"&actionParamapt="
							+session->favoris.at(tripChoiced-1).destination_place+
							"&actionParamda="
							+session->solutionVector.at(rankChoiced).date+
							"&actionParamsenu=1&actionParamcuph="
							+session->registredPhone+
							"&actionParampass="
							+session->psw;

		cerr<<"request: "<< req<<endl;

		// valeur de retour à reflechir
		string xml=Functions::makeRequest(req);

		/*
		<reservation>
		<reservation>-1</reservation>
		<sentence>Attention ! La r?servation n'a pas ?t? effectu?e pour des raisons techniques. Veuillez presser 0 pour contacter le centre d'appels. Merci pour votre appel et au revoir. </sentence>
		</reservations>
		*/
		if(Functions::smartXmlParser(xml,"reservation")=="-1")
		{
			Functions::readKey(agi,res,new int,1,1,Functions::smartXmlParser(xml,"sentence"),10);
		}
		else
		{
			Functions::playbackText(agi,res,Functions::smartXmlParser(xml,"sentence"));
		}

	return 0;
}
