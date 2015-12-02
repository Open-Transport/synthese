# Configure LDD
ldconfig

# If there is no configuration file, use the example
CFG="/etc/sysconfig/synthese"
if [ ! -s $CFG ]; then
  echo "Installing a default configuration file in $CFG"
  cp -v /usr/share/synthese/synthese.example "$CFG" 2>/dev/null || echo "No config file example found"
fi

# If the Synthese data directory doesn't exists, create it
mkdir -pv /var/lib/synthese/

# Create/Update the MySQL UDF plugin (if it exists)
cp -v /usr/lib/mysql_udf_plugin/libsynthese_mysql_udf.so /usr/lib64/mysql/plugin/synthese_mysql_udf.so 2>/dev/null || echo "No mysql UDF plugin found"

# Change the systemd file names with a @, this is a workaround for a current bug in CPack
mv -v /usr/lib/systemd/system/synthese_.service /usr/lib/systemd/system/synthese@.service 2>/dev/null || echo "No synthese@.service found"
mv -v /usr/lib/systemd/system/synthese-watchdog_.service /usr/lib/systemd/system/synthese-watchdog@.service 2>/dev/null || echo "No synthese-watchdog@.service found"

# Make the watchdog executable
chmod +x /usr/share/synthese/bin/watchdog.sh
