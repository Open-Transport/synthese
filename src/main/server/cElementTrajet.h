/*---------------------------------------*
|                                       |
|  APDOS / APISAPI - SYNTHESE v0.6      |
|  � Hugues Romain 2000/2003            |
|  cElementTrajet.h v2                  |
|  Header Classe Element Trajet         |
|                                       |
*---------------------------------------*/

#ifndef SYNTHESE_CELEMENTTRAJET_H
#define SYNTHESE_CELEMENTTRAJET_H

// Elements de trajets
class cElementTrajet;

/** @ingroup m33
*/
enum tTypeElementTrajet
{
    eTrajetAvecVoyageurs = 0,
    eTrajetAVide = 1,
    eTrajetVoirie = 2  // Trajetà pied sur voirie
};


#include "cLigne.h"
#include "cDistanceCarree.h"
#include "cArretPhysique.h"
#include "cAxe.h"

/** El�ment de trajet (pour r�sultats de calculs d'itin�raires)
 @ingroup m33
 \author Hugues Romain
 
Le pointeur suivant n'est utilis� que si l'�l�ment est int�gr� � un trajet. Dans le cas contraire, il peut �tre utilis� pour tout autre but, et sa valeur sera ignor�e lors de l'int�gration au trajet.
 */
class cElementTrajet
{
        friend class cTrajet;

    protected:
        //! \name Données
        //@{
        const NetworkAccessPoint* const _origin;   // Origine
        const NetworkAccessPoint* const _destination;  // Destination
        const synthese::time::DateTime vMomentDepart;    //!< Moment du d�part (premier si service continu)
        const synthese::time::DateTime vMomentArrivee;    //!< Moment d'arriv�e (premier si service continu)
        const size_t vNumeroService;    //!< Index du service utilis� au sein de la ligne
        const cLigne* const vLigne;      //!< Ligne utilis�e
        const tTypeElementTrajet vType;
        int vAmplitudeServiceContinu; //!< Amplitude du service continu (0 si service unique)
        //@}

        //! \name Chainages dans trajet (à virer)
        //@{
        cElementTrajet* vSuivant;     //!< Element de trajet suivant en correspondance
        cElementTrajet* _Precedent;     //!< Element de trajet precedent
        //@}

        //! \name Donn�es pr�calcul�es
        //@{
        const int vDureeEnMinutesRoulee;  //!< Temps pass� dans les circulations
        const cDistanceCarree vDistanceCarreeObjectif; //!< Carr� de la distance entre la destination de l'�l�ment et la destination finale du trajet (avec l'incertitude habituelle de ce type de donn�es)
        tDistanceM vDistanceParcourue;   //!< Distance lin�aire parcourue aucours du trajet
        //@}

    public :

        //! \name Accesseurs
        //@{
        const int& AmplitudeServiceContinu() const;
        const cAxe* Axe() const;
        const cDistanceCarree& getDistanceCarreeObjectif() const;
        tDistanceM DistanceParcourue() const;
        const int& DureeEnMinutesRoulee() const;
        const cTrain* getService() const;
        const cLigne* getLigne() const;
        const synthese::time::DateTime& MomentArrivee() const;
        const synthese::time::DateTime& MomentDepart() const;
        const cElementTrajet* Precedent() const;
        tTypeElementTrajet Type() const;
        const NetworkAccessPoint* getOrigin() const { return _origin; }
        const NetworkAccessPoint* getDestination() const { return _destination; }
        //@}

        //! \name Accesseurs d'objet permettant la modification
        //@{
        cDistanceCarree& getDistanceCarreeObjectif();
        cElementTrajet* getSuivant() const;
        //@}

        //! \name Operateurs
        //@{
        cElementTrajet* operator += ( cElementTrajet* );
        //@}

        //! \name Calculateurs
        //@{
        //cElementTrajet* Imite(const synthese::time::DateTime& MomentDepart, const synthese::time::DateTime& ArriveeMax) const;
        //@}

        //! \name Constructeur et destructeur
        //@{
        cElementTrajet( const NetworkAccessPoint* const _origin,
                        const NetworkAccessPoint* const _destination,
                        const synthese::time::DateTime& vMomentDepart,
                        const synthese::time::DateTime& vMomentArrivee,
                        const size_t& vNumeroService,
                        const cLigne* const vLigne,
                        const tTypeElementTrajet vType,
                        int amplitude,
                        const cDistanceCarree& squaredDistanceFromGoal
                      );
        ~cElementTrajet();
        //@}

        //! \name Modificateurs
        //@{
        cElementTrajet* deleteChainageColonne();
        void setAmplitudeServiceContinu( const int& newVal );
        void setSuivant( cElementTrajet* newVal );
        //@}

        //! \name Fonctions d'affichage
        //@{
        template <class T>
        T& toString( T& Objet ) const
        {
            Objet << this->getGareArrivee()
            << " Q" << TXT( this->VoieArrivee() ) << " ("
            << this->MomentArrivee()
            << ") \n";

            return ( Objet );
        }

        template <class T>
        T& toTrace( T& Objet ) const
        {
            Objet << this->getLigne() ->getCode()
            << " : " << this->getGareDepart()
            << "(" << this->MomentDepart()
            << ") � " << this->getGareArrivee()
            << "(" << this->MomentArrivee()
            << ")\n";

            if ( vSuivant != NULL )
                vSuivant->toTrace( Objet );

            return ( Objet );
        }

        /*template <class T> T& toXML(T& Objet, synthese::time::DateTime momentDepart, const int& amplitudeServiceContinu) const
        {
         synthese::time::DateTime debutArret, finArret;

         Objet << "\n<elementtrajet>";
         Objet << "\n<depart>";

         debutArret= this->MomentDepart();
         finArret = debutArret;
         if (amplitudeServiceContinu.Valeur())
         {
          finArret += amplitudeServiceContinu;
         }

         this->getGareDepart()->toXML(Objet, debutArret, finArret);
         this->MomentDepart().toXML(Objet,"date");
         Objet << "\n</depart>";
         Objet << "\n<arrivee>";
         
         debutArret = this->MomentArrivee();
         finArret = debutArret;
         if (amplitudeServiceContinu.Valeur())
         {
          finArret += amplitudeServiceContinu;
         } 
         this->getGareArrivee()->toXML(Objet, debutArret, finArret);
         
         this->MomentArrivee().toXML(Objet,"date");
         Objet << "\n</arrivee>";
         
         //SET & FMA : FE1.0.026 : Rajout du calcul de la fin de l'intervalle (ligne)  
         synthese::time::DateTime debutLigne, finLigne;
         debutLigne = this->MomentDepart();
         finLigne = this->MomentArrivee();
         if (amplitudeServiceContinu.Valeur())
         {
          finLigne += amplitudeServiceContinu;
         }
         
         this->getLigne()->toXML(Objet, this->getService(), momentDepart, debutLigne, finLigne);

         Objet << "<servicecontinu actif=\"";
         if (AmplitudeServiceContinu().Valeur())
         {
          Objet << "1\">";
          synthese::time::DateTime tempMoment = MomentDepart();
          tempMoment += AmplitudeServiceContinu();
          tempMoment.toXML(Objet,"datedernier");
         }
         else
          Objet << "0\">";

         Objet << "</servicecontinu>";
         Objet << "\n</elementtrajet>";

         return(Objet);
        }*/ 
        //@}


};



//! \name Fonctions de comparaisons entre ElementTrajet au format accept� par QSort
//@{
int CompareUtiliteETPourMeilleurDepart( const void* ET1, const void* ET2 );
int CompareUtiliteETPourMeilleureArrivee( const void* ET1, const void* ET2 );
//@}s



#endif
