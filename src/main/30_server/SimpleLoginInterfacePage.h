
/** SimpleLoginInterfacePage class header.
	@file SimpleLoginInterfacePage.h

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

#ifndef SYNTHESE_SimpleLoginInterfacePage_H__
#define SYNTHESE_SimpleLoginInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace server
	{
		class Request;

		/** Ecran de connexion intégré à la console d'administration.
			@ingroup m05

			@image html cap_admin_login.png
			@image latex cap_admin_login.png "Maquette de l'écran de connexion intégré à la console d'administration" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Login

			<i>Barre de navigation</i> :
				- Bienvenue sur SYNTHESE Admin

			<i>Zone de contenus</i> : Il s'agit d'un <b>Formulaire de connexion</b>.
				-# <tt>Utilisateur</tt> : Champ texte destiné à recevoir le nom d'utilisateur.
				-# <tt>Mot de passe</tt> : Champ texte caché destiné à recevoir le mot de passe de l'utilisateur.
				-# bouton <tt>Connexion</tt> : Lance la tentative d'identification :
				- Si l'utilisateur a le droit de se connecter (habilitation privée SecurityRight de niveau USE) et si le mot de passe est valide alors l'utilisateur est conduit sur la page d'accueil HomeAdmin.
				- Sinon un message d'erreur "Utilisateur incorrect" est retourné.

			<i>Sécurité</i>
				- Cette page est accessible sans restriction
				- Une habilitation privée SecurityRight de niveau USE est nécessaire pour pouvoir se connecter.

			<i>Journaux</i> : Les opérations suivantes sont consignées dans le journal de sécurité SecurityLog :
				- INFO : Connexion d'utilisateur
				- WARNING : Refus de connexion après cinq tentatives sur une même session

		*/
		class SimpleLoginInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(std::ostream& stream, const void* object = NULL, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_SimpleLoginInterfacePage_H__
 
