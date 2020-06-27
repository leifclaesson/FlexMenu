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

void FlexMenuItemLabelEx::OnVisibilityChange()
{
	if(cbOnVisiblilityChange) cbOnVisiblilityChange(this);
}