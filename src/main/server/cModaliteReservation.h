/*! \file cResa.h
\brief En-t�te classes Modalit�s de r�servation
*/

#ifndef SYNTHESE_CRESA_H
#define SYNTHESE_CRESA_H

class cTrain;
class cModaliteReservationEnLigne;

#include "04_time/Hour.h"
#include "04_time/DateTime.h"


/*! \brief Modalit�s de r�servation
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
        //! \name Caract�ristiques
        //@{
        RuleType vTypeResa;     //!< Type de r�servation (possible, obligatoire, impossible)
        bool vReservationEnLigne;  //!< R�servation via SYNTHESE active
        //tPrix   vPrix;     //!< Co�t de la r�servation (inutilis� pour l'instant)
        //@}

        //! \name Chainage et indexation
        //@{
        const size_t _id;    //!< Num�ro de la modalit� de r�servation dans l'environnement
        //@}

        //! \name D�lai de r�servation
        //@{
        bool vReferenceEstLOrigine; //!< Indique si le moment de d�part de r�f�rence est celle du d�part de la circulation � son origine (true) ou bien s'il s'agit du moment de d�part du voyageur (false)
        int vDelaiMinMinutes;  //!< D�lai minimal en minutes entre la r�servation et le moment de r�f�rence
        int vDelaiMinJours;   //!< D�lai minimal en jours entre la r�servation et le moment de r�f�rence
        synthese::time::Hour vDelaiMinHeureMax;  //!< Heure maximale de r�servation le dernier jour de la p�riode d'ouverture � la r�servation
        int vDelaiMaxJours;  //!< Nombre maximal de jours entre la r�servation et le d�part du service
        //@}

        //! \name Coordonn�es de contact
        //@{
        std::string vNumeroTelephone;  //!< Num�ro de t�l�phone de la centrale de r�servation
        std::string vHorairesTelephone;  //!< Description des horaires d'ouverture de la centrale de r�servation
        std::string vDescription;   //!< Informations compl�mentaires sur la centrale de r�servation ou la modalit� de r�servation
        std::string vSiteWeb;    //!< URL d'un site web permettant de r�server le service en ligne (incompatible avec la REL)
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

