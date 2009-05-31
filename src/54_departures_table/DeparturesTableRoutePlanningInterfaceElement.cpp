
/** DeparturesTableRoutePlanningInterfaceElement class implementation.
	@file DeparturesTableRoutePlanningInterfaceElement.cpp

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

#include <vector>

#include "Conversion.h"
#include "Interface.h"
#include "ValueElementList.h"
#include "InterfacePageException.h"
#include "Request.h"
#include "DeparturesTableTypes.h"
#include "DeparturesTableRoutePlanningInterfaceElement.h"
#include "DeparturesTableRoutePlanningRowInterfacePage.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::LibraryInterfaceElement, departurestable::DeparturesTableRoutePlanningInterfaceElement>::FACTORY_KEY("departurestablerouteplanning");
	}

	namespace departurestable
	{
		void DeparturesTableRoutePlanningInterfaceElement::storeParameters( ValueElementList& vel )
		{
			if(vel.isEmpty())
			{
				throw InterfacePageException("Missing parameter origin ID");
			}
			_originId = vel.front();

			if (!vel.isEmpty())
				_departuresToHide = vel.front();

			if (!vel.isEmpty())
				_displayServiceNumber = vel.front();

			if (!vel.isEmpty())
				_displayQuai = vel.front();

			if (!vel.isEmpty())
				_withTransfer = vel.front();

			if (!vel.isEmpty())
				_blinkingDelay = vel.front();
		}

		string DeparturesTableRoutePlanningInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			const RoutePlanningList& ptds(static_cast<const RoutePlanningListWithAlarm*>(object)->map);
			
			int departuresToHide(_departuresToHide ? Conversion::ToInt(_departuresToHide->getValue(parameters, variables, object, request)) : 0);
			bool displayServiceNumber(_displayServiceNumber ? Conversion::ToBool(_displayServiceNumber->getValue(parameters, variables, object, request)) : false);
			bool displayQuai(_displayQuai ? Conversion::ToBool(_displayQuai->getValue(parameters, variables, object, request)) : false);
			int withtransfer(_withTransfer ? Conversion::ToBool(_withTransfer->getValue(parameters, variables, object, request)) : false);
			int blinkingDelay(_blinkingDelay? Conversion::ToInt(_blinkingDelay->getValue(parameters, variables, object, request)) : 0);
			shared_ptr<const PublicTransportStopZoneConnectionPlace> place(ConnectionPlaceTableSync::Get(lexical_cast<RegistryKeyType>(_originId->getValue(parameters, variables, object, request)), Env::GetOfficialEnv()));
			
			// Lancement de l'affichage de la rangee
			const DeparturesTableRoutePlanningRowInterfacePage* page(_page->getInterface()->getPage<DeparturesTableRoutePlanningRowInterfacePage>());
			if(page == NULL || place.get() == NULL)
			{
				return string();
			}

			// Boucle sur les rangees
			int departuresNumber = ptds.size() - departuresToHide;
			int rank(0);
			for (RoutePlanningList::const_iterator it = ptds.begin(); departuresNumber && (it != ptds.end()); ++it, --departuresNumber, ++rank)
			{
				page->display(
					stream,
					variables,
					rank,
					displayQuai,
					displayServiceNumber,
					blinkingDelay,
					withtransfer,
					*place,
					*it,
					request
				);
			}
			return string();
		}
	}
}
