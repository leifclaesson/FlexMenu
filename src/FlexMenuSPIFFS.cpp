/*
 * FlexMenuSPIFFS.cpp
 *
 *  Created on: 30 Jun 2020
 *      Author: user
 */

#include <FlexMenuSPIFFS.h>
#include "SPIFFS.h"
#include "FlexMenu.h"
#include <map>

bool FlexMenuSPIFFS_Init(FlexMenuManager & flexmenu)
{

	if(!SPIFFS.begin())
	{
		csprintf("SPIFFS Mount Failed. Attempting to format.\n");
		flexmenu.ShowMessage("SPIFFS", "Formatting file system, please wait.",0);
		flexmenu.Loop(false);
		flexmenu.Output();

		if(SPIFFS.format())
		{
			csprintf("format successful\n");
			flexmenu.ShowMessage("SPIFFS", "Format Successful.",1000);
			flexmenu.Loop(false);
			flexmenu.Output();
			delay(3000);

			if(SPIFFS.begin())
			{
				csprintf("file system opened\n");
				flexmenu.ShowMessage("SPIFFS", "File system opened.",1000);
				flexmenu.Loop(false);
				flexmenu.Output();
				delay(3000);


				File config_file=SPIFFS.open( "/config.sys", FILE_WRITE);

				if(config_file)
				{
					config_file.println("placeholder=0");
					flexmenu.ShowMessage("SPIFFS", "Placeholder Written",1000);
					flexmenu.Loop(false);
					flexmenu.Output();
					delay(3000);
				}

			}
			else
			{
				csprintf("unable to open file system\n");
				flexmenu.ShowPermanentErrorMessage("SPIFFS", "Unable to open file system");
				flexmenu.Loop(false);
				flexmenu.Output();
			}

		}
		else
		{
			csprintf("error, format failed\n");
			flexmenu.ShowPermanentErrorMessage("SPIFFS", "ERROR: FORMAT FAILED");
			flexmenu.Loop(false);
			flexmenu.Output();
		}

		flexmenu.Loop(false);

		return false;

	}
	else
	{
		csprintf("mounted existing file system on the first try\n");

		flexmenu.Loop(false);

		return true;

	}



}


void FlexMenuSPIFFS_DoLoad(FlexMenuManager & flexmenu)
{
	File config_file=SPIFFS.open( "/config.sys", FILE_READ);

	typedef std::map<String, String> _mapConfig;

	_mapConfig mapConfig;

	if(config_file)
	{

		int iParametersRead=0;
		int iParametersUsed=0;

		String strText;
		int i=0;
		do
		{
			strText=config_file.readStringUntil('\n');
			int equ=strText.indexOf('=');

			if(equ>0)
			{
				String strValue=strText.substring(equ+1);
				int cr=strValue.indexOf('\r');
				mapConfig[strText.substring(0, equ)]=strValue.substring(0,cr);

				iParametersRead++;
				//Serial.printf("%i %i %i %s '%s'\n",i,equ,cr,strText.substring(0, equ).c_str(),mapConfig[strText.substring(0, equ)].c_str());
			}
			i++;
		} while(strText.length());




		flexmenu.IterateItems([&mapConfig,&iParametersUsed] (FlexMenuBase * pItem, FlexMenuManager * pManager)->bool
		{
			(void)(pManager);
			if(pItem->IsSaveable())
			{
				String strTitle;
				pItem->GetTitleText(strTitle);

				if(strTitle.length())
				{

					_mapConfig::const_iterator pos=mapConfig.find(strTitle);

					if(pos!=mapConfig.end())
					{
						pItem->LoadString(pos->second);
						iParametersUsed++;
						//Serial.printf("Found %s = '%s'\n",strTitle.c_str(),pos->second.c_str());
					}

				}
			}
			return true;
		}, 0);

		csprintf("Config file found. %i parameters read, %i used.\n",iParametersRead,iParametersUsed);

		String strTemp;
		strTemp+=iParametersRead; strTemp+=" params read\n";
		strTemp+=iParametersUsed; strTemp+=" params used";

		flexmenu.ShowMessage("Gate Controller", strTemp, 1000);

	}
	else
	{
//		csprintf("No config file found.\n");

		flexmenu.ShowMessage("Gate Controller", "No config file found.", 1000);

	}

}

void FlexMenuSPIFFS_DoSave(FlexMenuManager & flexmenu)
{

	File config_file=SPIFFS.open( "/config.sys", FILE_WRITE);

	if(config_file)
	{
		config_file.printf("Settings\n");

		int iItems=0;

		flexmenu.IterateItems([&iItems,&config_file] (FlexMenuBase * pItem, FlexMenuManager * pManager)->bool
		{
			if(pItem->IsSaveable())
			{
				String strTitle;
				pItem->GetTitleText(strTitle);

				if(strTitle.length())
				{
					String strSave;
					pItem->GetSaveString(strSave);


					config_file.print(strTitle);
					config_file.print('=');
					config_file.println(strSave);

					iItems++;

					//csprintf("Save %s: %s!\n",strTitle.c_str(),strSave.c_str());
				}
			}
			return true;
		}, 0);
		char temp[64];

		sprintf(temp,"Saved %i items",iItems);

		flexmenu.ShowMessage("Save Settings", temp, 2000);

	}
	else
	{
		flexmenu.ShowMessage("ERROR", "Unable to open file.", 2000);
	}

}

