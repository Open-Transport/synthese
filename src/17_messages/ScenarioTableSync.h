////////////////////////////////////////////////////////////////////////////////
/// ScenarioTableSync class header.
///	@file ScenarioTableSync.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_ScenarioTableSync_H__
#define SYNTHESE_ScenarioTableSync_H__

#include "Scenario.h"
#include "AlarmTableSync.h"
#include "AlarmTemplate.h"
#include "SentAlarm.h"

#include "DBInheritanceTableSyncTemplate.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class ScenarioTemplate;

		////////////////////////////////////////////////////////////////////
		/// Scenario table synchronizer.
		///	@ingroup m17LS refLS
		///
		///	@note As Scenario is an abstract class, do not use the get static
		/// method. Use getAlarm instead.
		///
		/// Only sent scenarios store its variables into the table. The
		/// scenario template contains all definitions within the text of
		/// its alarms.
		///
		/// The format of the variables column is :
		///		- for sent scenarios : <variable>|<value>, ...
		///
		class ScenarioTableSync : public db::DBInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>
		{
		public:
			static const std::string COL_IS_TEMPLATE;
			static const std::string COL_ENABLED;
			static const std::string COL_NAME;
			static const std::string COL_PERIODSTART;
			static const std::string COL_PERIODEND;
			static const std::string COL_FOLDER_ID;
			static const std::string COL_VARIABLES;
			static const std::string COL_TEMPLATE;

			ScenarioTableSync();
			~ScenarioTableSync();
		};
	}
}

#endif // SYNTHESE_ScenarioTableSync_H__
