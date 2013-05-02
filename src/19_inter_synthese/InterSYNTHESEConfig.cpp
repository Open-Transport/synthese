
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

#include "InterSYNTHESEConfigItem.hpp"
#include "InterSYNTHESEContent.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace inter_synthese;
	using namespace util;

	CLASS_DEFINITION(InterSYNTHESEConfig, "t094_inter_synthese_configs", 94)
	FIELD_DEFINITION_OF_OBJECT(InterSYNTHESEConfig, "config_id", "config_ids")

	FIELD_DEFINITION_OF_TYPE(LinkBreakMinutes, "link_break_minutes", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaxQueriesNumber, "max_queries_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ForceDump, "force_dump", SQL_BOOLEAN)
	
	namespace inter_synthese
	{
		const string InterSYNTHESEConfig::TAG_ITEM = "item";



		InterSYNTHESEConfig::InterSYNTHESEConfig(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<InterSYNTHESEConfig, InterSYNTHESEConfigRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_VALUE_CONSTRUCTOR(LinkBreakMinutes, minutes(30)),
					FIELD_VALUE_CONSTRUCTOR(MaxQueriesNumber, 5000),
					FIELD_VALUE_CONSTRUCTOR(ForceDump, false)
			)	)
		{
		}



		void InterSYNTHESEConfig::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}



		void InterSYNTHESEConfig::unlink()
		{

		}



		void InterSYNTHESEConfig::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			BOOST_FOREACH(const Items::value_type& it, _items)
			{
				shared_ptr<ParametersMap> itemPM(new ParametersMap);

				it->toParametersMap(*itemPM);

				map.insert(prefix + TAG_ITEM, itemPM);
			}

		}



		void InterSYNTHESEConfig::enqueueIfInPerimeter(
			const InterSYNTHESEContent& content,
			boost::optional<db::DBTransaction&> transaction
		) const {
			boost::mutex::scoped_lock(_configMutex);
			// Avoid useless check if no slave
			if(_slaves.empty())
			{
				return;
			}

			// Check if the content must be sent to the slaves
			bool mustBeEnqueued(false);
			BOOST_FOREACH(const Items::value_type& item, _items)
			{
				if(item->mustBeEnqueued(
					content.getType(),
					content.getPerimeter()
				)	){
					mustBeEnqueued = true;
					break;
				}
			}
			if(!mustBeEnqueued)
			{
				return;
			}

			// Enqueue in all slaves
			BOOST_FOREACH(const Slaves::value_type& slave, _slaves)
			{
				slave->enqueue(
					content.getType().getFactoryKey(),
					content.getContent(),
					transaction
				);
			}
		}
}	}

