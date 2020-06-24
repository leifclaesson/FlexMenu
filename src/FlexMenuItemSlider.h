#pragma once
#include "FlexMenuBase.h"
#include "FlexMenuItemLeave.h"

class FlexMenuItemSlider;

typedef std::function<void(FlexMenuItemSlider *, String &)> FlexMenuItemSliderDisplayValueCB;

typedef std::function<void(FlexMenuItemSlider *)> FlexMenuItemSliderValueChangingCB;
typedef std::function<void(FlexMenuItemSlider *)> FlexMenuItemSliderValueChangedCB;

/*
class FMISlider_DummySubItem : public FlexMenuBase
{
public:

	String strReturnTitle;
	String strReturnValue;

	virtual void GetTitleText(String & strTitleDestination) override { strTitleDestination=strReturnTitle; };
	virtual void GetValueText(String & strValueDestination) override { strValueDestination=strReturnValue; };
};
*/


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

	//virtual void OnEnter();
	virtual bool CanEnter() { return false; };

	virtual eFlexMenuScreenType GetScreenType() override;

	virtual bool CanNavigate(eFlexMenuNav direction, uint8_t accel) override;

	/*
	virtual int GetNumSubItems();
	virtual FlexMenuBase * GetSubItem(int idx);
	

	uint8_t iScrollPos=0;
	uint8_t iCurItem=0;


	virtual int GetScrollPos() override;
	virtual int GetCurItem() override;

	virtual void SetScrollPos( int iNewScrollPos ) override;
	virtual void SetCurItem( int iNewCurItem ) override;
	*/
	virtual eFlexMenuIcon UseIcon() override;

	FlexMenuItemSliderDisplayValueCB cbDisplayValue;
	FlexMenuItemSliderValueChangingCB cbValueChanging;
	FlexMenuItemSliderValueChangedCB cbValueChanged;

	FlexMenuItemLeave leave;

	//FMISlider_DummySubItem dummy;


	virtual void OnPush() override;


	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;

	bool bAdjusting=false;

	bool bModified=false;

	void DoAdjust(int8_t direction, uint8_t accel);

};

