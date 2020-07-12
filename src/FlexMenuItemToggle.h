// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"


class FlexMenuItemToggle :
	public FlexMenuBase
{
public:
	FlexMenuItemToggle();
	~FlexMenuItemToggle();

	virtual bool GetState();
	virtual void SetState(bool bState);

	virtual void GetTitleText(String & strTitleDestination);
	virtual void OnPush();

	String strTitle;

	virtual eFlexMenuIcon UseIcon();

	virtual void OnValueChanged() {};

	void SetSaveable(bool bSaveable) { if(bSaveable) flags |= 0x80; else flags &=(0xFF-0x80); }
	bool IsSaveable() override { return (flags & 0x80)!=0; }

	virtual void GetSaveString(String & strSave) override;
	virtual bool LoadString(const String & strLoad) override;

	virtual void SetSaveIdx( uint16_t idx ) override { derived_use_3 = (uint8_t) idx; }
	virtual uint16_t GetSaveIdx( ) override { return derived_use_3; }

private:

};


class FlexMenuItemToggleCB;

enum eFMIToggleCallback
{
	eFMIToggleCallback_ValueChanged,
};

typedef std::function<void(FlexMenuItemToggleCB *,eFMIToggleCallback)> fn_FlexMenuItemToggleCB;

/*
 * LAMBDA example
 *
 * 		p->fnCallback=[this](FlexMenuItemToggleCB * pSource,eFMIToggleCallback code)
		{
			(void)(pSource);
			if(code==eFMIToggleCallback_ValueChanged)
			{
				Serial.printf("Toggle switch %s changed to %i\n",pSource->strTitle.c_str(),pSource->GetState());

			}
		};
 *
 *
 */

class FlexMenuItemToggleCB : public FlexMenuItemToggle
{
public:

	virtual void OnValueChanged() override { if(fnCallback) fnCallback(this,eFMIToggleCallback_ValueChanged); }

	fn_FlexMenuItemToggleCB fnCallback;

};


class FlexMenuItemToggleModPtr : public FlexMenuItemToggle
{
public:

	bool * pModified=0;

	virtual void OnValueChanged() override { if(pModified) *pModified=true; }

};



