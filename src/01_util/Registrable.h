////////////////////////////////////////////////////////////////////////////////
/// Registrable class header.
///	@file Registrable.h
///	@author Hugues Romain
///	@date 2008-12-26 17:46
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_UTIL_REGISTRABLE_H
#define SYNTHESE_UTIL_REGISTRABLE_H

#include "UtilTypes.h"
#include "Field.hpp"
#include "FrameworkTypes.hpp"
#include "UtilConstants.h"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	class FilesMap;
	class Record;

	namespace util
	{
		class Env;
		class Registrable;
	}

	typedef std::vector<util::Registrable*> SubObjects;

	namespace util
	{
		class ParametersMap;



		////////////////////////////////////////////////////////////////////
		/// Base class for all registrable objects.
		///
		///	As this class is very much used, it is dangerous to derivate directly from it.
		///	Use a virtual derivation to ensure that there is always only one instantiation of the
		///	class in a sub-class instantiation.
		///
		///	Each Registrable class must determinate its Registry class by a typedef called Registry.
		///	Two ways are possible :
		///		- util::Registry : standard registry, allows abstract classes, but auto creation is forbidden
		///		- util::RegistryWithAutocreationEnabled : allows auto creation but incompatible with abstract classes
		///
		///	@ingroup m01Registry
		////////////////////////////////////////////////////////////////////
		class Registrable
		{
		public:
			static const std::string ATTR_ID;

		private:
			RegistryKeyType _key;	//!< ID of the object

		protected:
			////////////////////////////////////////////////////////////////////
			///	Registrable constructor.
			///	@param key ID of the object (use 0 if not determined)
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			explicit Registrable(RegistryKeyType key = 0);


		public:
			virtual ~Registrable();

			//! \name Getters
			//@{
				////////////////////////////////////////////////////////////////////
				/// ID of the object getter.
				///	@return the ID of the object.
				////////////////////////////////////////////////////////////////////
				virtual RegistryKeyType getKey() const { return _key; }


				virtual FieldsList getFields() const { return FieldsList(); }
			//@}

			//! \name Setters
			//@{
				////////////////////////////////////////////////////////////////////
				/// ID of the object setter.
				///	@param key the ID of the object to set
				////////////////////////////////////////////////////////////////////
				virtual void setKey(RegistryKeyType key){ _key = key; }
			//@}


			/// @name Importers/Exporters
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Builds the list of object to get in the environment for current
				/// object loading, according to the content of the record to load.
				/// @param record the record to load
				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const { return LinkedObjectsIds(); }

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false) {}
				virtual void unlink() {}

				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual void toFilesMap(
					FilesMap& map
				) const;

				virtual SubObjects getSubObjects() const;

				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);

				virtual util::RegistryTableType getClassNumber() const;

				virtual std::string getName() const { return std::string(); }

				virtual const std::string& getTableName() const;
			//@}
		};
}	}

#endif
