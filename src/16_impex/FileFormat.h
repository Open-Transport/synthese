/** Import/export file format class header.
	@file FileFormat.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_impex_FileFormat_h__
#define SYNTHESE_impex_FileFormat_h__

#include "FactoryBase.h"

#include <boost/filesystem/path.hpp>
#include <set>
#include <map>

////////////////////////////////////////////////////////////////////
/// @defgroup refFile 16 File formats
///	@ingroup ref

namespace synthese
{
	namespace db
	{
		class SQLiteTransaction;
	}

	namespace impex
	{
		class DataSource;
		class Importer;
		class Exporter;
		
		/** Import/export file format factorable base class.
			
			Each import subclass defines a format which can be imported.
			
			@ingroup m16

			Import

			There is two ways to launch the data import upon file paths :
				- run parseFiles with a paths set argument : there is only one file type to read, 
					but the data can be provided in more than one file. The set contains
					all the paths.
				- run parseFiles with a paths map argument : there is more than one file type, each
					type contains a part of the fields to import and is identified by a key.
					The key is also the parameter code Request field containing the file path.
					The files are loaded respecting the key alphabetical order.
		*/
		class FileFormat:
			public util::FactoryBase<FileFormat>
		{
		public:
			virtual bool canImport() const = 0;
			virtual bool canExport() const = 0;

			virtual boost::shared_ptr<Importer> getImporter(
				const DataSource& dataSource
			) const = 0;
			
			virtual boost::shared_ptr<Exporter> getExporter(
			) const	= 0;

		};
	}
}

#endif // SYNTHESE_impex_Import_h__
