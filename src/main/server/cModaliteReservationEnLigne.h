/** En-t�te modalit� de r�servation avec possibilit� de r�server en ligne
@file cModaliteReservationEnLigne.h
*/


#ifndef SYNTHESE_CMODALITERESERVATIONENLIGNE_H
#define SYNTHESE_CMODALITERESERVATIONENLIGNE_H


#include "cModaliteReservation.h"
#include <string>
#include "Parametres.h"



/** Modalit� de r�servation permettant la r�servation en ligne
 \author Hugues Romain
 \date 2005
 @ingroup m15
*/
class cModaliteReservationEnLigne : public cModaliteReservation
{
    public:

        /** Field needs for formular building in interface module */
        typedef enum
        {
            FieldNeed_NO_FIELD = 0,    //!< The field is not used
            FieldNeed_OPTIONAL = 1,    //!< The field is drawn, the filling of a value is optional
            FieldNeed_COMPULSORY = 2  //!< The field is drawn, the filling of a value is compulsory
    } FieldNeed;

        /** Level of seats occupation */
        typedef int OccupationLevel;

    private:
        //! \name Connexion Mnesia
        //@{
        std::string _Noeud;  //!< Noeud mnesia � contacter en local (serveur)
        std::string _Biscuit; //!< Passphrase commune � tous les noeuds du scope
        //@}

        //! \name Adresses d'envoi des �tats
        //@{
        std::string vEMail;  //!< Adresse e-mail du destinataire des mails d'�tats de r�servations (ex: le transporteur)
        std::string vEMailCopie; //!< Adresse e-mail de copie des mails d'�tats de r�servations (ex: l'organisme autoritaire)
        //@}

        //! \name Champs optionnels affich�s dans le formulaire
        //@{
        tBool3 vRELPrenom;   //!< Pr�nom du client (Indifferent = champ affich�, remplissage facultatif)
        tBool3 vRELAdresse;   //!< Adresse du client (Indifferent = champ affich�, remplissage facultatif)
        tBool3 vRELTelephone;   //!< Num�ro de t�l�phone du client (Indifferent = champ affich�, remplissage facultatif)
        tBool3 vRELNumeroAbonne;  //!< Num�ro d'abonn� du client (Indifferent = champ affich�, remplissage facultatif)
        tBool3 vRELEMail;   //!< Adresse e-mail du client (Indifferent = champ affich�, remplissage facultatif)
        //@}

        //! \name champs de gestion des places
        //@{
        int vMax;    //!< Nombre maximal de r�servations par service
        int* vSeuils;    //!< Paliers de nombre de r�servations g�n�rant un envoi de mail d'alerte
        //@}

    public:
        //! \name Accesseurs
        //@{
        const std::string& GetMail() const;
        const std::string& GetMailCopie() const;
        tBool3 GetRELPrenom() const;
        tBool3 GetRELAdresse() const;
        tBool3 GetRELEMail() const;
        tBool3 GetRELTel() const;
        tBool3 GetRELNumeroAbo() const;
        //@}

        //! \name Calculateurs
        //@{
        /* size_t  Disponibilite(const cTrain* Circulation, const synthese::time::DateTime&, cDatabaseManager* tBaseManager) const;
         tSeuilResa getSeuil(const int ancienneValeur, const int nouvelleValeur)       const;
         bool  Reserver(const cTrain* Service , const LogicalPlace* tPADepart, const LogicalPlace* tPAArrivee
              , const synthese::time::DateTime& DateDepart, const std::stringSQL& tNom, const std::stringSQL& tNomBrut
              , const std::stringSQL& tPrenom, const std::stringSQL& tAdresse, const std::stringSQL& tEmail
              , const std::stringSQL& tTelephone, const std::stringSQL& tNumAbonne
              , const std::stringSQL& tAdressePAArrivee, const std::stringSQL& tAdressePADepart
              , const size_t iNombrePlaces, cDatabaseManager* baseManager)    const;
         bool  Annuler(const std::string& CodeReservation, cDatabaseManager* tBaseManager)    const;
        */ //@}

        //! \name Modificateurs
        //@{
        bool addSeuil ( const OccupationLevel valeur, const size_t NombreSeuils = 0 );
        bool setMax ( const int valeur );
        bool SetRELAdresse ( const tBool3 );
        bool SetRELEMail ( const tBool3 );
        bool SetRELNumeroAbo ( const tBool3 );
        bool SetRELPrenom ( const tBool3 );
        bool SetRELTel ( const tBool3 );
        bool SetMail ( const std::string& newVal );
        bool SetMailCopie ( const std::string& newVal );
        //@}

        //! \name Constructeurs et destructeurs
        //@{
        cModaliteReservationEnLigne( const size_t& );
        ~cModaliteReservationEnLigne();
        //@}

};

#endif

