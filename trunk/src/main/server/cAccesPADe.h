/** En-tête classe désignation de lieu (arrêt/adresse/lieu public)
	@file cAccesPADe.h
	@author Hugues Romain
	@date 2000-2006
*/

#ifndef SYNTHESE_CACCESPADE_H
#define SYNTHESE_CACCESPADE_H

class cAccesPADe;


/** Type de lieu pointé par un élément de classe @ref cAccesPADe .

	Voir cAccesPADe
*/
enum tTypeAccesPADe
{
	eQuelconque 		= 0,	//<! Arrêt standard
	ePrincipale 		= 1,	//<! Arrêt principal de la commune
	eToutLieu   		= 2,	//<! Arrêt virtuel tout lieu de la commune
	eLieuPublic 		= 3,	//<! Lieu public (adresse précise)
	eLieuQuelconque 	= 4,	//<! Adresse large (
	ePrincipaleDefaut	= 5
};

#include "cTexte.h"
#include "cCommune.h"
#include "cArretLogique.h"
#include "cLigne.h"
#include "Temps.h"



/** Désignation de lieu (adresse/arrêt/lieu public)
	@ingroup m05
	@author Hugues Romain
	@date 2001-2006
	
Les objets de cette classe correspondent � des lieux r�els ou virtuels, associ�s � une description de l'acc�s �/aux point(s) d'arr�t le(s) plus proche(s).

Les objets de cette classe peuvent �tre issus de plusieurs origines, d�crites par la variable vTypeAcces :
	- <b>D�signations de point d'arr�t</b>
		- <b>D�signation officielle de point d'arr�t</b> (Type "eQuelconque"/ vIndex = 0) : Le lieu d�sign� est un point d'arr�t. La commune li�e est la commune o� se trouve r�ellement le point d'arr�t, et le nom pr�sent dans l'objet est la d�nomination officielle du point d'arr�t (au niveau arr�t logique). (Ex : TOULOUSE / Matabiau)
		- <b>D�signation secondaire de point d'arr�t</b> (Type "eQuelconque"/ vIndex > 0) : Le lieu d�sign� est un point d'arr�t. La commune li�e est la commune o� le point d'arr�t sera localis� au titre de la d�signation, par exemple pour aider un client � le trouver dans certains cas de confusion classique, et le nom pr�sent dans l'objet est une d�nomination du point d'arr�t accept�e au niveau d'une recherche de l'arr�t. (Ex : LA MONGIE / Station : la commune de La Mongie n'existe pas mais est souvent d�sign�e par les clients)
		- L'ensemble des d�signations de point d'arr�t d'une commune (qu'elles soient officielles ou secondaires) sont accessibles depuis la commune par le tableau de pointeurs cCommune::_AccesPADe, tri�es dans l'ordre alphab�tique des noms, avec le premier index r�serv� � la d�signation principale de la commune (voir plus bas) Voir cCommune::_AccesPADe.
		- L'ensemble des d�signations d'un point d'arr�t (quelle que soit la commune) sont accessibles depuis le point d'arr�t par le tableau de pointeurs cArretLogique::_AccesPADe, avec le premier index r�serv� � la d�signation officielle de l'arr�t. Voir cArretLogique::_AccesPADe.
  
  \dot digraph P
	{
		node [shape=plaintext fontsize=8 fontname="myriad pro" overlap=false]
		PA [URL="\ref cArretLogique" label=<
		<TABLE CELLSPACING="0">
			<TR><TD COLSPAN="2">Point d'arret<BR />(ex : TOULOUSE Matabiau)<BR />cArretLogique::_AccesPADe</TD></TR>
			<TR><TD>0</TD><TD PORT="PA0">Designation officielle</TD></TR>
			<TR><TD>1</TD><TD PORT="PA1">Designation secondaire</TD></TR>
			<TR><TD>2</TD><TD PORT="PA2">Designation secondaire</TD></TR>
			<TR><TD>...</TD><TD>...</TD></TR>
			<TR><TD>n</TD><TD>NULL</TD></TR>
		</TABLE>>]
		
		Commune [URL="\ref cCommune" label=<
		<TABLE CELLSPACING="0">
			<TR><TD COLSPAN="2">Commune d'implantation du point d'arret<BR /> (ex : TOULOUSE)<BR />cCommune::_AccesPADe</TD></TR>
			<TR><TD>...</TD><TD>...</TD></TR>
			<TR><TD PORT="C10">x</TD><TD>Nom officiel du point d'arret (ex : Matabiau)</TD></TR>
			<TR><TD>...</TD><TD>...</TD></TR>
			<TR><TD PORT="C11">y</TD><TD>Nom secondaire du point d'arret (ex : Marengo SNCF)</TD></TR>
			<TR><TD>...</TD><TD>...</TD></TR>
		</TABLE>>]
		
		Commune2 [URL="\ref cCommune" label=<
		<TABLE CELLSPACING="0">
			<TR><TD COLSPAN="2">Autre commune sur laquelle doit etre lie le point d'arret<BR />(ex : BLAGNAC)<BR />cCommune::_AccesPADe</TD></TR>
			<TR><TD>...</TD><TD>...</TD></TR>
			<TR><TD PORT="C21">z</TD><TD>Nom secondaire du point d'arret<BR />(ex : BLAGNAC Gare = TOULOUSE Matabiau)</TD></TR>
			<TR><TD>...</TD><TD>...</TD></TR>
		</TABLE>>]
		
		PA:PA0 -> Commune:C10
		PA:PA1 -> Commune:C11
		PA:PA2 -> Commune2:C21
    	} \enddot
	- <b>D�signations relatives � une commune</b>
		- <b>D�signation de s�lection d'arr�ts principaux de commune</b> (Type "ePrincipal"/ vIndex = INCONNU) : Le lieu d�sign� est une commune, au sens o� le point de mont�e dans la commune n'est pas connu, et aucune information ne permet de le conna�tre. La d�signation indiquera alors une liste de points d'arr�ts jug�s suffisemment faciles � identifier sur site (gare, arr�t central, etc.), et permettant le plus grand nombre de dessertes (point de correspondance...). Chaque arr�t de la liste sera jug� �quivalent (vDuree = 0). Cette d�signation ne comporte pas de nom. Cette d�signation est disponible au niveau de la commune � l'index 0 du tableau _AccesPADe. par la m�thode cCommune::GetPADePrincipale().
		- <b>D�signation de desserte int�grale de commune</b> (Type "eToutLieu" / vIndex = 0) : Le lieu d�sign� est une zone de desserte �quivalente, couvrant la commune enti�re, utilis�e notamment pour le TAD zonal. Elle ne contient qu'un �l�m�nt : un arr�t virtuel nomm� "Tout lieu". Si cette d�signation existe dans une commune, alors toute autre d�signation de la commune termine la liste d'arr�ts par un pointeur vSuivant vers cette d�signation. La dur�e d'acc�s au lieu r�el est alors consid�r�e nulle (vDuree = 0). Lorsque une telle d�signation intervient et qu'une description pr�cise du lieu est n�cessaire (rendez-vous de TAD par exemple), alors un compl�ment d'adresse doit �tre fourni. Si cette d�signation est obtenue par parcours de la liste des arr�ts d�sign�s, et que le lieu d�sign� � la racine de la liste est pr�cis, alors l'adresse n'est pas n�cessaire : le nom de la d�signation de l'arr�t racine pourra �tre utilis�.
	- <b>D�signations relatives � un lieu quelconque</b>
		- <b>D�signation de lieu public connu</b> (Type "eLieuPublic" / vIndex quelconque) : Le lieu d�sign� est un lieu public se trouvant � proximit� d'un ou plusieurs arr�ts. Chaque point d'arr�t fait l'objet d'un �l�ment cha�n� � la liste (pointeur vSuivant pour parcourir). Les d�signations contiennent la dur�e d'acc�s � pied entre le lieu et l'arr�t reli�. La liste des arr�ts reli�s est class�e du plus proche au plus �loign� (exception d'un arr�t tout lieu ne pouvant �tre reli� qu'en dernier). La d�signation est index�e dans le tableau des d�signations de la commune au m�me titre qu'une d�signation d'arr�t.
		- <b>D�signation de lieu quelconque</b> (Type eLieuQuelconque" / vIndex=0) : Le concept est identique aux lieux publics � la diff�rence que cette d�signation est un objet temporaire issu du calcul provenant d'un SIG. Elle ne comporte pas de nom.
 */
class cAccesPADe
{
private:

	//! \name Donn�es
	//@{
	cTexte			vNomArretLogique;	//!< Nom du point d'arr�t dans la commune
	cArretLogique*			vArretLogique;	//!< Point d'arr�t d�sign�
	cCommune*		vCommune;		//!< Commune dans laquelle le point d'arr�t est d�sign�
	cDureeEnMinutes	vDuree;			//!< Dur�e � compter entre l'arr�t d�sign� et l'arr�t racine de la d�signation
	//@}

	//! \name Informations pour utilisation comme classe de stockage
	//@{
	tTypeAccesPADe 	vTypeAcces;		//!< Type de d�signation dans la commune
	tIndex			_Index; 		//!< Index de la d�signation au sein de l'arr�t
	//@}
	
	//! \name Informations de cha�nage si plusieurs arr�ts
	//@{
	cAccesPADe*		vSuivant;		//!< Point d'arr�t suivant d�sign� par la d�signation
	//@}
	
	//! \name Autres informations
	//@{
	bool			_TousArretsDeCorrespondance;	//!< Type des arr�ts
	//@}
	
public:
	//! \name Calculateurs
	//@{ 
	bool		inclue(const cArretLogique*)																	const;
	cMoment		momentArriveePrecedente(const cMoment& MomentArrivee, const cMoment& MomentArriveeMin)	const;
	cMoment		momentDepartSuivant(const cMoment& MomentDepart, const cMoment& MomentDepartMax
									, const cMoment& __MomentCalcul)									const;
	//@}
	
	//! \name Accesseurs
	//@{
	bool			TousArretsDeCorrespondance()	const;
	cCommune*		getCommune()					const;
	cCommune*		getCommuneReelle()				const;
	const cTexte&	getNom()						const;
	cArretLogique*			getArretLogique()					const;
	cGareLigne*		getPremiereGareLigneDep()		const;
	cGareLigne*		getPremiereGareLigneArr()		const;
	cAccesPADe*		getSuivant()					const;
	tTypeAccesPADe 	getTypeAcces() 					const;
	tIndex			numeroDesignation()				const;
	tIndex			numeroArretLogique()				const;
	//@}
	
	//! \name Modificateurs
	//@{
	bool Chaine(cAccesPADe*);
	void setSuivant(cAccesPADe* newAccesPADe);
	bool setNumeroDesignation(tIndex);
	bool setTypeAcces(tTypeAccesPADe);
	//@}

	//! \name Constructeur
	//@{
	cAccesPADe(cArretLogique*, cCommune*, const cTexte&);
	cAccesPADe(cAccesPADe*);
	//@}
};

#define SYNTHESE_CACCESPADE_CLASSE

#include "cAccesPADe.inline.h"
#include "cCommuneAccesPADe.inline.h"
#include "cArretLogiqueAccesPADe.inline.h"
#include "cGareLigneAccesPADe.inline.h"

#endif
