
/** BroadcastPointAdmin class header.
	@file BroadcastPointAdmin.h

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

#ifndef SYNTHESE_BroadcastPointAdmin_H__
#define SYNTHESE_BroadcastPointAdmin_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}

	namespace departurestable
	{
		/** Ecran d'�dition de points de diffusion de zone d'arr�t.
			@ingroup m34

			@image html cap_admin_place.png
			@image latex cap_admin_place.png "Maquette de l'�cran de recherche de lieux" width=14cm
			
			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Points de diffusion - [Commune] [Nom]

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::BroadcastPointsAdmin
				- Texte [Commune] [Nom]

			<i>Zone de contenu</i> : <b>Formulaire d'�dition</b> :
				-# <b>Emplacements physiques</b>
					-# <tt>Arr�ts physiques</tt> : Les arr�ts physiques sont rappel�s ici pour �viter de cr�er des emplacements existants. Ils ne sont pas modifiables car proviennent des donn�es de d�marrage.
					-# <tt>Emplacements d'affichage</tt> : Tableau correspondant aux emplacements d'affichage non li�s � des arr�ts physiques (ex: salle d'attente). Chaque emplacement est d�sign� par son nom. Un formulaire permet d'�diter directement chaque emplacement :
						-# <tt>Nom</tt> : Champ texte obligatoire.
						-# Bouton <tt>Renommer</tt> : Change le nom de l'emplacement par celui entr�. En cas de champ nom vide, le renommage est abandonn� et un message d'erreur apparait :
							@code L'emplacement ne peut �tre renomm� par un nom vide @endcode
							En outre, le nom d'un emplacement doit �tre unique au sein du lieu logique, arr�ts physiques inclus. En cas de renommage avec un nom existant, il est abandonn� et un message d'erreur apparait :
							@code L'emplacement ne peut �tre renomm� par le nom sp�cifi� car il est d�j� utilis�. @endcode
						-# Bouton <tt>Supprimer</tt> : Supprime l'emplacement, seulement si celui-ci n'est utilis� par aucun afficheur. Le bouton supprimer n'apparait pas dans un tel cas. Lors de la suppression ce crit�re d'int�grit� est � nouveau control�. En cas d'�chec un message d'erreur apparait :
							@code Cet emplacement ne peut �tre supprim� car au moins un terminal d'afficheur 
							y est d�clar�. @endcode
						-# Bouton <tt>Ajouter</tt> : Comportement identique au bouton <tt>Renommer</tt>, mais sur un emplacement nouveau.
			
			<i>S�curit�</i>
				- Une habilitation publique PlacesRight de niveau READ est n�cessaire pour acc�der � la page en consultation.
				- Une habilitation publique PlacesRight de niveau WRITE sur le p�rim�tre du lieu est n�cessaire pour pouvoir renommer des emplacements d'affichage et pour pouvoir en cr�er
				- Une habilitation publique PlacesRight de niveau DELETE sur le p�rim�tre du lieu est n�cessaire pour pouvoir supprimer un emplacement d'affichage

			<i>Journaux</i> : Les �v�nements suivants entrainent la cr�ation d'une entr�e dans le journal de l'administration des donn�es r�seau de l'environnement TransportNetworkDataLog :
				- INFO : Renommage d'emplacement
				- INFO : Suppression d'emplacement
				- INFO : Cr�ation d'emplacement

		*/
		class BroadcastPointAdmin : public admin::AdminInterfaceElement
		{
			env::ConnectionPlace* _place;

		public:
			BroadcastPointAdmin();
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::Request::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(std::ostream& stream, const server::Request* request=NULL) const;

			/** Title of the admin compound.
				@return The title of the admin compound, for display purposes.
			*/
			std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_BroadcastPointAdmin_H__
