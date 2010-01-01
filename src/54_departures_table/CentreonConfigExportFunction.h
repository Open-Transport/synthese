
/** CentreonConfigExportFunction class header.
	@file DisplayScreenContentFunction.h

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

#ifndef SYNTHESE_DCentreonConfigExportFunctionRequest_H__
#define SYNTHESE_DCentreonConfigExportFunctionRequest_H__

#include "Function.h"
#include "FactorableTemplate.h"

#include <set>
#include <string>

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;
		class DisplayType;

		/** Centreon configuration export function class.

			Parameters :
				- ac=0|1 : action (0=clean the Centreon database, 1=export SYNTHESE configuration into Centreon database)

			@ingroup m54Functions refFunctions
		*/
		class CentreonConfigExportFunction :
			public util::FactorableTemplate<server::Function,CentreonConfigExportFunction>
		{
			typedef enum
			{
				CLEAN = 0,
				GENERATE = 1
			} Action;


			//! \name Page parameters
			//@{
				static const std::string PARAMETER_ACTION;
			//@}

			Action _action;

			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

			
			
			/** Makes a name compatible with Nagios.
				@param text name to convert
				@return converted name
				@author Hugues Romain
				@date 2009

				The Nagios compatible name mus respect the following rules :
					- No accentuated characters
					- Only alphanumeric characters
			*/
			static std::string _ConvertToNagiosName(const std::string& text);

		public:
			CentreonConfigExportFunction();

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const security::Profile& profile) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif
