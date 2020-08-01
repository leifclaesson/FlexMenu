// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_
#include "FlexMenuBase.h"

class FlexMenuItemEdit;


class FlexMenuItemEditBase :
	public FlexMenuBase
{
public:
	FlexMenuItemEditBase();
	virtual ~FlexMenuItemEditBase();

	// Inherited via FlexMenuBase
	virtual void GetValueText(String & strValueDestination) override;

	virtual eFlexMenuIcon UseIcon() override { return eFlexMenuIcon_Cursor; }

	virtual bool CanEnter() override { return true; }
	virtual void OnEnter() override;

	virtual int16_t GetNumSubItems() override { return 1; }
	virtual FlexMenuBase * GetSubItem(int16_t idx) override { (void)(idx); return GetTempItem(); }

	String strEdit;

	virtual void SetManager(FlexMenuManager * pManager) override;

	FlexMenuManager * pManager=NULL;

	void SetIsPassword(bool bPassword) { if(bPassword) flags |= 0x40; else flags &=(0xFF-0x40); }
	bool IsPassword() { return (flags & 0x40)!=0; }

	void SetSaveable(bool bSaveable) { if(bSaveable) flags |= 0x80; else flags &=(0xFF-0x80); }
	bool IsSaveable() override { return (flags & 0x80)!=0; }
	virtual void GetSaveString(String & strSave) override { strSave=strEdit; }
	virtual bool LoadString(const String & strLoad) override { strEdit=strLoad; OnValueChanged(); return true; };

	virtual eFlexMenuScreenType GetScreenType() { return eFlexMenuScreenType_Edit; }

	virtual void OnValueChanged() { }

	virtual void OskSetString(const String & str) { LoadString(str); }



};

class FlexMenuItemEdit : public FlexMenuItemEditBase
{
public:
	String strTitle;
	void GetTitleText(String & strTitleDestination) { strTitleDestination=strTitle; }

	virtual void SetSaveIdx( uint16_t idx ) override { derived_use_3 = (uint8_t) idx; }
	virtual uint16_t GetSaveIdx( ) override { return derived_use_3; }

};


class FlexMenuItemEditCB;

enum eFMIEditCallback
{
	eFMIEditCallback_ValueChanged,
};

typedef std::function<void(FlexMenuItemEditCB *,eFMIEditCallback)> fn_FlexMenuItemEditCB;

class FlexMenuItemEditCB : public FlexMenuItemEdit
{
public:

	virtual void OnValueChanged() override { if(fnCallback) fnCallback(this,eFMIEditCallback_ValueChanged); }

	fn_FlexMenuItemEditCB fnCallback;

};


class FlexMenuItemEditModPtr : public FlexMenuItemEdit
{
public:

	bool * pModified=0;

	virtual void OnValueChanged() override { if(pModified) *pModified=true; }

};



#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_ */
