
/** TimetableAddAction class implementation.
	@file TimetableAddAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"

#include "TimetableAddAction.h"
#include "TimetableRight.h"
#include "TimetableRow.h"
#include "Timetable.h"
#include "TimetableTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
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
		
		
		TimetableAddAction::TimetableAddAction()
			: util::FactorableTemplate<Action, TimetableAddAction>()
		{
		}
		
		
		
		ParametersMap TimetableAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_BOOK_ID, _book.get() ? _book->getKey() : uid(0));
			return map;
		}
		
		
		
		void TimetableAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getDefault<RegistryKeyType>(PARAMETER_BOOK_ID, RegistryKeyType(0)))
			{
				try
				{
					_book = TimetableTableSync::Get(map.get<RegistryKeyType>(PARAMETER_BOOK_ID), *_env);
				}
				catch (...)
				{
					throw ActionException("No such book");
				}
			}
			_rank = map.getDefault<int>(PARAMETER_RANK, TimetableTableSync::GetMaxRank(_book.get() ? _book->getKey() : 0) + 1);
			_title = map.getDefault<string>(PARAMETER_TITLE);
			_isBook = map.get<bool>(PARAMETER_IS_BOOK);
		}
		
		
		
		void TimetableAddAction::run()
		{
			// timetable creation
			Timetable t;
			t.setContentType(_isBook ? Timetable::CONTAINER : Timetable::TABLE_SERVICES_IN_COLS);
			t.setBookId(_book.get() ? _book->getKey() : 0);
			t.setRank(_rank);
			t.setTitle(_title);

			// rank shifting
			TimetableTableSync::Shift(_book.get() ? _book->getKey() : 0, _rank, 1);

			// Saving
			TimetableTableSync::Save(&t);

			// ID update
			_request->setActionCreatedId(t.getKey());
		}



		void TimetableAddAction::setBook( boost::shared_ptr<const Timetable> book )
		{
			assert(!book.get() || book->getContentType() == Timetable::CONTAINER);

			_book = book;
		}
		
		
		bool TimetableAddAction::_isAuthorized() const
		{
			return _request->isAuthorized<TimetableRight>(WRITE);
		}
	}
}
