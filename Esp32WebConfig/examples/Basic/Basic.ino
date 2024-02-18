#include "Esp32WebConfig.h"
#include <WiFi.h>


/*

Note: Information about the internal workings of the library are described n\in the header file "Esp32WebConfig.h"

The following example shows how the user can set up the wifi connection parameters and a few other parameters in order
to illustrate how the web interface works.

The example uses 2 button which are used on most esp32 boards.  The board which I am using is the "ESP32-S2-DevKitC-1"
by expressif.  The procedure to start the web interface is to press the reset button and then quickly press the
"Boot" button which is connected to input 0.  Make sure not to press the "boot" button while letting up on the "reset"
button since this will put the processor into "program" mode.

After entering into AP mode the user connects their device to the "ConfigureMe" SSID and uses their internet browser
to connect to the IP address 192.168.4.1 and a simple web page will be displayed which will allow the user to review
and save configuration parameters to NVM.
*/



void setup() {

Esp32WebConfig config;


	Serial.begin(115200);

	/*Here we specify the configuration parameters and obtain a pointer to the
	underlying items so that they can be used in the program
	*/
	
	static auto dynamicTime = config.addNote("This text replaced dynamically");
	config.groupStart("Router config");
	static auto Note1 = config.addNote("The subnet mask is always: 255.255.255.0");
	static auto staticIp = config.addCheckBox("Use static ip", true);
	static auto ip = config.addIpAddr("ip", "192.168.0.1");
	static auto gateway = config.addIpAddr("gateway", "192.168.0.1");
	auto ssid = config.addString("ssid", "");
	auto password = config.addString("password", "");
	config.groupStart("Module Config");
	auto rtuNode = config.addNumber("node", 0, 0, 57);

		HtmlItemBase::setDynamicUpdataCb([]() {
			dynamicTime->setValue(String("Updated at: ") + millis() + " ms");
		String disableStr = staticIp->state ? "" : "disabled";
		ip->extraParams1 = disableStr;
		gateway->extraParams1 = disableStr;
		});


	ArrayList<String> moduleTypes;

	moduleTypes[0] = "tiny";
	moduleTypes[1] = "small";
	moduleTypes[2] = "medium";
	moduleTypes[3] = "large";
	moduleTypes[4] = "XL";

	

	auto moduleType = config.addSelection("moduleType", moduleTypes);

	
	//config.addInputType_Selection("moduleType", moduleTypes);
	//HtmlItem_Selection* s1 = new HtmlItem_Selection("test", moduleTypes);
	//test(moduleTypes);

	/*now retrieve the configuration items from NVM.  If they do not exist yet, the defaults
	which were specified above will be used.
	*/

	config.readFromPrefs();

	//Serial.println(String("type: ") + moduleType->getValue());

	/*This example uses the "boot" button which is found on most ESP32 boards and connected to
	 pin 0.  The procedure to enter into boot mode is to press the "reset" button and the quickly
	 hold down the "boot" button for more that 1 second.  Make sure not to be pressing the "boot"
	 button while letting up on the "reset" button since this will put the processor into "program" mode.

	 */

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
