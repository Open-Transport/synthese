
/** DisplaySearchAdmin class implementation.
	@file DisplaySearchAdmin.cpp

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

#include "01_util/Conversion.h"

#include "11_interfaces/InterfaceModule.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/EnvModule.h"

#include "05_html/ActionResultHTMLTable.h"
#include "05_html/SearchFormHTMLTable.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminModule.h"

#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/CreateDisplayScreenAction.h"
#include "34_departures_table/DisplayAdmin.h"
#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplayScreenContentRequest.h"
#include "34_departures_table/DeparturesTableModule.h"


using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace html;

	namespace departurestable
	{
		const string DisplaySearchAdmin::PARAMETER_SEARCH_UID = "dsasuid";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LINE_ID = "dsaslid";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION = "dsasloc";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_TYPE_ID = "dsasti";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_STATE = "dsass";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_MESSAGE = "dsasm";

		DisplaySearchAdmin::DisplaySearchAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED)
			, _searchUId(0)
			, _searchLocalizationUId(0)
			, _searchLineId(0)
			, _searchTypeId(0)
			, _searchState(UNKNOWN_VALUE)
			, _searchMessage(UNKNOWN_VALUE)
		{}

		void DisplaySearchAdmin::setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it = map.find(PARAMETER_SEARCH_UID);
			if (it != map.end())
				_searchUId = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_SEARCH_LOCALIZATION);
			if (it != map.end())
				_searchLocalizationUId = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_SEARCH_LINE_ID);
			if (it != map.end())
				_searchLineId = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_SEARCH_TYPE_ID);
			if (it != map.end())
				_searchTypeId = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_SEARCH_STATE);
			if (it != map.end())
				_searchState = Conversion::ToInt(it->second);

			it = map.find(PARAMETER_SEARCH_MESSAGE);
			if (it != map.end())
				_searchMessage = Conversion::ToInt(it->second);

			_result = DisplayScreenTableSync::search(_searchUId, _searchLocalizationUId, _searchLineId, _searchTypeId, _searchState, _searchMessage);
		}

		string DisplaySearchAdmin::getTitle() const
		{
			return "Afficheurs";
		}

		void DisplaySearchAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<CreateDisplayScreenAction,AdminRequest> createDisplayRequest(request);
			createDisplayRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			createDisplayRequest.getFunction()->setActionFailedPage(Factory<AdminInterfaceElement>::create<DisplaySearchAdmin>());

			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplaySearchAdmin>());

			FunctionRequest<AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());

			FunctionRequest<DisplayScreenContentRequest> viewRequest(request);

			FunctionRequest<AdminRequest> maintRequest(request);
			maintRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayMaintenanceAdmin>());

			stream << "<h1>Recherche</h1>";

			stream << getHtmlSearchForm(searchRequest.getHTMLForm(), _searchUId, _searchLocalizationUId, _searchLineId, _searchTypeId, _searchState, _searchMessage);

			stream << "<h1>Résultats de la recherche</h1>";

			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_UID, "UID"));
			v.push_back(make_pair(PARAMETER_SEARCH_LOCALIZATION, "Emplacement"));
			v.push_back(make_pair(PARAMETER_SEARCH_TYPE_ID, "Type"));
			v.push_back(make_pair(PARAMETER_SEARCH_STATE, "Etat"));
			v.push_back(make_pair(PARAMETER_SEARCH_MESSAGE, "Msg"));
			v.push_back(make_pair(string(), "Actions"));
			v.push_back(make_pair(string(), "Actions"));
			v.push_back(make_pair(string(), "Actions"));

			ActionResultHTMLTable t(v, searchRequest.getHTMLForm("search"), string(), true, createDisplayRequest.getHTMLForm("create"), CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();

			for (vector<DisplayScreen*>::const_iterator it = _result.begin(); it != _result.end(); ++it)
			{
				DisplayScreen* screen = *it;
				updateRequest.setObjectId(screen->getKey());
				viewRequest.setObjectId(screen->getKey());
				maintRequest.setObjectId(screen->getKey());

				stream << t.row(Conversion::ToString(screen->getKey()));
				stream << t.col() << screen->getKey();
				stream << t.col() << (screen->getLocalization() ? screen->getLocalization()->getFullName() : "(indéterminé)");
				stream << t.col() << (screen->getType() ? screen->getType()->getName() : "(indéterminé)");
				stream << t.col(); // Bullets showing the states of the display
				stream << t.col(); // Bullet showing the message status
				stream << t.col() << HTMLModule::getLinkButton(updateRequest.getURL(), "Modifier");
				stream << t.col() << HTMLModule::getLinkButton(viewRequest.getURL(), "Simuler");
				stream << t.col() << HTMLModule::getLinkButton(maintRequest.getURL(), "Supervision");
			}

			stream << t.row();
			stream << t.col(5) << "(sélectionner un afficheur existant pour copier ses&nbsp;propriétés dans le nouvel afficheur)";
			stream << t.col(3) << t.getActionForm().getSubmitButton("Créer un nouvel afficheur");

			stream << t.close();
		}

		std::string DisplaySearchAdmin::getHtmlSearchForm(const HTMLForm& form, uid screenUid, uid placeUid, uid lineUid, uid typeUid, int state, int message )
		{
			vector<pair<int, string> > states;
			states.push_back(make_pair(UNKNOWN_VALUE, "(tous)"));
			states.push_back(make_pair(1, "OK"));
			states.push_back(make_pair(2, "Warning"));
			states.push_back(make_pair(3, "Warning+Error"));
			states.push_back(make_pair(4, "Error"));

			vector<pair<int, string> > messages;
			messages.push_back(make_pair(UNKNOWN_VALUE, "(tous)"));
			messages.push_back(make_pair(1, "Un message"));
			messages.push_back(make_pair(2, "Conflit"));
			messages.push_back(make_pair(3, "Messages"));

			stringstream stream;
			SearchFormHTMLTable s(form);
			stream << s.open();
			stream << s.cell("UID", s.getForm().getTextInput(PARAMETER_SEARCH_UID, screenUid ? Conversion::ToString(screenUid) : string()));
			stream << s.cell("Emplacement", s.getForm().getSelectInput(PARAMETER_SEARCH_LOCALIZATION, DeparturesTableModule::getPlacesWithBroadcastPointsLabels(true), placeUid));
			stream << s.cell("Ligne", s.getForm().getSelectInput(PARAMETER_SEARCH_LINE_ID, DeparturesTableModule::getCommercialLineWithBroadcastLabels(true), lineUid));
			stream << s.cell("Type", s.getForm().getSelectInput(PARAMETER_SEARCH_TYPE_ID, DeparturesTableModule::getDisplayTypeLabels(true), typeUid));
			stream << s.cell("Etat", s.getForm().getSelectInput(PARAMETER_SEARCH_TYPE_ID, states, state));
			stream << s.cell("Message", s.getForm().getSelectInput(PARAMETER_SEARCH_MESSAGE, messages, message));
			stream << s.close();

			return stream.str();
		}

		bool DisplaySearchAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}
	}
}
