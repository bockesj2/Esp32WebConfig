
#ifndef Esp32WebConfig_H
#define Esp32WebConfig_H


/*
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



*/


#include "Arduino.h"
#include "Esp32HtmlItems.h"
#include <ESPmDNS.h>
#include <Preferences.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>

void WiFiEvent(WiFiEvent_t event);  //debug
class Esp32WebConfig {

    static HtmlItemBase* addHtmlItem(HtmlItemBase* item);

public:

    
    ~Esp32WebConfig(){
        for (int i = 0; i < configCount; i++) {
            delete configData[i];
        }
        free( configData );
    }
    int size() {
        return configCount;
    }

    static HtmlItemBase** configData;
    static int configCount;
    static WebServer Esp32WebConfigServer;
    static Preferences prefs;
    static String htmlPageSource;

    
    static void groupStart(String label);
    static void groupEnd();
    static HtmlItem_Note* addNote(String label);
    static HtmlItem_String* addString(String label, String defaultvalue, String extraParams = "");
    static HtmlItem_IpAddr* addIpAddr(String label, String defaultvalue, String extraParams = "");
    static HtmlItem_Int* addNumber(String label, int defaultvalue, int min1, int max1, String extraParams = "");
    static HtmlItem_Selection* addSelection(String label, ArrayList<String>& elements1, String extraParams = "");
    static HtmlItem_CheckBox* addCheckBox(String label, bool defaultvalue, String extraParams = "");
    

    static void readFromPrefs();
    static void writeToPrefs();

    typedef std::function<void(void)> whileServerRunningCb;
    void startApWebServerBlocking(whileServerRunningCb runWhileBlocking = NULL);

    static void createHtmlPage(String Title, String pageId);
    static void handleRoot();
    static void update();
    static void handleNotFound();
};
#endif


