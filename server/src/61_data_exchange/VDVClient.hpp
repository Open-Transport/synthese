
/** VDVClient class header.
	@file VDVClient.hpp

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

#ifndef SYNTHESE_data_exchange_VDVClient_hpp__
#define SYNTHESE_data_exchange_VDVClient_hpp__

#include "Object.hpp"

#include "DataSource.h"
#include "NumericField.hpp"
#include "PointerField.hpp"
#include "SchemaMacros.hpp"
#include "StringField.hpp"
#include "TransportNetwork.h"


#include "VDVServer.hpp" // For DataSourcePointer (remove after refactoring)

#include <map>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;
		class TransportNetwork;
	}

	namespace data_exchange
	{
		class VDVClientSubscription;

		FIELD_STRING(ReplyAddress)
		FIELD_STRING(ReplyPort)
		FIELD_POINTERS_VECTOR(TransportNetworks, pt::TransportNetwork)
		FIELD_STRING(DefaultDirection)
		FIELD_BOOL(SBBMode)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(ReplyAddress),
			FIELD(ReplyPort),
			FIELD(ClientControlCentreCode),
			FIELD(ServerControlCentreCode),
			FIELD(ServiceUrl),
			FIELD(ServiceCode),
			FIELD(impex::DataSource),
			FIELD(TransportNetworks),
			FIELD(DefaultDirection),
			FIELD(Active),
			FIELD(TracePath),
			FIELD(SBBMode)
		> VDVClientRecord;

		/** VDV client.
			@ingroup m61
		*/
		class VDVClient:
			public Object<VDVClient, VDVClientRecord>
		{
		private:
			static const std::string TAG_SUBSCRIPTION;
			static const std::string TAG_TRANSPORT_NETWORK;

		public:
			/// Chosen registry class.
			typedef util::Registry<VDVClient>	Registry;

			typedef std::map<std::string, boost::shared_ptr<VDVClientSubscription> > Subscriptions;
			
		private:
			Subscriptions _subscriptions;
			mutable boost::posix_time::ptime _lastDataReady;

			std::string _getURL(const std::string& request) const;

		public:
			VDVClient(util::RegistryKeyType id = 0);

			void addSubscription(boost::shared_ptr<VDVClientSubscription> subscription);
			const Subscriptions& getSubscriptions() const { return _subscriptions; }
			void removeSubscription(const std::string& key);
			void cleanSubscriptions();
			const boost::posix_time::ptime& getLastDataReady() const { return _lastDataReady; }
			void setLastDataReadyNow() const;
			void clearLastDataReady() const;

			std::string getDirectionID(
				const pt::JourneyPattern& jp
			) const;

			bool checkUpdate() const;

			void sendUpdateSignal() const;

			//////////////////////////////////////////////////////////////////////////
			/// Adds parameters that are not intended to be saved (i.e. generated content).
			/// The default implementation adds nothing. This method may be overloaded
			/// @param map the map to populate
			/// @param prefix prefix to add to the keys of the map items
			virtual void addAdditionalParameters(
				util::ParametersMap& map,
				std::string prefix = std::string()
			) const;


			void trace(
				const std::string& tag,
				const std::string& content
			) const;

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_data_exchange_VDVClient_hpp__

