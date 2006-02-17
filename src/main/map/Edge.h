#ifndef EDGE_H_
#define EDGE_H_

namespace synmap
{

class Vertex;


class Edge
{
public:

private:
	const Vertex* _from;
	const Vertex* _to;
	
	const double _length;

	Edge(const Vertex* from, const Vertex* to);
	virtual ~Edge();
	
public:
	
	const Vertex* getFrom () const { return _from; }
	const Vertex* getTo () const { return _to; }
	
	double getLength () const { return _length; }
	
	friend class Topography;
	
};

}

#endif /*EDGE_H_*/
