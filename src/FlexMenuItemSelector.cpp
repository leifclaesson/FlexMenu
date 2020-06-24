#include "stdafx.h"
#include "FlexMenuItemSelector.h"


FlexMenuItemSelector::FlexMenuItemSelector()
{
}


FlexMenuItemSelector::~FlexMenuItemSelector()
{
}

void FlexMenuItemSelector::OnEnter()
{
	bInMenu=true;
	UpdateStatus();
	iCurItem=iCurSel+1;
}

void FlexMenuItemSelector::OnLeave()
{
	bInMenu=false;
}

int FlexMenuItemSelector::GetNumSubItems()
{
	return vecItems.size()+2;
}

FlexMenuBase * FlexMenuItemSelector::GetSubItem(int idx)
{
	if(idx==0 || idx>(int) vecItems.size()) return &leave;

	idx--;

	dummy.icon=idx==iCurSel?eFlexMenuIcon_Selection:eFlexMenuIcon_Blank;
	dummy.pReturnTitle=&vecItems[idx]->strText;
	//dummy.pReturnValue=&vecItems[idx].strText;
	
	return &dummy;
}

void FlexMenuItemSelector::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strText;

}

void FlexMenuItemSelector::GetValueText(String & strValueDestination)
{
	if(iCurSel>=0 && iCurSel<(int) vecItems.size())
	{
		strValueDestination=vecItems[iCurSel]->strText;
	}
	else
	{
		strValueDestination="INVALID";
	}
}

void FlexMenuItemSelector::OnPushChild()
{
	if(iCurSel!=GetCurItem()-1)
	{
		iCurSel=GetCurItem()-1;
		SetNeedsRefresh(true);
		OnSelChange();
	}
}

int FlexMenuItemSelector::GetScrollPos()
{
	return iScrollPos;
}

int FlexMenuItemSelector::GetCurItem()
{
	return iCurItem;
}

void FlexMenuItemSelector::SetScrollPos(int iNewScrollPos)
{
	iScrollPos=iNewScrollPos;
}

void FlexMenuItemSelector::SetCurItem(int iNewCurItem)
{
	iCurItem=iNewCurItem;
}

bool FlexMenuItemSelector::IsActive()
{
	return bInMenu || GetVisible();
}

void FlexMenuItemSelectorEx::UpdateStatus()
{
	FlexMenuBase::UpdateStatus();
	if(bLastActive!=IsActive())
	{
		bLastActive=IsActive();
		if(cbOnActivityChange) cbOnActivityChange(this);
	}
}

void FlexMenuItemSelectorEx::OnSelChange()
{
	if(cbOnSelChange) cbOnSelChange(this);
}
