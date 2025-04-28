/*
 * FlexMenuDisplayOLED.h
 *
 *  Created on: 26 Jun 2020
 *      Author: user
 */


#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYOLED_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYOLED_H_

#include "FlexMenuDisplayOLED_Base.h"

#ifdef FLEXMENU_OLED

class FlexMenuDisplay_OLED : public FlexMenuDisplay_OLED_Base
{
public:
	FlexMenuDisplay_OLED();
	virtual ~FlexMenuDisplay_OLED();

	virtual void InitResources_MenuIcons();
	virtual void InitResources_MenuFonts();
	virtual void InitResources_Edit(FlexMenuEditScreenParams & ep);

};

#endif

#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYOLED_H_ */
