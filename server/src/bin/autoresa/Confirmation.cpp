#include "Confirmation.h"

Confirmation::Confirmation(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res)
{
	agi=_agi;
	res=_res;
}

Confirmation::~Confirmation()
{

}

int Confirmation::start(SessionReturnType *_session)
{
	session=_session;

	if(_session->sessionId.empty())
	{
		Functions::setFatalError("session Id invalid in Confirmation");
		return -1;
	}

	//!(Functions::getFatalError().empty()) inside, but temp true
	if(true)
	{
		bool driverWantSelfConfirm=false;

		if(session->type==0)	//driver
		{
			int menuKey[]={7,8};
			dtmfInput=Functions::readKey(agi,res,menuKey,2,1,Functions::getMenu(4,1));
			if(dtmfInput==7) driverWantSelfConfirm=true;
			else driverWantSelfConfirm=false;
		}

		try
		{
			requestResaConfirmedHistory(driverWantSelfConfirm);

		}
		catch(int e)
		{
			Functions::translateExpt(e);
			return -1;
		}

		stringstream msg;
		msg<<Functions::getMenu(4,1)<<history.size()<<Functions::getMenu(4,2);
		Functions::playbackText(agi,res,msg.str());

		int menuKey[]={1,2,3,4,5,6,9};
		int currentStep=0;

		/*
			key notice: 	1,2,3 resa history number
					4 previous, 5 repeat, 6 next
					9: next 3 resa if any
					0: exit consultation
		**/
		do
		{
			dtmfInput=Functions::readKey(agi,res,menuKey,8,1,Functions::getMenu(3,3));
			//cout<<"Noop keyread: "<<dtmfInput<<endl;
			switch(dtmfInput)
			{
				case 4:
					if(--currentStep<(int)history.size()) currentStep++;
					break;
				case 5:
					break;
				case 6:
					if(++currentStep>(int)history.size()-1) currentStep--;
					break;
				/*
				case 9:
					currentStep+=3;
					if(currentStep>(int)history.size()-1) currentStep=history.size()-1;
					break;
				*/
				case 9:
					currentStep=-99;
					break;
				default:
					// delete, control right
					if((session->type==1) || ((session->type=0)&&(driverWantSelfConfirm)))
					{
						int reply=-1;
						try
						{
							reply=deleteResaToSynthese(currentStep);
						}
						catch(int e)
						{
							Functions::translateExpt(e);
							return -1;
						}

						if(reply==0) Functions::playbackText(agi,res,Functions::getMenu(3,6));
						else Functions::playbackText(agi,res,Functions::getMenu(3,4));
					}
					else
					{
						Functions::playbackText(agi,res,Functions::getMenu(3,5));
					}

			}
			Functions::playbackText(agi,res,history[currentStep]);
		}
		while(currentStep!=-99);


		return 0;
	}
	else  // fatalError raised, do nothing
	{
		cerr<<Functions::getFatalError()<<endl;
		return -1;
	}


}
/*

**/
int Confirmation::requestResaConfirmedHistory(bool _driverWantSelfConfirm) throw (int)
{
	// update history
	history.push_back("la reservation effectué: 1,aujourd\'hui, depuis l'aigle pour Toulouse à 19h35.");
	history.push_back("la reservation effectué: 2, le jeudi 6 décemdre, depuis Morteau pour Paris à 4h23.");
	return 0;
}
/*
	@return 0 ok, 1 delete failed
**/
int Confirmation::deleteResaToSynthese(int _n) throw (int)
{
	return 0;
}
