#include "RenderingConfig.h"


using synthese::util::RGBColor;


namespace synthese
{
namespace map
{


RenderingConfig::RenderingConfig ()
    : _enableCurves (false)
    , _radius (15.0)
    , _radiusDelta (10.0)
    , _lineWidth (5)
    , _spacing (5+1)
    , _borderColor ("black")
    , _borderWidth (5+2)
{
}


RenderingConfig::~RenderingConfig ()
{
}



bool
RenderingConfig::getEnableCurves () const
{
    return _enableCurves;
}



void
RenderingConfig::setEnableCurves (bool enableCurves)
{
    _enableCurves = enableCurves;
}



double
RenderingConfig::getRadius () const
{
    return _radius;
}


void
RenderingConfig::setRadius (double radius)
{
    _radius = radius;
}


double
RenderingConfig::getRadiusDelta () const
{
    return _radiusDelta;
}


void
RenderingConfig::setRadiusDelta (double radiusDelta)
{
    _radiusDelta = radiusDelta;
}




int
RenderingConfig::getLineWidth () const
{
    return _lineWidth;
}


void
RenderingConfig::setLineWidth (int lineWidth)
{
    _lineWidth = lineWidth;
}


double
RenderingConfig::getSpacing () const
{
    return _spacing;
}


void
RenderingConfig::setSpacing (double spacing)
{
    _spacing = spacing;
}




RGBColor
RenderingConfig::getBorderColor () const
{
    return _borderColor;
}


void
RenderingConfig::setBorderColor (const RGBColor& borderColor)
{
    _borderColor = borderColor;
}



int
RenderingConfig::getBorderWidth () const
{
    return _borderWidth;
}


void
RenderingConfig::setBorderWidth (int borderWidth)
{
    _borderWidth = borderWidth;
}



}
}


