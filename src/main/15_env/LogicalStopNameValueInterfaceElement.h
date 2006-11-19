
#ifndef SYNTHESE_LogicalStopNameValueInterfaceElement_H__
#define SYNTHESE_LogicalStopNameValueInterfaceElement_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	namespace env
	{
		/** Logical stop name.
			@code stop_name @endcode
		*/
		class LogicalStopNameValueInterfaceElement : public interfaces::ValueInterfaceElement
		{
		private:
			ValueInterfaceElement* _uid;

		public:
			const std::string& getValue(const interfaces::ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;

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
