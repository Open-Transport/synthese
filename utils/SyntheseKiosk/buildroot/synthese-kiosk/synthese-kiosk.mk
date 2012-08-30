#############################################################
#
# synthese-kiosk
#
#############################################################

SYNTHESE_KIOSK_VERSION = HEAD
SYNTHESE_KIOSK_SITE = https://extranet.rcsmobility.com/svn/synthese3/trunk/utils/SyntheseKiosk
SYNTHESE_KIOSK_SITE_METHOD = svn
SYNTHESE_KIOSK_DEPENDENCIES = python

define HOST_SYNTHESE_KIOSK_INSTALL_CMDS
	(cd $(@D); PYTHONPATH="$(HOST_DIR)/usr/lib/python$(PYTHON_VERSION_MAJOR)/site-packages"\
	$(HOST_DIR)/usr/bin/python setup.py install --prefix=$(HOST_DIR)/usr)
endef

define SYNTHESE_KIOSK_INSTALL_TARGET_CMDS
	(cd $(@D); PYTHONPATH="$(TARGET_DIR)/usr/lib/python$(PYTHON_VERSION_MAJOR)/site-packages"\
	$(HOST_DIR)/usr/bin/python setup.py install --prefix=$(TARGET_DIR)/usr)
	rm -rf $(TARGET_DIR)/usr/share/synthese_kiosk
	mkdir $(TARGET_DIR)/usr/share/synthese_kiosk
	cp -r package/synthese-kiosk/kiosk_config_template $(TARGET_DIR)/usr/share/synthese_kiosk
	cp package/synthese-kiosk/run_synthese_kiosk.sh $(TARGET_DIR)/usr/bin
	chmod +x $(TARGET_DIR)/usr/bin/run_synthese_kiosk.sh
endef

$(eval $(call GENTARGETS))
$(eval $(call GENTARGETS,host))

