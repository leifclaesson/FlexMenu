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


// Minimal class to replace std::vector
template<typename Data>
class Vector {
  size_t d_size; // Stores no. of actually stored objects
  size_t d_capacity; // Stores allocated capacity
  Data *d_data; // Stores data
  public:
    Vector() : d_size(0), d_capacity(0), d_data(0) {}; // Default constructor
    Vector(Vector const &other) : d_size(other.d_size), d_capacity(other.d_capacity), d_data(0) { d_data = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(d_data, other.d_data, d_size*sizeof(Data)); }; // Copy constuctor
    ~Vector() { free(d_data); }; // Destructor
    Vector &operator=(Vector const &other) { free(d_data); d_size = other.d_size; d_capacity = other.d_capacity; d_data = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(d_data, other.d_data, d_size*sizeof(Data)); return *this; }; // Needed for memory management
    void push_back(Data const &x) { if (d_capacity == d_size) resize(); d_data[d_size++] = x; }; // Adds new value. If needed, allocates more space
    size_t size() const { return d_size; }; // Size getter
    Data const &operator[](size_t idx) const { return d_data[idx]; }; // Const getter
    Data &operator[](size_t idx) { return d_data[idx]; }; // Changeable getter
    void clear() { if(d_data) free(d_data); d_data=0; d_capacity=0; d_size=0; };
  private:
    void resize() { d_capacity = d_capacity ? d_capacity*2 : 1; Data *newdata = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(newdata, d_data, d_size * sizeof(Data)); free(d_data); d_data = newdata; };// Allocates double the old space
};


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


FlexMenuBase * GetLeaveItem();
