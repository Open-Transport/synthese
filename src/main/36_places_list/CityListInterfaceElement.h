
#ifndef SYNTHESE_CityListInterfaceElement_H__
#define SYNTHESE_CityListInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace placeslist
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
		class CityListInterfaceElement : public interfaces::LibraryInterfaceElement
		{
		private:
			boost::shared_ptr<interfaces::ValueInterfaceElement> _errorMessage;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _openingText;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _closingText;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _n;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _inputText;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _emptyLineText;

		public:
			void display(std::ostream& stream, const interfaces::ParametersVector& parameters, boost::shared_ptr<const void> object = boost::shared_ptr<const void>(), const server::Request* request = NULL) const;
			void storeParameters(interfaces::ValueElementList& vel);
		};

	}
}

#endif // SYNTHESE_CityListInterfaceElement_H__

