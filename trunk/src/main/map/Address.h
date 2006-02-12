/** En-tête classe adresse.
 @file Address.h
*/


#ifndef SYNTHESE_MAP_ADDRESS_H_
#define SYNTHESE_MAP_ADDRESS_H_

#include <vector>
#include <set>
#include "NetworkAccessPoint.h"

namespace synmap
{


class PhysicalStop;
class Road;
class RoadChunk;


/** Adresse (rue + position métrique)
 
 L'adresse est un positionnement sur une rue déterminé par une position métrique de puis l'origine de la rue.
 
 Le numéro d'adresse est une donnée de désignation de l'adresse, mais peut également suffire à déterminer la position dès lors que la rue possède les informations nécessaires.
*/
class Address : public NetworkAccessPoint
{
public:

	typedef enum { RIGHT_SIDE, LEFT_SIDE, UNKNOWN_SIDE } AddressNumberSide;

	/** Numéro d'adresse.
		Ex : 70 rue de Paris : le numéro d'adresse est 70.
	*/
	typedef double AddressNumber;

	/** Constante numéro d'adresse inconnu.
	*/
	static const AddressNumber UNKNOWN_ADDRESS_NUMBER;

	


    /** Chemin d'accès aux arrêts physiques depuis l'adresse */
    typedef std::pair< std::vector<const RoadChunk*>, const PhysicalStop*> PathToPhysicalStop;

protected:

    //! @name Localisation
    //@{
    Road* const      _road;    //!< Route sur laquelle se trouve l'adresse
    const AddressNumber _number; //!< Numéro de l'adresse
    double       _metricOffset;  //!< Point métrique de l'adresse sur la rue
    //@}


public:

    //! @name Constructeur et destructeur
    //@{
    Address(Road* road, AddressNumber number = UNKNOWN_ADDRESS_NUMBER);
    ~Address();
    //@}

    //! @name Calculateurs
    //@{
    std::set
      < std::pair<double, const PhysicalStop*> >
      findDistancesToPhysicalStops (double maxDistance) const;

    std::set
        < PathToPhysicalStop >
        findPathsToPhysicalStops (double maxDistance) const;

		NetworkAccessPoint::AddressList getAddresses(bool forDeparture) const;

    //@}

    //! @name Accesseurs
    //@{
    Road* getRoad() const
    {
        return _road;
    }




    Address::AddressNumber getAddressNumber() const
    {
        return _number;
    }
    //@}
};

}

#endif
