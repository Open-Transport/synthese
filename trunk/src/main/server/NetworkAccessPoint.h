
class cPoint;

class NetworkAccessPoint : public cPoint
{
private:
	cTexte		_name;	//!< Nom

public:
	//!	@name Accesseurs
	//@{
		const cTexte&	getNom()		const;
	//@}
	
	//!	@name Modificateurs
	//@{
		bool setNom(const cTexte&);
	//@}

	//!	@name Constructeur et destructeur
	//@{
		NetworkAccessPoint() : cPoint() { }
		~NetworkAccessPoint();
	//@}

};