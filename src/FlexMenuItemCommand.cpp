// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemCommand.h"

static FlexMenuItemCommandDummy dummy;

void FlexMenuItemCommandDummy::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}


int FlexMenuItemCommandBase::GetNumSpacers()
{
	return 1;
}

void FlexMenuItemCommandBase::OnEnter()
{
	derived_use_2=3+GetNumSpacers();
	derived_use_3=0;
}

void FlexMenuItemCommandBase::OnPushChildLeave()
{
	csprintf("FlexMenuItemCommandBase::OnLeave()\n");

	int cur=derived_use_3>0?(derived_use_3-1):derived_use_2;

	if(cur==2+GetNumSpacers())
	{
		OnExecute();
	}
}

void FlexMenuItemCommandBase::OnPush()
{
	OnExecute();
}


eFlexMenuIcon FlexMenuItemCommandBase::UseIcon()
{
	if(GetConfirm())
	{
		return eFlexMenuIcon_RightArrow;
	}
	else
	{
		return eFlexMenuIcon_Execute;
	}
}

int16_t FlexMenuItemCommandBase::GetNumSubItems()
{
	return 4+GetNumSpacers();
}

FlexMenuBase * FlexMenuItemCommandBase::GetSubItem(int16_t idx)
{
	if(idx==0)
	{
		dummy.bIsLeave=false;
		GetTitleText(dummy.strTitle);
	}
	else if(idx==1+(GetNumSpacers()!=0))
	{
		dummy.bIsLeave=false;
		dummy.strTitle="Are you sure?";
	}
	else if(idx==2+GetNumSpacers())
	{
		dummy.bIsLeave=true;
		dummy.strTitle="Yes";
	}
	else if(idx==3+GetNumSpacers())
	{
		dummy.bIsLeave=true;
		dummy.strTitle="No";
	}
	else
	{
		dummy.bIsLeave=false;
		dummy.strTitle="";
	}

	return &dummy;
}

int16_t FlexMenuItemCommandBase::GetScrollPos()
{
	return derived_use_1;
}

int16_t FlexMenuItemCommandBase::GetCurItem()
{
	return derived_use_2;
}

void FlexMenuItemCommandBase::SetScrollPos(int16_t iNewScrollPos)
{
	derived_use_1=iNewScrollPos;
}

void FlexMenuItemCommandBase::SetCurItem(int16_t iNewCurItem)
{
	if(iNewCurItem<2+GetNumSpacers()) iNewCurItem=2+GetNumSpacers();
	derived_use_2=iNewCurItem;
}

bool FlexMenuItemCommandBase::GetConfirm()
{
	return (flags & 0x80) != 0;
}

void FlexMenuItemCommandBase::SetConfirm(bool bConfirm)
{
	if(bConfirm) flags |= 0x80; else flags &= 0x7f;
}

void FlexMenuItemCommandBase::HistoryBuffer(uintptr_t * data)
{
	derived_use_3=((int16_t) *data)+1;
	*data=derived_use_2;

}


