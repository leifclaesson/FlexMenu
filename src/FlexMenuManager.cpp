// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include <FlexMenuGlobalItems.h>
#include "stdafx.h"
#include "FlexMenuManager.h"

#include "FlexMenuSub.h"

FlexMenuItemLeave g_leave;

FlexMenuBase * GetLeaveItem()
{
	return &g_leave;
}


FlexMenuTempItem g_tempitem;

FlexMenuBase * GetTempItem()
{
	return &g_tempitem;
}


FlexMenuItemSpacer g_spaceritem;

FlexMenuBase * GetSpacerItem()
{
	return &g_spaceritem;
}


FlexMenuManager::FlexMenuManager()
{
}


FlexMenuManager::~FlexMenuManager()
{
}


FlexMenuSub * FlexMenuManager::Init(FlexMenuDisplay * pDisplay)
{
	this->pDisplay=pDisplay;
	pDisplay->Init();

	pTopMenu=new FlexMenuSub;
	pTopMenu->SetManager(this);
	return (FlexMenuSub *) pTopMenu;
}


void FlexMenuManager::InitialEnterMenu()
{
	if(!pCurMenu)
	{
		pCurMenu=pTopMenu;
		ClearHistoryBuffer();
		pCurMenu->OnEnter();
	}
}

bool FlexMenuManager::Loop(bool bForceRefresh)
{
	InitialEnterMenu();

	HandleHistoryBuffer();

	bool bNeedsRefresh=false;





	switch(stateShowMessage)
	{
	case eShowMessageState_Idle:
		break;
	case eShowMessageState_Displaying:
		if((int) (millis()-timestampShowMessage)>0 && !bPermanentErrorMessage)
		{
			bWeNeedRefresh=true;
			stateShowMessage=eShowMessageState_PostDisplaying_IgnoreInput;	//ignore input for ... (see below)
		}
		break;
	case eShowMessageState_PostDisplaying_IgnoreInput:
		if((int) (millis()-timestampShowMessage)>=500 || bSkipFirstIgnoreInput)	//ignore input for 500 milliseconds to prevent accidental selection if a user clicks to dismiss a message just as it's going away
		{
			bSkipFirstIgnoreInput=false;
			bWeNeedRefresh=true;
			stateShowMessage=eShowMessageState_Idle;
		}
		break;
	}


	if(pDisplay->DisplayNeedsRefresh(pCurMenu)) bNeedsRefresh=true;


	HandleRepeat();

	for(int i=0;i<pDisplay->GetVisibleItems();i++)
	{
		if(i+pCurMenu->GetScrollPos()>=0 && i+pCurMenu->GetScrollPos()<(int) pCurMenu->GetNumSubItems())
		{
			int iCurItem=i+pCurMenu->GetScrollPos();
			FlexMenuBase * pItem=pCurMenu->GetSubItem(iCurItem);
			bNeedsRefresh |= pItem->GetNeedsRefresh();
			pItem->SetNeedsRefresh(false);
			pItem->SetVisible(true);
			vecUpdateStatus.push_back(pItem);
		}
	}

	for(int i=0;i<(int) vecUpdateStatus.size();i++)
	{
		vecUpdateStatus[i]->UpdateStatus();
	}
	vecUpdateStatus.clear();

	if(bWeNeedRefresh || bNeedsRefresh || bForceRefresh || pLastMenu!=pCurMenu || iLastItem!=pCurMenu->GetCurItem() || iLastScrollPos!=pCurMenu->GetScrollPos() || pCurMenu->GetNeedsRefresh())
	{
		bWeNeedRefresh=false;
		pCurMenu->SetNeedsRefresh(false);
		pLastMenu=pCurMenu;
		iLastItem=pCurMenu->GetCurItem();
		iLastScrollPos=pCurMenu->GetScrollPos();

		if(stateShowMessage==eShowMessageState_Displaying)
		{
			pDisplay->DrawScreen(&dummyShowMessage);
		}
		else
		{
			pDisplay->DrawScreen(pCurMenu);
		}

		return true;
	}

	return false;

}

void FlexMenuManager::Output()
{
	pDisplay->Output();
}

void FlexMenuManager::Navigate(eFlexMenuNav nav)
{
	InitialEnterMenu();

	if(bPermanentErrorMessage) return;

	if(nav==eFlexMenuNav_Release)	//handle this first, so we never ignore release and keep repeating!
	{
		countRepeat=0;
		pCurMenu->CanNavigate(eFlexMenuNav_Release,0);
	}

	if(countRepeat)
	{
		if(nav!=eFlexMenuNav_Release)	//if we're repeating, ignore everything but release
		{
			return;
		}
	}


	if(nav!=eFlexMenuNav_None)
	{
		run_history=0;	//wake up the history buffer
	}



	if(stateShowMessage!=eShowMessageState_Idle)
	{

		if(stateShowMessage==eShowMessageState_Displaying)
		{
			//if we push while displaying, cancel the display

			if(nav==eFlexMenuNav_Push)
			{
				stateShowMessage=eShowMessageState_Idle;
				bWeNeedRefresh=true;
			}
		}
		return;

	}


	uint8_t safety=0;
re_navigate:
	safety++;

	if(safety>10) return;
	
	int cur_item=pCurMenu->GetCurItem();
	int scrollpos=pCurMenu->GetScrollPos();
	int visible_items=pDisplay->GetVisibleItems();

	int last_cur_item=cur_item;
	int last_scrollpos=scrollpos;

	{
		int accel=0;

		switch(nav)
		{
		default:
			break;
		case eFlexMenuNav_Prev:
			accel=HandleAcceleration(-1); break;
		case eFlexMenuNav_Next:
			accel=HandleAcceleration(1); break;
		}


		bool bLeave=pDisplay->OnNavigate(pCurMenu, nav, accel);

		if(nav==eFlexMenuNav_Push && bLeave)
		{
			ClearVisible();
			DoLeave();
			nav=eFlexMenuNav_None;
			goto re_navigate;
		}

		switch(nav)
		{
		default:
		case eFlexMenuNav_None:
			break;
		case eFlexMenuNav_Prev:
			if(pCurMenu->CanNavigate(eFlexMenuNav_Prev,HandleAcceleration(-1)))
			{
				cur_item--;
			}
			break;
		case eFlexMenuNav_Next:
			if(pCurMenu->CanNavigate(eFlexMenuNav_Next,HandleAcceleration(1)))
			{
				cur_item++;
			}
			break;
		case eFlexMenuNav_Back:
			if((pCurMenu->CanNavigate(eFlexMenuNav_Back,HandleAcceleration(0))) && pCurMenu->CanLeave())
			{
				ClearVisible();
				DoLeave();
				nav=eFlexMenuNav_None;
				goto re_navigate;
			}
			break;
		case eFlexMenuNav_Release:
			countRepeat=0;
			pCurMenu->CanNavigate(eFlexMenuNav_Release,0);
			break;
		case eFlexMenuNav_Push:
			countRepeat=1;
			timestampRepeat=millis();
			if(pCurMenu->CanNavigate(eFlexMenuNav_Push,HandleAcceleration(0)))
			{
				FlexMenuBase * pCurItem=pCurMenu->GetCurItemPtr_History();

				if(pCurMenu->AllowRewriteHistory() && pCurMenu->GetCurItem_History()!=pCurMenu->GetCurItem())
				{
					cur_item=pCurMenu->GetCurItem_History();	//go back to the intended (historic) item!
				}

				if(pCurMenu->CanLeave() && pCurItem->IsLeave())
				{
					ClearVisible();
					DoLeave();
					nav=eFlexMenuNav_None;
					goto re_navigate;
				}
				else if(pCurItem->CanEnter())
				{
					ClearVisible();
					menustack_count++;
					menustack.push_front(pCurMenu);
					pCurMenu=pCurItem;
					ClearHistoryBuffer();
					pCurMenu->OnEnter();

					if(pCurMenu->GetScreenType()==eFlexMenuScreenType_Edit)
					{
						pDisplay->OnEditMode(pCurMenu, true);
					}

					nav=eFlexMenuNav_None;
					goto re_navigate;

				}
				else
				{
					pCurItem->OnPush();
					pCurMenu->OnPushChild();
				}
			}
			break;
		}
	}

	if(cur_item>pCurMenu->GetNumSubItems()-1) cur_item=pCurMenu->GetNumSubItems()-1;
	if(cur_item<0) cur_item=0;

	if(cur_item-scrollpos >= visible_items-1)
	{
		scrollpos=cur_item-(visible_items-2);
		if(scrollpos+visible_items>(int) pCurMenu->GetNumSubItems()) scrollpos=(int) pCurMenu->GetNumSubItems()-visible_items;

	}
	if(cur_item-scrollpos < 1)
	{
		scrollpos=cur_item-1;
		if(scrollpos<0) scrollpos=0;
	}

	pCurMenu->SetCurItem(cur_item);
	pCurMenu->SetScrollPos(scrollpos);

	if(last_scrollpos!=scrollpos)
	{
		int bumped_item=-1;
		if(last_scrollpos>scrollpos)	//scrolling up
		{
			bumped_item=last_scrollpos+pDisplay->GetVisibleItems()-1;
			
		}
		else if(last_scrollpos<scrollpos)	//scrolling down
		{
			bumped_item=last_scrollpos;
		}

		if(bumped_item>=0)
		{
			FlexMenuBase * pBumpedItem=pCurMenu->GetSubItem(bumped_item);
			pBumpedItem->SetVisible(false);
			vecUpdateStatus.push_back(pBumpedItem);
		}
	}

	if(nav==eFlexMenuNav_Prev || nav==eFlexMenuNav_Next)
	{
		int cur_item=pCurMenu->GetCurItem();
		FlexMenuBase * pCurItem=pCurMenu->GetSubItem(cur_item);
		if(pCurItem && !pCurItem->AllowLand() && cur_item>=0 && cur_item<pCurMenu->GetNumSubItems()-1 && cur_item!=last_cur_item)
		{
			goto re_navigate;
		}
	}


}


void FlexMenuManager::HandleRepeat()
{
	if(countRepeat>0)
	{
		uint32_t thresh=500;

		if(countRepeat>=2) thresh=100;
		if(countRepeat>=6) thresh=70;
		if(countRepeat>=16) thresh=50;

		if(millis()-timestampRepeat>thresh)
		{
			if(countRepeat<255)
			{
				countRepeat++;

				pCurMenu->CanNavigate(eFlexMenuNav_PushRepeat,countRepeat-1);

				pDisplay->OnNavigate(pCurMenu, eFlexMenuNav_PushRepeat,countRepeat-1);

			}

			timestampRepeat=millis();
		}
	}
}


void FlexMenuManager::DoLeave()
{
	if(pCurMenu->CanLeave() && menustack_count>0 )
	{

		pCurMenu->OnLeave();

		if(pCurMenu->GetScreenType()==eFlexMenuScreenType_Edit)
		{
			pDisplay->OnEditMode(pCurMenu, false);
		}

		pCurMenu=*menustack.begin();
		menustack.pop_front();
		menustack_count--;

		ClearHistoryBuffer();

		pCurMenu->OnReturn();


	}
}

void FlexMenuManager::ClearVisible()
{
	for(int i=0;i<pDisplay->GetVisibleItems();i++)
	{
		if(i+pCurMenu->GetScrollPos()>=0 && i+pCurMenu->GetScrollPos()<(int) pCurMenu->GetNumSubItems())
		{
			int iCurItem=i+pCurMenu->GetScrollPos();
			FlexMenuBase * pItem=pCurMenu->GetSubItem(iCurItem);
			pItem->SetVisible(false);
			vecUpdateStatus.push_back(pItem);
		}
	}
}



uint8_t FlexMenuManager::HandleAcceleration(int8_t direction)
{


	if(!direction)
	{
		last_direction=0;
		last_timestamp=millis()-10000;
		return 0;
	}

	uint32_t diff=millis()-last_timestamp;
	int ctr=accel_counter;


	if(direction!=last_direction || diff>300)
	{
		ctr=0;
		accel_counter=0;
	}
	else
	{

		if(diff>90)
		{
			ctr-=3;
		}

		if(diff>60)
		{
			ctr-=3;
		}

		if(diff<40)
		{
			ctr+=1;
		}

		if(diff<20)
		{
			ctr+=1;
		}

	}

	if(ctr<0) ctr=0;
	if(ctr>150) ctr=150;

	accel_counter=ctr;
	
	last_timestamp=millis();
	
	last_direction=direction;
	
	return accel_counter;
}

void FlexMenuManager::ShowPermanentErrorMessage(const String & strTitle, const String & strValue, eFlexMenuFont font)
{
	ShowMessage(strTitle,strValue,font,0);
	bPermanentErrorMessage=true;

}

void FlexMenuManager::CancelMessage()
{
	if(stateShowMessage!=eShowMessageState_Idle)
	{
		bWeNeedRefresh=true;
		stateShowMessage=eShowMessageState_Idle;
	}
}

void FlexMenuManager::ShowMessage(const String & strTitle, const String & strValue, eFlexMenuFont font, uint32_t milliseconds)
{
	csprintf("PE %i\n",bPermanentErrorMessage);
	if(bPermanentErrorMessage) return;

	if(strTitle.length() || strValue.length())
	{

		timestampShowMessage=millis()+milliseconds;
		if(!timestampShowMessage) timestampShowMessage++;

		dummyShowMessage.inner.strTitle=strTitle;
		dummyShowMessage.inner.strValue=strValue;
		dummyShowMessage.font=font;

		stateShowMessage=eShowMessageState_Displaying;
	}
	else
	{
		stateShowMessage=eShowMessageState_PostDisplaying_IgnoreInput;
		timestampShowMessage=millis()+milliseconds;

		dummyShowMessage.inner.strTitle="";
		dummyShowMessage.inner.strValue="";
		dummyShowMessage.font=eFlexMenuFont_Normal;
	}

	bWeNeedRefresh=true;

}

void FlexMenuManager::IterateItems(FlexMenuManagerIterateCB fnCallback, FlexMenuBase * pStart)
{
	if(!pStart) pStart=pTopMenu;
	IterateItemsInternal(fnCallback, pStart);

	IterateItemsInternal(fnCallback, &data_items);

}

void FlexMenuManager::IterateItemsInternal(FlexMenuManagerIterateCB & fnCallback, FlexMenuBase * pStart)
{
	for(int16_t i=0;i<pStart->GetNumSubItems();i++)
	{
		FlexMenuBase * pCur=pStart->GetSubItem(i);
		fnCallback(pCur,this);

		delay(0);
		IterateItemsInternal(fnCallback,pCur);
	}
}

void FlexMenuManager::HandleHistoryBuffer()
{

	if(!pCurMenu) return;

	const int history_interval=30;

//	csprintf("run_history %i\n",run_history);

	if(run_history>(sizeof(vpHistoryBuffer)/sizeof(vpHistoryBuffer[0]))+1)	// +1 just in case
	{
		lastHistoryBufTimestamp=millis()-history_interval;
		return;
	}

	uint8_t i=0;

	while(millis()-lastHistoryBufTimestamp>=history_interval)
	{

		run_history++;

		history_idx++;
		if(history_idx>=sizeof(vpHistoryBuffer)/sizeof(vpHistoryBuffer[0])) history_idx=0;

		if(!pDisplay->HistoryBuffer(pCurMenu,&vpHistoryBuffer[history_idx]))
		{
			pCurMenu->HistoryBuffer(&vpHistoryBuffer[history_idx]);
		}

		lastHistoryBufTimestamp+=history_interval;
		i++;
		if(i>=3)
		{
			lastHistoryBufTimestamp=millis();
			break;
		}
	}

}

void FlexMenuManager::ClearHistoryBuffer()
{
	pCurMenu->ClearHistoryBuffer(vpHistoryBuffer, sizeof(vpHistoryBuffer)/sizeof(vpHistoryBuffer[0]));
}
