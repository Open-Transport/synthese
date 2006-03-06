/*! \file cPhoto.h
\brief En-t�te classes gestion des photos
\author Hugues Romain
\date 2000-2002
*/

#ifndef SYNTHESE_CPHOTO_H
#define SYNTHESE_CPHOTO_H

#include "cTexte.h"
#include "cDocument.h"

/*! \brief Zone cliquable sur photo, donnant acc�s � une autre photo ou une URL
 \author Hugues Romain
 \date 2002
 @ingroup m15
*/
class cMapPhoto
{
        cTexte vCoords; //!< Cha�ne de caract�res d�crivant les coordonn�es de la zone cliquable, au format des MAP du langage HTML
        int vLienPhoto; //!< Num�ro de la photo li�e par la zone cliquable
        cTexte vURL;  //!< URL li�e par la zone cliquable
        cTexte vAlt;  //!< Cha�nes de caract�res d�crivant la zone cliquable

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

/*!  \brief Photo � afficher dans la fiche arr�t
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
