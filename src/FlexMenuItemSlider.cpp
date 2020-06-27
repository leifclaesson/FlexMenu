// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemSlider.h"


FlexMenuItemSlider::FlexMenuItemSlider()
{
}


FlexMenuItemSlider::~FlexMenuItemSlider()
{
}

eFlexMenuScreenType FlexMenuItemSlider::GetScreenType()
{
	if(bAdjusting) return eFlexMenuScreenType_Slider;
	else return eFlexMenuScreenType_Normal;
}

bool FlexMenuItemSlider::CanNavigate(eFlexMenuNav direction, uint8_t accel)
{
	switch(direction)
	{
	default:
		break;
	case eFlexMenuNav_Prev:
		if(bAdjusting)
		{
			DoAdjust(-1,accel);
			return false;
		}
	case eFlexMenuNav_Next:
		if(bAdjusting)
		{
			DoAdjust(1,accel);
			return false;
		}
	case eFlexMenuNav_Push:
	case eFlexMenuNav_Back:
		if(bAdjusting)
		{
			bAdjusting=false;
			SetNeedsRefresh(true);
			if(bModified && pfnCallback && *pfnCallback) (*pfnCallback)(this, eFMISliderCallback_ValueChanged, 0);
			bModified=false;
			return false;
		}
		break;
	};

	return true;
}


void FlexMenuItemSlider::OnPush()
{
	bAdjusting=true;

	bModified=false;

	SetNeedsRefresh(true);
}

void FlexMenuItemSlider::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}

void FlexMenuItemSlider::GetValueText(String & strValueDestination)
{
	if(!pfnCallback || !(*pfnCallback) || !(*pfnCallback)(this,eFMISliderCallback_DisplayValue,&strValueDestination))
	{
#ifdef WIN32
		char temp[16];
		sprintf(temp,"%i",value);
		strValueDestination=temp;
#else
		strValueDestination=value;
#endif
	}
}

void FlexMenuItemSlider::DoAdjust(int8_t direction, uint8_t accel)
{
	int multiplier=1;
	if(accel>10)
	{
		multiplier=2;
	}

	if(abs(range_max-range_min)>=4000)
	{
		if(accel>20)
		{
			multiplier=5;
		}

		if(accel>35)
		{
			multiplier=13;
		}

		if(accel>50)
		{
			multiplier=23;
		}

		if(accel>70)
		{
			multiplier=53;
		}

		if(accel>110)
		{
			multiplier=73;
		}

		if(accel>148)
		{
			multiplier=107;
		}
	}
	else
	{
		if(accel>30 && abs(range_max-range_min)>=200)
		{
			multiplier=5;
		}

		if(accel>50 && abs(range_max-range_min)>=500)
		{
			multiplier=13;
		}

		if(accel>80 && abs(range_max-range_min)>=1000)
		{
			multiplier=23;
		}
	}

	int val=value;

	if(range_max<range_min)
	{
		val=value-(direction*multiplier);
		if(val>range_min) val=range_min;
		if(val<range_max) val=range_max;
	}
	else
	{
		val=value+(direction*multiplier);
		if(val<range_min) val=range_min;
		if(val>range_max) val=range_max;
	}

	if(value!=val)
	{
		value=val;
		if(pfnCallback && *pfnCallback) (*pfnCallback)(this,eFMISliderCallback_ValueChanging,0);
		bModified=true;
		SetNeedsRefresh(true);
	}

}

eFlexMenuIcon FlexMenuItemSlider::UseIcon()
{
	if(bAdjusting)
	{
		return eFlexMenuIcon_Circle;
	}

	return eFlexMenuIcon_Blank;
}

int FlexMenuItemSlider::GetProgressBar(int iPixelWidth)
{
	uint8_t downshift_bits=0;

	int fraction=0;

	if(range_max>range_min)
	{
		fraction=((value - range_min)<<(10-downshift_bits)) / ((range_max - range_min)>>downshift_bits);
	}
	else
	{
		fraction=(1<<10) - (((value - range_max)<<(10-downshift_bits)) / ((-(range_max - range_min))>>downshift_bits));
	}

	return (fraction*iPixelWidth)>>10;
}

void FlexMenuItemSlider::SetCallbackFn(FlexMenuItemSliderCB & fnCallback)
{
	if(!pfnCallback) pfnCallback=new FlexMenuItemSliderCB;
	*pfnCallback=fnCallback;
}
