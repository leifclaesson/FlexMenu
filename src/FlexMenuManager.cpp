// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include <Arduino.h>
#include <FlexMenuGlobalItems.h>
#include "stdafx.h"
#include "FlexMenuManager.h"
#include "FlexMenuSub.h"

FlexMenuItemLeave g_leave;

FlexMenuBase * GetLeaveItem()
{
	return &g_leave;
}




FlexMenuTempItem * pUseTempItem=nullptr;

FlexMenuBase * GetTempItem()
{
	return pUseTempItem;
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

void FlexMenuManager::Init(FlexMenuDisplay * pDisplay, FlexMenuBase * pTopMenu)
{
	this->pDisplay=pDisplay;
	pDisplay->Init();

	this->pTopMenu=pTopMenu;
	this->pTopMenu->SetManager(this);
}


void FlexMenuManager::InitialEnterMenu()
{
	pUseTempItem=&tempitem;

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

	if(!bNavigated && millis()-timestampLoop<10) return false;
	timestampLoop=millis();
	bNavigated=false;

	HandleHistoryBuffer();

	DoOnLoopCallback();

	bool bItemNeedsRefresh=false;





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
		if((int) (millis()-timestampShowMessage)>=250 || bSkipFirstIgnoreInput)	//ignore input for 250 milliseconds to prevent accidental selection if a user clicks to dismiss a message just as it's going away
		{
			bSkipFirstIgnoreInput=false;
			bWeNeedRefresh=true;
			stateShowMessage=eShowMessageState_Idle;
		}
		break;
	}



	HandleRepeat();

	for(int i=0;i<pDisplay->GetVisibleItems();i++)
	{
		if(i+pCurMenu->GetScrollPos()>=0 && i+pCurMenu->GetScrollPos()<(int) pCurMenu->GetNumSubItems())
		{
			int iCurItem=i+pCurMenu->GetScrollPos();
			FlexMenuBase * pItem=pCurMenu->GetSubItem(iCurItem);
			if(!bLastBlankDisplay)
			{
				if(pItem->GetNeedsRefresh())
				{
					bItemNeedsRefresh=true;
					//String strTitle;
					//pItem->GetTitleText(strTitle);
					//csprintf("%s needs refresh\n",strTitle.c_str());
				}
				pItem->SetNeedsRefresh(false);
			}
			pItem->SetVisible(true);
			vecUpdateStatus.push_back(pItem);
		}
	}

	for(int i=0;i<(int) vecUpdateStatus.size();i++)
	{
		vecUpdateStatus[i]->UpdateStatus();
	}
	vecUpdateStatus.clear();

	if(!bExternalBacklightControl)
	{
		bWeNeedRefresh |= HandleBacklight();
	}



	if(bWeNeedRefresh || pDisplay->DisplayNeedsRefresh(pCurMenu) || bItemNeedsRefresh || bForceRefresh || pLastMenu!=pCurMenu || iLastItem!=pCurMenu->GetCurItem() || iLastScrollPos!=pCurMenu->GetScrollPos() || pCurMenu->GetNeedsRefresh())
	{

/*
		if(bWeNeedRefresh) csprintf("W");
		if(bItemNeedsRefresh) csprintf("I");
		if(bForceRefresh ) csprintf("F");
		if( pLastMenu!=pCurMenu) csprintf("M");
		if(iLastItem!=pCurMenu->GetCurItem()) csprintf("D");
		if( iLastScrollPos!=pCurMenu->GetScrollPos()) csprintf("S");
		if(pCurMenu->GetNeedsRefresh()) csprintf("R");
		if(pDisplay->DisplayNeedsRefresh(pCurMenu)) csprintf("D");
		csprintf(".");
*/


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

void FlexMenuManager::ForceRefresh()
{
	lastNavigateTimestamp=millis();
	if(pCurMenu)
	{
		pCurMenu->SetNeedsRefresh(true);
	}
	last_filtered=0xFFFF;
}

void FlexMenuManager::ResetDisplayTimeout()
{
	lastNavigateTimestamp=millis();
}

uint32_t FlexMenuManager::GetLastNavigateAge()
{
	return millis()-lastNavigateTimestamp;
}

void FlexMenuManager::Navigate(eFlexMenuNav nav)
{
	InitialEnterMenu();

	if(bPermanentErrorMessage) return;

	bNavigated=true;

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
		lastNavigateTimestamp=millis();
	}


	//Check for top / bottom bumping here, i.e. are we being asked to navigate past the end?
	switch(nav)
	{
	case eFlexMenuNav_Prev:
		if(pCurMenu && pCurMenu->GetScreenType()==eFlexMenuScreenType_Normal && pCurMenu->GetNumSubItems()>1 && pCurMenu->GetCurItem()==0 && pCurMenu->GetCurItemPtr()->CanNavigate(eFlexMenuNav_Prev, 0))
		{
			iTopBumpCount++;
			timestampBump=millis();
		}
		else
		{
			iTopBumpCount=0;
		}
		iBottomBumpCount=0;
		break;
	case eFlexMenuNav_Next:
		if(pCurMenu && pCurMenu->GetScreenType()==eFlexMenuScreenType_Normal && pCurMenu->GetNumSubItems()>1 && pCurMenu->GetCurItem()==pCurMenu->GetNumSubItems()-1 && pCurMenu->GetCurItemPtr()->CanNavigate(eFlexMenuNav_Next, 0))
		{
			iBottomBumpCount++;
			timestampBump=millis();
		}
		else
		{
			iBottomBumpCount=0;
		}
		iTopBumpCount=0;
		break;
	default:
		iBottomBumpCount=0;
		iTopBumpCount=0;
		break;
	}

	if(iBottomBumpCount || iTopBumpCount)
	{
		if((millis()-timestampBump) > 500)
		{
			iBottomBumpCount=0;
			iTopBumpCount=0;
		}
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


		if(stateShowMessage==eShowMessageState_Displaying || nav==eFlexMenuNav_Push)	//ignore push if during the eShowMessageState_PostDisplaying_IgnoreInput period
		{
			return;
		}

	}


	uint8_t safety=0;
re_navigate:
	safety++;

	if((safety & 15) == 15)	//if iLoopSafety is above 15 then make sure we give the OS some time to run while we're looping
	{
		delay(0);	//yield
	}

	if(safety>iLoopSafety) return;
	
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
			if(pCurMenu->CanLeave())
			{
				DoLeave();
			}
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
		case eFlexMenuNav_First:
			if(pCurMenu->CanNavigate(eFlexMenuNav_Prev,HandleAcceleration(1)) && pCurMenu->CanNavigate(eFlexMenuNav_First,HandleAcceleration(1)))
			{
				cur_item=0;
			}
			break;
		case eFlexMenuNav_Last:
			if(pCurMenu->CanNavigate(eFlexMenuNav_Next,HandleAcceleration(1)) && pCurMenu->CanNavigate(eFlexMenuNav_Last,HandleAcceleration(1)))
			{
				cur_item=pCurMenu->GetNumSubItems()-1;
				if(cur_item<0) cur_item=0;
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
					csprintf("rewrite history! cur_item was %i, will be %i\n",cur_item,pCurMenu->GetCurItem_History());
					cur_item=pCurMenu->GetCurItem_History();	//go back to the intended (historic) item!
					pCurMenu->SetCurItem(cur_item);	//apply now in case we're leaving
				}

				if(pCurMenu->CanLeave() && pCurItem->IsLeave())
				{
					pCurItem->OnPushLeave();
					pCurMenu->OnPushChildLeave();
					ClearVisible();
					DoLeave();
					nav=eFlexMenuNav_None;
					goto re_navigate;
				}
				else if(pCurItem->CanEnter())
				{
					ClearVisible();
					pCurMenu->OnEnterChild();
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

	FlexMenuBase * pNewFocusItem=pCurMenu->GetCurItemPtr();
	if(pNewFocusItem!=pCurFocusItem)
	{
		if(pCurFocusItem) pCurFocusItem->OnFocusLoss();
		pCurFocusItem=pNewFocusItem;
		if(pCurFocusItem) pCurFocusItem->OnFocusGain();
	}

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

	if(nav==eFlexMenuNav_Prev || nav==eFlexMenuNav_Next || nav==eFlexMenuNav_First || nav==eFlexMenuNav_Last)
	{
		int cur_item=pCurMenu->GetCurItem();
		FlexMenuBase * pCurItem=pCurMenu->GetSubItem(cur_item);
		if(pCurItem && !pCurItem->AllowLand() && cur_item>=0 && cur_item<pCurMenu->GetNumSubItems()-1 && cur_item!=last_cur_item)
		{
			if(nav==eFlexMenuNav_First)
			{
				nav=eFlexMenuNav_Next;	//just in case we can't land on the first item
			}
			else if(nav==eFlexMenuNav_Last)
			{
				nav=eFlexMenuNav_Prev;	//just in case we can't land on the last item
			}

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
	if(menustack_count>0 )
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


	if(direction!=last_direction || diff>400)
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
	if(ctr>255) ctr=255;

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
	//csprintf("PE %i\n",bPermanentErrorMessage);
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

bool FlexMenuManager::HandleBacklight()
{

	bool bRet=false;

	//if(stateShowMessage==eShowMessageState_Displaying) return bRet;

	bool bBlankDisplay=false;

	if(millis()-lastNavigateTimestamp>0x40000000)
	{
		lastNavigateTimestamp=millis()-0x40000000;	//drag the timestamp along so the difference never wraps around
	}

	int32_t age=(int) (millis()-lastNavigateTimestamp);


	bBlankDisplay=age>(1000*iDisplayMuteSeconds);
	if(age>(1000*iDisplayMuteSeconds))
	{
		age=(1000*iDisplayMuteSeconds);
	}


	int subtract=(age - (1000*iBacklightDimSeconds)) / iBacklightDimDivisor;

	if(subtract<0) subtract=0;
	if(subtract>254) subtract=254;


	if(stateShowMessage==eShowMessageState_Displaying) subtract=0;


	uint16_t brightness[1]={(uint8_t) (255-subtract)};
	uint16_t filtered[1];

	brightness[0] = (brightness[0] * uBacklightScale) >> 8;

	filterBacklight.run(brightness, filtered);



	bool bDoCallback=false;

	if(last_filtered!=filtered[0])
	{
		last_filtered=filtered[0];
		bDoCallback=true;

	}

	if(bLastBlankDisplay!=bBlankDisplay)
	{
		bLastBlankDisplay=bBlankDisplay;
		bRet=true;
		bDoCallback=true;

		if(!bBlankDisplay)
		{
			if(fnWakingDisplay)
			{
				fnWakingDisplay();
			}
		}

	}

	if(bDoCallback)
	{
		pDisplay->SetBacklight(last_filtered, bLastBlankDisplay);
	}

	return bRet;

}

void FlexMenuManager::DoOnSettingsLoadedCallback()
{
	if(bOnSettingsLoadedDone) return;
	bOnSettingsLoadedDone=true;
	for(size_t i=0;i<vecFnOnSettingsLoaded.size();i++)
	{
		if(vecFnOnSettingsLoaded[i]) vecFnOnSettingsLoaded[i]();
	}
}

void FlexMenuManager::RegisterOnSettingsLoadedCallback(std::function<void(void)> fn)
{
	vecFnOnSettingsLoaded.push_back(fn);
}

void FlexMenuManager::DoOnLoopCallback()
{
	for(size_t i=0;i<vecFnOnLoop.size();i++)
	{
		if(vecFnOnLoop[i]) vecFnOnLoop[i]();
	}
}

void FlexMenuManager::RegisterOnLoopCallback(std::function<void(void)> fn)
{
	vecFnOnLoop.push_back(fn);
}

void FlexMenuManager::SetWakingDisplayCallback(std::function<void(void)> fn)
{
	fnWakingDisplay=fn;
}

FlexMenuBase * FlexMenuManager::GetCurMenu()
{
	return pCurMenu;
}

void FlexMenuManager::SetLoopSafety(int count)
{
	iLoopSafety=count;
}

int FlexMenuManager::GetBottomBumpCount()
{
	if(iBottomBumpCount && (millis()-timestampBump)>500)
	{
		iBottomBumpCount=0;
	}
	return iBottomBumpCount;
}

int FlexMenuManager::GetTopBumpCount()
{
	if(iTopBumpCount && (millis()-timestampBump)>500)
	{
		iTopBumpCount=0;
	}
	return iTopBumpCount;
}
