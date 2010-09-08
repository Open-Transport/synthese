s3-server:
	scons -j 4 _CPPMODE=debug s3-server.cppbin

s3-server-one-job:
	scons _CPPMODE=debug s3-server.cppbin

admin-interface:
	sqlite3 -init data/interface/common/s3-admin/deb/opt/rcs/common-datas3-interface-admin/install.sql build/debug/dev/synthese3/src/bin/server/config.db3

test-data:
	sqlite3 -init dev/synthese3/test/15_env/test_transport_network.sql build/debug/dev/synthese3/src/bin/server/config.db3

s2-server:
	scons -j 4 _CPPMODE=debug s2-server.cppbin

s2-cgi-client:
	scons -j 4 _CPPMODE=debug s2-cgi-client.cppbin
