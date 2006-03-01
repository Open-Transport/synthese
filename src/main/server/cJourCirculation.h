/*--------------------------------------*
|                                      |
|  APDOS - SYNTHESE v0.6               |
|  © Hugues Romain 2000/2002           |
|  CJourCirculation.cpp                |
|  Header Classes JC                   |
|                                      |
*--------------------------------------*/

#ifndef SYNTHESE_CJOURCIRCULATION_H
#define SYNTHESE_CJOURCIRCULATION_H



#include "04_time/Year.h"
#include "04_time/Date.h"
#include <string>
#include <vector>

/** Calendrier de circulation
 @ingroup m15
 
 Un tableau de 32 bits est créé par mois, stocké dans un int. Le bit de poids faible correspond au jour 1, le bit de poinds fort au jour 32 (inutilisé).
 */
class cJC
{
    public:

        /** Type of inclusion */
        typedef enum
        {
            InclusionType_POSITIVE = '+',
            InclusionType_NEGATIVE = '-'
    } InclusionType;

        /** Mask */
        typedef unsigned long Mask;

        /** Calendar */
        typedef std::vector<Mask> Calendar;

        /** Category */
        typedef unsigned char Category;

        /** Max category */
        static const Category Category_MAX = 255;

    private:
        Calendar vJoursAnnee;   //!< Tableau contenant les bits correspondant à chaque jour (l'élement 0 est inutilisé)
        const synthese::time::Year _PremiereAnnee;  //!< Année réelle correspondant au premier octet du masque
        const synthese::time::Year _DerniereAnnee;  //!< Année réelle correspondant au dernier octet du masque
        Category vCategorie;
        std::string vIntitule;
        const size_t _id;

        size_t getIndexMois( const synthese::time::Date& __Date ) const;
        size_t getIndexMois( int __Annee, int __Mois ) const;

    public:

        //! \name Constructeurs, fonctions de construction, et destructeur
        //@{
        cJC( const int&, const int&, const size_t&, const std::string& );
        cJC( const cJC& );
        ~cJC();
        //@}

        //! \name Modificateurs
        //@{
        void RAZMasque( bool ValeurBase = false );
        void setCategorie( Category newCategorie = Category_MAX ); //!< Defaut = Reset
        bool SetCircule( const synthese::time::Date&, InclusionType Sens = InclusionType_POSITIVE );
        void setCircule( const synthese::time::Date&, InclusionType Sens = InclusionType_POSITIVE );
        bool SetCircule( const synthese::time::Date& DateDebut, const synthese::time::Date& DateFin, InclusionType Sens = InclusionType_POSITIVE, int Pas = 1 );
        void setIntitule( const std::string& Texte );
        void setMasque( const Calendar& );
        //@}

        //! \name Calculateurs
        //@{
        void SetInclusionToMasque( Calendar&, InclusionType Sens = InclusionType_POSITIVE ) const;
        void SetInclusionToMasque( cJC&, InclusionType Sens = InclusionType_POSITIVE ) const;
        bool TousPointsCommuns( const cJC&, const Calendar& ) const; //!< Sur le masque de this uniquement: ce n'est pas l'égalité
        bool UnPointCommun( const Calendar& ) const;
        bool UnPointCommun( const cJC& ) const;
        size_t Card( const Calendar& ) const; //!< Sur le masque de this uniquement
        size_t Card( const cJC& ) const; //!< Sur le masque de this uniquement
        bool Circule( const synthese::time::Date& ) const;
        Calendar ElementsNonInclus( const cJC& AutreJC ) const;
        Calendar Et( const cJC& ) const;
        //@}

        //! \name Accesseurs
        //@{
        const Category& Categorie() const;
        const size_t& getId() const;
        const Calendar& JoursAnnee() const;
        synthese::time::Date PremierJourFonctionnement() const;
        //@}

        //! \name Affichage
        //@{
        template <class T>
        T& Intitule( T& Objet ) const
        {
            return ( Objet << vIntitule );
        }
        //@}
};



#endif
