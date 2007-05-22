#include "07_lex_matcher/LexicalMatcher.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::lexmatcher;


std::vector<std::string> 
getTestBase1 ()
{
    std::vector<std::string> testBase;
    testBase.push_back ("Abreuvoir, rue de l'");
    testBase.push_back ("Anicenne Mare, cour de l'");
    testBase.push_back ("Boulangers, chemin des");
    testBase.push_back ("Buc, rue de");
    testBase.push_back ("Charles de Gaulle, rue");
    testBase.push_back ("Chateau, all�e du");
    testBase.push_back ("Cimeti�re, chemin du");
    testBase.push_back ("Cocanne, rue de la");
    testBase.push_back ("Commanderie, rue de la");
    testBase.push_back ("C�tes Montbron, chemin des");
    testBase.push_back ("Couronne, rue de la");
    testBase.push_back ("Croix Blanche, rue de la");
    testBase.push_back ("Croix des Mortiers, rue de la");
    testBase.push_back ("Division Leclerc, rue de la");
    testBase.push_back ("Ferme de l'hopital, rue de la");
    testBase.push_back ("Folie, rue de la");
    testBase.push_back ("Fontaine, sente de la");
    testBase.push_back ("Garenne, rue de la");
    testBase.push_back ("Georges Guynemer, rue");
    testBase.push_back ("Grande Rue");
    testBase.push_back ("Guy Mocquet, rue");
    testBase.push_back ("Haies, rue des");
    testBase.push_back ("Haras de Vauptain, route du");
    testBase.push_back ("Jean Mermoz, rue");
    testBase.push_back ("Jouy, route de");
    testBase.push_back ("Juvini�re, rue de la");
    testBase.push_back ("Logeraie, all�e de la");
    testBase.push_back ("Louis Grenier, place");
    testBase.push_back ("Mansart, rue");
    testBase.push_back ("Marroniers, all�e des");
    testBase.push_back ("Maryse Basti�, all�e");
    testBase.push_back ("Midori, rue de");
    testBase.push_back ("Monument, place du");
    testBase.push_back ("Orme Rond, chemin de l'");
    testBase.push_back ("P�pini�re, rue de la");
    testBase.push_back ("Petit Jouy, rue du");
    testBase.push_back ("Peupliers, all�e des");
    testBase.push_back ("Pointe, rue de la");
    testBase.push_back ("Portes des Loges, chemin de la");
    testBase.push_back ("Poste, rue de la");
    testBase.push_back ("Saint Exup�ry, rue");
    testBase.push_back ("Sophoras, all�e des");
    testBase.push_back ("Tilleuls, rue des");
    testBase.push_back ("Trou Sal�, rue du");
    testBase.push_back ("Vall�e, chemin de la");
    testBase.push_back ("Vauptain, chemin de");
    return testBase;
}





BOOST_AUTO_TEST_CASE (testBasicMatches)
{
    LexicalMatcher<int> matcher (true, true);
    std::vector<std::string> testBase = getTestBase1 ();
    for (size_t i=0; i<testBase.size (); ++i) matcher.add (testBase[i], 0);

    BOOST_CHECK_EQUAL (std::string ("Peupliers, all�e des"), matcher.bestMatch ("all. peuplier").key);
    BOOST_CHECK_EQUAL (std::string ("Peupliers, all�e des"), matcher.bestMatch ("    all   .   peuplier   ").key);
    BOOST_CHECK_EQUAL (std::string ("Peupliers, all�e des"), matcher.bestMatch ("peupier").key);
    BOOST_CHECK_EQUAL (std::string ("Peupliers, all�e des"), matcher.bestMatch ("peupier all�e").key);
    BOOST_CHECK_EQUAL (std::string ("Peupliers, all�e des"), matcher.bestMatch ("PeuPlieER").key);
	
    BOOST_CHECK_EQUAL (std::string ("Division Leclerc, rue de la"), matcher.bestMatch ("rue div leclerc").key);
    BOOST_CHECK_EQUAL (std::string ("Division Leclerc, rue de la"), matcher.bestMatch ("ruedivleclerc").key);
    BOOST_CHECK_EQUAL (std::string ("Division Leclerc, rue de la"), matcher.bestMatch ("lecler").key);

    BOOST_CHECK_EQUAL (std::string ("Sophoras, all�e des"), matcher.bestMatch ("soforas").key);


    BOOST_CHECK_EQUAL (std::string ("Portes des Loges, chemin de la"), matcher.bestMatch ("loge porte chemin").key);

}



BOOST_AUTO_TEST_CASE (testWholeWordMatchingCriterium)
{
    {
	std::vector<std::string> testBase;
	testBase.push_back ("ANPE trucmuche");
	testBase.push_back ("ANPE bidule");
	testBase.push_back ("Anges");
	    
	    
	LexicalMatcher<int> matcher;
	for (size_t i=0; i<testBase.size (); ++i) matcher.add (testBase[i], 0);
	    
	BOOST_CHECK_EQUAL (std::string ("ANPE bidule"), matcher.bestMatch ("ANPE").key);
    }
    {
	std::vector<std::string> testBase;
	testBase.push_back ("Rue Sainte-Marie");
	testBase.push_back ("Rue de Saint-Cyprien");
	testBase.push_back ("Rue Saint-Cyr");
	    
	    
	LexicalMatcher<int> matcher;
	for (size_t i=0; i<testBase.size (); ++i) matcher.add (testBase[i], 0);
	    
	BOOST_CHECK_EQUAL (std::string ("Rue de Saint-Cyprien"), matcher.bestMatch ("rue saint-siprien").key);
    }
}


