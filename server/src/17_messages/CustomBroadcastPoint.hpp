
/** CustomBroadcastPoint class header.
	@file CustomBroadcastPoint.hpp

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

#ifndef SYNTHESE_messages_CustomBroadcastPoint_hpp__
#define SYNTHESE_messages_CustomBroadcastPoint_hpp__

#include "BroadcastPoint.hpp"
#include "FactorableTemplate.h"
#include "Object.hpp"
#include "TreeNode.hpp"
#include "TreeNodeField.hpp"
#include "TreeRankOrderingPolicy.hpp"
#include "TreeUniqueRootPolicy.hpp"

#include "CMSScriptField.hpp"
#include "MessageType.hpp"

namespace synthese
{
	namespace messages
	{
		class CustomBroadcastPoint;

		FIELD_TREE_NODE(CustomBroadcastPointTreeNode, CustomBroadcastPoint)
		FIELD_CMS_SCRIPT(BroadcastRule)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(CustomBroadcastPointTreeNode),
			FIELD(Name),
			FIELD(MessageType),
			FIELD(BroadcastRule)
		> CustomBroadcastPointRecord;

		/** CustomBroadcastPoint class.
			@ingroup m17
		*/
		class CustomBroadcastPoint:
			public tree::TreeNode<
				CustomBroadcastPoint,
				tree::TreeRankOrderingPolicy,
				tree::TreeUniqueRootPolicy<
					tree::TreeRankOrderingPolicy,
					CustomBroadcastPoint
			>	>,
			public Object<CustomBroadcastPoint, CustomBroadcastPointRecord>,
			public util::FactorableTemplate<BroadcastPoint, CustomBroadcastPoint>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<CustomBroadcastPoint>	Registry;

			CustomBroadcastPoint(
				util::RegistryKeyType id = 0
			);

			virtual MessageType* getMessageType() const;

			virtual bool displaysMessage(
				const Alarm::LinkedObjects& recipients,
				const util::ParametersMap& parameters
			) const;

			virtual void getBroadcastPoints(BroadcastPoints& result) const;

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();
		};
	}
}

#endif // SYNTHESE_messages_CustomBroadcastPoint_hpp__

