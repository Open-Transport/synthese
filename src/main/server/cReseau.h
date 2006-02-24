/*------------------*
 |                  |
 |  Classe Reseau   |
 |                  |
 *------------------*/

#ifndef SYNTHESE_CRESEAU_H
#define SYNTHESE_CRESEAU_H

#include <string>
 
/** Réseau de transport
	@ingroup m05
*/
class cReseau
{
	const size_t _id;
	std::string vNom;
	std::string vURL;
	std::string vDoc;
	
public:
	const size_t& getId() const;

	void setURL(const std::string& newVal);
	void setDoc(const std::string& newVal);
	void setNom(const std::string& newVal);

	cReseau(const size_t&);
	~cReseau();
};

#endif
