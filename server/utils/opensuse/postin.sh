# Configure LDD
ldconfig

# If there is no configuration file, use the example
CFG="/etc/sysconfig/synthese"
if [ ! -s $CFG ]; then
  echo "Installing a default configuration file in $CFG"
  cp /usr/share/synthese/synthese.example /etc/sysconfig/synthese
fi

# If the Synthese data directory doesn't exists, create it
mkdir -p /var/lib/synthese/

# Create/Update the MySQL UDF plugin (if it exists)
cp /usr/lib/mysql_udf_plugin/libsynthese_mysql_udf.so /usr/lib64/mysql/plugin/synthese_mysql_udf.so || echo "No mysql UDF plugin found"

# Change the systemd file names with a @, this is a workaround for a current bug in CPack
mv /usr/lib/systemd/system/synthese_.service /usr/lib/systemd/system/synthese@.service || echo "No synthese@.service found"
mv /usr/lib/systemd/system/synthese-watchdog_.service /usr/lib/systemd/system/synthese-watchdog@.service || echo "No synthese-watchdog@.service found"
