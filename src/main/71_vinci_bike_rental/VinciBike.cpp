
#include "71_vinci_bike_rental/VinciBike.h"

namespace synthese
{
	using namespace util;

	namespace vinci
	{


		const std::string& VinciBike::getNumber() const
		{
			return _number;
		}

		void VinciBike::setNumber( const std::string& number )
		{
			_number = number;
		}

		const std::string& VinciBike::getMarkedNumber() const
		{
			return _markedNumber;
		}

		void VinciBike::setMarkedNumber( const std::string& markedNumber )
		{
			_markedNumber = markedNumber;
		}

		VinciBike::VinciBike( uid id/*=0*/ )
			: Registrable<uid, VinciBike>(id)
		{
			
		}
	}
}
