////////////////////////////////////////////////////////////////////////////////
///	Calendar class header.
///	@file Calendar.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_ENV_CALENDAR_H
#define SYNTHESE_ENV_CALENDAR_H

#include "Registrable.h"

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <bitset>
#include <map>
#include <vector>
#include <boost/optional/optional.hpp>
#include <set>
#include <boost/thread/recursive_mutex.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarLink;

		/** Calendar described by an array of booleans (one per day).
			@ingroup m31

			The Calendar class implements the service calendar, holding a
			bitset representing year days. Each year day can be activated or not.

			The active dates of the calendar are marked as activated bits on a bitset per year.
			Each year containing at least one activated date has a full bitset.
			In a year bitset, a date is represented by the bit at the rank corresponding to its day number in the year.
			Example : 2008-02-03 => bit 34 of the 2008 bitset.
		*/
		class Calendar:
			public virtual util::Registrable
		{
		public:
			typedef std::vector<boost::gregorian::date> DatesVector;
			typedef std::set<boost::gregorian::date> DatesSet;
			typedef std::set<CalendarLink*> CalendarLinks;

		private:
			class BitSets
			{
			public:
				typedef std::map<
					boost::gregorian::greg_year,
					std::bitset<366>
				> _BitSets;

			private:
				_BitSets _value;

				static size_t _BitPos(const boost::gregorian::date& d);

			public:

				BitSets(
					const boost::gregorian::date& firstDate,
					const boost::gregorian::date& lastDate,
					boost::gregorian::date_duration step = boost::gregorian::days(1)
				);
				BitSets();
				BitSets& operator|= (const BitSets& op);
				BitSets& operator&= (const BitSets& op);
				bool operator==(const BitSets& op) const;
				bool operator!=(const BitSets& op) const;
				BitSets& operator-= (const BitSets& op);
				const _BitSets getValue() const { return _value; }
				boost::gregorian::date getFirstActiveDate() const;
				boost::gregorian::date getLastActiveDate() const;
				bool empty() const;
				DatesVector getActiveDates () const;
				bool isActive(
					const boost::gregorian::date& date
				) const;
				void setActive(const boost::gregorian::date& date);
				void setInactive(const boost::gregorian::date& date);
				void clear();
				bool hasAtLeastOneCommonDateWith(const BitSets& op) const;
				size_t size() const;
				void copyDates(const BitSets& calendar);
				void serialize(std::ostream& stream) const;
				void setFromSerializedString(const std::string& value);
				BitSets& operator<<= (std::size_t i);
			};

			/// @name Base data
			//@{
				BitSets _markedDates;
				DatesSet _datesToForce;
				DatesSet _datesToBypass;
				CalendarLinks _calendarLinks;
			//@}
		
			mutable boost::recursive_mutex _mutex;
			mutable boost::optional<boost::gregorian::date> _firstActiveDate;
			mutable boost::optional<boost::gregorian::date> _lastActiveDate;
			mutable boost::optional<BitSets> _datesCache; ///< Dates coming from the links or the marked dates
		
			void _resetDatesCache() const;

			const BitSets& _getDatesCache() const;


		public:
			//! @name Constructors
			//@{
				Calendar(
					util::RegistryKeyType id = 0
				);



				//////////////////////////////////////////////////////////////////////////
				/// Sets dates in a range with a specific step.
				/// @param fistDate start of the range
				/// @param lastDate end of the range
				/// @param step step
				/// @pre firstDate <= lastDate, and are all defined; step is positive days duration
				Calendar(
					const boost::gregorian::date& firstDate,
					const boost::gregorian::date& lastDate,
					boost::gregorian::date_duration step = boost::gregorian::days(1)
				);



				//////////////////////////////////////////////////////////////////////////
				/// Copy constructor.
				Calendar(
					const Calendar& other
				);

				//////////////////////////////////////////////////////////////////////////
				/// Constructs a calendar upon a serialized string.
				/// @param serialized the serialized string
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				///
				/// The serialized string comes from Calendar::serialize.
				Calendar(
					const std::string& serialized
				);
			//@}

			//! @name Getters
			//@{
				const CalendarLinks& getCalendarLinks() const { return _calendarLinks; }
				const DatesSet& getDatesToForce() const { return _datesToForce; }
				const DatesSet& getDatesToBypass() const { return _datesToBypass; }
				const BitSets& getMarkedDates() const { return _markedDates; }
			//@}

			//! @name Setters
			//@{
				void setCalendarLinks(const CalendarLinks& value);
				virtual void setDatesToForce(const DatesSet& value);
				virtual void setDatesToBypass(const DatesSet& value);
			//@}

			//! @name Services
			//@{
				bool empty() const;
				bool isLinked() const;

				boost::gregorian::date getFirstActiveDate() const;
				boost::gregorian::date getLastActiveDate() const;

				/** Tests if a date is active according to the calendar.
				 * This method can be overloaded by subclasses to do additional controls.
				 * @param date date to test
				 * @return true if the calendar is active at the specified date
				 */
				virtual bool isActive(
					const boost::gregorian::date& date
				) const;



				/** Gets all the active dates of the calendar.
				 *
				 * @return vector containing the active dates of the calendar
				 */
				DatesVector getActiveDates () const;

				/** Or comparison operator : tests if at least one date is marked in the two calendars.
					@param op calendar to compare with
					@return bool true if at least one date is marked in the two calendars
					@author Hugues Romain
					@date 2008
				*/
				bool hasAtLeastOneCommonDateWith(const Calendar& op) const;


				//////////////////////////////////////////////////////////////////////////
				/// Tests if the current calendar is active on each day of activity of an
				/// other one.
				/// @param other the other calendar
				/// @return true if the current calendar includes the other one
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.2.0
				bool includesDates(const Calendar& other) const;

				size_t size() const;

				Calendar& operator<<= (size_t i);
				Calendar operator<< (size_t i) const;
				Calendar& operator&= (const Calendar& op);
				Calendar& operator-= (const Calendar& op);
				virtual Calendar& operator|= (const Calendar& op);
				Calendar operator& (const Calendar& op2) const;
				Calendar operator| (const Calendar& op2) const;

				bool operator==(const Calendar& op) const;
				bool operator!=(const Calendar& op) const;

				Calendar& operator=(Calendar const& rhs);

				//////////////////////////////////////////////////////////////////////////
				/// Serialization for storage in SQL compatible string.
				/// @param stream stream to write the result on.
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				///
				/// A serialized calendar contains a 370 bytes block per year.
				/// Each block begins with the year in decimal (4 bytes), and is followed
				/// by 366 bytes corresponding to the streamed view of the bitset.
				///
				void serialize(std::ostream& stream) const;
			//@}



			//! @name Modifiers
			//@{
				void removeCalendarLink(const CalendarLink& link, bool updateCalendar);
				void addCalendarLink(const CalendarLink& link, bool updateCalendar);

				virtual void setActive(const boost::gregorian::date& date);
				virtual void setInactive(const boost::gregorian::date& date);
				void copyDates(const Calendar& calendar);
				void clear();

				//////////////////////////////////////////////////////////////////////////
				/// Fullfill the calendar upon a serialized string.
				/// @param serialized the serialized string
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				///
				/// The serialized string comes from Calendar::serialize.
				void setFromSerializedString(const std::string& value);
			//@}

			friend class CalendarLink;
		};
}	}

#endif
