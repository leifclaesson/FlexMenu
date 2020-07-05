/*
 * FlexMenuSPIFFS.cpp
 *
 *  Created on: 30 Jun 2020
 *      Author: user
 */

#include <FlexMenuSPIFFS.h>

#include "FlexMenu.h"
#include <map>
#include <list>

FS & FlexMenuGetFileSystem()
{
	return FLEXMENU_FILESYSTEM;
}

const char * FlexMenuGetConfigFileName()
{
	return "/config.txt";
}


bool FlexMenuSPIFFS_Init(FlexMenuManager & flexmenu)
{

	if(!FLEXMENU_FILESYSTEM.begin())
	{
		csprintf("SPIFFS Mount Failed. Attempting to format.\n");
		flexmenu.ShowMessage("SPIFFS", "Formatting file system, please wait.",eFlexMenuFont_Large,0);
		flexmenu.Loop(false);
		flexmenu.Output();

		if(FLEXMENU_FILESYSTEM.format())
		{
			csprintf("format successful\n");
			flexmenu.ShowMessage("SPIFFS", "Format Successful.",eFlexMenuFont_Large,1000);
			flexmenu.Loop(false);
			flexmenu.Output();
			delay(3000);

			if(FLEXMENU_FILESYSTEM.begin())
			{
				csprintf("file system opened\n");
				flexmenu.ShowMessage("SPIFFS", "File system opened.",eFlexMenuFont_Large,1000);
				flexmenu.Loop(false);
				flexmenu.Output();
				delay(3000);


				File config_file=FLEXMENU_FILESYSTEM.open( FlexMenuGetConfigFileName(), "w");

				if(config_file)
				{
					config_file.println("placeholder=0");
					flexmenu.ShowMessage("SPIFFS", "Placeholder Written",eFlexMenuFont_Large,1000);
					flexmenu.Loop(false);
					flexmenu.Output();
					delay(3000);
				}

			}
			else
			{
				csprintf("unable to open file system\n");
				flexmenu.ShowPermanentErrorMessage("SPIFFS", "Unable to open file system", eFlexMenuFont_Large);
				flexmenu.Loop(false);
				flexmenu.Output();
			}

		}
		else
		{
			csprintf("error, format failed\n");
			flexmenu.ShowPermanentErrorMessage("SPIFFS", "ERROR: FORMAT FAILED", eFlexMenuFont_Large);
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
	File config_file=FLEXMENU_FILESYSTEM.open( FlexMenuGetConfigFileName(), "r");

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
				String strIdentifier;
				pItem->GetIdentifier(strIdentifier);

				if(strIdentifier.length())
				{

					_mapConfig::const_iterator pos=mapConfig.find(strIdentifier);

					if(pos!=mapConfig.end())
					{
						pItem->LoadString(pos->second);
						iParametersUsed++;
						//Serial.printf("Found %s = '%s'\n",strIdentifier.c_str(),pos->second.c_str());
					}

				}
			}
			return true;
		}, 0);

		csprintf("Config file found. %i parameters read, %i used.\n",iParametersRead,iParametersUsed);

		String strTemp;
		strTemp+=iParametersRead; strTemp+=" params read\n";
		strTemp+=iParametersUsed; strTemp+=" params used";

		flexmenu.ShowMessage("Gate Controller", strTemp, eFlexMenuFont_Large, 1000);

	}
	else
	{
		csprintf("No config file found.\n");

		flexmenu.ShowMessage("Gate Controller", "No config file found.", eFlexMenuFont_Large, 1000);

	}

}

void FlexMenuSPIFFS_DoSave(FlexMenuManager & flexmenu)
{

	std::list<String> file_buffer;

	int iItems=0;

	flexmenu.IterateItems([&iItems,&file_buffer] (FlexMenuBase * pItem, FlexMenuManager * pManager)->bool
	{

		(void)(pManager);
		if(pItem)
		{
			if(pItem->IsSaveable())
			{

				String strTemp;
				pItem->GetIdentifier(strTemp);

//				csprintf("iterate identifier %s\n",strTemp.c_str());

				if(strTemp.length())
				{
					String strSave;
					pItem->GetSaveString(strSave);

					strTemp+='=';
					strTemp+=strSave;

					file_buffer.push_back(strTemp);

					iItems++;
				}
			}
		}
		else
		{
			csprintf("Iterate got NULL POINTER\n");
		}


		return true;
	}, 0);



	csprintf("Got %i items to save\n",iItems);

	char temp[64];
	sprintf(temp,"Saving %i items",iItems);

	flexmenu.ShowMessage("Save Settings", temp, eFlexMenuFont_Large, 0);
	flexmenu.Loop(false);
	flexmenu.Output();



	//csprintf("Save %s: %s!\n",strIdentifier.c_str(),strSave.c_str());

	//delay(50);

	File config_file=FLEXMENU_FILESYSTEM.open( FlexMenuGetConfigFileName(), "w");

	if(config_file)
	{
		csprintf("opened file %s\n",FlexMenuGetConfigFileName());
		//delay(50);

		for(int i=0;i<iItems;i++)
		{
			config_file.println(*file_buffer.begin());
			csprintf("printed line %i\n",i);
			//delay(50);
			file_buffer.pop_front();
		}


		sprintf(temp,"Saved %i items",iItems);
		flexmenu.ShowMessage("Save Settings", temp, eFlexMenuFont_Large, 2000);

	}
	else
	{
		flexmenu.ShowMessage("ERROR", "Unable to open file.", eFlexMenuFont_Large, 2000);
	}

}

