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
#include "FlexMenuItemEdit.h"


#define FILETOARRAY_EXTERN_ONLY
#include "icons\icons.h"

struct icondef
{
	uint8_t * data;
	uint8_t cx;
	uint8_t cy;
	int8_t shift_x;
	int8_t shift_y;
};

icondef keyboard_icons[7]=	//must match eFlexMenuEdit.
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



FlexMenuDisplay_SH1106::FlexMenuDisplay_SH1106()
{
	memset(icons,0,sizeof(icons));
}

FlexMenuDisplay_SH1106::~FlexMenuDisplay_SH1106() {
	// TODO Auto-generated destructor stub
}

void FlexMenuDisplay_SH1106::Init()
{
	OLEDDisplay & display=*pDisplay;
	(void)(display);

	iVisibleItems=iScreenCY/iLineHeight;


	//build osk_table, go through every ascii character and intersperse a row of commands to ensure the commands are always visible somewhere on the screen.



	iMaxCharsX=15;

	osk_width=14;
	osk_height=3;

	iEditTextShiftX=3;
	iIconShiftY=2;

//	uint8_t temp[256];
//	uint8_t idx=0;


	osk_table=(uint8_t *) _osk_table;
	osk_table_length=sizeof(_osk_table)-1;
	num_control_chars=6;


	/*
	osk_table=(uint8_t *) malloc(sizeof(okay)-1);
	memcpy(osk_table,okay,sizeof(okay)-1);
	osk_table_length=sizeof(okay)-1;

	csprintf("osk_table_length %i\n",osk_table_length);

	for(int i=0;i<16;i++)
	{
		csprintf("%02x ",osk_table[i]);

	}
	*/

//

/*


"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
"<>@'`{|}~^"_.,"
"0123456789-+=*"
"ABCDEFGHIJKLMN"
"OPQRSTUVWXYZ[]"
"#$%&:/\;().,!?"
"abcdefghijklmn"
"opqrstuvwxyz-_"



*/
	/*
	uint8_t osk_char='!';	//start at code 33



	while(1)
	{
		if(osk_char<110)
		{
			num_control_chars=0;
			for(int i=eFlexMenuEdit_CaptureCursor;i<=eFlexMenuEdit_OK;i++)
			{
				temp[idx]=i; idx++;
				num_control_chars++;
			}

			while(idx % osk_width != 0)
			{
				temp[idx]=0; idx++;
			}
		}


		for(int y=0;y<osk_height-1;y++)
		{
			for(int i=0;i<osk_width;i++)
			{
				if(osk_char<=126)
				{
					temp[idx]=osk_char; idx++; osk_char++;
				}
			}
		}

		while(idx % osk_width != 0)
		{
			temp[idx]=0; idx++;
		}

		if(osk_char>126)
		{
			break;
		}


	}


	osk_table=(uint8_t *) malloc(idx);
	memcpy(osk_table,temp,idx);
	osk_table_length=idx;

*/

//	Serial.printf("table_idx=%i\n",idx);





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
			case eFlexMenuScreenType_Edit:
				DrawEditScreen(pCurMenu,pCurItem);
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
			int widthLeft=display.getStringWidth(strLeft);

			display.setTextAlignment(TEXT_ALIGN_RIGHT);

			int chars=getCharsForWidth(pFont,strRight.c_str(),strRight.length(),iScreenCX-widthLeft-iIconCX-6);

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
	(void)(pCurMenu);
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


bool FlexMenuDisplay_SH1106::EditHandlePush(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)
{
	uint8_t osk_cur=ReadOskTable(position);

	if(direction==eFlexMenuNav_Repeat)
	{
		switch(osk_cur)
		{
		case eFlexMenuEdit_Backspace:
		case eFlexMenuEdit_Delete:
			break;
		default:
			return false;
		}
	}

	if(osk_cur<32)
	{
		switch(osk_cur)
		{
		case eFlexMenuEdit_CaptureCursor:
			csprintf("Capture cursor!\n");
			bCursorMode=true;
			pCurMenu->SetNeedsRefresh(true);
			break;
		case eFlexMenuEdit_Space:
			HandleOskInsert(' ');
			pCurMenu->SetNeedsRefresh(true);
			break;
		case eFlexMenuEdit_Backspace:
			HandleOskErase(osk_cur);
			pCurMenu->SetNeedsRefresh(true);
			break;
		case eFlexMenuEdit_Delete:
			HandleOskErase(osk_cur);
			pCurMenu->SetNeedsRefresh(true);
			break;
		case eFlexMenuEdit_Cancel:
			return true;
		case eFlexMenuEdit_OK:
			{
				FlexMenuItemEdit * pItemEdit=(FlexMenuItemEdit *) pCurMenu;
				pItemEdit->strEdit=strEdit;
			}
			return true;
		}
	}
	else
	{
		HandleOskInsert(osk_cur);
		pCurMenu->SetNeedsRefresh(true);
	}

	return false;
}

bool FlexMenuDisplay_SH1106::OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)
{

	FlexMenuBase * pCurItem=pCurMenu->GetCurItemPtr();

	bool bNewItem=false;
	if(pLastCurItem!=pCurItem)
	{
		pLastCurItem=pCurItem;
		bNewItem=true;
	}

	if(pCurItem)
	{
		switch(pCurItem->GetScreenType())
		{
		default:
			break;
		case eFlexMenuScreenType_Edit:
			//Serial.printf("OnNavigate EDIT %i\n",direction);
			if(bNewItem)
			{
				InitEdit(((FlexMenuItemEdit *) pCurMenu));
			}

			int use_accel=1;
			if(accel>=30) use_accel=2;
			switch(direction)
			{
			default:
				break;
			case eFlexMenuNav_Prev:
				if(bCursorMode)
				{
					CursorNav(-1);
				}
				else
				{
					do
					{
						if(ReadOskTable(position)<32) use_accel=1;
						position-=use_accel;
						if(position<0) position+=osk_table_length;
					} while(!ReadOskTable(position));

					iLastDirection=-1;

					DoScrollKeyboard(iLastDirection);

	//				Serial.printf("pos: %i  scroll_y=%i  pos/osk=%i\n",position,scroll_y,(position/osk_width)-scroll_y);

				}


				pCurMenu->SetNeedsRefresh(true);
				break;
			case eFlexMenuNav_Next:
				if(bCursorMode)
				{
					CursorNav(1);
				}
				else
				{
					do
					{
						if(ReadOskTable(position)<32) use_accel=1;
						position+=use_accel;
						if(position>=osk_table_length) position-=osk_table_length;
					} while(!ReadOskTable(position));

					iLastDirection=1;

					DoScrollKeyboard(iLastDirection);

				}

				pCurMenu->SetNeedsRefresh(true);
				break;
			case eFlexMenuNav_Repeat:
				if(!bCursorMode)
				{
					EditHandlePush(pCurMenu,direction,accel);
				}
				break;
			case eFlexMenuNav_Push:
				if(bCursorMode)
				{
					bCursorMode=false;
					pCurMenu->SetNeedsRefresh(true);
				}
				else
				{
					return EditHandlePush(pCurMenu,direction,accel);
				}

				csprintf("ENTER: POS %i\n",position);
				break;
			}
			return false;
		}
	}

	return false;

}

void FlexMenuDisplay_SH1106::HandleOskErase(char osk_cur)
{
	switch(osk_cur)
	{
	default:
		break;
	case eFlexMenuEdit_Backspace:
		if(iCursor>0)
		{
			String temp=strEdit.substring(0,iCursor-1);
			temp.concat(strEdit.substring(iCursor));
			strEdit=temp;
			CursorNav(-1);
		}
		break;
	case eFlexMenuEdit_Delete:
		if(iCursor<(int) strEdit.length())
		{
			String temp=strEdit.substring(0,iCursor);
			temp.concat(strEdit.substring(iCursor+1));
			strEdit=temp;
		}
		break;


	}

}

void FlexMenuDisplay_SH1106::HandleOskInsert(char osk_cur)
{
	if(iCursor>=(int) strEdit.length())
	{
		strEdit.concat(osk_cur);
	}
	else
	{
		String temp=strEdit.substring(0,iCursor);
		temp.concat(osk_cur);
		temp.concat(strEdit.substring(iCursor));
		strEdit=temp;
	}

	iCursor++;

	int iUseCursor=iCursor-iScrollX;

	if(iUseCursor>=(iMaxCharsX))
	{
//		csprintf("scroll_right iUseCursor=%i max=%i\n",iUseCursor,iMaxCharsX);
		iScrollX+=(iUseCursor-(iMaxCharsX));
		iUseCursor=iCursor-iScrollX;

	}


}


uint8_t FlexMenuDisplay_SH1106::ReadOskTable(int position)
{

	const void * ptr=&osk_table[position];	//second byte of font is height
	uint8_t test=0;
	pgm_read_with_offset(ptr,test)	//it's in flash so we need to call the helper function to read it

	return test;
}


void FlexMenuDisplay_SH1106::DrawEditScreen(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem)
{
	(void)(pCurMenu); (void)(pCurItem);

	OLEDDisplay & display=*pDisplay;

	//FlexMenuItemEdit & edit=*((FlexMenuItemEdit *) pCurMenu);


	int iFontHeight=0;

	{
		const void * ptr=&pFontEdit[1];	//second byte of font is height
		uint8_t test=0;
		pgm_read_with_offset(ptr,test)	//it's in flash so we need to call the helper function to read it
		iFontHeight=test-2;
	}

	int edit_height=iFontHeight+2;

	display.setFont(pFontEdit);

	display.setColor(INVERSE);

	display.setTextAlignment(TEXT_ALIGN_LEFT);

	int iUseCursor=iCursor-iScrollX;



	String strBeforeCursor=strEdit.substring(iScrollX, iUseCursor+iScrollX);

	//csprintf("iScrollX=%i iCursor=%i strbef=%s ",iScrollX,iCursor,strBeforeCursor.c_str());

	display.drawString(iEditTextShiftX, 1, strEdit.substring(iScrollX));
	int xpos=display.getStringWidth(strBeforeCursor);

	//csprintf("usecursor=%i ",iUseCursor);
	//csprintf("xpos=%i ",xpos);


	//display.drawHorizontalLine(0, 0, iScreenCX);
	//display.drawHorizontalLine(0, edit_height-1, iScreenCX);

	display.setColor(WHITE);

	display.drawRect(0,0,iScreenCX,edit_height);

	display.setColor(INVERSE);
	int cursor_xpos=xpos+iEditTextShiftX;
	if(cursor_xpos<1) cursor_xpos=1;
	if(bDrawCursor) display.drawVerticalLine(cursor_xpos, 1, iFontHeight+1);


	//csprintf("\n");

	int cells_y=osk_height;

	int cell_width_lsh10=((iScreenCX<<10) / osk_width);
	//int cell_height=edit_height-1;

	String str;
	str=" ";

	//int table_idx=0;

	for(int y=0;y<cells_y;y++)
	{
		int cur_y=(y*iFontHeight)+edit_height;

		//current table start position is
		int table_idx=(((y+scroll_y)*osk_width)+osk_table_length) % osk_table_length;
		if(ReadOskTable(table_idx)<32)
		{
			//control row

			int cell_x=((iScreenCX+2)<<10) / num_control_chars;

			display.setTextAlignment(TEXT_ALIGN_LEFT);

//			int cur_cy=iScreenCY-cur_y;

			for(int i=0;i<num_control_chars;i++)
			{
				int cur_x=(cell_x*i)>>10;
				int next_x=(cell_x*(i+1))>>10;
				int cur_cx=(next_x-cur_x)-2;

				if(position==table_idx+i)
				{
					bool bFill=true;
					if(ReadOskTable(position)==eFlexMenuEdit_CaptureCursor && bCursorMode) bFill=false;

					if(bFill)
					{
						display.fillRect(cur_x, cur_y, cur_cx, iFontHeight);
					}
					else
					{
						display.drawRect(cur_x, cur_y, cur_cx, iFontHeight);
					}
				}

				icondef & icon=keyboard_icons[ReadOskTable(table_idx+i)];
				if(icon.data)
				{
					display.setColor(INVERSE);
					display.drawXbm(cur_x+((cur_cx-icon.cx)>>1)+icon.shift_x, cur_y+((iFontHeight-icon.cy)>>1)+icon.shift_y, icon.cx, icon.cy, icon.data);
				}
			}

		}
		else
		{
			//normal row
			for(int i=0;i<osk_width;i++)
			{
				str.setCharAt(0, ReadOskTable(table_idx+i));

				int cur_x=(cell_width_lsh10*i)>>10;

				if(position==table_idx+i)
				{
					display.fillRect(cur_x,cur_y,(cell_width_lsh10)>>10,iFontHeight);
				}


				display.setColor(INVERSE);

				display.setTextAlignment(TEXT_ALIGN_CENTER);
				display.drawString(cur_x+(cell_width_lsh10>>11),cur_y,str);

			}
		}
	}

}

bool FlexMenuDisplay_SH1106::EditNeedsRefresh()
{
	bDrawCursor=((millis()-cursor_millis) & 1023)<512;
	if(bLastDrawCursor!=bDrawCursor)
	{
		bLastDrawCursor=bDrawCursor;
		return true;
	}

	return false;
}

void FlexMenuDisplay_SH1106::InitEdit(FlexMenuItemEdit * pItemEdit)
{
	strEdit=pItemEdit->strEdit;
	position=28+4;
	iCursor=strEdit.length();
	iScrollX=iCursor-iMaxCharsX+1;
	CursorNav(0);
	DoScrollKeyboard(-1);
	csprintf("InitEdit: %s. cursor pos %i\n",strEdit.c_str(),iCursor);
}





void FlexMenuDisplay_SH1106::CursorNav(int iDirection)
{

	if(iDirection)
	{
		cursor_millis=millis();
		iCursor+=iDirection;

		if(iDirection>0 && iCursor>(int) strEdit.length()) iCursor=strEdit.length();
		if(iDirection<0 && iCursor<0) iCursor=0;
	}


	if((int) strEdit.length()-iScrollX<iMaxCharsX)
	{
		iScrollX = strEdit.length()-iMaxCharsX;
		if(iScrollX<0) iScrollX=0;
	}


	int iUseCursor=iCursor-iScrollX;



	if(iUseCursor<1)
	{
		csprintf("A %i ",iUseCursor);
		iScrollX+=iUseCursor-1;
		if(iScrollX<0) iScrollX=0;
		iUseCursor=iCursor-iScrollX;

	}
	if(iScrollX<0) iScrollX=0;


	if(iUseCursor>=(iMaxCharsX-1) && iDirection>0 && iCursor<(int) strEdit.length())
	{
		csprintf("scroll_right iUseCursor=%i max=%i\n",iUseCursor,iMaxCharsX);
		iScrollX+=(iUseCursor-(iMaxCharsX-1));
		iUseCursor=iCursor-iScrollX;

	}

}


void FlexMenuDisplay_SH1106::DoScrollKeyboard(int iDirection)
{

	if(iDirection<0)
	{
		if((position / osk_width)-scroll_y<1 || (position / osk_width)-scroll_y>osk_height-2)
		{
			scroll_y = (position / osk_width) - 1;
		}
	}

	if(iDirection>0)
	{
		if((position / osk_width)-scroll_y<1 || (position / osk_width)-scroll_y>osk_height-2)
		{
			scroll_y = (position / osk_width) - (osk_height-2);
		}
	}
}



uint16_t FlexMenuDisplay_SH1106::getCharsForWidth(const uint8_t * pFont,const char* text, uint16_t length, uint16_t desiredWidth)
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













