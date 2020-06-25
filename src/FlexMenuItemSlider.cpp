#include "stdafx.h"
#include "FlexMenuItemSlider.h"


enum eSubItem
{
//	eSubItem_Leave,
	eSubItem_Slider,
	eSubItem_Count,
};


FlexMenuItemSlider::FlexMenuItemSlider()
{
}


FlexMenuItemSlider::~FlexMenuItemSlider()
{
}

/*
void FlexMenuItemSlider::OnEnter()
{
	iCurItem=eSubItem_Slider;
}
*/

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
			if(bModified && cbValueChanged) cbValueChanged(this);
			bModified=false;
			return false;
		}
		break;
	};

	return true;
}


/*
int FlexMenuItemSlider::GetNumSubItems()
{
	return eSubItem_Count;
}

FlexMenuBase * FlexMenuItemSlider::GetSubItem(int idx)
{
	switch(idx)
	{
	case eSubItem_Slider:
		GetTitleText(dummy.strReturnTitle);
		GetValueText(dummy.strReturnValue);
		break;
	default:
		dummy.strReturnTitle="unknown";
		break;
	};

	return &dummy;
}
*/

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
	if(cbDisplayValue)
	{
		cbDisplayValue(this,strValueDestination);
	}
	else
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
		if(cbValueChanging) cbValueChanging(this);
		bModified=true;
		SetNeedsRefresh(true);
	}

/*	FlexMenuItemSlider * pSlider=this;


	uint8_t downshift_bits=0;

	int fraction=0;

	if(pSlider->range_max>pSlider->range_min)
	{
		fraction=((pSlider->value - pSlider->range_min)<<(10-downshift_bits)) / ((pSlider->range_max - pSlider->range_min)>>downshift_bits);

		csprintf("fraction (pos)=%i\n",fraction);
	}
	else
	{
		fraction=(1<<10) - (((pSlider->value - pSlider->range_max)<<(10-downshift_bits)) / ((-(pSlider->range_max - pSlider->range_min))>>downshift_bits));

		csprintf("fraction (neg)=%i\n",fraction);
	}

	csprintf("pixels: %i\n",(fraction*173)>>10);
	*/

	//csprintf("range=%i\n",range);


//	csprintf("adjust %i, accel %i\n",direction,accel);
}

/*
int FlexMenuItemSlider::GetScrollPos()
{
	return iScrollPos;
}

int FlexMenuItemSlider::GetCurItem()
{
	return iCurItem;
}

void FlexMenuItemSlider::SetScrollPos(int iNewScrollPos)
{
	iScrollPos=(uint8_t) iNewScrollPos;
}

void FlexMenuItemSlider::SetCurItem(int iNewCurItem)
{
	iCurItem=(uint8_t) iNewCurItem;
}
*/

eFlexMenuIcon FlexMenuItemSlider::UseIcon()
{
	if(bAdjusting)
	{
		return eFlexMenuIcon_Circle;
	}

	return eFlexMenuIcon_Blank;
}

