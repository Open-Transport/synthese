Import ('env')

s3env = env.Clone ()
if s3env.GetPlatform () == 'posix':
  s3env.Append ( CCFLAGS = ['-fpermissive'] )
  s3env.Append ( CPPDEFINES = ['BOOST_ALL_DYN_LINK'] )

#s3env.Append ( CPPDEFINES = ['BOOST_ALL_NO_LIB'] )
s3env.Append (CPPPATH = [Dir ('src').abspath])

s3env.Append (S3VERSION = '3.1.12a')
s3env.Append(BOOSTVERSION = '1.35')

Export ('s3env')
  
s3env.SubBuild ('src', 'src')
s3env.SubBuild ('test', 'test')
s3env.SubBuild ('doc', 'doc')
s3env.SubBuild ('utils', 'utils')







