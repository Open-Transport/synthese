
/** MapModule class header.
	@file MapModule.h

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

#ifndef SYNTHESE_MapModule_H__
#define SYNTHESE_MapModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m59Actions 59 Actions
		@ingroup m59

		@defgroup m59Pages 59 Pages
		@ingroup m59

		@defgroup m59Functions 59 Functions
		@ingroup m59

		@defgroup m59Exceptions 59 Exceptions
		@ingroup m59

		@defgroup m59LS 59 Table synchronizers
		@ingroup m59

		@defgroup m59Admin 59 Administration pages
		@ingroup m59

		@defgroup m59Rights 59 Rights
		@ingroup m59

		@defgroup m59Logs 59 DB Logs
		@ingroup m59

		@defgroup m59 59 Map drawing
		@ingroup m5

		(Module documentation)

		Le tracé de carte se fait à partir de quatre catégories d'éléments d'entrée  :
			- données géographiques du serveur (chargées à l'initialisation) : voir @ref uc10 "liste des données"
			- données géographiques propres à la carte, fournies avec la demande de carte, permettant de tracer sur la carte des éléments supplémentaires provenant par exemple de l'application cliente. Les données pouvant être transmises à la volée sont les mêmes que les @ref uc10 "données du serveur" à l'exception des fonds de carte bitmap
			- définition du contenu de la carte
			- zone de couverture
			- échelle
			- définition de la liste des éléments géographiques à tracer
			- paramètres techniques
			- format du fichier produit
			- paramètres de compression

		Le résultat produit est un résultat contenant l'un ou l'autre ou les deux éléments suivants :
			- carte demandée au format bitmap
			- données vectorielles associées à la carte

		Selon le format choisi, les tables de liens et la carte sont ou non fabriqués :

		<table class="tableau">
		<tr><th>Format</th><th>Table de liens</th><th>Carte</th><th>Utilisation de Ghostscript</th></tr>
		<tr><th>PS</th><td>NON</td><td>OUI</td><td>NON</td></tr>
		<tr><th>PDF</th><td>OUI</td><td>OUI</td><td>OUI</td></tr>
		<tr><th>JPEG</th><td>NON</td><td>OUI</td><td>OUI</td></tr>
		<tr><th>HTML</th><td>OUI</td><td>NON</td><td>NON</td></tr>
		<tr><th>Mapinfo</th><td>OUI</td><td>NON</td><td>NON</td></tr>
		</table>

		@{
	*/

	/** 59 Map module namespace. */
	namespace map
	{

		/** 59 Map module class. */
		class MapModule:
			public server::ModuleClassTemplate<MapModule>
		{
		public:

		    static const std::string PARAM_HTTP_TEMP_DIR;
		    static const std::string PARAM_HTTP_TEMP_URL;
		    static const std::string PARAM_BACKGROUNDS_DIR;

		    /** Called whenever a parameter registered by this module is changed
		     */
		    static void ParameterCallback (const std::string& name,
						   const std::string& value);
		};
	}
	/** @} */
}

#endif // SYNTHESE_MapModule_H__
