/*! \file cCommune.h
\brief En-tête classe commune
\date 2000-2005
*/



#ifndef SYNTHESE_CCOMMUNE_H
#define SYNTHESE_CCOMMUNE_H

#include "Point.h"
#include "LogicalPlace.h"
#include "Interpretor.h"


/** Commune
 @ingroup m15
 @author Hugues Romain
*/
class cCommune : public cPoint
{
    public:
        /** Interpréteur de nom de commune */
        typedef interpretor::Interpretor<LogicalPlace*> LogicalPlaceInterpretor;

    private:
        const std::string _name;   //!< Nom officiel de la commune
        const size_t _id;   //!< Index de la commune (non conservé en mémoire morte)
        LogicalPlaceInterpretor _logicalPlaces; //!< Tableau des lieux logiques avec recherche par réseau de neurone
        LogicalPlace _mainLogicalPlace; //!< Arrêts sélectionnés si lieu de départ non précisé
        LogicalPlace* _allPlaces;   //!< Arrêt tout lieu

    public:

        //! \name Accesseurs
        //@{
        const std::string& getName() const;
        const size_t& getId() const { return _id; }
        LogicalPlace* getMainLogicalPlace()
        {
            return & _mainLogicalPlace;
        }
        LogicalPlace* getAllPlaces() const
        {
            return _allPlaces;
        }


        //@}

        //! \name Constructeur et destructeur
        //@{
        cCommune( int, std::string );
        ~cCommune();
        //@}

        //! \name Modificateurs
        //@{
        void addLogicalPlace( LogicalPlace* const );
        void addToMainLogicalPlace( LogicalPlace* const );
        void setAtAllPlaces( LogicalPlace* const );
        //@}

        //! \name Calculateurs
        //@{
        std::vector<LogicalPlace*> searchLogicalPlaces( std::string, size_t ) const;
        //@}
};



/*! \brief Affichage standard d'une commune
 \author Hugues Romain
 \date 2005
*/

/*
template <class T>
inline T& operator<<( T& flux, const cCommune& Obj )
{
    flux << Obj.getName();
    return flux;
}

*/

#endif

