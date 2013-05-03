
/** DisplayScreenAlarmRecipient class implementation.
	@file DisplayScreenAlarmRecipient.cpp

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

#include "DisplayScreenAlarmRecipient.h"

#include "AlarmTestOnDisplayScreenFunction.h"
#include "ArrivalDepartureTableRight.h"
#include "CustomBroadcastPoint.hpp"
#include "DeparturesTableModule.h"
#include "DisplaySearchAdmin.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "DisplayScreen.h"
#include "HTMLList.h"
#include "ImportableTableSync.hpp"
#include "ResultHTMLTable.h"
#include "SecurityConstants.hpp"
#include "User.h"
#include "Profile.h"
#include "StopArea.hpp"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "PTModule.h"
#include "Alarm.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "MessagesRight.h"
#include "AlarmObjectLinkException.h"
#include "AdminModule.h"

#include <vector>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace messages;
	using namespace util;
	using namespace departure_boards;
	using namespace server;
	using namespace pt;
	using namespace admin;
	using namespace interfaces;
	using namespace html;
	using namespace security;
	using namespace impex;

	namespace util
	{
		template<> const std::string FactorableTemplate<AlarmRecipient, DisplayScreenAlarmRecipient>::FACTORY_KEY("displayscreen");
	}

	namespace messages
	{
		template<> const string AlarmRecipientTemplate<DisplayScreenAlarmRecipient>::TITLE("Afficheurs");

		template<> void AlarmRecipientTemplate<DisplayScreenAlarmRecipient>::GetParametersLabels(
			ParameterLabelsVector& m
		){
			m.push_back(make_pair(FACTORY_KEY +"/" + GLOBAL_PERIMETER,"(tous les afficheurs)"));
			PTModule::getNetworkLinePlaceRightParameterList(m, FACTORY_KEY +"/", "Afficheur de ");
		}



		template<>
		RegistryKeyType AlarmRecipientTemplate<DisplayScreenAlarmRecipient>::GetObjectIdBySource(
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

	namespace departure_boards
	{
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_CITY_NAME = "dsascity";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_STOP_NAME = "dsaslid";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_NAME = "dsasloc";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_LINE = "dsasli";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_TYPE = "dsasti";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_STATUS = "dsass";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_MESSAGE = "dsasm";

		void DisplayScreenAlarmRecipient::displayBroadcastListEditor(
			std::ostream& stream
			, const messages::Alarm* alarm
			, const util::ParametersMap& parameters
			, Request& searchRequest
			, StaticActionRequest<messages::AlarmAddLinkAction>& addRequest
			, StaticActionRequest<messages::AlarmRemoveLinkAction>& removeRequest
		){
			Env env;
			vector<shared_ptr<DisplayScreen> > dsv(
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

				for (vector<shared_ptr<DisplayScreen> >::iterator dsit = dsv.begin(); dsit != dsv.end(); ++dsit)
				{
					shared_ptr<DisplayScreen> ds = *dsit;
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

			DisplayScreenTableSync::SearchResult screens(
				DisplayScreenTableSync::Search(
					env,
					searchRequest.getUser()->getProfile()->getRightsForModuleClass<MessagesRight>()
					, searchRequest.getUser()->getProfile()->getGlobalPublicRight<MessagesRight>() >= READ
					, WRITE
					, optional<RegistryKeyType>()
					, optional<RegistryKeyType>()
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

			BOOST_FOREACH(const shared_ptr<DisplayScreen>& screen, screens)
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
				stream << t1.col() << (screen->getType() ? screen->getType()->getName() : "inconnu");
				stream << t1.col();
				stream << t1.col();
				stream << t1.col() << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
			}

			stream << t1.close();
		}

		AlarmRecipientSearchFieldsMap DisplayScreenAlarmRecipient::getSearchFields(HTMLForm& form, const ParametersMap& parameters) const
		{
			shared_ptr<const JourneyPattern> line;
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



		boost::shared_ptr<security::Right> DisplayScreenAlarmRecipient::getRight( const std::string& perimeter ) const
		{
			ArrivalDepartureTableRight* result(new ArrivalDepartureTableRight);
			result->setParameter(perimeter);
			return shared_ptr<Right>(result);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Gets the tree of available recipients.
		AlarmRecipient::AvailableRecipients::Tree::value_type DisplayScreenAlarmRecipient::getAvailableRecipients() const
		{
			// Root item
			shared_ptr<AvailableRecipients> result(new AvailableRecipients);
			result->id = 0;
			result->name = "Points de diffusion";
			
			// Display screens
			shared_ptr<AvailableRecipients> displayScreens(new AvailableRecipients);
			displayScreens->id = DisplayScreenTableSync::TABLE.ID;
			displayScreens->name = "Bornes d'information voyageur";
			result->tree.push_back(displayScreens);

			// Loop on display screens
			BOOST_FOREACH(
				const Registry<DisplayScreen>::value_type& item,
				Env::GetOfficialEnv().getRegistry<DisplayScreen>()
			){
				shared_ptr<AvailableRecipients> displayScreen(new AvailableRecipients);
				displayScreen->id = item.first;
				displayScreen->name = item.second->getName();
				displayScreens->tree.push_back(displayScreen);
			}

			// Custom broadcast points
			shared_ptr<AvailableRecipients> customBroadcastPoints(new AvailableRecipients);
			customBroadcastPoints->id = CustomBroadcastPoint::CLASS_NUMBER;
			customBroadcastPoints->name = "Autres points de diffusion";
			result->tree.push_back(customBroadcastPoints);

			// Loop on custom broadcast points
			BOOST_FOREACH(
				const Registry<CustomBroadcastPoint>::value_type& item,
				Env::GetOfficialEnv().getRegistry<CustomBroadcastPoint>()
			){
				// Jump over non root elements
				if(item.second->getRoot())
				{
					continue;
				}

				// Add the broadcast point and its children
				customBroadcastPoints->tree.push_back(
					_addCustomBroadcastPointToAvailableRecipient(
						*item.second
				)	);
			}

			// Return the result
			return result;
		}



		shared_ptr<AlarmRecipient::AvailableRecipients> DisplayScreenAlarmRecipient::_addCustomBroadcastPointToAvailableRecipient(
			const messages::CustomBroadcastPoint& broadcastPoint
		){
			// Registration
			shared_ptr<AvailableRecipients> customBroadcastPoint(new AvailableRecipients);
			customBroadcastPoint->id = broadcastPoint.getKey();
			customBroadcastPoint->name = broadcastPoint.get<Name>();

			// Recursion
			BOOST_FOREACH(
				const CustomBroadcastPoint::ChildrenType::value_type& child,
				broadcastPoint.getChildren()
			){
				customBroadcastPoint->tree.push_back(
					_addCustomBroadcastPointToAvailableRecipient(
						*child.second
				)	);
			}

			// Return result
			return customBroadcastPoint;
		}
}	}
