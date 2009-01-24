
/** CalendarTemplate class header.
	@file CalendarTemplate.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_timetables_CalendarTemplate_h__
#define SYNTHESE_timetables_CalendarTemplate_h__

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "35_timetables/CalendarTemplateElement.h"

#include "15_env/Calendar.h"

namespace synthese
{
	namespace timetables
	{
		/** CalendarTemplate class.
			@ingroup m55
			
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
		class CalendarTemplate : public util::Registrable<uid,CalendarTemplate>
		{
			std::vector<CalendarTemplateElement>	_elements;
			std::string								_text;

		public:
			CalendarTemplate();

			//! @name Queries
			//@{
				env::Calendar	getCalendar(const env::Calendar& mask)	const;
			//@}

			//! @name Getters
			//@{
				std::string	getText()	const;
			//@}

			//! @name Setters
			//@{ 
				void	setText(const std::string& text);
			//@}

			//! @name Modifiers
			//@{
				void	addElement(const CalendarTemplateElement& element);
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_CalendarTemplate_h__
