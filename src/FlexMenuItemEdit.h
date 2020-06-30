// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_
#include "FlexMenuBase.h"

class FlexMenuItemEdit;



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
	virtual FlexMenuBase * GetSubItem(int16_t idx) override { (void)(idx); return GetTempItem(); }

	String strTitle;
	String strEdit;

	virtual void SetManager(FlexMenuManager * pManager) override;

	FlexMenuManager * pManager=NULL;

	void SetIsSaveable(bool bSaveable) { if(bSaveable) flags |= 0x80; else flags &=(0xFF-0x80); }
	bool IsSaveable() override { return (flags & 0x80)!=0; }
	virtual void GetSaveString(String & strSave) override { strSave=strEdit; }
	virtual bool LoadString(const String & strLoad) override { strEdit=strLoad; return true; };

	virtual eFlexMenuScreenType GetScreenType() { return eFlexMenuScreenType_Edit; }

	virtual void HistoryBuffer(uintptr_t * data) override;


};

#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUITEMEDIT_H_ */
