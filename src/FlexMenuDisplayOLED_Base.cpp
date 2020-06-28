// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include <FlexMenuDisplayOLED_Base.h>
#include <SH1106.h>
#include "FlexMenuBase.h"
#include "FlexMenuItemSlider.h"


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

void FlexMenuDisplay_OLED_Base::DrawDisplay(FlexMenuBase * pCurMenu)
{
	OLEDDisplay & display=*params.pOLEDDisplay;

	display.clear();
	display.setFont(pFont);


	{
		FlexMenuBase * pCurItem=pCurMenu->GetCurItemPtr();
		if(pCurItem)
		{
			switch(pCurItem->GetScreenType())
			{
			default:
			case eFlexMenuScreenType_Normal:
				break;
			case eFlexMenuScreenType_Slider:
				DrawSliderScreen(pCurMenu,pCurItem);
				return;
			case eFlexMenuScreenType_Edit:
				EditScreen_Draw(pCurMenu,pCurItem);
				return;
			case eFlexMenuScreenType_Message:
				DrawMessage(pCurMenu);
				return;
			}
		}

	}


	float fLineHeight=(float) (params.iScreenCY) / GetVisibleItems();

	for(int i=0;i<GetVisibleItems();i++)
	{

		if(i+pCurMenu->GetScrollPos()>=0 && i+pCurMenu->GetScrollPos()<(int) pCurMenu->GetNumSubItems())
		{
			int iCurItem=i+pCurMenu->GetScrollPos();
			FlexMenuBase * pItem=pCurMenu->GetSubItem(iCurItem);

			String strLeft;
			String strRight;
			pItem->GetTitleText(strLeft);
			pItem->GetValueText(strRight);
			eFlexMenuIcon icon=pItem->UseIcon();

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


			display.setColor(WHITE);

			display.setTextAlignment(TEXT_ALIGN_LEFT);
			display.drawString(left, i*fLineHeight, strLeft );
			int widthLeft=display.getStringWidth(strLeft);

			display.setTextAlignment(TEXT_ALIGN_RIGHT);

			int chars=getCharsForWidth(pFont,strRight.c_str(),strRight.length(),params.iScreenCX-widthLeft-iIconCX-6);

			if(chars!=(int) strRight.length())
			{
				String temp=strRight.substring(0,chars);
				temp+="..";
				display.drawString(right, i*fLineHeight, temp);
			}
			else
			{
				display.drawString(right, i*fLineHeight, strRight);
			}


		}

	}


	{
		int iCurItem=pCurMenu->GetCurItem()-pCurMenu->GetScrollPos();

		if(iCurItem>=0 && iCurItem<GetVisibleItems())
		{
			display.setColor(INVERSE);

			display.fillRect( 0, iCurItem*fLineHeight, params.iScreenCX, iLineHeight+1);
		}
	}


}

void FlexMenuDisplay_OLED_Base::DrawSliderScreen(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem)
{
	(void)(pCurMenu);
	OLEDDisplay & display=*params.pOLEDDisplay;

	FlexMenuItemSlider * pSlider=(FlexMenuItemSlider *) pCurItem;

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


	String strTitle;
	pCurItem->GetTitleText(strTitle);

	String strValue;
	pCurItem->GetValueText(strValue);

	display.setColor(WHITE);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.drawString(params.iScreenCX/2, 0, strTitle);

	display.drawString(params.iScreenCX/2, params.iScreenCY-iFontHeight, strValue);

	int bar_height=(params.iScreenCY * 30) / 100;

	int bar_y=(params.iScreenCY-bar_height)/2;

	//draw frame around the progress bar
	display.drawRect(0,bar_y,params.iScreenCX,bar_height);

	//fill in the progress bar
	int bar_pixels=pSlider->GetProgressBar(params.iScreenCX-4);
	display.fillRect(2, bar_y+2, bar_pixels, bar_height-4);

}



uint16_t FlexMenuDisplay_OLED_Base::getCharsForWidth(const uint8_t * pFont,const char* text, uint16_t length, uint16_t desiredWidth)
{

  uint16_t firstChar        = pgm_read_byte(pFont + FIRST_CHAR_POS);

  uint16_t numChars=0;

  uint16_t stringWidth = 0;

  while (length--) {
    stringWidth += pgm_read_byte(pFont + JUMPTABLE_START + (text[length] - firstChar) * JUMPTABLE_BYTES + JUMPTABLE_WIDTH);
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

	const uint8_t * pUseFont=pFont;
	if(pFontSlider)
	{
		pUseFont=pFontSlider;
	}

	display.setColor(WHITE);
	display.setFont(pUseFont);

	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);

	String strTemp1;
	String strTemp2;

	pCurMenu->GetTitleText(strTemp1);
	pCurMenu->GetValueText(strTemp2);

	if(strTemp1.length() && !strTemp2.length())
	{
		strTemp2=strTemp1;
		strTemp1="";
	}

	if(strTemp1.length() && strTemp2.length())
	{
		display.drawString(params.iScreenCX>>1 /*middle*/, (params.iScreenCY * 85) >> 8 /*one third*/ , strTemp1);
		display.drawString(params.iScreenCX>>1 /*middle*/, (params.iScreenCY * 171) >> 8 /*two thirds*/ , strTemp2);
	}
	else
	{
		display.drawString(params.iScreenCX>>1 /*middle*/, params.iScreenCY >> 1 /*middle*/ , strTemp2);
	}

}






