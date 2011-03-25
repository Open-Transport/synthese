////////////////////////////////////////////////////////////////////////////////
///	Calendar class header.
///	@file Calendar.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <bitset>
#include <map>
#include <vector>
#include <boost/optional/optional.hpp>

namespace synthese
{
	namespace calendar
	{
		/** Calendar described by an array of booleans (one per day).
			@ingroup m19

			The Calendar class implements the service calendar, holding a 
			bitset representing year days. Each year day can be activated or not.

			The active dates of the calendar are marked as activated bits on a bitset per year.
			Each year containing at least one activated date has a full bitset.
			In a year bitset, a date is represented by the bit at the rank corresponding to its day number in the year.
			Example : 2008-02-03 => bit 34 of the 2008 bitset.
		*/
		class Calendar 
		{
		public:
			typedef std::vector<boost::gregorian::date> DatesVector;

		private:

			typedef std::map<
				boost::gregorian::greg_year,
				std::bitset<366>
			> _BitSets;
			
			_BitSets _markedDates;

			mutable boost::optional<boost::gregorian::date> _firstActiveDate;
			mutable boost::optional<boost::gregorian::date> _lastActiveDate;

			static size_t _BitPos(const boost::gregorian::date& d);

		public:
			//! @name Constructors
			//@{
				Calendar();
				Calendar(
					const boost::gregorian::date& firstDate,
					const boost::gregorian::date& lastDate
				);
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

			//! @name Services
			//@{
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



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the calendar has no activated date.
				/// @return true if the calendar has no activated date.
				bool empty() const;

				size_t size() const;

				Calendar& operator<<= (std::size_t i);
				Calendar operator<< (std::size_t i);
				Calendar& operator&= (const Calendar& op);
				Calendar& operator|= (const Calendar& op);
				Calendar operator& (const Calendar& op2) const;
				Calendar operator| (const Calendar& op2) const;

				bool operator==(const Calendar& op) const;
				bool operator!=(const Calendar& op) const;


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
				virtual void setActive(const boost::gregorian::date& date);
				virtual void setInactive(const boost::gregorian::date& date);
				void subDates(const Calendar& calendar);
				void clear();
				void copyDates(const Calendar& calendar);

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
		};


	}
}

#endif
