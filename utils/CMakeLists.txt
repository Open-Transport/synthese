install(
  DIRECTORY .
  DESTINATION share/synthese/utils
  PATTERN ".svn" EXCLUDE
  PATTERN "*.pyc" EXCLUDE
  PATTERN "historical" EXCLUDE
)
