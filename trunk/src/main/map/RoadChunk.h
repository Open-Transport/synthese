#ifndef ROADCHUNK_H_
#define ROADCHUNK_H_

#include "Referrant.h"

namespace synmap
{

class Vertex;
class Topography;


class RoadChunk : public Referrant
{
public:
	
	typedef int AddressNumber;
	typedef enum { RIGHT_SIDE, LEFT_SIDE, UNKNOWN_SIDE } AddressNumberSide;

	static const AddressNumber ADDRESS_NUMBER_UNKNOWN = -1;
	
private:
	
	const Vertex* _start;   // Not oriented.
	const Vertex* _end;		// Not oriented.
	
	const AddressNumber _rightStartNumber;   // inclusive
	const AddressNumber _rightEndNumber;     // inclusive
	
	const AddressNumber _leftStartNumber;    // inclusive
	const AddressNumber _leftEndNumber;      // inclusive

public:

	RoadChunk(Topography* topography,
			  int key,
			  const Vertex* start, 
			  const Vertex* end, 
			  AddressNumber rightStartNumber, 
			  AddressNumber rightEndNumber, 
			  AddressNumber leftStartNumber, 
			  AddressNumber leftEndNumber);
			  
	virtual ~RoadChunk();



	AddressNumber getRightStartNumber () const { return _rightStartNumber; }
	AddressNumber getRightEndNumber () const { return _rightEndNumber; }
	AddressNumber getLeftStartNumber () const { return _leftStartNumber; }
	AddressNumber getLeftEndNumber () const { return _leftEndNumber; }

	AddressNumberSide getSideForNumber (AddressNumber number) const;
	bool hasNumber (AddressNumber number) const;

	const Vertex* getStart () const { return _start; }
	const Vertex* getEnd () const { return _end; }


};

}

#endif /*ROADCHUNK_H_*/
