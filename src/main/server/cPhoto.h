/*! \file cPhoto.h
\brief En-tête classes gestion des photos
\author Hugues Romain
\date 2000-2002
*/

#ifndef SYNTHESE_CPHOTO_H
#define SYNTHESE_CPHOTO_H

#include "cTexte.h"
#include "cDocument.h"

/*! \brief Zone cliquable sur photo, donnant accès à une autre photo ou une URL
 \author Hugues Romain
 \date 2002
 @ingroup m15
*/
class cMapPhoto
{
        cTexte vCoords; //!< Chaîne de caractères décrivant les coordonnées de la zone cliquable, au format des MAP du langage HTML
        int vLienPhoto; //!< Numéro de la photo liée par la zone cliquable
        cTexte vURL;  //!< URL liée par la zone cliquable
        cTexte vAlt;  //!< Chaînes de caractères décrivant la zone cliquable

    public:
        //! \name Accesseurs
        //@{
        const cTexte& Alt() const;
        const cTexte& Coords() const;
        const int LienPhoto() const;
        const cTexte& URL() const;
        //@}

        //! \name Modificateurs
        //@{
        bool SetDonnees( const cTexte& newCoords, int newLienPhoto, const cTexte& newAlt );
        bool SetDonnees( const cTexte& newCoords, const cTexte& newURL, const cTexte& newAlt );
        //@}
};

/*!  \brief Photo à afficher dans la fiche arrêt
 \author Hugues Romain
 \date 2002
 @ingroup m15
*/
class cPhoto : public cDocument
{
        cTableauDynamiqueObjets<cMapPhoto> _Map; //!< Liste des zones cliquables

    public:

        //! \name Accesseurs
        //@{
        const cMapPhoto& Map( int ) const;
        int NombreMaps() const;
        //@}

        //! \name Constructeur et fonctions de construction
        //@{
        cPhoto( const size_t& );
        //@}

        //! \name Modificateurs
        //@{
        bool addMapPhoto( const cTexte& newCoords, int newLienPhoto, const cTexte& newAlt );
        bool addMapPhoto( const cTexte& newCoords, const cTexte& newURL, const cTexte& newAlt );
        //@}
};

#endif
