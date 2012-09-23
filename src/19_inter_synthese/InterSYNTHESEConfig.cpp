
/** InterSYNTHESEConfig class implementation.
	@file InterSYNTHESEConfig.cpp

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

#include "InterSYNTHESEConfig.hpp"

using namespace boost::posix_time;

namespace synthese
{
	using namespace inter_synthese;
	using namespace util;

	CLASS_DEFINITION(InterSYNTHESEConfig, "t094_inter_synthese_configs", 94)
	FIELD_DEFINITION_OF_OBJECT(InterSYNTHESEConfig, "config_id", "config_ids")

	FIELD_DEFINITION_OF_TYPE(LinkBreakMinutes, "link_break_minutes", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaxQueriesNumber, "max_queries_number", SQL_INTEGER)
	
	namespace inter_synthese
	{
		InterSYNTHESEConfig::InterSYNTHESEConfig(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<InterSYNTHESEConfig, InterSYNTHESEConfigRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_VALUE_CONSTRUCTOR(LinkBreakMinutes, minutes(30)),
					FIELD_VALUE_CONSTRUCTOR(MaxQueriesNumber, 5000)
			)	)
		{
		}



		void InterSYNTHESEConfig::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}



		void InterSYNTHESEConfig::unlink()
		{

		}
}	}

