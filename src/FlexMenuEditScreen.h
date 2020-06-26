#include "FlexMenuBase.h"
class FlexMenuItemEdit;

struct osk_icondef
{
	uint8_t * data;
	uint8_t cx;
	uint8_t cy;
	int8_t shift_x;
	int8_t shift_y;
};

class OLEDDisplay;

class FlexMenuEditScreenParams
{
public:
	int iScreenCX;
	int iScreenCY;
	const uint8_t * pFontEdit;

	int osk_width;
	int osk_height;

	int initial_osk_selection=0;

	osk_icondef * osk_icons;

	uint8_t * osk_table;
	int osk_table_length;
	int num_control_chars;

	int iMaxCharsX=0;
	int iEditTextShiftX=0;

	OLEDDisplay * pOLEDDisplay;

};

class OLEDDisplay;

class FlexMenuEditScreen
{
public:
	FlexMenuEditScreen();
	virtual ~FlexMenuEditScreen();

	void EditScreen_Init(FlexMenuEditScreenParams * pParams);



	virtual void EditScreen_Draw(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem);

	virtual bool EditScreen_OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel);

	void EditScreen_OnEditMode(FlexMenuBase * pCurMenu, bool bEnable);
	bool EditScreen_NeedsRefresh(FlexMenuBase * pCurMenu);


private:

	FlexMenuEditScreenParams params;

	int selection=0;

	int scroll_y=0;
	int last_scroll_y=0;

	int iNudge=0;
	int iLastDirection=0;

	bool bCursorMode=false;

	int iCursor=0;

	String strEdit;


	uint32_t cursor_millis=0;

	bool bDrawCursor=false;
	bool bLastDrawCursor=false;

	int iScrollX=0;

	void CursorNav(int iDirection);

	void DoScrollKeyboard(int iDirection);

	uint8_t ReadOskTable(int selection);

	void HandleOskInsert(char osk_cur);

	void HandleOskErase(char osk_cur);

	bool EditHandlePush(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel);



};
