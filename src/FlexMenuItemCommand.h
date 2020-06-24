#pragma once
#include "FlexMenuBase.h"

class FlexMenuItemCommand;

typedef std::function<void(FlexMenuItemCommand *)> FlexMenuItemCommandCB;

class FlexMenuItemCommandDummy :
	public FlexMenuBase
{
public:
	String strTitle;
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual bool IsLeave() { return bIsLeave; };
	bool bIsLeave=false;
	virtual bool CanNavigate(eFlexMenuNav direction, uint8_t accel) override;

	eFlexMenuNav last_nav;
};

class FlexMenuItemCommand :
	public FlexMenuBase
{
public:
	FlexMenuItemCommand();
	~FlexMenuItemCommand();

	virtual bool CanEnter() { return bConfirm; };
	virtual void OnEnter();
	virtual void OnLeave();

	virtual void OnPush() override;

	String strTitle;
	virtual void GetTitleText(String & strTitleDestination) override;

	virtual eFlexMenuIcon UseIcon();

	FlexMenuItemCommandCB cbExecuteCommand;

	bool bConfirm=false;

	virtual int GetNumSubItems();
	virtual FlexMenuBase * GetSubItem(int idx);
	
	uint8_t iScrollPos=0;
	uint8_t iCurItem=0;

	virtual int GetScrollPos() override;
	virtual int GetCurItem() override;

	virtual void SetScrollPos( int iNewScrollPos ) override;
	virtual void SetCurItem( int iNewCurItem ) override;

	uint8_t iSpacers;


	FlexMenuItemCommandDummy dummy;


};

