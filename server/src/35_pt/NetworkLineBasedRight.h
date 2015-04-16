
/** NetworkLineBasedRight class header.
	@file NetworkLineBasedRight.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_env_NetworkLineBasedRight_H__
#define SYNTHESE_env_NetworkLineBasedRight_H__

#include "TransportNetworkTableSync.h"
#include "TransportNetwork.h"
#include "CommercialLineTableSync.h"
#include "StopAreaTableSync.hpp"
#include "CommercialLine.h"
#include "AdvancedSelectTableSync.h"
#include "PTModule.h"

#include "SecurityConstants.hpp"
#include "RightTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;
	}

	namespace pt
	{
		/** Network and commercial line based perimeter right, for derivating purposes.
			@ingroup m35

			Perimeter definition :
				- CommercialLine object : the rules apply only on reservation for the specified line
				- TransportNetwork : the rules apply only on reservations for lines of the specified network
		*/
		template<class T>
		class NetworkLineBasedRight : public security::RightTemplate<T>
		{
		public:
			static security::ParameterLabelsVector _getStaticParametersLabels()
			{
				security::ParameterLabelsVector m;
				m.push_back(make_pair(security::GLOBAL_PERIMETER, "(all)"));
				pt::PTModule::getNetworkLinePlaceRightParameterList(m);
				return m;
			}


			/** Displays the interpretation of the string perimeter of the right.
				@return description of the perimeter
			*/
			std::string	displayParameter(
				util::Env& env = util::Env::GetOfficialEnv()
			) const;


			/** Test of inclusion of an other perimeter by the current one.
				@param perimeter to compare with
				@return true if the current perimeter includes the compared one, false else
			*/
			bool perimeterIncludes(
				const std::string& perimeter,
				util::Env& env = util::Env::GetOfficialEnv()
			) const;
		};

		template<class T>
		std::string NetworkLineBasedRight<T>::displayParameter(
			util::Env& env
		) const	{
			if (this->_parameter == security::GLOBAL_PERIMETER)
				return "all";

			util::RegistryKeyType id(boost::lexical_cast<util::RegistryKeyType>(this->_parameter));
			try
			{
				util::RegistryTableType tableId(util::decodeTableId(id));

				if (tableId == pt::TransportNetworkTableSync::TABLE.ID)
				{
					boost::shared_ptr<const pt::TransportNetwork> network(
						pt::TransportNetworkTableSync::Get(id, env)
					);
					return network->getName();
				}

				if (tableId == CommercialLineTableSync::TABLE.ID)
				{
					boost::shared_ptr<const CommercialLine> line(CommercialLineTableSync::Get(id, env));
					return line->getName();
				}
			}
			catch (...)
			{
			}

			return "unknown";
		}

		template<class T>
		bool NetworkLineBasedRight<T>::perimeterIncludes(
			const std::string& perimeter,
			util::Env& env
		) const	{
			if (this->_parameter == security::GLOBAL_PERIMETER)
				return true;

			if (perimeter == security::GLOBAL_PERIMETER)
				return false;

			util::RegistryKeyType id1(boost::lexical_cast<util::RegistryKeyType>(this->_parameter));
			util::RegistryKeyType id2 = boost::lexical_cast<util::RegistryKeyType>(perimeter);

			try
			{
				util::RegistryTableType tableId1(util::decodeTableId(id1));
				if (tableId1 == pt::TransportNetworkTableSync::TABLE.ID)
				{
					util::RegistryTableType tableId2(util::decodeTableId(id2));
					if (tableId2 == pt::TransportNetworkTableSync::TABLE.ID)
						return id1 == id2;
					if (tableId2 == CommercialLineTableSync::TABLE.ID)
					{
						boost::shared_ptr<const CommercialLine> line(CommercialLineTableSync::Get(id2, env, util::UP_LINKS_LOAD_LEVEL));
						boost::shared_ptr<const pt::TransportNetwork> network(
							pt::TransportNetworkTableSync::Get(id1, env)
						);
						return line->getNetwork() == network.get();
					}
					if (tableId2 == StopAreaTableSync::TABLE.ID)
						return isPlaceServedByNetwork(id1, id2);
				}
				if (tableId1 == CommercialLineTableSync::TABLE.ID)
				{
					util::RegistryTableType tableId2(util::decodeTableId(id2));
					if (tableId2 == CommercialLineTableSync::TABLE.ID)
						return id1 == id2;
					if (tableId2 == StopAreaTableSync::TABLE.ID)
						return isPlaceServedByCommercialLine(id1, id2);
				}
			}
			catch(...)
			{

			}

			return false;
		}

	}
}

#endif // SYNTHESE_env_NetworkLineBasedRight_H__
