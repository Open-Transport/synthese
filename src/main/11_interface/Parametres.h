/*! \file Parametres.h
	\brief Paramètres de compilation et constantes
	*/

#ifndef SYNTHESE_PARAMETRES_H
#define SYNTHESE_PARAMETRES_H

/** @addtogroup m00
	@{
*/

//! @name Constantes
//@{
#define INCONNU -1
//@}

typedef int tIndex;	//!< Code d'index
typedef enum tBool3	//!< Booleen 3 etats
{
	Faux = 0,
	Vrai = 1,
	Indifferent = INCONNU
};

/** @} */


typedef long int tVitesseKMH;

//! Nombre par défaut d'espaces de calcul alloués par environnement
#define NOMBRE_CALCULATEURS_PAR_ENVIRONNEMENT_DEFAUT 10

//! Séparateur de répertoire
#ifdef WIN32
#define SEPARATEUR_REPERTOIRE_TXT	"\\"
#define SEPARATEUR_REPERTOIRE_CHAR	'\\'
#else
#define SEPARATEUR_REPERTOIRE_TXT "/"
#define SEPARATEUR_REPERTOIRE_CHAR '/'
#endif
	
	
//parametres pour verification de presence des champs
#define CHECK_NOM "checkNom"
#define CHECK_PRENOM "checkPrenom"
#define CHECK_ADRESSE "checkAdresse"
#define CHECK_EMAIL "checkEmail"
#define CHECK_TELEPHONE "checkTelephone"
#define CHECK_NUMERO_ABONNE	"checkNumAbonne" 
#define CHECK_ADRESSE_DEPART "checkAdresseDepart" 
#define CHECK_ADRESSE_ARRIVEE "checkAdresseArrivee" 
#define CHECK_NOMBRE_PLACES "checkNbPlaces" 
									
	
// Macro texte
#define TXT(a) cTexte().Copie((int) a)
#define TXT2(a, b) cTexte().Copie((int) a, b)


// Base de données
#define TABLE_RESERVATION 						"Réservation"
#define TABLE_RESERVATION_NUMERO 				"Numéro"
#define TABLE_RESERVATION_NOM 				"Nom"
#define TABLE_RESERVATION_PRENOM 				"Prénom"
#define TABLE_RESERVATION_ADRESSE				"Adresse"
#define TABLE_RESERVATION_TELEPHONE			"Téléphone"
#define TABLE_RESERVATION_EMAIL				"Email"
#define TABLE_RESERVATION_NUM_ABONNE		 	"Numéro_abonné"
#define TABLE_RESERVATION_NOMBRE_PLACES	 	"Nombre_places"
#define TABLE_RESERVATION_CODE_LIGNE		 	"Code_ligne"
#define TABLE_RESERVATION_LIBELLE_LIGNE	 		"Libellé_ligne"
#define TABLE_RESERVATION_MOD_RESA			"Code_modalité_réservation"
#define TABLE_RESERVATION_NUM_SERVICE		 	"Numéro_service"
#define TABLE_RESERVATION_CODE_PA_DEPART 		"Code_PA_départ"
#define TABLE_RESERVATION_DES_PA_DEPART	 	"Désignation_PA_départ"
#define TABLE_RESERVATION_CODE_PA_ARRIVEE 		"Code_PA_arrivée"
#define TABLE_RESERVATION_DES_PA_ARRIVEE		"Désignation_PA_arrivée"
#define TABLE_RESERVATION_DATE_DEPART	 		"Date_départ"
#define TABLE_RESERVATION_EMAIL_TRANSPORTEUR 	"Email_transporteur"
#define TABLE_RESERVATION_EMAIL_COPIE	 		"Email_copie"
#define TABLE_RESERVATION_PALIER	 			"Palier"
#define TABLE_RESERVATION_DATE_ENVOI			"Date_envoi_mail"
#define TABLE_RESERVATION_DATE_RESA			"Date_réservation"
#define TABLE_RESERVATION_DATE_ANNUL			"Date_annulation"
#define TABLE_RESERVATION_ETAT				"Etat"
#define TABLE_RESERVATION_RAISON_ECHEC		"Raison_echec"

#define TABLE_COMPTEUR						"Compteur"
#define TABLE_COMPTEUR_JOUR					"Jour_réservation"
#define TABLE_COMPTEUR_COMPTEUR				"Compteur"


// Tampon
#define TAILLETAMPON 1024
#define TAILLEGROSTAMPON 50000

// Paramètres
#define LARGEURDEFAUTNOMBRE 6
#define TAILLECODEJC 4
#define NJCMAX 100
#define NOMBREGARESPILE 300
#define NOMBREMAXDEPARTS 1000
#define NOMBREELEMENTTRAJET 500
#define NMAXPROFONDEUR 8
#define NTRAJETSDIRECTSMAX 200
#define NMAXGARESTABLEAU 30
#define NMAXINCLJC 10
#define NGARESPARCOMMUNE 1
#define TAILLENOMBREILLIMITEE 30

// Affichage
#define NOMBREARRETSECRAN 20


// Vitesses max
#define NOMBREVMAX 11
#define DVMAX1 5
#define DVMAX2 10
#define DVMAX3 15
#define DVMAX4 20
#define DVMAX5 25
#define DVMAX6 30
#define DVMAX7 50
#define DVMAX8 100
#define DVMAX9 150
#define DVMAX10 200
#define D2VMAX1 25
#define D2VMAX2 100
#define D2VMAX3 225
#define D2VMAX4 400
#define D2VMAX5 625
#define D2VMAX6 900
#define D2VMAX7 2500
#define D2VMAX8 10000
#define D2VMAX9 22500
#define D2VMAX10 40000
#define VMAXABSOLUE 250


// Optimisation 2
#define O2TRANCHE 4
#define O2COURTLONG 625


// Affichage
#define NMAXGARESFICHEHORAIRE 30


#define NOMBREPHOTOSPAMAX 50


// Heures
#define HEUREMIDIMIN 10
#define HEUREMIDIMAX 16
#define NOMBREMINSOLUTIONS 20







// Langues
typedef unsigned char tLangue;
#define LANGUECode 0
#define LANGUEFrancais 1
#define LANGUEAnglais 2
#define LANGUEEspagnol 3
#define LANGUEDefaut 1

// Prix
typedef long int tPrix; //(en centimes d'euro)

// Contenance
typedef long int tContenance;



#endif
