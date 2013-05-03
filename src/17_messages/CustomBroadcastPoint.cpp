
/** CustomBroadcastPoint class implementation.
	@file CustomBroadcastPoint.cpp

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

#include "CustomBroadcastPoint.hpp"

#include "Alarm.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace util;

	CLASS_DEFINITION(CustomBroadcastPoint, "t106_custom_broadcast_points", 106)
	FIELD_DEFINITION_OF_OBJECT(CustomBroadcastPoint, "custom_broadcast_point_id", "custom_broadcast_point_ids")

	FIELD_DEFINITION_OF_TYPE(BroadcastRule, "broadcast_rule", SQL_TEXT)

	template<> const Field ComplexObjectFieldDefinition<CustomBroadcastPointTreeNode>::FIELDS[] = {
		Field("root_id", SQL_INTEGER),
		Field("up_id", SQL_INTEGER),
		Field("rank", SQL_INTEGER),
	Field() };

	namespace util
	{
		template<>
		const string FactorableTemplate<BroadcastPoint, CustomBroadcastPoint>::FACTORY_KEY = "CustomBroadcastPoint";
	}

	namespace messages
	{
		CustomBroadcastPoint::CustomBroadcastPoint(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<CustomBroadcastPoint, CustomBroadcastPointRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(CustomBroadcastPointTreeNode),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(MessageType),
					FIELD_DEFAULT_CONSTRUCTOR(BroadcastRule)
			)	)
		{
		}



		MessageType* CustomBroadcastPoint::getMessageType() const
		{
			return
				get<MessageType>() ?
				&*get<MessageType>() :
				NULL
			;
		}



		bool CustomBroadcastPoint::displaysMessage(
			const Alarm::LinkedObjects& recipients,
			const util::ParametersMap& parameters
		) const	{

			// Populates the parameters map
			ParametersMap pm(parameters);
			Alarm::LinkedObjectsToParametersMap(recipients, pm);

			// Evaluation of the customized rule
			string s(get<BroadcastRule>().eval(pm));
			trim(s);

			// Result must be positive
			return !s.empty() && s != "0";
		}



		void CustomBroadcastPoint::getBrodcastPoints( BroadcastPoints& result ) const
		{
			BOOST_FOREACH(
				const CustomBroadcastPoint::Registry::value_type& it,
				Env::GetOfficialEnv().getRegistry<CustomBroadcastPoint>()
			){
				result.push_back(it.second.get());
			}
		}
}	}

