#ifndef SYNTHESE_UTIL_CLASS_FACTORY_H
#define SYNTHESE_UTIL_CLASS_FACTORY_H

#include <map>
#include <string>

namespace synthese
{

    
    namespace util
    {

		/** Class factory.
		@ingroup m01

		@todo Translate following explanation :
			- in english
			- replace A by ClassFactory
			- replace AT by ClassFactoryTemplate
			- integrate the pair key instead of string 

		http://www.cppfrance.com/codes/ANSI-ISO-CPLUSPLUS-081-CLASSE-FABRIQUE-USINE_9716.aspx
		
		Voici une construction de "classe fabrique" (ou usine), c'est à dire une classe capable de fournir de nouveaux objet dont la la classe est déterminé dynamiquement.
		Ici, la classe est coisie en fonction du premier paramettre passé aux programme.
		La class 'A' est abstraite et servira à définir l'interface commun aux différente classes de la fabrique. Ces classe sont déclaré à la fin du code, pour vous montrer qu'elle sont facilement extenssible (nouvelles classe ...)
		Je vai encore apporter plus d'explications par la suite (mises à jours prochaines)... 

		Je déclare une classe template, 'AT', dont hériteront nos classe CA (A1, A2, ...).
		La classe 'A', dont hérite 'AT' (oui oui !) contient donc les fonction d'interface commune aux 'CA', ici une fonction virtuel qu'implémentera chaque class 'CA'.
		La fonction statique permet d'obtenir l'instanciation d'une classe 'CA' à partir de son "nom" ("C1", "C2", ...). C'est le but de la manoeuvre :-)
		En privé, 'A' possède une varribale statique lui permettant de stoquer une table de correspondance entre une chaine de caractères et un pointeur de fonction retournant l'adresse d'un nouvelle objet de classe 'A'. En pratique, pour chaque classe 'CA', il y aura une fonction qui instantiera un objet de classe 'CA' correspondant et en retournera un pointeur en tant 'A'.
		Bon, et pour faciliter l'enregistrement de chaque classe 'CA', une classe template imbriqué, 'A::Register&lt;CA&gt;', amie bien entendu ;-)
		Bon, revenons maintenant à 'AT&lt;CA&gt;', dont hérite les 'CA'.
		En public, elle possède une fonction statique (une fonction par 'CA' !), celle-la même dont l'adresse est place dans la table de correspondance ('A::a_creators') !
		En privé, une fonction statique, 'name', qui retournera le nom de la classe. Pour chaque class 'CA', nous spécialiserons cette fonction (patience ;).
		Et !, une variable statique (une variable par 'CA' !), un objet de classe 'A::Register&lt;CA&gt;'.
		Bien entendu, on est tous amis ;-)
		Maintenant, que fait cette fameuse classe 'Register&lt;CA&gt;' ? Hé bien, à ça création (avant l'entré dans la fonction main donc, puisse que statique de 'AT&lt;CA&gt;'), elle enregistre la classe 'CA', en insérant un couple créé à partir du nom retourné par 'AT&lt;CA&gt;::name()' (spécialisée !) et l'adresse de la fonction 'AT::&lt;CA&gt;::get_new' (statique !). Ainsi, l'insertion est automatique pour toute classe 'CA', c'est à dire héritant de 'AT&lt;CA&gt;' et pour laquelle 'AT&lt;CA&gt;::name' est spécialisé (plus encore une petite chose...;).
		Ne pas oublier de définir les membre statique, en locurrence 'AT&lt;CA&gt;::m_register'.
		La fonction 'AT&lt;CA&gt;::get_new()' est trivialement simple.
		Définition, maintenant de notre table de correspondance, 'A::a_creators'.
		La fonction la plus important, 'A::get_new( class_name )' est celle qui vat chercher dans la table de correspondance, la fonction adéquat pour la création de l'objet dont la classe sera déterminé par 'class_name'. Pour cela, nous utilisont la fonction 'map&lt;&gt;::find( key )'. Dès lors que l'itérateur retourné est "valide", nous utilison la seconde partie de l'élément, à savoir le pointeur de fonction, pour appeler cette fonction et retourner, directement, l'adresse ainsi obtenu, du nouvelle objet. Ici, le chois est fait, en cas d'échec, de retourner un pointeur nul. Mais vous pourriez également émettre une exeption.
		Dans la fonction 'main( argc, arv )', nous appelons, tout simplement 'A::get_new' en passant un 'std::string' construit à partir du 1er argument ('argv[1]'). Pour "démonstration", nous envoyons via le flux de sortie, le "nom" de la classe, ici obtenu par l'appel de la fonction virtuelle 'A::cout_name'.
		Il ne nous reste plus qu'à concrétiser quelques classe 'CA'.
		C'est très simple, chaque classe 'CA' doit hériter de 'AT&lt;CA' (par exemple 'C1' hérite de 'AT&lt;C1&gt;'). Elle doit implémenter (de préférence ;) les fonctions virtuelles (ici, 'cout_name'). Il faut spécialiser la fonction 'AT&lt;CA&gt;::name', de sorte à lui donner un nom (ici, nous respectons le nom c++ de la classe ("A1", "A2", ...), mais vous pourriez mettre "bannane" et "abricot"). C'est ce nom qui est utilisé dans la table de correspondance. Et finalement, petit détail de grande importance, "instancier" la classe 'AT&lt;CA', sans quoi vous n'aurrez de définition des membres en question dans aucun module (.obj ou .o) à l'édition des liens !
		Bon, hé bien voila, n'hésitez pas à me poser des questions, de préférence sur le ng fr.comp.lang.c++ :-) 

		The Superclass must provide :
			- Public typedef FactoryFunctionPointer : the pointer type of the factory function. Example : typedef SuperClass *(*FactoryFunctionPointerType)();
			- Public method called get_new : the factory fonction


		*/
		class ClassFactory
		{ 
			public: 
		  
				static ClassFactory * get_new( const std::string& class_name ); 
		  
				virtual void cout_name() = 0; 
		  
			private: 
		  
				typedef ClassFactory *(*FactoryFunctionPointer)();
				typedef std::map<std::string, FactoryFunctionPointer > ClassFactoryFunctionsMap;
				static ClassFactoryFunctionsMap _factoryFunctions;
		  
				template<class CA> 
				class Register 
				{
					public: 
					Register(); 
				};
		  
			template<class CA> 
			friend class Register; 
		};


		template<class CA> 
		class ClassFactoryTemplate : public ClassFactory
		{ 
			public: 
		  
				static ClassFactory * get_new(); 
		  
			private: 
		  
				static char const * name(); 
				static ClassFactory::Register<CA> m_register; 
		  
			friend class ClassFactory::Register<CA>; 
		}; 
		  
		template<class CA> 
		ClassFactory::Register<CA>::Register() 
		{ 
			_factoryFunctions.insert( 
				ClassFactoryFunctionsMap::value_type( 
					std::string(ClassFactoryTemplate<CA>::name())
					, & ClassFactoryTemplate<CA>::get_new 
				) 
			);
		} 

		template<class CA> 
		ClassFactory::Register<CA> ClassFactoryTemplate<CA>::m_register; 
		  
		template<class CA>
		ClassFactory * ClassFactoryTemplate<CA>::get_new() 
		{
			return new CA;
		}
		  
		ClassFactory::ClassFactoryFunctionsMap ClassFactory::_factoryFunctions; 
		  
		ClassFactory* ClassFactory::get_new( std::string const & class_name ) 
		{ 
			ClassFactoryFunctionsMap::iterator i = _factoryFunctions.find( class_name ); 
			if( i != _factoryFunctions.end() ) 
				return (*i).second(); 
			else 
				return static_cast<ClassFactory*>(0); //! @todo Throw an exception
		} 
	}
}

#endif