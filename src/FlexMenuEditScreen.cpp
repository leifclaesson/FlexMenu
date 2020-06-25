#include "FlexMenuEditScreen.h"
#include "FlexMenuItemEdit.h"
#include <SH1106.h>



FlexMenuEditScreen::FlexMenuEditScreen()
{

}

FlexMenuEditScreen::~FlexMenuEditScreen()
{

}



void FlexMenuEditScreen::EditScreen_Init(FlexMenuEditScreenParams * pParams)
{
	params=*pParams;
	selection=params.initial_osk_selection;

}



bool FlexMenuEditScreen::EditHandlePush(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)
{
	(void)(accel);
	uint8_t osk_cur=ReadOskTable(selection);

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

bool FlexMenuEditScreen::EditScreen_OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)
{

	FlexMenuBase * pCurItem=pCurMenu->GetCurItemPtr();

	if(!pCurItem || pCurItem->GetScreenType()!=eFlexMenuScreenType_Edit) return false;

	//Serial.printf("OnNavigate EDIT %i\n",direction);
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
				if(ReadOskTable(selection)<32) use_accel=1;
				selection-=use_accel;
				if(selection<0) selection+=params.osk_table_length;
			} while(!ReadOskTable(selection));

			iLastDirection=-1;

			DoScrollKeyboard(iLastDirection);

//				Serial.printf("pos: %i  scroll_y=%i  pos/osk=%i\n",selection,scroll_y,(selection/osk_width)-scroll_y);

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
				if(ReadOskTable(selection)<32) use_accel=1;
				selection+=use_accel;
				if(selection>=params.osk_table_length) selection-=params.osk_table_length;
			} while(!ReadOskTable(selection));

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

		csprintf("ENTER: POS %i\n",selection);
		break;
	}
	return false;

}

void FlexMenuEditScreen::HandleOskErase(char osk_cur)
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

void FlexMenuEditScreen::HandleOskInsert(char osk_cur)
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

	if(iUseCursor>=(params.iMaxCharsX))
	{
//		csprintf("scroll_right iUseCursor=%i max=%i\n",iUseCursor,iMaxCharsX);
		iScrollX+=(iUseCursor-(params.iMaxCharsX));
		iUseCursor=iCursor-iScrollX;

	}


}


uint8_t FlexMenuEditScreen::ReadOskTable(int selection)
{

	const void * ptr=&params.osk_table[selection];	//second byte of font is height
	uint8_t test=0;
	pgm_read_with_offset(ptr,test)	//it's in flash so we need to call the helper function to read it

	return test;
}


void FlexMenuEditScreen::EditScreen_Draw(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem)
{
	(void)(pCurMenu); (void)(pCurItem);

	OLEDDisplay & display=*pDisplay;

	//FlexMenuItemEdit & edit=*((FlexMenuItemEdit *) pCurMenu);


	int iFontHeight=0;

	{
		const void * ptr=&params.pFontEdit[1];	//second byte of font is height
		uint8_t test=0;
		pgm_read_with_offset(ptr,test)	//it's in flash so we need to call the helper function to read it
		iFontHeight=test-2;
	}

	int edit_height=iFontHeight+2;

	display.setFont(params.pFontEdit);

	display.setColor(INVERSE);

	display.setTextAlignment(TEXT_ALIGN_LEFT);

	int iUseCursor=iCursor-iScrollX;



	String strBeforeCursor=strEdit.substring(iScrollX, iUseCursor+iScrollX);

	//csprintf("iScrollX=%i iCursor=%i strbef=%s ",iScrollX,iCursor,strBeforeCursor.c_str());

	display.drawString(params.iEditTextShiftX, 1, strEdit.substring(iScrollX));
	int xpos=display.getStringWidth(strBeforeCursor);

	//csprintf("usecursor=%i ",iUseCursor);
	//csprintf("xpos=%i ",xpos);


	//display.drawHorizontalLine(0, 0, iScreenCX);
	//display.drawHorizontalLine(0, edit_height-1, iScreenCX);

	display.setColor(WHITE);

	display.drawRect(0,0,params.iScreenCX,edit_height);

	display.setColor(INVERSE);
	int cursor_xpos=xpos+params.iEditTextShiftX;
	if(cursor_xpos<1) cursor_xpos=1;
	if(bDrawCursor) display.drawVerticalLine(cursor_xpos, 1, iFontHeight+1);


	//csprintf("\n");

	int cells_y=params.osk_height;

	int cell_width_lsh10=((params.iScreenCX<<10) / params.osk_width);
	//int cell_height=edit_height-1;

	String str;
	str=" ";

	//int table_idx=0;

	for(int y=0;y<cells_y;y++)
	{
		int cur_y=(y*iFontHeight)+edit_height;

		//current table start selection is
		int table_idx=(((y+scroll_y)*params.osk_width)+params.osk_table_length) % params.osk_table_length;
		if(ReadOskTable(table_idx)<32)
		{
			//control row

			int cell_x=((params.iScreenCX+2)<<10) / params.num_control_chars;

			display.setTextAlignment(TEXT_ALIGN_LEFT);

//			int cur_cy=iScreenCY-cur_y;

			for(int i=0;i<params.num_control_chars;i++)
			{
				int cur_x=(cell_x*i)>>10;
				int next_x=(cell_x*(i+1))>>10;
				int cur_cx=(next_x-cur_x)-2;

				if(selection==table_idx+i)
				{
					bool bFill=true;
					if(ReadOskTable(selection)==eFlexMenuEdit_CaptureCursor && bCursorMode) bFill=false;

					if(bFill)
					{
						display.fillRect(cur_x, cur_y, cur_cx, iFontHeight);
					}
					else
					{
						display.drawRect(cur_x, cur_y, cur_cx, iFontHeight);
					}
				}

				osk_icondef & icon=params.osk_icons[ReadOskTable(table_idx+i)];
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
			for(int i=0;i<params.osk_width;i++)
			{
				str.setCharAt(0, ReadOskTable(table_idx+i));

				int cur_x=(cell_width_lsh10*i)>>10;

				if(selection==table_idx+i)
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

bool FlexMenuEditScreen::EditScreen_NeedsRefresh(FlexMenuBase * pCurMenu)
{

	FlexMenuBase * pCurItem=pCurMenu->GetCurItemPtr();

	if(pCurItem && pCurItem->GetScreenType()==eFlexMenuScreenType_Edit)
	{
		bDrawCursor=((millis()-cursor_millis) & 1023)<512;
		if(bLastDrawCursor!=bDrawCursor)
		{
			bLastDrawCursor=bDrawCursor;
			return true;
		}
	}

	return false;
}

void FlexMenuEditScreen::EditScreen_OnEditMode(FlexMenuBase * pCurMenu, bool bEnable)
{
	FlexMenuItemEdit * pItemEdit=(FlexMenuItemEdit *) pCurMenu;
	if(bEnable)
	{
		strEdit=pItemEdit->strEdit;
		selection=28+4;
		iCursor=strEdit.length();
		iScrollX=iCursor-params.iMaxCharsX+1;
		CursorNav(0);
		DoScrollKeyboard(-1);
		csprintf("InitEdit: %s. cursor pos %i\n",strEdit.c_str(),iCursor);
	}
}






void FlexMenuEditScreen::CursorNav(int iDirection)
{

	if(iDirection)
	{
		cursor_millis=millis();
		iCursor+=iDirection;

		if(iDirection>0 && iCursor>(int) strEdit.length()) iCursor=strEdit.length();
		if(iDirection<0 && iCursor<0) iCursor=0;
	}


	if((int) strEdit.length()-iScrollX<params.iMaxCharsX)
	{
		iScrollX = strEdit.length()-params.iMaxCharsX;
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


	if(iUseCursor>=(params.iMaxCharsX-1) && iDirection>0 && iCursor<(int) strEdit.length())
	{
		csprintf("scroll_right iUseCursor=%i max=%i\n",iUseCursor,params.iMaxCharsX);
		iScrollX+=(iUseCursor-(params.iMaxCharsX-1));
		iUseCursor=iCursor-iScrollX;

	}

}


void FlexMenuEditScreen::DoScrollKeyboard(int iDirection)
{

	if(iDirection<0)
	{
		if((selection / params.osk_width)-scroll_y<1 || (selection / params.osk_width)-scroll_y>params.osk_height-2)
		{
			scroll_y = (selection / params.osk_width) - 1;
		}
	}

	if(iDirection>0)
	{
		if((selection / params.osk_width)-scroll_y<1 || (selection / params.osk_width)-scroll_y>params.osk_height-2)
		{
			scroll_y = (selection / params.osk_width) - (params.osk_height-2);
		}
	}
}


