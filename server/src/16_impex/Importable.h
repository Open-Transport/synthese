////////////////////////////////////////////////////////////////////////////////
///	Importable class header.
///	@file Importable.h
///	@author Hugues Romain (RCS)
///	@date 2009
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

#ifndef SYNTHESE_Importable_h__
#define SYNTHESE_Importable_h__

#include "Registrable.h"

#include "Exception.h"
#include "SchemaMacros.hpp"

#include <string>
#include <map>
#include <vector>
#include <boost/foreach.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace impex
	{
		class DataSource;

		////////////////////////////////////////////////////////////////////////
		/// Importable class.
		/// Defines links between data sources and an object.
		/// A link can contain the id of the object in the source database. Such
		/// an id can follow any pattern The only one limitation is not to
		/// contain the | or , characters.
		/// For historical reasons, an importable object can know its code in
		/// the database of its owner, without knowing its datasource.
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		/// @ingroup m16
		class Importable:
			virtual public util::Registrable
		{
		protected:
			static const std::string TAG_DATA_SOURCE_LINK;
			static const std::string ATTR_DATA_SOURCE_ID;
			static const std::string ATTR_DATA_SOURCE_NAME;
			static const std::string ATTR_ID;

		public:
			static const char SOURCES_SEPARATOR;
			static const char FIELDS_SEPARATOR;
			static const char SEPARATOR_ESCAPE;

			typedef std::multimap<const DataSource*, std::string> DataSourceLinks;

			class NotLinkedWithSourceException : public synthese::Exception
			{
			public:
				NotLinkedWithSourceException(
					const Importable& object,
					const DataSource& source
				);
			};

			class NotHaveUnknownDataSource : public Exception
			{
			public:
				NotHaveUnknownDataSource(
					const Importable& object
				);
			};

		protected:
			// Attributes
			DataSourceLinks _dataSourceLinks;

		public:
			/////////////////////////////////////////////////////////////////////
			/// DataSource Constructor.
			Importable();

			//! @name Getters
			//@{
				const DataSourceLinks& getDataSourceLinks() const { return _dataSourceLinks; }
			//@}

			//! @name Setters
			//@{
			//@}

			//! @name Modifiers
			//@{
				void setDataSourceLinksWithoutRegistration(const DataSourceLinks& value){ _dataSourceLinks = value; }
				virtual void setDataSourceLinksWithRegistration(const DataSourceLinks& value) = 0;

			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Checks if the object is linked with the specified data source.
				/// @param source the source with which the object must be linked
				/// @return true if the object is linked with the specified data source.
				bool hasLinkWithSource(const DataSource& source) const;

				bool hasUnknownOwner() const;

				//////////////////////////////////////////////////////////////////////////
				/// Returns the first code for the datasource.
				const std::string& getACodeBySource(const DataSource& source) const;


				bool hasCodeBySource(const DataSource& source, const std::string& code) const;


				std::vector<std::string> getCodesBySource(const DataSource& source) const;


				//////////////////////////////////////////////////////////////////////////
				/// Gets the code in the object owner database.
				/// @throws NotLinkedWithSourceException if the object does not have an
				/// anonymous owner.
				const std::string& getACodeBySource() const;

				std::string getCodeBySources() const;
				std::string getDataSourceName() const;


				void dataSourceLinksToParametersMap(
					util::ParametersMap& pm
				) const;
			//@}

			//! @name Static algorithms
			//@{
			//@}
		};
}	}

#endif // SYNTHESE_Importable_h__
