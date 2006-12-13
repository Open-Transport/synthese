
#ifdef NEPASDEFINIR

/*! \brief Constructeur
*/
cServiceEnGare::cServiceEnGare( char newType, cPhoto* newPhoto, const std::string& newDesignation )
{
    vType = newType;
    vPhoto = newPhoto;
    vDesignation << newDesignation;
}

#endif

