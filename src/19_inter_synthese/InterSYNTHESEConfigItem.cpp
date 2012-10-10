
/** InterSYNTHESEConfigItem class implementation.
	@file InterSYNTHESEConfigItem.cpp

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

#include "InterSYNTHESEConfigItem.hpp"

#include "Factory.h"
#include "InterSYNTHESESyncTypeFactory.hpp"

namespace synthese
{
	using namespace inter_synthese;
	using namespace util;

	CLASS_DEFINITION(InterSYNTHESEConfigItem, "t095_inter_synthese_config_items", 95)
	FIELD_DEFINITION_OF_OBJECT(InterSYNTHESEConfigItem, "config_item_id", "config_item_ids")

	FIELD_DEFINITION_OF_TYPE(SyncParameters, "parameters", SQL_TEXT)

	namespace inter_synthese
	{


		InterSYNTHESEConfigItem::InterSYNTHESEConfigItem(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<InterSYNTHESEConfigItem, InterSYNTHESEConfigItemRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(SyncType),
					FIELD_DEFAULT_CONSTRUCTOR(SyncParameters)
			)	)
		{
		}




		void InterSYNTHESEConfigItem::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}



		void InterSYNTHESEConfigItem::unlink()
		{

		}



		const InterSYNTHESESyncTypeFactory& InterSYNTHESEConfigItem::getInterSYNTHESE() const
		{
			if(	!_interSYNTHESE.get() ||
				_interSYNTHESE->getFactoryKey() != get<SyncType>()
			){
				_interSYNTHESE.reset(
					Factory<InterSYNTHESESyncTypeFactory>::create(get<SyncType>())
				);
			}
			return *_interSYNTHESE;
		}
}	}

