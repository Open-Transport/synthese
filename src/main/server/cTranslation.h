#ifndef SYNTHESE_CTRANSLATION_H_
#define SYNTHESE_CTRANSLATION_H_


#include <map>
#include <string>

class cTranslation
{
    private:

        static const std::string DEFAULT_LANGUAGE_CODE;

        std::map<std::string, std::string> _translations;

    public:

        cTranslation ();
        cTranslation ( const cTranslation& );

        ~cTranslation ();

        void addTranslation ( const std::string& languageCode, const std::string& translation );

        const std::string& getTranslation ( const std::string& code ) const;
        const std::string& getDefaultTranslation () const;

        cTranslation& operator=( const cTranslation& rhs );
};


#endif /*SYNTHESE_CTRANSLATION_H_*/
