#
# Copyright (C) 2025 intx82 <intx82@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=h02-gnss-client
PKG_RELEASE:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/meson.mk

define Package/h02-gnss-client
  SECTION:=net
  CATEGORY:=Network
  TITLE:=H02 GNSS Client
  DEPENDS:=+libstdcpp
endef

define Package/h02-gnss-client/description
 H02-Client
endef

define Package/h02-gnss-client/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/h02cl $(1)/usr/bin/
endef


$(eval $(call BuildPackage,h02-gnss-client))