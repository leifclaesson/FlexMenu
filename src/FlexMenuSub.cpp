// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuSub.h"


FlexMenuSub::FlexMenuSub()
{
}


FlexMenuSub::~FlexMenuSub()
{
}

void FlexMenuSub::AddSubItem(FlexMenuBase * pSubItem)
{
	pSubItem->SetManager(pManager);
	vecSubItems.push_back(pSubItem);
}

void FlexMenuSub::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}

void FlexMenuSub::GetValueText(String & strValueDestination)
{
	strValueDestination="";
}

int FlexMenuSub::GetNumSubItems()
{
	return (int) vecSubItems.size();
}

FlexMenuBase * FlexMenuSub::GetSubItem(int idx)
{
	if(idx>=0 && idx<(int) vecSubItems.size()) return vecSubItems[idx];
	return NULL;
}

int FlexMenuSub::GetScrollPos()
{
	return iScrollPos;
}

int FlexMenuSub::GetCurItem()
{
	return iCurItem;
}

void FlexMenuSub::SetScrollPos(int iNewScrollPos)
{
	iScrollPos=iNewScrollPos;
}

void FlexMenuSub::SetCurItem(int iNewCurItem)
{
	iCurItem=iNewCurItem;
}

void FlexMenuSub::SetManager(FlexMenuManager * pManager)
{
	this->pManager=pManager;
}

