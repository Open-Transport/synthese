/*------------------------------------*
|                                    |
|  APDOS / APISAPI - SYNTHESE v0.4   |
|  � Hugues Romain 2000/2002         |
|  CAxe.h                            |
|  Header Classes Axe                |
|                                    |
*------------------------------------*/

#ifndef SYNTHESE_CAXE_H
#define SYNTHESE_CAXE_H

class cAxe;

#include "cTexte.h"

/** Impl�mentation de la notion d'@ref defAxe
 @ingroup m15
*/
class cAxe
{
        bool vLibre;   //!< Indique si l'axe est un @ref defAxeLibre (d�faut non)
        bool vAutorise;  //!< Indique si l'axe n'est pas un @ref defAxeInterdit (d�faut oui)
        cTexte vCode;   //!< D�nomination unique de l'axe dans l'environnement
        cTexte vNomFichier; //!< Nom du fichier de stockage (memoire morte)


    public:
        //! \name Accesseurs
        //@{
        const cTexte& getCode() const;
        const cTexte& getNomFichier() const;
        bool Libre() const;
        bool Autorise() const;
        //@}

        //! \name Constructeur
        //@{
        cAxe( bool newLibre, const cTexte& newCode, const cTexte& newNomFichier, bool newAutorise = true );
        //@}
};


inline bool cAxe::Libre() const
{
    return ( vLibre );
}

inline const cTexte& cAxe::getCode() const
{
    return ( vCode );
}

inline const cTexte& cAxe::getNomFichier() const
{
    return ( vNomFichier );
}

inline bool cAxe::Autorise() const
{
    return ( vAutorise );
}

#endif
