
#ifndef SYNTHESE_CityNameValueInterfaceElement_H__
#define SYNTHESE_CityNameValueInterfaceElement_H__


#include "11_interfaces/ValueInterfaceElement.h"
#include "01_util/UId.h"

namespace synthese
{
	namespace interfaces
	{
		class CityNameValueInterfaceElement : public ValueInterfaceElement
		{
		private:
			ValueInterfaceElement* _uid;

		public:
			const std::string& getValue(const ParametersVector& parameters, const void* object = NULL, const Site* site = NULL) const;

			/** Parser.
				@param text Optional parameter :
					- if defined : the uid of the city to name
					- if empty : the name of the city provided at runtime as current object
			*/
			void parse(const std::string& text);
		};
	}
}
#endif // SYNTHESE_CityNameValueInterfaceElement_H__
