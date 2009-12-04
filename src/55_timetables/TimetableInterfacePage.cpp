
/** TimetableInterfacePage class implementation.
	@file TimetableInterfacePage.cpp
	@author Hugues
	@date 2009

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

#include "TimetableInterfacePage.h"
#include "Timetable.h"
#include "TimetableGenerator.h"
#include "TimetableNoteInterfacePage.h"
#include "TimetableServiceRowInterfacePage.h"
#include "Interface.h"
#include "InterfacePageException.h"
#include "Schedule.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace timetables;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, timetables::TimetableInterfacePage>::FACTORY_KEY("timetable");
	}

	namespace timetables
	{
		TimetableInterfacePage::TimetableInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, TimetableInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void TimetableInterfacePage::display(
			std::ostream& stream,
			const Timetable& object,
			const timetables::TimetableGenerator& generator,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			// 0 : Content
			stringstream content;
			try
			{
				switch(object.getFormat())
				{
				case Timetable::TABLE_SERVICES_IN_COLS:

					break;

				case Timetable::TABLE_SERVICES_IN_ROWS:
					{
						const TimetableServiceRowInterfacePage* page(getInterface()->getPage<TimetableServiceRowInterfacePage>());
						Schedule lastSchedule;
						size_t rank(0);
						size_t followingWithSameHour;
						for(TimetableGenerator::Columns::const_iterator it(generator.getColumns().begin()); it != generator.getColumns().end(); ++it)
						{
							const TimetableGenerator::Columns::value_type& column(*it);

							followingWithSameHour = 1;
							for(TimetableGenerator::Columns::const_iterator it2(it+1); 
								it2 != generator.getColumns().end() && it2->getContent().begin()->second.getHour().getHours() == it->getContent().begin()->second.getHour().getHours();
								++it2
							){
								++followingWithSameHour;
							}

							page->display(content, column, lastSchedule, rank++, followingWithSameHour, variables, request);
							lastSchedule = column.getContent().begin()->second;
						}
					}
					break;

				case Timetable::TIMES_IN_COLS:

					break;

				case Timetable::TIMES_IN_ROWS:

					break;
				}
			}
			catch(InterfacePageException)
			{
			}
			pv.push_back(content.str());

			pv.push_back(lexical_cast<string>(object.getMustBeginAPage())); //1
			pv.push_back(object.getTitle()); //2

			// 3 : Notes
			stringstream notes;
			try
			{
				const TimetableNoteInterfacePage* notePage(getInterface()->getPage<TimetableNoteInterfacePage>());
				BOOST_FOREACH(const TimetableGenerator::Warnings::value_type& warning, generator.getWarnings())
				{
					notePage->display(notes, warning.second, variables, request);
				}
			}
			catch(InterfacePageException)
			{
			}
			pv.push_back(notes.str());
			
			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&object)
				, request
			);
		}
	}
}
