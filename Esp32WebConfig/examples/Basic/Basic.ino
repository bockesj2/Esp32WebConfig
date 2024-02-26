#include "Esp32WebConfig.h"
#include <WiFi.h>


/*

Note: Information about the internal workings of the library are described in the header file "Esp32WebConfig.h"

The following example shows how the user can set up the wifi connection parameters and a few other parameters in order
to illustrate how the web interface works.

The example uses the 2 button which are used on most esp32 boards.  The board which I am using is the "ESP32-S2-DevKitC-1"
by Expressif.  The procedure to start the web interface is to press the reset button and then quickly press the
"Boot" button (which is connected to input 0).  Make sure not to press the "boot" button while letting up on the "reset"
button since this will put the processor into "program" mode.

After entering into AP mode the user connects their device to the "ConfigureMe" SSID and uses their internet browser
to connect to the IP address 192.168.4.1 and a simple web page will be displayed which will allow the user to review
and save configuration parameters to NVM.

Note that while this blockig function is running a callback function is also being run at this higher level and sees
if the user is requesting configuration info through the serial port.  If so, a JSON string is returned which the use can
edit in a text editor and then send back to the processor in order to update the configuration values.
*/
Esp32WebConfig config;

unsigned long lastSerialTimeMs = 0;
String serialStr = "";

//this callback is specified as a parameter in the "startApWebServerBlocking" function.
//it looks for a request of the configuration data which is sent through the serial port
//in JSON  format.  If this data is edited and sent back to the processor, then the configuration data
//will be updated.  Note that the serial monitor should send an carriage return '\n' as the last character
//since this is used to designate the end of the package.  Also JSON strings are enclosed in curly brackets
//and these characters are used to define the JSON string.
void serialWhileBlocking() {

	if (serialStr != "" && ((millis() - lastSerialTimeMs) > 2)) serialStr = "";

	while (Serial.available()) {
		lastSerialTimeMs = millis();
		char c = Serial.read();
			if (c == '\n') {
				if (serialStr[0] == '{') {
					if (config.setConfig(serialStr)) {
						Serial.println("Configuration has been updated");
					}
					else {
						Serial.println("Error updating configuration");
					}
				}
				else {
					serialStr.trim();
					serialStr.toUpperCase();
					if (serialStr == "GETCONFIG") {
						Serial.print(config.getJsonConfig());
					}
					if (serialStr == "HELP") {
						Serial.println();
						Serial.println(R"(Type "GetConfig" to get the current configuration
in JSON format.  Edit this string in a text editor such as notepad
and send in back to the processor
in order to update the configuration using the serial link.)" );
					}
				}
				
			}
			serialStr += c;
			//Serial.print(c);
	}

}

void setup() {
	Serial.begin(115200);

	/*Here we specify the configuration parameters and obtain a pointer to the
	underlying items so that they can be used in the program
	*/
	

	static auto dynamicTime = config.addNote("This text replaced dynamically");  //demonstrates dynamiclly changing text
	static auto enableWifi = config.addCheckBox("Enable Wifi Client", true); //demonstrates an HTML checkbox
	enableWifi->submitOnChange();  //HTML attributes will be specified to "submit on change"
	static auto routerConfigGroup = config.addFieldSet("Router config"); //demonstrates a group box (an HTML fieldset)
	auto ssid = config.addString("ssid", "");  //HTML text field
	auto password = config.addString("password", "");
	static auto staticIp = config.addCheckBox("Use static ip", true); //HTML checkbox
	staticIp->submitOnChange();
	static auto staticIpConfigGroup = config.addFieldSet("Static ip config");
	static auto ip = config.addIpAddr("ip", "192.168.0.1");
	static auto gateway = config.addIpAddr("gateway", "192.168.0.1");
	static auto Note1 = config.addNote("The subnet mask is always: 255.255.255.0");
	config.fieldSetEnd();
	config.fieldSetEnd();

	static auto useNode = config.addCheckBox("Use Node", true);
	useNode->submitOnChange();
	static auto rtuNode = config.addNumber("node", 0, 0, 57);


	//this callback function is called right before the HTML page is created and allows 
	//the attributes of certain fields to be updated dynamically
	//in this case, the time label is updated, the visibility of the node is changed based upon the value of a checkbox,
	//and certio groups of data (HTML fieldsets) are disabled based upon the value of a checkbox
		HtmlItemBase::setDynamicUpdataCb([]() {
			dynamicTime->setValue(String("Updated at: ") + millis() + " ms");
			rtuNode->visible(useNode->getValue());
		routerConfigGroup->disabled(!enableWifi->getValue());
		staticIpConfigGroup->disabled(!staticIp->getValue());
		});

		//implementation of a C++ Arraylist
	ArrayList<String> moduleTypes;


	//fill in values for the HTML selection box
	moduleTypes[0] = "tiny";
	moduleTypes[1] = "small";
	moduleTypes[2] = "medium";
	moduleTypes[3] = "large";
	moduleTypes[4] = "XL";

	auto moduleType = config.addSelection("moduleType", moduleTypes);

	/*now retrieve the configuration items from NVM.  If they do not exist yet, the defaults
	which were specified above will be used.
	*/
	config.readFromPrefs();


	/*This example uses the "boot" button which is found on most ESP32 boards and connected to
	 pin 0.  The procedure to enter into configure mode is to press the "reset" button and the quickly
	 hold down the "boot" button for more that 1 second.  Make sure not to be pressing the "boot"
	 button while letting up on the "reset" button since this will put the processor into "program" mode.
	 */

	//config.startApWebServerBlocking(serialWhileBlocking);  //uncomment to always start the web server and config webpage

	pinMode(0, INPUT_PULLUP);
	delay(1000);  //give the user time to press the "config" button (in this case the "boot" button on my board)

	if (digitalRead(0) == 0) {

		//if the "boot" button has been held down for more that 1 second on start up, then the processor
		//will enter into AP mode with an SSID of "ConfigureMe". The webpage can then be started and the values changed
		//Note that this is a blocking function, so all other process are halted while the Processor is in the 
		//configuration mode.
		Serial.print("Starting WiFi configuration Server");
		config.startApWebServerBlocking();
	}


	//if the processor is not put into configuration mode, then will will tray to connect to a router with the 
	//stored SSID and password and using the specified static IP address.
	if (staticIp->getValue() ){
		Serial.println("Connecting using static ip");
		if (!WiFi.config(ip->getValue(), gateway->getValue(), IPAddress(255, 255, 255, 0))) {
			Serial.println("STA Failed to configure");
		}
	}
	else {
		Serial.println("Connecting using default ip");
	}


	WiFi.begin( ssid->getValue(), password->getValue());

	int cnt = 0;

	Serial.print(String("Connecting to router.  SSID=") + ssid->getValue() + " password=" + password->getValue() );
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		cnt++;
	}

	if (WiFi.status() == WL_CONNECTED) {
		Serial.println(String("Wifi Connected on IP " + WiFi.localIP().toString()));
	}
}

void loop() {
  // put your main code here, to run repeatedly:

}
