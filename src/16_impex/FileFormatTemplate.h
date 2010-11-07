/** File Format template class header.
	@file FileFormatTemplate.h

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

#ifndef SYNTHESE_impex_FileFormatTemplate_h__
#define SYNTHESE_impex_FileFormatTemplate_h__

#include "FileFormat.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace impex
	{
		/*	File format template.
		*/
		template<class FF>
		class FileFormatTemplate:
			public util::FactorableTemplate<FileFormat,FF>
		{
		public:
			virtual bool canImport() const { return FF::Importer_::IMPORTABLE; }
			virtual bool canExport() const { return FF::Exporter_::EXPORTABLE; }

			virtual boost::shared_ptr<Importer> getImporter(
				const DataSource& dataSource
			) const	{
				return
					boost::static_pointer_cast<Importer, FF::Importer_>(
						boost::shared_ptr<FF::Importer_>(
							new FF::Importer_(dataSource)
					)	);
			}

			virtual boost::shared_ptr<Exporter> getExporter(
			) const	{
				return
					boost::static_pointer_cast<Exporter, FF::Exporter_>(
						boost::shared_ptr<FF::Exporter_>(
							new FF::Exporter_()
					)	);
			}
		};
	}
}

#endif // SYNTHESE_impex_Import_h__
