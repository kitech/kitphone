#!/bin/sh

# tested on debain5 i686

# set -x

#env
MYSELF=`readlink -f $0`
MYDIR=`dirname $MYSELF`

#######
gpasswd -a web audio  # for use snd_dummy

apt-get install pkg-config alsa-utils libasound2-dev libpq-dev
apt-get install xvfb x11vnc openbox libx11-dev libxext-dev libxss1 libxss-dev xterm
apt-get install tmux

# start flashpolicy.pl
FPP=`ps ax|grep flashpolicy.pl|grep -v grep`
if [ x"$FPP" = x"" ] ; then
    `readlink -f ./flashpolicy.pl` &
fi

kernel_release=`uname -r`
if [ ! -d /lib/modules/$kernel_release.orig ] ; then
    echo "Backup orignal kernel modules..."
    cp -Ra /lib/modules/$kernel_release /lib/modules/$kernel_release.orig
fi

## compile new module
if [ ! -f alsa-driver-1.0.24.tar.bz2 ] ; then
    wget -c ftp://ftp.alsa-project.org/pub/driver/alsa-driver-1.0.24.tar.bz2
fi
tar xvf alsa-driver-1.0.24.tar.bz2
cd alsa-driver-1.0.24
patch -Np1 < ../snd_dummy_128.patch

# ./configure --with-cards=dummy,usb-audio,hda-intel,hrtimer,rtctimer --with-card-options=all
./configure --with-cards=dummy --with-card-options=all
make
cd -

alsa_dir=`pwd`/alsa-driver-1.0.24
# replace old module
rep_modules="snd-dummy snd-pcm snd-seq snd-page-alloc snd-timer snd-seq-device snd"

## unload module before replace
for mn in $rep_modules
do
    rmmod -v $mn
#    sleep 1
done


cd /lib/modules/$kernel_release.orig
for mn in $rep_modules
do
    mod_rpath=`find  -name $mn.ko`
    if [ x"$mod_rpath" = x"" ] ; then
        echo "not found module: $mn.ko"
    else
        echo $mod_rpath
        new_mod_rpath=`find $alsa_dir/ -name $mn.ko`
        // echo "cp -v $new_mod_rpath --> /lib/modules/$kernel_release/$mod_rpath"
        if [ x"$new_mod_rpath" = x"" ] ; then
            echo "new module not found."
        else
            install -v $new_mod_rpath `dirname /lib/modules/$kernel_release/$mod_rpath`/
        fi
    fi
done
cd -

# load module
modprobe snd_dummy
sleep 1

# output info 
aplay -L
aplay -l | head
aplay -l | tail

## unload module test
for mn in $rep_modules
do
#    rmmod -v $mn
#    sleep 1
    true
done
