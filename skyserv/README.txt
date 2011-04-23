Requirement:
        >=libpq-8.x
        >=Qt 4.5, qmake, qt-core, qt-gui, qt-network
        >=pjproject-1.8.5

debian:
    apt-get install pkg-config alsa-utils libasound2-dev libpq-dev
    apt-get install xvfb x11vnc openbox libx11-dev libxext-dev libxss1 libxss-dev xterm

modules:
   modprobe snd_dummy
   rmmod snd_dummy snd_pcm snd_seq snd_page_alloc snd_timer snd_seq_device snd soundcore

