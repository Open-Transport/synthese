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
#include "01_util/XmlParser.h"
#include "01_util/XmlToolkit.h"
#include "30_server/BasicClient.h"
#include "30_server/LoginAction.h"
#include "30_server/StaticActionFunctionRequest.h"
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
		static unsigned int RSHash(const std::string& str);
		// call implicitly by readKey to prepare the filename
		static string text2Voice(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res,string _text);
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
		static int passToManuel(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res, string callId);

		// call BasicClient of Synthese
		static string makeRequest(string _request) throw (int);

		// return caller id
		static string getCallerId(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res);

		// smart xml parser
		static string smartXmlParser(string xml, string nodeName);

	private:
		static string fatalError;
		static int language;

		// envir. variables
		static string ipAcapela;
		static string ipSynthese3;
		static int portSynthese3;
		static string voiceChoiced;


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
		struct FavorisVectorStruct
		{
			int rank;
			string origin_city;
			string origin_place;
			string destination_city;
			string destination_place;
		};

		struct SolutionSt
		{
				int rank;
				string date;
				int reservation;
				string sentence;
		};


		string usr;
		string psw;
		string sessionId;
		int type;
		string name;
		string registredPhone;
		string userId;
		int totalResa;
		int driverTotalResa;
		string message;
		string callerId;
		string favorisSentence;
		vector<FavorisVectorStruct> favoris;
		StaticActionFunctionRequest<LoginAction,SimplePageRequest> loginRequest;
		vector<SolutionSt> solutionVector;


};

#endif

