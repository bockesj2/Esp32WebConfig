#include "Esp32HtmlItems.h"


int HtmlItemBase::pos = 0;
HtmlItemBase* HtmlItemBase::lastaddedItem = NULL;
bool HtmlItemBase::isGrouping = false;

dynamicUpdataCb HtmlItemBase::dynUpdataCb = placeholderForCb;
