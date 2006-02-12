/** En-tête classe adresse.
	@file Address.h
*/


#ifndef SYNTHESE_MAP_ADDRESS_H_
#define SYNTHESE_MAP_ADDRESS_H_

#include "RoadChunk.h"
#include "Road.h"
#include <vector>


namespace synmap
{


class PhysicalStop;
class Road;


/** Adresse (rue + position métrique)

	L'adresse est un positionnement sur une rue déterminé par une position métrique de puis l'origine de la rue.

	Le numéro d'adresse est une donnée de désignation de l'adresse, mais peut également suffire à déterminer la position dès lors que la rue possède les informations nécessaires.
*/
class Address
{
public:

	/** Chemin d'accès aux arrêts physiques depuis l'adresse */
	typedef std::pair<Road::RoadChunkVector, const PhysicalStop*> PathToPhysicalStop;

protected:

	//!	@name Localisation
	//@{
		Road* const						_road;				//!< Route sur laquelle se trouve l'adresse
		const RoadChunk::AddressNumber	_address_number;	//!< Numéro de l'adresse
		double							_metric_offset;		//!< Point métrique de l'adresse sur la rue
	//@}


public:

	//!	@name Constructeur et destructeur
	//@{
		Address(Road* __road, RoadChunk::AddressNumber __address_number = RoadChunk::ADDRESS_NUMBER_UNKNOWN);
		~Address();
	//@}

	//!	@name Calculateurs
	//@{
		std::set< PathToPhysicalStop >
	    findPathsToPhysicalStops (double distance) const;

	//@}

	//!	@name Accesseurs
	//@{
		Road* getRoad()	const { return _road; }
		RoadChunk::AddressNumber getAddressNumber() const { return _address_number; }
	//@}
};

}

#endif