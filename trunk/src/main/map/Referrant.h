#ifndef REFERRANT_H_
#define REFERRANT_H_

#include <map>
#include <string>

namespace synmap
{

class Topography;


class Referrant 
{
private:

	typedef std::map<std::string, std::string> Properties;
	
	Topography* _topography;
	const int _key;
	
	Properties _properties;
	
protected:
	Referrant(Topography* topography, int key);
	virtual ~Referrant();



public:
	const Topography* getTopography () const { return _topography; }
	int getKey () const { return _key; }

	bool hasProperty (const std::string& name) const;
	const std::string& getProperty (const std::string& name) const;
	void setProperty (const std::string& name, const std::string& value);


};

}

#endif /*REFERRANT_H_*/
