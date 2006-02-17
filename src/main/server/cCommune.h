/*! \file cCommune.h
 \brief En-tête classe commune
 \date 2000-2005
*/



#ifndef SYNTHESE_CCOMMUNE_H
#define SYNTHESE_CCOMMUNE_H

#include "Point.h"
#include "cTexte.h"
#include "Interpretor.h"

class cEnvironnement;

class LogicalPlace;

using namespace interpretor;

/** Commune
 @ingroup m05
 @author Hugues Romain
*/
class cCommune : public cPoint
{
 private:
    const std::string       _name;   //!< Nom officiel de la commune
    const tIndex       _id;   //!< Index de la commune (non conservé en mémoire morte)
    Interpretor<LogicalPlace*> _logicalPlaces; //!< Tableau des lieux logiques avec recherche par réseau de neurone
    LogicalPlace    _mainLogicalPlace; //!< Arrêts sélectionnés si lieu de départ non précisé
    LogicalPlace*     _allPlaces;   //!< Arrêt tout lieu
    const cEnvironnement& _environment;  //!< L'environnement
					 //contenant cette commune
  
public:

    //! \name Accesseurs
    //@{
    const std::string& getName()   const;
    tIndex  getId()   const;
    LogicalPlace* getMainLogicalPlace()
    {
        return &_mainLogicalPlace;
    }
    LogicalPlace* getAllPlaces() const
    {
        return _allPlaces;
    }

    const cEnvironnement& getEnvironment () const { return _environment; }


    //@}

    //! \name Constructeur et destructeur
    //@{
    cCommune(const cEnvironnement& environment, tIndex, std::string);
    ~cCommune();
    //@}

    //! \name Modificateurs
    //@{
    void addLogicalPlace(const LogicalPlace*);
    void addToMainLogicalPlace(const LogicalPlace*);
    void setAtAllPlaces(const LogicalPlace*);
    //@}

    //! \name Calculateurs
    //@{
    vector<LogicalPlace*> searchLogicalPlaces(std::string, size_t) const;
    //@}
};



/*! \brief Affichage standard d'une commune
 \author Hugues Romain
 \date 2005
*/
template <class T>
inline T& operator<<(T& flux, const cCommune& Obj)
{
    flux << Obj.getName();
    return flux;
}

#endif
