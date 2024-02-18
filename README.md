This library is meant to allow configuration parameters to be changed with a webpage interface.
It is the same idea as the WifiManger library, but the interface is simpler and allows extra parameters to be added
easier.  When the user requests that the interface be started the "startApWebServerBlocking" function is called and
the Esp32 starts an access point (AP) at 192.168.4.1 (note that a static ip is actually assigned) with an SSID of
"ConfigureMe".  In order to view the interface, the user must connect to the Access point on a PC or tablet and connect to the SSID
"ConfigureMe" which should appear in the scan list of their device.  (Note that android tablets tend to immediately drop this
connection in favor of one with an internet connection.  This can usually be resolved by quickly pressing the
"No internet connection" message which briefly pops up and selecting the "stay connected" option).  After connecting to the SSID use an
internet browser (Google Chrome) to connect to the ip address "192.168.4.1".  A simple webpage will appear which will
allow the user to configure to the items which have been specified in the ESP32 code.  Press submit and the variables will be saved.

Note that as the parameters are configured, a pointer to the underlying variables is obtained which allows access to the
configuration values which are stored in non-volatile memory (NVM) or have been submitted using the web page.  Since these variables are
pointers, they will need to be de-referenced when they are used.

Note that the library uses the "preferences" library to access NVM.  The Spiffs has been abandoned in preference for
FatFs type of memory which has caused issues in the past, so I am using the "preferences" library which seems to work
no matter which NVM file formatting is used.
