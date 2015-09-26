
/** StopAreaAlarmRecipient class implementation.
	@file StopAreaAlarmRecipient.cpp

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

#include "StopAreaAlarmRecipient.hpp"

#include "City.h"
#include "ResultHTMLTable.h"
#include "SecurityConstants.hpp"
#include "StopPoint.hpp"
#include "User.h"
#include "Profile.h"
#include "HTMLList.h"
#include "StopArea.hpp"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "PTModule.h"
#include "Alarm.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "AlarmObjectLinkException.h"
#include "AdminModule.h"
#include "PTPlacesAdmin.h"
#include "TransportNetwork.h"
#include "ImportableTableSync.hpp"
#include "TransportNetworkRight.h"

#include <boost/foreach.hpp>
#include <vector>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace geography;
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
		template<> const std::string FactorableTemplate<AlarmRecipient, StopAreaAlarmRecipient>::FACTORY_KEY("stoparea");
	}

	namespace messages
	{
		template<> const string AlarmRecipientTemplate<StopAreaAlarmRecipient>::TITLE("Arrêts");

		template<> void AlarmRecipientTemplate<StopAreaAlarmRecipient>::GetParametersLabels(
			ParameterLabelsVector& m
		){
			m.push_back(make_pair(FACTORY_KEY +"/"+ GLOBAL_PERIMETER,"(tous les arrêts)"));
		}



		template<>
		util::RegistryKeyType AlarmRecipientTemplate<StopAreaAlarmRecipient>::GetObjectIdBySource(
			const impex::DataSource& source,
			const std::string& key,
			util::Env& env
		){
			ImportableTableSync::ObjectBySource<StopAreaTableSync> stops(source, env);
			ImportableTableSync::ObjectBySource<StopAreaTableSync>::Set stopsSet(stops.get(key));
			if(stopsSet.empty())
			{
				throw Exception("No such stop");
			}
			return (*stopsSet.begin())->getKey();
		}

	}


	namespace pt
	{
		const string StopAreaAlarmRecipient::PARAMETER_SEARCH_STOP_AREA("ss");

		void StopAreaAlarmRecipient::displayBroadcastListEditor(
			std::ostream& stream,
			const messages::Alarm* alarm,
			const util::ParametersMap& parameters,
			Request& searchRequest,
			StaticActionRequest<messages::AlarmAddLinkAction>& addRequest,
			StaticActionRequest<messages::AlarmRemoveLinkAction>& removeRequest
		){
			Env env;
			StopAreaTableSync::SearchResult dsv(
				AlarmObjectLinkTableSync::search<StopAreaTableSync, StopArea>(
					env,
					alarm->getKey(),
					this->getFactoryKey()
			)	);
			set<util::RegistryKeyType> usedStops;

			stream << "<h1>Arrêts destinataires</h1>";

			if (dsv.empty())
			{
				stream << "<p>Aucun arrêt destinataire</p>";
			}
			else
			{
				HTMLList l;
				stream << l.open();

				// Loop on lines
				for (StopAreaTableSync::SearchResult::iterator dsit = dsv.begin(); dsit != dsv.end(); ++dsit)
				{
					boost::shared_ptr<StopArea> ds = *dsit;

					// Right check
//					if(!getRight()->perimeterIncludes(lexical_cast<string>(ds->getKey()))
//					{
//						continue;
//					}

					usedStops.insert(ds->getKey());
					removeRequest.getAction()->setObjectId(ds->getKey());

					stream << l.element("chart_line");
					stream << HTMLModule::getHTMLLink(removeRequest.getURL(), HTMLModule::getHTMLImage("/admin/img/delete.png","Supprimer"));

					stream << ds->getFullName();
				}

				stream << l.close();
			}

			stream << "<h1>Ajout d'arrêt</h1>";

			string searchName(parameters.getDefault<string>(PTPlacesAdmin::PARAM_SEARCH_NAME));
			string searchCity(parameters.getDefault<string>(PTPlacesAdmin::PARAM_SEARCH_CITY));

			PTPlacesAdmin::getHTMLStopAreaSearchForm(
				stream,
				searchRequest.getHTMLForm(),
				searchCity,
				searchName
			);

			StopAreaTableSync::SearchResult stops(
				StopAreaTableSync::Search(
					Env::GetOfficialEnv(),
					optional<RegistryKeyType>(),
					boost::logic::indeterminate,
					optional<string>(),
					string("%"+searchName+"%"),
					string("%"+searchCity+"%")
			)	);

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "N°"));
			h.push_back(make_pair(PTPlacesAdmin::PARAM_SEARCH_CITY, "Commune"));
			h.push_back(make_pair(PTPlacesAdmin::PARAM_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(
				h,
				searchRequest.getHTMLForm(),
				ResultHTMLTable::RequestParameters(),
				stops
			);

			stream << t.open();
			BOOST_FOREACH(const boost::shared_ptr<StopArea>& stop, stops)
			{
				if (usedStops.find(stop->getKey()) != usedStops.end())
				{
					continue;
				}
				addRequest.getAction()->setObjectId(stop->getKey());

				stream << t.row();
				stream << t.col();
				stream << stop->getCity()->getName();
				stream << t.col();
				stream << stop->getName();
				stream << t.col();
				stream << t.col() << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
			}

			stream << t.close();
		}



		AlarmRecipientSearchFieldsMap StopAreaAlarmRecipient::getSearchFields(HTMLForm& form, const ParametersMap& parameters) const
		{
			AlarmRecipientSearchFieldsMap map;
/*			boost::shared_ptr<const JourneyPattern> line;
			Env env;
			optional<RegistryKeyType> id(parameters.getOptional<RegistryKeyType>(PARAMETER_SEARCH_LINE));
			if (id)
			{
				line  = JourneyPatternTableSync::Get(*id, env, FIELDS_ONLY_LOAD_LEVEL);
			}

			AlarmRecipientFilter arf;

			arf.label = "Ligne";
			arf.htmlField = form.getSelectInput(
				PARAMETER_SEARCH_LINE,
				DeparturesTableModule::getCommercialLineWithBroadcastLabels(true),
				line ? line->getKey() : optional<RegistryKeyType>()
			);
			arf.query = "";
			map.insert(make_pair(PARAMETER_SEARCH_LINE, arf));
*/
			return map;
		}



		boost::shared_ptr<security::Right> StopAreaAlarmRecipient::getRight( const std::string& perimeter ) const
		{
			TransportNetworkRight* result(new TransportNetworkRight);
			result->setParameter(perimeter);
			return boost::shared_ptr<Right>(result);
		}
}	}
