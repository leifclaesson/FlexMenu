// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"
#include "FlexMenuItemLeave.h"

class FlexMenuItemSlider;

enum eFMISliderCallback
{
	eFMISliderCallback_DisplayValue,
	eFMISliderCallback_ValueChanging,
	eFMISliderCallback_ValueChanged,
};

typedef std::function<int(FlexMenuItemSlider *, eFMISliderCallback, String *)> FlexMenuItemSliderCB;




class FlexMenuItemSlider :
	public FlexMenuBase
{
	FlexMenuItemSliderCB * pfnCallback=NULL;
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

	void SetCallbackFn(FlexMenuItemSliderCB & fnCallback);


	virtual void OnPush() override;

	virtual int GetProgressBar(int iPixelWidth);


	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;


	bool bAdjusting=false;

	bool bModified=false;

	void DoAdjust(int8_t direction, uint8_t accel);


};

