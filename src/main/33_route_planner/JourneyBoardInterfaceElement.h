
#ifndef SYNTHESE_JourneyBoardInterfaceElement_H__
#define SYNTHESE_JourneyBoardInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"
#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
    namespace server 
    {
	class Request;
    }


	namespace interfaces
	{
		class JourneyBoardInterfaceElement : public LibraryInterfaceElement
		{
		private:
			ValueInterfaceElement* _handicappedFilter;
			ValueInterfaceElement* _bikeFilter;
			~JourneyBoardInterfaceElement();

		public:
			void display(std::ostream& stream, 
				     const ParametersVector& parameters, 
				     const void* object = NULL, 
				     const server::Request* request = NULL) const;
			
			/** Parser.
				@param text Text to parse : standard list of parameters
					-# Handicapped filter
					-# Bike filter
			*/
			void parse( const std::string& text);
		};

	}
}

#endif // SYNTHESE_JourneyBoardInterfaceElement_H__

