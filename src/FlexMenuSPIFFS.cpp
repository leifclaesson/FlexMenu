/*
 * FlexMenuSPIFFS.cpp
 *
 *  Created on: 30 Jun 2020
 *      Author: user
 */

#include <FlexMenuSPIFFS.h>

#include "FlexMenu.h"
#include <list>


fn_FlexMenuSPIFFSInterimCallback fmsInterimCallback;


static void DoInterimCallback()
{
	if(fmsInterimCallback) fmsInterimCallback();
}

FS & FlexMenuGetFileSystem()
{
	return FLEXMENU_FILESYSTEM;
}

const char * szFlexMenuConfigFilename="/config.txt";

void FlexMenuSetConfigFileName(const char * szFileName)
{
	szFlexMenuConfigFilename=szFileName;
}

const char * FlexMenuGetConfigFileName()
{
	return szFlexMenuConfigFilename;
}



bool FlexMenuSPIFFS_Init(FlexMenuManager & flexmenu)
{

	if(!FLEXMENU_FILESYSTEM.begin())
	{
		csprintf("SPIFFS Mount Failed. Attempting to format.\n");
		flexmenu.ShowMessage(flexmenu.strTitle, "Formatting file system, please wait.",eFlexMenuFont_Large,0);

		flexmenu.Loop(false);
		flexmenu.Output();

		if(FLEXMENU_FILESYSTEM.format())
		{
			csprintf("format successful\n");
			flexmenu.ShowMessage(flexmenu.strTitle, "Format Successful.",eFlexMenuFont_Large,1000);
			flexmenu.Loop(false);
			flexmenu.Output();
			delay(3000);

			if(FLEXMENU_FILESYSTEM.begin())
			{
				csprintf("file system opened\n");
				flexmenu.ShowMessage(flexmenu.strTitle, "File system opened.",eFlexMenuFont_Large,1000);
				flexmenu.Loop(false);
				flexmenu.Output();
				delay(3000);


				File config_file=FLEXMENU_FILESYSTEM.open( FlexMenuGetConfigFileName(), "w");

				if(config_file)
				{
					config_file.println("placeholder=0");
					flexmenu.ShowMessage(flexmenu.strTitle, "Placeholder Written",eFlexMenuFont_Large,1000);
					flexmenu.Loop(false);
					flexmenu.Output();
					delay(3000);
				}

			}
			else
			{
				csprintf("unable to open file system\n");
				flexmenu.ShowPermanentErrorMessage(flexmenu.strTitle, "Unable to open file system", eFlexMenuFont_Large);
				flexmenu.Loop(false);
				flexmenu.Output();
			}

		}
		else
		{
			csprintf("error, format failed\n");
			flexmenu.ShowPermanentErrorMessage(flexmenu.strTitle, "ERROR: FORMAT FAILED", eFlexMenuFont_Large);
			flexmenu.Loop(false);
			flexmenu.Output();
		}

		flexmenu.Loop(false);
		flexmenu.Output();

		return false;

	}
	else
	{
		csprintf("mounted existing file system on the first try\n");

		flexmenu.Loop(false);
		flexmenu.Output();

		return true;

	}

}

void FlexMenuSPIFFS_DoLoad(FlexMenuManager & flexmenu)
{
	_mapConfig mapConfig;
	if(FlexMenuSPIFFS_DoRead(mapConfig))
	{
		FlexMenuSPIFFS_DoApply(flexmenu,mapConfig);
	}

	flexmenu.DoOnSettingsLoadedCallback();

}

bool FlexMenuSPIFFS_DoRead(_mapConfig & mapConfig)
{
	csprintf("Opening %s\n",FlexMenuGetConfigFileName());

	File config_file=FLEXMENU_FILESYSTEM.open( FlexMenuGetConfigFileName(), "r");


	int iParametersRead=0;

	if(config_file)
	{


		csprintf("File opened.\n");

		String strFile;


		int size=config_file.size();

		if(size>8192)
		{
			csprintf("File too big. Reset to default instead.\n");
			return false;
		}


		{
		    String & ret=strFile;
		    ret.reserve(config_file.size() - config_file.position());
		    char temp[256+1];
		    int countRead = config_file.readBytes(temp, sizeof(temp)-1);
		    while (countRead > 0)
		    {
		        temp[countRead] = 0;
		        ret += temp;
		        countRead = config_file.readBytes(temp, sizeof(temp)-1);
		    }
		}


//		config_file.read((uint8_t *) strFile.begin(), size);

//		strFile=config_file.readString();
		csprintf("Read string. %i chars\n",strFile.length());




		String strText;
		int i=0;
		int read_ofs=0;
		do
		{
//			strText=config_file.readStringUntil('\n');

			int newline=strFile.indexOf('\n',read_ofs);
			if(newline<0) newline=strFile.length();
			//csprintf("newline at %i\n",newline);
			strText=strFile.substring(read_ofs, newline);
			//csprintf("text is %s\n",strText.c_str());
			read_ofs=newline+1;



			int equ=strText.indexOf('=');

			if(equ>0)
			{
				String strValue=strText.substring(equ+1);
				int cr=strValue.indexOf('\r');
				if(mapConfig[strText.substring(0, equ)]!="")
				{
					csprintf("CONFIG KEY DUPE! '%s' already exists\n",strText.substring(0,equ).c_str());
				}
				mapConfig[strText.substring(0, equ)]=strValue.substring(0,cr);

				iParametersRead++;
				//Serial.printf("%i %i %i %s '%s'\n",i,equ,cr,strText.substring(0, equ).c_str(),mapConfig[strText.substring(0, equ)].c_str());
			}
			i++;

			yield();

		} while(strText.length());

		csprintf("Config file found. %i parameters read.\n",iParametersRead);

		return true;
	}
	else
	{
		csprintf("No config file found.\n");
	}

	return false;

}

void FlexMenuSPIFFS_DoApply(FlexMenuManager & flexmenu, const _mapConfig & mapConfig)
{

	int iParametersUsed=0;

	if(mapConfig.size())
	{

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
						csprintf("Found %s = '%s'\n",strIdentifier.c_str(),pos->second.c_str());
						pItem->LoadString(pos->second);
						iParametersUsed++;

					}

				}
			}
			return true;
		}, 0);

		csprintf("%i parameters used.\n",iParametersUsed);

		String strTemp;
		strTemp+=mapConfig.size(); strTemp+=" params read\n";
		strTemp+=iParametersUsed; strTemp+=" params used";

		flexmenu.ShowMessage(flexmenu.strTitle, strTemp, eFlexMenuFont_Large, 1000);


	}
	else
	{

		flexmenu.ShowMessage(flexmenu.strTitle, "No config file found.", eFlexMenuFont_Large, 1000);

	}

	flexmenu.DoOnSettingsLoadedCallback();

}




void FlexMenuSPIFFS_DoSave(FlexMenuManager & flexmenu)
{

	std::list<String> file_buffer;

	int iItems=0;

	flexmenu.IterateItems([&iItems,&file_buffer] (FlexMenuBase * pItem, FlexMenuManager * pManager)->bool
	{

		(void)(pManager);

		DoInterimCallback();

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

	DoInterimCallback();
	flexmenu.ShowMessage("Save Settings", temp, eFlexMenuFont_Large, 0);
	DoInterimCallback();
	flexmenu.Loop(false);
	DoInterimCallback();
	flexmenu.Output();
	DoInterimCallback();



	//csprintf("Save %s: %s!\n",strIdentifier.c_str(),strSave.c_str());

	//delay(50);

	File config_file=FLEXMENU_FILESYSTEM.open( FlexMenuGetConfigFileName(), "w");
	DoInterimCallback();

	if(config_file)
	{
		csprintf("opened file %s\n",FlexMenuGetConfigFileName());
		//delay(50);

		for(int i=0;i<iItems;i++)
		{
			DoInterimCallback();
			config_file.println(*file_buffer.begin());
			//csprintf("printed line %i\n",i);
			//delay(50);
			file_buffer.pop_front();
		}


		csprintf("Saved %i items\n",iItems);
		sprintf(temp,"Saved %i items",iItems);
		flexmenu.ShowMessage("Save Settings", temp, eFlexMenuFont_Large, 2000);

	}
	else
	{
		flexmenu.ShowMessage("ERROR", "Unable to open file.", eFlexMenuFont_Large, 2000);
	}

}

