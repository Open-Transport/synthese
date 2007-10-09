
/** NetworkLineBasedRight class header.
	@file NetworkLineBasedRight.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_env_NetworkLineBasedRight_H__
#define SYNTHESE_env_NetworkLineBasedRight_H__

#include "15_env/TransportNetworkTableSync.h"
#include "15_env/TransportNetwork.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/CommercialLine.h"
#include "15_env/EnvModule.h"

#include "12_security/Constants.h"
#include "12_security/RightTemplate.h"

#include "01_util/UId.h"
#include "01_util/Conversion.h"

namespace synthese
{
	namespace security
	{
		class Profile;
	}

	namespace env
	{
		/** Network and commercial line based perimeter right, for derivating purposes.
			@ingroup m15

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
				env::EnvModule::getNetworkLinePlaceRightParameterList(m);
				return m;
			}


			/** Displays the interpretation of the string perimeter of the right.
				@return description of the perimeter
			*/
			std::string	displayParameter()	const;
			

			/** Test of inclusion of an other perimeter by the current one.
				@param perimeter to compare with
				@return true if the current perimeter includes the compared one, false else
			*/
			bool perimeterIncludes(const std::string& perimeter) const;
		};

		template<class T>
		std::string NetworkLineBasedRight<T>::displayParameter() const
		{
			if (this->_parameter == security::GLOBAL_PERIMETER)
				return "all";

			uid id(util::Conversion::ToLongLong(this->_parameter));
			try
			{
				int tableId(util::decodeTableId(id));
				
				if (tableId == TransportNetworkTableSync::TABLE_ID)
				{
					boost::shared_ptr<const TransportNetwork> network(TransportNetwork::Get(id));
					return network->getName();
				}

				if (tableId == CommercialLineTableSync::TABLE_ID)
				{
					boost::shared_ptr<const CommercialLine> line(CommercialLine::Get(id));
					return line->getName();
				}
			}
			catch (...)
			{
			}

			return "unknown";
		}

		template<class T>
		bool NetworkLineBasedRight<T>::perimeterIncludes(const std::string& perimeter) const
		{
			if (this->_parameter == security::GLOBAL_PERIMETER)
				return true;

			uid id1(util::Conversion::ToLongLong(this->_parameter));
			uid id2(util::Conversion::ToLongLong(perimeter));
			try
			{
				int tableId1(util::decodeTableId(id1));
				if (tableId1 == TransportNetworkTableSync::TABLE_ID)
				{
					if (util::decodeTableId(id2) == TransportNetworkTableSync::TABLE_ID)
						return id1 == id2;
					if (util::decodeTableId(id2) == CommercialLineTableSync::TABLE_ID)
					{
						boost::shared_ptr<const CommercialLine> line(CommercialLine::Get(id2));
						boost::shared_ptr<const TransportNetwork> network(TransportNetwork::Get(id1));
						return line->getNetwork() == network.get();
					}
				}
				if (tableId1 == CommercialLineTableSync::TABLE_ID)
				{
					int tableId2(util::decodeTableId(id2));
					if (tableId2 == CommercialLineTableSync::TABLE_ID)
						return id1 == id2;
//					if (tableId2 == ConnectionPlaceTableSync::TABLE_ID)
//						return isPlaceServedByCommercialLine(id1, id2);
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
