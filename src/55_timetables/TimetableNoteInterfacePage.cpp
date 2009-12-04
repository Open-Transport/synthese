
/** TimetableNoteInterfacePage class implementation.
	@file TimetableNoteInterfacePage.cpp
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

#include "TimetableNoteInterfacePage.h"
#include "TimetableWarning.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace timetables;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, timetables::TimetableNoteInterfacePage>::FACTORY_KEY("timetable_note");
	}

	namespace timetables
	{
		TimetableNoteInterfacePage::TimetableNoteInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, TimetableNoteInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void TimetableNoteInterfacePage::display(
			std::ostream& stream,
			const TimetableWarning& object,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
		
			pv.push_back(lexical_cast<string>(object.getNumber()));
			pv.push_back(object.getText());

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
