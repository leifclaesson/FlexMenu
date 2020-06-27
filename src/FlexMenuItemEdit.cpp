// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemEdit.h"
/*
 * FlexMenuItemEdit.cpp
 *
 *  Created on: 24 Jun 2020
 *      Author: user
 */




FlexMenuItemEdit::FlexMenuItemEdit() {
	// TODO Auto-generated constructor stub
	edit_internal.pParent=this;
}

FlexMenuItemEdit::~FlexMenuItemEdit() {
	// TODO Auto-generated destructor stub
}

void FlexMenuItemEdit::SetManager(FlexMenuManager * pManager)
{
	this->pManager=pManager;
}

void FlexMenuItemEdit::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}

void FlexMenuItemEdit::GetValueText(String & strValueDestination)
{
	strValueDestination=strEdit;
}

void FlexMenuItemEdit::OnEnter()
{
	edit_internal.Entering();
}



void FlexMenuItemEditInternal::Entering()
{
//	Serial.printf("FlexMenuItemEdit::OnEnter()\n");
}

void FlexMenuItemEditInternal::GetTitleText(String & strTitleDestination)
{
	strTitleDestination="edit_internal";

}

bool FlexMenuItemEditInternal::CanNavigate(eFlexMenuNav direction, uint8_t accel)
{
	(void)(accel);
	switch(direction)
	{
	default:
		break;
	case eFlexMenuNav_Prev:
	case eFlexMenuNav_Next:
		//Serial.printf("nav... pManager=%p\n",pParent->pManager);
		return false;
	}
	return true;
}


