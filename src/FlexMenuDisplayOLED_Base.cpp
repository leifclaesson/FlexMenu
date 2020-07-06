// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include <FlexMenuDisplayOLED_Base.h>
#include <SH1106.h>
#include "FlexMenuBase.h"
#include "FlexMenuItemSlider.h"
#include "FlexMenuShowMessage.h"


FlexMenuDisplay_OLED_Base::FlexMenuDisplay_OLED_Base()
{
}

FlexMenuDisplay_OLED_Base::~FlexMenuDisplay_OLED_Base()
{
}

void FlexMenuDisplay_OLED_Base::SetParams(FlexMenuDisplay_OLED_Base_Params * pParams)
{
	params=*pParams;
}

void FlexMenuDisplay_OLED_Base::Init()
{
	OLEDDisplay & display=*params.pOLEDDisplay;
	(void)(display);


	memset(icons,0,sizeof(icons));

	//the resources are in a separate class (FlexMenuDisplayOLED) which subclasses this class

	InitResources_MenuIcons();
	InitResources_MenuFonts();

	FlexMenuEditScreenParams ep;

	ep.iScreenCX=params.iScreenCX;
	ep.iScreenCY=params.iScreenCY;

	InitResources_Edit(ep);

	EditScreen_Init(&ep);

	iVisibleItems=params.iScreenCY/iLineHeight;


}



int FlexMenuDisplay_OLED_Base::GetVisibleItems()
{
	return iVisibleItems;
}

void FlexMenuDisplay_OLED_Base::DrawScreen(FlexMenuBase * pCurMenu)
{
	OLEDDisplay & display=*params.pOLEDDisplay;

	display.clear();
	display.setFont(pFont);

	int iWidthDots=display.getStringWidth("..");


	{
		switch(pCurMenu->GetScreenType())
		{
		default:
		case eFlexMenuScreenType_Normal:
			break;
		case eFlexMenuScreenType_Slider:
			DrawSliderScreen(pCurMenu);
			return;
		case eFlexMenuScreenType_Edit:
			EditScreen_Draw(pCurMenu);
			return;
		case eFlexMenuScreenType_Message:
			DrawMessage(pCurMenu);
			return;
		}

	}


	float fLineHeight=(float) (params.iScreenCY) / GetVisibleItems();

	for(int i=0;i<GetVisibleItems();i++)
	{
		if(interimCallback) interimCallback();
		yield();

		if(i+pCurMenu->GetScrollPos()>=0 && i+pCurMenu->GetScrollPos()<(int) pCurMenu->GetNumSubItems())
		{
			int iCurItem=i+pCurMenu->GetScrollPos();
			FlexMenuBase * pItem=pCurMenu->GetSubItem(iCurItem);

			String strLeft;
			String strRight;
			pItem->GetTitleText(strLeft);
			pItem->GetValueText(strRight);
			eFlexMenuIcon icon=pItem->UseIcon();

			if(interimCallback) interimCallback();

			int left=0;
			int right=params.iScreenCX;


			if(icon<eFlexMenuIcon_Count && icons[icon])
			{
				if(icon==eFlexMenuIcon_LeftArrow)
				{
					left+=iIconCX;
					if(icons[icon])
					{
						display.drawXbm(0, i*fLineHeight+iIconY, iIconCX, iIconCY, icons[icon]);
					}
				}
				else
				{
					if(icon!=eFlexMenuIcon_None) right-=iIconCX;
					//right-=iIconCX;
					if(icons[icon])
					{
						display.drawXbm(right, i*fLineHeight+iIconY, iIconCX, iIconCY, icons[icon]);
					}
				}
			}

			if(interimCallback) interimCallback();

			display.setColor(WHITE);

			if(pItem->DisplayCentered() && (!strLeft.length() || !strRight.length()))
			{
				display.setTextAlignment(TEXT_ALIGN_CENTER);
				display.drawString(params.iScreenCX/2, i*fLineHeight, strLeft.length()?strLeft:strRight );

				if(interimCallback) interimCallback();

			}
			else
			{

				display.setTextAlignment(TEXT_ALIGN_LEFT);
				display.drawString(left, i*fLineHeight, strLeft );
				int widthLeft=display.getStringWidth(strLeft);

				display.setTextAlignment(TEXT_ALIGN_RIGHT);

				if(interimCallback) interimCallback();

				int icon_width=iIconCX;
				if(icon==eFlexMenuIcon_None) icon_width=0;

				int chars=getCharsForWidth(pFont,strRight.c_str(),strRight.length(),params.iScreenCX-(widthLeft+icon_width-1));

				//csprintf("chars=%i scx=%i widthLeft=%i widthDots=%i iconCX=%i\n",chars,params.iScreenCX,widthLeft,widthDots,iIconCX);

				if(interimCallback) interimCallback();

				if(chars!=(int) strRight.length())
				{
					int chars=getCharsForWidth(pFont,strRight.c_str(),strRight.length(),params.iScreenCX-(widthLeft+icon_width+iWidthDots));
					String temp=strRight.substring(0,chars);
					temp+="..";
					display.drawString(right, i*fLineHeight, temp);
				}
				else
				{
					display.drawString(right, i*fLineHeight, strRight);
				}

				if(interimCallback) interimCallback();

			}


		}

	}


	if(interimCallback) interimCallback();

	{

		yield();

		int iCurItem=pCurMenu->GetCurItem()-pCurMenu->GetScrollPos();

		if(iCurItem>=0 && iCurItem<GetVisibleItems())
		{
			display.setColor(INVERSE);

			display.fillRect( 0, iCurItem*fLineHeight, params.iScreenCX, iLineHeight+1);
		}
	}


}

const uint8_t * FlexMenuDisplay_OLED_Base::GetFont(eFlexMenuFont font)
{

	switch(font)
	{
	default:
		return pFont;
	case eFlexMenuFont_Large:
		return pFontSlider;
	}
}

int FlexMenuDisplay_OLED_Base::GetStringWidth(const String & strInput, eFlexMenuFont font)
{
	OLEDDisplay & display=*params.pOLEDDisplay;
	display.setFont(GetFont(font));
	return display.getStringWidth(strInput);
}

int FlexMenuDisplay_OLED_Base::GetCharsForWidth(const String & strInput, int width_pixels, eFlexMenuFont font)
{
	return getCharsForWidth(GetFont(font),strInput.c_str(),strInput.length(),width_pixels);
}

int FlexMenuDisplay_OLED_Base::GetWidth()
{
	return params.iScreenCX;
}


void FlexMenuDisplay_OLED_Base::DrawSliderScreen(FlexMenuBase * pCurMenu)
{
	(void)(pCurMenu);
	OLEDDisplay & display=*params.pOLEDDisplay;

	FlexMenuItemSlider * pSlider=(FlexMenuItemSlider *) pCurMenu;

	const uint8_t * pUseFont=pFont;
	int iFontHeight=iLineHeight;
	if(pFontSlider)
	{
		pUseFont=pFontSlider;
#if defined(ARDUINO_ARCH_ESP32)
		iFontHeight=pUseFont[1] - 1;
#else

		const void * ptr=&pUseFont[1];	//second byte of font is height
		uint8_t test=0;
		pgm_read_with_offset(ptr,test)	//it's in flash so we need to call the helper function to read it
		iFontHeight=test-1;
#endif
	}

	display.setFont(pUseFont);

	yield();
	if(interimCallback) interimCallback();

	String strTitle;
	pCurMenu->GetTitleText(strTitle);

	String strValue;
	pCurMenu->GetValueText(strValue);

	display.setColor(WHITE);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.drawString(params.iScreenCX/2, 0, strTitle);

	yield();
	if(interimCallback) interimCallback();

	display.drawString(params.iScreenCX/2, params.iScreenCY-iFontHeight, strValue);

	int bar_height=(params.iScreenCY * 30) / 100;

	int bar_y=(params.iScreenCY-bar_height)/2;


	yield();
	if(interimCallback) interimCallback();

	//draw frame around the progress bar
	display.drawRect(0,bar_y,params.iScreenCX,bar_height);

	yield();
	if(interimCallback) interimCallback();

	//fill in the progress bar
	int bar_pixels=pSlider->GetProgressBar(params.iScreenCX-4);
	display.fillRect(2, bar_y+2, bar_pixels, bar_height-4);

}



uint16_t FlexMenuDisplay_OLED_Base::getCharsForWidth(const uint8_t * pFont,const char* text, uint16_t length, uint16_t desiredWidth)
{

  uint16_t firstChar        = pgm_read_byte(pFont + FIRST_CHAR_POS);

  uint16_t numChars=0;

  uint16_t stringWidth = 0;

  for(int i=0;i<length;i++)
  {
    stringWidth += pgm_read_byte(pFont + JUMPTABLE_START + (text[i] - firstChar) * JUMPTABLE_BYTES + JUMPTABLE_WIDTH);
    if(stringWidth>desiredWidth)
    	break;
    numChars++;
  }

  return numChars;
}


bool FlexMenuDisplay_OLED_Base::OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)
{

	if(EditScreen_OnNavigate(pCurMenu, direction, accel)) return true;

	return false;
}

void FlexMenuDisplay_OLED_Base::OnEditMode(FlexMenuBase * pCurMenu, bool bEnable)
{
	EditScreen_OnEditMode(pCurMenu,bEnable);
}

bool FlexMenuDisplay_OLED_Base::DisplayNeedsRefresh(FlexMenuBase * pCurMenu)
{
	if(EditScreen_NeedsRefresh(pCurMenu)) return true;

	return false;
}

void FlexMenuDisplay_OLED_Base::ESCB_DrawEditBox(const FlexMenuEditScreenParams & editparams, const String & strDisplayText, const String & strBeforeCursor, bool bDrawCursor)
{
	OLEDDisplay & display=*params.pOLEDDisplay;

	int edit_height=editparams.iFontHeightEdit+2;

	display.setFont(editparams.pFontEdit);
	display.setColor(WHITE);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(editparams.iEditTextShiftX, 1, strDisplayText);

	int xpos=display.getStringWidth(strBeforeCursor);

	display.drawRect(0,0,editparams.iScreenCX,edit_height);

	display.setColor(INVERSE);
	int cursor_xpos=xpos+editparams.iEditTextShiftX;
	if(cursor_xpos<1) cursor_xpos=1;
	if(bDrawCursor) display.drawVerticalLine(cursor_xpos, 1, editparams.iFontHeightEdit+1);


}


void FlexMenuDisplay_OLED_Base::ESCB_DrawOSK_Key(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy, eEditOskDrawKey mode, const osk_icondef * pIcon, const String * pText)
{
	OLEDDisplay & display=*params.pOLEDDisplay;
	display.setColor(INVERSE);

	switch(mode)
	{
	case eEditOskDrawKey_Selected:
		display.fillRect(x,y,cx,cy);
		break;
	case eEditOskDrawKey_LockedIn:
		display.drawRect(x,y,cx,cy);
		break;
	default:
		break;
	}

	yield();

	if(interimCallback) interimCallback();

	if(pIcon && pIcon->data)
	{
		display.setColor(INVERSE);

		display.drawXbm(x+((cx-pIcon->cx)>>1)+pIcon->shift_x, y+((cy-pIcon->cy)>>1)+pIcon->shift_y, pIcon->cx, pIcon->cy, pIcon->data);
	}

	if(pText)
	{
		display.setTextAlignment(TEXT_ALIGN_CENTER);
		display.drawString(x+(cx>>1),y,*pText);
	}


}


void FlexMenuDisplay_OLED_Base::DrawMessage(FlexMenuBase * pCurMenu)
{
	OLEDDisplay & display=*params.pOLEDDisplay;


	const uint8_t * pUseFont=GetFont(pCurMenu->GetFont());

	display.setFont(pUseFont);

	display.setColor(WHITE);


	String strTemp1;
	String strTemp2;

	pCurMenu->GetTitleText(strTemp1);
	pCurMenu->GetValueText(strTemp2);

	if(strTemp1.length() && !strTemp2.length())
	{
		strTemp2=strTemp1;
		strTemp1="";
	}


	int length1=display.getStringWidth(strTemp1);
	int length2=display.getStringWidth(strTemp2);

	//csprintf("screenCX is %i\n",params.iScreenCX);

	int line2_y=0;

	yield();

	if(length1 && length2)
	{
		display.setTextAlignment(TEXT_ALIGN_CENTER);
		display.drawString(params.iScreenCX>>1 /*middle*/, 0, strTemp1);

		line2_y=iLineHeight + ((params.iScreenCY - iLineHeight) >> 1);
	}
	else
	{
		line2_y=params.iScreenCY>>1;
	}


	yield();

	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	if(strTemp2.indexOf('\n')>=0)
	{
		display.drawString(params.iScreenCX>>1 /*middle*/, line2_y, strTemp2);
	}
	else
	{
		display.drawStringMaxWidth(params.iScreenCX>>1 /*middle*/, line2_y, params.iScreenCX, strTemp2);
	}


}

bool FlexMenuDisplay_OLED_Base::HistoryBuffer(FlexMenuBase * pCurMenu, uintptr_t * data)
{
	switch(pCurMenu->GetScreenType())
	{
	default:
		return false;
	case eFlexMenuScreenType_Edit:
		EditScreen_HistoryBuffer(data);
		return true;
	}
}


void FlexMenuDisplay_OLED_Base::Output()
{
	params.pOLEDDisplay->display();
}



