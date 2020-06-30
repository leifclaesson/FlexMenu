/*
 * FlexMenuSPIFFS.h
 *
 *  Created on: 30 Jun 2020
 *      Author: user
 */

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUSPIFFS_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUSPIFFS_H_

class FlexMenuManager;

bool FlexMenuSPIFFS_Init(FlexMenuManager & flexmenu);
void FlexMenuSPIFFS_DoLoad(FlexMenuManager & flexmenu);
void FlexMenuSPIFFS_DoSave(FlexMenuManager & flexmenu);




#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUSPIFFS_H_ */
