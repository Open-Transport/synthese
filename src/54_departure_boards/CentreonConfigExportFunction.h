
/** CentreonConfigExportFunction class header.
	@file DisplayScreenContentFunction.h

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
		/// Le principe originel de Nagios (host/service) est légèrement détourné pour prendre en compte les spécificités de l'architecture :
		///	<ul>
		///		<li>Les « hosts » correspondent aux afficheurs et aux unités centrales. Ils seront nommés COMMUNE_Arret_Nom pour bénéficier de tris automatiques à l'affichage par Centréon. Leur type sera rappelé dans le champ Alias.</li>
		///		<li>Un service est déclaré par host, dépendant du type d'entrée : un service de supervision d'afficheur ou un service de supervision d'unité centrale</li>
		///		<li>Les afficheurs dépendants d'une unité centrale sont déclarés comme tels : en cas de panne de l'unité centrale, aucune alerte n'est envoyée concernant les afficheurs branchés dessus.</li>
		///		<li>Le champ IP des hosts est utilisé pour stocker l'ID</li>
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
			util::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);



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
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif
