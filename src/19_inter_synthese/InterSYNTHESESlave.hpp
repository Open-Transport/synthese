
/** InterSYNTHESESlave class header.
	@file InterSYNTHESESlave.hpp

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

#ifndef SYNTHESE_cms_InterSYNTHESESlave_hpp__
#define SYNTHESE_cms_InterSYNTHESESlave_hpp__

#include "Object.hpp"

#include "InterSYNTHESEConfig.hpp"
#include "StandardFields.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESEQueue;

		FIELD_TYPE(ServerAddress, std::string)
		FIELD_TYPE(ServerPort, std::string)
		FIELD_TYPE(LastActivityReport, boost::posix_time::ptime)
		FIELD_TYPE(Active, bool)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(ServerAddress),
			FIELD(ServerPort),
			FIELD(LastActivityReport),
			FIELD(InterSYNTHESEConfig),
			FIELD(Active)
		> InterSYNTHESESlaveRecord;



		//////////////////////////////////////////////////////////////////////////
		/// InterSYNTHESESlave class.
		///	@ingroup m19
		/// @author Hugues Romain
		/// @since 3.5.0
		class InterSYNTHESESlave:
			public Object<InterSYNTHESESlave, InterSYNTHESESlaveRecord>
		{
		public:
		
			/// Chosen registry class.
			typedef util::Registry<InterSYNTHESESlave>	Registry;

			typedef std::map<
				util::RegistryKeyType,
				InterSYNTHESEQueue*
			> Queue;

		private:
			mutable Queue _queue;
		
		public:
			InterSYNTHESESlave(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				void send() const;
				void enqueue(
					const std::string& interSYNTHESEType,
					const std::string& parameter
				) const;
				void queue(
					InterSYNTHESEQueue& obj
				) const;
				void removeFromQueue(
					util::RegistryKeyType id
				) const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_cms_InterSYNTHESESlave_hpp__
