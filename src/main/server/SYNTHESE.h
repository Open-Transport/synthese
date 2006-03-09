/*! \file SYNTHESE.h
\brief En-tï¿½te classe principale de l'application SYNTHESE
*/

#ifndef SYNTHESE_SYNTHESE_H
#define SYNTHESE_SYNTHESE_H

class SYNTHESE;
class cInterface;
class cEnvironnement;
class cSite;
class cTableauAffichage;
class cCalculateur;
class TimeTables;

#include <string>
#include <vector>

#include "70_server/Request.h"
#include "cLog.h"

#include <boost/logic/tribool.hpp>


/** Application SYNTHESE
 @author Hugues Romain
 @date 2005
 @ingroup m10
*/
class SYNTHESE
{
    public:
        /** Environments map */
        typedef std::map<size_t, cEnvironnement*> EnvironmentsMap;

        /** Interfaces map */
        typedef std::map<size_t, cInterface*> InterfacesMap;

        /** Sites map */
        typedef std::map<std::string, cSite*> SitesMap;

        /** Departures tables map */
        typedef std::map<std::string, cTableauAffichage*> DeparturesTablesMap;

        /** TimeTables map */
        typedef std::map<std::string, TimeTables*> TimeTablesMap;

	

    private:

        //! \name Tableaux de donnï¿½es
        //@{
        EnvironmentsMap _Environnement;  //!< Environnements de donnï¿½es
        InterfacesMap _Interface;   //!< Interfaces d'affichage
        SitesMap _Site;    //!< Sites d'accï¿½s
        DeparturesTablesMap _TableauxAffichage; //!< Tableaux d'affichage
        TimeTablesMap _timeTables;  //!< Time tables collections
        //@}

        //! \name Fichier de log
        //@{
        tNiveauLog _NiveauLOG;    //!< Niveau de log
        std::string _CheminLOG;    //!< Rï¿½pertoire ou poser les fichiers de trace (vide = pas de trace)
	/* MJ review log
        cLog _FichierLogBoot;  //!< Fichier de log du boot
        cLog _FichierLogAcces;  //!< Fichier de log des accï¿½s
        // cLog   _FichierLogResa;  //!< Fichier de log des rï¿½servations


        std::vector<std::string> _MessageStandard;  //!< Messages standard
        std::vector<std::string> _CodesMessageStandard; //!< Codes Messages standard
        std::vector<tNiveauLog> _NiveauMessageStandard; //!< Messages standard

	*/



        std::map<long, cCalculateur*> _ThreadCalculateur; //!< Lien thread->calculateur
        friend void cLog::Ecrit( tNumeroMessageStandard, const std::string&, const std::string& );
        //@}

        //! \name Fonctions de gestion des logs
        //@{
        void OuvrirLogs();
        void ChargeMessagesStandard();
        //@}

        //! \name Fonctions pouvant ï¿½tre appelï¿½es dans une requï¿½te
        //@{
        bool FicheHoraire( std::ostream &pCtxt, std::ostream& pCerr, const cSite* __Site
                           , int NumeroGareOrigine, int NumeroGareDestination
                           , const synthese::time::Date& DateDepart, int codePeriode
                           , boost::logic::tribool velo, boost::logic::tribool handicape, boost::logic::tribool taxibus, int tarif
                           , long vThreadId );
        bool ListeCommunes( std::ostream &pCtxt, std::ostream& pCerr, const cSite* __Site
                            , bool depart, const std::string& Entree ) const;
        bool ListeArrets( std::ostream &pCtxt, std::ostream& pCerr, const cSite* __Site
                          , bool depart, const std::string& Commune, int NumeroCommune, const std::string& Arret ) const;
        bool Accueil( std::ostream &pCtxt, std::ostream& pCerr, const cSite* __Site ) const;
        bool ValidFH( std::ostream &pCtxt, std::ostream& pCerr, const cSite* __Site
                      , const std::string& txtCD, int nCD, int nAD, int nDD
                      , const std::string& txtCA, int nCA, int nAA, int nDA
                      , const std::string& txtAD, const std::string& txtAA
                      , const synthese::time::Date& DateDepart, int codePeriode
                      , boost::logic::tribool velo, boost::logic::tribool handicape, boost::logic::tribool taxibus, int tarif ) const;
        bool FormulaireReservation( std::ostream &pCtxt, std::ostream& pCerr, const cSite* __Site
                                    , const std::string& tCodeLigne, int iNumeroService
                                    , int iNumeroPADepart, int iNumeroPAArrivee, const synthese::time::Date& tDateDepart ) const;
        bool ValidationReservation( std::ostream &pCtxt, std::ostream& pCerr, const cSite* __Site
                                    , const std::string& CodeLigne
                                    , int NumeroService, int iNumeroPADepart, int iNumeroPAArrivee
                                    , const synthese::time::Date& tDateDepart, const std::string& tNom
                                    , const std::string& tPrenom, const std::string& tAdresse, const std::string& tEmail
                                    , const std::string& tTelephone, const std::string& tNumAbonne
                                    , const std::string& tAdressePADepart, const std::string& tAdressePAArrivee
                                    , int iNombrePlaces ) const;
        bool AnnulationReservation( std::ostream &pCtxt, std::ostream& pCerr, const cSite* __Site
                                    , const std::string& CodeReservation, const std::string& Nom ) const;
        bool TableauDepartsGare( std::ostream& pCtxt, std::ostream& pCerr, const cTableauAffichage*
                                 , const synthese::time::DateTime& ) const;
        //@}

    public:
        //! \name Exï¿½cution de requï¿½te
        //@{
        bool ExecuteRequete( std::ostream& pCtxt, std::ostream& pCerr, synthese::server::Request&, long vThreadId );
        //@}
        //! \name Terminaison forcée d'un calculateur
        //@{
        bool TermineCalculateur( long vThreadId );
        //@}

        //! \name Mï¿½thodes d'enregistrement
        //@{
        bool Enregistre( cSite* );
        bool Enregistre( cTableauAffichage* );
        bool Enregistre( cEnvironnement* );
        bool Enregistre( cInterface* );
        bool Enregistre( TimeTables* );
        //@}

        //! \name Modificateurs
        //@{
        // bool InitAssociateur(const std::string& NomAssociateur);
        bool Charge( const std::string& NomFichier );
        void SetNiveauLog( tNiveauLog );
        void SetCheminLog( const std::string& );
        //@}

        //! \name Accesseurs logs avec droit de modification
        //@{
/*
        cLog& FichierLogBoot();
        cLog& FichierLogAcces();
*/
        //@}

        //! \name Accesseurs
        //@{
        cEnvironnement* GetEnvironnement( size_t ) const;
        cInterface* GetInterface( size_t ) const;
        cSite* GetSite( const std::string& ) const;
        cTableauAffichage* GetTbDep( const std::string& ) const;
        TimeTables* getTimeTables( const std::string& ) const;
        tNiveauLog getNiveauLog() const;
        const std::string& getCheminLog() const;
        //@}

        //! \name Constructeur destructeur
        //@{
        SYNTHESE();
        ~SYNTHESE();
        //@}

};

/** @} */

#endif
