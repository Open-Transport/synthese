
//////////////////////////////////////////////////////////////////////////
/// CleanObsoleteDataAction class implementation.
/// @file CleanObsoleteDataAction.cpp
/// @author Hugues Romain
/// @date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "CleanObsoleteDataAction.hpp"

#include "ActionException.h"
#include "Request.h"
#include "ImportTableSync.hpp"
#include "Importer.hpp"
#include "FileFormat.h"

using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, impex::CleanObsoleteDataAction>::FACTORY_KEY("CleanObsoleteDataAction");
	}

	namespace impex
	{
		const string CleanObsoleteDataAction::PARAMETER_IMPORT_ID = Action_PARAMETER_PREFIX + "_import_id";
		const string CleanObsoleteDataAction::PARAMETER_FIRST_DATE = Action_PARAMETER_PREFIX + "first_date";


		ParametersMap CleanObsoleteDataAction::getParametersMap() const
		{
			ParametersMap map;
			if(_importer.get())
			{
				map.insert(PARAMETER_IMPORT_ID, _importer->getImport().getKey());
			}
			if(!_firstDate.is_not_a_date())
			{
				map.insert(PARAMETER_FIRST_DATE, _firstDate);
			}
			return map;
		}



		void CleanObsoleteDataAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Import
			try
			{
				setImport(*ImportTableSync::Get(map.get<RegistryKeyType>(PARAMETER_IMPORT_ID), *_env));
			}
			catch(ObjectNotFoundException<Import>&)
			{
				throw ActionException("No such import");
			}
			if(!_importer.get())
			{
				throw ActionException("Invalid import : maybe file format is undefined ?");
			}

			// Date
			_firstDate = from_string(map.get<string>(PARAMETER_FIRST_DATE));
		}



		void CleanObsoleteDataAction::run(
			Request& request
		){
			_importer->cleanObsoleteData(_firstDate);
		}



		bool CleanObsoleteDataAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		void CleanObsoleteDataAction::setImport(const Import& value)
		{
			try
			{
				_importer  = value.getImporter(
					*_env,
					IMPORT_LOG_WARN,
					string(),
					_output,
					_pm
				);
			}
			catch(FactoryException<FileFormat>&)
			{
			}
		}



		CleanObsoleteDataAction::CleanObsoleteDataAction():
		FactorableTemplate<server::Action, CleanObsoleteDataAction>(),
			_firstDate(not_a_date_time)
		{

		}
	}
}
