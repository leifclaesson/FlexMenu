// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_
#include "FlexMenuBase.h"

class FlexMenuItemEdit;

class FlexMenuItemEditInternal : public FlexMenuBase
{
public:
	FlexMenuItemEdit * pParent;

	void Entering();

	virtual void GetTitleText(String & strTitleDestination);

	virtual eFlexMenuScreenType GetScreenType() { return eFlexMenuScreenType_Edit; }
	virtual bool CanNavigate(eFlexMenuNav direction, uint8_t accel) override;

};


class FlexMenuItemEdit :
	public FlexMenuBase
{
public:
	FlexMenuItemEdit();
	virtual ~FlexMenuItemEdit();

	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;

	virtual eFlexMenuIcon UseIcon() override { return eFlexMenuIcon_Cursor; }

	virtual bool CanEnter() override { return true; }
	virtual void OnEnter() override;

	virtual int16_t GetNumSubItems() override { return 1; }
	virtual FlexMenuBase * GetSubItem(int16_t idx) override { (void)(idx); return &edit_internal; }

	String strTitle;
	String strEdit;

	virtual void SetManager(FlexMenuManager * pManager) override;

	FlexMenuItemEditInternal edit_internal;

	FlexMenuManager * pManager=NULL;

};

#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_ */
