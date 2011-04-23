#!/bin/sh

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/serv/stow/pjsip/lib/pkgconfig/
#echo $PKG_CONFIG_PATH
pkg-config --libs libpjproject