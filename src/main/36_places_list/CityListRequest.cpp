
#include "15_env/Environment.h"

#include "36_places_list/CityListRequest.h"

namespace synthese
{
	namespace placeslist
	{


		void CityListRequest::run( std::ostream& stream ) const
		{
			env::Environment::CityList tbCommunes = _site->getEnvironment()->guessCity(_input, _n );

		}
	}
}
