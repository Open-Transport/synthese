
/** InterSYNTHESEQueue class header.
	@file InterSYNTHESEQueue.hpp

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

#ifndef SYNTHESE_cms_InterSYNTHESEQueue_hpp__
#define SYNTHESE_cms_InterSYNTHESEQueue_hpp__

#include "Object.hpp"

#include "InterSYNTHESESlave.hpp"
#include "NumericField.hpp"
#include "PtimeField.hpp"
#include "StringField.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace inter_synthese
	{
		FIELD_PTIME(RequestTime)
		FIELD_PTIME(ExpirationTime)
		FIELD_STRING(SyncType)
		FIELD_STRING(SyncContent)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(InterSYNTHESESlave),
			FIELD(RequestTime),
			FIELD(ExpirationTime),
			FIELD(SyncType),
			FIELD(SyncContent)
		> InterSYNTHESEQueueRecord;



		//////////////////////////////////////////////////////////////////////////
		/// InterSYNTHESEQueue class.
		///	@ingroup m19
		/// @author Hugues Romain
		/// @since 3.5.0
		class InterSYNTHESEQueue:
			public Object<InterSYNTHESEQueue, InterSYNTHESEQueueRecord>
		{
		public:
		
			/// Chosen registry class.
			typedef util::Registry<InterSYNTHESEQueue>	Registry;

		private:
			bool _nonPersistent;
		
		public:
			InterSYNTHESEQueue(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				bool getNonPersistent() const { return _nonPersistent; }
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
				void setNonPersistent(){ _nonPersistent = true; }
			//@}
		};
}	}

#endif // SYNTHESE_cms_InterSYNTHESEQueue_hpp__
