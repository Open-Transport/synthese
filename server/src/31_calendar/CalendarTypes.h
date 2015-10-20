
/** Calendar module related types definitions file.
	@file CalendarTypes.h

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

#ifndef SYNTHESE_calendar_Types_H__
#define SYNTHESE_calendar_Types_H__

#include <string>
#include <vector>
#include <boost/optional.hpp>

namespace synthese
{
	namespace calendar
	{
		/** @addtogroup m31
			@{
		*/

		//////////////////////////////////////////////////////////////////////////
		/// Category of calendar template.
		/// Pour choisir le calendrier le plus simple d'affichage, pour l'édition des renvois d'indicateur par exemple, les calendriers sont rangés par categorie, selon le format binaire suivant&nbsp;:</p>
		///
		///	<table class="tableau">
		///	<tr><td colspan="2">Plage de dates</td><td></td><td></td><td>Influence periode scolaire</td><td></td><td></td><td>Tout/partiel</td></tr>
		///	<tr><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
		///	<tr><td>7</td><td colspan="6"></td><td>0</td></tr>
		///	</table>
		///
		///	On obtient la classification de catégories suivante :
		/// <table class="tableau">
		///	<tr><th>Plage de dates</th><th>Influence periode scolaire</th><th>Tout/partiel</th><th>Code binaire</th><th>Code décimal</th></tr>
		///	<tr><td rowspan="4">Service complet</td><td rowspan="2">Non</td><td>Totalite de la periode</td><td>00000000</td><td>0</td></tr>
		///	<tr><td>Restriction</td><td>00000001</td><td>1</td></tr>
		///	<tr><td rowspan="2">Oui</td><td>Totalite de la periode</td><td>00001000</td><td>0</td></tr>
		///	<tr><td>Restriction</td><td>00001001</td><td>1</td></tr>
		///	<tr><td rowspan="4">Service de transporteur (ete, hiver, SNCF, etc.)</td><td rowspan="2">Non</td><td>Totalité de la période</td><td>01000000</td><td>0</td></tr>
		///	<tr><td>Restriction</td><td>01000001</td><td>1</td></tr>
		///	<tr><td rowspan="2">Oui</td><td>Totalite de la periode</td><td>01001000</td><td>0</td></tr>
		///	<tr><td>Restriction</td><td>01001001</td><td>1</td></tr>
		///	<tr><td rowspan="4">Plage restreinte (ski...)</td><td rowspan="2">Non</td><td>Totalite de la periode</td><td>10000000</td><td>0</td></tr>
		///	<tr><td>Restriction</td><td>10000001</td><td>1</td></tr>
		///	<tr><td rowspan="2">Oui</td><td>Totalite de la periode</td><td>10001000</td><td>0</td></tr>
		///	<tr><td>Restriction</td><td>10001001</td><td>1</td></tr>
		///	<tr><td colspan="3">Autres calendriers (defaut)</td><td>11111111</td><td>255</td></tr>
		///	</table>
		enum CalendarTemplateCategory
		{
			ALL_DAYS = 0,
			ALL_DAYS_RESTRICTION = 1,
			ALL_DAYS_SCHOOL = 8,
			ALL_DAYS_SCHOOL_RESTRICTION = 9,
			TIMESTAMP = 64,
			TIMESTAMP_RESTRICTION = 65,
			TIMESTAMP_SCHOOL = 72,
			TIMESTAMP_SCHOOL_RESTRICTION = 73,
			RESTRICTED = 128,
			RESTRICTED_RESTRICTION = 129,
			RESTRICTED_SCHOOL = 136,
			RESTRICTED_SCHOOL_RESTRICTION = 137,
			OTHER_CALENDAR = 255
		};

		typedef std::vector<std::pair<boost::optional<CalendarTemplateCategory>, std::string> > CalendarTemplateCategoryList;

		enum CalendarTemplateElementOperation
		{
			ADD = '+',
			SUB = '-',
			AND = '*'
		};


		/** @} */
	}
}

#endif // SYNTHESE_calendar_Types_H
