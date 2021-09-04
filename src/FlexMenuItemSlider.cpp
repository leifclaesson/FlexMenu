// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include <FlexMenuItemSlider.h>
#include "stdafx.h"
#include "FlexMenuGlobalItems.h"


FlexMenuItemSliderBase::FlexMenuItemSliderBase()
{
	SetSaveable(true);
}


FlexMenuItemSliderBase::~FlexMenuItemSliderBase()
{
}

eFlexMenuScreenType FlexMenuItemSliderBase::GetScreenType()
{
	return eFlexMenuScreenType_Slider;
}

bool FlexMenuItemSliderBase::CanNavigate(eFlexMenuNav direction, uint8_t accel)
{
	switch(direction)
	{
	default:
		break;
	case eFlexMenuNav_Prev:
		DoAdjust(-1,accel);
		return false;
	case eFlexMenuNav_Next:
		DoAdjust(1,accel);
		return false;
	};

	return true;
}

bool FlexMenuItemSliderBase::CanEnter()
{
	return true;
}


void FlexMenuItemSliderBase::OnEnter()
{
	SetModified(false);

	FlexMenuTempItem & item=*((FlexMenuTempItem *)GetTempItem());

	item.Reset();
	item.pParent=this;
	item.bIsLeave=true;

}

void FlexMenuItemSliderBase::OnLeave()
{
	if(GetModified())
	{
		if(GetHistoryValue()!=(int) 0x80000000)
		{
			SetValue(GetHistoryValue());
		}
		OnValueChanged();
	}
	SetModified(false);
}

void FlexMenuItemSliderBase::GetValueText(String & strValueDestination)
{
	if(!OnDisplayValue(strValueDestination))
	{
#ifdef WIN32
		char temp[16];
		sprintf(temp,"%i",GetValue());
		strValueDestination=temp;
#else
		strValueDestination=String(GetValue(),10);
#endif
	}
}

void FlexMenuItemSliderBase::DoAdjust(int8_t direction, uint8_t accel)
{
	int multiplier=1;
	if(accel>10)
	{
		multiplier=2;
	}


	if(GetAbsRange()>=4000)
	{
		if(accel>20)
		{
			multiplier=5;
		}

		if(accel>35)
		{
			multiplier=13;
		}

		if(GetAbsRange()<100000)
		{

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
			if(accel>50)
			{
				multiplier=49;
			}

			if(accel>70)
			{
				multiplier=153;
			}

			if(accel>110)
			{
				multiplier=1163;
			}

			if(GetAbsRange()<500000)
			{
				if(accel>148)
				{
					multiplier=4177;
				}
			}
			else
			{
				if(accel>140)
				{
					multiplier=3177;
				}

				if(accel>178)
				{
					multiplier=11779;
				}
				if(accel>250)
				{
					multiplier=41779;
				}

			}
		}

	}
	else
	{
		if(accel>30 && GetAbsRange()>=200)
		{
			multiplier=5;
		}

		if(accel>50 && GetAbsRange()>=500)
		{
			multiplier=13;
		}

		if(accel>80 && GetAbsRange()>=1000)
		{
			multiplier=23;
		}
	}

	int val=GetValue();

	if(GetRangeMax()<GetRangeMin())
	{
		val=GetValue()-(direction*multiplier);
		if(val>GetRangeMin()) val=GetRangeMin();
		if(val<GetRangeMax()) val=GetRangeMax();
	}
	else
	{
		val=GetValue()+(direction*multiplier);
		if(val<GetRangeMin()) val=GetRangeMin();
		if(val>GetRangeMax()) val=GetRangeMax();
	}

	if(GetValue()!=val)
	{
		SetValue(val);
		OnValueChanging();
		SetModified(true);
		SetNeedsRefresh(true);
	}

}

int FlexMenuItemSliderBase::GetProgressBar(int iPixelWidth)
{
	uint8_t downshift_bits=0;

	if(GetAbsRange()>=10000) downshift_bits=5;
	if(GetAbsRange()>=1000000) downshift_bits=10;


	int fraction=0;

	if(GetRangeMax()>GetRangeMin())
	{
		fraction=((GetValue() - GetRangeMin())<<(10-downshift_bits)) / ((GetRangeMax() - GetRangeMin())>>downshift_bits);
	}
	else
	{
		fraction=(1<<10) - (((GetValue() - GetRangeMax())<<(10-downshift_bits)) / ((-(GetRangeMax() - GetRangeMin()))>>downshift_bits));
	}

	return (fraction*iPixelWidth)>>10;
}

void FlexMenuItemSliderBase::GetSaveString(String & strSave)
{
	strSave=String(GetValue(),10);
}

bool FlexMenuItemSliderBase::LoadString(const String & strLoad)
{
	bool bRet=true;

	int val=strLoad.toInt();

	if(GetRangeMax()<GetRangeMin())
	{
		if(val>GetRangeMin()) { bRet=false; val=GetRangeMin(); }
		if(val<GetRangeMax()) { bRet=false; val=GetRangeMax(); }
	}
	else
	{
		if(val<GetRangeMin()) { bRet=false; val=GetRangeMin(); }
		if(val>GetRangeMax()) { bRet=false; val=GetRangeMax(); }
	}

	SetValue(val);

	OnValueChanged();

	return bRet;

}

void FlexMenuItemSliderBase::HistoryBuffer(uintptr_t * data)
{
	SetHistoryValue((int) *data);
	*data=GetValue();

}

void FlexMenuItemSliderBase::ClearHistoryBuffer(uintptr_t * data, int count)
{
	for(int i=0;i<count;i++)
	{
		data[i]=0x80000000;
	}
}

