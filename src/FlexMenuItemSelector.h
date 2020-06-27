// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"
#include "FlexMenuItemLeave.h"


class FMISelector_DummySubItem : public FlexMenuBase
{
public:

	String * pReturnTitle=NULL;
	String * pReturnValue=NULL;

	virtual void GetTitleText(String & strTitleDestination) override { strTitleDestination=*pReturnTitle; };
	//virtual void GetValueText(String & strValueDestination) override { strValueDestination=*pReturnValue; };

	virtual eFlexMenuIcon UseIcon() { return icon; };
	eFlexMenuIcon icon;

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
	eFMISelector_Mode_SaveText,
	eFMISelector_Mode_SaveIndex,
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

	virtual int GetNumSubItems();
	virtual FlexMenuBase * GetSubItem(int idx);

	virtual void OnSelChange() {};

	int iCurSel=0;
	std::vector<FMISelector_Item *> vecItems;

	String strText;

	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;

	FMISelector_Item * GetCurSelectorItem();

	virtual void OnPushChild() override;

	virtual eFlexMenuIcon UseIcon() { return eFlexMenuIcon_RightArrow; };


	virtual int GetScrollPos() override;
	virtual int GetCurItem() override;

	virtual void SetScrollPos( int iNewScrollPos ) override;
	virtual void SetCurItem( int iNewCurItem ) override;

	virtual bool IsActive();

	virtual bool IsSaveable() override { return mode!=eFMISelector_Mode_NoSave; }
	virtual void GetSaveString(String & strSave) override;
	virtual bool LoadString(const String & strLoad) override;


	eFMISelector_Mode mode=eFMISelector_Mode_NoSave;

private:

	FlexMenuItemLeave leave;
	FMISelector_DummySubItem dummy;

	int iScrollPos=0;
	int iCurItem=0;

	bool bInMenu=false;


};

class FlexMenuItemSelectorEx;
typedef std::function<void(FlexMenuItemSelectorEx *)> FlexMenuItemSelectorExCB;


class FlexMenuItemSelectorEx : public FlexMenuItemSelector
{
public:
	FlexMenuItemSelectorExCB cbOnSelChange;
	FlexMenuItemSelectorExCB cbOnActivityChange;
protected:
	virtual void UpdateStatus() override;

	virtual void OnSelChange();
	bool bLastActive=false;
};
