#ifndef SYNTHESE_ENV_CALENDAR_H
#define SYNTHESE_ENV_CALENDAR_H



#include "04_time/Date.h"

#include <boost/dynamic_bitset.hpp>


namespace synthese
{
	namespace env
	{

		/** Calendar.
			@ingroup m15

			Les services ne circulent pas tous tous les jours. Ils suivent un calendrier de circulation, indiquant les jours où sont effectués des départs de l'origine des services qui le respectent.

			The Calendar class implements the service calendar, holding a bitset representing year days.
			Each year day can be marked or not.

			The first bit of the internal bitset corresponds to the first
			marked date. The last bit corresponds to the last date marked.


			<h3>Catégorie de calendrier</h3>

			@todo Reactivate calendar category

			Pour choisir le calendrier le plus simple d’affichage, pour l’édition des renvois d’indicateur par exemple, les calendriers sont rangés par catégorie, selon le format binaire suivant&nbsp;:</p>

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

			synthese::time::Date _firstMarkedDate;
			synthese::time::Date _lastMarkedDate;

			boost::dynamic_bitset<> _markedDates;    

		public:

			Calendar();
		    
			~Calendar();


			//! @name Getters/Setters
			//@{
			synthese::time::Date getFirstMarkedDate () const;
			synthese::time::Date getLastMarkedDate () const;
			//@}


			//! @name Query methods
			//@{
			int getNbMarkedDates () const;
			bool isMarked (synthese::time::Date date) const;
			//@}



			//! @name Update methods
			//@{
			void mark (synthese::time::Date date, bool state = true);

			//@}

			Calendar& operator&= (const Calendar& op);
			Calendar& operator|= (const Calendar& op);

			static void LogicalAnd (Calendar& dest, const Calendar& op1, const Calendar& op2);
			static void LogicalOr (Calendar& dest, const Calendar& op1, const Calendar& op2);

		private:

			void pop_front (int nbBits);
			void pop_back (int nbBits);

			void push_front (int nbBits, bool value = false);
			void push_back (int nbBits, bool value = false);

			void updateFirstMark ();
			void updateLastMark ();

			static synthese::time::Date DateAfter (synthese::time::Date date, unsigned int nbBits);
			static synthese::time::Date DateBefore (synthese::time::Date date, unsigned int nbBits);

			static int NbBitsBetweenDates (synthese::time::Date date1, synthese::time::Date date2);


		};


		Calendar operator& (const Calendar& op1, const Calendar& op2);
		Calendar operator| (const Calendar& op1, const Calendar& op2);

	}
}

#endif
