
#ifndef SYNTHESE_NETWORKACCESSPOINT_H
#define SYNTHESE_NETWORKACCESSPOINT_H

#include "Point.h"
#include <string>

#include <vector>
#include <utility>


class LogicalPlace;

namespace synmap
{
class Address;
}


/** @ingroup m15 */
class NetworkAccessPoint : public cPoint
{
    public:


        /** Liste d'adresses avec durÃ©e d'accÃ¨s */
        /** Accès à adresse avec durée */
        typedef std::pair<synmap::Address*, int> AddressWithAccessDuration;
        typedef std::vector<AddressWithAccessDuration> AddressList;

    private:
        std::string _name; //!< Nom
        LogicalPlace* const _logicalPlace; //!< Lieu logique
        const size_t _rank;   //!< Position dans le lieu logique

    public:
        //! @name Accesseurs
        //@{
        const std::string& getNom() const;
        LogicalPlace* getLogicalPlace() const { return _logicalPlace; }
        size_t getRankInLogicalPlace() const { return _rank; }
        //@}

        //! @name Calculateurs
        //@{
        //virtual AddressList getAddresses(bool forDeparture) = 0;
        //@}

        //! @name Modificateurs
        //@{
        void setNom( const std::string& );
        //@}

        //! @name Constructeur et destructeur
        //@{
        NetworkAccessPoint( LogicalPlace* const, size_t );
        ~NetworkAccessPoint();
        //@}

};

#endif

