Description of patched files:

Each of the modifed source file inside the patch directory has a .diff file in
the same directory that contains the change from the upstream source
at the time it was forked.
The patched files should be refreshed regularly from the .diff so that we
don't differ too much. And of course, remove the patched files once the upstream
issue is fixed.


geom/GeometryFactory.cpp:
  Memory allocation issue?

headers/geos/platform.h:
headers/geos/platform.h.in:
headers/geos/version.h:

io/WKBReader.cpp:
  only set SRID if hasSRID is true.

operation/distance/DistanceOp.cpp:
  DistanceOp returns a wrong value.
  Synthese bug: https://extranet.rcsmobility.com/issues/12100
  Geos bug: http://trac.osgeo.org/geos/ticket/492
