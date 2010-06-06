
/** TimetableGenerateFunction class implementation.
	@file TimetableGenerateFunction.cpp
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

#include "RequestException.h"
#include "Request.h"
#include "TimetableGenerateFunction.h"
#include "TimetableTableSync.h"
#include "LineTableSync.h"
#include "Timetable.h"
#include "TimetableRow.h"
#include "TimetableInterfacePage.h"
#include "Env.h"
#include "City.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "Line.h"
#include "CalendarTemplate.h"
#include "Calendar.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "TimetableResult.hpp"
#include "PhysicalStopTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "WebPage.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace geography;
	using namespace calendar;
	using namespace graph;
	using namespace pt;
	using namespace transportwebsite;

	template<> const string util::FactorableTemplate<Function,timetables::TimetableGenerateFunction>::FACTORY_KEY("TimetableGenerateFunction");
	
	namespace timetables
	{
		const string TimetableGenerateFunction::PARAMETER_CALENDAR_ID("cid");
		const string TimetableGenerateFunction::PARAMETER_STOP_PREFIX("stop");
		const string TimetableGenerateFunction::PARAMETER_CITY_PREFIX("city");
		const string TimetableGenerateFunction::PARAMETER_DAY("day");

		const string TimetableGenerateFunction::PARAMETER_PAGE_ID("page_id");
		const string TimetableGenerateFunction::PARAMETER_NOTE_PAGE_ID("note_page_id");
		const string TimetableGenerateFunction::PARAMETER_NOTE_CALENDAR_PAGE_ID("note_calendar_page_id");
		const string TimetableGenerateFunction::PARAMETER_ROW_PAGE_ID("row_page_id");
		const string TimetableGenerateFunction::PARAMETER_CELL_PAGE_ID("cell_page_id");
		const string TimetableGenerateFunction::PARAMETER_PAGE_FOR_SUB_TIMETABLE_ID("page_for_sub_timetable_id");


		TimetableGenerateFunction::TimetableGenerateFunction():
			FactorableTemplate<Function,TimetableGenerateFunction>()
		{
			setEnv(shared_ptr<Env>(new Env));
		}



		ParametersMap TimetableGenerateFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_timetable.get())
			{
				if(_line.get())
				{
					map.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
					if(_timetable->getBaseCalendar())
					{
						if(_timetable->getBaseCalendar()->getKey())
						{
							map.insert(PARAMETER_CALENDAR_ID, _timetable->getBaseCalendar()->getKey());
						}
						else if(_timetable->getBaseCalendar()->isLimited())
						{
							map.insert(PARAMETER_DAY, _timetable->getBaseCalendar()->getResult().getFirstActiveDate());
						}
					}
				}
				else
				{
					if(_timetable->getKey() > 0)
					{
						map.insert(Request::PARAMETER_OBJECT_ID, _timetable->getKey());
					}
					else
					{
						if(_timetable->getBaseCalendar())
						{
							map.insert(PARAMETER_CALENDAR_ID, _timetable->getBaseCalendar()->getKey());
						}
						size_t rank(0);
						BOOST_FOREACH(const TimetableRow& row, _timetable->getRows())
						{
							map.insert(
								PARAMETER_CITY_PREFIX + lexical_cast<string>(rank++),
								static_cast<const PublicTransportStopZoneConnectionPlace*>(row.getPlace())->getCity()->getName()
							);
							map.insert(
								PARAMETER_STOP_PREFIX + lexical_cast<string>(rank++),
								static_cast<const PublicTransportStopZoneConnectionPlace*>(row.getPlace())->getName()
							);
						}
					}
				}
			}
			return map;
		}



		void TimetableGenerateFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Way 1 : pre-configured timetable
			if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == TimetableTableSync::TABLE.ID)
			{
				try
				{
					_timetable = TimetableTableSync::Get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), *_env);
				}
				catch(ObjectNotFoundException<Timetable>)
				{
					throw RequestException("No such timetable");
				}
			}
			else
			{
				shared_ptr<Timetable> timetable(new Timetable);

				if(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_ID))
				{
					try
					{
						timetable->setBaseCalendar(
							Env::GetOfficialEnv().get<CalendarTemplate>(map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID)).get()
						);
					}
					catch(ObjectNotFoundException<CalendarTemplate>&)
					{
						throw RequestException("No such calendar");
					}
				}
				else
				{
					date curDate(day_clock::local_day());
					if(map.getOptional<string>(PARAMETER_DAY))
					{
						curDate = from_simple_string(map.get<string>(PARAMETER_DAY));
					}

					CalendarTemplate* calendarTemplate(new CalendarTemplate);
					CalendarTemplateElement element;
					element.setCalendar(calendarTemplate);
					element.setInterval(days(1));
					element.setMinDate(curDate);
					element.setMaxDate(curDate);
					element.setOperation(CalendarTemplateElement::ADD);
					element.setRank(0);
					calendarTemplate->addElement(element);
					calendarTemplate->setText(
						lexical_cast<string>(curDate.day()) + "/" + lexical_cast<string>(lexical_cast<int>(curDate.month())) + "/" + lexical_cast<string>(curDate.year())
					);
					timetable->setBaseCalendar(calendarTemplate);
					_calendarTemplate.reset(calendarTemplate);
				}
				

				// Way 2 : line time table
				if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == LineTableSync::TABLE.ID)
				{
					try
					{
						_line = Env::GetOfficialEnv().get<Line>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
					}
					catch(ObjectNotFoundException<Line>&)
					{
						throw RequestException("No such line route");
					}
					timetable->addAuthorizedLine(_line->getCommercialLine());
					timetable->setContentType(Timetable::TABLE_SERVICES_IN_COLS);
					
					size_t rank(0);
					BOOST_FOREACH(const Edge* edge, _line->getEdges())
					{
						TimetableRow row;
						row.setIsArrival(edge->isArrival());
						row.setIsDeparture(edge->isDeparture());
						row.setPlace(static_cast<const LineStop*>(edge)->getPhysicalStop()->getConnectionPlace());
						row.setRank(rank++);
						timetable->addRow(row);
					}
				} // Way 4.1 : stop area timetable
				if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == ConnectionPlaceTableSync::TABLE.ID)
				{
					shared_ptr<const PublicTransportStopZoneConnectionPlace> place;
					try
					{
						place = Env::GetOfficialEnv().get<PublicTransportStopZoneConnectionPlace>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
					}
					catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>&)
					{
						throw RequestException("No such place");
					}
					timetable->setContentType(Timetable::TABLE_SERVICES_IN_ROWS);
					TimetableRow row;
					row.setIsArrival(false);
					row.setIsDeparture(true);
					row.setPlace(place.get());
					row.setRank(0);
					timetable->addRow(row);
				} // Way 4.2 : physical stop timetable
				else if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == PhysicalStopTableSync::TABLE.ID)
				{
					shared_ptr<const PhysicalStop> stop;
					try
					{
						stop = Env::GetOfficialEnv().get<PhysicalStop>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
					}
					catch(ObjectNotFoundException<PhysicalStop>&)
					{
						throw RequestException("No such stop");
					}
					timetable->setContentType(Timetable::TABLE_SERVICES_IN_ROWS);
					TimetableRow row;
					row.setIsArrival(false);
					row.setIsDeparture(true);
					row.setPlace(stop->getConnectionPlace());
					row.setRank(0);
					timetable->addRow(row);
					timetable->addAuthorizedPhysicalStop(stop.get());
				}
				else // Way 3 : customized timetable
				{
/*					timetable->setContentType(Timetable::TABLE_SERVICES_IN_COLS);
					for(size_t rank(0);
						!map.getDefault<string>(PARAMETER_CITY_PREFIX + lexical_cast<string>(rank)).empty() &&
						!map.getDefault<string>(PARAMETER_STOP_PREFIX + lexical_cast<string>(rank)).empty();
						++rank
					){
						Site::ExtendedFetchPlaceResult placeResult(
							_site->extendedFetchPlace(
								map.get<string>(PARAMETER_CITY_PREFIX + lexical_cast<string>(rank)),
								map.get<string>(PARAMETER_STOP_PREFIX + lexical_cast<string>(rank))
						)	);
						TimetableRow row;
						if(!dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(placeResult.placeResult.value))
						{
							throw RequestException("No such place at rank "+ lexical_cast<string>(rank));
						}
						row.setPlace(dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(placeResult.placeResult.value));
						row.setRank(rank);
						timetable->addRow(row);
					}
*/				}

				_timetable = const_pointer_cast<const Timetable>(timetable);
			}
			
			// Display templates

			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
				if(id)
				{
					_page = Env::GetOfficialEnv().get<WebPage>(*id);
				}
			}
			catch (ObjectNotFoundException<WebPage>& e)
			{
				throw RequestException("No such page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_NOTE_PAGE_ID));
				if(id) 
				{
					_notePage = Env::GetOfficialEnv().get<WebPage>(*id);
				}
			}
			catch (ObjectNotFoundException<WebPage>& e)
			{
				throw RequestException("No such note row page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_NOTE_CALENDAR_PAGE_ID));
				if(id)
				{
					_noteCalendarPage = Env::GetOfficialEnv().get<WebPage>(*id);
				}
			}
			catch (ObjectNotFoundException<WebPage>& e)
			{
				throw RequestException("No such note calendar page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_ROW_PAGE_ID));
				if(id)
				{
					_rowPage = Env::GetOfficialEnv().get<WebPage>(*id);
				}
			}
			catch (ObjectNotFoundException<WebPage>& e)
			{
				throw RequestException("No such row page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_CELL_PAGE_ID));
				if(id) 
				{
					_cellPage = Env::GetOfficialEnv().get<WebPage>(*id);
				}
			}
			catch (ObjectNotFoundException<WebPage>& e)
			{
				throw RequestException("No such cell page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_FOR_SUB_TIMETABLE_ID));
				if(id) 
				{
					_pageForSubTimetable = Env::GetOfficialEnv().get<WebPage>(*id);
				}
			}
			catch (ObjectNotFoundException<WebPage>& e)
			{
				throw RequestException("No such page for sub timetable : "+ e.getMessage());
			}
		}

		void TimetableGenerateFunction::run( std::ostream& stream, const Request& request ) const
		{
			auto_ptr<TimetableGenerator> generator(_timetable->getGenerator(Env::GetOfficialEnv()));
			TimetableResult result(generator->build());
			TimetableInterfacePage::Display(
				stream,
				_page,
				_notePage,
				_noteCalendarPage,
				_pageForSubTimetable,
				_rowPage,
				_cellPage,
				request,
				*_timetable,
				*generator,
				result
			);
		}
		
		
		
		bool TimetableGenerateFunction::isAuthorized(const server::Session* session) const
		{
			return true;
		}



		std::string TimetableGenerateFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/plain";
		}
	}
}
