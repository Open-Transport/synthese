
/** VinciSitesAdminInterfaceElement class implementation.
	@file VinciSitesAdminInterfaceElement.cpp
	@author Hugues Romain
	@date 2007

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "71_vinci_bike_rental/VinciSitesAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/HomeAdmin.h"

#include "05_html/ActionResultHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace vinci;

	namespace util
	{
	    template <> const string FactorableTemplate<AdminInterfaceElement,VinciSitesAdminInterfaceElement>::FACTORY_KEY("vincisites");
	}

	namespace admin
	{
	    template <> const string AdminInterfaceElementTemplate<VinciSitesAdminInterfaceElement>::ICON("building.png");
	    template <> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<VinciSitesAdminInterfaceElement>::DISPLAY_MODE(AdminInterfaceElement::EVER_DISPLAYED);
	    template <> string AdminInterfaceElementTemplate<VinciSitesAdminInterfaceElement>::getSuperior()
		{
			return HomeAdmin::FACTORY_KEY;
		}
	}

	namespace vinci
	{
		/// @todo Verify the parent constructor parameters
		VinciSitesAdminInterfaceElement::VinciSitesAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciSitesAdminInterfaceElement>() {}

		void VinciSitesAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			_sites = VinciSiteTableSync::search();
		}

		string VinciSitesAdminInterfaceElement::getTitle() const
		{
			return "Sites";
		}

		void VinciSitesAdminInterfaceElement::display( std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request /*= NULL*/ ) const
		{

		}

		bool VinciSitesAdminInterfaceElement::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the right control;
			return false;
		}
	}
}
