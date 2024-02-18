#include "Esp32WebConfig.h"

HtmlItemBase** Esp32WebConfig::configData = NULL;


int Esp32WebConfig::configCount = 0;
WebServer  Esp32WebConfig::Esp32WebConfigServer(80);
Preferences Esp32WebConfig::prefs;
String Esp32WebConfig::htmlPageSource;

HtmlItemBase* Esp32WebConfig::addHtmlItem(HtmlItemBase* item) {
    configData = (HtmlItemBase**)realloc(configData, sizeof(HtmlItemBase*) * (configCount + 1));
    configData[configCount] = item;
    configCount++;
    return item;
}



void Esp32WebConfig::groupStart(String label) {
    if (label == "") label = "******";
    addHtmlItem(new HtmlItem_Group(label));
}

void Esp32WebConfig::groupEnd() {
    HtmlItemBase::isGrouping = false;
}

HtmlItem_Note* Esp32WebConfig::addNote(String label) {
    HtmlItem_Note* s1 = new HtmlItem_Note(label);
    addHtmlItem(s1);
    return s1;
}

HtmlItem_String* Esp32WebConfig::addString(String label, String defaultvalue, String extraParams) {
    HtmlItem_String* s1 = new HtmlItem_String(label, defaultvalue, extraParams);
    addHtmlItem(s1);
    return s1;
}

HtmlItem_IpAddr* Esp32WebConfig::addIpAddr(String label, String defaultvalue, String extraParams) {
    HtmlItem_IpAddr* s1 = new HtmlItem_IpAddr(label, defaultvalue, extraParams);
    addHtmlItem(s1);
    return s1;
}

HtmlItem_Int* Esp32WebConfig::addNumber(String label, int defaultvalue, int min1, int max1, String extraParams) {
    HtmlItem_Int* s1 = new HtmlItem_Int(label, defaultvalue, min1, max1, extraParams);
    addHtmlItem(s1);
    return s1;
}



HtmlItem_Selection* Esp32WebConfig::addSelection(String label, ArrayList<String>& elements1, String extraParams) {
    HtmlItem_Selection* s1 = new HtmlItem_Selection(label, elements1, extraParams);
    addHtmlItem(s1);
    //Serial.println(String("size: ") + elements1.size );
    //Serial.println(String("Pta ") + elements1[2] );
    return s1;
}

HtmlItem_CheckBox* Esp32WebConfig::addCheckBox(String label, bool defaultvalue, String extraParams) {
    HtmlItem_CheckBox* s1 = new HtmlItem_CheckBox(label, defaultvalue, extraParams);
    addHtmlItem(s1);
    return s1;
}



void Esp32WebConfig::readFromPrefs() {
    prefs.begin("config");
   // Serial.println(String("read prefs ") );
    for (int i = 0; i < configCount; i++) {
 
        if (configData[i]->type == VariableType::STRING) {
            HtmlItem_String* v1 = (HtmlItem_String*)configData[i]; 
            v1->dataStr = prefs.getString(v1->label.c_str(), v1->defaultStr);
            //Serial.println( v1->label + " " + v1->dataStr + String((unsigned long) & v1->dataStr));
        }
        if (configData[i]->type == VariableType::IP) {
            HtmlItem_IpAddr* v1 = (HtmlItem_IpAddr*)configData[i];
            prefs.getBytes((v1->label).c_str(), &(v1->ip), sizeof(v1->ip));
        }
        if (configData[i]->type == VariableType::INT) {
            HtmlItem_Int* v1 = (HtmlItem_Int*)configData[i];
            v1->dataInt = prefs.getInt((v1->label).c_str(), v1->defaultInt);
            //Serial.println(v1->label + " " + v1->dataInt);
        }
        if (configData[i]->type == VariableType::SELECT) {
            HtmlItem_Selection* v1 = (HtmlItem_Selection*)configData[i];
            v1->selectedItem = prefs.getInt(v1->label.c_str(), 0);
            //Serial.println( v1->label + " " + v1->dataStr + String((unsigned long) & v1->dataStr));
        }
         if (configData[i]->type == VariableType::CHECK_BOX) {
            HtmlItem_CheckBox* v1 = (HtmlItem_CheckBox*)configData[i]; 
            v1->state = prefs.getBool((v1->label).c_str(), v1->defaultVal);
            //Serial.println(v1->label + " " + v1->dataInt);
        }
    }
    prefs.end();
}

void Esp32WebConfig::writeToPrefs() {
    prefs.begin("config");
    Serial.println(String("write prefs ") );
    for (int i = 0; i < configCount; i++) {
        
        if (configData[i]->type == VariableType::STRING) {
            HtmlItem_String* v1 = (HtmlItem_String*)configData[i];
           prefs.putString((v1->label).c_str(), v1->dataStr);
           //Serial.println(v1->label + " " + v1->dataStr);
        }
        if (configData[i]->type == VariableType::IP) {
            HtmlItem_IpAddr* v1 = (HtmlItem_IpAddr*)configData[i];
            prefs.putBytes((v1->label).c_str(), &(v1->ip), sizeof(v1->ip));
            //Serial.println(v1->label + " " + (v1->ip).toString() );
        }
        if (configData[i]->type == VariableType::INT) {
            HtmlItem_Int* v1 = (HtmlItem_Int*)configData[i];
            prefs.putInt((v1->label).c_str(), v1->dataInt);
            //Serial.println(v1->label + " " + v1->dataInt);
        }
        if (configData[i]->type == VariableType::SELECT) {
            HtmlItem_Selection* v1 = (HtmlItem_Selection*)configData[i];
            prefs.putInt((v1->label).c_str(), v1->selectedItem);
            //Serial.println(v1->label + " " + v1->selectedItem);
        }
        if (configData[i]->type == VariableType::CHECK_BOX) {
            HtmlItem_CheckBox* v1 = (HtmlItem_CheckBox*)configData[i]; 
            prefs.putBool((v1->label).c_str(), v1->state);
            //Serial.println(v1->label + " " + v1->dataInt);
        }
    }
    prefs.end();
}



void Esp32WebConfig::startApWebServerBlocking(whileServerRunningCb runWhileBlocking) {
    //while (1) {};
    readFromPrefs();

    IPAddress local_ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.onEvent(WiFiEvent);
    WiFi.disconnect();

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP("ConfigMe");

    Serial.print("[+] AP Created with IP Gateway ");
    Serial.println(WiFi.softAPIP());

    Esp32WebConfigServer.begin(80);

    Esp32WebConfigServer.on("/", handleRoot);
    Esp32WebConfigServer.on("/update", []() {
        update();
        });
    Esp32WebConfigServer.onNotFound(handleNotFound);

    while (1) {
        Esp32WebConfigServer.handleClient();
        delay(1);
        if (runWhileBlocking != NULL) runWhileBlocking();
    }

}

void Esp32WebConfig::createHtmlPage(String title, String pageId) {
        htmlPageSource = R"(<html>
            <body>
            <h2>%t</h2>
            <form action = '/%p'>)";

htmlPageSource.replace("%t", title);
htmlPageSource.replace("%p", pageId);

/*
 <p>update Time: )";
    htmlPageSource += String(millis()) + "</p>";
    */

    HtmlItemBase::dynUpdataCb();
    
    for (int i = 0; i < configCount; i++) {
        HtmlItemBase* p1 = configData[i];
        
        htmlPageSource += p1->getHtmlItemText();
    }

    if (HtmlItemBase::isGrouping) {
        htmlPageSource += "</fieldset>\n\r";
    }

    htmlPageSource += R"(<input type = 'submit' value = 'Submit'>
      </form>
      </body>
      </html>)";
}

void Esp32WebConfig::handleRoot() {
    Serial.println("pt0");
    createHtmlPage( "Configure", "update" );
    Esp32WebConfigServer.send(200, "text/html", htmlPageSource);
    //Serial.println("done");
}

void Esp32WebConfig::update() {
    
    if (Esp32WebConfigServer.args() == 0) {
        return Esp32WebConfigServer.send(500, "text/plain", "BAD ARGS");
    }

    for (int i = 0; i < configCount; i++) {
        String label = configData[i]->htmlFriendlyLabel;
        if (Esp32WebConfigServer.hasArg(label)) {
            if (configData[i]->type == VariableType::STRING) {
                HtmlItem_String* v1 = (HtmlItem_String*)configData[i];
                v1->dataStr = (Esp32WebConfigServer.arg(label));

                Serial.println(String("Str: ") + label + " " + v1->dataStr);
            }
            if (configData[i]->type == VariableType::IP) {
                HtmlItem_IpAddr* v1 = (HtmlItem_IpAddr*)configData[i];
                v1->ip.fromString((Esp32WebConfigServer.arg(label)));
                Serial.println(v1->label + " " + v1->ip.toString());
            }
            if (configData[i]->type == VariableType::INT) {
                HtmlItem_Int* v1 = (HtmlItem_Int*)configData[i];
                v1->dataInt = (Esp32WebConfigServer.arg(label)).toInt();
                Serial.println(String("Int: ") + label + " " + v1->dataInt);
            }
            if (configData[i]->type == VariableType::SELECT) {
                HtmlItem_Selection* v1 = (HtmlItem_Selection*)configData[i];
                v1->selectedItem = v1->getPos(Esp32WebConfigServer.arg(label));
                Serial.println(v1->label + " " + v1->selectedItem);
            }
            if (configData[i]->type == VariableType::CHECK_BOX) {
                HtmlItem_CheckBox* v1 = (HtmlItem_CheckBox*)configData[i];
                v1->state = true;
            }
        }
        else {
            if (configData[i]->type == VariableType::CHECK_BOX) {
                HtmlItem_CheckBox* v1 = (HtmlItem_CheckBox*)configData[i];
                v1->state = false;
            }
        }
    }

    //Esp32WebConfigString* v1 = (Esp32WebConfigString*)configData[0];
    //Serial.println(String("Str2: ") + v1->dataStr);


    writeToPrefs();
    //Serial.println(String("Str3: ") + v1->dataStr);
    handleRoot();
    //Serial.println(String("Str4: ") + v1->dataStr);
    Serial.println("pt1");

}

void Esp32WebConfig::handleNotFound() {

    String message = "File Not Found\n\n";
    message += "URI: ";
    message += Esp32WebConfigServer.uri();
    message += "\nMethod: ";
    message += (Esp32WebConfigServer.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += Esp32WebConfigServer.args();
    message += "\n";

    for (uint8_t i = 0; i < Esp32WebConfigServer.args(); i++) {
        message += " " + Esp32WebConfigServer.argName(i) + ": " + Esp32WebConfigServer.arg(i) + "\n";
    }

    Esp32WebConfigServer.send(404, "text/plain", message);
}

void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
    case ARDUINO_EVENT_WIFI_READY:
        Serial.println("WiFi interface ready");
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        Serial.println("Completed scan for access points");
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("WiFi client started");
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        Serial.println("WiFi clients stopped");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("Connected to access point");
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("Disconnected from WiFi access point");
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        Serial.println("Authentication mode of access point has changed");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("Obtained IP address: ");
        Serial.println(WiFi.localIP());
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("Lost IP address and IP address is reset to 0");
        break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
        Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
        break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
        Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
        break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
        Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
        break;
    case ARDUINO_EVENT_WPS_ER_PIN:
        Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
        break;
    case ARDUINO_EVENT_WIFI_AP_START:
        Serial.println("WiFi access point started");
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        Serial.println("WiFi access point  stopped");
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        Serial.println("Client connected");
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        Serial.println("Client disconnected");
        break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
        Serial.println("Assigned IP address to client");
        break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
        Serial.println("Received probe request");
        break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
        Serial.println("AP IPv6 is preferred");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        Serial.println("STA IPv6 is preferred");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
        Serial.println("Ethernet IPv6 is preferred");
        break;
    case ARDUINO_EVENT_ETH_START:
        Serial.println("Ethernet started");
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("Ethernet stopped");
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("Ethernet connected");
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("Ethernet disconnected");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        Serial.println("Obtained IP address");
        break;
    default: break;
    }
}