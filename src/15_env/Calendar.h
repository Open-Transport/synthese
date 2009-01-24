#ifndef SYNTHESE_ENV_CALENDAR_H
#define SYNTHESE_ENV_CALENDAR_H



#include "04_time/Date.h"

#include <boost/dynamic_bitset.hpp>
#include <vector>


namespace synthese
{
	namespace env
	{

		/** Calendar.
			@ingroup m35

			Les services ne circulent pas tous tous les jours. Ils suivent un calendrier de circulation, indiquant les jours o�¹ sont effectués des départs de l'origine des services qui le respectent.

			The Calendar class implements the service calendar, holding a bitset representing year days.
			Each year day can be marked or not.

			The first bit of the internal bitset corresponds to the first
			marked date. The last bit corresponds to the last date marked.


			<h3>Catégorie de calendrier</h3>

			@todo Reactivate calendar category

			Pour choisir le calendrier le plus simple d'affichage, pour l'édition des renvois d'indicateur par exemple, les calendriers sont rangés par catégorie, selon le format binaire suivant&nbsp;:</p>

			<table class="tableau">
			<tr><td colspan="2">Plage de dates</td><td></td><td></td><td>Influence période scolaire</td><td></td><td></td><td>Tout/partiel</td></tr>
			<tr><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>7</td><td colspan="6"></td><td>0</td></tr>
			</table>

			On obtient la classification de catégories suivante :
			<table class="tableau">
			<tr><th>Plage de dates</th><th>Influence période scolaire</th><th>Tout/partiel</th><th>Code binaire</th><th>Code décimal</th></tr>
			<tr><td rowspan="4">Service complet</td><td rowspan="2">Non</td><td>Totalité de la période</td><td>00000000</td><td>0</td></tr>
			<tr><td>Restriction</td><td>00000001</td><td>1</td></tr>
			<tr><td rowspan="2">Oui</td><td>Totalité de la période</td><td>00001000</td><td>0</td></tr>
			<tr><td>Restriction</td><td>00001001</td><td>1</td></tr>
			<tr><td rowspan="4">Service de transporteur (été, hiver, SNCF, etc.)</td><td rowspan="2">Non</td><td>Totalité de la période</td><td>01000000</td><td>0</td></tr>
			<tr><td>Restriction</td><td>01000001</td><td>1</td></tr>
			<tr><td rowspan="2">Oui</td><td>Totalité de la période</td><td>01001000</td><td>0</td></tr>
			<tr><td>Restriction</td><td>01001001</td><td>1</td></tr>
			<tr><td rowspan="4">Plage restreinte (ski...)</td><td rowspan="2">Non</td><td>Totalité de la période</td><td>10000000</td><td>0</td></tr>
			<tr><td>Restriction</td><td>10000001</td><td>1</td></tr>
			<tr><td rowspan="2">Oui</td><td>Totalité de la période</td><td>10001000</td><td>0</td></tr>
			<tr><td>Restriction</td><td>10001001</td><td>1</td></tr>
			<tr><td colspan="3">Autres calendriers (défaut)</td><td>11111111</td><td>255</td></tr>
			</table>

		*/
		class Calendar 
		{
		public:


		private:

			time::Date _firstMarkedDate;
			time::Date _lastMarkedDate;

			boost::dynamic_bitset<> _markedDates;

		public:

			Calendar();
		 
			~Calendar();


			//! @name Getters/Setters
			//@{
			time::Date getFirstActiveDate () const;
			time::Date getLastActiveDate () const;
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
					const time::Date& date
				) const;
				
				
				
				/** Gets all the active dates of the calendar.
				 * 
				 * @return vector containing the active dates of the calendar
				 */
				std::vector<time::Date> getActiveDates () const;
			//@}



			//! @name Update methods
			//@{
				virtual void setActive(const time::Date& date);
				virtual void setInactive(const time::Date& date);
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

			static void LogicalAnd (Calendar& dest, const Calendar& op1, const Calendar& op2);
			static void LogicalOr (Calendar& dest, const Calendar& op1, const Calendar& op2);

		private:

			void pop_front (int nbBits);
			void pop_back (int nbBits);

			void push_front (int nbBits, bool value = false);
			void push_back (int nbBits, bool value = false);

			void updateFirstMark ();
			void updateLastMark ();

			static time::Date DateAfter (time::Date date, unsigned int nbBits);
			static time::Date DateBefore (time::Date date, unsigned int nbBits);

			static int NbBitsBetweenDates (time::Date date1, time::Date date2);
		};


		Calendar operator& (const Calendar& op1, const Calendar& op2);
		Calendar operator| (const Calendar& op1, const Calendar& op2);

	}
}

#endif

