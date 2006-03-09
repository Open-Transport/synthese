
#include <string>

/** @ingroup m10 */
class cPersonne
{
        //! \name Coordonn�es de la personne
        //@{
        std::string
        _Prenom;    //!< Pr�nom du client
        std::string _Nom;
        //!< Nom du client
        std::string _Adresse;
        //!< Adresse du client
        std::string _NumeroAbonne;
        //!< Num�ro de l'abonnement du client
        std::string _Login;
        //!< Login du client
        std::string _Telephone;
        //!< Num�ro de t�l�phone du client
        std::string _EMail;
        //!< Adresse e-mail
        //@}



    public:

        //! \name Accesseurs
        //@{
        const std::string& GetCode();

        const cLigne* GetLigne();
        const std::string& GetCodeService();
        const std::string& GetPrenom();
        const std::string&
        GetNom();
        const std::string& GetAdresse();
        const std::string& GetNumeroAbonne();
        const std::string& GetTelephone();

        const std::string& GetEMail();
};

