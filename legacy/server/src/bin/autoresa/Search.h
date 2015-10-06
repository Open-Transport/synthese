#ifndef SEARCH_H
#define SEARCH_H

#include "vector"

#include "Functions.h"
#include "33_route_planner/RoutePlannerFunction.h"
#include "04_time/DateTime.h"


using namespace std;
using namespace synthese::pt_website;
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
		string readDateTime(int prefix=4);
		string searchFromSynthese(int _favoris) throw (int);

	private:
		// common variables
		AGI_TOOLS *agi;
		AGI_CMD_RESULT *res;
		int dtmfInput;
		SessionReturnType *session;

		// local variables
		vector<string> favorisTrip;
		string sentence;

		string timeOfChoicedTrip;
		int nTimeOfChoicedTrip;

		int choicedFavorisTrajet;
			// the current choice number of the search given
		int choicedTimeOfChoicedTrip;

		tm dateTime;

		//StaticActionFunctionRequest<SearchRequest> searchRequest;




};
#endif


