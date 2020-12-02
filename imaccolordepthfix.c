//  imaccolordepthfix.c
//  Created by Peter Viszt on 2020/12/01.

#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include "header.h"

int main(int argc, char* argv[]) {
  CGDisplayCount screenCount;
  CGGetOnlineDisplayList(INT_MAX, NULL, &screenCount);

  CGDirectDisplayID screenList[screenCount];
  CGGetOnlineDisplayList(INT_MAX, screenList, &screenCount);

  ScreenConfig screenConfigs[screenCount];
  for (int i = 0; i < screenCount; i++) {
    screenConfigs[i].id = screenList[i];
  }

  for (int i = 0; i < screenCount; i++) {
    ScreenConfig curScreen = screenConfigs[i];

    //Skip if its not the main display
    if(CGMainDisplayID()!=curScreen.id) continue;

    int curModeId;
    CGSGetCurrentDisplayMode(curScreen.id, &curModeId);
    modes_D4 curMode;
    CGSGetDisplayModeDescriptionOfLength(curScreen.id, curModeId, &curMode, 0xD4);

    char curScreenUUID[UUID_SIZE];
    CFStringGetCString(CFUUIDCreateString(kCFAllocatorDefault, CGDisplayCreateUUIDFromDisplayID(curScreen.id)), curScreenUUID, sizeof(curScreenUUID), kCFStringEncodingUTF8);
    curScreen.id = convertUUIDtoID(curScreenUUID);

    CGDisplayConfigRef configRef;
    CGBeginDisplayConfiguration(&configRef);
    configureResolution(configRef, curScreen.id, curScreenUUID, (int) CGDisplayPixelsWide(curScreen.id), (int) CGDisplayPixelsHigh(curScreen.id));
    CGCompleteDisplayConfiguration(configRef, kCGConfigurePermanently);
  }
  return 0;
}

CGDirectDisplayID convertUUIDtoID(char* uuid) {
  if (strstr(uuid, "-") == NULL) {
    return atoi(uuid);
  }

  CFStringRef uuidStringRef = CFStringCreateWithCString(kCFAllocatorDefault, uuid, kCFStringEncodingUTF8);
  CFUUIDRef uuidRef = CFUUIDCreateFromString(kCFAllocatorDefault, uuidStringRef);
  return CGDisplayGetDisplayIDFromUUID(uuidRef);
}

bool configureResolution(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, int width, int height) {
  int modeCount;
  modes_D4* modes;

  CopyAllDisplayModes(screenId, &modes, &modeCount);

  modes_D4 bestMode = modes[0];
  bool modeFound = false;

  //loop through all modes looking for one that matches user input params
  for (int i = 0; i < modeCount; i++) {
    modes_D4 curMode = modes[i];

    //prioritize exact matches of user input params
    if (curMode.derived.width != width) continue;
    if (curMode.derived.height != height) continue;
    if (curMode.derived.freq != 0) continue;
    if (curMode.derived.depth != 8) continue; //This is the important bit, set it to 8 to fix color depth

    if (!modeFound) {
      modeFound = true;
      bestMode = curMode;
    }

    if (curMode.derived.freq > bestMode.derived.freq || (curMode.derived.freq == bestMode.derived.freq && curMode.derived.depth > bestMode.derived.depth)) {
      bestMode = curMode;
    }
  }

  if (modeFound) {
    CGSConfigureDisplayMode(configRef, screenId, bestMode.derived.mode);
    return true;
  }

  return false;
}
