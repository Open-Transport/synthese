////////////////////////////////////////////////////////////////////////////////
///	Importable class header.
///	@file Importable.h
///	@author Hugues Romain (RCS)
///	@date sam fév 21 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include <string>
#include <map>

namespace synthese
{
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
		public:
			typedef std::map<const DataSource*, std::string> DataSourceLinks;

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

		private:
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
				void setDataSourceLinks(const DataSourceLinks& value){ _dataSourceLinks = value; }
			//@}

			//! @name Modifiers
			//@{
				void setCodeBySource(const DataSource& source, const std::string& code);

				void cleanDataSourceLinks();
			//@}

			//! @name Services
			//@{
				bool hasLinkWithSource(const DataSource& source) const;

				bool hasUnknownOwner() const;

				const std::string& getCodeBySource(const DataSource& source) const;

				//////////////////////////////////////////////////////////////////////////
				/// Gets the code in the object owner database.
				/// @throws NotLinkedWithSourceException if the object does not have an
				/// anonymous owner.
				const std::string& getCodeBySource() const;

				std::string getCodeBySources() const;
			//@}

			//! @name Static algorithms
			//@{
			//@}
		};
	}
}

#endif // SYNTHESE_Importable_h__
