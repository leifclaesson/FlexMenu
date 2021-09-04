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
	SetCurItem(GetCurSel()+1);
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

	dummy.icon=idx==GetCurSel()?eFlexMenuIcon_Selection:eFlexMenuIcon_Blank;
	dummy.pReturnTitle=SelectorItem_GetText(idx);
	dummy.bIsLeave=GetLeaveOnSelect();
	
	return &dummy;
}

void FlexMenuItemSelectorBase::GetValueText(String & strValueDestination)
{
	if(GetCurSel()>=0 && GetCurSel()<SelectorItem_GetCount())
	{
		strValueDestination=SelectorItem_GetText(GetCurSel());
	}
	else
	{
		strValueDestination="INVALID";
	}
}

void FlexMenuItemSelectorBase::OnPushChild()
{
	if(GetCurSel()!=GetCurItem()-1)
	{
		SetCurSel(GetCurItem_History()-1);

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
		strSave=String(GetCurSel());
		break;
	case eFMISelector_Mode_SaveID:

		if(GetCurSel()>=0 && GetCurSel()<(int) SelectorItem_GetCount())
		{
			strSave=String(SelectorItem_GetID(GetCurSel()));
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
	{
		SetCurSel(-1);
		for(int i=0;i<(int) SelectorItem_GetCount();i++)
		{
			if(!strcmp(SelectorItem_GetText(i),strLoad.c_str()))
			{
				SetCurSel(i);
				bRet=true;
				break;
			}
		}
		break;
	}
	case eFMISelector_Mode_SaveIndex:
		SetCurSel(strLoad.toInt());
		if(GetCurSel()>=0 && GetCurSel()<(int) SelectorItem_GetCount())
		{
			bRet=true;
		}
		break;
	case eFMISelector_Mode_SaveID:
		SetCurSel(-1);

		int load_id=strLoad.toInt();

		for(int i=0;i<(int) SelectorItem_GetCount();i++)
		{
			if(SelectorItem_GetID(i)==load_id)
			{
				SetCurSel(i);
				bRet=true;
				break;
			}
		}
		break;
	}
	OnValueChanged();
	return bRet;
}


int16_t FlexMenuItemSelectorBase::GetCurItem_History()
{
	if(iCurItem_History>=0) return iCurItem_History; else return GetCurItem();
}

void FlexMenuItemSelectorBase::HistoryBuffer(uintptr_t * data)
{
	iCurItem_History=(int16_t) *data;
	*data=GetCurItem();
}



FMISelector_Item * FlexMenuItemSelector::GetCurSelectorItem()
{

	if(GetCurSel()>=0 && GetCurSel()<(int) vecItems.size())
	{
		return vecItems[GetCurSel()];
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

