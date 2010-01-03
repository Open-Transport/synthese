
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
#include "Timetable.h"
#include "Interface.h"
#include "TimetableInterfacePage.h"
#include "Env.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace interfaces;

	template<> const string util::FactorableTemplate<Function,timetables::TimetableGenerateFunction>::FACTORY_KEY("TimetableGenerateFunction");
	
	namespace timetables
	{
		TimetableGenerateFunction::TimetableGenerateFunction():
			FactorableTemplate<server::Function,TimetableGenerateFunction>()
		{
			setEnv(shared_ptr<Env>(new Env));
		}



		ParametersMap TimetableGenerateFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_timetable.get()) map.insert(Request::PARAMETER_OBJECT_ID, _timetable->getKey());
			return map;
		}

		void TimetableGenerateFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_timetable = TimetableTableSync::Get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), *_env);
			}
			catch(ObjectNotFoundException<Timetable>)
			{
				throw RequestException("No such timetable");
			}

			if(!_timetable->getInterface())
			{
				throw RequestException("This timetable has not interface");
			}

			if(!_timetable->getInterface()->hasPage<TimetableInterfacePage>())
			{
				throw RequestException("This timetable uses an incompatible interface");
			}
		}

		void TimetableGenerateFunction::run( std::ostream& stream, const Request& request ) const
		{
			auto_ptr<TimetableGenerator> generator(_timetable->getGenerator(Env::GetOfficialEnv()));
			generator->build();
			const TimetableInterfacePage* page(_timetable->getInterface()->getPage<TimetableInterfacePage>());
			VariablesMap variables;
			page->display(stream, *_timetable, *generator, variables, &request);
		}
		
		
		
		bool TimetableGenerateFunction::isAuthorized(const server::Session* session) const
		{
			return true;
		}



		std::string TimetableGenerateFunction::getOutputMimeType() const
		{
			return "text/html";
		}



		void TimetableGenerateFunction::setTimetable( boost::shared_ptr<const Timetable> value )
		{
			_timetable = value;
		}
	}
}
