/*! \file cResa.h
\brief En-tête classes Modalités de réservation
*/

#ifndef SYNTHESE_CRESA_H
#define SYNTHESE_CRESA_H

class cTrain;
class cModaliteReservationEnLigne;

#include "04_time/Hour.h"
#include "04_time/DateTime.h"


/*! \brief Modalités de réservation
 \author Hugues Romain
 \date 2001-2005
 @ingroup m15
*/
class cModaliteReservation
{
    public:

        /** Type of reservation rule */
        typedef enum
        {
            RuleType_IMPOSSIBLE = 'N',     //!< Reservation is not possible, services should be taken without any announcement
            RuleType_OPTIONNAL = 'F',     //!< Reservation is possible but not compulsory, services should be taken with announcement for much security
            RuleType_COMPULSORY = 'O',     //!< Reservation is compulsory, passengers without reservation are not accepted
            RuleType_AT_LEAST_ONE_REQUIRED = 'C' //!< At least on reservation is compulsory, the service do not go if no reservations are done, but will go if at least one is done, and will bi opened for passengers without reservation
    } RuleType;


    protected:
        //! \name Caractéristiques
        //@{
        RuleType vTypeResa;     //!< Type de réservation (possible, obligatoire, impossible)
        bool vReservationEnLigne;  //!< Réservation via SYNTHESE active
        //tPrix   vPrix;     //!< Coût de la réservation (inutilisé pour l'instant)
        //@}

        //! \name Chainage et indexation
        //@{
        const size_t _id;    //!< Numéro de la modalité de réservation dans l'environnement
        //@}

        //! \name Délai de réservation
        //@{
        bool vReferenceEstLOrigine; //!< Indique si le moment de départ de référence est celle du départ de la circulation à son origine (true) ou bien s'il s'agit du moment de départ du voyageur (false)
        int vDelaiMinMinutes;  //!< Délai minimal en minutes entre la réservation et le moment de référence
        int vDelaiMinJours;   //!< Délai minimal en jours entre la réservation et le moment de référence
        synthese::time::Hour vDelaiMinHeureMax;  //!< Heure maximale de réservation le dernier jour de la période d'ouverture à la réservation
        int vDelaiMaxJours;  //!< Nombre maximal de jours entre la réservation et le départ du service
        //@}

        //! \name Coordonnées de contact
        //@{
        std::string vNumeroTelephone;  //!< Numéro de téléphone de la centrale de réservation
        std::string vHorairesTelephone;  //!< Description des horaires d'ouverture de la centrale de réservation
        std::string vDescription;   //!< Informations complémentaires sur la centrale de réservation ou la modalité de réservation
        std::string vSiteWeb;    //!< URL d'un site web permettant de réserver le service en ligne (incompatible avec la REL)
        //@}

    public:

        //! \name Accesseurs
        //@{
        const RuleType& TypeResa() const;
        const synthese::time::Hour& GetDelaiMinHeureMax() const;
        const std::string& GetHorairesTelephone() const;
        const std::string& GetSiteWeb() const;
        const std::string& GetTelephone() const;
        const cModaliteReservationEnLigne* ReservationEnLigne() const;
        const size_t& Index() const;
        /*template <class T>
        T&    toXML(T& Tampon, const cTrain*, synthese::time::DateTime momentDepart) const;*/ 
        //@}

        //! \name Calculateurs
        //@{
        synthese::time::DateTime momentLimiteReservation ( const cTrain*, const synthese::time::DateTime& MomentDepart ) const;
        synthese::time::DateTime momentDebutReservation ( const synthese::time::DateTime& ) const;
        bool circulationPossible ( const cTrain*, const synthese::time::DateTime& MomentResa, const synthese::time::DateTime& MomentDepart ) const;
        bool reservationPossible ( const cTrain*, const synthese::time::DateTime& MomentResa, const synthese::time::DateTime& MomentDepart ) const;
        //@}

        //! \name Modificateurs
        //@[
        bool SetDelaiMaxJours ( const int );
        void setDelaiMaxJours ( const int );
        bool SetDelaiMinHeureMax ( const synthese::time::Hour& );
        void setDelaiMinHeureMax ( const synthese::time::Hour& );
        bool SetDelaiMinJours ( const int );
        void setDelaiMinJours ( const int );
        bool SetDelaiMinMinutes ( const int );
        void setDelaiMinMinutes ( const int& );
        bool SetDoc ( const std::string& );
        bool SetHorairesTel ( const std::string& );
        //bool SetPrix    (const float);
        //void setPrix    (const tPrix);
        bool SetSiteWeb ( const std::string& );
        bool SetTel ( const std::string& );
        bool SetTypeResa ( const char );
        void setTypeResa ( const RuleType& );
        bool setReferenceEstLOrigine ( const bool );
        //@}

        //! \name Constructeurs et destructeurs
        //@{
        cModaliteReservation( const size_t& );
        virtual ~cModaliteReservation();
        //@}
};

#endif

