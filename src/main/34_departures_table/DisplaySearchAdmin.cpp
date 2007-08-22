
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

#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/CreateDisplayScreenAction.h"
#include "34_departures_table/DisplayAdmin.h"
#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplayScreenContentRequest.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayType.h"
#include "34_departures_table/ArrivalDepartureTableRight.h"
#include "34_departures_table/DeparturesTableModule.h"

#include "11_interfaces/InterfaceModule.h"

#include "01_util/Conversion.h"

#include "05_html/ActionResultHTMLTable.h"
#include "05_html/SearchFormHTMLTable.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminModule.h"
#include "32_admin/HomeAdmin.h"
#include "32_admin/AdminParametersException.h"

#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/ConnectionPlaceTableSync.h"
#include "15_env/City.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace html;
	using namespace departurestable;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DisplaySearchAdmin>::FACTORY_KEY("displays");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplaySearchAdmin>::ICON("monitor.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<DisplaySearchAdmin>::DISPLAY_MODE(AdminInterfaceElement::EVER_DISPLAYED);
		template<> string AdminInterfaceElementTemplate<DisplaySearchAdmin>::getSuperior()
		{
			return HomeAdmin::FACTORY_KEY;
		}
	}

	namespace departurestable
	{
		const string DisplaySearchAdmin::PARAMETER_SEARCH_CITY = "dsascity";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_STOP = "dsaslid";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_NAME = "dsasloc";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LINE_ID = "dsasli";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_TYPE_ID = "dsasti";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_STATE = "dsass";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_MESSAGE = "dsasm";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION_ID("dsapsli");

		DisplaySearchAdmin::DisplaySearchAdmin()
			: AdminInterfaceElementTemplate<DisplaySearchAdmin>()
			, _searchLineId(UNKNOWN_VALUE)
			, _searchTypeId(UNKNOWN_VALUE)
			, _searchState(UNKNOWN_VALUE)
			, _searchMessage(UNKNOWN_VALUE)
		{}

		void DisplaySearchAdmin::setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;
			
			it = map.find(PARAMETER_SEARCH_LOCALIZATION_ID);
			if (it == map.end() || Conversion::ToLongLong(it->second) == UNKNOWN_VALUE)
			{
				it = map.find(PARAMETER_SEARCH_CITY);
				if (it != map.end())
					_searchCity = it->second;

				it = map.find(PARAMETER_SEARCH_STOP);
				if (it != map.end())
					_searchStop = it->second;

				it = map.find(PARAMETER_SEARCH_NAME);
				if (it != map.end())
					_searchName = it->second;

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
			}
			else
			{
				try
				{
					_place = ConnectionPlaceTableSync::get(Conversion::ToLongLong(it->second));
				}
				catch (...)
				{
					throw AdminParametersException("Specified place not found");
				}
			}

			_requestParameters = ActionResultHTMLTable::getParameters(map, PARAMETER_SEARCH_CITY, 30);
		}

		string DisplaySearchAdmin::getTitle() const
		{
			return _place.get() ? "Afficheurs " + _place->getName() : "Afficheurs";
		}

		void DisplaySearchAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{

			html::ActionResultHTMLTable::ResultParameters	_resultParameters;

			std::vector<boost::shared_ptr<DisplayScreen> >	_result;

			_result = DisplayScreenTableSync::search(
				request->getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
				, request->getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
				, READ
				, UNKNOWN_VALUE
				, _place.get() ? _place->getKey() : UNKNOWN_VALUE
				, _searchLineId
				, _searchTypeId
				, _searchCity
				, _searchStop
				, _searchName
				, _searchState
				, _searchMessage
				, _requestParameters.first
				, _requestParameters.maxSize
				, false
				, _requestParameters.orderField == PARAMETER_SEARCH_CITY
				, _requestParameters.orderField == PARAMETER_SEARCH_STOP
				, _requestParameters.orderField == PARAMETER_SEARCH_NAME
				, _requestParameters.orderField == PARAMETER_SEARCH_TYPE_ID
				, _requestParameters.orderField == PARAMETER_SEARCH_STATE
				, _requestParameters.orderField == PARAMETER_SEARCH_MESSAGE
				, _requestParameters.raisingOrder
				);

			_resultParameters = ActionResultHTMLTable::getParameters(_requestParameters, _result);


			ActionFunctionRequest<CreateDisplayScreenAction,AdminRequest> createDisplayRequest(request);
			createDisplayRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<DisplayAdmin>());
			createDisplayRequest.getFunction()->setActionFailedPage(Factory<AdminInterfaceElement>::createSharedPtr<DisplaySearchAdmin>());
			createDisplayRequest.getAction()->setPlace(_place);

			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<DisplaySearchAdmin>());

			FunctionRequest<AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<DisplayAdmin>());

			FunctionRequest<DisplayScreenContentRequest> viewRequest(request);

			FunctionRequest<AdminRequest> maintRequest(request);
			maintRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<DisplayMaintenanceAdmin>());

			if (!_place.get())
			{
				stream << "<h1>Recherche</h1>";

				stream << getHtmlSearchForm(searchRequest.getHTMLForm(), _searchCity, _searchStop, _searchName,  _searchLineId, _searchTypeId, _searchState, _searchMessage);
			}

			stream << "<h1>Résultats de la recherche</h1>";

			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_CITY, "Commune"));
			v.push_back(make_pair(PARAMETER_SEARCH_STOP, "Arrêt"));
			v.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			v.push_back(make_pair(PARAMETER_SEARCH_TYPE_ID, "Type"));
			v.push_back(make_pair(PARAMETER_SEARCH_STATE, "Etat"));
			v.push_back(make_pair(PARAMETER_SEARCH_MESSAGE, "Msg"));
			v.push_back(make_pair(string(), "Actions"));
			v.push_back(make_pair(string(), "Actions"));
			v.push_back(make_pair(string(), "Actions"));

			ActionResultHTMLTable t(
				v
				, searchRequest.getHTMLForm("search")
				, _requestParameters
				, _resultParameters
				, createDisplayRequest.getHTMLForm("create")
				, CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID
				, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE)
			);

			stream << t.open();

			for (vector<shared_ptr<DisplayScreen> >::const_iterator it = _result.begin(); it != _result.end(); ++it)
			{
				shared_ptr<DisplayScreen> screen = *it;
				updateRequest.setObjectId(screen->getKey());
				viewRequest.setObjectId(screen->getKey());
				maintRequest.setObjectId(screen->getKey());

				stream << t.row(Conversion::ToString(screen->getKey()));
				stream << t.col() << (screen->getLocalization().get() ? screen->getLocalization()->getCity()->getName() : "(indéterminé)");
				stream << t.col() << (screen->getLocalization().get() ? screen->getLocalization()->getName() : "(indéterminé)");
				stream << t.col() << screen->getLocalizationComment();
				stream << t.col() << (screen->getType() ? screen->getType()->getName() : "(indéterminé)");
				stream << t.col(); // Bullets showing the states of the display
				stream << t.col(); // Bullet showing the message status
				stream << t.col() << HTMLModule::getLinkButton(updateRequest.getURL(), "Modifier", string(), "monitor_edit.png");
				stream << t.col() << HTMLModule::getLinkButton(viewRequest.getURL(), "Simuler", string(), "monitor_go.png");
				stream << t.col() << HTMLModule::getLinkButton(maintRequest.getURL(), "Supervision", string(), "monitor_lightning.png");
			}

			if (_place.get())
			{
				stream << t.row();
				stream << t.col(6) << "(sélectionner un afficheur existant pour copier ses&nbsp;propriétés dans le nouvel afficheur)";
				stream << t.col(3) << t.getActionForm().getSubmitButton("Créer un nouvel afficheur");
			}

			stream << t.close();
		}

		std::string DisplaySearchAdmin::getHtmlSearchForm(const HTMLForm& form
			, const std::string& cityName
			, const std::string& stopName
			, const std::string& displayName
			, uid lineUid, uid typeUid, int state, int message
		){
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
			stream << s.cell("Commune", s.getForm().getTextInput(PARAMETER_SEARCH_CITY, cityName));
			stream << s.cell("Arrêt", s.getForm().getTextInput(PARAMETER_SEARCH_STOP, stopName));
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, displayName));
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
