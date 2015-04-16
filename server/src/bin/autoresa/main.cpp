#include "Functions.h"
#include "Login.h"
#include "Search.h"
#include "Reservation.h"
#include "Confirmation.h"
#include "Logout.h"


using namespace std;

int main(int argc, char *argv[])
{

	/*CAGI implementation*/
	AGI_TOOLS agi;
	AGI_CMD_RESULT res;
	AGITool_Init(&agi);
	int state=0;
	int stateRs=0;


	AGITool_answer(&agi, &res);
	cerr<<"main AutoResa"<<endl;

	//int menuKey[]={1,2,3,4,5};
	//Functions::setLanguage(Functions::readKey(&agi,&res,menuKey,5,1,Functions::getMenu(0,0)));
	Functions::setLanguage(1);

	// menu bienvenu, time to creat the objects
	Functions::playbackText(&agi,&res,"Bienvenu au système de reservation.");

	// prepare objects during the language
	Login *login=new Login(&agi,&res);
	Search *search=new Search(&agi,&res);
	Reservation *reservation=new Reservation(&agi,&res);
	Confirmation *confirmation=new Confirmation(&agi,&res);
	Logout *logout=new Logout(&agi,&res);

	state=login->start();
	cerr<<"login return: "<<state<<endl;
	/*
	* state is to signe the next step,
		7: FeedbackCst
		8: FeedbackDrv
		9: Search
		-1: interruption, stop system
		0 to operator
	**/
	switch(state)
	{
		case 7:
			cerr<<"jump to FeedbackCst"<<endl;
			cerr<<"Noop before confirmation, fatalError: "<<Functions::getFatalError()<<" sessionId: "<<login->getSession()<<endl;
			state=confirmation->start(login->getSession());
			cerr<<"state of confirmation: "<<state<<endl;
			break;

		case 8: cerr<<"jump to FeedbackDrv"<<endl;
			state=confirmation->start(login->getSession());
			break;
		case 9: cerr<<"jump to search"<<endl;
			do
			{
				state=search->start(login->getSession());
				switch(state)
				{
					case 1:
						stateRs=reservation->start(login->getSession(),search->getChoicedFavorisTrajet(),search->getRankOfChoicedTrajet());

						switch(stateRs)
						{
							case 1:
								cerr<<"reservation successful"<<endl;
								break;
							default:
								stateRs=3;
								break;
						}
						break;
					case -1:
						// do nothing, bcz fatalError raised
						break;
				}
			}
			while(stateRs==3);
			break;

		case -1:
			Functions::playbackText(&agi,&res,"Identification échuée. Veuillez renouveler votre appel.");
			cerr<<"interuption, system stopped"<<endl;
			break;
		default:
			cerr<<"state return: "<<state<<endl;
			break;
	}


	//Functions::playbackText(&agi,&res,Functions::getMenu(0,1));

	sleep(2);
	AGITool_Destroy(&agi);
	logout->start(Functions::getFatalError(),login->getSession());


	delete login;
	delete search;
	delete reservation;
	delete confirmation;
	delete logout;

	return 0;
}







