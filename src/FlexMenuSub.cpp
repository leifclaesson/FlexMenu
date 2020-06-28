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

int16_t FlexMenuSub::GetNumSubItems()
{
	return (int16_t) vecSubItems.size();
}

FlexMenuBase * FlexMenuSub::GetSubItem(int16_t idx)
{
	if(idx>=0 && idx<(int) vecSubItems.size()) return vecSubItems[idx];
	return NULL;
}

int16_t FlexMenuSub::GetScrollPos()
{
	return iScrollPos;
}

int16_t FlexMenuSub::GetCurItem()
{
	return iCurItem;
}

void FlexMenuSub::SetScrollPos(int16_t iNewScrollPos)
{
	iScrollPos=iNewScrollPos;
}

void FlexMenuSub::SetCurItem(int16_t iNewCurItem)
{
	iCurItem=iNewCurItem;
}

void FlexMenuSub::SetManager(FlexMenuManager * pManager)
{
	this->pManager=pManager;
}

