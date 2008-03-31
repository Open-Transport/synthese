
/** TransportNetworkTableSync class header.
	@file TransportNetworkTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_TRANSPORTNETWORKTABLESYNC_H
#define SYNTHESE_ENVLSSQL_TRANSPORTNETWORKTABLESYNC_H

#include <string>
#include <iostream>

#include "TransportNetwork.h"

#include "02_db/SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		/** TransportNetwork SQLite table synchronizer.
			@ingroup m15LS refLS
		*/
		class TransportNetworkTableSync : public db::SQLiteRegistryTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>
		{
		 public:
			 static const std::string COL_NAME;

			TransportNetworkTableSync ();

			/** Network search.
				@param name Part of the name of the server
				@param first First Line object to answer
				@param number Number of Line objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<shared_ptr<TransportNetwork>> Founded network objects.
				@author Hugues Romain
				@date 2007
			*/
			static std::vector<boost::shared_ptr<TransportNetwork> > search(
				std::string name = std::string()
				, int first = 0
				, int number = 0
				, bool orderByName = true
				, bool raisingOrder = true
				);
		};
	}
}

#endif
