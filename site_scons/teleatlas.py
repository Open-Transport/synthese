## @namespace teleatlas Teleatlas data extraction projects.
## @ingroup forge2Projects

from project import *
from debian import *
from tools import *

class TeleAtlasS3DataProject (Project):

    def __init__ (self, env, name, version='0.0'):
      Project.__init__ (self, env, name, version)
      DerivedDebianPaquetProject(self, '.')


    def get_type (self):
      return 'teleatlass3'


    def declare (self):

		#nwmif = senv.Command ('nw.mif', senv.Glob ('*nw.mif.gz', dir = 'res', abs_path = True), 'gunzip -c $SOURCE > $TARGET')[0]
		#nwmid = senv.Command ('nw.mid', senv.Glob ('*nw.mid.gz', dir = 'res', abs_path = True), 'gunzip -c $SOURCE > $TARGET')[0]
		#jcmif = senv.Command ('jc.mif', senv.Glob ('*jc.mif.gz', dir = 'res', abs_path = True), 'gunzip -c $SOURCE > $TARGET')[0]
		#jcmid = senv.Command ('jc.mid', senv.Glob ('*jc.mid.gz', dir = 'res', abs_path = True), 'gunzip -c $SOURCE > $TARGET')[0]
		#gcmif = senv.Command ('gc.mif', senv.Glob ('*gc.mif.gz', dir = 'res', abs_path = True), 'gunzip -c $SOURCE > $TARGET')[0]
		#gcmid = senv.Command ('gc.mid', senv.Glob ('*gc.mid.gz', dir = 'res', abs_path = True), 'gunzip -c $SOURCE > $TARGET')[0]
		#pbsmid = senv.Command ('pbs.mid', senv.Glob ('bus_stops_projected.mid.gz', dir = 'res', abs_path = True), 'gunzip -c $SOURCE > $TARGET')[0]
		#pssmid = senv.Command ('pss.mid', senv.Glob ('subway_stops_projected.mid.gz', dir = 'res', abs_path = True), 'gunzip -c $SOURCE > $TARGET')[0]

		#convertpy = senv.Glob ('convert.py', dir = 'src', abs_path = True)[0]
		#sources = [nwmif, nwmid, jcmif, jcmid, gcmif, gcmid, pbsmid, pssmid, convertpy]
		#sqldump = senv.Command (senv.File ('install.sql'),
		#							sources,
		#							'source /etc/opt/rcs/s3-server.conf && python2.5 ' + convertpy + 
		#							' ' + nwmif.abspath + ' ' + nwmid.abspath + ' ' + jcmif.abspath + ' ' + jcmid.abspath + ' ' + gcmif.abspath + ' ' + gcmid.abspath +
		#							' ' + pbsmid.abspath + ' ' + pssmid.abspath + ' $$S3_SERVER_DB_FILE > $TARGET')
		#files.append (sqldump)


                    #prereq = [ DebPrereq (senv,'rcs-tisseo-data-transport-s3') ],
                    #dependencies = ['s3-server.cppbin', 'sqlite3.cppbin'] 
      return
      