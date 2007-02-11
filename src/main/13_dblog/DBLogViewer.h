
/** DBLogViewer class header.
	@file DBLogViewer.h

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

#ifndef SYNTHESE_DBLogViewer_H__
#define SYNTHESE_DBLogViewer_H__

#include <string>

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace dblog
	{
		class DBLog;

		/** Visualisation de journal d'�v�nements.
			@ingroup m13
		
			@image html cap_admin_logviewer.png
			@image latex cap_admin_logviewer.png "Maquette de l'�cran de visualisation de journal" width=14cm

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminInterfaceElement
				- Lien vers synthese::interfaces::DBLogList
				- Nom du journal affich�

			<i>Zone de contenus</i> :
				-# <b>Formulaire de recherche</b> :
					-# <tt>Date d�but</tt> : champ texte qui permet de filtrer les entr�es post�rieures � la date saisie. Si aucune date valide n'est entr�e, alors le filtre est d�sactiv�.
					-# <tt>Date fin</tt> : champ texte qui permet de filtrer les entr�es ant�rieures � la date saisie. Si aucune date valide n'est entr�e, alors le filtre est d�sactiv�.
					-# <tt>Utilisateur</tt> : liste de choix contenant l'ensemble des utilisateurs ayant g�n�r� au moins une entr�e sur le journal affich�. 
						- si un utilisateur est s�lectionn�, alors seules les entr�es g�n�r�es par l'utilisateurs s�lectionn� sont affich�es
						- si aucun utilisateur n'est s�lectionn�, alors le filtre est d�sactiv�.
					-# <tt>Type</tt> : liste de choix permettant de filtrer sur le type d'entr�e :
						- <tt>Toutes</tt> : filtre d�sactive
						- <tt>Info</tt>
						- <tt>Warning</tt>
						- <tt>Warning + Error</tt>
						- <tt>Error</tt>
					-# <tt>Texte</tt> : champ texte permettant de chercher dans le contenu des autres colonnes (regroup�e sous le nom de <tt>Entr�e</tt>).
					-# Bouton <tt>Rechercher</tt> : lance la recherche
				-# <b>Tableau des r�sultats</b> : Les entr�es du journal sont affich�es dans les colonnes suivantes :
					-# <tt>Type</tt> : la couleur de l'entr�e est li�e au type pour une visualisation rapide
						- <tt>Info</tt> : l'entr�e correspont � un fait qui est survenu dans le cadre d'une utilisation normale des fonctionnalit�s de SYNTHESE (Exemple : cr�ation d'utilisateur)
						- <tt>Warning</tt> : l'entr�e correspond � un �v�nement susceptible de constituer une anomalie sans cons�quence majeure sur la continuit� de service en dehors de l'anomalie constat�e elle-m�me (Exemple : erreur de coh�rence sur un tableau d'affichage : l'arr�t physique de s�lection des d�parts n'existe plus)
						- <tt>Error</tt> : l'entr�e correspond � un �v�nement constituant une anomalie (Exemple : un terminal d'affichage ne fonctionne plus)
					-# <tt>Date</tt> : Date de l'entr�e
					-# <tt>Utilisateur</tt> : Utilisateur ayant g�n�r� l'entr�e. Attention, ne pas confondre cet utilisateur avec un �ventuel utilisateur sur lequel porterait l'entr�e (Exemple : utilisateur Paul cr�� par Jacques : l'utilisateur ayant g�n�r� l'entr�e est Jacques). Un clic sur le nom de l'utilisateur conduit vers la page synthese::interfaces::UserAdmin si l'utilisateur dispose des habilitations n�cessaires.
					-# <tt>Entr�e</tt> : Ensemble de colonnes d�finissant le contenu de l'entr�e. La liste des colonnes d�pend du journal affich� (voir documentation individuelle des classes d�riv�es)
					-# Un clic sur les titres de colonne effectue un tri selon la colonne s�lectionn�e
				-# Un maximum de 50 entr�es est affich� � l'�cran. En cas de d�passement de ce nombre d'apr�s les crit�res de recherche, un lien <tt>Entr�es suivantes</tt> apparait et permet de visualiser les entr�es suivantes. A partir de la seconde page, un lien <tt>Entr�es pr�c�dentes</tt> apparait �galement.

			<i>S�curit�</i>
				- Une habilitation priv�e DBLogRight de niveau READ, ainsi qu'un contr�le positif des habilitations par le journal visualis� sont n�cessaire pour acc�der � la page, et y visualiser les entr�es g�n�r�es par l'utilisateur courant.
				- Une habilitation publique DBLogRight de niveau READ, ainsi qu'un contr�le positif des habilitations par le journal visualis� sont n�cessaire pour acc�der � la page, et y visualiser toutes les entr�es.

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne g�n�re d'entr�e dans un journal.
		*/
		class DBLogViewer : public admin::AdminInterfaceElement
		{
			//! \name Stored parameters
			//@{
				std::string			_logKey;
				uid					_searchUser;
				int					_searchType;
			//@}

			//! \name Stored values
			//@{
			const DBLog*	_dbLog;
			//@}

		public:
			static const std::string PARAMETER_LOG_KEY;
			static const std::string PARAMETER_SEARCH_USER;
			static const std::string PARAMETER_SEARCH_TYPE;

			DBLogViewer();
			
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

#endif // SYNTHESE_DBLogViewer_H__
