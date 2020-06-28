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

	virtual void OnSelChange() {};

	std::vector <FMISelector_Item *> vecItems;

	String strText;

	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;

	FMISelector_Item * GetCurSelectorItem();

	virtual void OnPushChild() override;

	virtual eFlexMenuIcon UseIcon() { return eFlexMenuIcon_RightArrow; };


	virtual int16_t GetScrollPos() override;
	virtual int16_t GetCurItem() override;

	virtual void SetScrollPos( int16_t iNewScrollPos ) override;
	virtual void SetCurItem( int16_t iNewCurItem ) override;

	virtual bool IsActive();

	virtual bool IsSaveable() override { return GetMode()!=eFMISelector_Mode_NoSave; }
	virtual void GetSaveString(String & strSave) override;
	virtual bool LoadString(const String & strLoad) override;

private:

	int16_t iScrollPos=0;
	int16_t iCurItem=0;

public:
	int16_t iCurSel=0;
	void SetMode(eFMISelector_Mode mode) { derived_use_2=mode; }
	eFMISelector_Mode GetMode() { return (eFMISelector_Mode) derived_use_2; };



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
