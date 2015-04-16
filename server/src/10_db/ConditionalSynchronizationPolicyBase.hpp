
/** ConsitionalSynchronizationPolicyBase class header.
	@file ConsitionalSynchronizationPolicyBase.hpp

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

#ifndef SYNTHESE_db_ConditionalSynchronizationPolicyBase_hpp__
#define SYNTHESE_db_ConditionalSynchronizationPolicyBase_hpp__

namespace synthese
{
	namespace db
	{
		/** ConsitionalSynchronizationPolicyBase class.
			@ingroup m10
		*/
		class ConditionalSynchronizationPolicyBase
		{
		public:
			ConditionalSynchronizationPolicyBase() {}

			virtual bool isLoaded(const util::Registrable& object) const = 0;
			virtual void loadCurrentData() const = 0;

			virtual bool needsToReload() const = 0;

			virtual void removeObjects(const RowIdList& rowIds) const = 0;
		};
	}
}

#endif // SYNTHESE_db_ConsitionalSynchronizationPolicyBase_hpp__

