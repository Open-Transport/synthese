
#include "AdminInterfacePage.h"
#include "AdminInterfaceElement.h"
#include "01_util/Conversion.h"

namespace synthese
{
	using util::Conversion;

	namespace interfaces
	{
		void AdminInterfacePage::display( std::ostream& stream , const AdminInterfaceElement* page , const uid objectId , const Site* site /*= NULL */ ) const
		{
			ParametersVector parameters;
			parameters.push_back(Conversion::ToString(objectId));

			InterfacePage::display(stream, parameters, NULL, site);
		}
	}
}