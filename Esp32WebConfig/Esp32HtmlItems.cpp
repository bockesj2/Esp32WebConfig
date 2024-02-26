/*
See "Esp32WebConfig.h" for notes on library usage
*/

#include "Esp32HtmlItems.h"


int HtmlItemBase::pos = 0;
HtmlItemBase* HtmlItemBase::lastaddedItem = NULL;
int HtmlItemBase::groupingLayers = 0;
int HtmlItemBase::topmostDisabledGroup = 0;

dynamicUpdataCb HtmlItemBase::dynUpdataCb = placeholderForCb;
