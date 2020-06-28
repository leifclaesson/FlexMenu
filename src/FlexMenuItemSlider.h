// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"
#include "FlexMenuItemLeave.h"



class FlexMenuItemSlider :
	public FlexMenuBase
{
public:

	FlexMenuItemSlider();
	~FlexMenuItemSlider();

	int value=0;

	int range_min=0;
	int range_max=100;

	String strTitle;

	virtual bool CanEnter() { return false; };

	virtual eFlexMenuScreenType GetScreenType() override;

	virtual bool CanNavigate(eFlexMenuNav direction, uint8_t accel) override;

	virtual eFlexMenuIcon UseIcon() override;


	virtual void OnPush() override;

	virtual int GetProgressBar(int iPixelWidth);


	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;


	bool GetAdjusting() { return (flags & 0x40)!=0; };
	void SetAdjusting(bool bAdjusting) { if(bAdjusting) flags |= 0x40; else flags &=(0xFF-0x40); };

	bool GetModified() { return (flags & 0x80)!=0; };
	void SetModified(bool bModified) { if(bModified) flags |= 0x80; else flags &=(0xFF-0x80); };

	void DoAdjust(int8_t direction, uint8_t accel);

	virtual bool OnDisplayValue(String & strText) { return false; }
	virtual void OnValueChanging() {}
	virtual void OnValueChanged() {}

	virtual bool IsSaveable() { return true; }
	virtual void GetSaveString(String & strSave) override;
	virtual bool LoadString(const String & strLoad) override;


};


class FlexMenuItemSliderEx;

enum eFMISliderCallback
{
	eFMISliderCallback_DisplayValue,
	eFMISliderCallback_ValueChanging,
	eFMISliderCallback_ValueChanged,
};

typedef std::function<int(FlexMenuItemSliderEx *, eFMISliderCallback, String *)> FlexMenuItemSliderExCB;


class FlexMenuItemSliderEx : public FlexMenuItemSlider
{
public:


	virtual bool OnDisplayValue(String & strText) override { if(fnCallback) return fnCallback(this,eFMISliderCallback_DisplayValue,&strText); else return false; }
	virtual void OnValueChanging() override { if(fnCallback) fnCallback(this,eFMISliderCallback_ValueChanging,0); }
	virtual void OnValueChanged() override { if(fnCallback) fnCallback(this,eFMISliderCallback_ValueChanged,0); }

	FlexMenuItemSliderExCB fnCallback;


};



