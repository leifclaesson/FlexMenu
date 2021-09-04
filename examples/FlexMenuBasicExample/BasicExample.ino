#include "Arduino.h"
#include <Rotary.h>				// https://github.com/leifclaesson/Rotary
#include <SH1106.h>				// https://github.com/leifclaesson/esp8266-oled-ssd1306
#include <SSD1306.h>			// https://github.com/leifclaesson/esp8266-oled-ssd1306
#include <FlexMenuDisplayOLED.h>
#include <FlexMenu.h>
#include <FlexMenuSPIFFS.h>

//"Basic" flexmenu example.
//Needed hardware: SH1106 or SSD1306 display
//Rotary encoder

#define ROTARY_PIN_I 12
#define ROTARY_PIN_Q 14
#define ROTARY_PIN_BTN 13


//The FlexMenuDisplay object is separate from the menu logic (FlexMenuManager) to facilitate support for other display types in the future.
FlexMenuDisplay_OLED flexmenudisplay;
FlexMenuManager flexmenu;

SH1106Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h
//SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h

Rotary rotary;


static volatile int iRotaryCounter=0;

void IRAM_ATTR RotaryISR()
{

	volatile uint16_t g_gpi=(digitalRead(ROTARY_PIN_I)*2) + (digitalRead(ROTARY_PIN_Q)*1);

	uint8_t rot=rotary.process(g_gpi & 3);

	//No matter what speed the loop runs at, the interrupt service route won't miss any rotations, and will not fall behind.
	//This is critical for the usability and navigation "feel".
	//This particular rotary library is inherently debounced.

	switch(rot)
	{
	case DIR_CW:
		iRotaryCounter++;
		break;
	case DIR_CCW:
		iRotaryCounter--;
		break;
	default:
		break;
	}

}


// The SH1106/SSD1306 does some kind of character translation that *breaks* ümläut chäracters so we bypass it with our own do-nothing function
char MyFontTableLookup(const uint8_t ch)
{
	return ch;
}


void CreateMenuStructure();



//The setup function is called once at startup of the sketch
void setup()
{

//	Serial.begin(115200);
//	Serial.printf("\nhello\n");

	display.init();
	display.setColor(WHITE);
	display.clear();
	display.flipScreenVertically();

	display.display();

	display.setFontTableLookupFunction(MyFontTableLookup);


	CreateMenuStructure();


	pinMode(ROTARY_PIN_I,INPUT);
	pinMode(ROTARY_PIN_Q,INPUT);

	attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_I),RotaryISR,CHANGE);
	attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_Q),RotaryISR,CHANGE);

}

void loop()
{
	//debounce the rotary push action

	static bool bLastButtonInput=false;
	static bool bCurPush=false;
	bool bCurButtonInput=digitalRead(ROTARY_PIN_BTN)==0;	//get the actual GPI state

	static uint32_t uDebounceTimestamp=0;

	if(bLastButtonInput!=bCurButtonInput)	//has the state changed?
	{
		bLastButtonInput=bCurButtonInput;
		uDebounceTimestamp=millis();	//reset the timer
	}

	if(millis()-uDebounceTimestamp >= 10)	//ignore any input until it has been stable for 10 milliseconds, then accept
	{
		bCurPush=bCurButtonInput;	//bCurPush contains the debounced output!
	}

	static bool bLastPush=false;

	if(bLastPush!=bCurPush)	//has the debounced output state changed?
	{
		bLastPush=bCurPush;

		if(bCurPush)
		{
			flexmenu.Navigate(eFlexMenuNav_Push);
		}
		else
		{
			flexmenu.Navigate(eFlexMenuNav_Release);
		}
	}


	//loop through any buffered rotations _without_ updating the screen!

	while(iRotaryCounter<0)
	{
		iRotaryCounter++;
		flexmenu.Navigate(eFlexMenuNav_Prev);
	}

	while(iRotaryCounter>0)
	{
		iRotaryCounter--;
		flexmenu.Navigate(eFlexMenuNav_Next);
	}

	if(flexmenu.Loop(false))	//this draws the screen if necessary, and if so returns true so that we can
	{
		display.display();	//...tell the display to actually update.
	}

}



/*
This class is used below and shows how to make a *dynamic* menu.
All menu objects (whether sub menus or menu items) inherit the common FlexMenuBase interface.
The object is responsible for remembering the current item and scroll position within itself.
Thus, if we make our own object that inherits FlexMenuBase, we can make a menu programmatically,
which means not only dynamic updating but also less RAM use.
This way a menu could in theory be infinitely long even on a limited RAM system.
The data could come from anywhere.
 */

class DynamicMenu: public FlexMenuBase
{
public:

	int16_t GetNumSubItems() override { return 20; };	//how many items we say we have
	FlexMenuBase * GetSubItem(int16_t idx) override	//gets called when FlexMenuManager needs to act on a sub item. We're a sub menu so we have several, but a normal item may have none.
	{
		if(idx==0) return GetLeaveItem();	//The first item needs to be the LEAVE item, otherwise the user can't escape without a dedicated back button!

		FlexMenuTempItem & item=*((FlexMenuTempItem *)GetTempItem());	//there's one global temp item which lets us set the display text and other parameters.
		//This saves memory compared to having to allocate our own. It's a single core system, only one will be active at a time, so we can reuse this one.

		item.Reset();
		item.bIsLeave=false;
		item.SetDisplayCentered(true);

		switch(idx)	//what menu item are we?
		{
		case 1:	//let's make this a label
			item.SetDisplayCentered(true);
			item.strTitle="Uptime";
			break;
		case 2:
			break;
		case 3:	//display the uptime in milliseconds
			item.strTitle="Milliseconds";
			item.strValue=String(millis());
			break;
		case 4:	//display uptime in hours/minutes/seconds
			item.strTitle="Time";
			{
				char temp[64];
				sprintf(temp,"%02lu:%02lu:%02lu.%03lu",millis() / 360000,(millis() / 60000) % 60, (millis() / 1000) % 60,millis()%1000);
				item.strValue=temp;
			}
			break;
		default:	//all other items, just to show a nice long menu
			item.strTitle="Unused Item";
			item.strValue=String(idx);
			break;

		}

		return &item;
	};

	//FlexMenuBase has some uint8_t parameters (derived_use_x) squeezed into 32-bit alignment holes which are used for different purposes
	//by different menu objects. As long as we don't need more than 255 menu items we can save memory by using them instead of having our own!

	//each sub menu is responsible for remembering its own scroll position and current item.
	//that way if you leave and go back you'll be in the same position from where you left.
	int16_t GetScrollPos() override { return derived_use_1; }
	void SetScrollPos( int16_t iNewScrollPos ) override { derived_use_1=(uint8_t) iNewScrollPos; }

	int16_t GetCurItem() override { return derived_use_2; }
	void SetCurItem( int16_t iNewCurItem ) override { derived_use_2=(uint8_t) iNewCurItem; }

	//This text will be displayed left-aligned (or centered if GetValueText is empty and FlexMenuBase::DisplayCentered() returns true)
	void GetTitleText(String & strTitleDestination) override
	{
		strTitleDestination="Dynamic Menu";
	}

	//This text will be displayed right-aligned (or centered if GetTitleText is empty and FlexMenuBase::DisplayCentered() returns true)
	void GetValueText(String & strValueDestination) override
	{
		strValueDestination="";
	}

	//If we're a sub menu (or a control with a fine adjustment page) then CanEnter should return true
	virtual bool CanEnter() override { return true; }
	//The icon to display on our item in the parentmenu
	virtual eFlexMenuIcon UseIcon() override { return eFlexMenuIcon_RightArrow; }

	//FlexMenuManager tells us when we're the active menu! So, let's use that to remember whether we're active or not. but where to store it;
	void OnEnter() override { SetInMenu(true); }
	void OnLeave() override { SetInMenu(false); }


	//the three highest bits of FlexMenuBase::flags are available for derived object use. Tread lightly. :)
	void SetInMenu(bool bInMenu) { if(bInMenu) flags |= 0x40; else flags &=(0xFF-0x40); };
	bool GetInMenu() { return (flags & 0x40)!=0; };


	//FlexMenuManager calls us up to 30 times a second if we're currently displayed in the parent menu to ask if we have an update.
	uint32_t ulLastUpdateMillis=0;
	virtual bool GetNeedsRefresh() override
	{
		if(millis()-ulLastUpdateMillis>=250)	//let's update four times a second.
		{
			ulLastUpdateMillis=millis();
			FlexMenuBase::GetNeedsRefresh();	//when we return true, let's call our parent class function to make sure it doesn't retain a refresh flag for next time around
			return true;
		}
		return FlexMenuBase::GetNeedsRefresh();	//if we're not doing anything, call the parent class function.
	}

	void UpdateStatus() override	//any time our status has changed (like whether we're the active item) this function gets called
	{
		FlexMenuBase::UpdateStatus();
		ulLastUpdateMillis=millis();
	}

};



//I've really tried to make the base objects small. So, when we need to save a user ID per selector item (dropdown list item), we need to derive a class to have somewhere to keep it.

class SelectorItem : public FMISelector_Item
{
public:

	int iUserID=0;
};







void CreateMenuStructure()
{



	//Tell FlexMenuDisplay which OLED display to use.

	FlexMenuDisplay_OLED_Base_Params dp;

	dp.pOLEDDisplay=&display;

	flexmenudisplay.SetParams(&dp);


	//our project title
	flexmenu.strTitle="FlexMenu Example";


	//Init our FlexMenuManager object (flexmenu) and tell it which FlexMenuDisplay object to use.
	//In return, we get a pointer to the top menu where we can build our entire menu.

	FlexMenuSub * pTopMenu=flexmenu.Init(&flexmenudisplay);


	//This will be a label.
	{
		FlexMenuItemLabel * p;
		p=new FlexMenuItemLabel;

		p->strTitle=flexmenu.strTitle;
		p->SetDisplayCentered(true);

		pTopMenu->AddSubItem(p);
	}


	//Let's make a sub menu
	{
		FlexMenuSub * pSub;

		pSub=new FlexMenuSub;

		pSub->strTitle="Submenu A";

		pTopMenu->AddSubItem(pSub);

		//a sub menu has to have a LEAVE item otherwise the user can't escape without a back button!
		//there's one global leave item so we don't have to create our own. saves memory.
		pSub->AddSubItem(GetLeaveItem());


		//FlexMenuDisplay_OLED implements a decent on-screen keyboard to let you enter text!
		{
			FlexMenuItemEdit * p;
			p=new FlexMenuItemEdit;
			p->strTitle="Edit";
			//p->strEdit="Ögonskugga";
			p->strEdit="This is an edit box with some default text.";
			pSub->AddSubItem(p);

		}

		//Tall characters as an example (capital O with umlaut, and lower case G)
		{
			FlexMenuItemEdit * p;
			p=new FlexMenuItemEdit;
			p->strTitle="Edit";
			p->strEdit="Ögonskugga";
			pSub->AddSubItem(p);

		}


		//The Command item is for stateless execution of things, and can also ask you to confirm.
		{
			FlexMenuItemCommand * pCommand=new FlexMenuItemCommand;
			pCommand->strTitle="Execute";
			pCommand->SetConfirm(true);
			pCommand->cbExecuteCommand=[](FlexMenuItemCommand *)	//this function gets executed
			{
				Serial.printf("executed!\n");
				//display a fullscreen message for two seconds
				flexmenu.ShowMessage("Executed!", "", eFlexMenuFont_Large, 2000);
			};
			pSub->AddSubItem(pCommand);
		}


		//Let's make three sliders
		for(int i=0;i<3;i++)
		{
			FlexMenuItemSlider * pSlider=new FlexMenuItemSlider;

			char title[64];
			sprintf(title,"Slider %i",i+1);

			pSlider->SetTitleText(title);

			pSlider->SetValue(i*10+40);
			pSlider->SetRange(0,100);

			//pSlider->cbValueChanging=[](FlexMenuItemSlider * pSource
			pSub->AddSubItem(pSlider);

		}


		//Let's make a slider with a callback where we override the value displayed.
		//We're just displaying with a decimal point but you could override it to display anything you like, including a logarithmic curve.
		{
			FlexMenuItemSliderCB * p=new FlexMenuItemSliderCB;

			p->SetTitleText("Percentage");

			p->SetValue(500);
			p->SetRange(0,1000);

			//Lambda functions are pretty memory inefficient but if we only have a few it's okay.
			p->fnCallback=[](FlexMenuItemSliderCB * pSource, eFMISliderCallback code, String * pText) -> int
			{
				(void)(pText);
				switch(code)
				{
				default:
					break;
				case eFMISliderCallback_ValueChanged:	//happens when the knob is pushed after adjustment
					{
						String strTitle,strValue;
						pSource->GetTitleText(strTitle);
						pSource->GetValueText(strValue);
						Serial.printf("Slider \"%s\" changed to %s\n",strTitle.c_str(),strValue.c_str());
						break;
					}
				case eFMISliderCallback_ValueChanging:	//happens for every value change as the knob is turned
					{
						break;
					}
				case eFMISliderCallback_DisplayValue:	//Use to override the displayed text, in this case to present a decimal value
					{
						//single decimal.
						(*pText)=String(pSource->GetValue()/10);
						(*pText)+='.';
						(*pText)+=String(pSource->GetValue()%10);
						(*pText)+=" %";

						//alternative solution using floating point
						//char temp[16];
						//sprintf(temp,"%.01f %%",pSource->GetValue()*0.1);

						return true;
					}
				}

				return 0;
			};

			//any time we've created an item we should add it to either a sub menu or the top menu so that it can be displayed.
			pSub->AddSubItem(p);

		}

		{
			//FlexMenu knows how to load and save the state of all its menu items!
			//FlexMenu calls IsSaveable() on each item, and if true,
			//it calls GetIdentifier and then GetSaveString to get data for saving,
			//and LoadString with the saved data when loading.
			//GetIdentifier() returns the title of the item if no identifier has been set.
			FlexMenuItemCommand * pSaveCommand=new FlexMenuItemCommand;
			pSaveCommand->strTitle="Save Settings";
			pSaveCommand->cbExecuteCommand=[](FlexMenuItemCommand *)
			{
				FlexMenuSPIFFS_DoSave(flexmenu);
			};
			pSub->AddSubItem(pSaveCommand);

		}



	}

	//Add another sub menu
	{
		FlexMenuSub * pSub;

		pSub=new FlexMenuSub;

		pSub->strTitle="Submenu B";

		pTopMenu->AddSubItem(pSub);

		pSub->AddSubItem(GetLeaveItem());


		//Three toggle switches
		for(int i=0;i<3;i++)
		{
			FlexMenuItemToggle * p=new FlexMenuItemToggle;
			p->strTitle="Toggle ";
			p->strTitle+=String(i+1);
			pSub->AddSubItem(p);
		}





		//A selector (dropdown list), newing and removing the items as you enter and leave!
		{
			FlexMenuItemSelectorCB * p;
			p=new FlexMenuItemSelectorCB;


			p->strTitle="Magic";
			p->SetCurSel(3);
			p->SetMode(eFMISelector_Mode_SaveIndex);

			p->cbOnActivityChange=[](FlexMenuItemSelectorCB * pSource)
			{
				if(pSource->IsActive())
				{
					Serial.printf("newing selector items\n");
					for(int j=0;j<(int) 15;j++)
					{
						char szTemp[64];
						SelectorItem * pItem=new SelectorItem;
						sprintf(szTemp,"SelectorItem %i",j);
						pItem->strText=szTemp;
						pItem->iUserID=1000+j;

						pSource->vecItems.push_back(pItem);

					}
				}
				else
				{
					Serial.printf("DELETING selector items\n");
					for(int j=0;j<(int) pSource->vecItems.size();j++)
					{
						delete pSource->vecItems[j];
					}

					pSource->vecItems.clear();


				}
				//csprintf("SELECTOR ACTIVE=%i\n",pSource->IsActive());

			};

			pSub->AddSubItem(p);

		}

		//a normal selector (dropdown menu). this one saves the current selection by ID
		{
			FlexMenuItemSelector * p;
			p=new FlexMenuItemSelector;

			p->strTitle="Select";
			p->SetMode(eFMISelector_Mode_SaveID);

			FMISelector_Item * pItem;
			//add some items. Doing it this way is convenient but uses RAM.
			pItem=new FMISelector_Item;
			pItem->strText="Item One";
			pItem->id=1;
			p->vecItems.push_back(pItem);

			pItem=new FMISelector_Item;
			pItem->strText="Item Two";
			pItem->id=2;
			p->vecItems.push_back(pItem);

			pItem=new FMISelector_Item;
			pItem->strText="Item Seven";
			pItem->id=7;	//items don't have to be in order!
			p->SetCurSel(p->vecItems.size());	//but the current selection is always done by index number, not ID.
			p->vecItems.push_back(pItem);

			pSub->AddSubItem(p);
		}

		//a normal selector (dropdown menu). this one saves the current selection by the item text.
		{
			FlexMenuItemSelector * p;
			p=new FlexMenuItemSelector;

			p->strTitle="Name";
			p->SetMode(eFMISelector_Mode_SaveText);

			FMISelector_Item * pItem;
			pItem=new FMISelector_Item;
			pItem->strText="Tom";
			p->vecItems.push_back(pItem);

			pItem=new FMISelector_Item;
			pItem->strText="Dick";
			p->SetCurSel(p->vecItems.size());
			p->vecItems.push_back(pItem);

			pItem=new FMISelector_Item;
			pItem->strText="Harry";
			p->vecItems.push_back(pItem);

			pSub->AddSubItem(p);
		}


		{
			FlexMenuItemCommand * pSaveCommand=new FlexMenuItemCommand;
			pSaveCommand->strTitle="Save Settings";
			pSaveCommand->cbExecuteCommand=[](FlexMenuItemCommand *)
			{
				FlexMenuSPIFFS_DoSave(flexmenu);
			};
			pSub->AddSubItem(pSaveCommand);

		}


	}

	pTopMenu->AddSubItem(new DynamicMenu);








	//a spacer (empty item)
	{
		FlexMenuItemSpacer * p;
		p=new FlexMenuItemSpacer;

		pTopMenu->AddSubItem(p);
	}


	//a slider in the main menu
	{
		FlexMenuItemSlider * pSlider=new FlexMenuItemSlider;
		pSlider->SetTitleText("Pulse Count");
		pSlider->SetValue(73);
		pSlider->SetRange(0,100);
		/*pSlider->cbDisplayValue=[](FlexMenuItemSlider * pSource, String & strValue)
		{
			strValue=spr("%.02f pulses",pSource->value*0.01f);
		};*/

		//pSlider->cbValueChanging=[](FlexMenuItemSlider * pSource
		pTopMenu->AddSubItem(pSlider);

	}



	FlexMenuItemCommand * pSaveCommand=new FlexMenuItemCommand;
	pSaveCommand->strTitle="Save Settings";
	pSaveCommand->cbExecuteCommand=[](FlexMenuItemCommand *)
	{
		FlexMenuSPIFFS_DoSave(flexmenu);
	};
	pTopMenu->AddSubItem(pSaveCommand);




	//initialize (and optionally format) SPIFFS (ESP32) or LittleFS (ESP8266) filesystem

	if(FlexMenuSPIFFS_Init(flexmenu))
	{

		FlexMenuSPIFFS_DoLoad(flexmenu);
	}



}
