
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

#include "Exception.h"
#include "FrameworkTypes.hpp"

#include <map>
#include <string>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	// For allowCreate and allowUpdate possible overloads only :
	// not used in the default implementation so the server module is not
	// linked with the framework module
	namespace server
	{
		class Session;
	}

	// For beforeUpdate and afterUpdate possible overloads only :
	// not used in the default implementation so the db module is not
	// linked with the framework module
	namespace db
	{
		class DBTransaction;
	}

	// TODO migrate these classes into the framework module
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	class ObjectBase;
	class Record;
	class FilesMap;


	//////////////////////////////////////////////////////////////////////////
	/// Object base interface.
	/// Base class of every object using the definition by record.
	//////////////////////////////////////////////////////////////////////////
	/// @author Hugues Romain
	/// @date 2012
	/// @ingroup m00
	/// @todo merge it with Registrable when all existing classes will be migrated
	/// as ObjectBase implementations
	class ObjectBase:
		public virtual util::Registrable
	{
	public:

		static std::map<util::RegistryTableType, std::string> REGISTRY_NAMES_BY_TABLE_ID;
		
		ObjectBase(
			util::RegistryKeyType id = 0
		):	util::Registrable(id)
		{}


		class IntegrityException:
			public Exception
		{
			IntegrityException(
				const ObjectBase& object,
				const std::string& field,
				const std::string& value,
				const std::string& problem
			);
			IntegrityException(
				const ObjectBase& object,
				const std::string& problem
			);
		};


		/// @name Methods to overload if necessary
		//@{
			virtual void checkIntegrity() const throw(IntegrityException) {}
			virtual bool allowUpdate(const server::Session* session) const { return false; }
			virtual void beforeUpdate(const ObjectBase& existingObject, boost::optional<db::DBTransaction&> transaction) const { }
			virtual void afterUpdate(const ObjectBase& existingObject, boost::optional<db::DBTransaction&> transaction) const { }
			virtual bool allowCreate(const server::Session* session) const { return false; }
			virtual void beforeCreate(boost::optional<db::DBTransaction&> transaction) const { }
			virtual void afterCreate(boost::optional<db::DBTransaction&> transaction) const { }
			virtual bool allowDelete(const server::Session* session) const { return false; }
			virtual void beforeDelete(boost::optional<db::DBTransaction&> transaction) const { }
			virtual void afterDelete(boost::optional<db::DBTransaction&> transaction) const { }

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
			virtual const std::string& getClassName() const = 0;
			virtual FieldsList getFields() const = 0;
			virtual const void* _dynamic_get(const std::string& fieldKey) const = 0;
			virtual void _dynamic_set(const void* value, const std::string& fieldKey) = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Exports the content of the object into a DBContent object (to be stored in
			/// the database).
			virtual DBContent toDBContent() const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Constructs a new copy of the current object.
			/// @warning only the record is copied : the created object will probably
			/// be not usable as is. Use standard = operator for real object copy.
			virtual boost::shared_ptr<ObjectBase> copy() const = 0;
		//@}

		/// @name Services
		//@{
			//////////////////////////////////////////////////////////////////////////
			/// Gets dynamically the value of a field.
			/// @warning the performances of this method are poor : use the static get
			/// method of the typed object whenever it is possible
			/// @return the value of the field
			template<class FieldClass>
			const typename FieldClass::Type& dynamic_get() const;



			//////////////////////////////////////////////////////////////////////////
			/// Sets dynamically the value of a field.
			/// @warning the performances of this method are poor : use the static set
			/// method of the typed object whenever it is possible
			/// @param value the new value for the field
			template<class FieldClass>
			void dynamic_set(const typename FieldClass::Type& value);

			
			
			//////////////////////////////////////////////////////////////////////////
			/// Checks dynamically if the object has a field according to its schema.
			/// @warning the performances of this method are poor : use it only for
			/// dynamic contexts
			/// @return true if the field exists in the class of the object
			template<class FieldClass>
			bool hasField() const;
		//@}
	};



	template<class FieldClass>
	bool ObjectBase::hasField() const
	{
		return _dynamic_get(FieldClass::GetFieldKey()) != NULL;
	}



	template<class FieldClass>
	const typename FieldClass::Type& ObjectBase::dynamic_get() const
	{
		const void* result(_dynamic_get(FieldClass::GetFieldKey()));
		if(!result)
		{
			throw Exception("No such field");
		}
		return *( (const typename FieldClass::Type*) (result) );
	}



	template<class FieldClass>
	void ObjectBase::dynamic_set(
		const typename FieldClass::Type& value
	){
		_dynamic_set(
			(const void*) &value,
			FieldClass::GetFieldKey()
		);
	}
}

#endif // SYNTHESE__ObjectBase_hpp__

