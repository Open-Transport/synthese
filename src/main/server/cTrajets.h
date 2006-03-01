
#ifndef SYNTHESE_CTRAJETS_H
#define SYNTHESE_CTRAJETS_H

#include <vector>
#include "cTableauDynamique.h"
#include "cTrajet.h"

class LogicalPlace;


/** Liste de trajets
 @ingroup m33
 \author Hugues Romain
 \date 2005
*/
class cTrajets : public cTableauDynamiqueObjets<cTrajet>
{
    protected:
        cTexte _txtTaille;     //!< Nombre de trajets contenus dans l'objet (format chaine de caract�res)
        cTexte _AuMoinsUneAlerte;   //!< Indique si au moins un trajet de la liste comporte une alerte en activit�
        vector<LogicalPlace*> _ListeOrdonneePointsArret; //!< Liste ordonn�e des points d'arr�ts pour repr�sentation en fiche horaire
        vector<bool> _LigneVerrouillee;   //!< Etat v�rouill� de la position de certaines lignes

        //! \name M�thodes de gestion de la repr�sentation en tableau
        //@{
        size_t OrdrePAEchangeSiPossible( size_t PositionOrigine, size_t PositionSouhaitee );
        size_t OrdrePAInsere( LogicalPlace* const, size_t Position );
        void OrdrePAConstruitLignesAPermuter( const cTrajet& __TrajetATester, bool* Resultat, size_t LigneMax ) const;
        bool OrdrePARechercheGare( size_t& i, LogicalPlace* const GareAChercher );
        void GenererNiveauxEtAuMoinsUneAlerte();
        //@}

        //! \name M�thodes de finalisation
        //@{
        void GenererListeOrdonneePointsArrets();
        //@}

    public:
        //! \name Modificateurs
        //@{
        void Finalise();
        //@}

        //! \name Accesseurs
        //@{
        LogicalPlace* getListeOrdonneePointsArret( size_t ) const;
        size_t TailleListeOrdonneePointsArret() const;
        const cTexte& GetTailleTexte() const;
        const cTexte& getAuMoinsUneAlerte() const;
        //@}

        cTrajets() : cTableauDynamiqueObjets<cTrajet>() { }
        ~cTrajets();
};



#endif
