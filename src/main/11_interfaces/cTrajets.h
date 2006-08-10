
#ifndef SYNTHESE_CTRAJETS_H
#define SYNTHESE_CTRAJETS_H


#include <vector>
#include "cTableauDynamique.h"

#include "33_route_planner/Journey.h"
#include "15_env/ConnectionPlace.h"




/** Liste de trajets
 @ingroup m33
 \author Hugues Romain
 \date 2005
*/
class cTrajets : public cTableauDynamiqueObjets<synthese::routeplanner::Journey>
{
    protected:
        std::string _txtTaille;     //!< Nombre de trajets contenus dans l'objet (format chaine de caracteres)
        std::string _AuMoinsUneAlerte;   //!< Indique si au moins un trajet de la liste comporte une alerte en activite
        std::vector<const synthese::env::ConnectionPlace*> _ListeOrdonneePointsArret; //!< Liste ordonnee des points d'arrets pour representation en fiche horaire
        std::vector<bool> _LigneVerrouillee;   //!< Etat verouille de la position de certaines lignes

        //! \name Methodes de gestion de la representation en tableau
        //@{
        size_t OrdrePAEchangeSiPossible( size_t PositionOrigine, size_t PositionSouhaitee );
        size_t OrdrePAInsere( const synthese::env::ConnectionPlace*, size_t Position );
        void OrdrePAConstruitLignesAPermuter( const synthese::routeplanner::Journey& __TrajetATester, bool* Resultat, size_t LigneMax ) const;
        bool OrdrePARechercheGare( size_t& i, const synthese::env::ConnectionPlace* GareAChercher );
        void GenererNiveauxEtAuMoinsUneAlerte();
        //@}

        //! \name Methodes de finalisation
        //@{
        void GenererListeOrdonneePointsArrets();
        //@}

    public:

        cTrajets() : cTableauDynamiqueObjets<synthese::routeplanner::Journey>() { }
        ~cTrajets();


        //! \name Modificateurs
        //@{
        void Finalise();
        //@}

        //! \name Accesseurs
        //@{
        const synthese::env::ConnectionPlace* getListeOrdonneePointsArret( size_t ) const;
        size_t TailleListeOrdonneePointsArret() const;
        const std::string& GetTailleTexte() const;
        const std::string& getAuMoinsUneAlerte() const;
        //@}

};



#endif
