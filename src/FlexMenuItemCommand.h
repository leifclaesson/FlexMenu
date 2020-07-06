// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"

class FlexMenuItemCommand;

typedef std::function<void(FlexMenuItemCommand *)> fn_FlexMenuItemCommandCB;

class FlexMenuItemCommandDummy :
	public FlexMenuBase
{
public:
	String strTitle;
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual bool IsLeave() { return bIsLeave; };
	bool bIsLeave=false;
};

class FlexMenuItemCommand :
	public FlexMenuBase
{
public:
	FlexMenuItemCommand();
	~FlexMenuItemCommand();

	virtual bool CanEnter() { return GetConfirm(); };
	virtual void OnEnter();

	virtual void OnPush() override;
	virtual void OnPushChildLeave() override;

	String strTitle;
	virtual void GetTitleText(String & strTitleDestination) override;

	virtual eFlexMenuIcon UseIcon();

	fn_FlexMenuItemCommandCB cbExecuteCommand;

	virtual int16_t GetNumSubItems();
	virtual FlexMenuBase * GetSubItem(int16_t idx);
	
	virtual int16_t GetScrollPos() override;
	virtual int16_t GetCurItem() override;

	virtual void SetScrollPos( int16_t iNewScrollPos ) override;
	virtual void SetCurItem( int16_t iNewCurItem ) override;

	int GetNumSpacers();

	virtual bool GetConfirm();
	virtual void SetConfirm(bool bConfirm);

	virtual void HistoryBuffer(uintptr_t * data);

};

