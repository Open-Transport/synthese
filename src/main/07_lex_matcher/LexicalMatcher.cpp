#include "LexicalMatcher.h"


namespace synthese
{
namespace lexmatcher
{



    TranslationMap FrenchTranslationMap () 
    {
	TranslationMap map;

	map.globalTranslations.insert (std::make_pair ("à", "a"));
	map.globalTranslations.insert (std::make_pair ("â", "a"));

	map.globalTranslations.insert (std::make_pair ("é", "e"));
	map.globalTranslations.insert (std::make_pair ("è", "e"));
	map.globalTranslations.insert (std::make_pair ("ê", "e"));
	map.globalTranslations.insert (std::make_pair ("ë", "e"));

	map.globalTranslations.insert (std::make_pair ("î", "i"));
	map.globalTranslations.insert (std::make_pair ("ï", "i"));

	map.globalTranslations.insert (std::make_pair ("ô", "o"));
	map.globalTranslations.insert (std::make_pair ("ö", "o"));

	map.globalTranslations.insert (std::make_pair ("ù", "u"));
	map.globalTranslations.insert (std::make_pair ("û", "u"));
	map.globalTranslations.insert (std::make_pair ("ü", "u"));

	map.globalTranslations.insert (std::make_pair ("ç", "c"));
	return map;

    }




}
}
