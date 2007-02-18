
/** BikeComplyer class header.
	@file BikeComplyer.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_ENV_BIKECOMPLYER_H
#define SYNTHESE_ENV_BIKECOMPLYER_H


#include <vector>


namespace synthese
{


namespace env
{

    class BikeCompliance;


/** 
    Base class for an entity providing a bike regulation.
    If no regulation is provided, the parent complyer is called.
    
 @ingroup m15
*/
class BikeComplyer
{

private:

    const BikeComplyer* _parent;

    BikeCompliance* _bikeCompliance;

protected:

    BikeComplyer (const BikeComplyer* parent, 
		  BikeCompliance* bikeCompliance = 0);

public:

    ~BikeComplyer ();


    //! @name Getters/Setters
    //@{
    const BikeCompliance* getBikeCompliance () const;
    void setBikeCompliance (BikeCompliance* bikeCompliance);
	void	setParent(BikeComplyer* parent);
    //@}

    //! @name Query methods.
    //@{
    //@}
    
    //! @name Update methods.
    //@{
    //@}
    
    
};


}
}

#endif 	    

