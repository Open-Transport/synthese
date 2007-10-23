#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "08_acapela_client/PlaybackAcapela.h"

extern "C"
{
#include "09_agi_client/cagi.h"
}

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <ctime>
#include <sstream>
#include <set>



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
	
		
	private:
		static string fatalError;
		static int language;
	

};

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
};
#endif

