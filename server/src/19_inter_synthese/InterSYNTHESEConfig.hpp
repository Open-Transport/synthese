
/** InterSYNTHESEConfig class header.
	@file InterSYNTHESEConfig.hpp

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

#ifndef SYNTHESE_cms_InterSYNTHESEConfig_hpp__
#define SYNTHESE_cms_InterSYNTHESEConfig_hpp__

#include "Object.hpp"

#include "MinutesField.hpp"
#include "SchemaMacros.hpp"
#include "StringField.hpp"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESEConfigItem;
		class InterSYNTHESEContent;
		class InterSYNTHESESlave;

		FIELD_MINUTES(LinkBreakMinutes)
		FIELD_SIZE_T(MaxQueriesNumber)
		FIELD_BOOL(ForceDump)
		FIELD_BOOL(Multimaster)
		FIELD_BOOL(DisableWhereClause)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(LinkBreakMinutes),
			FIELD(MaxQueriesNumber),
			FIELD(ForceDump),
			FIELD(Multimaster),
			FIELD(DisableWhereClause)
		> InterSYNTHESEConfigRecord;



		//////////////////////////////////////////////////////////////////////////
		/// InterSYNTHESEConfig class.
		///	@ingroup m19
		/// @author Hugues Romain
		/// @since 3.5.0
		class InterSYNTHESEConfig:
			public Object<InterSYNTHESEConfig, InterSYNTHESEConfigRecord>
		{
		public:
			static const std::string TAG_ITEM;
		
			/// Chosen registry class.
			typedef util::Registry<InterSYNTHESEConfig>	Registry;

			typedef std::set<InterSYNTHESEConfigItem*> Items;
			typedef std::set<InterSYNTHESESlave*> Slaves;

		private:
			mutable boost::recursive_mutex _configMutex;
			Items _items;
			Slaves _slaves;
		
		public:
			InterSYNTHESEConfig(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				const Items getItems() const
				{
					boost::recursive_mutex::scoped_lock lock(_configMutex);
					return _items;
				}
				const Slaves getSlaves() const
				{
					boost::mutex::scoped_lock(_configMutex);
					return _slaves;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Checks if the sync message should be sent to the slaves of the config.
				/// @param type type of sync message
				/// @param parameter parameter of the message
				void enqueueIfInPerimeter(
					const InterSYNTHESEContent& content,
					boost::optional<db::DBTransaction&> transaction,
					util::Registrable* objectToRemember
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Adds parameters that are not intended to be saved (i.e. generated content).
				/// The default implementation adds nothing. This method may be overloaded
				/// @param map the map to populate
				/// @param prefix prefix to add to the keys of the map items
				virtual void addAdditionalParameters(
					util::ParametersMap& map,
					std::string prefix = std::string()
				) const;
			//@}

			//! @name Modifiers
			//@{
				void insertItem(InterSYNTHESEConfigItem* value)
				{
					boost::recursive_mutex::scoped_lock lock(_configMutex);
					_items.insert(value);
				}
				void insertSlave(InterSYNTHESESlave* value)
				{
					boost::recursive_mutex::scoped_lock lock(_configMutex);
					_slaves.insert(value);
				}
				void eraseItem(InterSYNTHESEConfigItem* value)
				{
					boost::recursive_mutex::scoped_lock lock(_configMutex);
					_items.erase(value);
				}
				void eraseSlave(InterSYNTHESESlave* value)
				{
					boost::recursive_mutex::scoped_lock lock(_configMutex);
					_slaves.erase(value);
				}
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_cms_InterSYNTHESEConfig_hpp__
