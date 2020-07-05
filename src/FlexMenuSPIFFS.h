/*
 * FlexMenuSPIFFS.h
 *
 *  Created on: 30 Jun 2020
 *      Author: user
 */

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUSPIFFS_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUSPIFFS_H_

#if defined(ARDUINO_ARCH_ESP32)
#include "SPIFFS.h"
#define FLEXMENU_FILESYSTEM SPIFFS
#else
#include "LittleFS.h"
#define FLEXMENU_FILESYSTEM LittleFS
#endif


class FlexMenuManager;

bool FlexMenuSPIFFS_Init(FlexMenuManager & flexmenu);
void FlexMenuSPIFFS_DoLoad(FlexMenuManager & flexmenu);
void FlexMenuSPIFFS_DoSave(FlexMenuManager & flexmenu);

FS & FlexMenuGetFileSystem();
const char * FlexMenuGetConfigFileName();



#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUSPIFFS_H_ */
