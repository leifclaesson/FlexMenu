// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemSelector.h"


static FMISelector_DummySubItem dummy;


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
	if(idx==0 || idx>(int) vecItems.size()) return GetLeaveItem();

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

FMISelector_Item * FlexMenuItemSelector::GetCurSelectorItem()
{
	if(iCurSel>=0 && iCurSel<(int) vecItems.size())
	{
		return vecItems[iCurSel];
	}
	else
	{
		return 0;
	}
}


void FlexMenuItemSelector::GetValueText(String & strValueDestination)
{
	FMISelector_Item * pItem=GetCurSelectorItem();

	if(pItem)
	{
		strValueDestination=pItem->strText;
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



void FlexMenuItemSelector::GetSaveString(String & strSave)
{
	switch(mode)
	{
	case eFMISelector_Mode_NoSave:
		strSave.clear();
		break;
	case eFMISelector_Mode_SaveText:
		GetValueText(strSave);
		break;
	case eFMISelector_Mode_SaveIndex:
		strSave=iCurSel;
		break;
	case eFMISelector_Mode_SaveID:
		FMISelector_Item * pItem=GetCurSelectorItem();

		if(pItem)
		{
			strSave=pItem->id;
		}
		else
		{
			strSave="INVALID";
		}
		break;
	}


}

bool FlexMenuItemSelector::LoadString(const String & strLoad)
{
	switch(mode)
	{
	case eFMISelector_Mode_NoSave:
		break;
	case eFMISelector_Mode_SaveText:
		iCurSel=-1;
		for(int i=0;i<(int) vecItems.size();i++)
		{
			if(vecItems[i]->strText==strLoad)
			{
				iCurSel=i;
				break;
			}
		}
		break;
	case eFMISelector_Mode_SaveIndex:
		iCurSel=atoi(strLoad.c_str());
		break;
	case eFMISelector_Mode_SaveID:
		iCurSel=-1;

		int load_id=atoi(strLoad.c_str());

		for(int i=0;i<(int) vecItems.size();i++)
		{
			if(vecItems[i]->id==load_id)
			{
				iCurSel=i;
				break;
			}
		}
		break;
	}
	return false;
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

