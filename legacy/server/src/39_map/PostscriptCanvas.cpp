#include "PostscriptCanvas.h"

#include "01_util/RGBColor.h"



namespace synthese
{
namespace map
{


    const std::string PostscriptCanvas::TRIANGLE_DEF = "\
/triangle {\n\
 /size exch 2 div def\n\
 size neg 0 rmoveto\n\
 size size rlineto\n\
 size size neg rlineto\n\
 size 2 mul neg 0 rlineto\n\
} def\n";


    const std::string PostscriptCanvas::SQUARE_DEF = "\
/square {\n\
 /size exch 2 div def\n\
 size neg size neg rmoveto\n\
 0 size 2 mul rlineto\n\
 2 size mul 0 rlineto\n\
 0 size 2 mul neg rlineto\n\
 2 size mul neg 0 rlineto\n\
} def\n";




// % string bgcolor width height sticker
    const std::string PostscriptCanvas::STICKER_DEF = "\
/sticker {\
 gsave gsave gsave\
 /sh exch def\
 /sw exch def\
 /bgcolor exch def\
 /str exch def\
 str false charpath flattenpath pathbbox\
 exch 4 1 roll exch\
 sub /strh exch def\
 sub /strw exch def\
 str stringwidth pop\
 /sw2 exch def\
 sw2 sw gt {/sw sw2 10 add def} if\
 strh sh gt {/sh strh 10 add def} if\
 grestore \
 sw 0 rlineto\
 0 sh rlineto\
 sw neg 0 rlineto\
 0 sh neg rlineto\
 bgcolor aload pop setrgbcolor false upath ufill\
 1 setlinewidth\
 0 0 0 setrgbcolor stroke\
 grestore\
 sw sw2 sub 2 div sh strh sub 2 div rmoveto\
 str show\
 grestore\
 sw 0 rmoveto\
} def";
/*
/sticker {\
 gsave gsave gsave\
 /sh exch def\
 /sw exch def\
 /bgcolor exch def\
 /str exch def\
 str false charpath flattenpath pathbbox\
 exch 4 1 roll exch\
 sub /strh exch def\
 sub /strw exch def\
 str stringwidth pop\
 /sw2 exch def\
 sw2 sw gt {/sw sw2 10 add def} if\
 strh sh gt {/sh strh 10 add def} if\
 grestore \
 sw 0 rlineto\
 0 sh rlineto\
 sw neg 0 rlineto\
 0 sh neg rlineto\
 bgcolor aload pop setrgbcolor false upath ufill\
 1 setlinewidth\
 0 0 0 setrgbcolor stroke\
 grestore\
 sw sw2 sub 2 div sh strh sub 2 div rmoveto\
 str show\
 grestore\
 sw 0 rmoveto\
} def";
*/


    const std::string PostscriptCanvas::PATHTEXT_DEF = "\
/pathtextdict 26 dict def\n\
/pathtext\n\
{ pathtextdict begin\n\
/offset exch def\n\
/str exch def\n\
/pathdist 0 def\n\
/setdist offset def\n\
/charcount 0 def\n\
gsave\n\
flattenpath\n\
{movetoproc} {linetoproc}\n\
    {curvetoproc} {closepathproc}\n\
				     pathforall\n\
				     grestore\n\
				     newpath\n\
				     end\n\
				     } def\n\
				     pathtextdict begin\n\
				     /movetoproc\n\
{ /newy exch def /newx exch def\n\
       /firstx newx def /firsty newy def\n\
       /ovr 0 def\n\
       newx newy transform\n\
       /cpy exch def /cpx exch def\n\
       } def\n\
       /linetoproc\n\
{ /oldx newx def /oldy newy def\n\
       /newy exch def /newx exch def\n\
       /dx newx oldx sub def\n\
       /dy newy oldy sub def\n\
       /dist dx dup mul dy dup mul add\n\
       sqrt def\n\
       dist 0 ne\n\
    {/dsx dx dist div ovr mul def\n\
	  /dsy dy dist div ovr mul def\n\
	  oldx dsx add oldy dsy add transform\n\
	  /cpy exch def /cpx exch def\n\
	  /pathdist pathdist dist add def\n\
	{ setdist pathdist le\n\
	    { charcount str length lt\n\
		{setchar} {exit} ifelse }\n\
		    { /ovr setdist pathdist sub def\n\
			   exit }\n\
				     ifelse\n\
				     } loop\n\
					   } if\n\
						 } def\n\
						 /curvetoproc\n\
{ (ERROR: No curveto's after flattenpath!)\n\
print\n\
} def\n\
/closepathproc\n\
{ firstx firsty linetoproc\n\
firstx firsty movetoproc\n\
} def\n\
/setchar\n\
{ /char str charcount 1 getinterval def\n\
       /charcount charcount 1 add def\n\
       /charwidth char stringwidth pop def\n\
       gsave\n\
       cpx cpy itransform translate\n\
       dy dx atan rotate\n\
       0 0 moveto char show\n\
       currentpoint transform\n\
       /cpy exch def /cpx exch def\n\
       grestore\n\
       /setdist setdist charwidth add def\n\
       } def\n";


const std::string PostscriptCanvas::FRAMETEXT_DEF = "\
       /wordbreak ( ) def\n\
       /BreakIntoLines\n\
{ /proc exch def\n\
       /linelength exch def\n\
       /textstring exch def\n\
       /breaklen wordbreak stringwidth pop\n\
       def\n\
       /curlen 0 def\n\
       /lastwordbreak 0 def\n\
       /startchar 0 def\n\
       /restoftext textstring def\n\
    { restoftext wordbreak search\n\
	{/nextword exch def pop\n\
	      /restoftext exch def\n\
	      /wordlen nextword stringwidth\n\
	      pop def\n\
	      curlen wordlen add linelength lt\n\
	    { /curlen curlen wordlen add\n\
		   breaklen add def }\n\
		       { textstring startchar\n\
			     lastwordbreak startchar sub\n\
			     getinterval proc\n\
			     /startchar lastwordbreak def\n\
			     /curlen wordlen breaklen\n\
			     add def\n\
			     } ifelse\n\
				   /lastwordbreak lastwordbreak\n\
				   nextword length add 1 add def\n\
				   }\n\
				       { pop exit }\n\
						       ifelse\n\
						       } loop\n\
							     /lastchar textstring length def\n\
							     textstring startchar lastchar\n\
							     startchar sub getinterval proc\n\
							     } def\n";











PostscriptCanvas::PostscriptCanvas(std::ostream& output)
: _output (output)
{
}

PostscriptCanvas::~PostscriptCanvas()
{
}


void
PostscriptCanvas::startPage (double x, double y, double width, double height)
{
    _output << "%!PS-Adobe-3.0" << std::endl;
    _output << "%%Title : toto" << std::endl;
    _output << "%%Creator : Synthese Map Module" << std::endl;
    _output << "%%CreationDate: Mon Jul  2 14:17:57 2001" << std::endl;
    _output << "%%BoundingBox: " << x << " " << y << " " << width << " " << height << std::endl;
    _output << "%%DocumentData: Clean7Bit" << std::endl;
    _output << "%%LanguageLevel: 3" << std::endl;
    _output << "%%Orientation: Portrait" << std::endl;
    _output << "%%EndComments" << std::endl;

    _output << PATHTEXT_DEF << std::endl;
    _output << TRIANGLE_DEF << std::endl;
    _output << SQUARE_DEF << std::endl;
    _output << STICKER_DEF << std::endl;

}

void
PostscriptCanvas::moveto (double x, double y)
{
    _output << x << " " << y << " moveto" << std::endl;
}

void
PostscriptCanvas::rmoveto (double x, double y)
{
    _output << x << " " << y << " rmoveto" << std::endl;
}



void
PostscriptCanvas::arct (double x1, double y1, double x2, double y2, double r)
{
    _output << x1 << " " << y1 << " " << x2 << " " << y2 << " "  << r << " arct" << std::endl;
}


void
PostscriptCanvas::setfont (const std::string& fontName, int fontSize) {
    _output << "/" << fontName << " findfont" << std::endl;
    _output << fontSize << " scalefont" << std::endl;
    _output << "setfont" << std::endl;

}



void
PostscriptCanvas::setlinecap (int lineCap) {
    // 0 = butt; 1 = round; 2 = projecting
    _output << lineCap << " setlinecap" << std::endl;
}

void
PostscriptCanvas::setlinejoin (int lineJoin) {
    _output << lineJoin << " setlinejoin" << std::endl;
}



void
PostscriptCanvas:: setlinewidth (int lineWidth) {
    _output << lineWidth << " setlinewidth" << std::endl;
}


void
PostscriptCanvas:: setrgbcolor (double red, double green, double blue) {
    _output << red << " " << green << " " << blue << " setrgbcolor" << std::endl;
}



void
PostscriptCanvas::setrgbcolor (const synthese::util::RGBColor& color)
{
    setrgbcolor (color.r, color.g, color.b);
}



void
PostscriptCanvas::text (const std::string& text) {
    _output << "(" << text << ") show" << std::endl;
}


void
PostscriptCanvas::pathtext (double offset, const std::string& text) {
    _output << "(" << text << ") " << (int) offset << " pathtext" << std::endl;
}





void
PostscriptCanvas:: newpath () {
    _output << "newpath" << std::endl;
}

void
PostscriptCanvas:: closepath () {
    _output << "closepath" << std::endl;
}


void
PostscriptCanvas:: lineto (double x, double y) {
    _output << x << " " << y << " lineto" << std::endl;
}


void
PostscriptCanvas:: curveto (double cp1x, double cp1y, double cp2x, double cp2y, double x, double y) {
    _output << cp1x << " " << cp1y << " " << cp2x
	    << " " << cp2y << " " << x << " " << y << " curveto" << std::endl;
}


void
PostscriptCanvas:: square (double width)
{
    _output << width << " square" << std::endl;
}


void
PostscriptCanvas::stroke () {
    _output << "stroke" << std::endl;
}


void
PostscriptCanvas::fill () {
    _output << "fill" << std::endl;
}

void
PostscriptCanvas::triangle (double width) {
    _output << width << " triangle" << std::endl;
}



void
PostscriptCanvas::showPage () {
    _output << "showpage" << std::endl;
}

void
PostscriptCanvas::gsave () {
    _output << "gsave" << std::endl;
}

void
PostscriptCanvas::grestore () {
    _output << "grestore" << std::endl;
}



void
PostscriptCanvas::scale (double xscale, double yscale) {
    _output << xscale << " " << yscale << " scale" << std::endl;
}

void
PostscriptCanvas::translate (double xtranslate, double ytranslate) {
    _output << xtranslate << " " << ytranslate << " translate" << std::endl;
}

void
PostscriptCanvas::rotate (double rotate) {
    _output << rotate << " " << " rotate" << std::endl;
}


void
PostscriptCanvas::sticker (const std::string& label,
                           const synthese::util::RGBColor& bgColor,
                           double xMargin, double yMargin)
{
    _output << "(" << label << ") " << "[" << bgColor.r << " "
            << bgColor.g << " " << bgColor.b << "] " << xMargin
            << " " << yMargin << " sticker" << std::endl;
}









}
}

