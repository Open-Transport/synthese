
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

#include "Object.hpp"
#include "Calendar.h"
#include "CalendarTemplateElement.h"
#include "Exception.h"
#include "ImportableTemplate.hpp"
#include "TreeAlphabeticalOrderingPolicy.hpp"
#include "TreeNode.hpp"
#include "TreeUniqueRootPolicy.hpp"
#include "CalendarTypes.h"
#include "EnumObjectField.hpp"
#include "DataSourceLinksField.hpp"

#include <map>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace calendar
	{

		class CalendarTemplate;

		FIELD_ENUM(Category, CalendarTemplateCategory)
		FIELD_POINTER(ParentCalendarTemplate, CalendarTemplate)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Category),
			FIELD(impex::DataSourceLinks),
			FIELD(ParentCalendarTemplate)
		> CalendarTemplateSchema;


		/** CalendarTemplate class.
			@ingroup m31

		*/
		class CalendarTemplate:
			public Object<CalendarTemplate, CalendarTemplateSchema>,
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

			static std::string GetCategoryName(CalendarTemplateCategory value);
			static CalendarTemplateCategoryList GetCategoriesList();

			typedef std::map<std::size_t, CalendarTemplateElement> Elements;

		private:

			mutable Elements	_elements;

		public:
			CalendarTemplate(
				util::RegistryKeyType id = 0
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
				CalendarTemplateCategory getCategory() const;
				const Elements& getElements() const { return _elements; }
				virtual std::string getName() const;
			//@}

			//! @name Setters
			//@{
				void	setCategory(CalendarTemplateCategory value);
				void	setName(const std::string& value);
			//@}

			//! @name Modifiers
			//@{
				void	clearElements();
				void	addElement(const CalendarTemplateElement& element);
				void	removeElement(const CalendarTemplateElement& element);

				virtual bool allowUpdate(const server::Session* session) const;
				virtual bool allowCreate(const server::Session* session) const;
				virtual bool allowDelete(const server::Session* session) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_timetables_CalendarTemplate_h__
