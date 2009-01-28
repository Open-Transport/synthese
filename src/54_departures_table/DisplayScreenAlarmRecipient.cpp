
/** DisplayScreenAlarmRecipient class implementation.
	@file DisplayScreenAlarmRecipient.cpp

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

// departurestable
#include "DisplayScreenAlarmRecipient.h"
#include "AlarmTestOnDisplayScreenFunction.h"
#include "DeparturesTableModule.h"
#include "DisplaySearchAdmin.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayType.h"
#include "DisplayScreen.h"

// Std
#include <vector>

// html
#include "ResultHTMLTable.h"
#include "HTMLList.h"

// security
#include "12_security/Constants.h"
#include "User.h"
#include "Profile.h"

// transport
#include "PublicTransportStopZoneConnectionPlace.h"
#include "Line.h"
#include "LineTableSync.h"
#include "EnvModule.h"

// messages
#include "Alarm.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "MessagesRight.h"
#include "AlarmObjectLinkException.h"

// admin
#include "AdminModule.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace messages;
	using namespace util;
	using namespace departurestable;
	using namespace server;
	using namespace env;
	using namespace admin;
	using namespace interfaces;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const std::string FactorableTemplate<AlarmRecipient, DisplayScreenAlarmRecipient>::FACTORY_KEY("displayscreen");
	}

	namespace messages
	{
	    /* IMPORTANT : for some reason, probably a gcc bug, is was necessary to 
	       explicitly call constructor with () in order to avoid undefined references.
	       This should be investigate further.
	    */

	    template<> AlarmRecipientTemplate<DisplayScreen, DisplayScreenAlarmRecipient>::AlarmLinks
	    AlarmRecipientTemplate<DisplayScreen, DisplayScreenAlarmRecipient>::_linksAlarm =
			std::map<const SentAlarm*, std::set<const DisplayScreen*> > ();

	    template<> AlarmRecipientTemplate<DisplayScreen, DisplayScreenAlarmRecipient>::ObjectLinks
	    AlarmRecipientTemplate<DisplayScreen, DisplayScreenAlarmRecipient>::_linksObject = 
			std::map<const DisplayScreen*, std::set<const SentAlarm*> > ();

		template<> const string AlarmRecipientTemplate<DisplayScreen, DisplayScreenAlarmRecipient>::TITLE("Afficheurs");

		template<> void AlarmRecipientTemplate<DisplayScreen, DisplayScreenAlarmRecipient>::getStaticParametersLabels(ParameterLabelsVector& m)
		{
			m.push_back(make_pair(GLOBAL_PERIMETER,"(tous les afficheurs)"));
			EnvModule::getNetworkLinePlaceRightParameterList(m);
		}

	}


	namespace departurestable
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
			, const server::ParametersMap& parameters
			, server::FunctionRequest<admin::AdminRequest>& searchRequest
			, server::ActionFunctionRequest<messages::AlarmAddLinkAction,admin::AdminRequest>& addRequest
			, server::ActionFunctionRequest<messages::AlarmRemoveLinkAction, admin::AdminRequest>& removeRequest
		){
			Env env;
			vector<shared_ptr<DisplayScreen> > dsv = AlarmObjectLinkTableSync::search<DisplayScreenTableSync,DisplayScreen> (env, alarm, this->getFactoryKey());
			set<uid> usedDisplayScreens;

			FunctionRequest<AlarmTestOnDisplayScreenFunction> testRequest(&addRequest);
			testRequest.getFunction()->setAlarmId(alarm->getKey());

			stream << "<h2>Test du message</h2>";
			
			HTMLForm testForm(testRequest.getHTMLForm("testForm"));
			stream << testForm.open() << "<p>";
			stream << "Type d'afficheur à tester : " << testForm.getSelectInput(AlarmTestOnDisplayScreenFunction::PARAMETER_DISPLAY_TYPE_ID, DeparturesTableModule::getDisplayTypeLabels(), static_cast<uid>(UNKNOWN_VALUE));
			stream << testForm.getSubmitOnPopupLink(HTMLModule::getHTMLImage("accept.png", "OK"), 800, 600);
			stream << "</p>" << testForm.close();

			stream << "<h2>Afficheurs destinataires</h2>";

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
					stream << HTMLModule::getHTMLLink(removeRequest.getURL(), HTMLModule::getHTMLImage("delete.png","Supprimer"));
					
					stream << ds->getLocalization()->getFullName() << "/" << ds->getLocalization()->getName();
					if (ds->getLocalizationComment() != "")
						stream << "/" << ds->getLocalizationComment();
				}

				stream << l.close();
			}

			stream << "<h2>Recherche d'afficheur à ajouter</h2>";

			string searchCity(parameters.getString(PARAMETER_SEARCH_CITY_NAME, false, FACTORY_KEY));
			string searchStop(parameters.getString(PARAMETER_SEARCH_STOP_NAME, false, FACTORY_KEY));
			string searchName(parameters.getString(PARAMETER_SEARCH_NAME, false, FACTORY_KEY));
			uid searchLine = UNKNOWN_VALUE;
			uid searchType = UNKNOWN_VALUE;
			int searchState = UNKNOWN_VALUE;
			int searchMessage = UNKNOWN_VALUE;

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


			ResultHTMLTable::HeaderVector v1;
			v1.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			v1.push_back(make_pair(PARAMETER_SEARCH_TYPE, "Type"));
			v1.push_back(make_pair(PARAMETER_SEARCH_STATUS, "Etat"));
			v1.push_back(make_pair(PARAMETER_SEARCH_MESSAGE, "Msg"));
			v1.push_back(make_pair(string(), "Add"));
			ResultHTMLTable t1(v1,searchRequest.getHTMLForm(), ResultHTMLTable::RequestParameters(), ResultHTMLTable::ResultParameters());

			stream << "<h2>Résultats de la recherche :</h2>";

			stream << t1.open();

			DisplayScreenTableSync::Search(
				env,
				searchRequest.getUser()->getProfile()->getRightsForModuleClass<MessagesRight>()
				, searchRequest.getUser()->getProfile()->getGlobalPublicRight<MessagesRight>() >= READ
				, WRITE
				, UNKNOWN_VALUE
				, UNKNOWN_VALUE
				, searchLine
				, searchType
				, searchCity
				, searchStop
				, searchName
				, searchState
				, searchMessage
			);
			BOOST_FOREACH(shared_ptr<DisplayScreen> screen, env.getRegistry<DisplayScreen>())
			{
				if (screen->getLocalization() == NULL)
					continue;
				if (usedDisplayScreens.find(screen->getKey()) != usedDisplayScreens.end())
					continue;

				addRequest.getAction()->setObjectId(screen->getKey());

				stream << t1.row(Conversion::ToString(screen->getKey()));
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
			shared_ptr<const Line> line;
			Env env;
			uid id(parameters.getUid(PARAMETER_SEARCH_LINE, false, FACTORY_KEY));
			if (id != UNKNOWN_VALUE)
			{
				line  = LineTableSync::Get(id, env, FIELDS_ONLY_LOAD_LEVEL);
			}

			AlarmRecipientSearchFieldsMap map;
			AlarmRecipientFilter arf;

			arf.label = "Ligne";
			arf.htmlField = form.getSelectInput(PARAMETER_SEARCH_LINE, DeparturesTableModule::getCommercialLineWithBroadcastLabels(true), line ? line->getKey() : UNKNOWN_VALUE);
			arf.query = "";
			map.insert(make_pair(PARAMETER_SEARCH_LINE, arf));

			return map;
		}

		void DisplayScreenAlarmRecipient::addObject(const SentAlarm* alarm, uid objectId )
		{
			try
			{
				add(Env::GetOfficialEnv().getRegistry<DisplayScreen>().get(objectId).get(), alarm);
			}
			catch(...)
			{
				throw AlarmObjectLinkException(objectId, alarm->getKey(), "Display screen not found");
			}
		}

		void DisplayScreenAlarmRecipient::removeObject(const SentAlarm* alarm, uid objectId )
		{
			Env env;
			remove(DisplayScreenTableSync::Get(objectId, env).get(), alarm);
		}
	}
}
