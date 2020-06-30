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
	derived_use_1=true;	//in_menu
	UpdateStatus();
	iCurItem=iCurSel+1;
}

void FlexMenuItemSelector::OnLeave()
{
	derived_use_1=false;	//in_menu
}

int16_t FlexMenuItemSelector::GetNumSubItems()
{
	return vecItems.size()+2;
}

FlexMenuBase * FlexMenuItemSelector::GetSubItem(int16_t idx)
{
	if(idx==0 || idx>(int) vecItems.size()) return GetLeaveItem();

	idx--;

	dummy.icon=idx==iCurSel?eFlexMenuIcon_Selection:eFlexMenuIcon_Blank;
	dummy.pReturnTitle=&vecItems[idx]->strText;
	
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
		iCurSel=GetCurItem_History()-1;

		SetNeedsRefresh(true);
		OnSelChange();
	}
}

int16_t FlexMenuItemSelector::GetScrollPos()
{
	return iScrollPos;
}

void FlexMenuItemSelector::SetScrollPos(int16_t iNewScrollPos)
{
	iScrollPos=iNewScrollPos;
}

void FlexMenuItemSelector::SetCurItem(int16_t iNewCurItem)
{
	iCurItem=iNewCurItem;
}

bool FlexMenuItemSelector::IsActive()
{
	return derived_use_1 /*in_menu*/ || GetVisible();
}



void FlexMenuItemSelector::GetSaveString(String & strSave)
{
	switch(GetMode())
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
	bool bRet=false;
	switch(GetMode())
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
				bRet=true;
				break;
			}
		}
		break;
	case eFMISelector_Mode_SaveIndex:
		iCurSel=strLoad.toInt();
		if(iCurSel>=0 && iCurSel<(int) vecItems.size())
		{
			bRet=true;
		}
		break;
	case eFMISelector_Mode_SaveID:
		iCurSel=-1;

		int load_id=strLoad.toInt();

		for(int i=0;i<(int) vecItems.size();i++)
		{
			if(vecItems[i]->id==load_id)
			{
				iCurSel=i;
				bRet=true;
				break;
			}
		}
		break;
	}
	return bRet;
}


int16_t FlexMenuItemSelector::GetCurItem()
{
	return iCurItem;
}

int16_t FlexMenuItemSelector::GetCurItem_History()
{
	if(iCurItem_History>=0) return iCurItem_History; else return iCurItem;
}

void FlexMenuItemSelector::HistoryBuffer(uintptr_t * data)
{
	iCurItem_History=(int16_t) *data;
	*data=iCurItem;
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

