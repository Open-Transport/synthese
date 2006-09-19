#!/usr/bin/python

import os.path
import os
import sys


deleteStandalonePsTiles = True



if len (sys.argv) < 3:
    print "Usage :"
    print "  rescale.py <inputdir> <scale> <outputdir>"
    print ""
    print "Requirements :"
    print " * <inputdir> is assumed to contain tiff files"
    print " * ImageMagick convert tool must be available"
    print " * tiff2ps tool must be available"
    print ""
    print "Example :"
    print "  rescale.py ./_0.253_0.253 0.5 ."
    print ""
    print "The input dir is assumed to conform to the following"
    print " naming convention : _scalex_scaley"
    print "On the previous example a directory called _0.1265_0.1265"
    print " will be created"
    print ""
    sys.exit()


inputdir = sys.argv[1]

scale = float (sys.argv[2])

outputdir = sys.argv[3]


# Parse the original scale
s = os.path.basename (os.path.normpath (inputdir))
s = s[s.find ('_')+1:]
scalex = float (s[:s.find ('_')])
s = s[s.find ('_')+1:]
scaley = float (s[:s.find ('_')])


newscalex = scalex * scale
newscaley = scaley * scale

outputdir = os.path.join (outputdir, 'scale_' + str(newscalex) + '_' + str(newscaley))
if not os.path.exists (outputdir): os.makedirs (outputdir)



for f in os.listdir (inputdir):
    if (f.endswith ('.tif')):
        
        tiffile = os.path.join (inputdir, f)
        
        print "... Processing", str(tiffile), " ..."
        
        # find back the original tile size in the file name
        s = f[f.find ('_')+1:]
        oldtilewidth = float (s[:s.find ('_')])
        s = s[s.find ('_')+1:]
        oldtileheight = float (s[:s.find ('_')])
        s = s[s.find ('_')+1:]

        newtilewidth = int (oldtilewidth * scale)
        newtileheight = int (oldtileheight * scale)

        newtiffile = os.path.join (outputdir, 'tile_' + str(newtilewidth) + '_' + str(newtileheight) + '_' + s)

        # now resize the file; we do not use ! for now since we prefer to keep tile ratio.
        # more info at http://www.cit.gu.edu.au/~anthony/graphics/imagick6/resize/
        os.system ('convert ' + tiffile + ' -resize ' + str(newtilewidth) + 'x' + str(newtileheight) + ' ' + newtiffile)

        print "    => " + str (newtiffile)

        # generate the ps file

        fileps = newtiffile.replace ('.tif', '.ps')
        os.system ('tiff2ps ' + newtiffile + ' > ' + fileps + '.tmp')

        # Filter all that is not the raw image itself
        tempf = open(fileps + '.tmp', 'r')
        finalf = open (fileps, 'w')
        whole = tempf.read ()

        start = whole.find ('\nfalse 3 colorimage') + 20
        whole = whole[start:]
        end = whole.find ('end')
        whole = whole[0:end]

        finalf.write (whole)
        if deleteStandalonePsTiles : os.remove (fileps + '.tmp')



        







