/*! \file cTrain.h
\brief En-t�te classe cTrain
*/

#ifndef SYNTHESE_CTRAIN_H
#define SYNTHESE_CTRAIN_H

#include <string>


class cTrain;
class cLigne;
class cSitPert;
class cJC;

namespace synthese
{
namespace time
{
  class Date;
  class DateTime;
  class Schedule;
  
}
}

/** Impl�mentation de la notion de @ref defCirculation
 @ingroup m15
*/
class cTrain
{
        int vAttente;   //!< Attente maximale (service continu)
        int vEtalementCadence; //!< Dur�e de la plage de service continu
        cJC* vCirculation;  //!< Calendrier de circulation
        std::string vNumero;   //!< Num�ro officiel de la circulation
        bool vEstCadence;  //!< Indique si la circulation est un service continu
        cSitPert* vSitPert;   //!< Lien vers la situation perturb�e applicable � la circulation (NULL = pas de situation perturb�e)
        cLigne* const vLigne;    //!< Ligne � laquelle appartient la circulation
        size_t vIndex;    //!< Index de la circulation dans la ligne
        synthese::time::Schedule* _HoraireDepart;  //!< Horaire de d�part de l'origine

    public:
        //! \name Accesseurs
        //@{
        const int& Attente() const;
        bool EstCadence() const;
        const int& EtalementCadence() const;
        cJC* getJC() const;
        const std::string& getNumero() const;
        cLigne* getLigne() const;
        const synthese::time::Schedule* getHoraireDepartPremier() const;
        //@}

        //! \name Constructeur et destructeur
        //@{
        cTrain( cLigne* const );
        ~cTrain();
        //@}

        //! \name Modificateurs
        //@{
        void setAmplitudeServiceContinu ( const int& newVal );
        void setAttente ( const int& newVal );
        void setNumero ( const std::string& );
        void setServiceContinu ();
        // cTrain* operator =  (const cTrain&);
        cJC* setJC ( cJC* );
        void setHoraireDepart ( synthese::time::Schedule* );
        //@}

        //! \name Calculateurs
        //@{
        bool ReservationOK( const synthese::time::DateTime& __MomentDepart, const synthese::time::DateTime& __MomentCalcul ) const;
        bool Circule( const synthese::time::Date& __DateDepart, int __JPlus ) const;
        //@}
};

#endif
