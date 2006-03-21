#!/usr/bin/python

import os.path
import os
import sys


deleteStandalonePsTiles = True
deleteTiffTiles = False




if len (sys.argv) < 3:
    print "Usage :"
    print "  subtile.py <inputdir> <tilewidth>x<tileheight> <outputdir>"
    print ""
    print "Requirements :"
    print " * <inputdir> is assumed to conatin tiff and tab files"
    print " * ImageMagick convert tool must be available"
    print " * tiff2ps tool must be available"
    print ""
    print "Example :"
    print "  subtile.py ./25k 303x303 ./tiles"
    print ""
    print "A sub dir is created in output dir with the following"
    print " naming convention : _scalex_scaley"
    print ""
    sys.exit()


inputdir = sys.argv[1]

tiledims = sys.argv[2]
tilexycouple = tiledims.split ('x')

outputdir = sys.argv[3]

if not os.path.exists (outputdir): os.makedirs (outputdir)



scaleDirCreated = False

xscale = 0.0
yscale = 0.0

for f in os.listdir (inputdir):
    if (f.endswith ('.tab')):

        
        tabfile = os.path.join (inputdir, f)
        tiffile = tabfile.replace ('.tab', '.tif')
        
        print "... Processing", str(tabfile), "and", str(tiffile), " ..."
        
        prefix = f.replace ('.tab', '_')
        # Open the tab file and find coordinates
        tabf = open(tabfile, 'r')

        minlatvalue = 999999999
        minlonvalue = 999999999
        maxlatvalue = 0
        maxlonvalue = 0
        minxvalue = 999999999
        minyvalue = 999999999
        maxxvalue = 0
        maxyvalue = 0
        
        for line in tabf:
            if line.startswith ('('):
                line2 = line[1: line.find (')')]
                latloncouple = line2.split (',')
                line3 = line[line.find (')')+1:]
                line3 = line3[line3.find ('(')+1:line3.find (')')]
                xycouple = line3.split (',')

                if int(latloncouple[0]) < int(minlatvalue) :
                    minlatvalue = int (latloncouple[0])
                    
                if int(latloncouple[0]) > int(maxlatvalue) :
                    maxlatvalue = int (latloncouple[0])
                    
                if int(latloncouple[1]) < int(minlonvalue) :
                    minlonvalue = int (latloncouple[1])
                    
                if int(latloncouple[1]) > int(maxlonvalue) :
                    maxlonvalue = int (latloncouple[1])
                    
                if int(xycouple[0]) < int(minxvalue) :
                    minxvalue = int (xycouple[0])
                    
                if int(xycouple[0]) > int(maxxvalue) :
                    maxxvalue = int (xycouple[0])
                    
                if int(xycouple[1]) < int(minyvalue) :
                    minyvalue = int (xycouple[1])
                    
                if int(xycouple[1]) > int(maxyvalue) :
                    maxyvalue = int (xycouple[1])

        print "  Region (lambert) = [", minlatvalue, ",", maxlonvalue, ",", maxlatvalue, ",", minlonvalue, "]"
        print "  Region (pixels)  = [", minxvalue, ",", minyvalue, ",", maxxvalue, ",", maxyvalue, "]"


        xscale = float(maxxvalue - minxvalue) / float(maxlatvalue - minlatvalue)
        yscale = float(maxyvalue - minyvalue) / float(maxlonvalue - minlonvalue)
        print "  Computed xscale = ", xscale
        print "  Computed yscale = ", yscale

        
        if not scaleDirCreated:
            scaleDirCreated = True
            outputdir = os.path.join (outputdir, 'scale_' + str (xscale) + '_' + str (yscale))
            if not os.path.exists (outputdir): os.makedirs (outputdir)
            
            

        # Now split the files into n tiles given the dimension on command
        # line. 
        
        os.system ('convert ' + tiffile + ' -crop ' + tiledims + ' ' + outputdir + '/' + prefix + '%d.tif')


        nbTilesX = (int(maxxvalue) - int(minxvalue)) / int(tilexycouple[0])
        nbTilesY = (int(maxyvalue) - int(minyvalue)) / int(tilexycouple[1])


        imageWidth = int(maxxvalue) - int(minxvalue)
        imageHeight = int(maxyvalue) - int(minyvalue)

        # Check if modulo < 2 ; can be some slight inaccuracy on tile data
        
        if (imageWidth % int(tilexycouple[0]) > 1):
            raise "!! Forbidden tile width : " + str (imageWidth) + " % " + tilexycouple[0] + " != 0 !!"
        
        if (imageHeight % int(tilexycouple[1]) > 1): 
            raise "!! Forbidden tile width : " + str (imageHeight) + " % " + tilexycouple[1] + " != 0 !!"
        
        print "  Nb tiles X       = ", nbTilesX
        print "  Nb tiles Y       = ", nbTilesY

        numTile = 0
        for ty in range (1,nbTilesY+1):
            for tx in range (0,nbTilesX):
                numTile = (nbTilesY - ty) * nbTilesY + tx

                # todo do a round(lat)...
                lat = minlatvalue + tx * (int (maxlatvalue) - int (minlatvalue)) / nbTilesX
                lon = minlonvalue + ty * (int (maxlonvalue) - int (minlonvalue)) / nbTilesY
                lat2 = lat + (int (maxlatvalue) - int (minlatvalue)) / nbTilesX
                lon2 = lon - (int (maxlonvalue) - int (minlonvalue)) / nbTilesY
                filename = 'tile_' + tilexycouple[0] + '_' + tilexycouple[1] + '_' + str(lat) + '_' + str(lon) + '_' + str(lat2) + '_' + str(lon2) + '.tif'

                filename = os.path.join (outputdir, filename)
                
                oldfilename = prefix + str(numTile) + '.tif'
                oldfilename = os.path.join (outputdir, oldfilename)
                
                # Renames the file using according to the coordinates found
                print "    => " + filename
                
                os.rename (oldfilename, filename)

                fileps = filename.replace ('.tif', '.ps')
                os.system ('tiff2ps ' + filename + ' > ' + fileps + '.tmp')

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
                if deleteTiffTiles : os.remove (filename)
    

    #break





