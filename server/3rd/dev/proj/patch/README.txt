Description of patched files:

See geos/patch/README.txt for background about this this.

patch/src/pj_mutex.c:
  To fix "correction mutex creation on win32 to be in unlocked state"
  (http://trac.osgeo.org/proj/ticket/63, only affects Windows). 
  from http://trac.osgeo.org/proj/changeset/1788
