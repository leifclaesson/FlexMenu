// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemCommand.h"

static FlexMenuItemCommandDummy dummy;

void FlexMenuItemCommandDummy::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}

bool FlexMenuItemCommandDummy::CanNavigate(eFlexMenuNav direction, uint8_t accel)
{
	(void)(accel);
	last_nav=direction;
	return true;
}

FlexMenuItemCommand::FlexMenuItemCommand()
{
	iSpacers=1;
}


FlexMenuItemCommand::~FlexMenuItemCommand()
{
}

void FlexMenuItemCommand::OnEnter()
{
	iCurItem=3+iSpacers;
}

void FlexMenuItemCommand::OnLeave()
{
	if(iCurItem==2+iSpacers && dummy.last_nav==eFlexMenuNav_Push)
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
	if(bConfirm)
	{
		return eFlexMenuIcon_RightArrow;
	}
	else
	{
		return eFlexMenuIcon_Execute;
	}
}

int FlexMenuItemCommand::GetNumSubItems()
{
	return 4+iSpacers;
}

FlexMenuBase * FlexMenuItemCommand::GetSubItem(int idx)
{
	if(idx==0)
	{
		dummy.bIsLeave=false;
		dummy.strTitle=strTitle;
	}
	else if(idx==1+(iSpacers!=0))
	{
		dummy.bIsLeave=false;
		dummy.strTitle="Are you sure?";
	}
	else if(idx==2+iSpacers)
	{
		dummy.bIsLeave=true;
		dummy.strTitle="Yes";
	}
	else if(idx==3+iSpacers)
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

int FlexMenuItemCommand::GetScrollPos()
{
	return iScrollPos;
}

int FlexMenuItemCommand::GetCurItem()
{
	return iCurItem;
}

void FlexMenuItemCommand::SetScrollPos(int iNewScrollPos)
{
	iScrollPos=iNewScrollPos;
}

void FlexMenuItemCommand::SetCurItem(int iNewCurItem)
{
	if(iNewCurItem<2+iSpacers) iNewCurItem=2+iSpacers;
	iCurItem=iNewCurItem;
}



