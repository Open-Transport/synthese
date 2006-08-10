#ifndef SYNTHESE_CINTERFACE_OBJET_AEVALUER_PAGEECRAN_H
#define SYNTHESE_CINTERFACE_OBJET_AEVALUER_PAGEECRAN_H


#include "cInterface_Objet.h"
#include "cInterface_Objet_AEvaluer.h"


class cInterface_Objet_AEvaluer_PageEcran;
class cInterface_Objet_AEvaluer_ListeParametres;


/*! \addtogroup m11
 @{
*/



/*! \class cInterface_Objet_AEvaluer_PageEcran
 \brief Objet d'interface d�finissant une page d'�cran
 \author Hugues Romain
 \date 2005
 
Cette classe particuli�re d'objet � �valuation ult�rieure d�finit tout ou partie d'une page �cran.
 
Pour construire les objets de cette classe, le cha�nage classique d'�l�ments est naturellement possible. S'ajoute cependant une fonctionnalit� d'inclusion d'objets du m�me type d�finis auparavant assur�e par la m�thode cInterface_Objet_AEvaluer_PageEcran::InclureCopie.
 
Les �l�ments de cette classe sont issus des fichiers .elementinterface.per et sont lus par la m�thode \ref cFichierObjetInterfaceStandard::Charge() .
 
Les pages �cran qui sont fournies par les fichiers sont reli�es � l'interface par un index, d�fini par les define pr�sents dans le fichier cInterface_Objet_AEvaluer_PageEcran.h
*/
class cInterface_Objet_AEvaluer_PageEcran : public cInterface_Objet_AEvaluer
{
    public:
        //! \name Calculateurs
        //@{
        bool InclureCopie( const cInterface_Objet_AEvaluer_PageEcran&, const cInterface_Objet_AEvaluer_ListeParametres& );
        //@}
};



//! \name Liste des pages �cran � fournir dans les donn�es d'interface
//@{


/** En-t�te pr�sent sur toute page
 @code >00 @endcode
 @return En-t�te pr�sent sur toute page
*/
#define INTERFACEEnTete       0


/** Pied-de-page pr�sent sur toute page
 @code >01 @endcode
 @return Pied-de-page pr�sent sur toute page
*/
#define INTERFACEPied       1



/** Formulaire d'entr�e recherche d'itin�raires
 @code >02 @endcode
 @param 0 Texte par d�faut dans le champ commune de depart
 @param 1 Numero de commune de depart par defaut
 @param 2 Texte par d�faut dans le champ arret de depart
 @param 3 Numero d'arret de depart par defaut
 @param 4 Texte par d�faut dans le champ commune d'arriv�e
 @param 5 Numero de commune d'arriv�e par defaut
 @param 6 Texte par d�faut dans le champ arret d'arriv�e
 @param 7 Numero d'arret d'arriv�e par defaut
 @param 8 Numero de d�signation d�part par defaut
 @param 9 Numero de d�signation d'arriv�e par defaut
 @param 10 P�riode de la journ�e par d�faut
 @param 11 Etat par d�faut filtre v�lo
 @param 12 Etat par d�faut filtre handicap�
 @param 13 Etat par d�faut filtre lignes � r�server
 @param 14 Valeur par d�faut filtre tarif
 @param 15 Date par d�faut
 @return Formulaire d'entr�e recherche d'itin�raires
*/
#define INTERFACEFormulaireEntree    2


/** Nom de point d'arr�t
 @code >03 @endcode
 @param Objet LogicalPlace * : Point d'arr�t � nommer
 @param 0 Num�ro de d�signation
 @return Nom de point d'arr�t
*/
#define INTERFACENomArret      3


/** D�signation compl�te de ligne
 @code >04 @endcode
 @param Objet cLigne * : Ligne � afficher
 @return D�signation compl�te de ligne
*/
#define INTERFACEDesignationLigne    5


/** Cartouche de ligne
 @code >05 @endcode
 @param Objet cLigne * : Ligne � afficher
 @param 0 HTML d'ouverture du tableau (RIEN = Pas d'ouverture du tableau)
 @param 1 HTML de fermeture du tableau (RIEN = Pas de fermeture du tableau)
 @param 2 Largeur de la case en pixels
 @param 3 Hauteur de la case en pixels
 @return Cartouche de ligne
*/
#define INTERFACECartoucheLigne     6


/** Liste de communes d'origine
 @code >07 @endcode
 @param 0 Texte d�entr�e pour recherche commune
 @return Liste de communes d'origine
*/
#define INTERFACEListeCommunesDepart   7


/** Liste de communes de destination
 @code >08 @endcode
 @param 0 Texte d�entr�e pour recherche commune
 @return Liste de communes de destination
*/
#define INTERFACEListeCommunesArrivee   8


/** Liste de points d'arr�t d'origine
 @code >09 @endcode
 @param 0 Texte d�entr�e pour recherche arret
 @param 1 Num�ro de la commune
 @return Liste de points d'arr�t d'origine
*/
#define INTERFACEListeArretsDepart    9


/** Liste de points d'arr�t de destination
 @code >10 @endcode
 @param 0 Texte d�entr�e pour recherche arret
 @param 1 Num�ro de la commune
 @return Liste de points d'arr�t de destination
*/
#define INTERFACEListeArretsArrivee    10


/** Page d'erreur de validation des donn�es du formulaire de recherche d'itin�raire
 @code >11 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
 <tr><th>Num�ro</th><th>Nom</th><th>Description</th><th>Valeur</th></tr>
 <tr><td>0</td><td></td><td>Texte entr� dans le champ commune d�part</td><td></td></tr>
 <tr><td>1</td><td></td><td>Num�ro commune d�part</td><td></td></tr>
 <tr><td>2</td><td></td><td>Texte entr� dans le champ arr�t d�part</td><td></td></tr>
 <tr><td>3</td><td></td><td>Num�ro arret de d�part</td><td></td></tr>
 <tr><td>4</td><td></td><td>Texte entr� dans le champ commune arriv�e</td><td></td></tr>
 <tr><td>5</td><td></td><td>Num�ro commune arriv�e</td><td></td></tr>
 <tr><td>6</td><td></td><td>Texte entr� dans le champ arr�t arriv�e</td><td></td></tr>
 <tr><td>7</td><td></td><td>Num�ro arret d'arriv�e</td><td></td></tr>
 <tr><td>8</td><td></td><td>Num�ro d�signation de d�part</td><td></td></tr>
 <tr><td>9</td><td></td><td>Num�ro d�signation d'arriv�e</td><td></td></tr>
 <tr><td>10</td><td></td><td>Code p�riode de la journ�e</td><td></td></tr>
 <tr><td>11</td><td></td><td>Filtre prise en charge des v�los</td><td></td></tr>
 <tr><td>12</td><td></td><td>Filtre prise en charge handicap�s</td><td></td></tr>
 <tr><td>13</td><td></td><td>Filtre lignes r�sa possible</td><td></td></tr>
 <tr><td>14</td><td></td><td>Filtre tarif</td><td>-1 = tout tarif</td></tr>
 <tr><td>15</td><td></td><td>Date du d�part</td><td></td></tr>
 <tr><td>16</td><td></td><td>Message d'erreur</td><td></td></tr>
</table>
 
Ce message s�affiche si l�utilisateur a lanc� le calcul sans avoir valid� ses arr�ts de d�part ou arriv�e, et si une ambigu�t� rend impossible le choix des arr�ts par les m�thodes cEnvironnement::TextToCommune()
et cCommune::TextToPADe().
 
De ce fait, ce message va afficher, pour le d�part et l�arriv�e&nbsp;:
 - un message explicitant l�erreur commise
 - rien s�il obtient num�ro de commune et num�ro d'arr�t coh�rents
 - une erreur de requ�te sinon
*/
#define INTERFACEErreurArretsFicheHoraire  11



/** Ecran d'attente
 @code >12 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
 <tr><th>Num�ro</th><th>Nom</th><th>Description</th><th>Valeur</th></tr>
 <tr><td>0</td><td></td><td>Num�ro arret de d�part</td><td></td></tr>
 <tr><td>1</td><td></td><td>Num�ro arret d'arriv�e</td><td></td></tr>
 <tr><td>2</td><td></td><td>Date du d�part</td><td></td></tr>
 <tr><td>3</td><td></td><td>Num�ro d�signation de d�part</td><td></td></tr>
 <tr><td>4</td><td></td><td>Num�ro d�signation d'arriv�e</td><td></td></tr>
 <tr><td>5</td><td></td><td>Code p�riode de la journ�e</td><td></td></tr>
 <tr><td>6</td><td></td><td>Filtre prise en charge des v�los</td><td></td></tr>
 <tr><td>7</td><td></td><td>Filtre prise en charge handicap�s</td><td></td></tr>
 <tr><td>8</td><td></td><td>Filtre lignes r�sa possible</td><td></td></tr>
 <tr><td>9</td><td></td><td>Filtre tarif</td><td>-1 = tout tarif</td></tr>
</table> 
*/
#define INTERFACEAttente      12

/** Tableau de d�part de t�l�affichage
 @code >13 @endcode
*/
#define INTERFACETbDepGare      13

/** Tableau de d�part
 @code >14 @endcode
*/
#define INTERFACETbDep       14


/** Formulaire de r�servation
 @code >15 @endcode
 @param 0 Cl� du site (inutile)
 @param 1 Code de la ligne
 @param 2 Num�ro du service emprunt�
 @param 3 Code point arr�t de d�part
 @param 4 Code point arr�t d'arriv�e
 @param 5 Moment du d�part (format interne)
 @return Formulaire de r�servation
*/
#define INTERFACEFormResa      15



/** Ecran de validation de r�servation
 @code >16 @endcode
*/
#define INTERFACEValidResa      16



/** Colonne de fiche horaire
 @code >17 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
 <tr><th>Num�ro</th><th>Description</th><th>Valeur</th></tr>
 <tr><td>0</td><td>Premier d�part ou arriv�e ?</td><td>1 si oui RIEN si non</td></tr>
 <tr><td>1</td><td>D�part ou derni�re arriv�e ?</td><td>1 si oui RIEN si non</td></tr>
 <tr><td>2</td><td>Num�ro de colonne</td><td>Sur trois chiffres</td></tr>
 <tr><td>3</td><td>Jonction � pied ?</td><td>1 si oui RIEN si non</td></tr>
 <tr><td>4</td><td>Heure (d�but si service continu)</td><td><td></td></tr>
 <tr><td>5</td><td>Heure de fin de service continu</td><td>RIEN si pas de service continu<td></td></tr>
 <tr><td>6</td><td>Service continu et pas � pied ?</td><td>1 si oui RIEN si non<td></td></tr>
</table>
*/
#define INTERFACEFicheHoraireColonne   17



/** Feuille de route
 @code >18 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
 <tr><th>Num�ro</th><th>Description</th><th>Valeur</th></tr>
 <tr><td>0</td><td>Index du trajet dans la fiche horaire</td><td><i>1..infini</i></td></tr>
 <tr><td>1</td><td>Etat du filtre handicap�</td><td>1|0</td></tr>
 <tr><td>2</td><td>Etat du filtre v�lo</td><td>1|0</td></tr>
 <tr><td>3</td><td>Index du trajet pr�c�dent dans la fiche horaire</td><td><i>RIEN</i> si premier trajet<br><i>1..infini</i> sinon</td></tr>
 <tr><td>4</td><td>Index du trajet pr�c�dent dans la fiche horaire</td><td><i>RIEN</i> si dernier trajet<br><i>1..infini</i> sinon</td></tr>
 <tr><td>5</td><td>Date du d�part</td><td><i>AAAAMMJJ</i><br>NB : Peut �tre sup�rieure � la date de la fiche horaire</td></tr>
</table>
*/
#define INTERFACEFeuilleRoute     18



/** Ligne de feuille de route d�crivant le passage � un arr�t
 @code >19 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
 <tr><th>Num�ro</th><th>Description</th><th>Valeur</th></tr>
 <tr><td>0</td><td>Descente � l'arr�t</td><td>1 si l'arr�t est utilis� � la descente d'une circulation<br />RIEN si l'arr�t est utilis�e pour la mont�e</td></tr>
 <tr><td>1</td><td>Message d'alerte</td><td></td></tr>
 <tr><td>2</td><td>Niveau du message d'alerte</td><td></td></tr>
 <tr><td>3</td><td>Descente au terminus</td><td>1 si l'arr�t correspond au terminus de la ligne<br />RIEN si l'arr�t est un autre arr�t de la ligne</td></tr>
</table>
*/
#define INTERFACELigneFeuilleRouteArret   19



/** Ligne de feuille de route d�crivant l'utilisation d'une circulation
 @code >20 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
 <tr><th>Num�ro</th><th>Description</th><th>Valeur</th></tr>
 <tr><td>0</td><td>Heure de d�part (premier si service continu)</td><td></td></tr>
 <tr><td>1</td><td>Heure de dernier d�part si service continu</td><td>RIEN si service unique (permet de tester)</td></tr>
 <tr><td>4</td><td>Code mat�riel roulant</td><td></td></tr>
 <tr><td>5</td><td>Nom mat�riel roulant</td><td></td></tr>
 <tr><th>8</th><td>Etat du filtre handicap�</td><td></td></tr>
 <tr><th>9</th><td>Nombre de places pour handicap�</td><td></td></tr>
 <tr><th>10</th><td>Etat du filtre v�lo</td><td></td></tr>
 <tr><th>11</th><td>Nombre de places pour v�lo</td><td></td></tr>
 <tr><th>12</th><td>Ligne � r�servation obligatoire ?</td><td></td></tr>
 <tr><th>13</th><td>Ligne � r�servation facultative ?</td><td></td></tr>
 <tr><th>14</th><td>Date limite de r�servation</td><td>RIEN si sans objet</td></tr>
 <tr><th>15</th><td>Num�ro de t�l�phone pour r�server</td><td>RIEN si sans objet</td></tr>
 <tr><th>16</th><td>Horaires d'ouverture de la centrale de r�servation</td><td>RIEN si sans objet</td></tr>
 <tr><th>17</th><td>Site internet de r�servation</td><td>RIEN si sans objet</td></tr>
 <tr><th>18</th><td>R�servation en ligne possible ?</td><td></td></tr>
 <tr><th>19</th><td>Message d'alerte</td><td></td></tr>
 <tr><th>20</th><td>Niveau d'alerte</td><td></td></tr>
 <tr><th>21</th><td>Couleur de fond</td><td>1 ou RIEN alternativement</td></tr>
</table>
*/
#define INTERFACELigneFeuilleRouteCirculation 20


/** Ligne de feuille de route d�crivant un passage par une jonction � pied
 @code >21 @endcode
 @param 0 Arr�t � rejoindre � pied</th><th>Index de l'arr�t
 @param 1 Message d'alerte de l'arr�t � rejoindre � pied (RIEN = Pas d'alerte)
 @param 2 Niveau d'alerte de l'arr�t � rejoindre � pied
 @return Ligne de feuille de route d�crivant un passage par une jonction � pied
*/
#define INTERFACELigneFeuilleRouteJonction  21


#define INTERFACEFicheArret      23
#define INTERFACECaseLignes      24
#define INTERFACEAucuneSolutionFicheHoraire  25



/** Case dur�e pour fiche horaire
 @code >26 @endcode
*/
#define INTERFACECaseDuree      26



/** Case renvoi pour fiche horaire
 @code >27 @endcode
 @param 0 Num�ro de la colonne
 @param 1 Niveau maximal d'alerte
 @return Case renvoi pour fiche horaire
*/
#define INTERFACECaseRenvoi      27


/** Ligne de fiche horaire
 @code >28 @endcode
 @param Objet LogicalPlace * : Point d'arr�t de la ligne
 @param 0 Texte colonnes
 @param 1 Parit� du rang de la ligne, valeurs :
  - 0 = pair
  - 1 = impair
 @return Ligne de fiche horaire
*/
#define INTERFACEFicheHoraireLigne    28

#define INTERFACEDuree       29



/** Mini tableau de d�part de gare
 @code >30 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
 <tr><th>Num�ro</th><th>Description</th><th>Valeur</th></tr>
 <tr><td>NPA</td><td>1</td><td>Num�ro du point d�arr�t</td></tr>
 <tr><td>NPropositions</td><td>2</td><td>Nombre de propositions (-1 = illimit�)</td></tr>
</table>
*/
#define INTERFACEMiniTbDepGare     30


/** Page d'accueil
 @code >33 @endcode
 @return Page d'accueil
*/
#define INTERFACEPageAccueil     33


/** Fiche horaire
 @code >34 @endcode
 @param 0 Texte par d�faut dans le champ commune de depart
 @param 1 Numero de commune de depart par defaut
 @param 2 Texte par d�faut dans le champ arret de depart
 @param 3 Numero d'arret de depart par defaut
 @param 4 Texte par d�faut dans le champ commune d'arriv�e
 @param 5 Numero de commune d'arriv�e par defaut
 @param 6 Texte par d�faut dans le champ arret d'arriv�e
 @param 7 Numero d'arret d'arriv�e par defaut
 @param 8 Numero de d�signation d�part par defaut
 @param 9 Numero de d�signation d'arriv�e par defaut
 @param 10 P�riode de la journ�e par d�faut
 @param 11 Etat par d�faut filtre v�lo
 @param 12 Etat par d�faut filtre handicap�
 @param 13 Etat par d�faut filtre lignes � r�server
 @param 14 Valeur par d�faut filtre tarif
 @param 15 Date par d�faut
 @return Fiche horaire
*/
#define INTERFACEFicheHoraire     34


/** Ligne de tableau de d�part
 @code >35 @endcode
 @param 0 Num�ro de rang�e
 @param 1 Num�ro de page de la rang�e
 @param (projet) Situation perturb�e ?
 @param (projet) Heure de passage r�elle
*/
#define INTERFACELigneTableauDepart    35

#define INTERFACECaseParticularite    36


//! \brief Nombre de pages �cran disposibles, pour allocation exacte des tableaux
#define INTERFACESNOMBREELEMENTS    37

//@}
/*! @} */

#endif

