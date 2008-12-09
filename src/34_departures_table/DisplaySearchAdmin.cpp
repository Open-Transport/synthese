
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

#include "DisplaySearchAdmin.h"
#include "AdvancedSelectTableSync.h"
#include "DisplayScreenTableSync.h"
#include "CreateDisplayScreenAction.h"
#include "DisplayAdmin.h"
#include "DisplayMaintenanceAdmin.h"
#include "DisplayScreenContentRequest.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableModule.h"
#include "BroadcastPointsAdmin.h"

#include "InterfaceModule.h"

#include "Conversion.h"

#include "ActionResultHTMLTable.h"
#include "SearchFormHTMLTable.h"

#include "ActionFunctionRequest.h"

#include "AdminModule.h"
#include "AdminRequest.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"

#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"
#include "City.h"

#include <boost/foreach.hpp>

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
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DisplaySearchAdmin>::FACTORY_KEY("0displays");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplaySearchAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<DisplaySearchAdmin>::DEFAULT_TITLE("Afficheurs");
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
			uid placeId(map.getUid(PARAMETER_SEARCH_LOCALIZATION_ID, false, FACTORY_KEY));
			if (placeId == UNKNOWN_VALUE)
			{
				_searchCity = map.getString(PARAMETER_SEARCH_CITY, false, FACTORY_KEY);
				_searchStop = map.getString(PARAMETER_SEARCH_STOP, false, FACTORY_KEY);
				_searchName = map.getString(PARAMETER_SEARCH_NAME, false, FACTORY_KEY);
				_searchLineId = map.getUid(PARAMETER_SEARCH_LINE_ID, false, FACTORY_KEY);
				_searchTypeId = map.getUid(PARAMETER_SEARCH_TYPE_ID, false, FACTORY_KEY);
				_searchState = map.getInt(PARAMETER_SEARCH_STATE, false, FACTORY_KEY);
				_searchMessage = map.getInt(PARAMETER_SEARCH_MESSAGE, false, FACTORY_KEY);
			}
			else
			{
				try
				{
					_place = ConnectionPlaceTableSync::Get(placeId);
				}
				catch (...)
				{
					throw AdminParametersException("Specified place not found");
				}
			}

			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_CITY, 30);
		}


		void DisplaySearchAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			Env env;
			DisplayScreenTableSync::Search(
				env,
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
			ResultHTMLTable::ResultParameters	_resultParameters;
			_resultParameters.setFromResult(_requestParameters, env.template getEditableRegistry<DisplayScreen>());


			ActionFunctionRequest<CreateDisplayScreenAction,AdminRequest> createDisplayRequest(request);
			createDisplayRequest.getFunction()->setPage<DisplayAdmin>();
			createDisplayRequest.getFunction()->setActionFailedPage<DisplaySearchAdmin>();
			createDisplayRequest.getAction()->setPlace(_place);

			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<DisplaySearchAdmin>();

			FunctionRequest<AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<DisplayAdmin>();

			FunctionRequest<DisplayScreenContentRequest> viewRequest(request);

			FunctionRequest<AdminRequest> maintRequest(request);
			maintRequest.getFunction()->setPage<DisplayMaintenanceAdmin>();

			if (!_place.get())
			{
				stream << "<h1>Recherche</h1>";

				stream << getHtmlSearchForm(searchRequest.getHTMLForm(), _searchCity, _searchStop, _searchName,  _searchLineId, _searchTypeId, _searchState, _searchMessage);
			}

			stream << "<h1>" << (_place.get() ? "Afficheurs" : "Résultats de la recherche") << "</h1>";

			ActionResultHTMLTable::HeaderVector v;
			if (!_place.get())
			{
				v.push_back(make_pair(PARAMETER_SEARCH_CITY, "Commune"));
				v.push_back(make_pair(PARAMETER_SEARCH_STOP, "Arrêt"));
			}
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

			BOOST_FOREACH(shared_ptr<DisplayScreen> screen, env.template getRegistry<DisplayScreen>())
			{
				updateRequest.setObjectId(screen->getKey());
				viewRequest.setObjectId(screen->getKey());
				maintRequest.setObjectId(screen->getKey());

				stream << t.row(Conversion::ToString(screen->getKey()));
				if (!_place.get())
				{
					stream << t.col() << (screen->getLocalization() ? screen->getLocalization()->getCity()->getName() : "(indéterminé)");
					stream << t.col() << (screen->getLocalization() ? screen->getLocalization()->getName() : "(indéterminé)");
				}
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
				stream << t.col(_place.get() ? 4 : 6) << "(sélectionner un afficheur existant pour copier ses&nbsp;propriétés dans le nouvel afficheur)";
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
			return request->isAuthorized<ArrivalDepartureTableRight>(READ);
		}

		AdminInterfaceElement::PageLinks DisplaySearchAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;

			// General search page
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == DeparturesTableModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}

			// Broadcast points search
			if (parentLink.factoryKey == BroadcastPointsAdmin::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY && static_cast<const DisplaySearchAdmin&>(currentPage)._place.get())
			{
				links.push_back(currentPage.getPageLink());
			}
			return links;
		}

		std::string DisplaySearchAdmin::getTitle() const
		{
			return _place.get() ? _place->getFullName() : DEFAULT_TITLE;
		}

		std::string DisplaySearchAdmin::getParameterName() const
		{
			return _place.get() ? PARAMETER_SEARCH_LOCALIZATION_ID : string();
		}

		std::string DisplaySearchAdmin::getParameterValue() const
		{
			return _place.get() ? Conversion::ToString(_place->getKey()) : string();
		}

		std::string DisplaySearchAdmin::getIcon() const
		{
			return _place.get() ? BroadcastPointsAdmin::ICON : ICON;
		}

		bool DisplaySearchAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}
	}
}
