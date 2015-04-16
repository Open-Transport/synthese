#ifndef RESERVATION_H
#define RESERVATION_H

#include "Functions.h"
#include "BookReservationAction.h"

using namespace std;
using namespace synthese::security;
using namespace synthese::resa;

class Reservation
{
	public:
		Reservation(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res);
		~Reservation();

		int start(SessionReturnType *_session, int _tripChoiced, int _rankChoiced);

		int requestReservationToSynthese() throw (int);

	private:
		// common variables
		AGI_TOOLS *agi;
		AGI_CMD_RESULT *res;
		int dtmfInput;
		SessionReturnType *session;

		// local variables
		int tripChoiced;
		int rankChoiced;
};
#endif



