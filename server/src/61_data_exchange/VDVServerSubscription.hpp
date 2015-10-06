  
/** VDVServerSubscription class header.
	@file VDVServerSubscription.hpp

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

#ifndef SYNTHESE_cms_VDVServerSubscription_hpp__
#define SYNTHESE_cms_VDVServerSubscription_hpp__

#include "Object.hpp"

#include "EnumObjectField.hpp"
#include "MinutesField.hpp"
#include "NumericField.hpp"
#include "StopArea.hpp"
#include "VDVServer.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace data_exchange
	{
		enum VDVSubscriptionTypeEnum
		{
			Dfi = 0,
			Ans = 1
		};

		FIELD_MINUTES(SubscriptionDuration)
		FIELD_MINUTES(TimeSpan)
		FIELD_ENUM(VDVSubscriptionType, VDVSubscriptionTypeEnum)
			
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(VDVServer),
			FIELD(pt::StopArea),
			FIELD(SubscriptionDuration),
			FIELD(TimeSpan),
			FIELD(VDVSubscriptionType)
		> VDVServerSubscriptionRecord;



		//////////////////////////////////////////////////////////////////////////
		/// Subscription to a 3rd party VDV server.
		///	@ingroup m36
		/// @author Hugues Romain
		/// @since 3.5.0
		class VDVServerSubscription:
			public Object<VDVServerSubscription, VDVServerSubscriptionRecord>
		{
		public:
			static const std::string ATTR_ONLINE;
			static const std::string ATTR_EXPIRATION;
		
			/// Chosen registry class.
			typedef util::Registry<VDVServerSubscription>	Registry;


		private:
			mutable boost::posix_time::ptime _expiration;
			mutable bool _online;
		
		public:
			VDVServerSubscription(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				bool getOnline() const { return _online; }
				const boost::posix_time::ptime& getExpiration() const { return _expiration; }
				void addAdditionalParameters(util::ParametersMap& map, std::string prefix) const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
				void setOnline(bool value){ _online = value; }
				void setExpiration(const boost::posix_time::ptime& value){ _expiration = value; }
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_cms_VDVClientItem_hpp__
