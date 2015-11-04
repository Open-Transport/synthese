
/** TimetableModule class implementation.
	@file TimetableModule.cpp

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

#include "TimetableModule.h"

#include "CalendarTemplate.h"
#include "Env.h"
#include "StopArea.hpp"
#include "Timetable.h"
#include "TimetableTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace timetables;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,TimetableModule>::FACTORY_KEY("55_timetables");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<TimetableModule>::NAME("Fiches horaires");

		template<> void ModuleClassTemplate<TimetableModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<TimetableModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<TimetableModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<TimetableModule>::End()
		{
		}



		template<> void ModuleClassTemplate<TimetableModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<TimetableModule>::CloseThread(
			
			){
		}
	}

	namespace timetables
	{
		TimetableModule::TimetableContainersLabels TimetableModule::GetTimetableContainersLabels(
			util::RegistryKeyType folderId /*= 0*/,
			std::string prefix /*= std::string() */,
			boost::optional<util::RegistryKeyType> forbiddenFolderId /*= boost::optional<util::RegistryKeyType>() */
		){
			TimetableContainersLabels m;
			if (folderId == 0)
				m.push_back(make_pair(0, "(racine)"));

			Env env;
			TimetableTableSync::SearchResult folders(
				TimetableTableSync::Search(env, folderId)
			);
			BOOST_FOREACH(const boost::shared_ptr<Timetable>& folder, folders)
			{
				if (folder->getKey() == forbiddenFolderId)
					continue;

				m.push_back(make_pair(folder->getKey(), prefix + folder->get<Title>()));

				TimetableContainersLabels r(GetTimetableContainersLabels(folder->getKey(), prefix + folder->get<Title>() +"/", forbiddenFolderId));
				m.insert(m.end(),r.begin(), r.end());
			}
			return m;
		}
	}
}
