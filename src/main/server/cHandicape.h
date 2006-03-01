/*! \brief cHandicape.h
\brief En-t�te classe modalit� de prise en charge des handicap�s
*/



#ifndef SYNTHESE_CHANDICAPE_H
#define SYNTHESE_CHANDICAPE_H

class cModaliteReservation;

#include "cTexte.h"
#include "Parametres.h"



/*! \brief Types de prise en charge des handicap�s
 @ingroup m15
 \author Hugues Romain
 \date 2005
*/
class cHandicape
{
        const size_t _id;
        tBool3 _Valeur;  //!< Type de prise en charge (Accept�/Interdit/Inconnu)
        cModaliteReservation* _Resa;  //!< Modalit� de r�servation de la prise en charge si applicable (NULL sinon)
        tContenance _Contenance; //!< Nombre de places offertes aux handicap�s
        cTexte _Doc;  //!< Texte de description de la modalit�
        tPrix _Prix;  //!< Prix de l'utilisation du service par un handicap�

    public:
        //! \name Modificateurs
        //@{
        void setPrix( tPrix );
        void setDoc( const cTexte& );
        void setContenance( tContenance );
        void setResa( cModaliteReservation* );
        void setTypeHandicape( tBool3 );
        //@}

        //! \name Accesseurs
        //@{
        tContenance getContenance() const;
        tBool3 getTypeHandicape() const;
        const size_t& getId() const;
        //@}

        //! \name Constructeur et destructeur
        //@{
        cHandicape( const size_t& );
        ~cHandicape();
        //@}
};

#endif
