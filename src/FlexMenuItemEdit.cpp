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




FlexMenuItemEditBase::FlexMenuItemEditBase() {
	// TODO Auto-generated constructor stub
	SetSaveable(true);
}

FlexMenuItemEditBase::~FlexMenuItemEditBase() {
	// TODO Auto-generated destructor stub
}

void FlexMenuItemEditBase::SetManager(FlexMenuManager * pManager)
{
	this->pManager=pManager;
}

void FlexMenuItemEditBase::GetValueText(String & strValueDestination)
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

void FlexMenuItemEditBase::OnEnter()
{
	Serial.printf("FlexMenuItemEditBase::OnEnter()\n");

	FlexMenuTempItem & item=*((FlexMenuTempItem *)GetTempItem());

	item.Reset();
	item.pParent=this;
}

