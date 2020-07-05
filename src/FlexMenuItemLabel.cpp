// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemLabel.h"


FlexMenuItemLabel::FlexMenuItemLabel()
{
}


FlexMenuItemLabel::~FlexMenuItemLabel()
{
}

void FlexMenuItemLabel::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
	
}

void FlexMenuItemLabel::GetValueText(String & strValueDestination)
{
	strValueDestination=strValue;
}

void FlexMenuItemLabel::UpdateTitle(String & strTitle)
{
	if(this->strTitle!=strTitle)
	{
		this->strTitle=strTitle;
		SetNeedsRefresh(true);
	}
}
void FlexMenuItemLabel::UpdateValue(String & strValue)
{
	if(this->strValue!=strValue)
	{
		this->strValue=strValue;
		SetNeedsRefresh(true);
	}
}

void FlexMenuItemLabelEx::OnVisibilityChange()
{
	if(cbOnVisiblilityChange) cbOnVisiblilityChange(this);
}
