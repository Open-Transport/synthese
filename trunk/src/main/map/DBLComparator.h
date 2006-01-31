#ifndef DBLCOMPARATOR_H_
#define DBLCOMPARATOR_H_

#include <vector>

namespace synmap
{

class Vertex;
class DrawableBusLine;


class DBLComparator
{
private:	
	const DrawableBusLine* _reference;
	const Vertex* _referenceVertex;

	const Vertex* _vertex;

	
	int firstIndexOf (const Vertex* vertex, const std::vector<const Vertex*>& vertices) const;
	
	
	
public:

	DBLComparator(const DrawableBusLine* reference, 
                  const Vertex* referenceVertex, 
                  const Vertex* vertex);
	
	virtual ~DBLComparator();
	
	void setVertex (const Vertex* vertex);
	
	double calculateStartAngleAtIndex (const std::vector<const Vertex*>& vertices, int index) const;
	double calculateEndAngleAtIndex (const std::vector<const Vertex*>& vertices, int index) const;
	
	int operator() (const DrawableBusLine* bl1, const DrawableBusLine* bl2) const;
	
};

}

#endif /*DBLCOMPARATOR_H_*/
