/*! \file cPhoto.h
\brief En-tête classes gestion des photos
\author Hugues Romain
\date 2000-2002
*/

#ifndef SYNTHESE_CPHOTO_H
#define SYNTHESE_CPHOTO_H

#include <string>
#include "cDocument.h"
#include "cTableauDynamique.h"

/*! \brief Zone cliquable sur photo, donnant accès à une autre photo ou une URL
 \author Hugues Romain
 \date 2002
 @ingroup m15
*/
class cMapPhoto
{
        std::string vCoords; //!< Chaîne de caractères décrivant les coordonnées de la zone cliquable, au format des MAP du langage HTML
        int vLienPhoto; //!< Numéro de la photo liée par la zone cliquable
        std::string vURL;  //!< URL liée par la zone cliquable
        std::string vAlt;  //!< Chaînes de caractères décrivant la zone cliquable

    public:
        //! \name Accesseurs
        //@{
        const std::string& Alt() const;
        const std::string& Coords() const;
        const int LienPhoto() const;
        const std::string& URL() const;
        //@}

        //! \name Modificateurs
        //@{
        bool SetDonnees( const std::string& newCoords, int newLienPhoto, const std::string& newAlt );
        bool SetDonnees( const std::string& newCoords, const std::string& newURL, const std::string& newAlt );
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
        bool addMapPhoto( const std::string& newCoords, int newLienPhoto, const std::string& newAlt );
        bool addMapPhoto( const std::string& newCoords, const std::string& newURL, const std::string& newAlt );
        //@}
};

#endif

