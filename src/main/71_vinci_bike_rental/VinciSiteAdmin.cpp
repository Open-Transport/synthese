
/** VinciSiteAdmin class implementation.
	@file VinciSiteAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "VinciSiteAdmin.h"
#include "71_vinci_bike_rental/VinciSitesAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteUpdateAction.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

#include "30_server/QueryString.h"
#include "30_server/ActionFunctionRequest.h"

#include "05_html/HTMLForm.h"
#include "05_html/HTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace vinci;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VinciSiteAdmin>::FACTORY_KEY("vincisiteadmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VinciSiteAdmin>::ICON("building.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<VinciSiteAdmin>::DISPLAY_MODE(AdminInterfaceElement::DISPLAYED_IF_CURRENT);

		template<> string AdminInterfaceElementTemplate<VinciSiteAdmin>::getSuperior()
		{
			return VinciSitesAdminInterfaceElement::FACTORY_KEY;
		}
	}

	namespace vinci
	{
		VinciSiteAdmin::VinciSiteAdmin()
			: AdminInterfaceElementTemplate<VinciSiteAdmin>()
		{ }

		void VinciSiteAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id = map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY);
			if (id != UNKNOWN_VALUE)
			{
				if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
					return;
				_site = VinciSiteTableSync::Get(id);
			}
			else
				throw AdminParametersException("Site not found");
		}

		string VinciSiteAdmin::getTitle() const
		{
			return "Site " + (_site.get() ? _site->getName() : "inconnu");
		}

		void VinciSiteAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			ActionFunctionRequest<VinciSiteUpdateAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<VinciSiteAdmin>();
			updateRequest.setObjectId(_site->getKey());

			// Output
			HTMLForm uf(updateRequest.getHTMLForm("update"));

			stream << "<h1>Propriétés</h1>";
			stream << uf.open();

			HTMLTable ut;
			stream << ut.open();
			stream << ut.row();
			stream << ut.col() << "Nom";
			stream << ut.col() << uf.getTextInput(VinciSiteUpdateAction::PARAMETER_NAME, _site->getName());

			stream << ut.row();
			stream << ut.col() << "Adresse";
			stream << ut.col() << uf.getTextInput(VinciSiteUpdateAction::PARAMETER_ADDRESS, _site->getAddress());

			stream << ut.row();
			stream << ut.col() << "Téléphone";
			stream << ut.col() << uf.getTextInput(VinciSiteUpdateAction::PARAMETER_PHONE, _site->getPhone());

			stream << ut.row();
			stream << ut.col() << "Ouvert";
			stream << ut.col() << uf.getOuiNonRadioInput(VinciSiteUpdateAction::PARAMETER_OPENED, !_site->getLocked());

			vector<pair<uid, string> > othersites;
			othersites.push_back(make_pair(UNKNOWN_VALUE, "Pas de site d'approvisionnement"));
			vector<shared_ptr<VinciSite> > sites(VinciSiteTableSync::search(string("%")));
			for (vector<shared_ptr<VinciSite> >::const_iterator it(sites.begin()); it != sites.end(); ++it)
				if ((*it)->getKey() != _site->getKey())
					othersites.push_back(make_pair((*it)->getKey(), (*it)->getName()));
			stream << ut.row();
			stream << ut.col() << "Site d'approvisionnement";
			stream << ut.col() << uf.getSelectInput(VinciSiteUpdateAction::PARAMETER_PARENT_SITE_ID, othersites, _site->getParentSiteId());

			stream << ut.row();
			stream << ut.col();
			stream << ut.col() << uf.getSubmitButton("Enregistrer");
			
			stream << ut.close() << uf.close();

			stream << "<h1>Stocks suivis</h1>";

			stream << "<h1>Autres stocks</h1>";

			stream << "<h1>Derniers mouvements de stock</h1>";
		}

		bool VinciSiteAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
	}
}
