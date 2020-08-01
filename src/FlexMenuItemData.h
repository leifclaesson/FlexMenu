/*
 * FlexMenuItemData.h
 *
 *  Created on: 5 Jul 2020
 *      Author: user
 */

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMDATA_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMDATA_H_

#include <FlexMenuGlobalItems.h>
#include "FlexMenuBase.h"

class FlexMenuDataItems : public FlexMenuBase
{
public:

	virtual void GetTitleText(String & strTitleDestination) override { strTitleDestination="Data Items"; }

	virtual int16_t GetNumSubItems() { return (int16_t) vecDataItems.size(); };
	virtual FlexMenuBase * GetSubItem(int16_t idx) { if(idx>=0 && idx<(int) vecDataItems.size()) return vecDataItems[idx]; else return 0; }

	std::vector<FlexMenuBase *> vecDataItems;

};


class FlexMenuItemDataBase : public FlexMenuBase
{
public:

	virtual void GetTitleText(String & strTitleDestination) override { strTitleDestination=""; }

	virtual bool IsSaveable() override { return true; }
	virtual void GetSaveString(String & strSave) override { strSave=strData; }
	virtual bool LoadString(const String & strLoad) override { strData=strLoad; OnValueChanged(); return true; }

	virtual void OnValueChanged() {};

	String strData;

};

class FlexMenuItemData : public FlexMenuItemDataBase
{
public:

	virtual void GetIdentifier(String & strIdentifier) override { strIdentifier=strID; }
	String strID;
};


enum eFMIDataCallback
{
	eFMIDataCallback_ValueChanged,
};

class FlexMenuItemDataCB;

typedef std::function<void(FlexMenuItemDataCB *,eFMIDataCallback)> fn_FlexMenuItemDataCB;

class FlexMenuItemDataCB : public FlexMenuItemData
{
public:

	virtual void OnValueChanged() override
	{
		if(fnCallback) fnCallback(this,eFMIDataCallback_ValueChanged);
	};

	fn_FlexMenuItemDataCB fnCallback;


};

#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMDATA_H_ */
