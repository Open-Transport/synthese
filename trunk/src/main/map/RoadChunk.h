#ifndef ROADCHUNK_H_
#define ROADCHUNK_H_

#include "Referrant.h"
#include <vector>



namespace synmap
{

class Vertex;
class Location;
class Topography;
class Road;


class RoadChunk : public Referrant
{
public:
	
	typedef enum { RIGHT_SIDE, LEFT_SIDE, UNKNOWN_SIDE } AddressNumberSide;

	/** Numéro d'adresse.
		Ex : 70 rue de Paris : le numéro d'adresse est 70.
	*/
	typedef double AddressNumber;

	/** Constante numéro d'adresse inconnu.
	*/
	static const AddressNumber ADDRESS_NUMBER_UNKNOWN;

	
private:
	
	const std::vector<const Location*> _steps;

	const AddressNumber _rightStartNumber;   // inclusive
	const AddressNumber _rightEndNumber;     // inclusive
	
	const AddressNumber _leftStartNumber;    // inclusive
	const AddressNumber _leftEndNumber;      // inclusive

	const Road* _road;

public:

	RoadChunk(Topography* topography,
		  int key,
		  const std::vector<const Location*>& steps,
		  AddressNumber rightStartNumber, 
		  AddressNumber rightEndNumber, 
		  AddressNumber leftStartNumber, 
		  AddressNumber leftEndNumber);
	
	virtual ~RoadChunk();


	const Road* getRoad () const { return _road; }
	void setRoad (const Road* road) { _road = road; }

	AddressNumber getRightStartNumber () const { return _rightStartNumber; }
	AddressNumber getRightEndNumber () const { return _rightEndNumber; }
	AddressNumber getLeftStartNumber () const { return _leftStartNumber; }
	AddressNumber getLeftEndNumber () const { return _leftEndNumber; }

	AddressNumberSide getSideForNumber (AddressNumber number) const;
	bool hasNumber (AddressNumber number) const;

	const Location* getStep (int index) const { return _steps[index]; }

};

}

#endif /*ROADCHUNK_H_*/
