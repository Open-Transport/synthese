
#ifndef SYNTHESE_DISPLAY_ADMIN_H
#define SYNTHESE_DISPLAY_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran de param�trage d'un afficheur.
			@ingroup m34

			@image html cap_admin_display.png
			@image latex cap_admin_display.png "Maquette de l'�cran de param�trage d'un afficheur" width=14cm

			<i>Titre de la fen�tre</i> : SYNTHESE Admin - Afficheurs - [Lieu logique] - [Lieu physique] - [Compl�ment de pr�cision]

			<i>Barre de navigation</i> :
 				- Lien vers @ref synthese::interface::HomeAdmin
				- Lien vers @ref synthese::interface::DisplaySearchAdmin
				- Nom du lieu logique auquel appartient l'afficheur, lien vers @ref synthese::interface::DisplayLocationsAdmin sur le lieu logique de l'afficheur
				- Nom du lieu physique auquel appartient l'afficheur
				- Compl�ment de pr�cision sur l'emplacement de l'afficheur (non affich� si le compl�ment est vide)

			<i>Zone de contenus</i> : Un formulaire de modification constitu� par les �l�ments suivants :
				-# <b>Emplacement</b>
					-# <tt>Lieu logique</tt> : Liste d�roulante constitu�e par l'ensemble des arr�ts logiques tri�s par ordre alphab�tique de commune puis par ordre alphab�rique de nom. Champ obligatoire. En cas de changement de lieu logique, la page est rafraichie pour mettre � jour la liste des lieux physiques.
					-# <tt>Lieu physique</tt> : Liste d�roulante constitu�e par l'ensemble des lieux physiques pr�sents dans le lieu logique s�lectionn� (arr�ts physiques + autres lieux d'affichages g�r�s par la fonction @ref synthese::interfaces::DisplayLocationsAdmin. Ce champ est obligatoire.
					-# <tt>Compl�ment de pr�cision</tt> : Champ texte libre permettant de pr�ciser l'emplacement de l'afficheur, utile s'il se trouve dans un lieu physique d�j� �quip� d'un autre afficheur. Ce champ est obligatoire si au moins deux afficheurs sont pr�sents dans le m�me lieu physique. Il est facultatif sinon.
				-# <b>Donn�es techniques</b>
					-# <tt>Type d'afficheur</tt> : Liste d�roulante constitu�e par l'ensemble des types d'afficheur disponibles (issus de l'�cran @ref synthese::interface::DisplayTypesAdmin). Ce champ est obligatoire.
					-# <tt>Code de branchement</tt> : Liste d�roulante consitu�e par les nombres de 0 � 128. Ce champ correspond au code � envoyer par l'interface pour s�lectionner l'afficheur destinataire parmi un groupe branch� sur le m�me port RS485. Ce champ est obligatoire si l'afficheur est de type Oscar. Si l'afficheur n'est pas de type Oscar, la valeur donn�es � ce champ est ignor�e.
					-# <tt>UID</tt> : ID unique g�n�r� par le syst�me pour d�signer l'afficheur. L'UID est fourni � titre informatif mais ne peut �tre modifi� par l'utilisateur.
				-# <b>Apparence</b>
					-# <tt>Titre</tt> : Titre � afficher en haut de l'afficheur. Champ facultatif. Champ inop�rant pour l'interface Lumiplan (ce param�tre peut �tre inop�rant si le champ n'est pas int�gr� � l'interface).
					-# <tt>Clignotement</tt> : Liste d�roulante permettant de choisir la r�gle de clignotement de l'heure de d�part (ce param�tre peut �tre d�sactiv� par l'interface) :
						- Pas de clignotement
						- 1 minute avant l'effacement
						- 2 minutes avant l'effacement
						- 3 minutes avant l'effacement
					-# <tt>Affichage num�ro de quai</tt> : Choix OUI/NON indiquant si le num�ro de quai de d�part doit �tre affich� (ce param�tre peut �tre inop�rant si le champ n'est pas int�gr� � l'interface)
					-# <tt>Affichage num�ro de service</tt> : Choix OUI/NON indiquant si le num�ro de service doit �tre affich� (ce param�tre peut �tre inop�rant si le champ n'est pas int�gr� � l'interface)
				-# <b>Contenu</b>
					-# <tt>Horaires</tt> : Liste d�roulante permettant de choisir entre l'affichage du tableau de d�parts ou des arriv�es
					-# <tt>S�lection terminus</tt> : Liste d�roulante permettant de choisir entre :
						- l'affichage de tous les passages au droit des arr�ts s�lectionn�s
						- l'affichage des seuls services ayant pour origine ou terminus les arr�ts s�lectionn�s
					-# <tt>D�lai maximum d'affichage</tt> : Champ num�rique pr�cisant le d�lai maximal entre l'heure courante et l'heure de d�part pour qu'un service soit affich�. La dur�e fournie est en minutes. Une valeur 0 d�sactive l'affichage. Une valeur non renseign�e implique l'affichage jusqu'� remplissage de l'�cran, quel que soit le d�lai entre l'heure courante et l'heure de d�part. Attention : il est d�conseill� de mettre un d�lai sup�rieur � 24 heures, car dans le cas contraire on ne peut alors pas diff�rencier un d�part du jour d'un d�part le lendemain, le jour de d�part n'�tant pas affich�.
					-# <tt>D�lai d'effacement</tt> : Liste d�roulante permettant de choisir le moment o� le d�part n'est plus affich� :
						- 3 minutes avant le d�part
						- 2 minutes avant le d�part
						- 1 minute avant le d�part
						- � l'heure exacte du d�part
						- 1 minutes apr�s le d�part
						- 2 minutes apr�s le d�part
						- 3 minutes apr�s le d�part
					-# <tt>S�lection sur arr�t physique</tt> : Liste pr�sentant l'ensemble des arr�ts physiques de l'arr�t logique. Cocher l'un d'entre eux entra�ne la s�lection des d�parts se produisant � l'arr�t physique. Plusieurs arr�ts physiques peuvent �tre s�lectionn�s. Une case suppl�mentaire est propos�e en fin de liste : <tt>Tous (y compris nouveaux)</tt>. Ce choix consiste � afficher l'ensemble des d�parts de l'arr�t logique. La diff�rence entre ce choix et cocher tous les arr�ts physiques r�side dans le fait qu'en cas de cr�ation d'un nouvel arr�t physique, il est int�gr� automatiquement � l'affichage si la case Tous est coch�e, et ne l'est pas sinon.
					-# <tt>S�lection sur arr�t logique � ne pas desservir</tt> : Liste pr�sentant l'ensemble des arr�ts logiques qui ne doivent pas faire partie des destinations d'une ligne pour qu'elle soit affich�e :
						- Un bouton <tt>Supprimer</tt> est pr�sent au niveau de chaque arr�t logique permettant de supprimer un arr�t � ne pas desservir
						- Une liste d�roulante munie d'un bouton <tt>Ajouter</tt> permet d'ajouter un arr�t logique � ne pas desservir. Le contenu de la liste d�roulante est l'ensemble des arr�ts logiques desservis par les lignes partant de l'emplacement de l'afficheur.
				-# <b>Pr�s�lection</b>
					-# <tt>Activer</tt> : Choix OUI/NON permettant de mettre en service la r�gle de pr�s�lection de d�parts s'ils desservent une destination majeure non desservie par les prochains d�parts selon l'ordre chronologique. En cas d'activation de cette r�gle, les derniers d�parts de l'ordre chronologique sont remplac�s par les premiers d�parts permettant de se rendre � l'une des destinations faisant l'objet de la pr�selection.
					-# <tt>D�lai maximum pr�s�lection</tt> : Champ num�rique indiquant le d�lai maximum entre l'heure courante et l'heure de d�part qu'un service potentiellement pr�selectionn� doit respect� pour �tre affich�. La valeur fournie est en minutes. Une valeur 0 d�sactive la pr�s�lection. Une valeur non fournie indique que la pr�s�lection s'op�re sans limite de dur�e.
					-# <tt>Arr�ts de desserte interm�diaire suppl�mentaires</tt> : Liste pr�sentant l'ensemble des arr�t logiques faisant l'objet d'une pr�s�lection qui s'ajoutent aux terminus des diff�rents parcours types des lignes affich�es. Il est inutile d'ajouter ici un terminus de ligne : il fait obligatoirement partie de la pr�s�lection.
						- Un bouton <tt>Supprimer</tt> est pr�sent au niveau de chaque arr�t logique permettant de supprimer un arr�t � pr�s�lectionner
						- Une liste d�roulante munie d'un bouton <tt>Ajouter</tt> permet d'ajouter un arr�t logique � pr�s�lectionner. Le contenu de la liste d�roulante est l'ensemble des arr�ts logiques desservis par les lignes partant de l'emplacement de l'afficheur.
						
			<i>S�curit�</i> :
				- Une habilitation publique ArrivalDepartureTableRight de niveau READ est n�cessaire pour afficher la page en mode consultation. L'emplacement de l'afficheur doit entrer dans le p�rim�tre de l'habilitation.
				- Une habilitation publique ArrivalDepartureTableRight de niveau WRITE est n�cessaire pour afficher la page en mode modification. L'emplacement de l'afficheur doit entrer dans le p�rim�tre de l'habilitation. Le contenu des diverses listes d�roulantes est impact� par le p�rim�tre de l'habilitation.

			<i>Journal</i> : Les actions suivantes g�n�rent une entr�e dans le journal du t�l�affichage ArrivalDepartureTableLog :
				- INFO : Modification d'un afficheur
		*/
		class DisplayAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif