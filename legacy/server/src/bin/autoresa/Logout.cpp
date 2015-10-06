#include "Logout.h"

Logout::Logout(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res)
{
	agi=_agi;
	res=_res;
}

Logout::~Logout()
{

}

/*
	this function is to logout the session near the synthese
	@parameters:
		SessionReturnType*: the current session
	@return:
		int: 0 correct, -1 failed (no notice will be saved evenif the session logout failed)
**/
int Logout::start(string _fatalError, SessionReturnType *session)
{
	return 0;
}
