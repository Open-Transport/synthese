#ifndef SEARCH_H
#define SEARCH_H

#include "Functions.h"


using namespace std;

class Search
{
	public:
		Search(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res);
		~Search();
		
		int start(string _fatalError, SessionReturnType *_session);
		int getChoicedFavorisTrajet();
		int getChoicedTimeOfChoicedTrip();
		
		int getFavorisFromSynthese() throw (int);
		int searchFromSynthese(int _favoris, tm _dataTime) throw (int);
		int searchFromSynthese(string _orignal, string _destination, tm _dataTime) throw (int);
		
		
	private:
		// common variables
		AGI_TOOLS *agi;
		AGI_CMD_RESULT *res;
		int dtmfInput;
		SessionReturnType *session;
		int *menuKey;
		
		// local variables
		string favorisTrip;
		int nFavorisTrip;

		string timeOfChoicedTrip;
		int nTimeOfChoicedTrip;
		
		int choicedFavorisTrajet;
			// the current choice number of the search given
		int choicedTimeOfChoicedTrip;
		
		tm dateTime;

};
#endif


