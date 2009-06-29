
/** PTUseRulesTableSync class header.
	@file PTUseRulesTableSync.h

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

#ifndef SYNTHESE_PTUseRulesTableSync_H__
#define SYNTHESE_PTUseRulesTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace pt
	{
		class PTUseRule;
		
		/** Public transportation use rules 
		table synchronizer.
			@ingroup m35LS refLS
		*/
		class PTUseRuleTableSync
		:	public db::SQLiteRegistryTableSyncTemplate<PTUseRuleTableSync,PTUseRule>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_CAPACITY;
			static const std::string COL_RESERVATION_TYPE;
			static const std::string COL_ORIGINISREFERENCE;
			static const std::string COL_MINDELAYMINUTES;
			static const std::string COL_MINDELAYDAYS;
			static const std::string COL_MAXDELAYDAYS;
			static const std::string COL_HOURDEADLINE;
			static const std::string COL_DEFAULT_FARE;
			
			PTUseRuleTableSync();
		};
	}
}

#endif // SYNTHESE_ServiceDateTableSync_H__
