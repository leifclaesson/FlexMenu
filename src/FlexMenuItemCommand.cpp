// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemCommand.h"

static FlexMenuItemCommandDummy dummy;

void FlexMenuItemCommandDummy::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}


FlexMenuItemCommand::FlexMenuItemCommand()
{
}


FlexMenuItemCommand::~FlexMenuItemCommand()
{
}

int FlexMenuItemCommand::GetNumSpacers()
{
	return 1;
}

void FlexMenuItemCommand::OnEnter()
{
	derived_use_2=3+GetNumSpacers();
	derived_use_3=0;
}

void FlexMenuItemCommand::OnLeave()
{
	csprintf("FlexMenuItemCommand::OnLeave()\n");

	int cur=derived_use_3>0?(derived_use_3-1):derived_use_2;

	if(cur==2+GetNumSpacers() && last_nav==eFlexMenuNav_Push)
	{
		if(cbExecuteCommand) cbExecuteCommand(this);
	}
}

void FlexMenuItemCommand::OnPush()
{
	if(cbExecuteCommand) cbExecuteCommand(this);
}

void FlexMenuItemCommand::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}

eFlexMenuIcon FlexMenuItemCommand::UseIcon()
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

int16_t FlexMenuItemCommand::GetNumSubItems()
{
	return 4+GetNumSpacers();
}

FlexMenuBase * FlexMenuItemCommand::GetSubItem(int16_t idx)
{
	if(idx==0)
	{
		dummy.bIsLeave=false;
		dummy.strTitle=strTitle;
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

int16_t FlexMenuItemCommand::GetScrollPos()
{
	return derived_use_1;
}

int16_t FlexMenuItemCommand::GetCurItem()
{
	return derived_use_2;
}

void FlexMenuItemCommand::SetScrollPos(int16_t iNewScrollPos)
{
	derived_use_1=iNewScrollPos;
}

void FlexMenuItemCommand::SetCurItem(int16_t iNewCurItem)
{
	if(iNewCurItem<2+GetNumSpacers()) iNewCurItem=2+GetNumSpacers();
	derived_use_2=iNewCurItem;
}

bool FlexMenuItemCommand::GetConfirm()
{
	return (flags & 128) != 0;
}

void FlexMenuItemCommand::SetConfirm(bool bConfirm)
{
	if(bConfirm) flags |= 0x80; else flags &= 0x7f;
}

bool FlexMenuItemCommand::CanNavigate(eFlexMenuNav direction, uint8_t accel)
{
	(void)(accel);
	last_nav=direction;
	return true;
}

void FlexMenuItemCommand::HistoryBuffer(uintptr_t * data)
{
	derived_use_3=((int16_t) *data)+1;
	*data=derived_use_2;

}


