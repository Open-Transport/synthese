
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
	namespace departure_boards
	{
		class DisplayScreen;
		class DisplayType;

		//////////////////////////////////////////////////////////////////////////
		/// Centreon configuration export function class.
		/// @author Hugues Romain
		/// @date 2009
		/// @ingroup m54Functions refFunctions
		//////////////////////////////////////////////////////////////////////////
		/// <h2>Parameters</h2>
		///	<ul>
		///		<li>ac=0|1 : action (0=clean the Centreon database, 1=export SYNTHESE configuration into Centreon database)</li>
		///	</ul>
		///
		/// <h2>Output</h2>
		/// Le principe originel de Nagios (host/service) est l�g�rement d�tourn� pour prendre en compte les sp�cificit�s de l'architecture :
		///	<ul>
		///		<li>Les � hosts � correspondent aux afficheurs et aux unit�s centrales. Ils seront nomm�s COMMUNE_Arret_Nom pour b�n�ficier de tris automatiques � l'affichage par Centr�on. Leur type sera rappel� dans le champ Alias.</li>
		///		<li>Un service est d�clar� par host, d�pendant du type d'entr�e : un service de supervision d'afficheur ou un service de supervision d'unit� centrale</li>
		///		<li>Les afficheurs d�pendants d'une unit� centrale sont d�clar�s comme tels : en cas de panne de l'unit� centrale, aucune alerte n'est envoy�e concernant les afficheurs branch�s dessus.</li>
		///		<li>Le champ IP des hosts est utilis� pour stocker l'ID</li>
		///	</ul>
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

			//////////////////////////////////////////////////////////////////////////
			/// Generates the output of the function.
			/// @param stream stream to write the output on
			/// @param request request which has launched the function
			//////////////////////////////////////////////////////////////////////////
			void run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif
