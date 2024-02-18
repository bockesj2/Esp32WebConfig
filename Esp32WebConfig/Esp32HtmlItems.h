
#ifndef Esp32HtmlItems_H
#define Esp32HtmlItems_H


/*
See "Esp32WebConfig.h" for notes on library usage

*/


#include "Arduino.h"
#include "ArrayList.h"
//#include<array>

enum VariableType {
    INT,
    FLOAT,
    STRING,
    IP,
    GROUP,
    LABEL,
    SELECT,
    CHECK_BOX
};

typedef std::function<void()> dynamicUpdataCb;
static void placeholderForCb() {}

class  HtmlItemBase {
public:
    String label = "";
    VariableType type;
    static int pos;
    int id;
    String htmlFriendlyLabel = "";
    static HtmlItemBase* lastaddedItem;
    String htmlPageSource = "";
    static bool isGrouping;
    bool isGroup = false;
    String extraParams1 = "";
    String extraParams2 = "";

    static dynamicUpdataCb dynUpdataCb;

    static void setDynamicUpdataCb(dynamicUpdataCb dynUpdataCb1){
            dynUpdataCb = dynUpdataCb1;
    }


    HtmlItemBase(String label1) {
        label = label1;
        id = pos;
        pos++;
        for (int i = 0; i < label.length(); i++) {
            if (isAlphaNumeric(label[i])) htmlFriendlyLabel += label[i];
        }
        if (htmlFriendlyLabel == "") htmlFriendlyLabel = "def";
        htmlFriendlyLabel += String(id);
        lastaddedItem = this;
        isGroup = isGrouping;
    }
    static void reset(){
        isGrouping = false;
    }
    virtual String getHtmlItemText() = 0;
};


    

class HtmlItem_Group : public HtmlItemBase {
public:
    HtmlItem_Group(String label1) :HtmlItemBase(label1) {
        type = VariableType::GROUP;
    }
    virtual String getHtmlItemText() {
        String str;
        if (isGrouping) {
            str += "</fieldset>\n\r";
        }

            str += "<fieldset>\n\r";
            str  += "<legend>" + label + "</legend>\n\r";
            isGrouping = true;

       return str;
    }
};

class HtmlItem_Note : public HtmlItemBase {
public:
    HtmlItem_Note(String label1) :HtmlItemBase(label1) {
        type = VariableType::LABEL;
    }

    virtual String getHtmlItemText() {
        String str = String("<p>") + label + "</p>";
        return str;
    }

    String& getValue() { return label; }
    void setValue(String str) { label = str; }

    String& operator()(void) {
        return label;
    }
};


class HtmlItem_String : public HtmlItemBase {
public:
    String dataStr;
    String defaultStr;
    HtmlItem_String(String label1, String defaultVal01, String extraParams01 = "") :HtmlItemBase(label1) {
        type = VariableType::STRING;
        dataStr = defaultVal01;
        defaultStr = defaultVal01;
        extraParams1 = extraParams01;
    }

    virtual String getHtmlItemText() {
        String str = R"(<label for = '%l'>%l2 </label>
      <input type = 'text' id = '%l' name = '%l' value = '%d' %ep1><br>
)";
        str.replace("%l2", label);
        String s1 = htmlFriendlyLabel;
        str.replace("%l", s1 );
        str.replace("%d", dataStr);
        str.replace("%ep1", extraParams1);
        //str.replace("%ep2", extraParams2);
        return str;
    }

    String& getValue() { return dataStr; }

    void setValue(String str) { dataStr = str; }

    String& operator()(void) {
        return dataStr;
    }

};

class HtmlItem_IpAddr : public HtmlItemBase {
public:
    IPAddress ip;
    HtmlItem_IpAddr(String label1, String defaultVal1, String extraParams01 = "") : HtmlItemBase(label1){
        type = VariableType::IP;
        ip.fromString(defaultVal1);
        extraParams1 = extraParams01;
    };

    virtual String getHtmlItemText() {
        String str = R"(<label for = '%l'>%l2 </label>
      <input type = 'text' id = '%l' name = '%l' value = '%d' %ep1 pattern='^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$'><br>
)";
        str.replace("%l2", label);
        String s1 = htmlFriendlyLabel;
        str.replace("%l", s1);
        str.replace("%d", toString());
        str.replace("%ep1", extraParams1);
        str.replace("%ep2", extraParams2);
        return str;
    }

    String toString() {
        return ip.toString();
    }

    IPAddress& getValue() { return ip; }

    void setValue(String str) { ip.fromString(str);}

    IPAddress& operator()(void) {
        return ip;
    }

};

class HtmlItem_Int : public HtmlItemBase {
public:
    int dataInt;
    int defaultInt;
    int min, max;
    //bool isGrouping = false;

    HtmlItem_Int(String label1, int defaultVal1, int min1, int max1, String extraParams01 = "") : HtmlItemBase(label1) {
        min = min1;
        max = max1;
        type = VariableType::INT;
        dataInt = defaultVal1;
        defaultInt = defaultVal1;
        extraParams1 = extraParams01;
    }

    virtual String getHtmlItemText() {
        String str = R"(<label for = '%l'>%l2 </label><br>
      <input type = 'number' id = '%l' name = '%l' value = '%d'  min='%min' max='%max'><br>
)";

        str.replace("%l2", label);
        String s1 = htmlFriendlyLabel;
        str.replace("%l", s1);
        str.replace("%d", String(dataInt));
        str.replace("%min", String(min));
        str.replace("%max", String(max));
        return str;
    }

    int& getValue() { return dataInt; }

    void setValue(int i ) { dataInt = i;}

    int& operator()(void) {
        return dataInt;
    }
};

class HtmlItem_Selection : public HtmlItemBase {
public:
    ArrayList<String> elements;
    int selectedItem = 0;


    /*
    HtmlItem_Selection() :HtmlItemBase("Debug") {
    type = VariableType::SELECT;
    }  //debug
    */

    HtmlItem_Selection(String label1, ArrayList<String>& elements1 ,  String extraParams01 = "") :HtmlItemBase(label1) {
        type = VariableType::SELECT;
        extraParams1 = extraParams01;
        elements = elements1;
    }

    virtual String getHtmlItemText() {
        String str = R"(<label for = '%l'>%l2</label>
<select id = '%l' name = '%l'>)";



        String selStr = "";

            for (int i = 0; i < elements.size; i++) {
                if (i == selectedItem) selStr = "selected";
                else selStr = "";
                str += "<option value='" + elements[i] + "' " + selStr + ">" + elements[i] + "</option>";
            }

            
        str += "</select>";
        str.replace("%l2", label);
        String s1 = htmlFriendlyLabel;
        str.replace("%l", s1);
        return str;
    }

    int getPos(String label) {
        for (int i = 0; i < elements.size; i++) {
            if (elements[i] == label) return i;
        }
        return 0;
    }

    String& operator[](int index)
    {
        if (index >= elements.size) {
            return elements[0];
        }
        return elements[index];
    }

    String getItemName(int index)
    {
        if (index >= elements.size) {
            return elements[0];
        }
        return elements[index];
    }

    int& getValue() { return selectedItem; }

    void setValue(int i ) { selectedItem = constrain(i, 0, (elements.size - 1) );}

    int& operator()(void) {
        return selectedItem;
    }
};

class HtmlItem_CheckBox : public HtmlItemBase {
public:
    bool state;
    bool defaultVal = false;

    HtmlItem_CheckBox(String label1, bool defaultVal1, String extraParams01 = "") : HtmlItemBase(label1) {

        type = VariableType::CHECK_BOX;
        state = defaultVal1;
        defaultVal = defaultVal1;
        extraParams1 = extraParams01;
    }

    virtual String getHtmlItemText() {

        /*
        String str = R"(<label for = '%l'>%l2 </label><br>
      <input type = 'number' id = '%l' name = '%l' value = '%d'  min='%min' max='%max'><br>
)";
*/

        String str = R"(<input type='checkbox' %s id='%l' name='%l' value='State'>
  <label for='%l'>%l2</label><br>
)";
        if (state)  str.replace("%s", "checked" );
        else str.replace("%s", "" );
        str.replace("%l2", label);
        String s1 = htmlFriendlyLabel;
        str.replace("%l", s1);
        return str;
    }

    bool& getValue() { return state; }

    void setValue(bool i ) { state = i;}

    bool& operator()(void) {
        return state;
    }
};


#endif


