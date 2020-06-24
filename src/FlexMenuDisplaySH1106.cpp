/*
 * FlexMenuDisplaySH1106.cpp
 *
 *  Created on: 21 Jun 2020
 *      Author: user
 */

#include <FlexMenuDisplaySH1106.h>
#include <SH1106.h>
#include "FlexMenuBase.h"
#include "FlexMenuItemSlider.h"
//#include "icons\icons_10.h"
//#include "icons\icons_12.h"


FlexMenuDisplay_SH1106::FlexMenuDisplay_SH1106()
{
	memset(icons,0,sizeof(icons));
}

FlexMenuDisplay_SH1106::~FlexMenuDisplay_SH1106() {
	// TODO Auto-generated destructor stub
}

void FlexMenuDisplay_SH1106::Init()
{
	iVisibleItems=iScreenCY/iLineHeight;
}

int FlexMenuDisplay_SH1106::GetVisibleItems()
{
	return iVisibleItems;
}

void FlexMenuDisplay_SH1106::DrawDisplay(FlexMenuBase * pCurMenu)
{
	OLEDDisplay & display=*pDisplay;

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
			}
		}


	}


	float fLineHeight=(float) (iScreenCY) / GetVisibleItems();

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
			int right=iScreenCX;


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

			//if(i==0) iUseLineHeight++;


			display.setColor(WHITE);

			display.setTextAlignment(TEXT_ALIGN_LEFT);
			display.drawString(left, i*fLineHeight, strLeft );

			display.setTextAlignment(TEXT_ALIGN_RIGHT);
			display.drawString(right, i*fLineHeight, strRight );


		}

	}


	{
		int iCurItem=pCurMenu->GetCurItem()-pCurMenu->GetScrollPos();

		if(iCurItem>=0 && iCurItem<GetVisibleItems())
		{
			display.setColor(INVERSE);

			display.fillRect( 0, iCurItem*fLineHeight, iScreenCX, iLineHeight+1);
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

void FlexMenuDisplay_SH1106::DrawSliderScreen(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem)
{
	OLEDDisplay & display=*pDisplay;

	FlexMenuItemSlider * pSlider=(FlexMenuItemSlider *) pCurItem;

	const uint8_t * pUseFont=pFont;
	int iFontHeight=iLineHeight;
	if(pFontSlider)
	{
		pUseFont=pFontSlider;

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
	display.drawString(iScreenCX/2, 0, strTitle);

	display.drawString(iScreenCX/2, iScreenCY-iFontHeight, strValue);


	int bar_height=((iScreenCY * 30) / 100) & 0xFFFFFFFE;


	int bar_y=(iScreenCY-bar_height)/2;


	display.drawRect(0,bar_y,iScreenCX,bar_height);


	//if(abs(range)>1048576) downshift_bits=8;	//i doubt we'll ever need to support a range that large but at least now we're prepared.

	//int range=pSlider->range_max-pSlider->range_min;

	uint8_t downshift_bits=0;

	int fraction=0;

	if(pSlider->range_max>pSlider->range_min)
	{
		fraction=((pSlider->value - pSlider->range_min)<<(10-downshift_bits)) / ((pSlider->range_max - pSlider->range_min)>>downshift_bits);

		//csprintf("fraction (pos)=%i\n",fraction);
	}
	else
	{
		fraction=(1<<10) - (((pSlider->value - pSlider->range_max)<<(10-downshift_bits)) / ((-(pSlider->range_max - pSlider->range_min))>>downshift_bits));

		//csprintf("fraction (neg)=%i\n",fraction);
	}

	//csprintf("pixels: %i\n",(fraction*173)>>10);

	int bar_pixels=(fraction*(iScreenCX-4)>>10);


	display.fillRect(2, bar_y+2, bar_pixels, bar_height-4);



	//if(


/*
	display.drawVerticalLine(0, (iScreenCY-bar_height)/2, bar_height);
	display.drawVerticalLine(iScreenCX-1, (iScreenCY-bar_height)/2, bar_height);

	display.drawHorizontalLine(1, (iScreenCY-bar_height)/2, iScreenCX-2);
	display.drawHorizontalLine(1, (iScreenCY+bar_height-2)/2, iScreenCX-2);*/

//	display.drawProgressBar(0, (iScreenCY-bar_height)/2, iScreenCX-1, bar_height, 98);

//	display.fillRect( 0, 0, iScreenCX, iScreenCY);
}

