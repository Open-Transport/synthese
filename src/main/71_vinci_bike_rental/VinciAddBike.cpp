
#include "30_server/ServerModule.h"

#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciAddBike.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace vinci
	{
		const string VinciAddBike::PARAMETER_NUMBER = Action::PARAMETER_PREFIX + "nu";
		const string VinciAddBike::PARAMETER_MARKED_NUMBER = Action::PARAMETER_PREFIX + "mn";


		Request::ParametersMap VinciAddBike::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_NUMBER, _number));
			map.insert(make_pair(PARAMETER_MARKED_NUMBER, _marked_number));
			return map;
		}

		void VinciAddBike::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_NUMBER);
			if (it != map.end())
			{
				_number = it->second;
				map.erase(it);
			}

			it = map.find(PARAMETER_MARKED_NUMBER);
			if (it != map.end())
			{
				_marked_number = it->second;
				map.erase(it);
			}
		}

		void VinciAddBike::run()
		{
			VinciBike* bike = new VinciBike;
			bike->setNumber(_number);
			bike->setMarkedNumber(_marked_number);
			VinciBikeTableSync::save(ServerModule::getSQLiteThread(), bike);
			_request->setObjectId(bike->getKey());
		}
	}
}