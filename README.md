ShairPort-Sync with hue and OpenGL|ES 2
==================================
By [Michael Kwasnicki](mailto:git@kwasi-ich.de)


What it is
----------
This fork of ShairPort-Sync adds three new audio output devices and is aiming at [Raspberry Pi](http://www.raspberrypi.org) as the target platform.

* [hue](http://www.meethue.com/)
* [OpenGL|ES 2](http://www.khronos.org/opengles/)
* I2C for my LED strip HAT [TinyLEDStripPWM](https://github.com/kwasmich/TinyLEDStripPWM)

Actually those devices do not output any audio but are supporting the ambient created by music.


Build Requirements
------------------
Required:
* OpenSSL
* FFTW 3
* curl
* libpng
* jpeglib


Debian/Raspbian users can get the basics with
`apt-get install libssl-dev libavahi-client-dev libasound2-dev libcurl4-openssl-dev libfftw3-dev`

Since the OpenGL|ES 2 code is bound to the VideoCore IV of the Raspberry Pi you may want to opt out this part from the configure script.


Runtime Requirements
--------------------
You must be running avahi-daemon or Howl.

Debian/Raspbian users can get the basics with
`apt-get install avahi-daemon`

You may want to have the IPv6 kernel module loaded. This improves the reachability in some (all?) networks.
Without it I had to wait a timeout of 2 minutes until iTunes was able to connect with shairport-sync.

Just add the line
```
ipv6
```
into `/etc/modules`.



How to get started
------------------
```
autoreconf -i -f
./configure CFLAGS='-I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux' LDFLAGS='-L/opt/vc/lib' --with-gl --with-hue --with-i2c --with-alsa --with-avahi --with-ssl=openssl --with-metadata --with-soxr --with-systemd
make
./shairport-sync -a 'My Shairport Name'
```

The triangle-in-rectangle AirPlay logo will appear in the iTunes status bar of any machine on the network, or on iPod/iPhone play controls screen. Choose your access point name to start streaming to the ShairPort-Sync instance.



Audio Output
------------
For a list of available backends and their options, run `shairport-sync -h`.



hue Audio Output
----------------
To have your hue lights go with the music you will need to provide some information to ShairPort-Sync.

* IP address of the hue Bridge (Ask your router to get the IP address)
* An App name that is registered on the hue Bridge (this is more tricky - you will find a description on the [hue developer](http://www.developers.meethue.com/documentation/getting-started) site)
* Lights that are going to be controlled (The number of the lamp as it is shown in your hue App)

```
shairport-sync -o hue -- -b 192.168.0.56 -i newdeveloper -l 1,6,2,5,3,7,4,8
```


OpenGL|ES 2 Audio Output
------------------------
The OpenGL|ES 2 visualizer does not take any parameters yet.

```
shairport-sync --metadata-pipename=/tmp/shairport-sync-metadata --get-coverart -o gl
```



I2C Audio Output
----------------
This is very special as it requires you to create your own driver board for analogue 12V LED stripes.

More details can be found on my [TinyLEDStripPWM](https://github.com/kwasmich/TinyLEDStripPWM) project page. 

```
shairport-sync -o i2c -- -d /dev/i2c-1 -a 0x3f
```
