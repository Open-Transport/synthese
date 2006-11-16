
#ifndef SYNTHESE_LogicalStopNameValueInterfaceElement_H__
#define SYNTHESE_LogicalStopNameValueInterfaceElement_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		/** Logical stop name.
			@code stop_name @endcode
		*/
		class LogicalStopNameValueInterfaceElement : public ValueInterfaceElement
		{
		private:
			static const std::string _value_factory_key;
			static const std::string _library_factory_key;
			ValueInterfaceElement* _uid;

		public:
			const std::string& getValue(const ParametersVector& parameters, const void* object = NULL, const Site* site = NULL) const;

			/** Parser.
			@param text Optional parameter :
				- if defined : the uid of the stop to name
				- if empty : the name of the stop provided at runtime as current object
			@todo Handle the designation number
			*/
			void parse(const std::string& text);
		};
	}
}
#endif // SYNTHESE_LogicalStopNameValueInterfaceElement_H__
