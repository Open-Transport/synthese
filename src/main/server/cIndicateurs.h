/*! \file cIndicateurs.h
\brief En-t�te classes Indicateurs horaires
*/

#ifndef SYNTHESE_CINDICATEURS_H
#define SYNTHESE_CINDICATEURS_H

// Indicateurs
enum tTypeGareIndicateur
{
    PassageFacultatif = 'F',
    PassageObligatoire = 'O',
    PassageSuffisant = 'S'
};
enum tTypeOD
{
    Texte,
    Terminus,
    Indetermine
};
class cIndicateurs;
class cColonneIndicateurs;
class cGareIndicateurs;
class cRenvoiIndicateurs;
typedef int tNumeroIndicateur;
typedef float tDistancePt;

//! @name Mesures (� mettre dans interface)
//@{
#define IndicateursMarge 50
#define IndicateursHaut 841
#define IndicateursDroite 595
#define IndicateursDistanceTableauHaut 30
#define IndicateursLargeurColonneCode 22
#define IndicateursLargeurColonneCommune 50
#define IndicateursLargeurColonnePA 60
#define IndicateursLargeurColonneDA 6
#define IndicateursLargeurColonneTrain 20.8
#define IndicateursHauteurLigne 7
#define IndicateursNombreColonnesMax 17
#define IndicateursNombreLignesSupplementaires 5 
//@}

#include <string>
#include <fstream>


#include "cEnvironnement.h"
#include "cGareLigne.h"

/** Ligne de tableau d'indicateur
 @author Hugues Romain
 @date 2001
 @ingroup m35
 
 caract�ris�e par:
   - un point d'arr�t
   - un type de dessete (Depart/Arrivee/Passage)
   - un caractere obligatoire ou facultatif (pour la selection des lignes du tableau)
 Le chainage des colonnes se fait par le biais du pointeur Suivant.
*/
class cGareIndicateurs
{
    private:
        // Variables
        LogicalPlace* vPA;
        cGareLigne::tTypeGareLigneDA vTypeDA; //!< Gare de d�part, d'arriv�e, ou de passage
        tTypeGareIndicateur vObligatoire;
        cGareIndicateurs* vSuivant;

    public:
        // Accesseurs
        LogicalPlace* getArretLogique() const;
        cGareIndicateurs* getSuivant() const;
        tTypeGareIndicateur Obligatoire() const;
        cGareLigne::tTypeGareLigneDA TypeDA() const;

        // Calculateurs
        bool EstDepart() const;
        bool EstArrivee() const;

        // Modificateur
        void setSuivant( cGareIndicateurs* newVal );

        // Constructeur
        cGareIndicateurs( LogicalPlace*, cGareLigne::tTypeGareLigneDA, tTypeGareIndicateur );
};


/** Colonne de tableau d'indicateur papier
 @author Hugues Romain
 @date 2001
 @ingroup m35
 
 caract�ris�e par:
   - des pointeurs vers les cArrets d�crits
   - le code Postscript de la colonne
   - le nombre de lignes de la colonne
 Le chainage des colonnes se fait par le biais du pointeur Suivant.
*/
class cColonneIndicateurs
{
    private:
        // Variables
        const synthese::time::Schedule** vColonne;
        cColonneIndicateurs* vSuivant;
        std::string vPostScript;
        cJC::Calendar vMasque;
        cRenvoiIndicateurs* vRenvoi;
        cLigne* vLigne;
        size_t vNombreGares;
        tTypeOD vOrigineSpeciale;
        tTypeOD vDestinationSpeciale;

    public:
        // Modificateurs
        void CopiePostScript( const std::string& newVal );
        void setColonne( size_t n, const synthese::time::Schedule *newVal );
        void setSuivant( cColonneIndicateurs* newVal );
        void setDestinationSpeciale( tTypeOD newVal );
        void setOrigineSpeciale( tTypeOD newVal );
        void setRenvoi( cRenvoiIndicateurs* newVal );

        // Calculateurs
        int operator <= ( const cColonneIndicateurs& ) const;

        // Accesseurs
        tTypeOD DestinationSpeciale() const;
        cLigne* getLigne() const;
        const std::string& getPostScript() const;
        cJC::Calendar& getMasque() ;
        cColonneIndicateurs* getSuivant() const;
        cRenvoiIndicateurs* getRenvoi() const;
        tTypeOD OrigineSpeciale() const;

        // Constructeur
        cColonneIndicateurs( size_t NombreGares, cLigne*, cJC* );
};


/** Renvoi d'indicateur papier, caract�ris�e par:
   - un num�ro
   - un libell�
   - un code de jour de circulation
 
   Le libell� est clacul� en fonction du JC du renvoi, et de celui de la page d'indicateur (diff�rence).
   @author Hugues Romain
   @date 2001
   @ingroup m35
*/
class cRenvoiIndicateurs
{
    private:
        // Variables
        size_t vNumero;
        cJC* vJC;

    public:
        // Accesseurs
        size_t Numero() const;
        cJC* getJC() const;

        // Constructeur
        cRenvoiIndicateurs( cJC*, size_t );
};


/** Tableau d'indicateur papier, caract�ris�e par:
   - des gares (cGareIndicateurs)
   - des colonnes (cColonneIndicateurs)
   - un masque de circulation de base (JC)
   - Un titre
 @author Hugues Romain
 @date 2001
 @ingroup m10
*/
class cIndicateurs
{
    private:
        // Variables
        bool vCommencePage;
        cGareIndicateurs* vDerniereGI;
        cEnvironnement* const vEnvironnement;
        cJC vJC;
        size_t vNombreColonnes;
        size_t vNombreGares;
        size_t vNombreRenvois;
        cColonneIndicateurs* vPremiereCI;
        cGareIndicateurs* vPremiereGI;
        std::string const vTitre;

    public:

        // Fonction
        bool Add( cColonneIndicateurs*, cJC* );

        // Calculateurs
        void EcritTableaux( size_t HDispo, size_t NumeroColonne, size_t NombreTableaux, bool RenvoisAEcrire, size_t NumeroPageRelatif, std::ofstream& FichierSortie ) const;

        // Accesseurs
        cColonneIndicateurs* Colonne( size_t ) const;
        bool CommencePage() const;
        const cJC& getJC() const;
        cGareIndicateurs* getPremiereGI() const;
        const std::string& getTitre() const;
        size_t NombreColonnes() const;
        size_t NombreGares() const;
        size_t NombreRenvois() const;
        cRenvoiIndicateurs* Renvoi( size_t ) const;

        // Modificateurs
        void addArretLogique( LogicalPlace* newArretLogique, cGareLigne::tTypeGareLigneDA newTypeDA, tTypeGareIndicateur newTypeGI );
        void ConstruitRenvois();
        void Reset();
        void setJC( const cJC& newVal, const cJC& newVal2 );
        void setCommencePage( bool newVal );

        // Constructeur
        cIndicateurs( const std::string& newTitre, cEnvironnement* newEnvironnement );
};

/** @} */

#endif
