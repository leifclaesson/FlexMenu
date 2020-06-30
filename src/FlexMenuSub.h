// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"

class FlexMenuSub :
	public FlexMenuBase
{
public:
	FlexMenuSub();
	~FlexMenuSub();

	String strTitle;

	virtual void SetManager(FlexMenuManager * pManager) override;

	virtual void AddSubItem(FlexMenuBase * pSubItem);

	virtual bool CanEnter() override { return true; };

	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;


	virtual int16_t GetNumSubItems() override;
	virtual FlexMenuBase * GetSubItem(int16_t idx) override;

	virtual int16_t GetScrollPos() override;
	virtual int16_t GetCurItem() override;

	virtual void SetScrollPos( int16_t iNewScrollPos ) override;
	virtual void SetCurItem( int16_t iNewCurItem ) override;

	virtual eFlexMenuIcon UseIcon() override { return eFlexMenuIcon_RightArrow; };

	virtual int16_t GetCurItem_History();
	virtual void HistoryBuffer(uintptr_t * data);


private:

	FlexMenuManager * pManager=NULL;

	int16_t iScrollPos=0;
	int16_t iCurItem=0;

	int16_t iCurItem_History=-1;

	std::vector<FlexMenuBase *> vecSubItems;

};

