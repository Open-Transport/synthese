
#ifndef SYNTHESE_CRESULTATBASEDEDONNEES_CELLULE_H
#define SYNTHESE_CRESULTATBASEDEDONNEES_CELLULE_H

#include <string>
#include <sstream>

#include "01_util/Conversion.h"
#include "04_time/DateTime.h"

/** @ingroup m02 */
class cResultatBaseDeDonnees_Cellule
{
    protected:

    public:
        virtual int getNombre() const { return INCONNU; }
        virtual std::string getTexte() const { std::string __Texte; return __Texte; }
        virtual synthese::time::DateTime getMoment() const { return synthese::time::DateTime(); }
        virtual tBool3 getBool() const { return Indifferent; }
};



/** @ingroup m02 */
class cResultatBaseDeDonnees_Cellule_Nombre : public cResultatBaseDeDonnees_Cellule
{
    protected:
        int _Valeur; //!< Valeur entière

    public:

        //! \name Accesseurs
        //@{
        int getNombre() const { return _Valeur; }
        
	std::string getTexte() const { 
	    std::stringstream ss;
	    ss << _Valeur; 
	    return ss.str (); 
	}

tBool3 getBool() const { return _Valeur == INCONNU ? Indifferent : ( _Valeur > 0 ? Vrai : Faux ); }
        //@}

        /*! \brief Constructeur
         CRO : Ce constructeur peut être modifié en fonction des besoins
        */
        cResultatBaseDeDonnees_Cellule_Nombre( int __Valeur ) { _Valeur = __Valeur; }
};



/** @ingroup m02 */
class cResultatBaseDeDonnees_Cellule_Texte : public cResultatBaseDeDonnees_Cellule
{
    protected:
        std::string _Valeur; //!< Valeur texte

    public:

        //! \name Accesseurs
        //@{
        int getNombre() const { return synthese::util::Conversion::ToInt (_Valeur); }
        std::string getTexte() const { return _Valeur; }
        //@}

        /*! \brief Constructeur
         CRO : Ce constructeur peut être modifié en fonction des besoins
        */
        cResultatBaseDeDonnees_Cellule_Texte( const std::string& __Valeur ) { _Valeur = __Valeur; }
};



/** @ingroup m02 */
class cResultatBaseDeDonnees_Cellule_Moment : public cResultatBaseDeDonnees_Cellule
{
    protected:
        synthese::time::DateTime _Valeur; //!< Valeur moment

    public:

        //! \name Accesseurs
        //@{
        synthese::time::DateTime getMoment() const { return _Valeur; }
        //@}

        /*! \brief Constructeur
        */
        cResultatBaseDeDonnees_Cellule_Moment( const synthese::time::DateTime& __Valeur ) { _Valeur = __Valeur; }
};

#endif
