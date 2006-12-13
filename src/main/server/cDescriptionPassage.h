/*---------------------------------------*
|                                       |
|  APDOS / APISAPI - SYNTHESE v0.5      |
|  � Hugues Romain 2000/2001            |
|  cDescriptionPassage.h                |
|  Header Classes Description passages  |
|                                       |
*---------------------------------------*/

#ifndef SYNTHESE_CDESCRIPTIONPASSAGE_H
#define SYNTHESE_CDESCRIPTIONPASSAGE_H

class cDescriptionPassage;
class LogicalPlace;
class cTrain;
class cGareLigne;
class cSitPert;

#include "cTableauDynamique.h"
#include "04_time/DateTime.h"

/** R�sultat de tableau d'affichage
 @ingroup m34
*/
class cDescriptionPassage
{
        friend class cTableauAffichage;

    private:
        // Variables
        synthese::time::DateTime vMomentPrevu;
        synthese::time::DateTime vMomentReel;
        synthese::time::DateTime _ArriveeTerminus; //!< Moment d'arriv�e au terminus de la ligne
        cSitPert* vSitPert;
        size_t vNumArret;
        cGareLigne* _GareLigne;
        cTableauDynamique<LogicalPlace*> _Gare;

        cDescriptionPassage* vSuivant;
        cDescriptionPassage* vPrecedent;

    public:
        //! \name Accesseurs
        //@{
        LogicalPlace* GetGare( int __i = 0 ) const;
        int NombreGares() const;
        cGareLigne* getGareLigne() const;
        const synthese::time::DateTime& getMomentPrevu() const;
        const synthese::time::DateTime& getMomentReel() const;
        cSitPert* getSitPert() const;
        const synthese::time::DateTime& MomentFin() const;
        const size_t& NumArret() const;
        cDescriptionPassage* Suivant() const;
        //@}

        //! \name Calculateurs
        //@{
        synthese::time::DateTime MomentArriveeDestination() const;
        const LogicalPlace* Destination() ;
        //@}

        // Constructeur et fonctions de construction
        cDescriptionPassage();
        void Remplit( cGareLigne* GareLigne, const synthese::time::DateTime& tempMomentDepart, const size_t& iNumeroService );
        cDescriptionPassage* GetDernierEtLibere();
        cDescriptionPassage* Insere( cDescriptionPassage* ); // Retourne le premier de la liste apr�s avoir ins�r� le newDP


};

#endif

