/** En-t�te classe �l�ment de trajet � pied suivant la voirie.
@file cElementTrajetVoirie.h
*/

#ifndef SYNTHESE_CELEMENTTRAJETVOIRIE_H
#define SYNTHESE_CELEMENTTRAJETVOIRIE_H

#include "cElementTrajet.h"
#include "map/RoadChunk.h"

/** El�ment de trajet pi�ton empruntant la voirie.
 @ingroup m33
 
 Si l'arret de d�part ou d'arriv�e est � NULL, alors il s'agit des origines ou destinations de la recherche d'itin�raires
*/
class cElementTrajetVoirie : public cElementTrajet
{
        //! @name Donn�es (A REVOIR PAR MJ : Une struct dans un seul tableau ? :)
        //@{
        std::vector<int> _IDRue;  //!< ID de la rue emprunt�e
        std::vector<tDistanceM> _PMDebut; //!< Point m�trique de d�but d'utilisation de la rue
        std::vector<tDistanceM> _PMFin;  //!< Point m�trique de fin d'utilisation de la rue
        //@}


        //! @name Parametres
        //@{
        const bool _AjoutEnFin; //!< Vrai = le trajet est constitu� par la succession des segments ajout�s dans l'ordre de leur ajout. Faux = ordre inverse.
        //@}

    public:

        //! @name Constructeur et destructeur
        //@{

        /** Constructeur.
         @param __AjoutEnFin Vrai = le trajet est constitu� par la succession des segments ajout�s dans l'ordre de leur ajout. Faux = ordre inverse.
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
         @param __Segment Segment de rue � ajouter

         L'ajout s'effectue en fin ou en d�but de tableau selon la valeur de _AjoutEnFin.
         Si le segment est sur la m�me rue que l'ajout pr�c�dent, alors seules les valeurs _PMDebut/_PMFin sont impact�es.
         Si le segment est sur une rue diff�rente, alors une entr�e est cr��e dans le tableau de donn�es.
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
