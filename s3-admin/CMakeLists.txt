install(
  FILES deb/etc/apache2/conf.d/s3-admin.conf
  DESTINATION ${ETC_PREFIX}etc/apache2/conf.d
)

install(
  DIRECTORY deb/opt/rcs/s3-admin
  DESTINATION .
  PATTERN ".svn" EXCLUDE
)
