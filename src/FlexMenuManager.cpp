#include "stdafx.h"
#include "FlexMenuManager.h"

#include "FlexMenuSub.h"


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
		pCurMenu->OnEnter();
	}
}

void FlexMenuManager::Display(bool bForce)
{
	InitialEnterMenu();

	HandleRepeat();


	bool bNeedsRefresh=false;

	if(pDisplay->EditNeedsRefresh()) bNeedsRefresh=true;


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

	if(bNeedsRefresh || bForce || pLastMenu!=pCurMenu || iLastItem!=pCurMenu->GetCurItem() || iLastScrollPos!=pCurMenu->GetScrollPos() || pCurMenu->GetNeedsRefresh())
	{
		pCurMenu->SetNeedsRefresh(false);
		pLastMenu=pCurMenu;
		iLastItem=pCurMenu->GetCurItem();
		iLastScrollPos=pCurMenu->GetScrollPos();

		pDisplay->DrawDisplay(pCurMenu);
	}



}

void FlexMenuManager::Navigate(eFlexMenuNav nav)
{
	InitialEnterMenu();

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
		FlexMenuBase * pCurItem=pCurMenu->GetSubItem(cur_item);

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
			if(!pCurItem || pCurItem->CanNavigate(eFlexMenuNav_Prev,HandleAcceleration(-1)))
			{
				cur_item--;
			}
			break;
		case eFlexMenuNav_Next:
			if(!pCurItem || pCurItem->CanNavigate(eFlexMenuNav_Next,HandleAcceleration(1)))
			{
				cur_item++;
			}
			break;
		case eFlexMenuNav_Back:
			if((!pCurItem || pCurItem->CanNavigate(eFlexMenuNav_Back,HandleAcceleration(0))) && pCurMenu->CanLeave())
			{
				ClearVisible();
				DoLeave();
				nav=eFlexMenuNav_None;
				goto re_navigate;
			}
			break;
		case eFlexMenuNav_Release:
			countRepeat=0;
			pCurItem->CanNavigate(eFlexMenuNav_Release,0);
			break;
		case eFlexMenuNav_Push:
			countRepeat=1;
			timestampRepeat=millis();
			if(pCurItem && pCurItem->CanNavigate(eFlexMenuNav_Push,HandleAcceleration(0)))
			{
				if(pCurItem->IsLeave())
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
					pCurMenu->OnEnter();

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
//			csprintf("renav ");
			goto re_navigate;
		}
	}


}


void FlexMenuManager::HandleRepeat()
{
	if(countRepeat>0)
	{
		uint32_t thresh=countRepeat>1?100:500;
		if(millis()-timestampRepeat>thresh)
		{
			if(countRepeat<255)
			{
				countRepeat++;

				FlexMenuBase * pCurItem=pCurMenu->GetCurItemPtr();
				if(pCurItem) pCurItem->CanNavigate(eFlexMenuNav_Repeat,countRepeat-1);

				pDisplay->OnNavigate(pCurMenu, eFlexMenuNav_Repeat,countRepeat-1);

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
		pCurMenu=*menustack.begin();
		menustack.pop_front();
		menustack_count--;
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
		//Serial.printf("reset\n");
	}
	else
	{
		//csprintf("diff=%i\n",diff);

		if(diff>80)
		{
			ctr-=3;

			//Serial.printf("sub... %i\n",ctr);
	
		}

		if(diff<50)
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

	
	//csprintf("accel_counter=%i\n",accel_counter);

	last_direction=direction;
	
	return accel_counter;
}
