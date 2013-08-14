
/** TransportNetwork class header.
	@file TransportNetwork.h

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

#ifndef SYNTHESE_ENV_TRANSPORTNETWORK_H
#define SYNTHESE_ENV_TRANSPORTNETWORK_H

#include "Registrable.h"
#include "Registry.h"
#include "PathClass.h"
#include "ImportableTemplate.hpp"
#include "TreeFolderRoot.hpp"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Transport network class.
		/// TRIDENT OK
		///	@ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class TransportNetwork:
			virtual public util::Registrable,
			public graph::PathClass,
			public impex::ImportableTemplate<TransportNetwork>,
			public tree::TreeFolderRoot
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<TransportNetwork> Registry;

		private:
			static const std::string DATA_NETWORK_ID;
			static const std::string DATA_NAME;

		protected:
			calendar::CalendarTemplate* _daysCalendarsParent;
			calendar::CalendarTemplate* _periodsCalendarsParent;
			std::string _name;

		public:

			TransportNetwork (
				util::RegistryKeyType id = 0,
				std::string name = std::string()
			);
			~TransportNetwork ();

			//! @name Getters
			//@{
				calendar::CalendarTemplate* getDaysCalendarsParent() const { return _daysCalendarsParent; }
				calendar::CalendarTemplate* getPeriodsCalendarsParent() const { return _periodsCalendarsParent; }
				virtual std::string getName() const { return _name; }
			//@}

			//! @name Setters
			//@{
				void setDaysCalendarsParent(calendar::CalendarTemplate* value){ _daysCalendarsParent = value; }
				void setPeriodsCalendarsParent(calendar::CalendarTemplate* value){ _periodsCalendarsParent = value; }
				void setName(const std::string& value){ _name = value; }
			//@}

			/// @name Modifiers
			//@{
				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);
			//@}

			//! @name Services
			//@{
				virtual std::string getRuleUserName() const { return "Réseau " + getName(); }

				virtual PathClass::Identifier getIdentifier() const;

				//////////////////////////////////////////////////////////////////////////
				/// Exporter.
				//////////////////////////////////////////////////////////////////////////
				/// @param pm parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual SubObjects getSubObjects() const;
			//@}
		};
	}
}

#endif
