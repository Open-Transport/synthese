#include "LexicalMatcherTest.h"

#include "01_util/LexicalMatcher.h"

#include <iostream>



namespace synthese
{
namespace util
{
    std::vector<std::string> 
    LexicalMatcherTest::getTestBase1 () const
    {
	std::vector<std::string> testBase;
	testBase.push_back ("Abreuvoir, rue de l'");
	testBase.push_back ("Anicenne Mare, cour de l'");
	testBase.push_back ("Boulangers, chemin des");
	testBase.push_back ("Buc, rue de");
	testBase.push_back ("Charles de Gaulle, rue");
	testBase.push_back ("Chateau, allée du");
	testBase.push_back ("Cimetière, chemin du");
	testBase.push_back ("Cocanne, rue de la");
	testBase.push_back ("Commanderie, rue de la");
	testBase.push_back ("Côtes Montbron, chemin des");
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
	testBase.push_back ("Juvinière, rue de la");
	testBase.push_back ("Logeraie, allée de la");
	testBase.push_back ("Louis Grenier, place");
	testBase.push_back ("Mansart, rue");
	testBase.push_back ("Marroniers, allée des");
	testBase.push_back ("Maryse Bastié, allée");
	testBase.push_back ("Midori, rue de");
	testBase.push_back ("Monument, place du");
	testBase.push_back ("Orme Rond, chemin de l'");
	testBase.push_back ("Pépinière, rue de la");
	testBase.push_back ("Petit Jouy, rue du");
	testBase.push_back ("Peupliers, allée des");
	testBase.push_back ("Pointe, rue de la");
	testBase.push_back ("Portes des Loges, chemin de la");
	testBase.push_back ("Poste, rue de la");
	testBase.push_back ("Saint Exupéry, rue");
	testBase.push_back ("Sophoras, allée des");
	testBase.push_back ("Tilleuls, rue des");
	testBase.push_back ("Trou Salé, rue du");
	testBase.push_back ("Vallée, chemin de la");
	testBase.push_back ("Vauptain, chemin de");
	return testBase;
    }


    void 
    LexicalMatcherTest::setUp () 
    {
    }
    

    void 
    LexicalMatcherTest::tearDown() 
    {

    } 

    



    void
    LexicalMatcherTest::testBasicMatches ()
    {
	LexicalMatcher<int> matcher (true, true);
	std::vector<std::string> testBase = getTestBase1 ();
	for (size_t i=0; i<testBase.size (); ++i) matcher.add (testBase[i], 0);

	CPPUNIT_ASSERT_EQUAL (std::string ("Peupliers, allée des"), matcher.bestMatch ("all. peuplier").key);
	CPPUNIT_ASSERT_EQUAL (std::string ("Peupliers, allée des"), matcher.bestMatch ("    all   .   peuplier   ").key);
	CPPUNIT_ASSERT_EQUAL (std::string ("Peupliers, allée des"), matcher.bestMatch ("peupier").key);
	CPPUNIT_ASSERT_EQUAL (std::string ("Peupliers, allée des"), matcher.bestMatch ("peupier allée").key);
	CPPUNIT_ASSERT_EQUAL (std::string ("Peupliers, allée des"), matcher.bestMatch ("PeuPlieER").key);
	
	CPPUNIT_ASSERT_EQUAL (std::string ("Division Leclerc, rue de la"), matcher.bestMatch ("rue div leclerc").key);
	CPPUNIT_ASSERT_EQUAL (std::string ("Division Leclerc, rue de la"), matcher.bestMatch ("lecler").key);

	CPPUNIT_ASSERT_EQUAL (std::string ("Sophoras, allée des"), matcher.bestMatch ("soforas").key);

	CPPUNIT_ASSERT_EQUAL (std::string ("Portes des Loges, chemin de la"), matcher.bestMatch ("loge chemin").key);

    }







}
}

