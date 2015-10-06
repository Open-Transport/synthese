
/** OperationUnit class header.
	@file OperationUnit.hpp

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

#ifndef SYNTHESE_timetable_OperationUnit_hpp__
#define SYNTHESE_timetable_OperationUnit_hpp__

#include "NumericField.hpp"
#include "Object.hpp"
#include "PointersSetField.hpp"
#include "StringField.hpp"

#include "CommercialLine.h"
#include "SchemaMacros.hpp"

namespace synthese
{
	FIELD_POINTERS_SET(AllowedLines, pt::CommercialLine)

	namespace pt_operation
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(AllowedLines)
		> OperationUnitRecord;



		/** OperationUnit class.
			@ingroup m55
		*/
		class OperationUnit:
			public Object<OperationUnit, OperationUnitRecord>
		{
		public:
			static const std::string TAG_ALLOWED_LINE;

			OperationUnit(
				util::RegistryKeyType id = 0
			);

		private:

		public:

			/// @name Services
			//@{
				bool contains(const pt::CommercialLine& line) const;

				virtual void addAdditionalParameters(
					util::ParametersMap& map,
					std::string prefix = std::string()
				) const;
			//@}

			/// @name Modifiers
			//@{
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_timetable_OperationUnit_hpp__

