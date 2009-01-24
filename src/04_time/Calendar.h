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

#include "Date.h"

#include <boost/dynamic_bitset.hpp>
#include <vector>


namespace synthese
{
	namespace time
	{
		/** Calendar described by an array of booleans (one per day).
			@ingroup m04

			The Calendar class implements the service calendar, holding a 
			bitset representing year days. Each year day can be activated or not.

			The first bit of the internal bitset corresponds to the first
			marked date. The last bit corresponds to the last date marked.

		*/
		class Calendar 
		{
		public:


		private:

			Date _firstMarkedDate;
			Date _lastMarkedDate;

			boost::dynamic_bitset<> _markedDates;

		public:

			Calendar();
		 
			~Calendar();


			//! @name Getters/Setters
			//@{
				Date getFirstActiveDate () const;
				Date getLastActiveDate () const;
			//@}


			//! @name Query methods
			//@{
				int getNbActiveDates () const;
				
				
				
				/** Tests if a date is active according to the calendar.
				 * This method can be overloaded by subclasses to do additional controls.
				 * @param date date to test
				 * @return true if the calendar is active at the specified date
				 */
				virtual bool isActive(
					const Date& date
				) const;
				
				
				
				/** Gets all the active dates of the calendar.
				 * 
				 * @return vector containing the active dates of the calendar
				 */
				std::vector<Date> getActiveDates () const;
			//@}



			//! @name Update methods
			//@{
				virtual void setActive(const Date& date);
				virtual void setInactive(const Date& date);
				void subDates(const Calendar& calendar);
				void clearDates();
			//@}


			
			
			/** Or comparison operator : tests if at least one date is marked in the two calendars.
				@param op calendar to compare with
				@return bool true if at least one date is marked in the two calendars
				@author Hugues Romain
				@date 2008				
			*/
			bool operator || (const Calendar& op) const;

			Calendar& operator&= (const Calendar& op);
			Calendar& operator|= (const Calendar& op);

			bool operator==(const Calendar& op) const;

			static void LogicalAnd(
				Calendar& dest,
				const Calendar& op1,
				const Calendar& op2
			);
			static void LogicalOr(
				Calendar& dest,
				const Calendar& op1,
				const Calendar& op2
			);

		private:

			void pop_front (int nbBits);
			void pop_back (int nbBits);

			void push_front (int nbBits, bool value = false);
			void push_back (int nbBits, bool value = false);

			void updateFirstMark ();
			void updateLastMark ();

			static Date DateAfter(
				Date date,
				unsigned int nbBits
			);
			static Date DateBefore(
				Date date,
				unsigned int nbBits
			);

			static int NbBitsBetweenDates(
				Date date1,
				Date date2
			);
		};


		Calendar operator& (const Calendar& op1, const Calendar& op2);
		Calendar operator| (const Calendar& op1, const Calendar& op2);
	}
}

#endif
