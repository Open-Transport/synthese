/*! \file cLigne.h
\brief En-t�te classe cLigne
*/

#ifndef SYNTHESE_CLIGNE_H
#define SYNTHESE_CLIGNE_H

class LogicalPlace;
class cAxe;
class cGareLigne;
class cVelo;
class cHandicape;
class cReseau;
class cTrain;
class cMateriel;
class cModaliteReservation;
class cTarif;
class cArretPhysique;
class cEnvironnement;

#include <vector>
#include <string>
#include "cJourCirculation.h"
#include <string>
#include "cDistanceCarree.h"
#include "cAlerte.h"

/** Impl�mentation de la notion de @ref defLigne
 @ingroup m15
 @author Hugues Romain
 @date 2000-2003
*/
class cLigne
{
    public:

        /** Vector of Line-stops */
        typedef std::vector<cGareLigne*> LineStops;

        /** Vector of circulations */
        typedef std::vector<cTrain*> CirculationsVector;

    protected:
        //! @name Donn�es et chainages
        //@{
        CirculationsVector vTrain;    //!< Circulations
        cReseau* vReseau;   //!< R�seau de transport
        cAxe* const vAxe;    //!< Axe
        LineStops _lineStops; //!< arrets de la ligne
        cMateriel* vMateriel;   //!< Mat�riel roulant
        cModaliteReservation* vResa;    //!< Modalit� de r�servation
        cVelo* vVelo;    //!< Modalit� de prise en charge des v�los
        cHandicape* vHandicape;   //!< Modalit� d'acceptation des handicap�s
        cTarif* vTarif;    //!< Tarification
        cAlerte vAlerte;   //!< Message d'alerte � afficher
        //@}

        //! \name Identification de la ligne
        //@{
        std::string vLibelleSimple;  //!< Libell� mentionn� dans le cartouche
        std::string vLibelleComplet; //!< Libell� mentionn� dans la feuille de route
        std::string vStyle;    //!< Style CSS du cartouche
        std::string vImage;    //!< Image � afficher en tant que cartouche
        const std::string vCode;    //!< Code d'identification de la ligne
        std::string vNomPourIndicateur; //!< Libelle mentionne dans les tableaux d'indicateurs horaires
        std::string vGirouette;   //!< Destination affich�e sur les v�hicules
        //@}

        //! \name Parametres
        //@{
        bool vAAfficherSurTableauDeparts; //!< Indique si la ligne doit figurer sur les tableaux de d�part
        bool vAAfficherSurIndicateurs;  //!< Indique si la ligne doit figurer sur les tableaux d'indicateurs horaires
        bool _AUtiliserDansCalculateur;  //!< Indique si la ligne doit �tre utilis�e dans les recherches d'itin�raire
        cJC vCirculation;     //!< Calendrier de circulation de la ligne
        //@}

    public:
        //! \name Constructeurs et destructeur
        //@{
        cLigne( const std::string& newCode, cAxe* const newAxe, cEnvironnement* );
        ~cLigne();
        //@}

        //! \name Modificateurs
        //@{
        void addGareLigne( cGareLigne* );
        void addService( cTrain* const );
        void Ferme();
        void MajCirculation();
        cLigne* operator =( const cLigne& AutreLigne );
        void setAAfficherSurTableauDeparts( bool newVal );
        void setAAfficherSurIndicateurs( bool newVal );
        bool SetAUtiliserDansCalculateur( bool __Valeur );
        void setGirouette( const std::string& newGirouette );
        void setImage( const std::string& newImage );
        void setLibelleComplet( const std::string& newLibelleComplet );
        void setLibelleSimple( const std::string& newNom );
        void setMateriel( cMateriel* );
        void setNomPourIndicateur( const std::string& newNom );
        void setResa( cModaliteReservation* );
        void setReseau( cReseau* newReseau );
        // void    setServices(const std::string& TamponJC, const std::string& TamponAtt, const std::string& TamponDernier
        //        , size_t LargeurColonne, cEnvironnement* curEnv, ofstream& FichierLOG);
        void setStyle( const std::string& newStyle );
        void setVelo( cVelo* );
        void setHandicape( cHandicape* );
        void setTarif( cTarif* );
        cAlerte& getAlerteForModification();
        //@}

        // Copie de la ligne sans les services
        cLigne* Copie();
        // Nom libre pour une copie
        char* NomLibrePourCopie();

        //! \name Fonctions de modification de la base � chaud
        //@{
        // tNumeroService addServiceHoraire(const synthese::time::Schedule* tbHoraires, const std::string& newNumero, cJC* newCIS, tCodeBaseTrains newCodeBaseTrains=0, tNumeroService NombreNouveauxServices=1);
        bool allowAddServiceHoraire( const synthese::time::Schedule* ) const;
        bool Sauvegarde() const;
        // void    updateServiceHoraire(tNumeroService curNumeroService, const synthese::time::Schedule* tbHoraires, const std::string& newNumero, cJC* newCIS, tCodeBaseTrains newCodeBaseTrains=0);
        //@}

        //! \name Accesseurs
        //@{
        bool AAfficherSurTableauDeparts() const;
        bool AAfficherSurIndicateurs() const;
        cAxe* Axe() const;
        cGareLigne* DerniereGareLigne() const;
        bool EstUneLigneAPied() const;
        const cAlerte& getAlerte() const;
        const std::string& getCode() const;
        const std::string& getGirouette() const;
        cHandicape* getHandicape() const;
        const std::string& getImage() const;
        const std::string& getLibelleComplet() const;
        const std::string& getLibelleSimple() const;
        const std::string& getNomPourIndicateur() const;
        cModaliteReservation* GetResa() const;
        cReseau* getReseau() const;
        const std::string& getStyle() const;
        const cTarif* getTarif() const;
        const std::vector<cTrain*> getServices() const { return vTrain; }
        cTrain* getTrain( size_t ) const;
        cVelo* getVelo() const;
        cMateriel* Materiel() const;
        const LineStops& getLineStops() const { return _lineStops; }
        // std::string     Code()        const;
        //@}

        //! \name Calculateurs
        //@{
        bool PeutCirculer( const synthese::time::Date& ) const;
        //@}

        /* template <class T>
         T& toXML(T& Tampon, tNumeroService iNumeroService, synthese::time::DateTime momentDepart, synthese::time::DateTime debutLigne, synthese::time::DateTime finLigne) const
         {
          Tampon << "<ligne>";
          
          //ajout libelle de la ligne
          Tampon << "<libelle>";
          LibelleComplet(Tampon, false);
          Tampon <<"</libelle>";
          
          //ajout destination ligne
          Tampon <<"<destination>";
          synthese::time::DateTime pNull;
          this->PremiereGareLigne()->Destination()->ArretLogique()->toXML(Tampon, pNull, pNull, false);
          Tampon <<"</destination>";
          
          // ajout mat�riel
          vMateriel->toXML(Tampon);
          
          //ajout messages
          bool showMessage = getAlerte().showMessage(debutLigne, finLigne);
          bool showResa = GetResa()->TypeResa() == Obligatoire;
          if (showMessage || showResa)
          {
           Tampon << "<warnings>";
           
           if (showMessage)
            getAlerte().toXML(Tampon);
         
           if (showResa)
            GetResa()->toXML(Tampon, GetTrain(iNumeroService), momentDepart);
           
           Tampon << "</warnings>"; 
          }
          
          Tampon << "</ligne>";
          return(Tampon);
         }
         
        */

};


#endif

