////////////////////////////////////////////////////////////////////////////////
/// Function class header.
///	@file Function.h
///	@author Hugues Romain
///	@date 2008-12-26 18:00
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_Function_h__
#define SYNTHESE_Function_h__

#include "FactoryBase.h"
#include "ParametersMap.h"
#include "Env.h"

#include <boost/shared_ptr.hpp>

////////////////////////////////////////////////////////////////////
/// @defgroup refFunctions Functions
///	@ingroup ref

namespace synthese
{
	namespace server
	{
		class Request;

		//////////////////////////////////////////////////////////////////////////
		/// Public function abstract class to use in a Request.
		///
		/// A public function is a feature that can be used by passing a SYNTHESE Request,
		/// and that produces an output.
		/// NB : Not to be confused with public action, that produces no output and can be launched
		/// before a public function (see Action class)
		///
		/// The Function class is the root class of a factory which is derived by each public
		/// function class.
		///
		/// A public function object is a mapping between a request object (see Request class)
		/// and some features of SYNTHESE.
		/// Warning : the public function object is not supposed to contain any feature itself.
		///
		/// The public function object can be used by two symmetrical ways :
		///		- run a feature from a request : the key of the child class, contained in the request,
		///		determines which subclass handles the requested feature. It provides a method in charge
		///		of the control of the provided arguments of the feature. (see _setFromParametersMap method)
		///		- produce a request object from a feature : a method builds the whole request string from
		///		the parameters stored previously in the object by the setters. (see _getParametersMap method)
		///
		/// @ingroup m18
		//////////////////////////////////////////////////////////////////////////
		class Function
		:	public util::FactoryBase<Function>
		{
		public:

		protected:
			Request* _request;
			mutable util::Env	_env;

			//////////////////////////////////////////////////////////////////////////
			///	Constructor.
			//////////////////////////////////////////////////////////////////////////
			Function();

		public:
			/** Conversion from fixed attributes to generic parameter map.
				@return synthese::server::ParametersMap The generated parameters map
				@author Hugues Romain
				@date 2007
				
				The fixed attributes are those which concern the session and other "always the same" information, like interface number, etc.
				The variables attributes are the business parameters of the function.
			*/
			virtual ParametersMap getFixedParametersMap() const { return ParametersMap(); }

			/** Conversion from attributes to generic parameter maps.
				@return The generated parameters map
			*/
			virtual ParametersMap _getParametersMap() const = 0;

		private:

			/** Conversion from generic parameters map to attributes.
				@param map The map to interpret (comes from _parseString())
				@throw RequestException if a parameter is missing or has corrupted value
			*/
			virtual void _setFromParametersMap(const ParametersMap& map) = 0;

			/** Method to run after action execution.
				@return true if the current execution must be stopped

				This method has to be overloaded.
				The default behavior is to do nothing and continue the execution after the action run.
			*/
			virtual bool _runAfterSucceededAction(std::ostream& stream) { return false; }

			/** Method to run before display when no session is detected.
				@return true if the current execution must be stopped

				This method has to be overloaded.
				The default behavior is to do nothing and continue the execution without any session.
			*/
			virtual bool _runBeforeDisplayIfNoSession(std::ostream& stream) { return false; }


			/** Authorization control.
				@return True if the action run is authorized
				@author Hugues Romain
				@date 2007
			*/
			virtual bool _isAuthorized() const = 0;

			/** Copy of the function parameters.
				@param function
				@return void
				@author Hugues Romain
				@date 2007				
			*/
			virtual void _copy(boost::shared_ptr<const Function> function) {}

			/** Function to display, defined by each subclass.
			@param stream Stream to write the output on.
			*/
			virtual void _run(std::ostream& stream) const = 0;

			friend class Request;
		};
	}
}

#endif // SYNTHESE_Function_h__
