#include "FlexMenuDisplayU8G2.h"
#include "FlexMenuBase.h"

#ifdef FLEXMENU_U8G2

#include "icons\icons.h"
#include <U8g2lib.h>

#include <FlexMenuItemSlider.h>


osk_icondef osk_icons[7] =  	//must match eFlexMenuEdit.
{
	{0, 0, 0, 0, 0},
	{arrows, 10, 8, 0, 1},
	{spacebar, 10, 8, 0, 2},
	{delete_symbol, 12, 9, 1, 0},
	{backspace, 12, 9, -1, 0},
	{cancel, 7, 6, 0, 1},
	{okay, 8, 6, 0, 1},
};

/*
char PROGMEM _osk_table[] =
{
	"0123456789_+=*"
	"ABCDEFGHIJKLMN"
	"OPQRSTUVWXYZ.-"
	"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
	"abcdefghijklmn"
	"opqrstuvwxyz.-"
	"0123456789-+=*"
	"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
	"!?#$%&:;/\\()[]"
	"<>@'`{|}~^\"_.,"
	"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
};
*/

char PROGMEM _osk_table[] =
{
	"0123456789_.-"
	"ABCDEFGHIJKLM"
	"NOPQRSTUVWXYZ"
	"\1\2\3\4\5\6\0\0\0\0\0\0\0"
	"abcdefghijklm"
	"nopqrstuvwxyz"
	"0123456789_.-"
	"\1\2\3\4\5\6\0\0\0\0\0\0\0"
	",+=*!?#$%&:;@"
	"/\\()[]<>'`{|}"
	"~^\"_.,\0\0\0\0\0\0\0"
	"\1\2\3\4\5\6\0\0\0\0\0\0\0"
};


FlexMenuDisplay_U8G2::FlexMenuDisplay_U8G2()
{


}

void FlexMenuDisplay_U8G2::SetParams(FlexMenuDisplay_U8G2_Params * pParams)
{
	params = *pParams;
}

void FlexMenuDisplay_U8G2::InitResources_MenuIcons()
{

	icons[eFlexMenuIcon_None] = blank_12;
	icons[eFlexMenuIcon_Blank] = blank_12;
	icons[eFlexMenuIcon_LeftArrow] = leftarrow_12;
	icons[eFlexMenuIcon_RightArrow] = rightarrow_12;
	icons[eFlexMenuIcon_Circle] = circle_12;
	icons[eFlexMenuIcon_Selection] = selection_12;
	icons[eFlexMenuIcon_Checked] = checked_12;
	icons[eFlexMenuIcon_Unchecked] = unchecked_12;
	icons[eFlexMenuIcon_Execute] = execute_12;
	icons[eFlexMenuIcon_Cursor] = cursor_12;

	iIconCX = 8;
	iIconCY = 10;
	iIconY = 2;

	iFontShiftY = 11;

}

void FlexMenuDisplay_U8G2::InitResources_MenuFonts()
{
	pFont = (const uint8_t *) u8g2_font_crox1hb_tr;
	pFontSlider = (const uint8_t *) u8g2_font_crox1hb_tr;
	iFontHeightSlider = 0;
	iLineHeight = 14;

}

void FlexMenuDisplay_U8G2::InitResources_Edit(FlexMenuEditScreenParams & ep)
{


	ep.iMaxCharsX = 12;

	ep.osk_width = 13;
	ep.osk_height = 5;
	ep.osk_shift_y = 5;

	ep.iEditTextShiftX = 3;

	ep.osk_table = (uint8_t *) _osk_table;
	ep.osk_table_length = sizeof(_osk_table) - 1;
	ep.num_control_chars = 6;

	ep.osk_icons = osk_icons;

	ep.pFontEdit = (const uint8_t *) u8g2_font_courB12_tf;
	ep.iFontHeightEdit = 20;
	ep.iEditTextShiftY = 15;

	ep.initial_osk_selection = (ep.osk_width * 3) + 4;	//start on the X (cancel/exit) key


}





void FlexMenuDisplay_U8G2::Init()
{
	U8G2 & u = *params.pU8G2;

	(void)(u);


	memset(icons, 0, sizeof(icons));

	//the resources are in a separate class (FlexMenuDisplayOLED) which subclasses this class

	InitResources_MenuIcons();
	InitResources_MenuFonts();

	FlexMenuEditScreenParams ep;

	ep.iScreenCX = params.iScreenCX;
	ep.iScreenCY = params.iScreenCY;

	InitResources_Edit(ep);

	EditScreen_Init(&ep);

	iVisibleItems = params.iScreenCY / iLineHeight;


}



int FlexMenuDisplay_U8G2::GetVisibleItems()
{
	return iVisibleItems;
}

void FlexMenuDisplay_U8G2::DrawScreen(FlexMenuBase * pCurMenu)
{
	U8G2 & u = *params.pU8G2;

	if(params.pCallback)
	{
		if(params.pCallback->OverrideDrawScreen(pCurMenu))
		{
			return;
		}
	}


	u.clearBuffer();
	u.setFont(pFont);

	if(bDisplayMute)
	{
		return;
	}


	int iWidthDots = u.getStrWidth("..");


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


	float fLineHeight = (float)(params.iScreenCY) / GetVisibleItems();

	for(int i = 0; i < GetVisibleItems(); i++)
	{
		if(interimCallback)
		{
			interimCallback();
		}
		yield();

		if(i + pCurMenu->GetScrollPos() >= 0 && i + pCurMenu->GetScrollPos() < (int) pCurMenu->GetNumSubItems())
		{
			int iCurItem = i + pCurMenu->GetScrollPos();
			FlexMenuBase * pItem = pCurMenu->GetSubItem(iCurItem);

			String strLeft;
			String strRight;
			pItem->GetTitleText(strLeft);
			pItem->GetValueText(strRight);
			eFlexMenuIcon icon = pItem->UseIcon();

			if(interimCallback)
			{
				interimCallback();
			}

			int left = 0;
			int right = params.iScreenCX;

			if(icon < eFlexMenuIcon_Count && icon > eFlexMenuIcon_None && icons[icon])
			{
				if(icon == eFlexMenuIcon_LeftArrow)
				{
					left += iIconCX;
					if(icons[icon])
					{
						u.drawXBM(0, i * fLineHeight + iIconY, iIconCX, iIconCY, icons[icon]);
					}
				}
				else
				{
					if(icon != eFlexMenuIcon_None)
					{
						right -= iIconCX;
					}
					//right-=iIconCX;
					if(icons[icon])
					{
						u.drawXBM(right, i * fLineHeight + iIconY, iIconCX, iIconCY, icons[icon]);
					}

					if(icon == eFlexMenuIcon_Cursor)
					{
						right += iIconCX * 0.3;
					}

				}
			}

			if(interimCallback)
			{
				interimCallback();
			}

			u.setDrawColor(1);

			if(pItem->DisplayCentered() && (!strLeft.length() || !strRight.length()))
			{
				String strDisplay = strLeft.length() ? strLeft : strRight;

				u.drawStr(params.iScreenCX / 2 - u.getStrWidth(strDisplay.c_str()) / 2, i * fLineHeight + iFontShiftY, strDisplay.c_str());

				if(interimCallback)
				{
					interimCallback();
				}

			}
			else
			{

				u.drawStr(left, i * fLineHeight + iFontShiftY, strLeft.c_str());
				int widthLeft = u.getStrWidth(strLeft.c_str());

				int widthRight = u.getStrWidth(strRight.c_str());

				if(interimCallback)
				{
					interimCallback();
				}

				int icon_width = iIconCX;
				if(icon == eFlexMenuIcon_None)
				{
					icon_width = 0;
				}

				uint16_t desired_width = params.iScreenCX - (widthLeft + icon_width - 1);
				int chars = strRight.length();

				//csprintf("wr: %i  dw: %i\n",widthRight,desired_width);

				if(widthRight > desired_width)
				{
					chars = getCharsForWidth(pFont, strRight.c_str(), strRight.length(), desired_width);
				}

				//csprintf("chars=%i scx=%i widthLeft=%i widthDots=%i iconCX=%i\n",chars,params.iScreenCX,widthLeft,widthDots,iIconCX);

				if(interimCallback)
				{
					interimCallback();
				}

				if(chars != (int) strRight.length())
				{
					int chars = getCharsForWidth(pFont, strRight.c_str(), strRight.length(), params.iScreenCX - (widthLeft + icon_width + iWidthDots));
					String temp = strRight.substring(0, chars);
					temp += "..";
					int width = u.getStrWidth(temp.c_str());
					u.drawStr(right - width, i * fLineHeight + iFontShiftY, temp.c_str());
				}
				else
				{
					//csprintf("str '%s' right %i wr %i\n",strRight.c_str(),right,widthRight);
					u.drawStr(right - widthRight, i * fLineHeight + iFontShiftY, strRight.c_str());
				}

				if(interimCallback)
				{
					interimCallback();
				}

			}


		}

	}


	if(interimCallback)
	{
		interimCallback();
	}

	{

		yield();

		int iCurItem = pCurMenu->GetCurItem() - pCurMenu->GetScrollPos();

		if(iCurItem >= 0 && iCurItem < GetVisibleItems())
		{
			u.setDrawColor(2);

			u.drawBox(0, iCurItem * fLineHeight, params.iScreenCX, iLineHeight + 1);
		}
	}


}

const uint8_t * FlexMenuDisplay_U8G2::GetFont(eFlexMenuFont font)
{

	switch(font)
	{
	default:
		return pFont;
	case eFlexMenuFont_Large:
		return pFontSlider;
	}
}

int FlexMenuDisplay_U8G2::GetStringWidth(const String & strInput, eFlexMenuFont font)
{
	U8G2 & u = *params.pU8G2;
	u.setFont(GetFont(font));
	return u.getStrWidth(strInput.c_str());
}

int FlexMenuDisplay_U8G2::GetCharsForWidth(const String & strInput, int width_pixels, eFlexMenuFont font)
{
	return getCharsForWidth(GetFont(font), strInput.c_str(), strInput.length(), width_pixels);
}

int FlexMenuDisplay_U8G2::GetWidth()
{
	return params.iScreenCX;
}


void FlexMenuDisplay_U8G2::DrawSliderScreen(FlexMenuBase * pCurMenu)
{

	(void)(pCurMenu);
	U8G2 & u = *params.pU8G2;

	FlexMenuItemSlider * pSlider = (FlexMenuItemSlider *) pCurMenu;

	const uint8_t * pUseFont = pFont;
	int iFontHeight = iLineHeight;
	if(pFontSlider)
	{
		pUseFont = pFontSlider;
#if defined(ARDUINO_ARCH_ESP32)
		iFontHeight = pUseFont[1] - 1;
#else

		const void * ptr = &pUseFont[1];	//second byte of font is height
		uint8_t test = 0;
		pgm_read_with_offset(ptr, test);	//it's in flash so we need to call the helper function to read it
		iFontHeight = test - 1;
#endif
	}

	u.setFont(pUseFont);

	yield();
	if(interimCallback)
	{
		interimCallback();
	}

	String strTitle;
	pCurMenu->GetTitleText(strTitle);

	String strValue;
	pCurMenu->GetValueText(strValue);

	u.setColorIndex(1);

	int iSliderTextEdgeDistance = 20;

	u.drawStr(params.iScreenCX / 2 - u.getStrWidth(strTitle.c_str()) / 2, 0 + iFontShiftY + iSliderTextEdgeDistance, strTitle.c_str());

	//display.setTextAlignment(TEXT_ALIGN_CENTER);
	//display.drawString(params.iScreenCX/2, 0, strTitle);

	yield();
	if(interimCallback)
	{
		interimCallback();
	}

	//display.drawString(params.iScreenCX/2, params.iScreenCY-iFontHeight, strValue);

	u.drawStr(params.iScreenCX / 2 - u.getStrWidth(strValue.c_str()) / 2, params.iScreenCY - iFontHeight - iFontShiftY - iSliderTextEdgeDistance, strValue.c_str());


	int bar_height = (params.iScreenCY * 20) / 100;

	int bar_y = (params.iScreenCY - bar_height) / 2;


	yield();
	if(interimCallback)
	{
		interimCallback();
	}

	//draw frame around the progress bar
	//display.drawRect(0,bar_y,params.iScreenCX,bar_height);
	u.drawFrame(0, bar_y, params.iScreenCX, bar_height);

	yield();
	if(interimCallback)
	{
		interimCallback();
	}

	//fill in the progress bar
	int bar_pixels = pSlider->GetProgressBar(params.iScreenCX - 4);
	//display.fillRect(2, bar_y+2, bar_pixels, bar_height-4);
	u.drawBox(2, bar_y + 2, bar_pixels, bar_height - 4);

}



uint16_t FlexMenuDisplay_U8G2::getCharsForWidth(const uint8_t * pFont, const char * text, uint16_t length, uint16_t desiredWidth)
{

	U8G2 & u = *params.pU8G2;

	uint16_t numChars = 0;

	uint16_t stringWidth = 0;

	for(int i = 0; i < length; i++)
	{
		char tempstr[2] = {text[i], 0};
		stringWidth += u.getStrWidth(tempstr);
		if(stringWidth > desiredWidth)
		{
			break;
		}
		numChars++;
	}

	return numChars;
}


bool FlexMenuDisplay_U8G2::OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)
{

	if(EditScreen_OnNavigate(pCurMenu, direction, accel))
	{
		return true;
	}

	return false;
}

void FlexMenuDisplay_U8G2::OnEditMode(FlexMenuBase * pCurMenu, bool bEnable)
{
	EditScreen_OnEditMode(pCurMenu, bEnable);
}

bool FlexMenuDisplay_U8G2::DisplayNeedsRefresh(FlexMenuBase * pCurMenu)
{
	if(EditScreen_NeedsRefresh(pCurMenu))
	{
		return true;
	}

	return false;
}

void FlexMenuDisplay_U8G2::ESCB_DrawEditBox(const FlexMenuEditScreenParams & editparams, const String & strDisplayText, const String & strBeforeCursor, bool bDrawCursor)
{

	U8G2 & u=*params.pU8G2;

	int edit_height=editparams.iFontHeightEdit+2;

	u.setFont(editparams.pFontEdit);
	u.setDrawColor(1);
	u.drawStr(editparams.iEditTextShiftX, 1 + editparams.iEditTextShiftY, strDisplayText.c_str());

	//csprintf("displaytext: %s\n",strDisplayText.c_str());

	int xpos=u.getStrWidth(strBeforeCursor.c_str());

	u.drawFrame(0,0,editparams.iScreenCX,edit_height);

	u.setDrawColor(2);
	int cursor_xpos=xpos+editparams.iEditTextShiftX;
	if(cursor_xpos<1) cursor_xpos=1;
	if(bDrawCursor) u.drawVLine(cursor_xpos, 1, editparams.iFontHeightEdit+1);


}


void FlexMenuDisplay_U8G2::ESCB_DrawOSK_Key(const FlexMenuEditScreenParams & editparams, uint16_t x, uint16_t y, uint16_t cx, uint16_t cy, eEditOskDrawKey mode, const osk_icondef * pIcon, const String * pText)
{

	U8G2 & u=*params.pU8G2;
	u.setDrawColor(1);

	yield();

	if(interimCallback) interimCallback();

	if(pIcon && pIcon->data)
	{

		u.drawXBM(x+((cx-pIcon->cx)>>1)+pIcon->shift_x, y+((cy-pIcon->cy)>>1)+pIcon->shift_y, pIcon->cx, pIcon->cy, pIcon->data);
	}

	if(pText)
	{
		//display.setTextAlignment(TEXT_ALIGN_CENTER);
		u.drawStr(x+(cx>>1) - (u.getStrWidth(pText->c_str()) >> 1),y + editparams.iEditTextShiftY ,pText->c_str());
	}

	u.setDrawColor(2);

	switch(mode)
	{
	case eEditOskDrawKey_Selected:
		u.drawBox(x,y,cx,cy);
		break;
	case eEditOskDrawKey_LockedIn:
		u.drawFrame(x,y,cx,cy);
		break;
	default:
		break;
	}

}


void FlexMenuDisplay_U8G2::DrawMessage(FlexMenuBase * pCurMenu)
{

	U8G2 & u = *params.pU8G2;


	const uint8_t * pUseFont = GetFont(pCurMenu->GetFont());

	u.setFont(pUseFont);

	u.setDrawColor(1);


	String strTemp1;
	String strTemp2;

	pCurMenu->GetTitleText(strTemp1);
	pCurMenu->GetValueText(strTemp2);

	if(strTemp1.length() && !strTemp2.length())
	{
		strTemp2 = strTemp1;
		strTemp1 = "";
	}


	int length1 = u.getStrWidth(strTemp1.c_str());
	int length2 = u.getStrWidth(strTemp2.c_str());

	//csprintf("screenCX is %i\n",params.iScreenCX);

	int line2_y = 0;

	yield();

	if(length1 && length2)
	{
		//u.setTextAlignment(TEXT_ALIGN_CENTER);
		u.drawStr(params.iScreenCX / 2  - u.getStrWidth(strTemp1.c_str()) / 2, 0 + iFontShiftY, strTemp1.c_str());

		line2_y = iLineHeight + ((params.iScreenCY - iLineHeight) >> 1);
	}
	else
	{
		line2_y = params.iScreenCY >> 1;
	}

	//csprintf("l1=%i  l2=%i  y=%i\n",length1,length2,line2_y);

	yield();


	drawStrWordWrap(&u, 64, 32, 128, 8, strTemp2.c_str(), 1);

	//display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);

/*
	if(strTemp2.indexOf('\n') >= 0)
	{
		printwords(strTemp2.c_str(), 0, 32);
		//u.drawStr(params.iScreenCX / 2 - u.getStrWidth(strTemp2.c_str()) / 2, line2_y + iFontShiftY, strTemp2.c_str());
	}
	else
	{
		//display.setTextAlignment(TEXT_ALIGN_CENTER);
		u.drawStr(params.iScreenCX / 2 - u.getStrWidth(strTemp2.c_str()) / 2, line2_y + iFontShiftY, strTemp2.c_str());
		//u.drawStr(params.iScreenCX / 2 - u.getStrWidth(strTemp2.c_str()) / 2, iLineHeight+(iLineHeight>>1) + iFontShiftY, params.iScreenCX-1, strTemp2.c_str());
	}
*/

}

bool FlexMenuDisplay_U8G2::HistoryBuffer(FlexMenuBase * pCurMenu, uintptr_t * data)
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


void FlexMenuDisplay_U8G2::Output()
{
	params.pU8G2->sendBuffer();
}


void FlexMenuDisplay_U8G2::SetBacklight(uint8_t brightness, bool bMute)
{
	/*
	U8G2 & u=*params.pU8G2;
	if(bDisplayMute!=bMute)
	{
		if(bMute)
		{
			display.displayOff();
		}
		else
		{
			display.displayOn();
		}
	}
	bDisplayMute=bMute;

	display.setBrightness(brightness);
	*/
}




void drawStrWordWrap(U8G2 * pU8G2, int16_t x, int16_t y, uint16_t xmax_pixels, uint16_t max_lines, const char * text, int align)
{
	//align: 0 = left, 1 = center, 2 = right

	uint16_t uLineHeight = pU8G2->getMaxCharHeight();

	char glyph[2];
	glyph[1] = 0;

	uint16_t len = (uint16_t) strlen(text);
	uint16_t uCharOffset = 0;
	uint16_t uCurLine = 0;
	uint16_t uAckumulatedWidth = 0;

	uint16_t uSplitPos = 0;
	uint16_t uCurWidth = 0;

	for(uint16_t i = 0; i < len; i++)
	{
		if(uCurLine>=max_lines) return;

		glyph[0]=text[i];
		uAckumulatedWidth += pU8G2->getStrWidth(glyph) - 1;	//it seems getStrWidth adds an extra pixel for each call

		// try to break on a space, dash, or newline character
		switch(text[i])
		{
		case '\n':
		case ' ':
		case '-':
			uSplitPos = i;
			uCurWidth = uAckumulatedWidth;
			break;
		default:
			break;
		}

		if(uAckumulatedWidth >= xmax_pixels || text[i]=='\n')
		{
			if(uSplitPos == 0)
			{
				uSplitPos = i;
				uCurWidth = uAckumulatedWidth;
			}

			const char * usetext=&text[uCharOffset];
			uint16_t textLength=uSplitPos - uCharOffset;

			char nextchar[2]={ usetext[textLength], 0 };

			char * doprint=(char *) malloc(textLength+1);
			strncpy(doprint,usetext,textLength+1);
			doprint[textLength]=0;
			int use_x=x;
			if(align==1) use_x -= pU8G2->getStrWidth(doprint) >> 1;
			if(align==2) use_x -= pU8G2->getStrWidth(doprint);
			pU8G2->drawStr(use_x,y + (uCurLine++) * uLineHeight,doprint);
			free(doprint);

			uCharOffset = uSplitPos + 1;	//skip the next space character

			switch( nextchar[0] )
			{
			case ' ':
			case '\n':
				break;
			default:	//but if it wasn't a space then we do need to draw it next time
				uCharOffset -= 1;
				uCurWidth -= pU8G2->getStrWidth(nextchar) - 1;	//we didn't draw it. still need to account for that extra pixel though.
				break;
			}

			// subtract the actual width of what we just drew
			uAckumulatedWidth -= uCurWidth;
			uSplitPos = 0;

		}
	}

	// draw any remaining characters
	if(uCharOffset < len)
	{
		const char * usetext=&text[uCharOffset];
		uint16_t textLength=len - uCharOffset;

		char * doprint=(char *) malloc(textLength+1);
		strncpy(doprint,usetext,textLength+1);
		doprint[textLength]=0;
		int use_x = x;
		if(align==1) use_x -= pU8G2->getStrWidth(doprint) >> 1;
		if(align==2) use_x -= pU8G2->getStrWidth(doprint);

		pU8G2->drawStr(use_x,y + uCurLine * uLineHeight, doprint);
		free(doprint);
	}
}



#endif
