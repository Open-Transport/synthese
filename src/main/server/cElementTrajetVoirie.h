/** En-tête classe élément de trajet à pied suivant la voirie.
@file cElementTrajetVoirie.h
*/

#ifndef SYNTHESE_CELEMENTTRAJETVOIRIE_H
#define SYNTHESE_CELEMENTTRAJETVOIRIE_H

#include "cElementTrajet.h"
#include "map/RoadChunk.h"

/** Elément de trajet piéton empruntant la voirie.
 @ingroup m33
 
 Si l'arret de départ ou d'arrivée est à NULL, alors il s'agit des origines ou destinations de la recherche d'itinéraires
*/
class cElementTrajetVoirie : public cElementTrajet
{
        //! @name Données (A REVOIR PAR MJ : Une struct dans un seul tableau ? :)
        //@{
        std::vector<int> _IDRue;  //!< ID de la rue empruntée
        std::vector<tDistanceM> _PMDebut; //!< Point métrique de début d'utilisation de la rue
        std::vector<tDistanceM> _PMFin;  //!< Point métrique de fin d'utilisation de la rue
        //@}


        //! @name Parametres
        //@{
        const bool _AjoutEnFin; //!< Vrai = le trajet est constitué par la succession des segments ajoutés dans l'ordre de leur ajout. Faux = ordre inverse.
        //@}

    public:

        //! @name Constructeur et destructeur
        //@{

        /** Constructeur.
         @param __AjoutEnFin Vrai = le trajet est constitué par la succession des segments ajoutés dans l'ordre de leur ajout. Faux = ordre inverse.
        */
        cElementTrajetVoirie(
            const NetworkAccessPoint* const _origin,
            const NetworkAccessPoint* const _destination,
            const synthese::time::DateTime& vMomentDepart,
            const synthese::time::DateTime& vMomentArrivee,
            int amplitude,
            const cDistanceCarree& squaredDistanceFromGoal,
            bool __AjoutEnFin = true )
                : cElementTrajet( _origin,
                                  _destination,
                                  vMomentDepart,
                                  vMomentArrivee,
                                  0,
                                  NULL,
                                  eTrajetVoirie,
                                  amplitude,
                                  squaredDistanceFromGoal ), _AjoutEnFin( __AjoutEnFin )
        {}
        //@}


        //! @name Modificateurs
        //@{

        /** Ajout de segment de rue.
         @param __Segment Segment de rue à ajouter

         L'ajout s'effectue en fin ou en début de tableau selon la valeur de _AjoutEnFin.
         Si le segment est sur la même rue que l'ajout précédent, alors seules les valeurs _PMDebut/_PMFin sont impactées.
         Si le segment est sur une rue différente, alors une entrée est créée dans le tableau de données.
        */
        void AjoutSegment( const synmap::RoadChunk* __Segment );
        //@}


        //! @name Accesseurs
        //@{

        /** Lecture de segment.
         SPEC A FAIRE LORS DE LA MISE EN INTERFACE
         (peut etre faire une methode GetRue seulement)
        */
        void GetSegment( int __Index );
        //@}
};

#endif
