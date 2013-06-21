
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

#include "NumericField.hpp"
#include "InterSYNTHESEConfig.hpp"
#include "PtimeField.hpp"
#include "StringField.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/thread/recursive_mutex.hpp"

namespace synthese
{
	namespace db
	{
		class DBTransaction;
	}

	namespace inter_synthese
	{
		class InterSYNTHESEQueue;

		FIELD_STRING(ServerAddress)
		FIELD_STRING(ServerPort)
		FIELD_PTIME(LastActivityReport)

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
			static const std::string TAG_QUEUE_ITEM;
			static const std::string TAG_QUEUE_SIZE;
		public:
		
			/// Chosen registry class.
			typedef util::Registry<InterSYNTHESESlave>	Registry;

			typedef std::map<
				util::RegistryKeyType,
				InterSYNTHESEQueue*
			> Queue;

			typedef std::pair<
				Queue::iterator,
				Queue::iterator
			> QueueRange;

		private:
			mutable Queue _queue;
			mutable QueueRange _lastSentRange;
			mutable boost::recursive_mutex _queueMutex;

			// Keep the previous config at unlink time and use it at link
			// time only if it has changed. Don't forget to unlink it in
			// our destructor
			InterSYNTHESEConfig *_previousConfig;

		public:
			InterSYNTHESESlave(util::RegistryKeyType id = 0);
			~InterSYNTHESESlave();

			//! @name Services
			//@{
				bool isObsolete() const;

				QueueRange getQueueRange() const;

				void enqueue(
					const std::string& interSYNTHESEType,
					const std::string& parameter,
					boost::optional<db::DBTransaction&> transaction,
					bool force = false
				) const;
				void queue(
					InterSYNTHESEQueue& obj
				) const;
				void removeFromQueue(
					util::RegistryKeyType id
				) const;
				Queue& getQueue() const { return _queue; }

				//////////////////////////////////////////////////////////////////////////
				/// Adds parameters that are not intended to be saved (i.e. generated content).
				/// The default implementation adds nothing. This method may be overloaded
				/// @param map the map to populate
				/// @param prefix prefix to add to the keys of the map items
				virtual void addAdditionalParameters(
					util::ParametersMap& map,
					std::string prefix = std::string()
				) const;

				const QueueRange& getLastSentRange() const { return _lastSentRange; }
			//@}

			//! @name Modifiers
			//@{
				void setLastSentRange(const QueueRange& value) const { _lastSentRange = value; }

				void clearLastSentRange() const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_cms_InterSYNTHESESlave_hpp__
