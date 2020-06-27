// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"
#include "FlexMenuItemLeave.h"

class FlexMenuItemSlider;

typedef std::function<void(FlexMenuItemSlider *, String &)> FlexMenuItemSliderDisplayValueCB;

typedef std::function<void(FlexMenuItemSlider *)> FlexMenuItemSliderValueChangingCB;
typedef std::function<void(FlexMenuItemSlider *)> FlexMenuItemSliderValueChangedCB;


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

	FlexMenuItemSliderDisplayValueCB cbDisplayValue;
	FlexMenuItemSliderValueChangingCB cbValueChanging;
	FlexMenuItemSliderValueChangedCB cbValueChanged;

	FlexMenuItemLeave leave;


	virtual void OnPush() override;

	virtual int GetProgressBar(int iPixelWidth);


	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;

	bool bAdjusting=false;

	bool bModified=false;

	void DoAdjust(int8_t direction, uint8_t accel);



};

