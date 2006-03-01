/*! \file cTarif.h
\brief En-tête classe de gestion des tarifications
*/

#ifndef SYNTHESE_CTARIF_H
#define SYNTHESE_CTARIF_H

#include "cTexte.h"

// Modalités de tarification
typedef int tNumeroTarif;
class cTarif;

/**
 @ingroup m15
*/
enum tTypeTarif
{
    Zonage = 'Z',
    Section = 'S',
    Distance = 'D'
};


/** Classes de gestion des tarifications
 @ingroup m15
*/
class cTarif
{
    private:

        const size_t _id;  //!< Index du tarif
        tTypeTarif vTypeTarif; //!< Type de tarif
        cTexte vLibelle; //!< Nom du tarif


    public:
        //! \name constructeur
        //@{
        cTarif( const size_t& );
        //@}

        //! \name Modificateurs
        //@{
        void setTypeTarif( const tTypeTarif newVal );
        void setLibelle( const cTexte& libelle );
        //@}

        //! \name Accesseurs
        //@{
        tTypeTarif getTypeTarif() const;
        const cTexte& getLibelle() const;
        size_t getNumeroTarif() const;
        //@}
};

#endif
