// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemEdit.h"
#include "FlexMenuGlobalItems.h"
/*
 * FlexMenuItemEdit.cpp
 *
 *  Created on: 24 Jun 2020
 *      Author: user
 */




FlexMenuItemEdit::FlexMenuItemEdit() {
	// TODO Auto-generated constructor stub
	SetSaveable(true);
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
	if(IsPassword())
	{
		strValueDestination="";
		strValueDestination.reserve(strEdit.length());
		for(int i=0;i<(int) strEdit.length();i++)
		{
			strValueDestination+='*';
		}
	}
	else
	{
		strValueDestination=strEdit;
	}
}

void FlexMenuItemEdit::OnEnter()
{
	Serial.printf("FlexMenuItemEdit::OnEnter()\n");

	FlexMenuTempItem & item=*((FlexMenuTempItem *)GetTempItem());

	item.Reset();
	item.pParent=this;
}

