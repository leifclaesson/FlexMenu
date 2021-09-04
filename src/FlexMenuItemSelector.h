// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include <FlexMenuGlobalItems.h>
#include "FlexMenuBase.h"


class FMISelector_DummySubItem : public FlexMenuBase
{
public:

	const char * pReturnTitle=NULL;
	const char * pReturnValue=NULL;

	virtual void GetTitleText(String & strTitleDestination) override { strTitleDestination=pReturnTitle; };
	//virtual void GetValueText(String & strValueDestination) override { strValueDestination=pReturnValue; };

	virtual eFlexMenuIcon UseIcon() { return icon; };
	eFlexMenuIcon icon;

	virtual bool IsLeave() override { return bIsLeave; }

	bool bIsLeave=false;




};


enum eFMISelector_Mode
{
	eFMISelector_Mode_NoSave,
	eFMISelector_Mode_SaveIndex,
	eFMISelector_Mode_SaveText,
	eFMISelector_Mode_SaveID,
};

class FlexMenuItemSelectorBase :
	public FlexMenuBase
{
public:
	FlexMenuItemSelectorBase();
	~FlexMenuItemSelectorBase();

	virtual bool CanEnter() { return true; };

	virtual void OnEnter();
	virtual void OnLeave();

	virtual int16_t GetNumSubItems();
	virtual FlexMenuBase * GetSubItem(int16_t idx);


	virtual void GetValueText(String & strValueDestination) override;

	virtual void OnValueChanged() {};

	virtual void OnPushChild() override;
	virtual void OnPushChildLeave() override;

	virtual eFlexMenuIcon UseIcon() { return eFlexMenuIcon_RightArrow; };

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

	virtual int16_t SelectorItem_GetCount()=0;
	virtual const char * SelectorItem_GetText(int16_t idx)=0;
	virtual int16_t SelectorItem_GetID(int16_t idx)=0;

	virtual int16_t GetCurSel()=0;
	virtual void SetCurSel(int16_t iNewCurSel)=0;

private:

	int16_t iCurItem_History;

public:
	void SetMode(eFMISelector_Mode mode) { flags &= 0b11001111; flags |= (((uint8_t) mode) & 3)<<4; }
	eFMISelector_Mode GetMode() { return (eFMISelector_Mode) ((flags >> 4) & 3); };
/*	void SetMode(eFMISelector_Mode mode) { this->mode=mode; }
	eFMISelector_Mode GetMode() { return mode; };
	eFMISelector_Mode mode;*/
};


class FlexMenuItemSelectorBase8 : public FlexMenuItemSelectorBase
{
public:

	virtual int16_t GetScrollPos() override { return derived_use_1; }
	virtual void SetScrollPos(int16_t iNewScrollPos) override { derived_use_1=(uint8_t) iNewScrollPos; };

	virtual int16_t GetCurItem() override { return derived_use_2; }
	virtual void SetCurItem(int16_t iNewCurItem) { derived_use_2=(uint8_t) iNewCurItem; };

	virtual int16_t GetCurSel() { return ((int16_t) derived_use_3)-1; }
	virtual void SetCurSel( int16_t iNewCurSel )
	{
		int temp=iNewCurSel+1;
		if(temp<0) temp=0; else if(temp>255) temp=255;
		derived_use_3=(uint8_t) temp;
	}

private:
};



class FMISelector_Item
{
public:
	FMISelector_Item() {};

	String strText;
	int16_t id;

};



class FlexMenuItemSelector :
	public FlexMenuItemSelectorBase8
{
public:

	virtual int16_t SelectorItem_GetCount() override { return (int16_t) vecItems.size(); }
	virtual const char * SelectorItem_GetText(int16_t idx) override { return vecItems[idx]->strText.c_str(); }
	virtual int16_t SelectorItem_GetID(int16_t idx) override { return vecItems[idx]->id; }

	FMISelector_Item * GetCurSelectorItem();

	String strTitle;

	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override
	{
		strTitleDestination=strTitle;
	}

	std::vector <FMISelector_Item *> vecItems;

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



