/** Registrable class implementation.
	@file Registrable.cpp
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

#include "Registrable.h"

#include "DBModule.h" // Temporary modules dependencies rule violation : will be useless when all objects will iherit from ObjectBase
#include "ParametersMap.h"
#include "RegistryKeyException.h"
#include "ObjectNotFoundException.h"

#include <assert.h>

using namespace boost;
using namespace std;

namespace synthese
{
	namespace util
	{
		const string Registrable::ATTR_ID = "id";



		Registrable::Registrable(
			RegistryKeyType key
		):	_key(key)
		{}



		Registrable::~Registrable()
		{}



		//////////////////////////////////////////////////////////////////////////
		/// Exports the content of the object into a ParametersMap object.
		///
		/// The default version exports only the id of the object and do not
		/// validate the postcondition.
		///
		/// @post The parameters map should contain all necessary field to re-import
		/// the object through loadFromRecord
		/// @param withFiles Exports fields as independent files
		/// @param withAdditionalParameters if true the map is filled up by
		/// addAdditionalParameters
		/// @retval map the ParametersMap to populate
		void Registrable::toParametersMap(
			util::ParametersMap& map,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{
			map.insert(ATTR_ID, _key);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Exports the content of the object into a FilesMap object (fields to store as files only).
		/// The default implementation exports nothing.
		/// @param map the FilesMap to fill
		void Registrable::toFilesMap( FilesMap& map ) const
		{
		}



		synthese::SubObjects Registrable::getSubObjects() const
		{
			return SubObjects();
		}



		bool Registrable::loadFromRecord( const Record& record, util::Env& env )
		{
			throw Exception("Method loadFromRecord not implemented for the object "+ lexical_cast<string>(_key));
			return false;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Gets the class number of the object.
		/// The default implementation reads the class number in the object key.
		/// Without defined key, the method can not work.
		/// @return the number of the class of the object
		/// @throw Exception if the object has no id
		util::RegistryTableType Registrable::getClassNumber() const
		{
			// Without id, the class number cannot be guessed
			if(!_key)
			{
				throw Exception("Method getClassNumber not implemented for an old-style object without id");
			}

			return decodeTableId(_key);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Gets the name of the table corresponding to the class
		/// @return the name of the table corresponding to the class of the object
		/// @throw Exception if the object has no id
		const std::string& Registrable::getTableName() const
		{
			RegistryTableType tableId(getClassNumber());
			boost::shared_ptr<db::DBTableSync> tableSync(
				db::DBModule::GetTableSync(tableId) // Temporary modules dependencies rule violation : will be useless when all objects will iherit from ObjectBase
			);
			return tableSync->getFormat().NAME;
		}
}	}
