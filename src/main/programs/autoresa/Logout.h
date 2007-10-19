#ifndef LOGOUT_H
#define LOGOUT_H

#include "Functions.h"

using namespace std;

class Logout
{
	public:
		Logout(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res);
		~Logout();
		
		int start(string _fatalError, SessionReturnType *session);
	
	private:
		AGI_TOOLS *agi;
		AGI_CMD_RESULT *res;
		
};
#endif



