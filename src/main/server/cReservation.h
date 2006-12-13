
#ifndef SYNTHESE_CRESERVATION_H
#define SYNTHESE_CRESERVATION_H

class cReservation;

#include "cModaliteReservation.h"
#include "LogicalPlace.h"
#include "cAxe.h"
#include <string>

#include "cPersonne.h"
#include "cProfil.h"
#include "cModaliteReservationEnLigne.h"

/** R�servation de transport � la demande
 @ingroup m31
*/
class cReservation
{
        //! \name Service r�serv�
        //@{
        cAxe* _AxeLigne;    //!< Axe de la ligne � la demande
        std::string _CodeService;   //!< Code du service au sein l'axe
        LogicalPlace* _ArretLogiqueMontee;  //!< Code du point d'arr�t de mont�e
        LogicalPlace* _ArretLogiqueDescente; //!< Code du point d'arr�t de descente
        cModaliteReservationEnLigne* _ModaliteReservation; //!< Code de la modalit� de reservation
        std::string _AdresseDepart;   //!< Adresse du d�part
        std::string _AdresseArrivee;  //!< Adresse d'arriv�e
        synthese::time::DateTime _MomentDepart;   //!< Moment de mont�e pr�vu
        synthese::time::DateTime _MomentArrivee;   //!< Moment d'arriv�e pr�vue
        //@}

        //! \name Caract�ristiques de la r�servation
        //@{
        std::string _Code;     //!< Code � 7 lettres d'identification unique
        std::string _CodePredecesseur;  //!< Code de la r�servation annul�e en cas de modification
        size_t _NombrePlaces;   //!< Nombre de places
        synthese::time::DateTime _MomentReservation;  //!< Date de la r�servation
        bool _Annulee;    //!< R�servation annul�e?
        synthese::time::DateTime _MomentAnnulation;  //!< Date de l'annulation
        //@}

        //! \name Personnes
        //@{
        cPersonne* _Personne;
        cProfil* _PersonneSaisie;
        cProfil* _PersonneAnnulation;
        //@}

    public:
        //! \name Modificateurs
        //@{
        void SetValeurs( const cTrain* Service , 
			 const LogicalPlace* tPADepart, 
			 const LogicalPlace* tPAArrivee,
                         const synthese::time::DateTime& DateDepart, 
			 const std::string& tNom, 
			 const std::string& tNomBrut,
                         const std::string& tPrenom, 
			 const std::string& tAdresse, 
			 const std::string& tEmail,
                         const std::string& tTelephone, 
			 const std::string& tNumAbonne,
                         const std::string& tAdressePAArrivee, 
			 const std::string& tAdressePADepart,
                         const size_t iNombrePlaces );

        void SetCode( const std::string& __Code );
        //@}

        //! \name Modificateurs
        //@{
        bool Reserver();
        bool Annuler();
        bool Charger();
        //@}

        //! \name Accesseurs
        //@{
        const synthese::time::DateTime& MomentDepart() const;
        const synthese::time::DateTime& MomentArrivee() const;
        const cPersonne* GetPersonne() const;
        bool GetAnnulee() const;
        //@}

        cReservation();
        ~cReservation();
};

#endif


