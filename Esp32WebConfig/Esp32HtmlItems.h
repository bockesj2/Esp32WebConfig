
#ifndef Esp32HtmlItems_H
#define Esp32HtmlItems_H


/*
See "Esp32WebConfig.h" for notes on library usage

*/


#include "Arduino.h"
#include "ArrayList.h"
//#include<array>


// The base object for each HtmlItem is "HtmlItemBase".  The VarableType tells the functions which 
//loop through all of the HTML items how to cast the item and how to handle the data which is stored
//in a particular item.
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

    bool isDisabled = false;
    bool submitOnChangeVal = false;
    bool isVisible = true;
    

    String label = "";
    VariableType type;
    static int pos;
    int id;

    //HTML labels cannot contain spaces or strange characters, so get rid of all of the fluff
    //and also add a number to the end of the item to guarantee that it is unique
    String htmlFriendlyLabel = "";
    static HtmlItemBase* lastaddedItem;
    String htmlPageSource = "";
    String userAttributes = "";


    // the "grouping" is done with HTML "fieldset" items
    //i f a fieldset is disabled then all items in the fielset are also disabled
    //some items such as checkboxes need to know if they are disabled in this manner
    //so that they can tell if their value wasn't submitted due to it being disabled or
    //due to the checkbox not being checked
    static int groupingLayers;
    static int topmostDisabledGroup;
 
    static dynamicUpdataCb dynUpdataCb;


    //this function is called before the html page is created
    //and allows the upper levels to dynamically update things such as 
    //checked checked, visibility, etc
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
    }
    static void reset(){
        groupingLayers = 0;
        topmostDisabledGroup = 0;
    }
    virtual String getHtmlItemText() = 0;


    // attributes can manually be specified when the item is created (or adjusted dynamically in the variable "userAttributes"
    //other attributes are set based on whether the HTML item has been disabled or causes a subbmittal upon change, etc
    virtual String getAttributeStr() {
        String str = userAttributes + (isDisabled?" disabled ":"") + " " + (submitOnChangeVal?R"( onchange = 'this.form.submit()' )":"");
         return str;
    }


    //the higher level classes call this base funtion to replace certain tags which are
    //created in the HTML description of this item
    virtual String replaceInHtml(String& HtmlStr) {
        HtmlStr.replace("%label", label);
        String s1 = htmlFriendlyLabel;
        HtmlStr.replace("%tag", s1 );
        HtmlStr.replace("%attr", getAttributeStr() );
        return HtmlStr;
    }

    void disabled(bool checked = true) {
        isDisabled = checked;
    }

    void submitOnChange(bool checked = true) {
        submitOnChangeVal = checked;
    }

    void visible(bool checked = true) {
        isVisible = checked;
    }
};


    
//if label == "" then this object signifies the end of the grouping
// the fieldset is an HTML item which draws a group box around a set of related items
//note that the attributes of the contained items reflects the checked of the field set item
//for example if the fieldset is disabled then all items contained within that fieldset are also disabled.
class HtmlItem_FieldSet : public HtmlItemBase {
public:
    HtmlItem_FieldSet(String label1) :HtmlItemBase(label1) {
        type = VariableType::GROUP;
    }
    virtual String getHtmlItemText() {
        if (!isVisible) return "";
        String str;
        if (label != "") {
            str = "<fieldset %attr>\n\r";
            str += "<legend>" + label + "</legend>\n\r";
            groupingLayers += 1;
            if (isDisabled && topmostDisabledGroup == 0) topmostDisabledGroup = groupingLayers;
        }
        else {
            str = "</fieldset>\n\r";
            if (groupingLayers == topmostDisabledGroup) topmostDisabledGroup = 0;
            if( groupingLayers > 0) groupingLayers -= 1;
        }
       return replaceInHtml(str);
    }
};


//A simple text HTML item
class HtmlItem_Note : public HtmlItemBase {
public:
    HtmlItem_Note(String label1) :HtmlItemBase(label1) {
        type = VariableType::LABEL;
    }

    virtual String getHtmlItemText() {
        if (!isVisible) return "";
        String str = String("<p>") + label + "</p>\r\n";
        return str;
    }

    String& getValue() { return label; }
    void setValue(String str) { label = str; }

    String& operator()(void) {
        return label;
    }
};

//A text box type of HTML item which stores this string in the dataStr variable
class HtmlItem_String : public HtmlItemBase {
public:
    String dataStr;
    String defaultStr;
    HtmlItem_String(String label1, String defaultVal01, String extraParams01 = "") :HtmlItemBase(label1) {
        type = VariableType::STRING;
        dataStr = defaultVal01;
        defaultStr = defaultVal01;
        userAttributes = extraParams01;
    }

    virtual String getHtmlItemText() {
        if (!isVisible) return "";
        String str = R"(<label for = '%tag'>%label </label>
      <input type = 'text' id = '%tag' name = '%tag' value = '%value' %attr><br>
)";
        return replaceInHtml(str);
    }

    String& getValue() { return dataStr; }

    void setValue(String str) { dataStr = str; }

    String& operator()(void) {
        return dataStr;
    }

    virtual String replaceInHtml(String& HtmlStr) {
        HtmlItemBase::replaceInHtml(HtmlStr);
        HtmlStr.replace("%value", dataStr);
        return HtmlStr;
    }

};

//A specialized HTML text box which only accepts an ip addess and stores this in an IPAddess variable
class HtmlItem_IpAddr : public HtmlItemBase {
public:
    IPAddress ip;
    HtmlItem_IpAddr(String label1, String defaultVal1, String extraParams01 = "") : HtmlItemBase(label1){
        type = VariableType::IP;
        ip.fromString(defaultVal1);
        userAttributes = extraParams01;
    };

    virtual String getHtmlItemText() {
        if (!isVisible) return "";
        String str = R"(<label for = '%tag'>%label </label>
      <input type = 'text' id = '%tag' name = '%tag' value = '%value' %attr pattern='^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$'><br>
)";
        return replaceInHtml(str);
    }

    String toString() {
        return ip.toString();
    }

    IPAddress& getValue() { return ip; }

    void setValue(String str) { ip.fromString(str);}

    IPAddress& operator()(void) {
        return ip;
    }

    virtual String replaceInHtml(String& HtmlStr) {
        HtmlItemBase::replaceInHtml(HtmlStr);
        HtmlStr.replace("%value", ip.toString());
        return HtmlStr;
    }

};

//A HTML number textbox which stores this string in the dataInt variable
class HtmlItem_Int : public HtmlItemBase {
public:
    int dataInt;
    int defaultInt;
    int min, max;

    HtmlItem_Int(String label1, int defaultVal1, int min1, int max1, String extraParams01 = "") : HtmlItemBase(label1) {
        min = min1;
        max = max1;
        type = VariableType::INT;
        dataInt = defaultVal1;
        defaultInt = defaultVal1;
        userAttributes = extraParams01;
    }

    virtual String getHtmlItemText() {
        if (!isVisible) return "";
        String str = R"(<label for = '%tag'>%label </label><br>
      <input type = 'number' id = '%tag' name = '%tag' value = '%value'  %attr'><br>
)";

        return replaceInHtml(str);
    }

    int& getValue() { return dataInt; }

    void setValue(int i ) { dataInt = i;}

    int& operator()(void) {
        return dataInt;
    }

    virtual String getAttributeStr() {
        String str = HtmlItemBase::getAttributeStr() + " min=" + min + " max=" + max;
        return str;
    }

    virtual String replaceInHtml(String& HtmlStr) {
        HtmlItemBase::replaceInHtml(HtmlStr);
        HtmlStr.replace("%value", String(dataInt));
        return HtmlStr;
    }
};

//A HTML selection textbox which requires the user to select from a list of item.  The index of the item is
// stored in the "selectedItem" variable.  Note that a C++ Arraylist object is used and the header file
//for this dependancy must be included
class HtmlItem_Selection : public HtmlItemBase {
public:
    ArrayList<String> elements;
    int selectedItem = 0;

    HtmlItem_Selection(String label1, ArrayList<String>& elements1 ,  String extraParams01 = "") :HtmlItemBase(label1) {
        type = VariableType::SELECT;
        userAttributes = extraParams01;
        elements = elements1;
    }

    virtual String getHtmlItemText() {
        if (!isVisible) return "";
        String str = R"(<label for = '%tag'>%label</label>
<select id = '%tag' name = '%tag'>
)";

        String selStr = "";

            for (int i = 0; i < elements.size; i++) {
                if (i == selectedItem) selStr = "selected";
                else selStr = "";
                str += "<option value='" + elements[i] + "' " + selStr + ">" + elements[i] + "</option>";
            }

            
        str += "</select><br>\r\n";
        return replaceInHtml(str);
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

//An HTML CheckBox item. The value of the checkbox is stored in the "checked" variable
class HtmlItem_CheckBox : public HtmlItemBase {
public:
    bool checked;
    bool defaultVal = false;
    bool isInDisabledFieldset = false;

    HtmlItem_CheckBox(String label1, bool defaultVal1, String extraParams01 = "") : HtmlItemBase(label1) {

        type = VariableType::CHECK_BOX;
        checked = defaultVal1;
        defaultVal = defaultVal1;
        userAttributes = extraParams01;
    }

    virtual String getHtmlItemText() {
        if (!isVisible) return "";
        isInDisabledFieldset = (topmostDisabledGroup != 0);
        String str = R"(<input type='checkbox' id='%tag' name='%tag' value='checked' %attr>
  <label for='%tag'>%label</label><br>
)";
        str = replaceInHtml(str);
         //if(submitOnChangeVal) Serial.println(String("pt1 ") + str);
         return str;
    }

    bool& getValue() { return checked; }

    void setValue(bool i ) { checked = i;}

    bool& operator()(void) {
        return checked;
    }
    virtual String getAttributeStr() {
        String str = HtmlItemBase::getAttributeStr();
        str += checked?" checked ":"";
        return str;
    }
};


#endif


