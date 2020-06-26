/*
 * FlexMenuDisplaySH1106.cpp
 *
 *  Created on: 21 Jun 2020
 *      Author: user
 */

#include <FlexMenuDisplayOLEDDisplay.h>
#include <SH1106.h>
#include "FlexMenuBase.h"
#include "FlexMenuItemSlider.h"


#include "fonts\Monospaced_14.h"


#define FILETOARRAY_EXTERN_ONLY
#include "icons\icons.h"


osk_icondef osk_icons[7]=	//must match eFlexMenuEdit.
{
	{0,0,0,0,0},
	{arrows,10,8,0,1},
	{spacebar,10,8,0,2},
	{delete_symbol,12,9,1,0},
	{backspace,12,9,-1,0},
	{cancel,7,6,0,1},
	{okay,8,6,0,1},
};


char PROGMEM _osk_table[]={
		"ABCDEFGHIJKLMN"
		"OPQRSTUVWXYZ.,"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		"abcdefghijklmn"
		"opqrstuvwxyz-_"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		"#$%&:/\\;()[]!?"
		"<>@'`{|}~^\"_.,"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		"0123456789-+=*"
		};



FlexMenuDisplay_OLEDDisplay::FlexMenuDisplay_OLEDDisplay()
{
}

FlexMenuDisplay_OLEDDisplay::~FlexMenuDisplay_OLEDDisplay()
{
	// TODO Auto-generated destructor stub
}

void FlexMenuDisplay_OLEDDisplay::SetParams(FlexMenuDisplay_OLEDDisplay_Params * pParams)
{
	params=*pParams;
}

void FlexMenuDisplay_OLEDDisplay::Init()
{
	OLEDDisplay & display=*params.pOLEDDisplay;
	(void)(display);

	iVisibleItems=params.iScreenCY/params.iLineHeight;

	FlexMenuEditScreenParams ep;

	ep.iScreenCX=params.iScreenCX;
	ep.iScreenCY=params.iScreenCY;

	ep.iMaxCharsX=15;

	ep.osk_width=14;
	ep.osk_height=3;

	ep.iEditTextShiftX=3;

	ep.osk_table=(uint8_t *) _osk_table;
	ep.osk_table_length=sizeof(_osk_table)-1;
	ep.num_control_chars=6;

	ep.osk_icons=osk_icons;

	ep.pFontEdit=(const uint8_t *) Monospaced_14;

	ep.pOLEDDisplay=params.pOLEDDisplay;

	EditScreen_Init(&ep);

}



int FlexMenuDisplay_OLEDDisplay::GetVisibleItems()
{
	return iVisibleItems;
}

void FlexMenuDisplay_OLEDDisplay::DrawDisplay(FlexMenuBase * pCurMenu)
{
	OLEDDisplay & display=*params.pOLEDDisplay;

	display.clear();
	display.setFont(params.pFont);

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


			if(icon<eFlexMenuIcon_Count && params.icons[icon])
			{
				if(icon==eFlexMenuIcon_LeftArrow)
				{
					left+=params.iIconCX;
					if(params.icons[icon])
					{
						display.drawXbm(0, i*fLineHeight+params.iIconY, params.iIconCX, params.iIconCY, params.icons[icon]);
					}
				}
				else
				{
					if(icon!=eFlexMenuIcon_None) right-=params.iIconCX;
					//right-=params.iIconCX;
					if(params.icons[icon])
					{
						display.drawXbm(right, i*fLineHeight+params.iIconY, params.iIconCX, params.iIconCY, params.icons[icon]);
					}
				}
			}

			//if(i==0) iUseLineHeight++;


			display.setColor(WHITE);

			display.setTextAlignment(TEXT_ALIGN_LEFT);
			display.drawString(left, i*fLineHeight, strLeft );
			int widthLeft=display.getStringWidth(strLeft);

			display.setTextAlignment(TEXT_ALIGN_RIGHT);

			int chars=getCharsForWidth(params.pFont,strRight.c_str(),strRight.length(),params.iScreenCX-widthLeft-params.iIconCX-6);

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


			//drawString(right, i*fLineHeight, strRight );


		}

	}


	{
		int iCurItem=pCurMenu->GetCurItem()-pCurMenu->GetScrollPos();

		if(iCurItem>=0 && iCurItem<GetVisibleItems())
		{
			display.setColor(INVERSE);

			display.fillRect( 0, iCurItem*fLineHeight, params.iScreenCX, params.iLineHeight+1);
		}
	}



	/*
	display.drawFastImage(0, 0, 6, 8, checked_10);
	display.drawFastImage(10, 0, 6, 8, unchecked_10);
	display.drawFastImage(20, 0, 6, 8, execute_10);
	display.drawFastImage(30, 0, 6, 8, leftarrow_10);
	display.drawFastImage(40, 0, 6, 8, rightarrow_10);
	display.drawFastImage(50, 0, 6, 8, selection_10);*/


/*
	display.drawXbm(0, 20, 6, 8, checked_10);
	display.drawXbm(10, 20, 6, 8, unchecked_10);
	display.drawXbm(20, 20, 6, 8, execute_10);
	display.drawXbm(30, 20, 6, 8, leftarrow_10);
	display.drawXbm(40, 20, 6, 8, rightarrow_10);
	display.drawXbm(50, 20, 6, 8, selection_10);
	display.drawXbm(60, 20, 6, 8, circle_10);


	display.drawXbm(0, 0, 7, 10, checked_12);
	display.drawXbm(10, 0, 7, 10, unchecked_12);
	display.drawXbm(20, 0, 7, 10, execute_12);
	display.drawXbm(30, 0, 7, 10, leftarrow_12);
	display.drawXbm(40, 0, 7, 10, rightarrow_12);
	display.drawXbm(50, 0, 7, 10, selection_12);
	display.drawXbm(60, 0, 7, 10, circle_12);

*/
}

void FlexMenuDisplay_OLEDDisplay::DrawSliderScreen(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem)
{
	(void)(pCurMenu);
	OLEDDisplay & display=*params.pOLEDDisplay;

	FlexMenuItemSlider * pSlider=(FlexMenuItemSlider *) pCurItem;

	const uint8_t * pUseFont=params.pFont;
	int iFontHeight=params.iLineHeight;
	if(params.pFontSlider)
	{
		pUseFont=params.pFontSlider;

		const void * ptr=&pUseFont[1];	//second byte of font is height
		uint8_t test=0;
		pgm_read_with_offset(ptr,test)	//it's in flash so we need to call the helper function to read it
		iFontHeight=test-1;
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



uint16_t FlexMenuDisplay_OLEDDisplay::getCharsForWidth(const uint8_t * pFont,const char* text, uint16_t length, uint16_t desiredWidth)
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


bool FlexMenuDisplay_OLEDDisplay::OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)
{

	if(EditScreen_OnNavigate(pCurMenu, direction, accel)) return true;

	return false;
}

void FlexMenuDisplay_OLEDDisplay::OnEditMode(FlexMenuBase * pCurMenu, bool bEnable)
{
	EditScreen_OnEditMode(pCurMenu,bEnable);
}

bool FlexMenuDisplay_OLEDDisplay::DisplayNeedsRefresh(FlexMenuBase * pCurMenu)
{
	if(EditScreen_NeedsRefresh(pCurMenu)) return true;

	return false;
}









