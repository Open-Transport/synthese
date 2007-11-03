#ifndef SEARCH_H
#define SEARCH_H

#include "vector"

#include "Functions.h"
#include "33_route_planner/RoutePlannerFunction.h"


using namespace std;
using namespace synthese::transportwebsite;
using namespace synthese::env;
using namespace synthese::routeplanner;

class Search
{
	public:
		Search(AGI_TOOLS *_agi, AGI_CMD_RESULT *_res);
		~Search();
		
		int start(SessionReturnType *_session);
		int getChoicedFavorisTrajet();
		int getRankOfChoicedTrajet();
		
		int getFavorisFromSynthese() throw (int);
		string searchFromSynthese(int _favoris) throw (int);
		
	private:
		// common variables
		AGI_TOOLS *agi;
		AGI_CMD_RESULT *res;
		int dtmfInput;
		SessionReturnType *session;
		int *menuKey;
		
		// local variables
		vector<string> favorisTrip;
		string sentence;

		string timeOfChoicedTrip;
		int nTimeOfChoicedTrip;
		
		int choicedFavorisTrajet;
			// the current choice number of the search given
		int choicedTimeOfChoicedTrip;
		
		tm dateTime;
		
		//ActionFunctionRequest<SearchRequest> searchRequest;
		
		
		
		
};
#endif


