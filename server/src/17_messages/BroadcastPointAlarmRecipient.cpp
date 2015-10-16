
/** BroadcastPointAlarmRecipient class implementation.
	@file BroadcastPointAlarmRecipient.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "BroadcastPointAlarmRecipient.hpp"

#include "AdminModule.h"
#include "Alarm.h"
#include "AlarmAddLinkAction.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkException.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmTestOnDisplayScreenFunction.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableModule.h"
#include "Destination.hpp"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplaySearchAdmin.h"
#include "DisplayType.h"
#include "HTMLList.h"
#include "ImportableTableSync.hpp"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "MessagesRight.h"
#include "Profile.h"
#include "PTModule.h"
#include "ResultHTMLTable.h"
#include "RollingStock.hpp"
#include "SecurityConstants.hpp"
#include "StopArea.hpp"
#include "User.h"

#include <vector>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace departure_boards; // Temporary. TODO : remove all call to this module
	
	using namespace messages;
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace admin;
	using namespace html;
	using namespace security;
	using namespace impex;

	namespace util
	{
		template<> const std::string FactorableTemplate<AlarmRecipient, BroadcastPointAlarmRecipient>::FACTORY_KEY("displayscreen");
	}

	namespace messages
	{
		template<> const string AlarmRecipientTemplate<BroadcastPointAlarmRecipient>::TITLE("Afficheurs");

		template<> void AlarmRecipientTemplate<BroadcastPointAlarmRecipient>::GetParametersLabels(
			ParameterLabelsVector& m
		){
			m.push_back(make_pair(FACTORY_KEY +"/" + GLOBAL_PERIMETER,"(tous les afficheurs)"));
			PTModule::getNetworkLinePlaceRightParameterList(m, FACTORY_KEY +"/", "Afficheur de ");
		}



		template<>
		RegistryKeyType AlarmRecipientTemplate<BroadcastPointAlarmRecipient>::GetObjectIdBySource(
			const impex::DataSource& source,
			const std::string& key,
			util::Env& env
		){
			ImportableTableSync::ObjectBySource<DisplayScreenTableSync> screens(source, env);
			ImportableTableSync::ObjectBySource<DisplayScreenTableSync>::Set screensSet(screens.get(key));
			if(screensSet.empty())
			{
				throw Exception("No such screen");
			}
			return (*screensSet.begin())->getKey();
		}
	}

	namespace messages
	{
		const std::string BroadcastPointAlarmRecipient::PARAMETER_SEARCH_CITY_NAME = "dsascity";
		const std::string BroadcastPointAlarmRecipient::PARAMETER_SEARCH_STOP_NAME = "dsaslid";
		const std::string BroadcastPointAlarmRecipient::PARAMETER_SEARCH_NAME = "dsasloc";
		const std::string BroadcastPointAlarmRecipient::PARAMETER_SEARCH_LINE = "dsasli";
		const std::string BroadcastPointAlarmRecipient::PARAMETER_SEARCH_TYPE = "dsasti";
		const std::string BroadcastPointAlarmRecipient::PARAMETER_SEARCH_STATUS = "dsass";
		const std::string BroadcastPointAlarmRecipient::PARAMETER_SEARCH_MESSAGE = "dsasm";

		void BroadcastPointAlarmRecipient::displayBroadcastListEditor(
			std::ostream& stream
			, const messages::Alarm* alarm
			, const util::ParametersMap& parameters
			, Request& searchRequest
			, StaticActionRequest<messages::AlarmAddLinkAction>& addRequest
			, StaticActionRequest<messages::AlarmRemoveLinkAction>& removeRequest
		){
			Env env;
			vector<boost::shared_ptr<DisplayScreen> > dsv(
				AlarmObjectLinkTableSync::search<DisplayScreenTableSync,DisplayScreen>(
					env,
					alarm->getKey(),
					this->getFactoryKey()
			)	);
			set<util::RegistryKeyType> usedDisplayScreens;

			StaticFunctionRequest<AlarmTestOnDisplayScreenFunction> testRequest(addRequest, true);
			testRequest.getFunction()->setAlarmId(alarm->getKey());

			stream << "<h1>Test du message</h1>";

			HTMLForm testForm(testRequest.getHTMLForm("testForm"));
			stream << testForm.open() << "<p>";
			stream << "Type d'afficheur à tester : " <<
				testForm.getSelectInput(
					AlarmTestOnDisplayScreenFunction::PARAMETER_DISPLAY_TYPE_ID,
					DeparturesTableModule::getDisplayTypeLabels(),
					optional<RegistryKeyType>()
				);
			stream << testForm.getSubmitOnPopupLink(HTMLModule::getHTMLImage("/admin/img/accept.png", "OK"), 800, 600);
			stream << "</p>" << testForm.close();

			stream << "<h1>Afficheurs destinataires</h1>";

			if (dsv.empty())
			{
				stream << "<p>Aucun afficheur destinataire</p>";
			}
			else
			{
				HTMLList l;
				stream << l.open();

				for (vector<boost::shared_ptr<DisplayScreen> >::iterator dsit = dsv.begin(); dsit != dsv.end(); ++dsit)
				{
					boost::shared_ptr<DisplayScreen> ds = *dsit;
					usedDisplayScreens.insert(ds->getKey());
					removeRequest.getAction()->setObjectId(ds->getKey());

					stream << l.element("display");
					stream << HTMLModule::getHTMLLink(removeRequest.getURL(), HTMLModule::getHTMLImage("/admin/img/delete.png","Supprimer"));

					stream << ds->getFullName();
				}

				stream << l.close();
			}

			stream << "<h1>Ajout d'afficheur</h1>";

			string searchCity(parameters.getDefault<string>(PARAMETER_SEARCH_CITY_NAME));
			string searchStop(parameters.getDefault<string>(PARAMETER_SEARCH_STOP_NAME));
			string searchName(parameters.getDefault<string>(PARAMETER_SEARCH_NAME));
			optional<RegistryKeyType> searchLine;
			optional<RegistryKeyType> searchType;
			optional<int> searchState;
			optional<int> searchMessage;

			stream << DisplaySearchAdmin::getHtmlSearchForm(
				searchRequest.getHTMLForm()
				, searchCity
				, searchStop
				, searchName
				, searchLine
				, searchType
				, searchState
				, searchMessage
			);

			security::RightsOfSameClassMap rights = searchRequest.getUser()->getProfile()->getRightsForModuleClass<MessagesRight>();
			//boost::optional<const security::RightsOfSameClassMap&> bor(borv);
			DisplayScreenTableSync::SearchResult screens(
				DisplayScreenTableSync::Search(
					env,
					boost::optional<const security::RightsOfSameClassMap&>(rights) //bor //boost::optional<const security::RightsOfSameClassMap&>(searchRequest.getUser()->getProfile()->getRightsForModuleClass<MessagesRight>())
					, searchRequest.getUser()->getProfile()->getGlobalPublicRight<MessagesRight>() >= READ
					, WRITE
					, boost::optional<RegistryKeyType>()
					, boost::optional<RegistryKeyType>()
					, searchLine
					, searchType
					, searchCity
					, searchStop
					, searchName
					, searchState
					, searchMessage
			)	);

			ResultHTMLTable::HeaderVector v1;
			v1.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			v1.push_back(make_pair(PARAMETER_SEARCH_TYPE, "Type"));
			v1.push_back(make_pair(PARAMETER_SEARCH_STATUS, "Etat"));
			v1.push_back(make_pair(PARAMETER_SEARCH_MESSAGE, "Msg"));
			v1.push_back(make_pair(string(), "Ajout"));
			ResultHTMLTable t1(
				v1,
				searchRequest.getHTMLForm(),
				ResultHTMLTable::RequestParameters(),
				screens
			);

			stream << t1.open();

			BOOST_FOREACH(const boost::shared_ptr<DisplayScreen>& screen, screens)
			{
				if(!screen->getLocation())
				{
					continue;
				}
				if (usedDisplayScreens.find(screen->getKey()) != usedDisplayScreens.end())
					continue;

				addRequest.getAction()->setObjectId(screen->getKey());

				stream << t1.row();
				stream << t1.col() << screen->getFullName();
				stream << t1.col() << (screen->get<DisplayTypePtr>() ? screen->get<DisplayTypePtr>()->get<Name>() : "inconnu");
				stream << t1.col();
				stream << t1.col();
				stream << t1.col() << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
			}

			stream << t1.close();
		}

		AlarmRecipientSearchFieldsMap BroadcastPointAlarmRecipient::getSearchFields(HTMLForm& form, const ParametersMap& parameters) const
		{
			boost::shared_ptr<const JourneyPattern> line;
			Env env;
			optional<RegistryKeyType> id(parameters.getOptional<RegistryKeyType>(PARAMETER_SEARCH_LINE));
			if (id)
			{
				line  = JourneyPatternTableSync::Get(*id, env, FIELDS_ONLY_LOAD_LEVEL);
			}

			AlarmRecipientSearchFieldsMap map;
			AlarmRecipientFilter arf;

			arf.label = "Ligne";
			arf.htmlField = form.getSelectInput(
				PARAMETER_SEARCH_LINE,
				DeparturesTableModule::getCommercialLineWithBroadcastLabels(true),
				line ? line->getKey() : optional<RegistryKeyType>()
			);
			arf.query = "";
			map.insert(make_pair(PARAMETER_SEARCH_LINE, arf));

			return map;
		}



		boost::shared_ptr<security::Right> BroadcastPointAlarmRecipient::getRight( const std::string& perimeter ) const
		{
			ArrivalDepartureTableRight* result(new ArrivalDepartureTableRight);
			result->setParameter(perimeter);
			return boost::shared_ptr<Right>(result);
		}
}	}
