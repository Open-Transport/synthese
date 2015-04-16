install(
  DIRECTORY .
  DESTINATION share/synthese/packages
  PATTERN ".svn" EXCLUDE
  PATTERN "*.pyc" EXCLUDE
)
