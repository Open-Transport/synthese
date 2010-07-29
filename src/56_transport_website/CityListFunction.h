
/** CityListRequest class header.
	@file CityListRequest.h

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

#ifndef SYNTHESE_CityListRequest_H__
#define SYNTHESE_CityListRequest_H__

#include "FunctionWithSite.h"
#include "FactorableTemplate.h"

#include <string>

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace transportwebsite
	{
		class PlacesListInterfacePage;

		/** City list query public function.
			@ingroup m56Functions refFunctions

			<h3>Request</h3>

			<pre>
			fonction=lc
			si=<id website> : site id
			t=<texte saisi> : texte entré par l’utilisateur
			n=<nombre resultats> : nombre de résultats devant être fournis par le serveur
			p=<id> : id of page to use to display the list by PlacesListInterfacePage
			ip=<id> : id of page to use to display each item of the list
			</pre>

			<h3>Réponse</h3>

			La réponse propose, dans l’ordre décroissant de pertinence, les n lieux de la commune, dont le nom
			est le plus proche possible du texte entré.

			Si p est spécifié, la réponse est définie par la page PlacesListInterfacePage désignée.
			Si p n'est pas spécifié, la réponse respecte le schéma XML suivant :
			
			@image html options.png
			
			Les objets suivants sont définis :
			<ul>
			<li>options : balise racine</li>
			<li>option : définit un élément retourné</li>
			<li>score : taux de correspondance entre le texte proposé et le texte entré, entre 0
			(limite basse théorique) et 1 (texte identique).</li>
			<li>Type : type d’objet retourné, porte systématiquement la valeur <i>city</i>, choix parmi les valeurs suivantes (ne peut être que
			city dans le cas d’une recherche de commune) :
			<ul>
			<li>city : commune</li>
			<li>stop : arrêt du réseau de transport</li>
			<li>publicPlace : lieu public</li>
			<li>street : rue entière (tous points de la rue considérés équivalents)</li>
			<li>address : adresse sur une rue (point précis sur la rue)</li>
			</ul></ul>

			<h3>Attachments</h3>

			<ul>
			<li><a href="include/56_transport_website/places_list.xsd">Response XSD schema</a></li>
			<li><a href="include/56_transport_website/places_listSample.xml">Example of XML response</a></li>
			</ul>
		*/
		class CityListFunction:
			public util::FactorableTemplate<cms::FunctionWithSite<true>, CityListFunction>
		{
		public:
			static const std::string PARAMETER_INPUT;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_IS_FOR_ORIGIN;
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_ITEM_PAGE;

		private:
			std::string _input;
			int _n;
			bool _isForOrigin;
			boost::shared_ptr<const cms::Webpage>	_page;
			boost::shared_ptr<const cms::Webpage>	_itemPage;

		protected:
			server::ParametersMap _getParametersMap() const;
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			void run(std::ostream& stream, const server::Request& request) const;

			void setTextInput(const std::string& text);
			void setNumber(int number);
			void setIsForOrigin(bool isForOrigin);

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_CityListRequest_H__
