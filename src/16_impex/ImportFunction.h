
/** TridentExportFunction class header.
	@file TridentImportFunction.h
	@author Hugues Romain
	@date 2009

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

#ifndef SYNTHESE_ImportFunction_H__
#define SYNTHESE_ImportFunction_H__

#include "Function.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;
		
		/** Import Function class.
			
			Usage :
			@code
data_source=<id>
path=file1,file2,file3,file4...
do_import=0|1
			@endcode
			
				data_source : id of the data source referring to the ImportDataSource table
				path [compulsory] : each file to import (one per line) separated by comas
				do_import :
					- 0 = simulates the import, display all warning messages
					- 1 = do the import with the following rules :
						- all the future service dates of the lines handled by the data source are deleted
						- if a stop is missing in a line, all the routes of the line are ignored (even those which not use
							acttually the missing stops). This ignored routes are displayed in the output
						- the commercial lines are automatically created
						
				
			@author Hugues Romain
			@date 2009
			@ingroup m16Functions refFunctions
		*/
		class ImportFunction:
			public util::FactorableTemplate<server::Function, ImportFunction>
		{
		public:
			static const std::string PARAMETER_PATH;	//!< Path of the files to import
			static const std::string PARAMETER_DO_IMPORT;	//!< do_import parameter
			static const std::string PARAMETER_DATA_SOURCE;	//!< clear_network parameter

			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// Initializes a dedicated environment for the storage of the imported
			/// data.
			ImportFunction();

		protected:
			//! \name Page parameters
			//@{
				bool								_doImport;
				boost::shared_ptr<const DataSource>	_dataSource;
				boost::shared_ptr<FileFormat>		_fileFormat;
				std::string							_output;
			//@}
			
			
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;
			
			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);
			
		public:
			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;

			virtual bool _isAuthorized() const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_TridentExportFunction_H__
