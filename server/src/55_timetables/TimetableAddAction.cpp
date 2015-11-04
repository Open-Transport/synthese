
/** TimetableAddAction class implementation.
	@file TimetableAddAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "TimetableAddAction.h"

#include "ActionException.h"
#include "CalendarTemplate.h"
#include "DBTransaction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "StopArea.hpp"
#include "Timetable.h"
#include "TimetableRight.h"
#include "TimetableRow.h"
#include "TimetableRowTableSync.h"
#include "TimetableTableSync.h"
#include "User.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::TimetableAddAction>::FACTORY_KEY("TimetableAddAction");
	}

	namespace timetables
	{
		const string TimetableAddAction::PARAMETER_BOOK_ID = Action_PARAMETER_PREFIX + "bi";
		const string TimetableAddAction::PARAMETER_RANK = Action_PARAMETER_PREFIX + "rk";
		const string TimetableAddAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "ti";
		const string TimetableAddAction::PARAMETER_IS_BOOK = Action_PARAMETER_PREFIX + "ib";
		const string TimetableAddAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "te";
		const string TimetableAddAction::PARAMETER_REVERSE = Action_PARAMETER_PREFIX + "re";


		TimetableAddAction::TimetableAddAction():
			util::FactorableTemplate<Action, TimetableAddAction>(),
			_isBook(false),
			_reverse(false)
		{
		}



		ParametersMap TimetableAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_template.get())
			{
				map.insert(PARAMETER_TEMPLATE_ID, _template->getKey());
				map.insert(PARAMETER_REVERSE, _reverse);
			}
			else
			{
				map.insert(PARAMETER_BOOK_ID, _book.get() ? _book->getKey() : RegistryKeyType(0));
				map.insert(PARAMETER_TITLE, _title);
				map.insert(PARAMETER_RANK, _rank);
				map.insert(PARAMETER_IS_BOOK, _isBook);
			}
			return map;
		}



		void TimetableAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getOptional<RegistryKeyType>(PARAMETER_TEMPLATE_ID))
			{
				try
				{
					_template = TimetableTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TEMPLATE_ID), *_env);
				}
				catch (...)
				{
					throw ActionException("No such template");
				}
				_reverse = map.getDefault<bool>(PARAMETER_REVERSE, false);
				_rank = _template->get<Rank>() + 1;
				_title = _template->get<Title>() + " (" + (_reverse ? "reversed " : "") + "copy)";
				_isBook = (_template->getContentType() == Timetable::CONTAINER);
				if(_template->get<Book>().get_ptr() != NULL)
				{
					try
					{
						_book = TimetableTableSync::GetEditable(_template->get<Book>()->getKey(), *_env);
					}
					catch (...)
					{
						throw ActionException("No such book");
					}
				}
			}
			else
			{
				if(map.getDefault<RegistryKeyType>(PARAMETER_BOOK_ID, RegistryKeyType(0)))
				{
					try
					{
						_book = TimetableTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_BOOK_ID), *_env);
					}
					catch (...)
					{
						throw ActionException("No such book");
					}
				}

				// Rank
				optional<size_t> existingRank(
					TimetableTableSync::GetMaxRank(_book.get() ? _book->getKey() : 0)
				);
				_rank = map.getDefault<size_t>(
					PARAMETER_RANK,
					existingRank ? *existingRank + 1 : 0
				);

				// Title
				_title = map.getDefault<string>(PARAMETER_TITLE);
				_isBook = map.get<bool>(PARAMETER_IS_BOOK);
			}
		}



		void TimetableAddAction::run(Request& request)
		{
			DBTransaction transaction;

			// timetable creation
			Timetable t;
			t.setContentType(_isBook ? Timetable::CONTAINER : Timetable::TABLE_SERVICES_IN_COLS);
			if(_book.get())
			{
				t.set<Book>(const_cast<Timetable&>(*_book.get()));
			}
			t.set<Rank>(_rank);
			t.set<Title>(_title);
			if(_template.get())
			{
				t.set<BaseCalendar>(_template->get<BaseCalendar>());
			}

			// rank shifting
			TimetableTableSync::Shift(_book.get() ? _book->getKey() : 0, _rank, 1);

			// Saving
			TimetableTableSync::Save(&t);

			// ID update
			request.setActionCreatedId(t.getKey());

			// Copy of a template
			if(_template.get())
			{
				size_t rank(0);
				if(_reverse)
				{
					BOOST_REVERSE_FOREACH(const Timetable::Rows::value_type& row, _template->getRows())
					{
						TimetableRow r;
						r.setTimetableId(t.getKey());
						r.setRank(rank++);
						r.setPlace(row.getPlace());
						r.setIsDeparture(row.getIsArrival());
						r.setIsArrival(row.getIsDeparture());
						r.setCompulsory(row.getCompulsory());
						TimetableRowTableSync::Save(&r, transaction);
					}
				}
				else
				{
					BOOST_FOREACH(const Timetable::Rows::value_type& row, _template->getRows())
					{
						TimetableRow r;
						r.setTimetableId(t.getKey());
						r.setRank(rank++);
						r.setPlace(row.getPlace());
						r.setIsDeparture(row.getIsDeparture());
						r.setIsArrival(row.getIsArrival());
						r.setCompulsory(row.getCompulsory());
						TimetableRowTableSync::Save(&r, transaction);
					}
				}
			}

			// Database saving
			transaction.run();
		}



		void TimetableAddAction::setBook( boost::shared_ptr<const Timetable> book )
		{
			assert(!book.get() || book->getContentType() == Timetable::CONTAINER);

			_book = book;
		}


		bool TimetableAddAction::isAuthorized(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(WRITE);
		}
	}
}
