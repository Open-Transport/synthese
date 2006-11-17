
#ifndef SYNTHESE_AdminRequest_H__
#define SYNTHESE_AdminRequest_H__


#include "Request.h"

namespace synthese
{
	namespace interfaces
	{
		class AdminInterfaceElement;

		class AdminRequest : public Request
		{
			static const std::string _factory_key;
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_OBJECT_ID;

			//! \name Page parameters
			//@{
				const AdminInterfaceElement*	_page;
				uid								_object_id;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void setFromParametersMap(const ParametersMap& map);

		public:
			~AdminRequest();

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream) const;

		};
	}
}
#endif // SYNTHESE_AdminRequest_H__