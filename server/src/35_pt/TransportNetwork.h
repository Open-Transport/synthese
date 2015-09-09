
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
		class ReservationContact;

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
			static const std::string DATA_IMAGE;
			static const std::string DATA_COUNTRY_CODE;

		protected:
			calendar::CalendarTemplate* _daysCalendarsParent;
			calendar::CalendarTemplate* _periodsCalendarsParent;
			std::string _name;
			std::string _image;
			std::string _timeZone;
			std::string _lang;
			pt::ReservationContact* _contact;
			pt::ReservationContact* _fareContact;
			std::string _country_code;

		public:

			TransportNetwork (
				util::RegistryKeyType id = 0,
				std::string name = std::string(),
				std::string image = std::string(),
				std::string country_code = std::string()
			);
			~TransportNetwork ();

			//! @name Getters
			//@{
				calendar::CalendarTemplate* getDaysCalendarsParent() const { return _daysCalendarsParent; }
				calendar::CalendarTemplate* getPeriodsCalendarsParent() const { return _periodsCalendarsParent; }
				virtual std::string getName() const { return _name; }
				virtual std::string getImage() const { return _image; }
				std::string getTimezone() const { return _timeZone; }
				std::string getLang() const { return _lang; }
				pt::ReservationContact* getContact() const { return _contact; }
				pt::ReservationContact* getFareContact() const { return _fareContact; }
				std::string getCountryCode() const { return _country_code; }
			//@}

			//! @name Setters
			//@{
				void setDaysCalendarsParent(calendar::CalendarTemplate* value){ _daysCalendarsParent = value; }
				void setPeriodsCalendarsParent(calendar::CalendarTemplate* value){ _periodsCalendarsParent = value; }
				void setName(const std::string& value){ _name = value; }
				void setImage(const std::string& value){ _image = value; }
				void setTimezone(const std::string& value){ _timeZone = value; }
				void setLang(const std::string& value){ _lang = value; }
				void setContact(pt::ReservationContact* value){ _contact = value; }
				void setFareContact(pt::ReservationContact* value){ _fareContact = value; }
				void setCountryCode(std::string value){ _country_code = value; }
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

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			//@}
		};
	}
}

#endif
