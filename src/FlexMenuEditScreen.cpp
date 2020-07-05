// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "FlexMenuEditScreen.h"
#include "FlexMenuItemEdit.h"


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
	history_selection=-1;
}


bool FlexMenuEditScreen::EditHandlePush(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)
{
	(void)(accel);

	int use_selection=history_selection>=0?history_selection:selection;

	if(use_selection!=selection)
	{
		csprintf("EditHandlePush rewrite history!\n");
		selection=use_selection;
		pCurMenu->SetNeedsRefresh(true);
	}

	uint8_t osk_cur=ReadOskTable(use_selection);

	if(direction==eFlexMenuNav_PushRepeat)
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
				pItemEdit->OskSetString(strEdit);
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

	if(pCurMenu->GetScreenType()!=eFlexMenuScreenType_Edit) return false;

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
	case eFlexMenuNav_PushRepeat:
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
		iScrollX+=(iUseCursor-(params.iMaxCharsX));
		iUseCursor=iCursor-iScrollX;

	}


}


uint8_t FlexMenuEditScreen::ReadOskTable(int selection)
{
#if defined(ARDUINO_ARCH_ESP32)
	return params.osk_table[selection];
#else
	const void * ptr=&params.osk_table[selection];	//second byte of font is height
	uint8_t test=0;
	pgm_read_with_offset(ptr,test)	//it's in flash so we need to call the helper function to read it

	return test;
#endif
}


void FlexMenuEditScreen::EditScreen_Draw(FlexMenuBase * pCurMenu)
{
	(void)(pCurMenu);

	int iUseCursor=iCursor-iScrollX;
	ESCB_DrawEditBox(params,strEdit.substring(iScrollX),strEdit.substring(iScrollX, iUseCursor+iScrollX),bDrawCursor);


	int iFontHeight=params.iFontHeightEdit;

	int edit_height=iFontHeight+2;

	int cells_y=params.osk_height;

	int cell_width_lsh10=((params.iScreenCX<<10) / params.osk_width);	//fixed point. calculate width multiplied by 1024 (left shifted by 10)

	String str;
	str=" ";

	for(int y=0;y<cells_y;y++)
	{
		int cur_y=(y*iFontHeight)+edit_height;

		//index of the beginning of this row
		int table_idx=(((y+scroll_y)*params.osk_width)+params.osk_table_length) % params.osk_table_length;
		if(ReadOskTable(table_idx)<32)
		{
			//control row

			int cell_x=((params.iScreenCX+2)<<10) / params.num_control_chars;

			for(int i=0;i<params.num_control_chars;i++)
			{
				int cur_x=(cell_x*i)>>10;
				int next_x=(cell_x*(i+1))>>10;
				int cur_cx=(next_x-cur_x)-2;


				eEditOskDrawKey mode=eEditOskDrawKey_NoFrame;

				if(selection==table_idx+i)
				{
					mode=eEditOskDrawKey_Selected;
					if(ReadOskTable(selection)==eFlexMenuEdit_CaptureCursor && bCursorMode) mode=eEditOskDrawKey_LockedIn;
				}

				osk_icondef & icon=params.osk_icons[ReadOskTable(table_idx+i)];

				ESCB_DrawOSK_Key(cur_x, cur_y, cur_cx, iFontHeight, mode, &icon, NULL);

			}

		}
		else
		{
			//normal row
			for(int i=0;i<params.osk_width;i++)
			{
				str.setCharAt(0, ReadOskTable(table_idx+i));

				int cur_x=(cell_width_lsh10*i)>>10;

				eEditOskDrawKey mode=eEditOskDrawKey_NoFrame;

				if(selection==table_idx+i)
				{
					mode=eEditOskDrawKey_Selected;
				}

				ESCB_DrawOSK_Key(cur_x, cur_y, (cell_width_lsh10)>>10, iFontHeight, mode, NULL, &str);

			}
		}
	}

}

bool FlexMenuEditScreen::EditScreen_NeedsRefresh(FlexMenuBase * pCurMenu)
{

	if(pCurMenu->GetScreenType()==eFlexMenuScreenType_Edit)
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
		csprintf("pItemEdit->strEdit=%s\n",pItemEdit->strEdit.c_str());
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

void FlexMenuEditScreen::EditScreen_HistoryBuffer(uintptr_t * data)
{
	history_selection=*data;
	*data=selection;

	//csprintf("FlexMenuEditScreen::HistoryBuffer    was: %i   now: %i\n",history_selection,selection);

}


