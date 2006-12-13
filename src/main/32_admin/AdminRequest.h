
#ifndef SYNTHESE_AdminRequest_H__
#define SYNTHESE_AdminRequest_H__


#include "30_server/Request.h"

namespace synthese
{
	namespace admin
	{
		class AdminInterfaceElement;

		class AdminRequest : public server::Request
		{
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_OBJECT_ID;

			//! \name Page parameters
			//@{
			const AdminInterfaceElement*	_page;
			ParametersMap					_parameters;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void setFromParametersMap(const ParametersMap& map);

		public:
			AdminRequest();
			~AdminRequest();

			void setPage(const AdminInterfaceElement* aie);
			const AdminInterfaceElement* getPage() const;

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream) const;

			const std::string& getStringParameter(const std::string& name, const std::string& defaultValue);
			long long getLongLongParameter(const std::string& name, long long defaultValue = 0);
			int getIntParameter(const std::string& name, int defaultValue = 0);

			std::string getHTMLFormHeader(const std::string& name) const;

			void setParameter(const std::string& name, const std::string value);

		};
	}
}
#endif // SYNTHESE_AdminRequest_H__
