
/** BroadcastPointsRight class implementation.
	@file BroadcastPointsRight.cpp

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

#include "34_departures_table/BroadcastPointsRight.h"

#include "15_env/TransportNetworkTableSync.h"
#include "15_env/TransportNetwork.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/CommercialLine.h"

#include "12_security/Constants.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace security;
	using namespace departurestable;
	using namespace env;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, BroadcastPointsRight>::FACTORY_KEY("BroadcastPoints");
	}

	namespace security
	{
		template<> const string RightTemplate<BroadcastPointsRight>::NAME("Gestion de points de diffusion de tableaux de départs");

		template<>
		RightTemplate<BroadcastPointsRight>::ParameterLabelsVector RightTemplate<BroadcastPointsRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER,"(tous les points de diffusion)"));
			m.push_back(make_pair(UNKNOWN_PERIMETER, "--- Réseaux ---"));

			vector<shared_ptr<TransportNetwork> > tn = TransportNetworkTableSync::search();
			for (vector<shared_ptr<TransportNetwork> >::const_iterator itn = tn.begin(); itn != tn.end(); ++itn)
			{
				m.push_back(make_pair(Conversion::ToString((*itn)->getKey()), (*itn)->getName() ));
			}

			m.push_back(make_pair(UNKNOWN_PERIMETER, "--- Lignes ---"));

			vector<shared_ptr<CommercialLine> > tl = CommercialLineTableSync::search();
			for (vector<shared_ptr<CommercialLine> >::const_iterator itl = tl.begin(); itl != tl.end(); ++itl)
			{
				m.push_back(make_pair(Conversion::ToString((*itl)->getKey()), (*itl)->getName() ));
			}

			return m;
		}
	}

	namespace departurestable
	{
		std::string BroadcastPointsRight::displayParameter() const
		{
			uid id = Conversion::ToLongLong(_parameter);
			if (!id)
				return _parameter;
			int tableId = decodeTableId(id);
			if (tableId == TransportNetworkTableSync::TABLE_ID)
			{
				try
				{
					shared_ptr<TransportNetwork> tn = TransportNetworkTableSync::get(id);
					return string("Réseau ") + tn->getName();
				}
				catch (...) {
				}
			}
			else if (tableId == CommercialLineTableSync::TABLE_ID)
			{
				try
				{
					shared_ptr<CommercialLine> cl = CommercialLineTableSync::get(id);
					return string("Ligne ") + cl->getShortName();
				}
				catch (...)
				{					
				}
			}
			return _parameter;
		}

		bool BroadcastPointsRight::perimeterIncludes( const std::string& perimeter ) const
		{
			 return true;
		}
	}
}
