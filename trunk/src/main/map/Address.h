/** En-t�te classe adresse.
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


/** Adresse (rue + position m�trique)

	L'adresse est un positionnement sur une rue d�termin� par une position m�trique de puis l'origine de la rue.

	Le num�ro d'adresse est une donn�e de d�signation de l'adresse, mais peut �galement suffire � d�terminer la position d�s lors que la rue poss�de les informations n�cessaires.
*/
class Address
{
public:

	/** Chemin d'acc�s aux arr�ts physiques depuis l'adresse */
	typedef std::pair<Road::RoadChunkVector, const PhysicalStop*> PathToPhysicalStop;

protected:

	//!	@name Localisation
	//@{
		Road* const						_road;				//!< Route sur laquelle se trouve l'adresse
		const RoadChunk::AddressNumber	_address_number;	//!< Num�ro de l'adresse
		double							_metric_offset;		//!< Point m�trique de l'adresse sur la rue
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