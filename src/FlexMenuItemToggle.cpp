// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemToggle.h"


FlexMenuItemToggle::FlexMenuItemToggle()
{
}


FlexMenuItemToggle::~FlexMenuItemToggle()
{
}

void FlexMenuItemToggle::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}

void FlexMenuItemToggle::OnPush()
{
	bState=bState==false;
	if(cbStateChange) cbStateChange(this);
	SetNeedsRefresh(true);
}

eFlexMenuIcon FlexMenuItemToggle::UseIcon()
{
	return bState?eFlexMenuIcon_Checked:eFlexMenuIcon_Unchecked;
}

