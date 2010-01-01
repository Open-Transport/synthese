
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

	namespace transportwebsite
	{
		class PlacesListInterfacePage;

		/** City list request.
			@ingroup m36Functions refFunctions


			fonction=lc
			si=7036874417766401
			t=<texte saisi> : texte entr� par l�utilisateur
			n=<nombre resultats> : nombre de r�sultats devant �tre fournis par le serveur
			R�ponse
			La r�ponse propose, dans l�ordre d�croissant de pertinence, les n communes dont le nom
			est le plus proche possible du texte entr�.
			Les objets suivants sont d�finis :
			� options : balise racine
			� option : d�finit un �l�ment retourn�
			� score : taux de correspondance entre le texte propos� et le texte entr�, entre 0
			(limite basse th�orique) et 1 (texte identique).
			� Type : type d�objet retourn�, choix parmi les valeurs suivantes (ne peut �tre que
			city dans le cas d�une recherche de commune) :
			o city : commune
			o stop : arr�t du r�seau de transport
			o publicPlace : lieu public
			o street : rue enti�re (tous points de la rue consid�r�s �quivalents)
			o address : adresse sur une rue (point pr�cis sur la rue)
		*/
		class CityListRequest : public util::FactorableTemplate<FunctionWithSite,CityListRequest>
		{
		public:
			static const std::string PARAMETER_INPUT;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_IS_FOR_ORIGIN;
			static const std::string PARAMETER_PAGE;
			
		private:
			std::string _input;
			int _n;
			bool _isForOrigin;
			const PlacesListInterfacePage*	_page;

		protected:
			server::ParametersMap _getParametersMap() const;
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			CityListRequest();

			void run(std::ostream& stream, const server::Request& request) const;

			void setTextInput(const std::string& text);
			void setNumber(int number);
			void setIsForOrigin(bool isForOrigin);

			virtual bool isAuthorized(const security::Profile& profile) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_CityListRequest_H__
