
/** ComplexObjectField class header.
	@file ComplexObjectField.hpp

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

#ifndef SYNTHESE__ComplexObjectField_hpp__
#define SYNTHESE__ComplexObjectField_hpp__

#include "ComplexObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	class ObjectBase;
	class Record;
	class FilesMap;



	template<class C, class T>
	class ComplexObjectField:
		public ComplexObjectFieldDefinition<C>
	{
	public:
		typedef T Type;
		
		static void LoadFromRecord(
			T& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		);
		
		static void SaveToParametersMap(
			const T& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		);

		static void SaveToFilesMap(
			const T& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		);

		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list,
			const Record& record
		);
	};
}

#endif
