#ifndef RESERVATION_H
#define RESERVATION_H

#include "Functions.h"
#include "31_resa/BookReservationAction.h"

using namespace std;
using namespace synthese::security;
using namespace synthese::resa;

class Reservation
{
	public:
		Reservation(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res);
		~Reservation();
		
		int start(string _fatalError,SessionReturnType *_session,int _currentSearch,int _currentChoice);
		
		int requestReservationToSynthese() throw (int);
		
	private:
		// common variables
		AGI_TOOLS *agi;
		AGI_CMD_RESULT *res;
		int dtmfInput;
		SessionReturnType *session;
		
		// local variables
		int *menuKey;
		int tripChoiced;
		int timeChoiced;
};
#endif



