
#ifdef NEPASDEFINIR

/*!	\brief Constructeur
*/
cServiceEnGare::cServiceEnGare(char newType, cPhoto* newPhoto, const cTexte& newDesignation)
{
	vType = newType;
	vPhoto = newPhoto;
	vDesignation << newDesignation;
}

#endif