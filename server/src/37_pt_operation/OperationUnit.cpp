
/** OperationUnit class implementation.
	@file OperationUnit.cpp

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

#include "OperationUnit.hpp"

#include "Profile.h"
#include "Session.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace pt_operation;

	CLASS_DEFINITION(OperationUnit, "t116_operation_units", 116)
	FIELD_DEFINITION_OF_OBJECT(OperationUnit, "operation_unit_id", "operation_unit_ids")

	FIELD_DEFINITION_OF_TYPE(AllowedLines, "allowed_lines", SQL_TEXT)
	
	namespace pt_operation
	{
		const string OperationUnit::TAG_ALLOWED_LINE = "allowed_line";



		OperationUnit::OperationUnit(
			RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<OperationUnit, OperationUnitRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(AllowedLines)
			)	)
		{}



		bool OperationUnit::contains(
			const CommercialLine& line
		) const	{

			return get<AllowedLines>().find(&const_cast<CommercialLine&>(line)) != get<AllowedLines>().end();
		}



		void OperationUnit::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			// Detail about allowed lines
			AllowedLines::Type allowedLines(get<AllowedLines>()); // Copy for thread safety
			BOOST_FOREACH(const CommercialLine* line, allowedLines)
			{
				boost::shared_ptr<ParametersMap> linePM(new ParametersMap);
				line->toParametersMap(*linePM, false);
				map.insert(TAG_ALLOWED_LINE, linePM);
			}

		}


		bool OperationUnit::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool OperationUnit::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool OperationUnit::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
