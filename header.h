//Thanks to Phoenix Dev for modes_D4 type for use in Apple's private core graphics APIs
typedef union
{
  uint8_t rawData[0xDC];
  struct
  {
    uint32_t mode;
    uint32_t flags;		// 0x4
    uint32_t width;		// 0x8
    uint32_t height;	// 0xC
    uint32_t depth;		// 0x10
    uint32_t dc2[42];
    uint16_t dc3;
    uint16_t freq;		// 0xBC
    uint32_t dc4[4];
    float density;		// 0xD0
  } derived;
} modes_D4;

//Apple's private core graphics APIs
void CGSGetCurrentDisplayMode(CGDirectDisplayID display, int* modeNum);
void CGSGetNumberOfDisplayModes(CGDirectDisplayID display, int* nModes);
void CGSGetDisplayModeDescriptionOfLength(CGDirectDisplayID display, int idx, modes_D4* mode, int length);
void CGSConfigureDisplayMode(CGDisplayConfigRef config, CGDirectDisplayID display, int modeNum);

//Thanks to Phoenix Dev for CopyAllDisplayModes to fill the modes_D4 structures
void CopyAllDisplayModes(CGDirectDisplayID display, modes_D4** modes, int* cnt)
{
  int nModes;
  CGSGetNumberOfDisplayModes(display, &nModes);

  if(nModes)
    *cnt = nModes;

  if(!modes)
    return;

  *modes = (modes_D4*) malloc(sizeof(modes_D4)* nModes);
  for(int i=0; i<nModes; i++) {
    CGSGetDisplayModeDescriptionOfLength(display, i, &(*modes)[i], 0xD4);
  }
}

const int UUID_SIZE = 37;
const int MIRROR_MAX = 127;

typedef struct
{
  char uuid[UUID_SIZE];                    //user input display identifier that stays consistent despite GPU or port changes (persistent screen id)
  CGDirectDisplayID id;                    //display identifier used for Quartz Display Services (contextual screen id)
  int width;                               //pixels wide
  int height;                              //pixels tall
  int depth;                               //color depth
  bool scaled;                             //scaling
  int x;                                   //origin x position
  int y;                                   //origin y position
  int modeNum;                             //display mode id
} ScreenConfig;

CGDirectDisplayID convertUUIDtoID(char* uuid);
bool configureResolution(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, int width, int height);