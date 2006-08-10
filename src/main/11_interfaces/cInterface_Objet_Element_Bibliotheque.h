
#ifndef SYNTHESE_CINTERFACE_OBJET_ELEMENT_BIBLIOTHEQUE_H
#define SYNTHESE_CINTERFACE_OBJET_ELEMENT_BIBLIOTHEQUE_H

#include "70_server/Request.h"
#include "cInterface_Objet_Element.h"
#include "cInterface_Objet_AEvaluer_ListeParametres.h"


/*! \addtogroup m11
 @{
*/

//! \name Liste des objets d'interface disponibles dans la biblioth�que (Commande O)
//@{


/** Nom de lieu / nom d'arr�t
 @code O03 @endcode
 @param 0 Num�ro de la d�signation
 @param 1 Num�ro de l'arr�t
 @return Affichage de la d�signation sp�cidi�es
*/
#define ELEMENTINTERFACELieuArret 3


/** Fiche horaire
 @code O04 @endcode
 @param Objet cTrajets * : Liste des trajets
 @return Fiche horaire repr�sentant la liste des trajets fournie
*/
#define ELEMENTINTERFACEFicheHoraire       4


/** Mini tableau de d�parts
 @code O05 @endcode
 @param Objet cDescriptionPassage * : Liste cha�n�e de d�parts ou d�arriv�es � afficher
 @return Mini tableau de d�parts
 @deprecated
*/
#define ELEMENTINTERFACEMiniTbDepGare        5


/** Liste des lignes utilis�es pour chaque trajet d'une liste
 @code O06 @endcode
 @param Objet cTrajets *  : Liste des trajets
 @return Liste des lignes utilis�es pour chaque trajet d'une liste
*/
#define EI_OBJETDYNAMIQUE_ListeLignesTrajets     6


/** Liste des lignes utilis�es sur un trajet
 @code O07 @endcode
 @param Objet cTrajet * : Trajet � d�crire
 @param 0 Affichage des lignes � pied
 @param 1 HTML en d�but de ligne (conseill� tr)
 @param 2 HTML en fin de ligne (conseill� /tr)
 @param 3 Largeur en pixels de la case de lignes
 @param 4 Hauteur en pixels de la case de lignes
 @return Liste des lignes utilis�es sur le trajet
*/
#define EI_OBJETDYNAMIQUE_ListeLignesTrajet      7


/** Nom de p�riode de la journ�e
 @code O09 @endcode
 @param 0 Num�ro de la p�riode � afficher
 @return Nom de la p�riode d�sign�e
*/
#define EI_BIBLIOTHEQUE_PERIODE         9


/** Liens vers photos de quais
 @code O12 @endcode
 @param Objet LogicalPlace * Point d�arr�t
 @param 0 Texte du d�but du lien
 @param 1 Texte du lien si pas de nom de quai
 @param 2 Texte de fin du lien
 @return Liens vers photos de quais
 @deprecated A remettre aux normes
*/
#define ELEMENTINTERFACELienPhotosArretPhysiques 12


/** Liens vers autres photos
 @code O13 @endcode
 @param Objet LogicalPlace * : Point d'arr�t
 @param 0 Texte du d�but du lien
 @param 1 Texte de fin du lien
 @param 2 Texte d�ouverture de la zone
 @param 3 Texte de fermeture de la zone
 @return Liens vers autres photos
 @deprecated A remettre aux normes
*/
#define ELEMENTINTERFACELienAutresPhotos      13


/** Liens et description des services
 @code O14 @endcode
 @param Objet LogicalPlace * : Point d�arr�t
 @param 0 Texte du d�but du lien
 @param 1 Texte de fin du lien
 @param 2 Texte d�ouverture de la zone
 @param 3 Texte de fermeture de la zone
 @return Liens et description des services
 @deprecated A remettre aux normes
*/
#define ELEMENTINTERFACELienServices       14


/** Div de quais
 @code O17 @endcode
 @param Objet LogicalPlace * : Point d'arr�t
 @return Div de quais
 @deprecated A remettre aux normes
*/
#define ELEMENTINTERFACEDivArretPhysiques 17


/** Div de photos
 @code O18 @endcode
 @param Objet LogicalPlace * : Point d'arr�t
 @return Div de photos
 @deprecated A remettre aux normes
*/
#define ELEMENTINTERFACEDivPhotos 18


/** Div de services
 @code O19 @endcode
 @param Objet LogicalPlace * : Point d'arr�t
 @return Div de services
 @deprecated A remettre aux normes
*/
#define ELEMENTINTERFACEDivServices 19


/** Scripts de fiche arr�t
 @code O20 @endcode
 @param Objet LogicalPlace * : Point d'arr�t
 @return Scripts de fiche arr�t
 @deprecated A remettre aux normes
*/
#define ELEMENTINTERFACEFicheArretScript 20


/** Date
 @code O22 @endcode
 @param 0 Date � afficher
 @return Affichage de la date
*/
#define EI_BIBLIOTHEQUE_Date         22


/** Liste de communes
 @code O23 @endcode
 @param 0 Message d�erreur si aucune commune trouv�e
 @param 1 Texte d�ouverture, plac� avant le lien
 @param 2 Texte de fermeture, plac� apr�s le lien
 @param 3 Nombre de communes r�pondues
 @param 4 Texte entr�
 @param 5 Texte � �crire si ligne vide
 @return Liste de communes
*/
#define ELEMENTINTERFACEListeCommunes       23


/** Liste d'arr�ts
 @code 024 @endcode
 @param 0 Message d�erreur si aucun arr�t trouv�
 @param 1 Texte d�ouverture, plac� avant le lien
 @param 2 Texte de fermeture, plac� apr�s le lien
 @param 3 Nombre d�arr�ts r�pondus
 @param 4 Num�ro de la commune
 @param 5 Texte entr�
 @param 6 Texte � �crire si ligne vide
 @return Liste d'arr�ts
*/
#define ELEMENTINTERFACEListeArrets      24


/** Nom de Commune
 @code O25 @endcode
 @param 0 Num�ro de la commune
 @return Nom de la commune
*/
#define ELEMENTINTERFACENomCommune  25


/** Tableau de d�part de gare
 @code O26 @endcode
<<<<<<< .working
 @param Objet cDescriptionPassage *  : Liste des d�parts et arriv�es
 @return Tableau de d�parts de gare
=======
 @param Objet cDescriptionPassage *  : Liste des d�parts et arriv�es
 @param 0 Facteur multiplicateur du num�ro de rang�e transmis � l'affichage de rang�e (vide=1)
 @param 1 Gestion des pages. Valeurs :
  - @c normal Une page unique
  - @c intermediate Une page par destination interm�diaire
  - @c destination Une page par destination interm�diaire et une pour le terminus
 @param 2 Texte s�parateur de page
 @return Tableau de d�parts de gare
>>>>>>> .merge-right.r440
*/
#define ELEMENTINTERFACETbDepGare 26


/** Tableau de d�part
 @code O27 @endcode
 @param Objet cDescriptionPassage * : Liste des d�parts
 @return Tableau de d�parts
 @deprecated Utilit� douteuse (objet 26 avec autre interface suffit)
*/
#define ELEMENTINTERFACETbDep 27

/** S�lecteur du filtre velo
 @code O29 @endcode
 @param 0 Valeur par d�faut (1/0)
 @return HTML �l�ment de formulaire du filtre v�lo
 @todo int�grer cet objet � l'objet 41
*/
#define ELEMENTINTERFACESelecteurVelo 29

/** S�lecteur du filtre taxi bus
 @code O31 @endcode
 @param 0 Valeur par d�faut (1/0)
 @return HTML �l�ment de formulaire du filtre taxi bus
 @todo int�grer cet objet � l'objet 41
*/

#define ELEMENTINTERFACESelecteurTaxiBus 31

/** Formulaire de r�servation
 @code O33 @endcode
 @param 0 Contenu champ nom
 @param 1 Contenu champ pr�nom
 @param 2 Champ pr�nom obligatoire
 @param 3 Contenu champ Adresse
 @param 4 Champ Adresse obligatoire
 @param 5 Contenu champ e-mail
 @param 6 Champ e-mail obligatoire
 @param 7 Contenu champ t�l�phone
 @param 8 Champ t�l�phone obligatoire
 @param 9 Contenu champ num�ro abonn�
 @param 10 Champ num�ro abonn� obligatoire
 @param 11 Contenu champ adresse de d�part
 @param 12 Champ adresse de d�part obligatoire
 @param 13 Contenu champ adresse d'arriv�e
 @param 14 Champ adresse d'arriv�e obligatoire
 @param 15 Contenu champ nombre de places
 @param 16 Contenu champ r�servation warning
 @return Formulaire de r�servation
 @todo Remettre aux normes
*/
#define ELEMENTINTERFACEChampsReservation       33


/** S�rie des feuilles de routes des trajets de la liste
 @code O35 @endcode
 @param Objet cTrajets * : Liste de trajets
 @param 0 Etat du \ref IHMFiltreHandicape
 @param 1 Etat du \ref IHMFiltreVelo
 @return S�rie des feuilles de routes des trajets de la liste
*/
#define ELEMENTINTERFACEFeuillesRoute        35


/** Feuille de route d'un trajet
 @code O36 @endcode
 @param Objet cTrajet * : Trajets � d�crire
 @param 0 Etat du \ref IHMFiltreHandicape
 @param 1 Etat du \ref IHMFiltreVelo
*/
#define ELEMENTINTERFACEFeuilleRoute        36


/** Dur�e du trajet
 @code 038 @endcode
 @param Objet cTrajet *
 @return Dur�e du trajet
*/
#define EI_BIBLIOTHEQUE_Trajet_Duree        38


/** Ligne de tableau contenant les dur�es de chaque trajet
 @code O39 @endcode
 @param Objet cTrajets * Liste de trajets
 @return Ligne de tableau contenant les dur�es de chaque trajet
*/
#define EI_BIBLIOTHEQUE_FicheHoraire_LigneDurees     39


/** Fabrication d'objets HTML permettant le lancement de fonctionnalit�s de SYNTHEES (formulaire ou lien)
 @code O40 @endcode
 @param 0 Type de sortie, valeurs :
  - @c url : fabrique l'adresse complete d'un appel
  - @c form @a texte : fabrique les champs d'un formulaire HTML, et place @a texte dans le tag form
 @param 1 Fonction lanc�e, valeurs :
  - @c timetable : Fiche horaire
  - @c timetable @c validation : Validation avant fiche horaire
  - @c from @c city @c list : Liste de communes de d�part
  - @c to @c city @c list : Liste de communes d'arriv�e
  - @c from @c station @c list : Liste d'arr�ts de d�part
  - @c to @c station @c list : Liste d'arr�ts d'arriv�e
 
 @code 040 {url/form} {timetable validation} @endcode
 @param 2 Num�ro arret de d�part
 @param 3 Num�ro arret d'arriv�e
 @param 4 Num�ro d�signation de d�part
 @param 5 Num�ro d�signation d'arriv�e
 @param 6 Num�ro de comune de d�part
 @param 7 Num�ro de comune d'arriv�e
 
 @code 040 {url/form} {timetable} @endcode
 @param 2 Num�ro arret de d�part
 @param 3 Num�ro arret d'arriv�e
 @param 4 Date du d�part
 @param 5 Num�ro d�signation de d�part
 @param 6 Num�ro d�signation d'arriv�e
 @param 7 Code p�riode de la journ�e
 @param 8 Filtre prise en charge des v�los
 @param 9 Filtre prise en charge handicap�s
 @param 10 Filtre lignes r�sa possible
 @param 11 Filtre tarif :
  - -1 = tout tarif
 
 @code 040 {url/form} {from/to city/station list} @endcode
 @param - Pas de param�tre suppl�mentaire
 
 @return Formulaire ou lien permettant le lancement de fonctionnalit�s de SYNTHESE
*/
#define ELEMENTINTERFACEURLFormulaire        40


/** Fabrication d'�l�ments HTML permettant l'entr�e de param�tres pour des formulaires de lancement de fonctionnalit�s de SYNTHESE
 @code O41 @endcode
 @param 0 Champ, valeurs :
  - @c date
  - @c from @c city
  - @c from @c station
  - @c to @c city
  - @c to @c station
  - @c period : P�riode de la journ�e
  - @c handicap @c filter
  - @c tariff
 @param 1 Valeur par d�faut
 @param 2 Type de la balise input, valeurs :
  - (rien) : type automatique
  - @c text
  - @c select
  - @c checkbox : case � cocher
  - @c checkbox @a texte : case � cocher de valeur @a tetxe si coch�e
 NB : en cas de choix auto, l'�l�ment est fabriqu� int�gralement (ex: liste des dates) et est ferm�. en cas de choix d�termin�, alors aucun remplissage n'est effectu� et le select doit �tre ferm�
 @param 3 Code HTML � ins�rer � l'interieur de la balise
 
 @code O41 {date} @endcode
 @param 1 Valeur par d�faut (format AAAAMMJJ, ou commandes  de date (\ref cEnvironnement::dateInterpretee )
 @param 2 Laisser vide pour s�lection
 @param 3 Code HTML � ins�rer � l'int�rieur de la balise
 @param 4 Premi�re date de la liste
 @param 5 Derni�re date de la liste
 
 @return El�ment HTML permettant l'entr�e de param�tres pour des formulaires de lancement de fonctionnalit�s de SYNTHESE
*/
#define ELEMENTINTERFACEInputHTML         41


/** Pr�fixe des messages d'alerte selon le niveau fourni en param�tre
 @code O42 @endcode
 @param 0 Niveau de l'alerte
 @return Pr�fixe des messages d'alerte selon le niveau fourni en param�tre
*/
#define EI_BIBLIOTHEQUE_Interface_PrefixeAlerte      42


/** Destination d'une ligne
 @code O43 @endcode
 @param Objet cLigne * : Ligne
 @return
  - La destination affich�e sur les v�hicules (girouette) si renseign�e
  - Le nom du terminus sinon
*/
#define EI_BIBLIOTHEQUE_Ligne_Destination       43


/** Ligne d'un d�part
 @code O O44 @endcode
 @param Objet cDescriptionPassage * : D�part
 @param 0 HTML en d�but de ligne (conseill� tr)
 @param 1 HTML en fin de ligne (conseill� /tr)
 @param 2 Largeur en pixels de la case de lignes
 @param 3 Hauteur en pixels de la case de lignes
 @return Ligne d'un d�part
*/
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne     44


/** Affichage d'un nombre format�.
 @code O 045 @endcode
 @param 0 Nombre � afficher
 @param 1 Nombre de chiffres � afficher syst�matiquement
 @param 2 Nombre � ajouter au nombre � afficher
*/
#define EI_BIBLIOTHEQUE_Nombre_Formate        45


/** Affichage de destination d'un service au d�part
 @code 0 046 @endcode
 @param Objet cDescriptionPassage* : D�part � afficher
 @param 0 Destination(s) � afficher. Valeurs :
  - @c terminus Aucune destination interm�diaire
  - @c all Toutes destinations
  - @a num Rang de la destination � afficher
 @param 1 Affichage du terminus. Valeurs :
  - @c 0 Pas d'affichage du terminus
  - @c 1 Affichage du terminus
 @param 2 Type d'affichage de la destination. Valeurs :
  - @c char(13) D�signation sur 13 caract�res
  - @c char(26) D�signation sur 26 caract�res
  - @c station Nom de l'arr�t
  - @c station_city Nom de l'arr�t pr�c�d� du nom de commune
  - @c station_city_if_new Nom de l'arr�t pr�c�d� du nom de commune uniquement si non d�j� affich�e par le parcours
 @param 3 S�parateur entre arr�ts
 @param 4 Texte � placer avant nom de commune
 @param 5 Texte � placer entre nom de commune et nom d'arr�t
 
 Table de v�rit� des affichages de destinations :
 <table class="tableau">
 <tr><th>Param.0 \ Param 1</th><th>0</th><th>1</th></tr>
 <tr><th>terminus</th><td colspan="2">Terminus</td></tr>
 <tr><th>all</th><td>Interm�diaires</td><td>Interm�diaires + Terminus</td></tr>
 <tr><th>@a num entre 1 et le nombre de gares - 1 exclu</th><td colspan="2">Interm�diaire d�sign�e</td></tr>
 <tr><th>@a num = le nombre de gares - 1</th><td>Aucun affichage</td><td>Terminus</td></tr>
 <tr><th>@a num sup�rieur au nombre de gares - 1 </th><td colspan="2">Aucun affichage</td></tr>
 </table>
*/
#define EI_BIBLIOTHEQUE_DescriptionPassage_Destination    46

#define EI_BIBLIOTHEQUE_DescriptionPassage_ArretPhysique      47


/** Affichage de l'heure de d�part.
 @code O 048 @endcode
 @param Objet cDescriptionPassage* D�part
 @param 0 Texte pr�c�dent le chiffre des heures si inf�rieur � 10 (conseill� : espace, vide, ou 0)
 @param 1 Texte pr�c�dent l'heure si le d�part s'effectue � moins d'une minute
 @param 2 Texte suivant l'heure si le d�part s'effectue � moins d'une minute
*/
#define EI_BIBLIOTHEQUE_DescriptionPassage_Heure     48

#define EI_BIBLIOTHEQUE_Cases_Particularites      49



/** Num�ro de panneau affichant un tableau de d�part
 @code O50 @endcode
 @param Objet cTableauAffichage * : Tableau de d�part
 @param 0 Format du num�ro. Valeurs :
  - @c char(2) Num�ro sur 2 caract�res
*/
#define EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau       50

//@}



/*! @} */










//! @name 3 : ELEMENTINTERFACELieuArret
//@{
#define ELEMENTINTERFACELieuArretNumeroDesignation 0
#define ELEMENTINTERFACELieuArretNumeroArretLogique 1 
//@}

//! \name 7 : EI_OBJETDYNAMIQUE_ListeLignesTrajet
//@{
#define EI_OBJETDYNAMIQUE_ListeLignesTrajet_AffichePietons  0
#define EI_OBJETDYNAMIQUE_ListeLignesTrajet_HTMLDebutLigne  1
#define EI_OBJETDYNAMIQUE_ListeLignesTrajet_HTMLFinLigne  2
#define EI_OBJETDYNAMIQUE_ListeLignesTrajet_LargeurCase   3
#define EI_OBJETDYNAMIQUE_ListeLignesTrajet_HauteurCase   4 
//@}


//! @name 9 : EI_BIBLIOTHEQUE_PERIODE
//@{
#define EI_BIBLIOTHEQUE_PERIODE_INDEX       0 
//@}


//! @name 12 : ELEMENTINTERFACELienPhotosArretPhysiques
//@{
#define ELEMENTINTERFACELienPhotosArretPhysiquesDebut 0
#define ELEMENTINTERFACELienPhotosArretPhysiquesLibelleDefaut 1
#define ELEMENTINTERFACELienPhotosArretPhysiquesFin 2 
//@}


//! @name 13 : ELEMENTINTERFACELienAutresPhotos
//@{
#define ELEMENTINTERFACELienAutresPhotosDebut 0
#define ELEMENTINTERFACELienAutresPhotosFin 1
#define ELEMENTINTERFACELienAutresPhotosOuverture 2
#define ELEMENTINTERFACELienAutresPhotosFermeture 3 
//@}


//! @name 14 : ELEMENTINTERFACELienServices
//@{
#define ELEMENTINTERFACELienServicesDebut 0
#define ELEMENTINTERFACELienServicesFin 1
#define ELEMENTINTERFACELienServicesOuverture 2
#define ELEMENTINTERFACELienServicesFermeture 3 
//@}


//! @name 22 : EI_BIBLIOTHEQUE_Date
//@{
#define EI_BIBLIOTHEQUE_Date_Date       0 
//@}


//! @name 23 : ELEMENTINTERFACEListeCommunes
//@{
#define ELEMENTINTERFACEListeCommunesMessErreur  0
#define ELEMENTINTERFACEListeCommunesOuverture  1
#define ELEMENTINTERFACEListeCommunesFermeture  2
#define ELEMENTINTERFACEListeCommunesNombre   3
#define ELEMENTINTERFACEListeCommunesEntree   4
#define ELEMENTINTERFACEListeCommunesTexteSiVide 5 
//@}


//! @name 24 : ELEMENTINTERFACEListeArrets
//@{
#define ELEMENTINTERFACEListeArretsMessErreur 0
#define ELEMENTINTERFACEListeArretsOuverture 1
#define ELEMENTINTERFACEListeArretsFermeture 2
#define ELEMENTINTERFACEListeArretsNombre  3
#define ELEMENTINTERFACEListeArretsCommune  4
#define ELEMENTINTERFACEListeArretsEntree  5
#define ELEMENTINTERFACEListeArretsTexteSiVide 6 
//@}


//! @name 25 : ELEMENTINTERFACENomCommune
//@{
#define ELEMENTINTERFACENomCommuneNumero 0 
//@}


//! @name 29 : ELEMENTINTERFACESelecteurVelo
//@{
#define ELEMENTINTERFACESelecteurVeloDefaut 0 
//@}


//! @name 30 : ELEMENTINTERFACESelecteurHandicape
//@{
#define ELEMENTINTERFACESelecteurHandicapeDefaut 0 
//@}


//! @name 31 : ELEMENTINTERFACESelecteuTaxiBus
//@{
#define ELEMENTINTERFACESelecteurTaxiBusDefaut 0 
//@}


//! @name 32 : ELEMENTINTERFACESelecteurTarif
//@{
#define ELEMENTINTERFACESelecteurTarifDefaut 0 
//@}


//! @name 33 : ELEMENTINTERFACEChampsReservation
//@{
#define ELEMENTINTERFACEChampsReservationNom       0
#define ELEMENTINTERFACEChampsReservationPrenom      1
#define ELEMENTINTERFACEChampsReservationPrenomObligatoire   2
#define ELEMENTINTERFACEChampsReservationAdresse     3
#define ELEMENTINTERFACEChampsReservationAdresseObligatoire   4
#define ELEMENTINTERFACEChampsReservationEmail      5
#define ELEMENTINTERFACEChampsReservationEmailObligatoire   6
#define ELEMENTINTERFACEChampsReservationTelephone     7
#define ELEMENTINTERFACEChampsReservationTelephoneObligatoire  8
#define ELEMENTINTERFACEChampsReservationNumeroAbonne    9
#define ELEMENTINTERFACEChampsReservationNumeroAbonneObligatoire 10
#define ELEMENTINTERFACEChampsReservationAdresseDepart    11
#define ELEMENTINTERFACEChampsReservationAdresseDepartObligatoire  12
#define ELEMENTINTERFACEChampsReservationAdresseArrivee    13
#define ELEMENTINTERFACEChampsReservationAdresseArriveeObligatoire  14
#define ELEMENTINTERFACEChampsReservationNbPlaces     15
#define ELEMENTINTERFACEChampsReservationWarning       16 
//@}


//! @name 35 : ELEMENTINTERFACEFeuillesRoute
//@{
#define ELEMENTINTERFACEFeuillesRouteFiltreHandicape    0
#define ELEMENTINTERFACEFeuillesRouteFiltreVelo      1 
//@}


//! @name 36 : ELEMENTINTERFACEFeuilleRoute
//@{
#define ELEMENTINTERFACEFeuilleRouteFiltreHandicape     0
#define ELEMENTINTERFACEFeuilleRouteFiltreVelo      1 
//@}


//! \name 40 : ELEMENTINTERFACEURLFormulaire
//@{
#define ELEMENTINTERFACEURLFormulaireTypeSortie      0
#define ELEMENTINTERFACEURLFormulaireFonction      1
#define ELEMENTINTERFACEURLFormulaireFHNumeroArretDepart   2
#define ELEMENTINTERFACEURLFormulaireFHNumeroArretArrivee   3
#define ELEMENTINTERFACEURLFormulaireFHDate       4
#define ELEMENTINTERFACEURLFormulaireFHNumeroDesignationDepart  5
#define ELEMENTINTERFACEURLFormulaireFHNumeroDesignationArrivee  6
#define ELEMENTINTERFACEURLFormulaireFHPeriode      7
#define ELEMENTINTERFACEURLFormulaireFHVelo       8
#define ELEMENTINTERFACEURLFormulaireFHHandicape     9
#define ELEMENTINTERFACEURLFormulaireFHResa       10
#define ELEMENTINTERFACEURLFormulaireFHTarif      11
#define ELEMENTINTERFACEURLFormulaireVFHNumeroArretDepart   2
#define ELEMENTINTERFACEURLFormulaireVFHNumeroArretArrivee   3
#define ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationDepart  4
#define ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationArrivee 5
#define ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneDepart   6
#define ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneArrivee  7 
//@}

//! \name 41 : ELEMENTINTERFACEInputHTML
//@{
#define ELEMENTINTERFACEInputHTMLChamp        0
#define ELEMENTINTERFACEInputHTMLValeurDefaut      1
#define ELEMENTINTERFACEInputHTMLType        2
#define ELEMENTINTERFACEInputHTMLSuite        3
#define ELEMENTINTERFACEInputHTMLListeDatesMin      4
#define ELEMENTINTERFACEInputHTMLListeDatesMax      5 
//@}

//! \name 42 : EI_BIBLIOTHEQUE_Interface_PrefixeAlerte
//@{
#define EI_BIBLIOTHEQUE_Interface_PrefixeAlerte_Niveau    0 
//@}

//! \name 44 : EI_BIBLIOTHEQUE_DescriptionPassage_Ligne
//@{
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLDebutLigne  0
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLFinLigne  1
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_LargeurCase  2
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HauteurCase  3 
//@}


//! \name 50 : EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau
//@{
#define EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau_Format     0 
//@}



#define TEMPS_MIN_CIRCULATIONS 'r'
#define TEMPS_MAX_CIRCULATIONS 'R' 

namespace synthese
{

namespace env
{
	class Environment;
}

}


/*! \brief Element d'interface de type objet dynamique : contient un objet g�n�r� par calcul en fonction des donn�es
 \author Hugues Romain
 \date 2005
 \ingroup m51
 
L'�valuation d'un �l�ment de cette classe fait appel � une biblioth�que de fonctions d'�valuation plus ou moins complexes. La fonction � utiliser est d�termin�e par l'attribut _Index, en faisant r�f�rence aux constantes d�finies da
Cette classe d�finit une �val �l�ment qui d�termine quel objet dynamique devra �tre repr�sent� lors de la fabrication des r�sultats
*/
class cInterface_Objet_Element_Bibliotheque : public cInterface_Objet_Element
{
        int _Index;  //!< Index de l'objet dynamique qui sera �valu� (Voir parametres.h)
        cInterface_Objet_AEvaluer_ListeParametres _Parametres; //!< Liste des param�tres de l'�l�ment d�finis par les fichiers de donn�es

	synthese::time::Date dateInterpretee( const synthese::env::Environment* env, 
					      const std::string& Texte ) const;


    public:
        //! \name Modificateurs
        //@{
        void SetParametres( const std::string& );
        //@}

        //! \name Calculateurs
        //@{
        int Evalue( std::ostream&, const cInterface_Objet_Connu_ListeParametres&
                       , const void* ObjetAAfficher = NULL, const cSite* __Site = NULL ) const;
        cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const;
        //@}

        //! \name Constructeurs
        //@{
        explicit cInterface_Objet_Element_Bibliotheque( int );
        explicit cInterface_Objet_Element_Bibliotheque( int, const std::string& );
        //@}
};


#endif
