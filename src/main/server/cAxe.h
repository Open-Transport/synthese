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

#include <string>

/** Impl�mentation de la notion d'@ref defAxe
 @ingroup m15
*/
class cAxe
{
        bool vLibre;   //!< Indique si l'axe est un @ref defAxeLibre (d�faut non)
        bool vAutorise;  //!< Indique si l'axe n'est pas un @ref defAxeInterdit (d�faut oui)
        std::string vCode;   //!< D�nomination unique de l'axe dans l'environnement
        std::string vNomFichier; //!< Nom du fichier de stockage (memoire morte)


    public:
        //! \name Accesseurs
        //@{
        const std::string& getCode() const;
        const std::string& getNomFichier() const;
        bool Libre() const;
        bool Autorise() const;
        //@}

        //! \name Constructeur
        //@{
        cAxe( bool newLibre, const std::string& newCode, const std::string& newNomFichier, bool newAutorise = true );
        //@}
};


inline bool cAxe::Libre() const
{
    return ( vLibre );
}

inline const std::string& cAxe::getCode() const
{
    return ( vCode );
}

inline const std::string& cAxe::getNomFichier() const
{
    return ( vNomFichier );
}

inline bool cAxe::Autorise() const
{
    return ( vAutorise );
}

#endif
