install(
  DIRECTORY .
  DESTINATION share/synthese/projects
  PATTERN ".svn" EXCLUDE
  PATTERN "*.pyc" EXCLUDE
)
