// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.
#pragma once

#include <functional>

#ifdef WIN32
#define String std::string
#define millis GetTickCount
#else
#include <Arduino.h>
#ifndef HAS_CSPRINTF
#define csprintf Serial.printf
#endif
#endif


enum eFlexMenuNav
{
	eFlexMenuNav_None,
	eFlexMenuNav_Prev,		//rotary encoder counterclockwise
	eFlexMenuNav_Next,		//rotary encoder clockwise
	eFlexMenuNav_Push,		//rotary encoder push
	eFlexMenuNav_Release,	//rotary encoder release
	eFlexMenuNav_Back,		//discrete back key

	eFlexMenuNav_PushRepeat,	//FlexMenuManager feeds to child objects while rotary encoder is pushed
};


enum eFlexMenuIcon
{
	eFlexMenuIcon_None,
	eFlexMenuIcon_Blank,
	eFlexMenuIcon_LeftArrow,
	eFlexMenuIcon_RightArrow,
	eFlexMenuIcon_Circle,
	eFlexMenuIcon_Selection,
	eFlexMenuIcon_Checked,
	eFlexMenuIcon_Unchecked,
	eFlexMenuIcon_Execute,
	eFlexMenuIcon_Cursor,

	eFlexMenuIcon_Count,
};

enum eFlexMenuScreenType
{
	eFlexMenuScreenType_Normal,
	eFlexMenuScreenType_Slider,
	eFlexMenuScreenType_Edit,
};

enum eFlexMenuEdit
{
	eFlexMenuEdit_INVALID,
	eFlexMenuEdit_CaptureCursor,
	eFlexMenuEdit_Space,
	eFlexMenuEdit_Delete,
	eFlexMenuEdit_Backspace,
	eFlexMenuEdit_Cancel,
	eFlexMenuEdit_OK,
};

class FlexMenuManager;

class FlexMenuBase
{
public:
	FlexMenuBase();
	virtual ~FlexMenuBase();

	virtual bool CanEnter() { return false; }
	virtual bool CanLeave() { return true; }
	virtual bool IsLeave() { return false; }
	virtual bool CanNavigate(eFlexMenuNav direction, uint8_t accel) { (void)(direction); (void)(accel); return true; }

	virtual void OnEnter();
	virtual void OnReturn();
	virtual void OnLeave();

	virtual bool AllowLand() { return true; }

	virtual int GetNumSubItems() { return 0; };
	virtual FlexMenuBase * GetSubItem(int idx) { (void)(idx); return 0; }

	virtual int GetScrollPos() { return 0; }
	virtual int GetCurItem() { return 0; }
	virtual FlexMenuBase * GetCurItemPtr()
	{
		if(GetCurItem()>=0 && GetCurItem()<GetNumSubItems())
		{
			return GetSubItem(GetCurItem());
		}
		return 0;
	}

	virtual void SetScrollPos( int iNewScrollPos ) { (void)(iNewScrollPos); }

	virtual void SetCurItem( int iNewCurItem ) { (void)(iNewCurItem); }

	virtual void GetTitleText(String & strTitleDestination)=0;
	virtual void GetValueText(String & strValueDestination) { strValueDestination=""; }

	virtual eFlexMenuIcon UseIcon() { return eFlexMenuIcon_Blank; }

	virtual void OnPush() {};
	virtual void OnPushChild() {};

	virtual void OnVisibilityChange() {};

	bool GetNeedsRefresh();
	void SetNeedsRefresh(bool bSet);

	bool GetVisible();
	void SetVisible(bool bSet);

	virtual eFlexMenuScreenType GetScreenType() { return eFlexMenuScreenType_Normal; }
	

	virtual void UpdateStatus();

	virtual void SetManager(FlexMenuManager * pManager) { (void)(pManager); }

	virtual bool IsSaveable() { return false; }
	virtual void GetSaveString(String & strSave) {}
	virtual bool LoadString(const String & strLoad) { return false; }


private:


	uint8_t flags=0;

	bool GetLastVisible();
	void SetLastVisible(bool bSet);


};
