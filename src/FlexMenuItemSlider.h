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

/*
 * Example LAMBDA (simple):
 * 		p->fnCallback=[this](FlexMenuItemSliderCB * pSource, eFMISliderCallback code, String * pText) -> int
		{
			(void)(pText);
			if(code==eFMISliderCallback_ValueChanged)
			{
				Serial.printf("Slider %s changed to %i\n",pSource->strTitle.c_str(),pSource->value);
			}
			return 0;
		};
 *
 *
 *
 */


/*
 * Example LAMBDA with display text override
 *
 * 		p->fnCallback=[this](FlexMenuItemSliderCB * pSource, eFMISliderCallback code, String * pText) -> int
		{
			(void)(pText);
			switch(code)
			{
			default:
				break;
			case eFMISliderCallback_ValueChanged:	//happens when the knob is pushed after adjustment
			{
				Serial.printf("Slider \"%s\" changed to %i\n",pSource->strTitle.c_str(),pSource->value);
				break;
			}
			case eFMISliderCallback_ValueChanging:	//happens for every value change as the knob is turned
			{
				break;
			}
			case eFMISliderCallback_DisplayValue:	//Use to override the displayed text, in this case to present a decimal value
			{
				//single decimal.
				(*pText)=String(pSource->value/10);
				(*pText)+='.';
				(*pText)+=String(pSource->value%10);

				//alternative solution using floating point
				//char temp[16];
				//sprintf(temp,"%.01f",pSource->value*0.1);

				break;
			}
			}

			return 0;
		};
 *
 *
 */

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



