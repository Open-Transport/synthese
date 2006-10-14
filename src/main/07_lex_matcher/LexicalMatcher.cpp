#include "LexicalMatcher.h"


namespace synthese
{
namespace lexmatcher
{



    TranslationMap FrenchTranslationMap () 
    {
	TranslationMap map;

	map.globalTranslations.insert (std::make_pair ("�", "a"));
	map.globalTranslations.insert (std::make_pair ("�", "a"));

	map.globalTranslations.insert (std::make_pair ("�", "e"));
	map.globalTranslations.insert (std::make_pair ("�", "e"));
	map.globalTranslations.insert (std::make_pair ("�", "e"));
	map.globalTranslations.insert (std::make_pair ("�", "e"));

	map.globalTranslations.insert (std::make_pair ("�", "i"));
	map.globalTranslations.insert (std::make_pair ("�", "i"));

	map.globalTranslations.insert (std::make_pair ("�", "o"));
	map.globalTranslations.insert (std::make_pair ("�", "o"));

	map.globalTranslations.insert (std::make_pair ("�", "u"));
	map.globalTranslations.insert (std::make_pair ("�", "u"));
	map.globalTranslations.insert (std::make_pair ("�", "u"));

	map.globalTranslations.insert (std::make_pair ("�", "c"));
	return map;

    }




}
}
