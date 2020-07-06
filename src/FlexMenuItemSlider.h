// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include <FlexMenuGlobalItems.h>
#include "FlexMenuBase.h"




class FlexMenuItemSlider :
	public FlexMenuBase
{
public:

	FlexMenuItemSlider();
	~FlexMenuItemSlider();

	int value=0;
	int history_value=-1;

	int range_min=0;
	int range_max=100;

	String strTitle;

	virtual bool CanEnter() override;
	virtual void OnEnter() override;
	virtual void OnLeave() override;

	virtual eFlexMenuScreenType GetScreenType() override;

	virtual bool CanNavigate(eFlexMenuNav direction, uint8_t accel) override;

	virtual int GetProgressBar(int iPixelWidth);


	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;

	virtual int16_t GetNumSubItems() override { return 1; };
	virtual FlexMenuBase * GetSubItem(int16_t idx) override { (void)(idx); return GetTempItem(); }


	bool GetModified() { return (flags & 0x80)!=0; };
	void SetModified(bool bModified) { if(bModified) flags |= 0x80; else flags &=(0xFF-0x80); };

	void DoAdjust(int8_t direction, uint8_t accel);

	virtual bool OnDisplayValue(String & strText) { (void)(strText); return false; }
	virtual void OnValueChanging() {}
	virtual void OnValueChanged() {}

	void SetSaveable(bool bSaveable) { if(bSaveable) flags |= 0x40; else flags &=(0xFF-0x40); }
	bool IsSaveable() override { return (flags & 0x40)!=0; }
	virtual void GetSaveString(String & strSave) override;
	virtual bool LoadString(const String & strLoad) override;

	virtual void ClearHistoryBuffer(uintptr_t * data, int count) override;
	virtual void HistoryBuffer(uintptr_t * data) override;

	virtual void SetSaveIdx( uint16_t idx ) override { derived_use_3 = (uint8_t) idx; }
	virtual uint16_t GetSaveIdx( ) override { return derived_use_3; }


};


class FlexMenuItemSliderCB;

enum eFMISliderCallback
{
	eFMISliderCallback_DisplayValue,
	eFMISliderCallback_ValueChanging,
	eFMISliderCallback_ValueChanged,
};

typedef std::function<int(FlexMenuItemSliderCB *, eFMISliderCallback, String *)> fn_FlexMenuItemSliderCB;


class FlexMenuItemSliderCB : public FlexMenuItemSlider
{
public:


	virtual bool OnDisplayValue(String & strText) override { if(fnCallback) return fnCallback(this,eFMISliderCallback_DisplayValue,&strText); else return false; }
	virtual void OnValueChanging() override { if(fnCallback) fnCallback(this,eFMISliderCallback_ValueChanging,0); }
	virtual void OnValueChanged() override { if(fnCallback) fnCallback(this,eFMISliderCallback_ValueChanged,0); }

	fn_FlexMenuItemSliderCB fnCallback;
};

class FlexMenuItemSliderModPtr : public FlexMenuItemSlider
{
public:

	bool * pModified=0;

	virtual void OnValueChanged() override { if(pModified) *pModified=true; }

};



