/*! \file cGareLigne.h
\brief En-t�te Classe GareLigne
*/

#ifndef SYNTHESE_CGARELIGNE_H
#define SYNTHESE_CGARELIGNE_H

#include "cDistanceCarree.h"
#include <string>


class cArretPhysique;
class cLigne;
class cVelo;
class cHandicape;

namespace synthese
{
namespace time
{
  class Schedule;
  class DateTime;
}
}





/** Liaison entre points d'arr�t et lignes
 @ingroup m15
 \author Hugues Romain
 \date 2000-2003
 \version 2.0
*/
class cGareLigne
{
    public:
        enum tTypeGareLigneDA
        {
            Depart = 'D',
            Arrivee = 'A',
            Passage = 'P'
    };

    private:

        //! \name Data
        //@{
        cArretPhysique* const _physicalStop;   //!< Arrêt physique desservi par la gare ligne
        cLigne* const vLigne;      //!< Ligne desservant l'arr�t
        tDistanceM const vPM;      //!< Point m�trique de l'arr�t sur la ligne
        const bool vHorairesSaisis;    //!< Horaires avec ou sans saisie
        tTypeGareLigneDA vTypeDA;      //!< Gare de d�part, d'arriv�e, ou de passage
        //@}

        //! \name Variables Ligne
        //@{
        cGareLigne* vDepartPrecedent;    //!< Arr�t de passage au d�part pr�c�dent sur la ligne
        cGareLigne* vDepartCorrespondancePrecedent; //!< Arr�t de correspondance au d�part pr�c�dent sur la ligne
        cGareLigne* vArriveeSuivante;    //!< Arr�t de passage � l'arriv�e suivant sur la ligne
        cGareLigne* vArriveeCorrespondanceSuivante; //!< Arr�t de passage de destination suivant sur la ligne
        cGareLigne* _Precedent;      //!< Arr�t pr�c�dent ind�pendemment de sa desserte
        cGareLigne* vSuivant;      //!< Arr�t suivant ind�pendemment de sa desserte
        //@}

        //! \name Variables Horaires
        //@{
        synthese::time::Schedule* vHoraireDepartPremier;  //!< Horaires de d�part th�oriques (par circulation) Service continu : premiers d�parts de la plage
        synthese::time::Schedule* vHoraireDepartPremierReel; //!< Horaires de d�part r�els (par circulation)
        synthese::time::Schedule* vHoraireDepartDernier;  //!< Horaires de d�part th�oriques (par circulation) Service continu : derniers d�parts de la plage
        synthese::time::Schedule* vHoraireArriveePremier;  //!< Horaires d'arriv�e th�oriques (par circulation) Service continu : permi�res arriv�es de la plage
        synthese::time::Schedule* vHoraireArriveePremierReel; //!< Horaires d'arriv�e r�elles (par circulation)
        synthese::time::Schedule* vHoraireArriveeDernier;  //!< Horaires d'arriv�e th�oriques (par circulation) Service continu : derni�res arriv�es de la plage
        int vIndexDepart[ 24 ];   //!< Index des circulations selon l'heure de d�part th�orique
        int vIndexDepartReel[ 24 ];  //!< Index des circulations selon l'heure de d�part r�elle du jour
        int vIndexArrivee[ 24 ];   //!< Index des circulations selon l'heure d'arriv�e th�orique
        int vIndexArriveeReel[ 24 ];  //!< Index des circulations selon l'heure d'arriv�e r�elle du jour
        //@}

        //! \name Modificateurs
        //@{
        void AlloueHoraires(); //!< Alloue l'espace n�cessaire au stockage des horaires
        //@}

    public:

        //! \name Constructeurs et destructeurs
        //@{
        cGareLigne (
            cLigne*,
            tDistanceM,
            tTypeGareLigneDA,
            cArretPhysique* const,
            bool newHorairesSaisis
        );
        ~cGareLigne();
        //@}


        //! \name Modificateurs
        //@{
        void ChaineAvecHoraireSuivant( const cGareLigne& AHPrecedent, 
				       const cGareLigne& AHSuivant, 
				       size_t Position, 
				       size_t Nombre, 
				       int NumeroServiceATraiter = -1 );
        void EcritIndexDepart();
        void EcritIndexArrivee();
        void RealloueHoraires( size_t newService );
        void setArriveeCorrespondanceSuivante( cGareLigne* newVal );
        void setArriveeSuivante( cGareLigne* newVal );
        void setDepartCorrespondancePrecedent( cGareLigne* );
        void setDepartPrecedent( cGareLigne* newVal );
        void setHoraires( const std::string& Tampon, int Position, int LargeurColonne, bool DepartDifferentPassage );
        void setTypeDA( tTypeGareLigneDA );
        //@}

        void ValiditeSolution( size_t, synthese::time::DateTime& );

        //int Circule(tNumeroService, tTypeGareLigneDA, synthese::time::Date&);
        // Calculs d'heures sens depart -> arrivee (pour meilleure arriv�e)
        // bool HeureDepartDA(tNumeroService, synthese::time::DateTime &MomentDepart, synthese::time::Schedule& HoraireDepart);
        // void HeureArriveeDA(tNumeroService, synthese::time::DateTime& MomentArrivee, synthese::time::DateTime& MomentDepart, synthese::time::Schedule& HoraireDepart);
        // Calculs d'heures sens arriv�e -> d�part (pour meilleur d�part)
        // bool HeureArriveeAD(tNumeroService, synthese::time::DateTime &MomentDepart, synthese::time::Schedule& HoraireDepart);
        // void HeureDepartAD(tNumeroService, synthese::time::DateTime& MomentDepart, synthese::time::DateTime& MomentArrivee, synthese::time::Schedule& HoraireArrivee);

        //! \name Calculateurs
        //@{
        bool Circule( const synthese::time::DateTime& DateDebut, const synthese::time::DateTime& DateFin ) const;
        void CalculeArrivee( const cGareLigne& GareLigneDepart, size_t iNumeroService
                             , const synthese::time::DateTime& MomentDepart, synthese::time::DateTime& MomentArrivee ) const;
        // synthese::time::DateTime   CalculeArrivee(const cDescriptionPassage* curDP)          const;
        void CalculeDepart( const cGareLigne& GareLigneArrivee, 
			    size_t iNumeroService,
                            const synthese::time::DateTime& MomentArrivee, 
			    synthese::time::DateTime& MomentDepart ) const;

        bool CoherenceGeographique( const cGareLigne& AutreGareLigne ) const;
        // bool   ControleAxe(const cElementTrajet* ET, tBool3 velo,
        // tBool3 handicape, tBool3 taxibus, int codeTarif) const;

        bool controleHoraire( const cGareLigne* GareLigneAvecHorairesPrecedente ) const;

        int MeilleurTempsParcours( const cGareLigne& autreGL ) const;

        int Prochain( synthese::time::DateTime &MomentDepart, 
		      const synthese::time::DateTime& MomentDepartMax,
		      int& AmplitudeServiceContinu, int NumProchainMin,
		      const synthese::time::DateTime& __MomentCalcul ) const;

        int Prochain( synthese::time::DateTime &MomentDepart, 
		      const synthese::time::DateTime& MomentDepartMax,
                      const synthese::time::DateTime& __MomentCalcul,
                      int NumProchainMin = INCONNU ) const;

        int Precedent( synthese::time::DateTime &MomentArrivee, 
		       const synthese::time::DateTime& MomentArriveeMin,
                       int& AmplitudeServiceContinu ) const;

        int Precedent( synthese::time::DateTime &MomentArrivee, 
		       const synthese::time::DateTime& MomentArriveeMin ) const;


        // void   RemplitDP(cDescriptionPassage& curDP, const synthese::time::DateTime& tempMomentDepart
        //       , const tNumeroService& iNumeroService)          const;
        //@}

        //! \name Accesseurs
        //@{
        bool EstArrivee() const;
        bool EstDepart() const;
        cGareLigne* getArriveeCorrespondanceSuivante() const;
        cGareLigne* getArriveeSuivante() const;
        cGareLigne* getDepartCorrespondancePrecedent() const;
        cGareLigne* getDepartPrecedent() const;
        // const cGareLigne*  getLiaisonDirecteVers(const cAccesPADe*)  const;
        // const cGareLigne*  getLiaisonDirecteDepuis(const cAccesPADe*)  const;
        const synthese::time::Schedule& getHoraireArriveeDernier( size_t ) const;
        const synthese::time::Schedule& getHoraireArriveePremier( size_t ) const;
        const synthese::time::Schedule& getHoraireArriveePremierReel( size_t ) const;
        const synthese::time::Schedule& getHoraireDepartDernier( size_t ) const;
        const synthese::time::Schedule& getHoraireDepartPremier( size_t ) const;
        const synthese::time::Schedule& getHoraireDepartPremierReel( size_t ) const;
        bool HorairesSaisis() const;
        cLigne* Ligne() const;
        tDistanceM PM() const;
        cArretPhysique* ArretPhysique() const;
        tTypeGareLigneDA TypeDA() const;
        //@}
};




#endif

