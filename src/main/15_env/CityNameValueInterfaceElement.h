
#ifndef SYNTHESE_CityNameValueInterfaceElement_H__
#define SYNTHESE_CityNameValueInterfaceElement_H__

#include "01_util/UId.h"

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueElementList;
	}
	namespace env
	{
		class CityNameValueInterfaceElement : public interfaces::ValueInterfaceElement
		{
		private:
			interfaces::ValueInterfaceElement* _uid;

		public:
			std::string getValue(const interfaces::ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;

			/** Parser.
				@param text Optional parameter :
					- if defined : the uid of the city to name
					- if empty : the name of the city provided at runtime as current object
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};
	}
}
#endif // SYNTHESE_CityNameValueInterfaceElement_H__

