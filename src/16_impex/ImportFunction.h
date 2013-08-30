
/** Import service class header.
	@file ImportFunction.h
	@author Hugues Romain
	@date 2009

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

#ifndef SYNTHESE_ImportFunction_H__
#define SYNTHESE_ImportFunction_H__

#include "Function.h"
#include "FactorableTemplate.h"
#include "Importer.hpp"

namespace synthese
{
	namespace impex
	{
		class DataSource;
		class FileFormat;

		//////////////////////////////////////////////////////////////////////////
		/// 16.15 Import service.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Import
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		///	@date 2009
		///	@ingroup m16Functions refFunctions
		class ImportFunction:
			public util::FactorableTemplate<server::Function, ImportFunction>
		{
		public:
			static const std::string PARAMETER_DO_IMPORT;	//!< do_import parameter
			static const std::string PARAMETER_IMPORT_ID;	//!< 
			static const std::string PARAMETER_LOG_PATH;	//!< 
			static const std::string PARAMETER_OUTPUT_LOGS;
			static const std::string PARAMETER_MIN_LOG_LEVEL;

			static const std::string ATTR_SUCCESS;
			static const std::string ATTR_DONE;
			static const std::string TAG_LOG_ENTRY;

			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// Initializes a dedicated environment for the storage of the imported
			/// data.
			ImportFunction();

		protected:
			//! \name Page parameters
			//@{
				bool							_doImport;
				bool							_importDone;
				boost::shared_ptr<Importer>		_importer;
				mutable util::ParametersMap		_result;
			//@}

			mutable std::stringstream _output;

			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Import#Request
			//////////////////////////////////////////////////////////////////////////
			/// @return Generated parameters map
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads each parameter and run the parsing of the input files.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Import#Request
			/// At the end of the parsing, the temporary environment used by the function
			/// is cleaned in order to avoid memory overload.
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_ImportFunction_H__
