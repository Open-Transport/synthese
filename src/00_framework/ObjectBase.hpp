
/** ObjectBase class header.
	@file ObjectBase.hpp

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

#ifndef SYNTHESE__ObjectBase_hpp__
#define SYNTHESE__ObjectBase_hpp__

#include "Registrable.h"

#include "FrameworkTypes.hpp"

#include <string>
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

	typedef std::vector<ObjectBase*> SubObjects;

	/** ObjectBase class.
		@ingroup m
	*/
	class ObjectBase:
		public virtual util::Registrable
	{
	public:
		ObjectBase(
			util::RegistryKeyType id = 0
		):	util::Registrable(id)
		{}


		/// @name Methods to overload if necessary
		//@{
			virtual std::string getName() const { return std::string(); }
			virtual SubObjects getSubObjects() const { return SubObjects(); }
			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false) {}
			virtual void unlink() {}

			
			//////////////////////////////////////////////////////////////////////////
			/// Adds parameters that are not intended to be saved (i.e. generated content).
			/// The default implementation adds nothing. This method may be overloaded
			/// @param map the map to populate
			/// @param prefix prefix to add to the keys of the map items
			virtual void addAdditionalParameters(
				util::ParametersMap& map,
				std::string prefix = std::string()
			) const {}
		//@}

		/// @name Methods automatically defined by the Object template
		//@{
			virtual util::RegistryTableType getClassNumber() const = 0;
			virtual const std::string& getClassName() const = 0;
			virtual const std::string& getTableName() const = 0;
			virtual FieldsList getFields() const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Loads the content of a record into the current object.
			/// @param record the record to load
			/// @param env the environment to read to get the linked objects
			/// @warning be sure the environment is populated before the load
			virtual void loadFromRecord(
				const Record& record,
				util::Env& env
			) = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Exports the content of the object into a ParametersMap object.
			/// @param withFiles Exports fields according to their EXPORT_CONTENT_AS_FILE
			/// attribute.
			/// @param withAdditionalParameters if true the map is filled up by
			/// addAdditionalParameters
			/// @param map the generated ParametersMap
			virtual void toParametersMap(
				util::ParametersMap& map,
				bool withAdditionalParameters = false,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()
			) const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the list of object to get in the environment for current
			/// object loading, according to the content of the record to load.
			/// @param record the record to load
			virtual LinkedObjectsIds getLinkedObjectsIds(
				const Record& record
			) const = 0;
		//@}
	};
}

#endif // SYNTHESE__ObjectBase_hpp__

