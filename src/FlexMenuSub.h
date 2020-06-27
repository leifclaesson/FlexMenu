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


	virtual int GetNumSubItems() override;
	virtual FlexMenuBase * GetSubItem(int idx) override;

	virtual int GetScrollPos() override;
	virtual int GetCurItem() override;

	virtual void SetScrollPos( int iNewScrollPos ) override;
	virtual void SetCurItem( int iNewCurItem ) override;

	virtual eFlexMenuIcon UseIcon() override { return eFlexMenuIcon_RightArrow; };


private:

	FlexMenuManager * pManager=NULL;

	int iScrollPos=0;
	int iCurItem=0;

	std::vector<FlexMenuBase *> vecSubItems;

};

