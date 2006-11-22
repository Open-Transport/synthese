
#include "01_util/Conversion.h"

#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminInterfacePage.h"

namespace synthese
{
	using util::Conversion;
	using namespace interfaces;

	namespace admin
	{
		void AdminInterfacePage::display( std::ostream& stream , const AdminInterfaceElement* page , const uid objectId , const server::Request* request /*= NULL */ ) const
		{
			ParametersVector parameters;
			parameters.push_back(Conversion::ToString(objectId));

			InterfacePage::display(stream, parameters, (const void*) page, request);
		}
	}
}