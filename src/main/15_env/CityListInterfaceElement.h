
#ifndef SYNTHESE_CityListInterfaceElement_H__
#define SYNTHESE_CityListInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"

namespace synthese
{
	namespace interfaces
	{
		/** List of cities.
		@code city_list @endcode
		@param 0 Error message if no city founded
		@param 1 Opening text : will be putted before the link
		@param 2 Closing text : will be putted after the link
		@param 3 Number of answered cities
		@param 4 Input text
		@param 5 Text to write if empty line
		*/
		class CityListInterfaceElement : public LibraryInterfaceElement
		{
		private:
			ValueElementList _parameters;

		public:
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const Site* site = NULL) const;
			void parse(const std::string& text);
		};

	}
}

#endif // SYNTHESE_CityListInterfaceElement_H__
