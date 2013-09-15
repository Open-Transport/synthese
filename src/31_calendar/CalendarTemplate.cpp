
/** CalendarTemplate class implementation.
	@file CalendarTemplate.cpp

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

#include "CalendarTemplate.h"

#include "CalendarTemplateElement.h"
#include "CalendarTemplateTableSync.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace util;

	namespace util
	{
		template<> const std::string Registry<calendar::CalendarTemplate>::KEY("CalendarTemplate");
	}

	namespace calendar
	{
		const string CalendarTemplate::ATTR_NAME = "name";
	


		CalendarTemplate::CalendarTemplate(
		):	util::Registrable(0),
			_category(OTHER_CALENDAR)
		{}



		CalendarTemplate::CalendarTemplate(
			RegistryKeyType id
		):	util::Registrable(id),
			_category(OTHER_CALENDAR)
		{}



		CalendarTemplate::CalendarTemplate( const gregorian::date& day ):
			Registrable(0)
		{
			CalendarTemplateElement element;
			element.setCalendar(this);
			element.setStep(days(1));
			element.setMinDate(day);
			element.setMaxDate(day);
			element.setOperation(CalendarTemplateElement::ADD);
			element.setRank(0);
			addElement(element);
			setName(
				lexical_cast<string>(day.day()) + "/" +
				lexical_cast<string>(static_cast<int>(day.month())) + "/" +
				lexical_cast<string>(day.year())
			);
		}



		Calendar CalendarTemplate::getResult(
			const Calendar& mask
		) const {
			Calendar result;
			BOOST_FOREACH(const Elements::value_type& element, _elements)
			{
				element.second.apply(result, mask);
			}
			return result;
		}



		Calendar CalendarTemplate::getResult() const
		{
			if(!isLimited()) throw InfiniteCalendarException();
			Calendar mask(getMinDate(), getMaxDate());
			return getResult(mask);
		}


		void CalendarTemplate::addElement( const CalendarTemplateElement& element )
		{
			removeElement(element);
			_elements.insert(make_pair(element.getRank(), element));
		}



		boost::gregorian::date CalendarTemplate::getMinDate() const
		{
			date result(pos_infin);
			BOOST_FOREACH(const Elements::value_type& element, _elements)
			{
				date elementMinDate(element.second.getRealMinDate());

				if(	(elementMinDate < result &&
					 element.second.getOperation() != CalendarTemplateElement::AND) ||
					(element.second.getOperation() == CalendarTemplateElement::AND &&
					 elementMinDate > result)
				){
					result = elementMinDate;
				}
			}
			return result;
		}



		boost::gregorian::date CalendarTemplate::getMaxDate() const
		{
			date result(neg_infin);
			BOOST_FOREACH(const Elements::value_type& element, _elements)
			{
				date elementMaxDate(element.second.getRealMaxDate());

				if(	(elementMaxDate > result &&
					 element.second.getOperation() != CalendarTemplateElement::AND) ||
					(element.second.getOperation() == CalendarTemplateElement::AND &&
					 elementMaxDate < result)
				){
					result = elementMaxDate;
				}
			}
			return result;
		}



		bool CalendarTemplate::isLimited() const
		{
			return !getMinDate().is_infinity() && !getMaxDate().is_infinity();
		}



		CalendarTemplate::Category CalendarTemplate::getCategory() const
		{
			return _category;
		}



		void CalendarTemplate::setCategory( Category value )
		{
			_category = value;
		}



		std::string CalendarTemplate::GetCategoryName( Category value )
		{
			switch(value)
			{
			case ALL_DAYS: return "Sans limite";
			case ALL_DAYS_RESTRICTION: return "Sans limite avec jours restreints";
			case ALL_DAYS_SCHOOL: return "Sans limite avec périodes scolaires";
			case ALL_DAYS_SCHOOL_RESTRICTION: return "Sans limite avec périodes scolaires avec jours restreints";
			case TIMESTAMP: return "Plage de dates";
			case TIMESTAMP_RESTRICTION: return "Plage de dates avec jours restreints";
			case TIMESTAMP_SCHOOL: return "Plage de dates avec périodes scolaires";
			case TIMESTAMP_SCHOOL_RESTRICTION: return "Plage de dates avec périodes scolaires avec jours restreints";
			case RESTRICTED: return "Période particulière";
			case RESTRICTED_RESTRICTION: return "Période particulière avec jours restreints";
			case RESTRICTED_SCHOOL: return "Période particulière avec périodes scolaires";
			case RESTRICTED_SCHOOL_RESTRICTION: return "Période particulière avec périodes scolaires avec jours restreints";
			default:
				return "Autre catégorie";
			}
		}



		CalendarTemplate::CategoryList CalendarTemplate::GetCategoriesList()
		{
			CategoryList result;
			result.push_back(make_pair(ALL_DAYS, GetCategoryName(ALL_DAYS)));
			result.push_back(make_pair(ALL_DAYS_RESTRICTION, GetCategoryName(ALL_DAYS_RESTRICTION)));
			result.push_back(make_pair(ALL_DAYS_SCHOOL, GetCategoryName(ALL_DAYS_SCHOOL)));
			result.push_back(make_pair(ALL_DAYS_SCHOOL_RESTRICTION, GetCategoryName(ALL_DAYS_SCHOOL_RESTRICTION)));
			result.push_back(make_pair(TIMESTAMP, GetCategoryName(TIMESTAMP)));
			result.push_back(make_pair(TIMESTAMP_RESTRICTION, GetCategoryName(TIMESTAMP_RESTRICTION)));
			result.push_back(make_pair(TIMESTAMP_SCHOOL, GetCategoryName(TIMESTAMP_SCHOOL)));
			result.push_back(make_pair(TIMESTAMP_SCHOOL_RESTRICTION, GetCategoryName(TIMESTAMP_SCHOOL_RESTRICTION)));
			result.push_back(make_pair(RESTRICTED, GetCategoryName(RESTRICTED)));
			result.push_back(make_pair(RESTRICTED_RESTRICTION, GetCategoryName(RESTRICTED_RESTRICTION)));
			result.push_back(make_pair(RESTRICTED_SCHOOL, GetCategoryName(RESTRICTED_SCHOOL)));
			result.push_back(make_pair(RESTRICTED_SCHOOL_RESTRICTION, GetCategoryName(RESTRICTED_SCHOOL_RESTRICTION)));
			result.push_back(make_pair(OTHER_CALENDAR, GetCategoryName(OTHER_CALENDAR)));
			return result;
		}



		void CalendarTemplate::clearElements()
		{
			_elements.clear();
		}



		void CalendarTemplate::removeElement( const CalendarTemplateElement& element )
		{
			Elements::iterator it(_elements.find(element.getRank()));
			if(it != _elements.end())
			{
				_elements.erase(it);
			}
		}



		void CalendarTemplate::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const {

			pm.insert(ATTR_ID, getKey());
			pm.insert(ATTR_NAME, getName());
			dataSourceLinksToParametersMap(pm);

			pm.insert(
				CalendarTemplateTableSync::COL_TEXT,
				getName()
			);
			pm.insert(
				CalendarTemplateTableSync::COL_CATEGORY,
				static_cast<int>(getCategory())
			);
			pm.insert(
				CalendarTemplateTableSync::COL_DATASOURCE_LINKS,
				impex::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			pm.insert(
				CalendarTemplateTableSync::COL_PARENT_ID,
				getParent(true) ? getParent()->getKey() : 0
			);
		}



		bool CalendarTemplate::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			if(record.isDefined(TABLE_COL_ID))
			{
				RegistryKeyType id(record.getDefault<RegistryKeyType>(TABLE_COL_ID, 0));
				if(id != getKey())
				{
					result = true;
					setKey(id);
				}

			}

			if(record.isDefined(CalendarTemplateTableSync::COL_TEXT))
			{
				string value(record.get<string>(CalendarTemplateTableSync::COL_TEXT));
				if(value != getName())
				{
					result = true;
					setName(value);
				}
			}

			if(record.isDefined(CalendarTemplateTableSync::COL_CATEGORY))
			{
				CalendarTemplate::Category value(
					static_cast<CalendarTemplate::Category>(
						record.getDefault<int>(CalendarTemplateTableSync::COL_CATEGORY, 0)
				)	);
				if(value != getCategory())
				{
					result = true;
					setCategory(value);
				}
			}

//			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Elements
 /*				CalendarTemplateElementTableSync::SearchResult elements(
 					CalendarTemplateElementTableSync::Search(
 						env,
 						object->getKey(),
						optional<RegistryKeyType>(),
 						0, optional<size_t>(),
 						UP_LINKS_LOAD_LEVEL
 				)	);
 				BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& e, elements)
 				{
 					object->addElement(*e);
 				}
*/
				// Data source links
				if(record.isDefined(CalendarTemplateTableSync::COL_DATASOURCE_LINKS))
				{
					Importable::DataSourceLinks value(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							record.get<string>(CalendarTemplateTableSync::COL_DATASOURCE_LINKS),
							env
					)	);
					if(value != getDataSourceLinks())
					{
						result = true;
						setDataSourceLinksWithoutRegistration(value);
					}
				}

				// Parent
				if(record.isDefined(CalendarTemplateTableSync::COL_PARENT_ID))
				{
					CalendarTemplate* value(NULL);
					RegistryKeyType id(
						record.getDefault<RegistryKeyType>(CalendarTemplateTableSync::COL_PARENT_ID, 0)
					);
						
					if(id > 0) try
					{
						value = CalendarTemplateTableSync::GetEditable(
							id, env
						).get();
					}
					catch (ObjectNotFoundException<CalendarTemplate>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + CalendarTemplateTableSync::TABLE.NAME + "/" + CalendarTemplateTableSync::COL_PARENT_ID +" (" + lexical_cast<string>(getKey()) + ")", e);
					}

					if(value != getParent(true))
					{
						result = true;
						setParent(value);
					}
				}
			}

			return result;
		}



		void CalendarTemplate::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			registerInParentOrRoot();
		}



		synthese::LinkedObjectsIds CalendarTemplate::getLinkedObjectsIds( const Record& record ) const
		{
			return LinkedObjectsIds();
		}



		SubObjects CalendarTemplate::getSubObjects() const
		{
			SubObjects result;

			// Calendars
			BOOST_FOREACH(const ChildrenType::value_type& it, getChildren())
			{
				result.push_back(it.second);
			}

			// Elements
			BOOST_FOREACH(const Elements::value_type& element, getElements())
			{
				result.push_back(const_cast<CalendarTemplateElement*>(&element.second));
			}

			return result;
		}


		CalendarTemplate::InfiniteCalendarException::InfiniteCalendarException()
			: Exception("The calendar template defines an infinite sized result.")
		{}
}	}
