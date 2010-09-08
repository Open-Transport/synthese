## @namespace project Project definitions root elements.
## @ingroup forge2Projects

import os.path
import datetime

class PrjVersion :
  
  def __init__ (self, prj, version):
    self.prj = prj
    self.version = version
    self.svn_revision = None
    self.revision = None
  

  def is_locally_modified (self):
    return self.get_svn_revision ()[-2:] == '.0'

  def get_svn_revision (self):
    if self.svn_revision == None: 
      rev = os.popen ('cd ' + self.prj.src_dir + ' && svnversion -cn').readline()  # last changed revision, without newline
      if rev == 'exported': rev = '0'
      if rev.find (':') != -1: 
        rev = rev[rev.find(':')+1:]
      if rev.find ('M') != -1: 
        rev = rev[:rev.find('M')]
        # compute decimal from build timestamp
        rev = rev + '.0' # to mark local modification
      self.svn_revision = rev
    return self.svn_revision


  def get_revision (self):
    if self.revision == None: 
      # take this project + all other project dependencies
      # and merge into one revision number
      lmt = 0
      prjs = self.prj.get_dependency_projects ()
      prjs.append (self.prj)
      total_rev = 0.0
      for prj in prjs:
        if prj == None:
          continue
        if prj.version.is_locally_modified ():
          lmt = max (lmt, prj.env.LastLocallyModified ())
        if self.prj.env.GetPlatform() == 'posix':
          total_rev = total_rev + float (prj.version.get_svn_revision ())
      if lmt > 0:
        total_rev = total_rev + (lmt / 10000000000.0)
      self.revision = str (total_rev)
    return self.revision


  def add_prereq_version (self, prereq_version):
    self.version = self.version + '+' + prereq_version

  def get (self):
    return self.version + '-' + self.get_revision ()




class DBPrjVersion(PrjVersion):
	def __init__ (self, prj, version):
		PrjVersion.__init__(self, prj, version)

	def get(self):
		return self.version + '-' + datetime.datetime.now().strftime("%Y-%m-%d.%H-%M-%S")

