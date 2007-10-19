#ifndef CONFIRMATION_H
#define CONFIRMATION_H

#include "Functions.h"
#include <vector>

using namespace std;

/**

*/
class Confirmation
{
	public:
		Confirmation(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res);
		~Confirmation();
		
		int start(string _fatalError,SessionReturnType *_session);
		int requestResaConfirmedHistory(bool _driverWantSelfConfirm) throw (int);
		int deleteResaToSynthese(int _n) throw (int);
		
	private:
		// common variables
		AGI_TOOLS *agi;
		AGI_CMD_RESULT *res;
		int dtmfInput;

		// local variables
		SessionReturnType* session;
		vector<string> history;

};
#endif

