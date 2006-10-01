
#ifndef SYNTHESE_UTIL_DBLOG_VIEWER_H
#define SYNTHESE_UTIL_DBLOG_VIEWER_H

namespace synthese
{
	namespace interfaces
	{
		/** Visualisation de journal d'�v�nements.
			@ingroup m01
		
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
				- Les habilitations n�cessaires pour visualiser le journal sont d�finies individuellement par chaque journal (voir classe correspondante)

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne g�n�re d'entr�e dans un journal.
		*/
		class DBLogViewer : public AdminInterfaceElement
		{
		};
	}
}

