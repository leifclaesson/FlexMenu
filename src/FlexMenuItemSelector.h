// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include <FlexMenuGlobalItems.h>
#include "FlexMenuBase.h"


class FMISelector_DummySubItem : public FlexMenuBase
{
public:

	String * pReturnTitle=NULL;
	String * pReturnValue=NULL;

	virtual void GetTitleText(String & strTitleDestination) override { strTitleDestination=*pReturnTitle; };
	//virtual void GetValueText(String & strValueDestination) override { strValueDestination=*pReturnValue; };

	virtual eFlexMenuIcon UseIcon() { return icon; };
	eFlexMenuIcon icon;

	virtual bool IsLeave() override { return bIsLeave; }

	bool bIsLeave=false;




};


class FMISelector_Item
{
public:
	FMISelector_Item() {};

	String strText;
	int16_t id;

};

enum eFMISelector_Mode
{
	eFMISelector_Mode_NoSave,
	eFMISelector_Mode_SaveIndex,
	eFMISelector_Mode_SaveText,
	eFMISelector_Mode_SaveID,
};

class FlexMenuItemSelector :
	public FlexMenuBase
{
public:
	FlexMenuItemSelector();
	~FlexMenuItemSelector();

	virtual bool CanEnter() { return true; };

	virtual void OnEnter();
	virtual void OnLeave();

	virtual int16_t GetNumSubItems();
	virtual FlexMenuBase * GetSubItem(int16_t idx);

	virtual void OnValueChanged() {};

	std::vector <FMISelector_Item *> vecItems;

	String strText;

	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;

	FMISelector_Item * GetCurSelectorItem();

	virtual void OnPushChild() override;
	virtual void OnPushChildLeave() override;

	virtual eFlexMenuIcon UseIcon() { return eFlexMenuIcon_RightArrow; };


	virtual int16_t GetScrollPos() override;
	virtual int16_t GetCurItem() override;

	virtual void SetScrollPos( int16_t iNewScrollPos ) override;
	virtual void SetCurItem( int16_t iNewCurItem ) override;

	virtual bool IsActive();

	virtual bool IsSaveable() override { return GetMode()!=eFMISelector_Mode_NoSave; }
	virtual void GetSaveString(String & strSave) override;
	virtual bool LoadString(const String & strLoad) override;

	virtual int16_t GetCurItem_History();
	virtual void HistoryBuffer(uintptr_t * data);

	bool GetLeaveOnSelect() { return (flags & 0x80)!=0; };
	void SetLeaveOnSelect(bool bLeaveOnSelect) { if(bLeaveOnSelect) flags |= 0x80; else flags &=(0xFF-0x80); };

	bool GetInMenu() { return (flags & 0x40)!=0; };
	void SetInMenu(bool bInMenu) { if(bInMenu) flags |= 0x40; else flags &=(0xFF-0x40); };


private:

	int16_t iScrollPos=0;
	int16_t iCurItem=0;
	int16_t iCurItem_History;

public:
	int16_t iCurSel=0;
	void SetMode(eFMISelector_Mode mode) { derived_use_2=mode; }
	eFMISelector_Mode GetMode() { return (eFMISelector_Mode) derived_use_2; };



};

class FlexMenuItemSelectorCB;
typedef std::function<void(FlexMenuItemSelectorCB *)> fn_FlexMenuItemSelectorCB;


class FlexMenuItemSelectorCB : public FlexMenuItemSelector
{
public:
	fn_FlexMenuItemSelectorCB cbOnValueChanged;
	fn_FlexMenuItemSelectorCB cbOnActivityChange;
protected:
	virtual void UpdateStatus() override;

	virtual void OnValueChanged();
	bool bLastActive=false;
};

class FlexMenuItemSelectorModPtr : public FlexMenuItemSelector
{
public:

	bool * pModified=0;

	virtual void OnValueChanged() override { if(pModified) *pModified=true; }

};



