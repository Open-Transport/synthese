#############################################################
#
# synthese-kiosk
#
#############################################################

SYNTHESE_KIOSK_VERSION = HEAD
SYNTHESE_KIOSK_SITE = https://extranet.rcsmobility.com/svn/synthese3/trunk/utils/SyntheseKiosk
SYNTHESE_KIOSK_SITE_METHOD = svn
SYNTHESE_KIOSK_DEPENDENCIES = python

define SYNTHESE_KIOSK_BUILD_CMDS
	(cd $(@D); $(HOST_DIR)/usr/bin/python setup.py build --executable=/usr/bin/python)
endef

define SYNTHESE_KIOSK_INSTALL_TARGET_CMDS
	(cd $(@D); $(HOST_DIR)/usr/bin/python setup.py install --prefix=$(TARGET_DIR)/usr)
	rm -rf $(TARGET_DIR)/etc/synthese_kiosk
	mkdir $(TARGET_DIR)/etc/synthese_kiosk
	cp package/synthese-kiosk/run_synthese_kiosk.sh $(TARGET_DIR)/usr/bin
	chmod +x $(TARGET_DIR)/usr/bin/run_synthese_kiosk.sh
endef

$(eval $(generic-package))
