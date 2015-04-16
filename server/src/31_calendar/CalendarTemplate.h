
/** CalendarTemplate class header.
	@file CalendarTemplate.h

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

#ifndef SYNTHESE_timetables_CalendarTemplate_h__
#define SYNTHESE_timetables_CalendarTemplate_h__

#include "Calendar.h"
#include "CalendarTemplateElement.h"
#include "Exception.h"
#include "ImportableTemplate.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "TreeAlphabeticalOrderingPolicy.hpp"
#include "TreeNode.hpp"
#include "TreeUniqueRootPolicy.hpp"

#include <map>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace calendar
	{
		/** CalendarTemplate class.
			@ingroup m31

			<h3>Calendar template category</h3>

			@see CalendarTemplate::Category

		*/
		class CalendarTemplate:
			public virtual util::Registrable,
			public impex::ImportableTemplate<CalendarTemplate>,
			public tree::TreeNode<
				CalendarTemplate,
				tree::TreeAlphabeticalOrderingPolicy,
				tree::TreeUniqueRootPolicy<
					tree::TreeAlphabeticalOrderingPolicy,
					CalendarTemplate
			>	>
		{
		public:
			static const std::string ATTR_NAME;

			/// Chosen registry class.
			typedef util::Registry<CalendarTemplate>	Registry;

			class InfiniteCalendarException:
				public synthese::Exception
			{
			public:
				InfiniteCalendarException();
			};

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
			enum Category
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

			static std::string GetCategoryName(Category value);

			typedef std::vector<std::pair<boost::optional<Category>, std::string> > CategoryList;

			static CategoryList GetCategoriesList();

			typedef std::map<std::size_t, CalendarTemplateElement> Elements;

		private:
			mutable Elements	_elements;
			std::string _name;
			Category								_category;

		public:
			CalendarTemplate();

			CalendarTemplate(
				util::RegistryKeyType id
			);

			CalendarTemplate(
				const boost::gregorian::date& day
			);

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Result calendar generation according to a mask.
				/// @param mask the mask to apply on the template to compute the result
				/// @return The result calendar object
				Calendar	getResult(const Calendar& mask)	const;



				//////////////////////////////////////////////////////////////////////////
				/// Result calendar generation.
				/// This method can compute the result calendar only if the object is
				/// limited (see CalendarTemplate::isLimited())
				/// @return The result calendar object
				/// @throws InfiniteCalendarException if the object is not limited
				Calendar	getResult() const;

				boost::gregorian::date	getMinDate() const;
				boost::gregorian::date	getMaxDate() const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the calendar template defines a limited time period.
				/// @return true if the min and max dates are not infinite.
				bool isLimited() const;

				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual SubObjects getSubObjects() const;
			//@}

			//! @name Getters
			//@{
				Category getCategory() const;
				const Elements& getElements() const { return _elements; }
				virtual std::string getName() const { return _name; }
			//@}

			//! @name Setters
			//@{
				void	setCategory(Category value);
				void setName(const std::string& value){ _name = value; }
			//@}

			//! @name Modifiers
			//@{
				void	clearElements();
				void	addElement(const CalendarTemplateElement& element);
				void	removeElement(const CalendarTemplateElement& element);

				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_timetables_CalendarTemplate_h__
