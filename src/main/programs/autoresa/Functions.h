#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <ctime>
#include <sstream>
#include <set>
#include <cmath>
#include <vector>

extern "C"
{
	#include "09_agi_client/cagi.h"
}

#include "08_acapela_client/PlaybackAcapela.h"
#include "01_util/XmlToolkit.h"
#include "30_server/BasicClient.h"
#include "30_server/LoginAction.h"
#include "30_server/ActionFunctionRequest.h"
#include "11_interfaces/SimplePageRequest.h"
#include "01_util/Conversion.h"
#include "30_server/QueryString.h"

using namespace synthese::server;
using namespace synthese::util;
using namespace synthese::util::XmlToolkit;
using namespace synthese::interfaces;
using namespace std;

class Functions
{
	public:
		// call implicitly by readKey to prepare the filename
		static string text2Voice(string _text);
		static bool validateInput(int *_menuKey,int _nMenuKey, int _inputKey);
		
		// call directly
		static int readKey(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res,int* _menuKey, int _nMenuKey, int _nKey, string _menu, int tryTime=0);
		// call directly
		static int playbackText(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res, string _msg);
		
		// functions expt are called implicitly by translateExpt
		static void exptMsgDtmf();
		static void exptMsgTimeout();
		static void exptMsgRmtFailed();
		
		// call directly
		static void setFatalError(string _fatalError);
		// call directly
		static string getFatalError();
		
		// call directly but 1 time
		static void setLanguage(int _lang);
		static int getLanguage();
		
		// call directly
		static void translateExpt(int _n);
		// call directly
		static string getMenu(int _category, int _step);
		// call directly or implicitly
		static int passToManuel(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res, char* callId);
		
		// call BasicClient of Synthese
		static string makeRequest(string _request) throw (int);
	
		
	private:
		static string fatalError;
		static int language;
	

};

/**
; the return variables:
;       @sessionId: the session number, if is null, no need look others
;       @type: 1 usr, 0 driver
        @name: the user name
;       @totalResa: the numeber means the total resa done
        @driverTotalResa: the total resa for the driver if type=0
;       @message: the eventual message for the custumer
*/

class SessionReturnType
{
	public:
		string sessionId;
		int type;
		string name;
		int totalResa;
		int driverTotalResa;
		string message;
		string callerId;
		ActionFunctionRequest<LoginAction,SimplePageRequest> loginRequest;
		
};
#endif

