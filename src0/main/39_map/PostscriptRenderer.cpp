
/** PostscriptRenderer class implementation.
	@file PostscriptRenderer.cpp

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


#include "PostscriptRenderer.h"

#include "Geometry.h"
#include "DrawableLine.h"
#include "DrawablePhysicalStop.h"

#include "Map.h"
#include "MapBackground.h"
#include "MapBackgroundManager.h"

#include "06_geometry/Point2D.h"

#include "01_util/Log.h"
#include "01_util/Conversion.h"

#include <cmath>
#include <iostream>
#include <fstream>

namespace synthese
{
	using namespace geometry;
	using namespace util;

	namespace util
	{
		template<>
		const std::string FactorableTemplate<map::Renderer,map::PostscriptRenderer>::FACTORY_KEY ("ps");
	}


	namespace map
	{

		PostscriptRenderer::PostscriptRenderer()
			: FactorableTemplate<Renderer,PostscriptRenderer>()
		{
		}

		PostscriptRenderer::~PostscriptRenderer()
		{
		}


		std::string
		PostscriptRenderer::render (
		const boost::filesystem::path& tempDir, 
		const std::string& filenamePrefix,
		const synthese::env::Line::Registry& lines,
		synthese::map::Map& map,
		const synthese::map::RenderingConfig& config
		)		{

			std::string resultFilename = filenamePrefix + ".ps";
			const boost::filesystem::path psFile (tempDir / resultFilename);

			// Create the postscript canvas for output
			std::ofstream of (psFile.string ().c_str ());

			// Filter accents (temporary workaround til having found how to
			// render accents properly in ghostscript.

			// boost::iostreams::filtering_ostream fof;
			// fof.push (synthese::util::PlainCharFilter());
			// fof.push (of);

			// ---- Render postscript file ----
			PostscriptCanvas _canvas(of);

			_config = config;

			_canvas.startPage(0, 0, map.getWidth (), map.getHeight ());
		    
			renderBackground (_canvas, map);
			renderLines (_canvas, map);
			renderPhysicalStops (_canvas, map);

			_canvas.showPage();

			of.close ();

			return resultFilename;

		}





		void 
		PostscriptRenderer::renderBackground (PostscriptCanvas& _canvas,Map& map)
		{
		    
			if (map.hasBackgroundManager ()) 
			{
			const Rectangle realFrame = map.getRealFrame ();

				const MapBackground* mbg = 
				map.getBackgroundManager ()->getBestScalingBackground (map.getScaleX (), 
										map.getScaleY ());


				if (mbg != 0) 
			{
				Log::GetInstance ().debug ("Best scaling background scaleX=" + 
							Conversion::ToString (mbg->getScaleX ()) + " scaleY=" + 
							Conversion::ToString (mbg->getScaleY ()));
					// Draw background
					std::pair<int,int>  tlIndexes = mbg->getIndexesOfTileContaining (realFrame.getX(), realFrame.getY ());
					std::pair<int,int>  brIndexes = mbg->getIndexesOfTileContaining (
				realFrame.getX () + realFrame.getWidth (), 
				realFrame.getY () + realFrame.getHeight ());
		            
				// TODO : additional checks in case indexes are negative/out of frame.

					int nbtiles = 0;

					for (int i=tlIndexes.first; i<=brIndexes.first; ++i) 
				{
						if (i < 0) continue; 
						if (i > mbg->getNbTilesX()-1) continue;

						for (int j=tlIndexes.second; j<=brIndexes.second; ++j) 
				{
						if (j < 0) continue; 
						if (j > mbg->getNbTilesY()-1) continue;

							const MapBackgroundTile* tile = mbg->getTile (i, j);
					Log::GetInstance ().debug ("Dumping background tile [" + Conversion::ToString (i) + 
					"," + Conversion::ToString (j) + "]");
							if (tile != 0) { // Any background available for this tile ?
								// cout << "Drawing tile " << i << "," << j <<  "  "<< tile->getPath ().string () << endl;
								++nbtiles;
								tile->draw (map, _canvas);
							}
						}
					}
				}
			}

		}


		void 
		PostscriptRenderer::renderLines (PostscriptCanvas& _canvas,Map& map)
		{
			// Draw drawableLines
			_canvas.setlinejoin (1);
		    
			const std::set<DrawableLine*>& selectedLines = map.getSelectedLines ();
		    

			// Predraw
			for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
				it != selectedLines.end () ; ++it) {
			const DrawableLine* dbl = *it;
			dbl->prepare (map, _config.getSpacing ());
			}    

			_canvas.setlinewidth (_config.getBorderWidth ());
			_canvas.setrgbcolor(_config.getBorderColor ());
			// Draw
			for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
				it != selectedLines.end () ; ++it) {
			const DrawableLine* dbl = *it;
			const std::vector<Point2D>& shiftedPoints = dbl->getShiftedPoints ();
			
			doDrawCurvedLine(_canvas, dbl);
			}
			_canvas.setlinewidth (_config.getLineWidth ());
		    
			for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
				it != selectedLines.end () ; ++it) {
			const DrawableLine* dbl = *it;
			const std::vector<Point2D>& shiftedPoints = dbl->getShiftedPoints ();
			_canvas.setrgbcolor(dbl->getColor ());

			doDrawCurvedLine(_canvas, dbl);
				}
		    
			for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
				it != selectedLines.end () ; ++it) {
			const DrawableLine* dbl = *it;
			if (dbl->getWithPhysicalStops() == false) continue;
			const std::vector<Point2D>& shiftedPoints = dbl->getShiftedPoints ();
			for (unsigned int i=1; i<shiftedPoints.size()-1; ++i) 
			{
				Point2D pt (shiftedPoints[i].getX() + 100.0, shiftedPoints[i].getY());
				double angle = calculateAngle (pt, shiftedPoints[i], shiftedPoints[i+1]);

				if (dbl->isStopPoint (i)) 
				{
				doDrawTriangleArrow(_canvas, shiftedPoints[i], toDegrees(angle - M_PI_2));
				doDrawSquareStop(_canvas, shiftedPoints[i], toDegrees(angle - M_PI_2));
				}
			
			}
			}    


			// Postdraw
			for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
				it != selectedLines.end () ; ++it) {
			const DrawableLine* dbl = *it;
			const std::vector<Point2D>& shiftedPoints = dbl->getShiftedPoints ();

			// Draw Terminuses
			if (shiftedPoints.size () >= 2) {

				Point2D pt (shiftedPoints[0].getX() + 100.0, 
					shiftedPoints[0].getY());

				double angle = calculateAngle (pt, 
							shiftedPoints[0], 
							shiftedPoints[1]);

				doDrawSquareTerminus (_canvas, shiftedPoints[0], 
						toDegrees(angle - M_PI_2));
		        
				pt = Point2D (shiftedPoints[shiftedPoints.size ()-2].getX() + 100.0, 
					shiftedPoints[shiftedPoints.size ()-2].getY());

				angle = calculateAngle (pt, 
							shiftedPoints[shiftedPoints.size ()-2], 
							shiftedPoints[shiftedPoints.size ()-1]);
		     
				doDrawSquareTerminus (_canvas, shiftedPoints[shiftedPoints.size ()-1], 
						toDegrees(angle - M_PI_2));
			}


			}    



		}





		void 
		PostscriptRenderer::doDrawCurvedLine (PostscriptCanvas& _canvas,const DrawableLine* dbl)
		{
			const std::vector<Point2D>& shiftedPoints = dbl->getShiftedPoints ();
			_canvas.newpath();
		/*
			_canvas.moveto(shiftedPoints[0].getX()+5, shiftedPoints[0].getY()+10);
			_canvas.rotate (45.0);
			_canvas.text (dbl->getShortName ());
			_canvas.rotate (-45.0);
		*/
			_canvas.moveto(shiftedPoints[0].getX(), shiftedPoints[0].getY());
		    
			for (unsigned int i=1; i<shiftedPoints.size (); ++i) 
			{
			double x = shiftedPoints[i].getX();
			double y = shiftedPoints[i].getY();
			double radiusShift = 0.0;
			
			if (_config.getEnableCurves () && (i < shiftedPoints.size () - 1)) 
			{
				// Take care of intern/extern turn to invert radius
				const Point2D& p_minus_1 = shiftedPoints[i-1];
				const Point2D& p_plus_1 = shiftedPoints[i+1];
			    
				double angle = calculateAngle (p_minus_1, shiftedPoints[i], p_plus_1);
				if (angle < 0) 
				{
				radiusShift = -dbl->getShift (i) * _config.getSpacing ();
				} 
				else 
				{
				radiusShift = +dbl->getShift (i) * _config.getSpacing ();
				} 
			    
			    
				double radiusToUse = _config.getRadius () + radiusShift;
				if (radiusToUse > _config.getRadius () + _config.getRadiusDelta ()) 
				{
				radiusToUse = _config.getRadius () - _config.getRadiusDelta ();
				}

				if (radiusToUse < _config.getRadius () - _config.getRadiusDelta ()) 
				{
				radiusToUse = _config.getRadius () - _config.getRadiusDelta ();
				}
			    
			    
				_canvas.arct(x, y, shiftedPoints[i+1].getX(), 
					shiftedPoints[i+1].getY(), radiusToUse);
			    
			} 
			else 
			{ 
				_canvas.lineto(x, y);	
			}
			
			}
		    
			_canvas.stroke();
		    
		}










		void 
		PostscriptRenderer::doDrawTriangleArrow (PostscriptCanvas& _canvas,const Point2D& point, 
							double angle)
		{
			_canvas.gsave ();
			_canvas.setrgbcolor(0, 0, 0);
			_canvas.moveto(point.getX(), point.getY ());
			_canvas.rotate (angle);
			_canvas.rmoveto (0.0, _config.getLineWidth () / 2.0 + 1);
			_canvas.triangle(_config.getLineWidth ()-1);  // size of the base side
			_canvas.fill ();
			_canvas.grestore ();	
		}



		void 
		PostscriptRenderer::doDrawSquareStop (PostscriptCanvas& _canvas,const Point2D& point, 
							double angle)
		{
			_canvas.gsave ();
			_canvas.setrgbcolor(0, 0, 0);
			_canvas.moveto(point.getX(), point.getY ());
			_canvas.rotate (angle);
			_canvas.square(_config.getLineWidth ()-1);  // size of the base side
			_canvas.fill ();
			_canvas.grestore ();	
		}







		void 
		PostscriptRenderer::doDrawSquareTerminus (PostscriptCanvas& _canvas,const Point2D& point, 
							double angle)
		{
			_canvas.gsave ();
			_canvas.setrgbcolor(0, 0, 0);
			_canvas.moveto(point.getX(), point.getY ());
			_canvas.rotate (angle);
			_canvas.square(_config.getLineWidth ()*2);  // size of the base side
			_canvas.fill ();
			_canvas.grestore (); 
		}




		void 
		PostscriptRenderer::renderPhysicalStops (PostscriptCanvas& _canvas,Map& map)
		{
			_canvas.setfont("Helvetica", 8);
			_canvas.setrgbcolor(0, 0, 0);

			// Draw
			for (std::set<DrawablePhysicalStop*>::const_iterator it = 
				map.getSelectedPhysicalStops ().begin ();
				it != map.getSelectedPhysicalStops ().end () ; ++it) 
			{
				const DrawablePhysicalStop* dps = *it;
			
			Point2D cp = dps->getPoint ();
			_canvas.moveto (cp.getX (), cp.getY ());
			_canvas.sticker (dps->getName (), synthese::util::RGBColor ("yellow"), 10, 10);
			}

		}

	}
}


