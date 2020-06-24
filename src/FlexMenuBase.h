#pragma once

#include <functional>

#ifdef WIN32
#define String std::string
#define millis GetTickCount
#else
#include <Arduino.h>
#endif

enum eFlexMenuNav
{
	eFlexMenuNav_None,
	eFlexMenuNav_Prev,
	eFlexMenuNav_Next,
	eFlexMenuNav_Enter,
	eFlexMenuNav_Back,
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

	eFlexMenuIcon_Count,
};

enum eFlexMenuScreenType
{
	eFlexMenuScreenType_Normal,
	eFlexMenuScreenType_Slider,
};

class FlexMenuBase
{
public:
	FlexMenuBase();
	virtual ~FlexMenuBase();

		
//	virtual FlexMenuBase * GetParent() { return 0; };

	virtual bool CanEnter() { return false; };
	virtual bool CanLeave() { return true; };
	virtual bool IsLeave() { return false; };
	virtual bool CanNavigate(eFlexMenuNav direction, uint8_t accel) { return true; }

	virtual void OnEnter();
	virtual void OnReturn();
	virtual void OnLeave();

	virtual bool AllowLand() { return true; };

	virtual int GetNumSubItems() { return 0; };
	virtual FlexMenuBase * GetSubItem(int idx) { return 0; };

	virtual int GetScrollPos() { return 0; };
	virtual int GetCurItem() { return 0; };
	virtual FlexMenuBase * GetCurItemPtr()
	{
		if(GetCurItem()>=0 && GetCurItem()<GetNumSubItems())
		{
			return GetSubItem(GetCurItem());
		}
		return 0;
	}

	virtual void SetScrollPos( int iNewScrollPos ) {};
	virtual void SetCurItem( int iNewCurItem ) {};

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

private:

	uint8_t flags=0;

	bool GetLastVisible();
	void SetLastVisible(bool bSet);


};
