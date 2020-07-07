// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemSelector.h"


static FMISelector_DummySubItem dummy;


FlexMenuItemSelectorBase::FlexMenuItemSelectorBase()
{
}


FlexMenuItemSelectorBase::~FlexMenuItemSelectorBase()
{
}

void FlexMenuItemSelectorBase::OnEnter()
{
	SetInMenu(true);
	UpdateStatus();
	iCurItem=iCurSel+1;
}

void FlexMenuItemSelectorBase::OnLeave()
{
	SetInMenu(false);
}

int16_t FlexMenuItemSelectorBase::GetNumSubItems()
{
	return SelectorItem_GetCount()+(GetLeaveOnSelect()?1:2);
}

FlexMenuBase * FlexMenuItemSelectorBase::GetSubItem(int16_t idx)
{
	if(idx==0 || idx>(int) SelectorItem_GetCount()) return GetLeaveItem();

	idx--;

	dummy.icon=idx==iCurSel?eFlexMenuIcon_Selection:eFlexMenuIcon_Blank;
	dummy.pReturnTitle=SelectorItem_GetText(idx);
	dummy.bIsLeave=GetLeaveOnSelect();
	
	return &dummy;
}

void FlexMenuItemSelectorBase::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;

}

void FlexMenuItemSelectorBase::GetValueText(String & strValueDestination)
{
	if(iCurSel>=0 && iCurSel<SelectorItem_GetCount())
	{
		strValueDestination=SelectorItem_GetText(iCurSel);
	}
	else
	{
		strValueDestination="INVALID";
	}
}

void FlexMenuItemSelectorBase::OnPushChild()
{
	if(iCurSel!=GetCurItem()-1)
	{
		iCurSel=GetCurItem_History()-1;

		SetNeedsRefresh(true);
		OnValueChanged();
	}
}

void FlexMenuItemSelectorBase::OnPushChildLeave()
{
	if(GetCurItem()>0 && GetLeaveOnSelect())
	{
		csprintf("FlexMenuItemSelectorBase::OnPushChildLeave()\n");
		OnPushChild();
	}
}

int16_t FlexMenuItemSelectorBase::GetScrollPos()
{
	return iScrollPos;
}

void FlexMenuItemSelectorBase::SetScrollPos(int16_t iNewScrollPos)
{
	iScrollPos=iNewScrollPos;
}

void FlexMenuItemSelectorBase::SetCurItem(int16_t iNewCurItem)
{
	iCurItem=iNewCurItem;
}

bool FlexMenuItemSelectorBase::IsActive()
{
	return GetInMenu() /*in_menu*/ || GetVisible();
}



void FlexMenuItemSelectorBase::GetSaveString(String & strSave)
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

		if(iCurSel>=0 && iCurSel<(int) SelectorItem_GetCount())
		{
			strSave=SelectorItem_GetID(iCurSel);
		}
		else
		{
			strSave="INVALID";
		}

		break;
	}

}

bool FlexMenuItemSelectorBase::LoadString(const String & strLoad)
{
	bool bRet=false;
	switch(GetMode())
	{
	case eFMISelector_Mode_NoSave:
		break;
	case eFMISelector_Mode_SaveText:
		iCurSel=-1;
		for(int i=0;i<(int) SelectorItem_GetCount();i++)
		{
			if(!strcmp(SelectorItem_GetText(i),strLoad.c_str()))
			{
				iCurSel=i;
				bRet=true;
				break;
			}
		}
		break;
	case eFMISelector_Mode_SaveIndex:
		iCurSel=strLoad.toInt();
		if(iCurSel>=0 && iCurSel<(int) SelectorItem_GetCount())
		{
			bRet=true;
		}
		break;
	case eFMISelector_Mode_SaveID:
		iCurSel=-1;

		int load_id=strLoad.toInt();

		for(int i=0;i<(int) SelectorItem_GetCount();i++)
		{
			if(SelectorItem_GetID(i)==load_id)
			{
				iCurSel=i;
				bRet=true;
				break;
			}
		}
		break;
	}
	OnValueChanged();
	return bRet;
}


int16_t FlexMenuItemSelectorBase::GetCurItem()
{
	return iCurItem;
}

int16_t FlexMenuItemSelectorBase::GetCurItem_History()
{
	if(iCurItem_History>=0) return iCurItem_History; else return iCurItem;
}

void FlexMenuItemSelectorBase::HistoryBuffer(uintptr_t * data)
{
	iCurItem_History=(int16_t) *data;
	*data=iCurItem;
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





void FlexMenuItemSelectorCB::UpdateStatus()
{
	FlexMenuBase::UpdateStatus();
	if(bLastActive!=IsActive())
	{
		bLastActive=IsActive();
		if(cbOnActivityChange) cbOnActivityChange(this);
	}
}

void FlexMenuItemSelectorCB::OnValueChanged()
{
	if(cbOnValueChanged) cbOnValueChanged(this);
}

