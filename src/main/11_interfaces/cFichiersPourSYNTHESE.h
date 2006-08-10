
#ifndef SYNTHESE_CFICHIERSPOURSYNTHESE_H
#define SYNTHESE_CFICHIERSPOURSYNTHESE_H

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

//! @name Format du fichier Sites
//@{
#define SITESFORMAT "sites"
#define SITESEXTENSION "sites.per"
#define SITESFORMATLIGNEEnv 					0
#define SITESFORMATLIGNEInterface 				1
#define SITESFORMATLIGNEIdentifiant 			2
#define SITESFORMATLIGNEDateDebut 				3
#define SITESFORMATLIGNEDateFin 				4
#define SITESFORMATLIGNEResaEnLigne				5
#define SITESFORMATLIGNEURLClient				6
#define SITESFORMATLIGNESolutionsPassees		7
#define SITESFORMATLIGNENombreFormats			8

#define SITESFORMATCOLONNEStandard 				0
#define SITESFORMATCOLONNEClef		 			1
#define SITESFORMATCOLONNEDate		 			2
#define SITESFORMATCOLONNEURLClient				3
#define SITESFORMATCOLONNESolutionsPassees		4
#define SITESFORMATCOLONNEResaEnLigne			5
#define SITESFORMATCOLONNENombreFormats			6
//@}


//! @name Format du fichier tbdep
//@{
#define TBDEPFORMAT "tbdep"
#define TBDEPEXTENSION "tbdep.per"

#define TBDEPFORMATLIGNEEnvironnement			0
#define TBDEPFORMATLIGNEInterface				1
#define TBDEPFORMATLIGNETitre					2
#define TBDEPFORMATLIGNEOriginesSeulement		3
#define TBDEPFORMATLIGNEPointArret				4
#define TBDEPFORMATLIGNEQuai					5
#define TBDEPFORMATLIGNEDestinationAffichee		6
#define TBDEPFORMATLIGNEDestinationSelection	7
#define TBDEPFORMATLIGNENombreDeparts			8
#define TBDEPFORMATLIGNENumeroPanneau			9
#define TBDEPFORMATLIGNENombreFormats			10

#define TBDEPFORMATCOLONNEStandard 				0
#define TBDEPFORMATCOLONNEMethode 				1
#define TBDEPFORMATCOLONNECode					2
#define TBDEPFORMATCOLONNENombreFormats			3
//@}

//! @name Format du fichier serveurs_data
//@{
#define SERVEURSDATAFORMAT "serveursdata"
#define SERVEURSDATAEXTENSION "serveursdata.per"

#define SERVEURSDATALIGNEHost					0
#define SERVEURSDATALIGNELogin					1
#define SERVEURSDATALIGNEPassword				2
#define SERVEURSDATALIGNEBase					3
#define SERVEURSDATALIGNENombreFormats			4

#define SERVEURSDATAFORMATCOLONNEStandard 		0
#define SERVEURSDATAFORMATCOLONNENombreFormats	1
//@}

//! @name Format du fichier bases réservations
//@{
#define DBFORMAT "databases"
#define DBEXTENSION "databases.per"
#define DBFORMATLIGNEServeur						0
#define DBFORMATLIGNETableReservations				1
#define DBFORMATLIGNEChampID						2
#define DBFORMATLIGNEMethodeID						3
#define DBFORMATLIGNEChampNom						4
#define DBFORMATLIGNEChampPrenom					5
#define DBFORMATLIGNEChampAdresse					6
#define DBFORMATLIGNEChampTelephone					7
#define DBFORMATLIGNEChampEMail						8
#define DBFORMATLIGNEChampNumeroAbonne				9
#define DBFORMATLIGNEChampNombrePlaces				10
#define DBFORMATLIGNEChampCodeLigne					11
#define DBFORMATLIGNEChampLibelleLigne				12
#define DBFORMATLIGNEChampCodeModaliteReservation	13
#define DBFORMATLIGNEChampNumeroService				14
#define DBFORMATLIGNEChampArretDepart				15
#define DBFORMATLIGNEChampDesignationDepart			16
#define DBFORMATLIGNEChampArretArrivee				17
#define DBFORMATLIGNEChampDesignationArrivee		18
#define DBFORMATLIGNEChampMomentDepart				19
#define DBFORMATLIGNEChampEMailTransporteur1		20
#define DBFORMATLIGNEChampEMailTransporteur2		21
#define DBFORMATLIGNEChampPalier					22
#define DBFORMATLIGNEChampMomentEnvoiEMail			23
#define DBFORMATLIGNEChampMomentReservation			24
#define DBFORMATLIGNEChampMomentAnnulation			25
#define DBFORMATLIGNEChampEtat						26
#define DBFORMATLIGNEChampRaisonEchec				27
#define DBFORMATLIGNETableCompteurParJour			28
#define DBFORMATLIGNEChampCompteur					29
#define DBFORMATLIGNENombreFormats					30

#define DBFORMATCOLONNEStandard						0
#define DBFORMATCOLONNENombreFormats				1
//@}

#include "cFichier.h"
#include "SYNTHESE.h"
#include "cFichierObjetInterfaceStandard.h"



/*!	\brief Fichier décrivant des sites clients
*/
class cFichierSites : public cFichier
{
public:
	bool Charge();
	
	cFichierSites(const cTexte& CheminFichier, const cTexte& NomFichierFormats)
 		: cFichier(cTexte(CheminFichier).Copie(SITESEXTENSION), new cFormatFichier(NomFichierFormats, SITESFORMAT, SITESFORMATLIGNENombreFormats, SITESFORMATCOLONNENombreFormats))
		{ }
};



/*!	\brief Fichier décrivant des environnements
*/
class cFichierEnvironnements : public cFichier
{
public:
	bool Charge(int __NombreCalculateurs);
	cFichierEnvironnements(const cTexte& NomFichier, const cTexte& NomFichierFormats)
		: cFichier(cTexte(NomFichier).Copie(ENVSEXTENSION), new cFormatFichier(NomFichierFormats, ENVSFORMAT, ENVSFORMATLIGNENombreFormats, ENVSFORMATCOLONNENombreFormats))
		{ _CheminFichierFormats = NomFichierFormats; }
};






/*!	\brief Fichier décrivant des tableaux de départs
*/
class cFichierTbDep : public cFichier
{
public:
	bool	Charge();
	cFichierTbDep(const cTexte& NomFichier, const cTexte& NomFichierFormats)
		: cFichier(cTexte(NomFichier).Copie(TBDEPEXTENSION), new cFormatFichier(NomFichierFormats, TBDEPFORMAT, TBDEPFORMATLIGNENombreFormats, TBDEPFORMATCOLONNENombreFormats))
		{ _CheminFichierFormats = NomFichierFormats; }
};



/*!	\brief Fichier décrivant les serveurs de base de données
*/
class cFichierServeursData : public cFichier
{
public:
	bool	Charge();
	cFichierServeursData(const cTexte& NomFichier, const cTexte& NomFichierFormats)
		: cFichier(cTexte(NomFichier).Copie(SERVEURSDATAEXTENSION), new cFormatFichier(NomFichierFormats, SERVEURSDATAFORMAT, SERVEURSDATALIGNENombreFormats, SERVEURSDATAFORMATCOLONNENombreFormats))
		{ _CheminFichierFormats = NomFichierFormats; }
};



/*!	\brief Fichier décrivant les serveurs de base de données
*/
class cFichierDatabases : public cFichier
{
public:
	bool	Charge();
	cFichierDatabases(const cTexte& NomFichier, const cTexte& NomFichierFormats)
		: cFichier(cTexte(NomFichier).Copie(DBEXTENSION), new cFormatFichier(NomFichierFormats, SERVEURSDATAFORMAT, SERVEURSDATALIGNENombreFormats, SERVEURSDATAFORMATCOLONNENombreFormats))
		{ _CheminFichierFormats = NomFichierFormats; }
};


/** Fichier décrivant des interfaces
*/
class cFichierInterfaces : public cFichier
{
public:
	bool Charge();
	cFichierInterfaces(const cTexte& NomFichier, const cTexte& NomFichierFormats)
		: cFichier(cTexte(NomFichier).Copie(INTERFACESEXTENSION), new cFormatFichier(NomFichierFormats, INTERFACESFORMAT, INTERFACESFORMATLIGNENombreFormats, INTERFACESFORMATCOLONNESNombreFormats))
		{ _CheminFichierFormats = NomFichierFormats; }
};

/** @} */

#endif
