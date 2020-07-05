#pragma once


class FMM_ShowMessage : public FlexMenuBase
{
public:

	virtual void GetTitleText(String & strTitleDestination) { strTitleDestination=strTitle; }
	virtual void GetValueText(String & strValueDestination) { strValueDestination=strValue; }

	String strTitle;
	String strValue;

};

class FMM_ShowMessageOuter : public FlexMenuBase
{
public:

	virtual eFlexMenuScreenType GetScreenType() override { return eFlexMenuScreenType_Message; }

	virtual int16_t GetNumSubItems() override { return 1; };
	virtual FlexMenuBase * GetSubItem(int16_t idx) override { (void)(idx); return &inner; }

	virtual void GetTitleText(String & strTitleDestination) { strTitleDestination=inner.strTitle; }
	virtual void GetValueText(String & strValueDestination) { strValueDestination=inner.strValue; }

	FMM_ShowMessage inner;

	eFlexMenuFont font;
};


