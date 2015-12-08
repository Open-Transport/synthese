#!/usr/bin/python2

import os.path
import pyspatialite.dbapi2 as pyspa


def connect_to_spatialite_database(db_file):
    connection = pyspa.connect(db_file)
    return connection

def execute_spatialite_script(connection, script_file):
    with open(script_file, 'r') as content_file:
      sql_script = content_file.read()
      connection.executescript(sql_script)

def disconnect_from_spatialite_database(connection):
    connection.close()

