// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.
#include "stdafx.h"
#include "FlexMenuBase.h"

FlexMenuBase::FlexMenuBase()
{
}

FlexMenuBase::~FlexMenuBase()
{
}

void FlexMenuBase::OnEnter()
{
	String strTemp;
	GetTitleText(strTemp);
//	csprintf("%s OnEnter\n",strTemp.c_str());
}

void FlexMenuBase::OnReturn()
{
	String strTemp;
	GetTitleText(strTemp);
//	csprintf("%s OnReturn\n",strTemp.c_str());
}

void FlexMenuBase::OnLeave()
{
	String strTemp;
	GetTitleText(strTemp);
//	csprintf("%s OnLeave\n",strTemp.c_str());
}

bool FlexMenuBase::GetNeedsRefresh()
{
	return (flags & 1)!=0;
}

void FlexMenuBase::SetNeedsRefresh(bool bSet)
{
	if(bSet) flags |= 1; else flags &= 0xff-1;
}

bool FlexMenuBase::GetVisible()
{
	return (flags & 2)!=0;
}

void FlexMenuBase::SetVisible(bool bSet)
{
	if(bSet) flags |= 2; else flags &= 0xff-2;
}

bool FlexMenuBase::GetLastVisible()
{
	return (flags & 4)!=0;
}

void FlexMenuBase::SetLastVisible(bool bSet)
{
	if(bSet) flags |= 4; else flags &= 0xff-4;
}

void FlexMenuBase::UpdateStatus()
{
	if(GetVisible()!=GetLastVisible())
	{
		OnVisibilityChange();
		SetLastVisible(GetVisible());
	}
}

void FlexMenuBase::GetIdentifier(String & strIdentifier)
{
	GetTitleText(strIdentifier);
	if(GetSaveIdx())
	{
		strIdentifier+="_";
		strIdentifier+=GetSaveIdx();
	}
}
