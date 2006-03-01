
#ifndef SYNTHESE_CTABLEAUAFFICHAGE_H
#define SYNTHESE_CTABLEAUAFFICHAGE_H

class cTableauAffichage;
class cLigne;
class cDescriptionPassage;
class cArretPhysique;
class LogicalPlace;
class cGareLigne;

#include "cSite.h"
#include "Parametres.h"
#include "04_time/Hour.h"
#include "cTexte.h"


#include <set>

/** Tableau d'affichage.
 @ingroup m34
*/
class cTableauAffichage : public cSite
{
    protected:
        //! \name Param�tres du tableau
        //@{
        cTexte _Titre;    //!< Titre pour affichage
        int _NombreDeparts;  //!< Nombre de d�parts affich�s
        int _PremierDepart;  //!< Premier d�part affich�
        synthese::time::Hour _HeureBascule;  //!< Heure de basculement entre deux jours
        bool _OriginesSeulement; //!< Origines seulement ?
        int _NumeroPanneau;  //!< Num�ro du panneau dans la gare pour afficheur
        //@}

        //! \name Donn�es
        //@{
        LogicalPlace* _ArretLogique;  //!< Point d'arr�t affich�
        set
            <cArretPhysique*> _ArretPhysiques;    //!< ArretPhysique(s) affich�s
        set
            <cLigne*> _LignesInterdites; //!< Lignes ne devant pas �tre affich�es
        set
            <LogicalPlace*> _DestinationsAffichees;
        //@}

        //! \name M�thodes prot�g�es
        //@{
        synthese::time::DateTime _MomentFin( const synthese::time::DateTime& __MomentDebut ) const;
        bool _LigneAutorisee( const cGareLigne* ) const;
        //@}

    public:
        //! \name Constructeur et destructeur
        //@{
        cTableauAffichage( const std::string& __Code );
        ~cTableauAffichage( void );
        //@}

        //! \name Modificateurs
        //@{
        virtual bool SetArretLogique( LogicalPlace* const __ArretLogique );
        bool SetNombreDeparts( int __NombreDeparts );
        void AddLigneInterdte( cLigne* __Ligne );
        void AddArretPhysiqueAutorise( int __NumeroArretPhysique );
        bool SetTitre( const cTexte& );
        bool SetOriginesSeulement( bool __Valeur );
        void AddDestinationAffichee( LogicalPlace* __ArretLogique );
        void SetNumeroPanneau( int );
        //@}

        //! \name Accesseurs
        //@{
        const cTexte& getTitre() const;
        int getNumeroPanneau() const;
        //@}

        //! \name Calculateurs
        //@{
        virtual cDescriptionPassage* Calcule( const synthese::time::DateTime& __MomentDebut ) const;
        void ListeArretsAffiches( cDescriptionPassage* __DP ) const;
        //@}

};

/** @} */

#endif
