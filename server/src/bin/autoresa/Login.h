#ifndef LOGIN_H
#define LOGIN_H

#include "Functions.h"
#include "30_server/LoginAction.h"

#include "30_server/StaticActionFunctionRequest.h"
#include "11_interfaces/SimplePageRequest.h"

using namespace synthese::server;
using namespace synthese::interfaces;
using namespace std;


/**
; the return variables:
;       @sessionId: the session number, if is null, no need look others
;       @type: 1 usr, 0 driver
        @name: the user name
;       @totalResa: the numeber means the total resa done
        @driverTotalResa: the total resa for the driver if type=0
;       @message: the eventual message for the custumer
*/


class Login
{
	public:
		Login(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res);
		~Login();

		int start();
		bool identifyUser() throw (int);

		SessionReturnType* getSession();



	private:
		// common variables
		AGI_TOOLS *agi;
		AGI_CMD_RESULT *res;
		int dtmfInput;
		int *menuKey;


		// local variables
		SessionReturnType* session;


};
#endif

