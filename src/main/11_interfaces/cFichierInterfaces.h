
#ifndef SYNTHESE_CFICHIERINTERFACES_H
#define SYNTHESE_CFICHIERINTERFACES_H


#include "cFichier.h"
#include "Interface.h"



// #include "cFichierObjetInterfaceStandard.h"


/**	@addtogroup m11
	@{ */

//!	\name Fichier des interfaces (cFichierInterfaces)
//@{
#define INTERFACESFORMAT "interfaces"
#define INTERFACESEXTENSION "interfaces.per"


/** Déclaration d'une interface
	@code [code @endcode
	@param code Code de l'interface (voir @ref base)
*/
#define INTERFACESFORMATLIGNEInterface			TYPESousSection


/** Déclaration d'une description d'un élément
	@code E numero chemin @endcode
	@param numero Numéro de l’élément décrit (trois chiffres). Voir @ref interface pour le codage
	@param chemin chemin est au format habituel : relatif à l'emplacement du fichier courant, et sans extension.
*/
#define INTERFACESFORMATLIGNEElement			0


/** Fixation de la valeur d'un paramètre
	@code P numero valeur @endcode
	@param numero Numéro du paramètre fixé (3 chiffres)
	@param valeur Valeur donnée au paramètre
	@deprecated Vérifier l'inutilité de cette commande et la supprimer
*/
#define INTERFACESFORMATLIGNEParametre			1

/** Déclaration d'un période de la journée
	@code PERIODE heure_debut heure_fin nom @endcode
	@param heure_debut Heure de début de la période (4 chiffres format HHMM)
	@param heure_fin Heure de fin de la période (4 chiffres format HHMM) : peut être inférieure à l'heure de début, la période finira alors le lendemain à l'heure indiquée
	@param nom Nom de la période
*/
#define INTERFACESFORMATLIGNEPeriode			2


/** Traduction d'un numéro de jour en mot
	@code JOUR code mot @endcode
	@param code Code du jour (1 chiffre) :
		- @c 0 : dimanche
		- @c 1 : lundi
		- @c 2 : mardi
		- ...
		- @c 6 : samedi
	@param mot Mot dans la langue de l'interface traduisant le jour
*/
#define INTERFACESFORMATLIGNELibelleJour		3


/** Traduction d'un numéro de mois en mot
	@code MOIS code mot @endcode
	@param code Code du mois (1 chiffre) :
		- @c 1 : janvier
		- @c 2 : février
		- ...
		- @c 12 : décembre
	@param mot Mot dans la langue de l'interface traduisant le mois
*/
#define INTERFACESFORMATLIGNELibelleMois		4


/** Traduction d'un numéro d'alerte en mot
	@code ALERTE niveau mmot @endcode
	@param niveau Niveau d'alerte :
		- @c 99 : attention
		- @c 50 : information
	@param mot Mot dans la langue de l'interface traduisant le niveau d'alerte
*/
#define INTERFACESFORMATLIGNEPrefixeAlerte		5
#define INTERFACESFORMATLIGNENombreFormats		6

#define INTERFACESFORMATCOLONNENumero				0
#define INTERFACESFORMATCOLONNEChemin				1
#define INTERFACESFORMATCOLONNEPeriodeDebut			2
#define INTERFACESFORMATCOLONNEPeriodeFin			3
#define INTERFACESFORMATCOLONNEPeriodeLibelle		4
#define INTERFACESFORMATCOLONNELibelleJourIndex		5
#define INTERFACESFORMATCOLONNELibelleJourLibelle	6
#define INTERFACESFORMATCOLONNELibelleMoisIndex		7
#define INTERFACESFORMATCOLONNELibelleMoisLibelle	8
#define INTERFACESFORMATCOLONNEPrefixeAlerteNiveau	9
#define INTERFACESFORMATCOLONNEPrefixeAlerteTexte	10
#define INTERFACESFORMATCOLONNESNombreFormats		11
//@}





/** Fichier décrivant des interfaces
*/
class cFichierInterfaces : public cFichier
{
public:
	cFichierInterfaces(const cTexte& NomFichier, const cTexte& NomFichierFormats)
		: cFichier(cTexte(NomFichier).Copie(INTERFACESEXTENSION), 
			   new cFormatFichier(NomFichierFormats, 
					      INTERFACESFORMAT, 
					      INTERFACESFORMATLIGNENombreFormats, 
					      INTERFACESFORMATCOLONNESNombreFormats))
	    { 
		_CheminFichierFormats = NomFichierFormats; 
	    }

	bool Charge(synthese::interfaces::Interface::Registry& interfaces);


};

/** @} */

#endif
