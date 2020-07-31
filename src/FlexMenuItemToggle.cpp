// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemToggle.h"


FlexMenuItemToggleBase::FlexMenuItemToggleBase()
{
	SetSaveable(true);
}


void FlexMenuItemToggleBase::OnPush()
{
	SetState(GetState()==false);
	OnValueChanged();
	SetNeedsRefresh(true);
}

eFlexMenuIcon FlexMenuItemToggleBase::UseIcon()
{
	return GetState()?eFlexMenuIcon_Checked:eFlexMenuIcon_Unchecked;
}

void FlexMenuItemToggleBase::GetSaveString(String & strSave)
{
	strSave=GetState();
}

bool FlexMenuItemToggleBase::LoadString(const String & strLoad)
{
	if(strLoad=="1")
	{
		SetState(true);
		OnValueChanged();
		return true;
	}
	if(strLoad=="0")
	{
		SetState(false);
		OnValueChanged();
		return true;
	}
	return false;
}

