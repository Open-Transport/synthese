#ifndef SYNTHESE_CARTO_POSTSCRIPTCANVAS_H
#define SYNTHESE_CARTO_POSTSCRIPTCANVAS_H


#include <iostream>
#include <string>



namespace synthese
{

namespace util
{
    struct RGBColor;
}


namespace map
{


/** @ingroup m39 */
class PostscriptCanvas
{
 private:

    static const std::string SQUARE_DEF;
    static const std::string TRIANGLE_DEF;
    static const std::string PATHTEXT_DEF;
    static const std::string FRAMETEXT_DEF;
    static const std::string STICKER_DEF;

    std::ostream& _output; //!< Stream used for postscript generation


 public:

    PostscriptCanvas(std::ostream& output);

    virtual ~PostscriptCanvas();

    std::ostream& getOutput () { return _output; }

    void startPage (double x, double y, double width, double height);

    void translate (double xtranslate, double ytranslate);
    void scale (double xscale, double yscale);
    void rotate (double rotate);

    void moveto (double x, double y);
    void rmoveto (double x, double y);

    void setfont (const std::string& fontName, int fontSize);
    void setlinecap (int lineCap);
    void setlinejoin (int lineJoin);
    void setlinewidth (int lineWidth);
    void setrgbcolor (const synthese::util::RGBColor& color);
    void setrgbcolor (double red, double green, double blue);
    void text (const std::string& text);

    void newpath ();
    void closepath ();

    void lineto (double x, double y);
    void curveto (double cp1x, double cp1y, double cp2x, double cp2y, double x, double y);
    void arct (double x1, double y1, double x2, double y2, double r);

    void stroke ();
    void fill ();
    void showPage ();

    void gsave ();
    void grestore ();

    void pathtext (double offset, const std::string& text);

    void triangle (double width);
    void square (double width);
    void sticker (const std::string& label,
                  const synthese::util::RGBColor& bgColor,
                  double xMargin, double yMargin);


 private:

    PostscriptCanvas (const PostscriptCanvas&);
    PostscriptCanvas& operator= (const PostscriptCanvas& rhs);



};

}
}

#endif

