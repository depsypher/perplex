#include "common.cpp"

#ifdef QB64_MACOSX
#include <ApplicationServices/ApplicationServices.h>
#endif

#ifdef QB64_LINUX
#ifndef QB64_MACOSX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#endif


/*
int32 allocated_bytes=0;
void *malloc2(int x){
allocated_bytes+=x;
return malloc(x);
}
void *realloc2(void *x, int y){
allocated_bytes+=y;
return realloc(x,y);
}
void *calloc2(int x, int y){
allocated_bytes+=(x*y);
return calloc(x,y);
}
#define malloc(x) malloc2(x)
#define calloc(x,y) calloc2(x,y)
#define realloc(x,y) realloc2(x,y)
*/

int64 device_event_index=0;
int32 device_mouse_relative=0;

int32 lock_mainloop=0;//0=unlocked, 1=lock requested, 2=locked
 
int32 lpos=1;
int32 width_lprint=80;

//forward refs
int32 func__source();
int32 func_pos(int32 ignore);
void sub__printimage(int32 i);
double func_timer(double accuracy,int32 passed);
int32 func__newimage(int32 x,int32 y,int32 bpp,int32 passed);
void display();
void validatepage(int32);
void sub__dest(int32);
void sub__source(int32);
int32 func__printwidth(qbs*,int32,int32);
void sub_cls(int32,uint32,int32);
void qbs_print(qbs*,int32);
int32 func__copyimage(int32,int32);
int32 func__dest();
int32 func__display();
void qbg_sub_view_print(int32,int32,int32);
qbs *qbs_new(int32,uint8);
qbs *qbs_new_txt(const char*);
qbs *qbs_add(qbs*,qbs*);
qbs *qbs_set(qbs*,qbs*);
void qbg_sub_window(float,float,float,float,int32);
int32 autodisplay=1;
//GFS forward references
int32 gfs_eof_passed(int32 i);
int32 gfs_eof_reached(int32 i);
int64 gfs_getpos(int32 i);
int32 gfs_fileno_valid(int32 f);
int32 gfs_fileno_freefile();//like FREEFILE
void gfs_fileno_use(int32 f, int32 i);
int32 gfs_open(qbs *filename,int32 access,int32 restrictions, int32 how);
int32 gfs_close(int32 i);
int64 gfs_lof(int32 i);
int32 gfs_setpos(int32 i, int64 position);
int32 gfs_write(int32 i,int64 position,uint8 *data,int64 size);
int32 gfs_read(int32 i,int64 position,uint8 *data,int64 size);
int64 gfs_read_bytes();
extern qbs *qbs_str(int64 value);
extern qbs *qbs_str(int32 value);
extern qbs *qbs_str(int16 value);
extern qbs *qbs_str(int8 value);
extern qbs *qbs_str(uint64 value);
extern qbs *qbs_str(uint32 value);
extern qbs *qbs_str(uint16 value);
extern qbs *qbs_str(uint8 value);
extern qbs *qbs_str(float value);
extern qbs *qbs_str(double value);
extern qbs *qbs_str(long double value);
void key_update();
int32 key_display_state=0;
int32 key_display=0;
int32 key_display_redraw=0;
extern int32 device_last;
extern int32 device_max;
extern device_struct *devices;
//...


//format:[deadkey's symbol in UTF16],[ASCII code of alphabet letter],[resulting UTF16 character]...0
static uint16 deadchar_lookup[] = {
96,97,224,96,65,192,180,97,225,180,65,193,94,97,226,94,65,194,126,97,227,126,65,195,168,97,228,168,65,196,730,97,229,730,65,197,180,99,263,94,99,265,96,101,232,96,69,200,180,101,233,180,69,201,94,101,234,94,69,202,126,101,7869,168,101,235,168,69,203,180,103,501,94,103,285,94,104,293,168,104,7719,96,105,236,96,73,204,180,105,237,180,73,205,94,105,238,94,73,206,126,105,297,168,105,239,168,73,207,94,106,309,180,107,7729,180,108,314,180,109,7743,96,110,505,180,110,324,126,110,241,126,78,209,96,111,242,96,79,210,180,111,243,180,79,211,94,111,244,94,79,212,126,111,245,126,79,213,168,111,246,168,79,214,180,112,7765,180,114,341,180,115,347,94,115,349,168,116,7831,96,117,249,96,85,217,180,117,250,180,85,218,94,117,251,94,85,219,126,117,361,168,117,252,168,85,220,730,117,367,126,118,7805,96,119,7809,180,119,7811,94,119,373,168,119,7813,730,119,7832,168,120,7821,96,121,7923,180,121,253,180,89,221,94,121,375,126,121,7929,168,121,255,168,89,376,730,121,7833,180,122,378,94,122,7825
,0
};

int32 keydown_glyph=0;

static uint16 codepage437_to_unicode16[] = {
0x0020,0x263A,0x263B,0x2665,0x2666,0x2663,0x2660,0x2022,0x25D8,0x25CB,0x25D9,0x2642,0x2640,0x266A,0x266B,0x263C,
0x25BA,0x25C4,0x2195,0x203C,0x00B6,0x00A7,0x25AC,0x21A8,0x2191,0x2193,0x2192,0x2190,0x221F,0x2194,0x25B2,0x25BC,
0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,
0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x2302,
0x00C7,0x00FC,0x00E9,0x00E2,0x00E4,0x00E0,0x00E5,0x00E7,0x00EA,0x00EB,0x00E8,0x00EF,0x00EE,0x00EC,0x00C4,0x00C5,
0x00C9,0x00E6,0x00C6,0x00F4,0x00F6,0x00F2,0x00FB,0x00F9,0x00FF,0x00D6,0x00DC,0x00A2,0x00A3,0x00A5,0x20A7,0x0192,
0x00E1,0x00ED,0x00F3,0x00FA,0x00F1,0x00D1,0x00AA,0x00BA,0x00BF,0x2310,0x00AC,0x00BD,0x00BC,0x00A1,0x00AB,0x00BB,
0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255D,0x255C,0x255B,0x2510,
0x2514,0x2534,0x252C,0x251C,0x2500,0x253C,0x255E,0x255F,0x255A,0x2554,0x2569,0x2566,0x2560,0x2550,0x256C,0x2567,
0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256B,0x256A,0x2518,0x250C,0x2588,0x2584,0x258C,0x2590,0x2580,
0x03B1,0x00DF,0x0393,0x03C0,0x03A3,0x03C3,0x00B5,0x03C4,0x03A6,0x0398,0x03A9,0x03B4,0x221E,0x03C6,0x03B5,0x2229,
0x2261,0x00B1,0x2265,0x2264,0x2320,0x2321,0x00F7,0x2248,0x00B0,0x2219,0x00B7,0x221A,0x207F,0x00B2,0x25A0,0x0020
};

int32 convert_unicode(int32 src_fmt,void *src_buf,int32 src_size,int32 dest_fmt,void *dest_buf){
/*
important: to ensure enough space is available for the conversion, dest_buf must be at least src_size*4+4 in length
returns: the number of bytes written to dest_buf
fmt values:
	1=ASCII(CP437)
	8=UTF8
	16=UTF16
	32=UTF32
*/

static int32 dest_size;
dest_size=0;

//setup source
uint8 *src_uint8p=NULL;
if (src_fmt==1){
src_uint8p=(uint8*)src_buf;
}
uint16 *src_uint16p=NULL;
if (src_fmt==16){
src_uint16p=(uint16*)src_buf;
src_size=src_size-(src_size&1);//cull trailing bytes
}
uint32 *src_uint32p=NULL;
if (src_fmt==32){
src_uint32p=(uint32*)src_buf;
src_size=src_size-(src_size&3);//cull trailing bytes
}

//setup dest
uint16 *dest_uint16p=NULL;
if (dest_fmt==16){
dest_uint16p=(uint16*)dest_buf;
}
uint32 *dest_uint32p=NULL;
if (dest_fmt==32){
dest_uint32p=(uint32*)dest_buf;
}

uint32 x;//scalar

while(src_size){

//convert src to scalar UNICODE value 'x'

if (src_fmt==1){//CP437
x=*src_uint8p++;
src_size--;
x=codepage437_to_unicode16[x];
}
if (src_fmt==16){//UTF16
src_size-=2;
x=*src_uint16p++;
//note: does not handle surrogate pairs yet
}
if (src_fmt==32){//UTF32
src_size-=4;
x=*src_uint32p++;
}

//convert scalar UNICODE value 'x' to dest

if (dest_fmt==16){//UTF16
*dest_uint16p++=x;
dest_size+=2;
//note: does not handle surrogate pairs yet
}
if (dest_fmt==32){//UTF32
*dest_uint32p++=x;
dest_size+=4;
}

}//loop

//add NULL terminator (does not change the size in bytes returned)
if (dest_fmt==16) *dest_uint16p=0;
if (dest_fmt==32) *dest_uint32p=0;

return dest_size;
}





#ifdef QB64_WINDOWS
void showvalue(__int64);
#endif
void sub_beep();

int32 func__loadfont(qbs *filename,double size,qbs *requirements,int32 passed);

//font management
#define lastfont 1023
TTF_Font *font[lastfont+1];//NULL=unused index
int32 fontheight[lastfont+1];
int32 fontwidth[lastfont+1];
int32 fontflags[lastfont+1];
#ifdef QB64_WINDOWS
#define QB64_IME
#endif

//keyboard input upgrade

//global IME related variables
#ifdef QB64_IME
int32 ime_DrawMessageBlock_lastpixeloffset=0;
SDL_Surface *ime_back;
static qbs *imefontname;
static qbs *imefontname2;
static qbs *imefontname3;
static qbs *imefontname4;
static TTF_Font *ime_font=NULL;
#endif
int32 qb64_ime_reading=0;




//keyhit cyclic buffer
int64 keyhit[8192];
//    keyhit specific internal flags: (stored in high 32-bits)
//    &4294967296->numpad was used
int32 keyhit_nextfree=0;
int32 keyhit_next=0;
//note: if full, the oldest message is discarded to make way for the new message




void update_shift_state();




uint32 bindkey=0;

void scancodedown(uint8 scancode);
void scancodeup(uint8 scancode);

#define QBK 200000
#define VK 100000
#define UC 1073741824



/* QB64 specific codes:

200000-200010: Numpad keys with Num-Lock off
    NO_NUMLOCK_KP0=INSERT
	NO_NUMLOCK_KP1=END
	NO_NUMLOCK_KP2=DOWN
	NO_NUMLOCK_KP3=PGDOWN
	NO_NUMLOCK_KP4...
	NO_NUMLOCK_KP5
	NO_NUMLOCK_KP6
	NO_NUMLOCK_KP7
	NO_NUMLOCK_KP8
	NO_NUMLOCK_KP9
	NO_NUMLOCK_KP_PERIOD=DEL
200011: SCROLL_LOCK_ON
200012: INSERT_MODE_ON
*/
#define QBK_SCROLL_LOCK_MODE 11
#define QBK_INSERT_MODE 12
#define QBK_CHR0 13



/*
QB64 Mapping of audio control keyboard keys:
MEDIA_PLAY_PAUSE 0x2200
MEDIA_STOP 0x2400
MEDIA_NEXT_TRACK 0x1900
MEDIA_PREV_TRACK 0x1000
*/

/*
compatibility upgrade:
0-255
num & caps versions are totally ignored
shift is USUALLY the same as ascii but not always
0 anywhere else means a key cannot be entered
*/


static const int32 scancode_lookup[]={
//DESCRIPTION OFFSET  SCANCODE      ASCII      SHIFT       CTRL        ALT        NUM       CAPS SHIFT+CAPS  SHIFT+NUM
/* ?       */    0 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    1 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    2 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    3 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    4 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    5 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    6 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    7 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* Bksp    */    8 ,      0x0E,      0x08,      0x08,      0x7F,    0x0E00,         0,         0,         0,         0,
/* Tab     */    9 ,      0x0F,      0x09,    0x0F00,    0x9400,         0,         0,         0,         0,         0,
/* ?       */   10 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   11 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   12 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* enter   */   13 ,      0x1C,      0x0D,      0x0D,      0x0A,         0,         0,         0,         0,         0,
/* ?       */   14 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   15 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   16 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   17 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   18 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   19 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   20 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   21 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   22 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   23 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   24 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   25 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   26 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* Esc     */   27 ,      0x01,      0x1B,      0x1B,      0x1B,         0,         0,         0,         0,         0,
/* ?       */   28 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   29 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   30 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   31 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* space   */   32 ,      0x39,      0x20,      0x20,      0x20,         0,         0,         0,         0,         0,
/* 1 !     */   33 ,      0x02,      0x21,      0x21,         0,    0x7800,         0,         0,         0,         0,
/*       */   34 ,      0x28,      0x22,      0x22,         0,    0x2800,         0,         0,         0,         0,
/* 3 #     */   35 ,      0x04,      0x23,      0x23,         0,    0x7A00,         0,         0,         0,         0,
/* 4 $     */   36 ,      0x05,      0x24,      0x24,         0,    0x7B00,         0,         0,         0,         0,
/* 5 %     */   37 ,      0x06,      0x25,      0x25,         0,    0x7C00,         0,         0,         0,         0,
/* 7 &     */   38 ,      0x08,      0x26,      0x26,         0,    0x7E00,         0,         0,         0,         0,
/*       */   39 ,      0x28,      0x27,      0x27,         0,    0x2800,         0,         0,         0,         0,
/* 9 (     */   40 ,      0x0A,      0x28,      0x28,         0,    0x8000,         0,         0,         0,         0,
/* 0 )     */   41 ,      0x0B,      0x29,      0x29,         0,    0x8100,         0,         0,         0,         0,
/* 8 *     */   42 ,      0x09,      0x2A,      0x2A,         0,    0x7F00,         0,         0,         0,         0,
/* = +     */   43 ,      0x0D,      0x2B,      0x2B,         0,    0x8300,         0,         0,         0,         0,
/* , <     */   44 ,      0x33,      0x2C,      0x2C,         0,    0x3300,         0,         0,         0,         0,
/* - _     */   45 ,      0x0C,      0x2D,      0x2D,      0x1F,    0x8200,         0,         0,         0,         0,
/* . >     */   46 ,      0x34,      0x2E,      0x2E,         0,    0x3400,         0,         0,         0,         0,
/* / ?     */   47 ,      0x35,      0x2F,      0x2F,         0,    0x3500,         0,         0,         0,         0,
/* 0 )     */   48 ,      0x0B,      0x30,      0x30,         0,    0x8100,         0,         0,         0,         0,
/* 1 !     */   49 ,      0x02,      0x31,      0x31,         0,    0x7800,         0,         0,         0,         0,
/* 2 @     */   50 ,      0x03,      0x32,      0x32,    0x0300,    0x7900,         0,         0,         0,         0,
/* 3 #     */   51 ,      0x04,      0x33,      0x33,         0,    0x7A00,         0,         0,         0,         0,
/* 4 $     */   52 ,      0x05,      0x34,      0x34,         0,    0x7B00,         0,         0,         0,         0,
/* 5 %     */   53 ,      0x06,      0x35,      0x35,         0,    0x7C00,         0,         0,         0,         0,
/* 6 ^     */   54 ,      0x07,      0x36,      0x36,      0x1E,    0x7D00,         0,         0,         0,         0,
/* 7 &     */   55 ,      0x08,      0x37,      0x37,         0,    0x7E00,         0,         0,         0,         0,
/* 8 *     */   56 ,      0x09,      0x38,      0x38,         0,    0x7F00,         0,         0,         0,         0,
/* 9 (     */   57 ,      0x0A,      0x39,      0x39,         0,    0x8000,         0,         0,         0,         0,
/* ; :     */   58 ,      0x27,      0x3A,      0x3A,         0,    0x2700,         0,         0,         0,         0,
/* ; :     */   59 ,      0x27,      0x3B,      0x3B,         0,    0x2700,         0,         0,         0,         0,
/* , <     */   60 ,      0x33,      0x3C,      0x3C,         0,    0x3300,         0,         0,         0,         0,
/* = +     */   61 ,      0x0D,      0x3D,      0x3D,         0,    0x8300,         0,         0,         0,         0,
/* . >     */   62 ,      0x34,      0x3E,      0x3E,         0,    0x3400,         0,         0,         0,         0,
/* / ?     */   63 ,      0x35,      0x3F,      0x3F,         0,    0x3500,         0,         0,         0,         0,
/* 2 @     */   64 ,      0x03,      0x40,      0x40,    0x0300,    0x7900,         0,         0,         0,         0,
/* A       */   97 ,      0x1E,      0x41,      0x41,      0x01,    0x1E00,         0,         0,         0,         0,
/* B       */   98 ,      0x30,      0x42,      0x42,      0x02,    0x3000,         0,         0,         0,         0,
/* C       */   99 ,      0x2E,      0x43,      0x43,      0x03,    0x2E00,         0,         0,         0,         0,
/* D       */  100 ,      0x20,      0x44,      0x44,      0x04,    0x2000,         0,         0,         0,         0,
/* E       */  101 ,      0x12,      0x45,      0x45,      0x05,    0x1200,         0,         0,         0,         0,
/* F       */  102 ,      0x21,      0x46,      0x46,      0x06,    0x2100,         0,         0,         0,         0,
/* G       */  103 ,      0x22,      0x47,      0x47,      0x07,    0x2200,         0,         0,         0,         0,
/* H       */  104 ,      0x23,      0x48,      0x48,      0x08,    0x2300,         0,         0,         0,         0,
/* I       */  105 ,      0x17,      0x49,      0x49,      0x09,    0x1700,         0,         0,         0,         0,
/* J       */  106 ,      0x24,      0x4A,      0x4A,      0x0A,    0x2400,         0,         0,         0,         0,
/* K       */  107 ,      0x25,      0x4B,      0x4B,      0x0B,    0x2500,         0,         0,         0,         0,
/* L       */  108 ,      0x26,      0x4C,      0x4C,      0x0C,    0x2600,         0,         0,         0,         0,
/* M       */  109 ,      0x32,      0x4D,      0x4D,      0x0D,    0x3200,         0,         0,         0,         0,
/* N       */  110 ,      0x31,      0x4E,      0x4E,      0x0E,    0x3100,         0,         0,         0,         0,
/* O       */  111 ,      0x18,      0x4F,      0x4F,      0x0F,    0x1800,         0,         0,         0,         0,
/* P       */  112 ,      0x19,      0x50,      0x50,      0x10,    0x1900,         0,         0,         0,         0,
/* Q       */  113 ,      0x10,      0x51,      0x51,      0x11,    0x1000,         0,         0,         0,         0,
/* R       */  114 ,      0x13,      0x52,      0x52,      0x12,    0x1300,         0,         0,         0,         0,
/* S       */  115 ,      0x1F,      0x53,      0x53,      0x13,    0x1F00,         0,         0,         0,         0,
/* T       */  116 ,      0x14,      0x54,      0x54,      0x14,    0x1400,         0,         0,         0,         0,
/* U       */  117 ,      0x16,      0x55,      0x55,      0x15,    0x1600,         0,         0,         0,         0,
/* V       */  118 ,      0x2F,      0x56,      0x56,      0x16,    0x2F00,         0,         0,         0,         0,
/* W       */  119 ,      0x11,      0x57,      0x57,      0x17,    0x1100,         0,         0,         0,         0,
/* X       */  120 ,      0x2D,      0x58,      0x58,      0x18,    0x2D00,         0,         0,         0,         0,
/* Y       */  121 ,      0x15,      0x59,      0x59,      0x19,    0x1500,         0,         0,         0,         0,
/* Z       */  122 ,      0x2C,      0x5A,      0x5A,      0x1A,    0x2C00,         0,         0,         0,         0,
/* [ {     */   91 ,      0x1A,      0x5B,      0x5B,      0x1B,    0x1A00,         0,         0,         0,         0,
/* \ |     */   92 ,      0x2B,      0x5C,      0x5C,      0x1C,    0x2B00,         0,         0,         0,         0,
/* ] }     */   93 ,      0x1B,      0x5D,      0x5D,      0x1D,    0x1B00,         0,         0,         0,         0,
/* 6 ^     */   94 ,      0x07,      0x5E,      0x5E,      0x1E,    0x7D00,         0,         0,         0,         0,
/* - _     */   95 ,      0x0C,      0x5F,      0x5F,      0x1F,    0x8200,         0,         0,         0,         0,
/* ` ~     */   96 ,      0x29,      0x60,      0x60,         0,    0x2900,         0,         0,         0,         0,
/* A       */   97 ,      0x1E,      0x61,      0x61,      0x01,    0x1E00,         0,         0,         0,         0,
/* B       */   98 ,      0x30,      0x62,      0x62,      0x02,    0x3000,         0,         0,         0,         0,
/* C       */   99 ,      0x2E,      0x63,      0x63,      0x03,    0x2E00,         0,         0,         0,         0,
/* D       */  100 ,      0x20,      0x64,      0x64,      0x04,    0x2000,         0,         0,         0,         0,
/* E       */  101 ,      0x12,      0x65,      0x65,      0x05,    0x1200,         0,         0,         0,         0,
/* F       */  102 ,      0x21,      0x66,      0x66,      0x06,    0x2100,         0,         0,         0,         0,
/* G       */  103 ,      0x22,      0x67,      0x67,      0x07,    0x2200,         0,         0,         0,         0,
/* H       */  104 ,      0x23,      0x68,      0x68,      0x08,    0x2300,         0,         0,         0,         0,
/* I       */  105 ,      0x17,      0x69,      0x69,      0x09,    0x1700,         0,         0,         0,         0,
/* J       */  106 ,      0x24,      0x6A,      0x6A,      0x0A,    0x2400,         0,         0,         0,         0,
/* K       */  107 ,      0x25,      0x6B,      0x6B,      0x0B,    0x2500,         0,         0,         0,         0,
/* L       */  108 ,      0x26,      0x6C,      0x6C,      0x0C,    0x2600,         0,         0,         0,         0,
/* M       */  109 ,      0x32,      0x6D,      0x6D,      0x0D,    0x3200,         0,         0,         0,         0,
/* N       */  110 ,      0x31,      0x6E,      0x6E,      0x0E,    0x3100,         0,         0,         0,         0,
/* O       */  111 ,      0x18,      0x6F,      0x6F,      0x0F,    0x1800,         0,         0,         0,         0,
/* P       */  112 ,      0x19,      0x70,      0x70,      0x10,    0x1900,         0,         0,         0,         0,
/* Q       */  113 ,      0x10,      0x71,      0x71,      0x11,    0x1000,         0,         0,         0,         0,
/* R       */  114 ,      0x13,      0x72,      0x72,      0x12,    0x1300,         0,         0,         0,         0,
/* S       */  115 ,      0x1F,      0x73,      0x73,      0x13,    0x1F00,         0,         0,         0,         0,
/* T       */  116 ,      0x14,      0x74,      0x74,      0x14,    0x1400,         0,         0,         0,         0,
/* U       */  117 ,      0x16,      0x75,      0x75,      0x15,    0x1600,         0,         0,         0,         0,
/* V       */  118 ,      0x2F,      0x76,      0x76,      0x16,    0x2F00,         0,         0,         0,         0,
/* W       */  119 ,      0x11,      0x77,      0x77,      0x17,    0x1100,         0,         0,         0,         0,
/* X       */  120 ,      0x2D,      0x78,      0x78,      0x18,    0x2D00,         0,         0,         0,         0,
/* Y       */  121 ,      0x15,      0x79,      0x79,      0x19,    0x1500,         0,         0,         0,         0,
/* Z       */  122 ,      0x2C,      0x7A,      0x7A,      0x1A,    0x2C00,         0,         0,         0,         0,
/* [ {     */  123 ,      0x1A,      0x7B,      0x7B,      0x1B,    0x1A00,         0,         0,         0,         0,
/* \ |     */  124 ,      0x2B,      0x7C,      0x7C,      0x1C,    0x2B00,         0,         0,         0,         0,
/* ] }     */  125 ,      0x1B,      0x7D,      0x7D,      0x1D,    0x1B00,         0,         0,         0,         0,
/* ` ~     */  126 ,      0x29,      0x7E,      0x7E,         0,    0x2900,         0,         0,         0,         0,
/* ?       */  127 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  128 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  129 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  130 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  131 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  132 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  133 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  134 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  135 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  136 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  137 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  138 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  139 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  140 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  141 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  142 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  143 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  144 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  145 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  146 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  147 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  148 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  149 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  150 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  151 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  152 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  153 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  154 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  155 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  156 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  157 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  158 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  159 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  160 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  161 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  162 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  163 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  164 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  165 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  166 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  167 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  168 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  169 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  170 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  171 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  172 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  173 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  174 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  175 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  176 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  177 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  178 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  179 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  180 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  181 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  182 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  183 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  184 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  185 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  186 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  187 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  188 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  189 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  190 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  191 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  192 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  193 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,

/* ?       */  194 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  195 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  196 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  197 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  198 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  199 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  200 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  201 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  202 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  203 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  204 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  205 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  206 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  207 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  208 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  209 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  210 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  211 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  212 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  213 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  214 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  215 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  216 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  217 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  218 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  219 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  220 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  221 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  222 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  223 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  224 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  225 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  226 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  227 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  228 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  229 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  230 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  231 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  232 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  233 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  234 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  235 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  236 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  237 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  238 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  239 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  240 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  241 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  242 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  243 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  244 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  245 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  246 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  247 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  248 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  249 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  250 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  251 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  252 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  253 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  254 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  255 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    0 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    1 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    2 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    3 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    4 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    5 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    6 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    7 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    8 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */    9 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   10 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   11 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   12 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   13 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   14 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   15 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   16 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   17 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   18 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   19 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   20 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   21 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   22 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   23 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   24 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   25 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   26 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   27 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   28 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   29 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   30 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   31 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   32 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   33 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   34 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   35 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   36 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   37 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   38 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   39 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   40 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   41 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   42 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   43 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   44 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   45 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   46 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   47 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   48 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   49 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   50 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   51 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   52 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   53 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   54 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   55 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   56 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   57 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   58 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* F1      */   59 ,      0x3B,    0x3B00,    0x5400,    0x5E00,    0x6800,         0,         0,         0,         0,
/* F2      */   60 ,      0x3C,    0x3C00,    0x5500,    0x5F00,    0x6900,         0,         0,         0,         0,
/* F3      */   61 ,      0x3D,    0x3D00,    0x5600,    0x6000,    0x6A00,         0,         0,         0,         0,
/* F4      */   62 ,      0x3E,    0x3E00,    0x5700,    0x6100,    0x6B00,         0,         0,         0,         0,
/* F5      */   63 ,      0x3F,    0x3F00,    0x5800,    0x6200,    0x6C00,         0,         0,         0,         0,
/* F6      */   64 ,      0x40,    0x4000,    0x5900,    0x6300,    0x6D00,         0,         0,         0,         0,
/* F7      */   65 ,      0x41,    0x4100,    0x5A00,    0x6400,    0x6E00,         0,         0,         0,         0,
/* F8      */   66 ,      0x42,    0x4200,    0x5B00,    0x6500,    0x6F00,         0,         0,         0,         0,
/* F9      */   67 ,      0x43,    0x4300,    0x5C00,    0x6600,    0x7000,         0,         0,         0,         0,
/* F10     */   68 ,      0x44,    0x4400,    0x5D00,    0x6700,    0x7100,         0,         0,         0,         0,
/* ?       */   69 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   70 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* home *X */   71 ,      0x47,    0x4700,    0x4700,    0x7700,    0x9700,         0,         0,         0,         0, //note: X=not on NUMPAD
/* up *X   */   72 ,      0x48,    0x4800,    0x4800,    0x8D00,    0x9800,         0,         0,         0,         0,
/* pgup *X */   73 ,      0x49,    0x4900,    0x4900,    0x8400,    0x9900,         0,         0,         0,         0,
/* ?       */   74 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* left *X */   75 ,      0x4B,    0x4B00,    0x4B00,    0x7300,    0x9B00,         0,         0,         0,         0,
/* (center)*/   76 ,         0,         0,         0,         0,         0,         0,         0,         0,         0, //note: not used
/* right *X*/   77 ,      0x4D,    0x4D00,    0x4D00,    0x7400,    0x9D00,         0,         0,         0,         0,
/* ?       */   78 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* end  *X */   79 ,      0x4F,    0x4F00,    0x4F00,    0x7500,    0x9F00,         0,         0,         0,         0,
/* down *X */   80 ,      0x50,    0x5000,    0x5000,    0x9100,    0xA000,         0,         0,         0,         0,
/* pgdn *X */   81 ,      0x51,    0x5100,    0x5100,    0x7600,    0xA100,         0,         0,         0,         0,
/* ins *X  */   82 ,      0x52,    0x5200,    0x5200,    0x9200,    0xA200,         0,         0,         0,         0,
/* del *X  */   83 ,      0x53,    0x5300,    0x5300,    0x9300,    0xA300,         0,         0,         0,         0,
/* ?       */   84 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   85 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   86 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   87 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   88 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   89 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   90 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   91 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   92 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   93 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   94 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   95 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   96 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   97 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   98 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */   99 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  100 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  101 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  102 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  103 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  104 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  105 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  106 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  107 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  108 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  109 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  110 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  111 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  112 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  113 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  114 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  115 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  116 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  117 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  118 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  119 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  120 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  121 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  122 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  123 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  124 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  125 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  126 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  127 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  128 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  129 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  130 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  131 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  132 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* F11     */  133 ,      0x57,    0x8500,    0x8500,    0x8900,    0x8B00,         0,         0,         0,         0,
/* F12     */  134 ,      0x58,    0x8600,    0x8600,    0x8A00,    0x8C00,         0,         0,         0,         0,
/* ?       */  135 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  136 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  137 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  138 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  139 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  140 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  141 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  142 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  143 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  144 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  145 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  146 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  147 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  148 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  149 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  150 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  151 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  152 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  153 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  154 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  155 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  156 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  157 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  158 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  159 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  160 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  161 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  162 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  163 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  164 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  165 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  166 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  167 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  168 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  169 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  170 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  171 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  172 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  173 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  174 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  175 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  176 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  177 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  178 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  179 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  180 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  181 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  182 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  183 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  184 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  185 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  186 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  187 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  188 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  189 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  190 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  191 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  192 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  193 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  194 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  195 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  196 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  197 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  198 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  199 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  200 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  201 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  202 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  203 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  204 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  205 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  206 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,


/* ?       */  207 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  208 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  209 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  210 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  211 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  212 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  213 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  214 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  215 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  216 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  217 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  218 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  219 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  220 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  221 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  222 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  223 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  224 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  225 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  226 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  227 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  228 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  229 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  230 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  231 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  232 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  233 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  234 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  235 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  236 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  237 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  238 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  239 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  240 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  241 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  242 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  243 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  244 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  245 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  246 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  247 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  248 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  249 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  250 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  251 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  252 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  253 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  254 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
/* ?       */  255 ,         0,         0,         0,         0,         0,         0,         0,         0,         0,
//NUMPAD keys in VK order
/* ins     */  512 ,      0x52,    0x5200,      0x30,    0x9200,         0,      0x30,         0,         0,    0x5200,
/* end     */  513 ,      0x4F,    0x4F00,      0x31,    0x7500,         0,      0x31,         0,         0,    0x4F00,
/* down    */  514 ,      0x50,    0x5000,      0x32,    0x9100,         0,      0x32,         0,         0,    0x5000,
/* pgdn    */  515 ,      0x51,    0x5100,      0x33,    0x7600,         0,      0x33,         0,         0,    0x5100,
/* left    */  516 ,      0x4B,    0x4B00,      0x34,    0x7300,         0,      0x34,         0,         0,    0x4B00,
/* center  */  517 ,      0x4C,    0x4C00,      0x35,    0x8F00,         0,      0x35,         0,         0,    0x4C00,
/* right   */  518 ,      0x4D,    0x4D00,      0x36,    0x7400,         0,      0x36,         0,         0,    0x4D00,
/* home    */  519 ,      0x47,    0x4700,      0x37,    0x7700,         0,      0x37,         0,         0,    0x4700,
/* up      */  520 ,      0x48,    0x4800,      0x38,    0x8D00,         0,      0x38,         0,         0,    0x4800,
/* pgup    */  521 ,      0x49,    0x4900,      0x39,    0x8400,         0,      0x39,         0,         0,    0x4900,
/* del     */  522 ,      0x53,    0x5300,      0x2E,    0x9300,         0,      0x2E,         0,         0,    0x5300,
/* divide  */  523 ,      0x35,      0x2F,      0x2F,    0x9500,    0xA400,      0x2F,         0,         0,      0x2F,
/* mult    */  524 ,      0x37,      0x2A,      0x2A,    0x9600,    0x3700,      0x2A,         0,         0,      0x2A,
/* -d      */  525 ,      0x4A,      0x2D,      0x2D,    0x8E00,    0x4A00,      0x2D,         0,         0,      0x2D,
/* +e      */  526 ,      0x4E,      0x2B,      0x2B,    0x9000,    0x4E00,      0x2B,         0,         0,      0x2B,
/* enter   */  527 ,      0x1C,      0x0D,      0x0D,      0x0A,    0xA600,      0x0D,         0,         0,      0x0D,

/* ctrl    */  000 ,         0,         0,         0,         0,         0,         0,         0,         0,      0x1D,
/* Lshift  */  000 ,      0x2A,         0,         0,         0,         0,         0,         0,         0,         0,
/* Rshift  */  000 ,      0x36,         0,         0,         0,         0,         0,         0,         0,         0,
/* alt     */  000 ,      0x38,         0,         0,         0,         0,         0,         0,         0,         0,
/* caps    */  000 ,      0x3A,         0,         0,         0,         0,         0,         0,         0,         0,
/* num     */  000 ,      0x45,         0,         0,         0,         0,         0,         0,         0,         0,
/* scrl    */  000 ,      0x46,         0,         0,         0,         0,         0,         0,         0,         0,
/* * PrtSc */  000 ,      0x37,      0x2A,         0,      0x10,         0,      0x2A,      0x2A,         0,         0,
};


void keydown(uint32 x);
void keyup(uint32 x);

uint32 unicode_to_cp437(uint32 x){
static int32 i;
for (i=0;i<=255;i++){
if (x==codepage437_to_unicode16[i]) return i;
}
return 0;
}

uint32 *keyheld_buffer=(uint32*)malloc(1);
uint32 *keyheld_bind_buffer=(uint32*)malloc(1);
int32 keyheld_n=0;
int32 keyheld_size=0;

int32 keyheld(uint32 x){
static int32 i;
for (i=0;i<keyheld_n;i++){
if (keyheld_buffer[i]==x) return 1;
}
//check multimapped NUMPAD keys
if ((x>=42)&&(x<=57)){
if ((x>=48)&&(x<=57)) return keyheld(VK+SDLK_KP0+(x-48));//0-9
if (x==46) return keyheld(VK+SDLK_KP_PERIOD);
if (x==47) return keyheld(VK+SDLK_KP_DIVIDE);
if (x==42) return keyheld(VK+SDLK_KP_MULTIPLY);
if (x==45) return keyheld(VK+SDLK_KP_MINUS);
if (x==43) return keyheld(VK+SDLK_KP_PLUS);
}
if (x==13) return keyheld(VK+SDLK_KP_ENTER);
if (x&0xFF00){
static uint32 x2;
x2=(x>>8)&255;
if ((x2>=71)&&(x2<=83)){
if (x2==82) return keyheld(QBK+SDLK_KP0-SDLK_KP0);
if (x2==79) return keyheld(QBK+SDLK_KP1-SDLK_KP0);
if (x2==80) return keyheld(QBK+SDLK_KP2-SDLK_KP0);
if (x2==81) return keyheld(QBK+SDLK_KP3-SDLK_KP0);
if (x2==75) return keyheld(QBK+SDLK_KP4-SDLK_KP0);
if (x2==76) return keyheld(QBK+SDLK_KP5-SDLK_KP0);
if (x2==77) return keyheld(QBK+SDLK_KP6-SDLK_KP0);
if (x2==71) return keyheld(QBK+SDLK_KP7-SDLK_KP0);
if (x2==72) return keyheld(QBK+SDLK_KP8-SDLK_KP0);
if (x2==73) return keyheld(QBK+SDLK_KP9-SDLK_KP0);
if (x2==83) return keyheld(QBK+SDLK_KP_PERIOD-SDLK_KP0);
}
}
return 0;
}


void keyheld_add(uint32 x){
static int32 i; for (i=0;i<keyheld_n;i++){if (keyheld_buffer[i]==x) return;}//already in buffer
if (keyheld_n==keyheld_size){keyheld_size++; keyheld_buffer=(uint32*)realloc(keyheld_buffer,keyheld_size*4); keyheld_bind_buffer=(uint32*)realloc(keyheld_bind_buffer,keyheld_size*4);}//expand buffer
keyheld_buffer[keyheld_n]=x;//add entry
keyheld_bind_buffer[keyheld_n]=bindkey; bindkey=0;//add binded key (0=none)
keyheld_n++;//note: inc. must occur after setting entry (threading reasons)
}
void keyheld_remove(uint32 x){
static int32 i;
for (i=0;i<keyheld_n;i++){
if (keyheld_buffer[i]==x){//exists
memmove(&keyheld_buffer[i],&keyheld_buffer[i+1],(keyheld_n-i-1)*4);
memmove(&keyheld_bind_buffer[i],&keyheld_bind_buffer[i+1],(keyheld_n-i-1)*4);
keyheld_n--;//note: dec. must occur after memmove (threading reasons)
return;
}
}
}
void keyheld_unbind(uint32 x){
static int32 i;
for (i=0;i<keyheld_n;i++){
if (keyheld_bind_buffer[i]==x){//exists
keyup(keyheld_buffer[i]);
return;
}
}
}


void keydown_ascii(uint32 x){
keydown(x);
}
void keydown_unicode(uint32 x){
keydown_glyph=1;
//note: UNICODE 0-127 map directly to ASCII 0-127
if (x<=127){keydown_ascii(x); return;}
//note: some UNICODE values map directly to CP437 values found in the extended ASCII set
static uint32 x2; if (x2=unicode_to_cp437(x)){keydown_ascii(x2); return;}
//note: full width latin characters will be mapped to their normal width equivalents
//Wikipedia note: Range U+FF01FF5E reproduces the characters of ASCII 21 to 7E as fullwidth forms, that is, a fixed width form used in CJK computing. This is useful for typesetting Latin characters in a CJK  environment. U+FF00 does not correspond to a fullwith ASCII 20 (space character), since that role is already fulfilled by U+3000 "ideographic space."
if ((x>=0x0000FF01)&&(x<=0x0000FF5E)){keydown_ascii(x-0x0000FF01+0x21); return;}
if (x==0x3000){keydown_ascii(32); return;}
x|=UC;
keydown(x);
}
void keydown_vk(uint32 x){
keydown(x);
}
void keyup_ascii(uint32 x){
keyup(x);
}
void keyup_unicode(uint32 x){
//note: UNICODE 0-127 map directly to ASCII 0-127
if (x<=127){keyup_ascii(x); return;}
//note: some UNICODE values map directly to CP437 values found in the extended ASCII set
static uint32 x2; if (x2=unicode_to_cp437(x)){keyup_ascii(x2); return;}
//note: full width latin characters will be mapped to their normal width equivalents
//Wikipedia note: Range U+FF01FF5E reproduces the characters of ASCII 21 to 7E as fullwidth forms, that is, a fixed width form used in CJK computing. This is useful for typesetting Latin characters in a CJK  environment. U+FF00 does not correspond to a fullwith ASCII 20 (space character), since that role is already fulfilled by U+3000 "ideographic space."
if ((x>=0x0000FF01)&&(x<=0x0000FF5E)){keyup_ascii(x-0x0000FF01+0x21); return;}
if (x==0x3000){keyup_ascii(32); return;}
x|=UC;
keyup(x);
}
void keyup_vk(uint32 x){
keyup(x);
}


#ifdef QB64_IME

#include <Imm.h> //Microsoft specific include QB64 IME support is dependent on

//---------------------------------------------start of SDL_inputmethod.h--------------------------------------------------------------------
/*
    SDL_inputmethod
    Copyright (C) 2004  Kazunori Itoyanagi

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    ITOYANAGI Kazunori
    itkz@users.sourceforge.jp
*/

#ifndef _SDL_ime_H_
#define _SDL_ime_H_

/*
#include <stdio.h>
#include "SDL_version.h"
#include "SDL.h"
*/
#include "begin_code.h"

#define INPUT_METHOD_MAJOR_VERSION	0
#define INPUT_METHOD_MINOR_VERSION	1
#define INPUT_METHOD_PATCHLEVEL		0

/* This macro can be used to fill a version structure with the compile-time
 * version of the SDL_inputmethod library.
 */
#define INPUTMETHOD_VERSION(X)							\
{									\
	(X)->major = INPUT_METHOD_MAJOR_VERSION;					\
	(X)->minor = INPUT_METHOD_MINOR_VERSION;					\
	(X)->patch = INPUT_METHOD_PATCHLEVEL;					\
}


/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif





/* public ... for Applications */

typedef enum
{
	INPUT_METHOD_MESSAGE_ON,
	INPUT_METHOD_MESSAGE_CHANGE,
	INPUT_METHOD_MESSAGE_RESULT,
	INPUT_METHOD_MESSAGE_OFF,
	INPUT_METHOD_MESSAGE_CHAR,
	INPUT_METHOD_MESSAGE_NO_EXIST
} InputMethod_Message;

typedef enum
{
	INPUT_METHOD_SUCCESS = 0,
	INPUT_METHOD_ERROR_ALREADY_INIT,
	INPUT_METHOD_ERROR_ALLOCATED_MEMORY,
	INPUT_METHOD_ERROR_NO_STRING,
	INPUT_METHOD_ERROR_UNKNOWN_MESSAGE,
	INPUT_METHOD_ERROR_SDL_NO_INIT,
	INPUT_METHOD_ERROR_NO_INIT,
	INPUT_METHOD_ERROR_SYSTEM_SPECIFIC,
	INPUT_METHOD_ERROR_ALREADY_VALIDATED,
	INPUT_METHOD_ERROR_ALREADY_INVALIDATED,
	INPUT_METHOD_ERROR_NOT_AVAILABLE,
	INPUT_METHOD_ERROR_INVALID_BOOTSTRAP
} InputMethod_Result;

/*
typedef enum
{
	INPUT_METHOD_STATUS_ON,
	INPUT_METHOD_STATUS_OFF
} InputMethod_Status;
*/

extern DECLSPEC InputMethod_Result SDLCALL InputMethod_Init(void);

extern DECLSPEC int SDLCALL InputMethod_GetEventNumber(void);
extern DECLSPEC void SDLCALL InputMethod_MoveNextEvent(void);
extern DECLSPEC InputMethod_Message SDLCALL InputMethod_GetCurrentMessage(void);
extern DECLSPEC Uint16 * SDLCALL InputMethod_GetCurrentEditingString(void);
extern DECLSPEC Uint16 SDLCALL InputMethod_GetCurrentChar(void);
extern DECLSPEC int SDLCALL InputMethod_GetCurrentCursorPosition(void);
extern DECLSPEC int SDLCALL InputMethod_GetCurrentCompositionPosition(void);
extern DECLSPEC int SDLCALL InputMethod_GetCurrentCompositionLength(void);
extern DECLSPEC void SDLCALL InputMethod_Reset(void);
extern DECLSPEC InputMethod_Result SDLCALL InputMethod_Validate(void);
extern DECLSPEC InputMethod_Result SDLCALL InputMethod_Invalidate(void);
extern DECLSPEC Uint16 * SDLCALL InputMethod_GetInputMethodName(void);
extern DECLSPEC void SDLCALL InputMethod_Quit(void);





/* protected? ... for add other Input Methods */

#define INPUT_METHOD_NAME_STRING_LENGTH 256

typedef struct _SDL_InputMethod
{
	SDL_bool (*available)(void);
	InputMethod_Result (*init)(void);
	void (*quit)(void);
	void (*reset)(void);
	InputMethod_Result (*validate)(void);
	InputMethod_Result (*invalidate)(void);
	
	/*
	  Return value is Input Method name in Unicode.
	  You have to set string to `InputMethodName' and
	  return this in get_name(),
	  because get_name() is a possible
	  to call without init() and quit(),
	  and so even if use malloc() to string, no time to free().
	*/
	Uint16 *(*get_name)(void);
	Uint16 InputMethodName[INPUT_METHOD_NAME_STRING_LENGTH];
/*
	InputMethod_Status (*get_status)(void);
	void (*set_status)(InputMethod_Status status);
*/
} SDL_InputMethod;

extern DECLSPEC InputMethod_Result SDLCALL InputMethod_PostEvent(
	InputMethod_Message message,
	Uint16 *editingString,
	int cursorPosition,
	int compositionPosition,
	int compositionLength);
extern DECLSPEC InputMethod_Result SDLCALL InputMethod_InitFromOther(
	SDL_InputMethod inputMethod);





/* private */

#ifdef ENABLE_WIN32
extern SDL_InputMethod InputMethod_win32;
#endif
#ifdef ENABLE_XIM
extern SDL_InputMethod InputMethod_xim;
#endif
/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"
#endif
//---------------------------------------------end of SDL_inputmethod.h--------------------------------------------------------------------

//---------------------------------------------start of SDL_inputmethod.c--------------------------------------------------------------------
/*
    SDL_inputmethod
    Copyright (C) 2004  Kazunori Itoyanagi

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    ITOYANAGI Kazunori
    itkz@users.sourceforge.jp
*/

static SDL_InputMethod *InputMethod_BootStrap[] =
{
#ifdef ENABLE_WIN32
	&InputMethod_win32,
#endif
#ifdef ENABLE_XIM
	&InputMethod_xim,
#endif
	NULL
};


typedef struct _InputMethod_Event
{
	InputMethod_Message message;
	Uint16 *editingString;
	int cursorPosition;
	struct _InputMethod_Event *next;
	int compositionPosition;
	int compositionLength;
	Uint16 character;
} InputMethod_Event;


static SDL_InputMethod *InputMethod_Use = NULL;
SDL_EventFilter OriginalFilterSDL = NULL;

static InputMethod_Event *InputMethod_EventQue = NULL;
static int InputMethod_EventQueNumber = 0;
static int32 IsOriginalInputMethod = SDL_FALSE;
static int32 IsInited = SDL_FALSE;
static int32 IsValid;
static int32 IsEnableUNICODEOld;

static int InputMethod_GetUnicodeStringLength(Uint16 *unicodeString);
static int InputMethod_SDLEventFilter(const SDL_Event *e);
static void InputMethod_SetEventFilter(void);
static void InputMethod_RestoreEventFilter(void);
static InputMethod_Result InputMethod_PostCharEvent(Uint16 unicode);


void InputMethod_SetEventFilter(void)
{
	IsEnableUNICODEOld = SDL_EnableUNICODE(1);
	//OriginalFilterSDL = SDL_GetEventFilter();
	//SDL_SetEventFilter(InputMethod_SDLEventFilter);
}


void InputMethod_RestoreEventFilter(void)
{
	SDL_EnableUNICODE(IsEnableUNICODEOld);
	//SDL_SetEventFilter(OriginalFilterSDL);
	//OriginalFilterSDL = NULL;
}


InputMethod_Result InputMethod_Init(void)
{
	int i;
	InputMethod_Result result;
	
	if (IsInited == 1) {
		return INPUT_METHOD_ERROR_ALREADY_INIT;
	}
	

	
	for (i = 0; InputMethod_BootStrap[i]; i++) {
		if (InputMethod_BootStrap[i]->available()) {
			
			
			InputMethod_Use = InputMethod_BootStrap[i];
			break;
			
			

		}
	}
	


	InputMethod_SetEventFilter();
	if (InputMethod_Use != NULL) {
		result = InputMethod_Use->init();
		if (result != INPUT_METHOD_SUCCESS) {
			InputMethod_Use->quit();
			InputMethod_Use = NULL;
			InputMethod_RestoreEventFilter();
			return result;
		}
	}
	InputMethod_EventQue = NULL;
	InputMethod_EventQueNumber = 0;
	IsOriginalInputMethod = SDL_FALSE;
	IsValid = 1;
	IsInited = 1;
	
	return INPUT_METHOD_SUCCESS;
}


void InputMethod_Quit(void)
{
	InputMethod_Event *event;
	InputMethod_Event *next;
	
	if (InputMethod_Use != NULL) {
		if (InputMethod_Use->quit != NULL) {
			InputMethod_Use->quit();
		}
		if (IsOriginalInputMethod) {
			free(InputMethod_Use);
		}
		InputMethod_Use = NULL;
	}
	
	event = InputMethod_EventQue;
	while (event != NULL) {
		next = event->next;
		if (event->editingString != NULL) {
			free(event->editingString);
		}
		free(event);
		event = next;
	}
	InputMethod_EventQue = NULL;
	InputMethod_EventQueNumber = 0;
	IsOriginalInputMethod = SDL_FALSE;
	IsInited = SDL_FALSE;
	InputMethod_RestoreEventFilter();
}


int InputMethod_BootstrapNum(void)
{
	int count;
	
	count = 0;
	while (InputMethod_BootStrap[count]) {
		count++;
	}
	
	return count;
}


int32 InputMethod_BootstrapIsAvailable(int num) {
	if (0 <= num && num < InputMethod_BootstrapNum()) {
		return InputMethod_BootStrap[num]->available();
	} else {
		return SDL_FALSE;
	}
}


Uint16 *InputMethod_BootstrapGetInputMethodName(int num) {
	if (0 <= num && num < InputMethod_BootstrapNum()) {
		if (InputMethod_BootStrap[num]->available()) {
			return InputMethod_BootStrap[num]->get_name();
		}
	}
	return NULL;
}


InputMethod_Result InputMethod_InitFromBootstrap(int num)
{
	InputMethod_Result result;
	
	if (IsInited == 1) {
		return INPUT_METHOD_ERROR_ALREADY_INIT;
	}
	
	if (0 <= num && num < InputMethod_BootstrapNum()) {
		if (InputMethod_BootStrap[num]->available()) {
			InputMethod_Use = InputMethod_BootStrap[num];
		} else {
			return INPUT_METHOD_ERROR_NOT_AVAILABLE;
		}
	} else {
		return INPUT_METHOD_ERROR_INVALID_BOOTSTRAP;
	}
	
	result = InputMethod_Use->init();
	if (result != INPUT_METHOD_SUCCESS) {
		InputMethod_Use->quit();
		InputMethod_Use = NULL;
		return result;
	}
	InputMethod_EventQue = NULL;
	InputMethod_EventQueNumber = 0;
	IsOriginalInputMethod = SDL_FALSE;
	IsValid = 1;
	IsInited = 1;
	
	return INPUT_METHOD_SUCCESS;
}


void InputMethod_Reset(void)
{
	InputMethod_Event *event;
	InputMethod_Event *next;
	
	event = InputMethod_EventQue;
	while (event != NULL) {
		next = event->next;
		if (event->editingString != NULL) {
			free(event->editingString);
		}
		free(event);
		event = next;
	}
	InputMethod_EventQue = NULL;
	InputMethod_EventQueNumber = 0;
	
	if (InputMethod_Use != NULL) {
		if (InputMethod_Use->reset != NULL) {
			InputMethod_Use->reset();
		}
	}
}


InputMethod_Result InputMethod_Validate(void)
{
	InputMethod_Result result;
	
	if (InputMethod_Use == NULL) {
		SDL_SetError("Input method is no init");
		return INPUT_METHOD_ERROR_NO_INIT;
	}
	
	if (IsValid == 1) {
		return INPUT_METHOD_ERROR_ALREADY_VALIDATED;
	}
	
	if (InputMethod_Use != NULL) {
		if (InputMethod_Use->validate != NULL) {
			result = InputMethod_Use->validate();

			if (result == INPUT_METHOD_SUCCESS) {
				IsValid = 1;
			}
			return result;
		}
	}
	
	SDL_SetError("Input method is no init");
	return INPUT_METHOD_ERROR_NO_INIT;
}


InputMethod_Result InputMethod_Invalidate(void)
{
	InputMethod_Result result;
	
	if (InputMethod_Use == NULL) {
		SDL_SetError("Input method is no init");
		return INPUT_METHOD_ERROR_NO_INIT;
	}
	
	if (IsValid == SDL_FALSE) {
		return INPUT_METHOD_ERROR_ALREADY_INVALIDATED;
	}
	
	if (InputMethod_Use != NULL) {
		if (InputMethod_Use->invalidate !=NULL) {
			result = InputMethod_Use->invalidate();
			if (result == INPUT_METHOD_SUCCESS) {
				IsValid = SDL_FALSE;
			}
			return result;
		}
	}
	
	SDL_SetError("Input method is no init");
	return INPUT_METHOD_ERROR_NO_INIT;
}


int InputMethod_GetEventNumber(void)
{
	return InputMethod_EventQueNumber;
}


void InputMethod_MoveNextEvent(void)
{
	InputMethod_Event *next;
	
	if (InputMethod_EventQueNumber > 0) {
		if (InputMethod_EventQue->editingString != NULL) {
			free(InputMethod_EventQue->editingString);
		}
		
		if (InputMethod_EventQue != NULL) {
			next = InputMethod_EventQue->next;
			free(InputMethod_EventQue);
			InputMethod_EventQue = next;
		}
		
		InputMethod_EventQueNumber--;
	}
}


InputMethod_Message InputMethod_GetCurrentMessage(void)
{
	if (InputMethod_EventQue == NULL) {
		SDL_SetError("Message is not found");
		return INPUT_METHOD_MESSAGE_NO_EXIST;
	} else {
		return InputMethod_EventQue->message;
	}
}


Uint16 *InputMethod_GetCurrentEditingString(void)
{
	if (InputMethod_EventQue == NULL) {
		return NULL;
	}
	
	return InputMethod_EventQue->editingString;
}


Uint16 InputMethod_GetCurrentChar(void)
{
	if (InputMethod_EventQue == NULL) {
		return 0x0000;
	}
	
	return InputMethod_EventQue->character;
}


int InputMethod_GetCurrentCursorPosition(void)
{
	if (InputMethod_EventQue == NULL) {
		return 0;
	}
	
	return InputMethod_EventQue->cursorPosition;
}


int InputMethod_GetCurrentCompositionPosition(void)
{
	if (InputMethod_EventQue == NULL) {
		return 0;
	}
	
	return InputMethod_EventQue->compositionPosition;
}


int InputMethod_GetCurrentCompositionLength(void)
{
	if (InputMethod_EventQue == NULL) {
		return 0;
	}
	
	return InputMethod_EventQue->compositionLength;
}


Uint16 *InputMethod_GetInputMethodName(void)
{
	if (InputMethod_Use != NULL) {
		if (InputMethod_Use->get_name != NULL) {
			return InputMethod_Use->get_name();
		}
	}
	return NULL;
}


int InputMethod_GetUnicodeStringLength(Uint16 *unicodeString)
{
	int i;
	
	if (unicodeString == NULL) {
		return 0;
	}
	
	i = 0;
	while (unicodeString[i]) {
		i++;
	}
	
	return i;
}


InputMethod_Result InputMethod_PostEvent(
	InputMethod_Message message,
	Uint16 *editingString,
	int cursorPosition,
	int compositionPosition,
	int compositionLength)
{

	//showvalue(99998);
	InputMethod_Event *event;
	InputMethod_Event *lastEvent;
	int size;
	
	event = (InputMethod_Event*)malloc(sizeof(InputMethod_Event));
	if (event == NULL) {
		SDL_SetError("Allocate memory failed");
		return INPUT_METHOD_ERROR_ALLOCATED_MEMORY;
	}
	event->next = NULL;
	
	switch (message) {
	case INPUT_METHOD_MESSAGE_ON:
	case INPUT_METHOD_MESSAGE_OFF:
		event->message = message;
		event->editingString = NULL;
		event->cursorPosition = 0;
		event->compositionPosition = 0;
		event->compositionLength = 0;
		event->character = 0x0000;
		break;
	case INPUT_METHOD_MESSAGE_RESULT:
		if (editingString == NULL) {
			free(event);
			SDL_SetError(
				"post INPUT_METHOD_MESSAGE_RESULT: "
				"Editing string is NULL");
			return INPUT_METHOD_ERROR_NO_STRING;
		}
		size =
			InputMethod_GetUnicodeStringLength(editingString) * sizeof(Uint16) +
			sizeof(Uint16);
		event->editingString = (Uint16*)malloc(size);
		if (event->editingString == NULL) {
			free(event);
			SDL_SetError("Allocate memory failed");
			return INPUT_METHOD_ERROR_ALLOCATED_MEMORY;
		}
		memcpy(event->editingString, editingString, size);
		event->message = message;
		event->cursorPosition = 0;
		event->compositionPosition = 0;
		event->compositionLength = 0;
		event->character = 0x0000;
		break;
	case INPUT_METHOD_MESSAGE_CHANGE:
		if (editingString == NULL) {
			free(event);
			SDL_SetError(
				"post INPUT_METHOD_MESSAGE_CHANGE: "
				"Editing string is NULL");
			return INPUT_METHOD_ERROR_NO_STRING;
		}
		size =
			InputMethod_GetUnicodeStringLength(editingString) * sizeof(Uint16) +
			sizeof(Uint16);
		event->editingString = (Uint16*)malloc(size);
		if (event->editingString == NULL) {
			free(event);
			return INPUT_METHOD_ERROR_ALLOCATED_MEMORY;
		}
		memcpy(event->editingString, editingString, size);
		event->message = message;
		event->cursorPosition = cursorPosition;
		event->compositionPosition = compositionPosition;
		event->compositionLength = compositionLength;
		event->character = 0x0000;
		break;
	default:
		free(event);
		SDL_SetError("post message: unknown message");
		return INPUT_METHOD_ERROR_UNKNOWN_MESSAGE;
	}
	
	if (InputMethod_EventQue == NULL) {
		InputMethod_EventQue = event;
	} else {
		lastEvent = InputMethod_EventQue;
		while (lastEvent->next != NULL) {
			lastEvent = lastEvent->next;
		}
		lastEvent->next = event;
	}

	//showvalue(99999);
	InputMethod_EventQueNumber++;
	
	return INPUT_METHOD_SUCCESS;
}

//IMPORTANT: THIS FUNCTION IS NOT USED BUT HAS BEEN KEPT FOR REFERENCE PURPOSES
InputMethod_Result InputMethod_PostCharEvent(Uint16 unicode)
{
//IMPORTANT: THIS FUNCTION IS NOT USED BUT HAS BEEN KEPT FOR REFERENCE PURPOSES

	InputMethod_Event *event;
	InputMethod_Event *lastEvent;
	
	event = (InputMethod_Event*)malloc(sizeof(InputMethod_Event));
	if (event == NULL) {
		SDL_SetError("Allocate memory failed");
		return INPUT_METHOD_ERROR_ALLOCATED_MEMORY;
	}
	event->next = NULL;
	
	event->message = INPUT_METHOD_MESSAGE_CHAR;
	
	//exit(unicode);
	event->character = unicode;
	event->editingString = NULL;
	event->cursorPosition = 0;
	event->compositionPosition = 0;
	event->compositionLength = 0;
	
	if (InputMethod_EventQue == NULL) {
		InputMethod_EventQue = event;
	} else {
		lastEvent = InputMethod_EventQue;
		while (lastEvent->next != NULL) {
			lastEvent = lastEvent->next;
		}
		lastEvent->next = event;
	}
	InputMethod_EventQueNumber++;
	
	return INPUT_METHOD_SUCCESS;
}


InputMethod_Result InputMethod_InitFromOther(
	SDL_InputMethod inputMethod)
{
	InputMethod_Result result;
	
	if (IsInited == 1) {
		SDL_SetError("Input method is already inited");
		return INPUT_METHOD_ERROR_ALREADY_INIT;
	}
	
	if (inputMethod.available() == SDL_FALSE) {
		SDL_SetError("Bootstrap is invalid");
		return INPUT_METHOD_ERROR_INVALID_BOOTSTRAP;
	} else {
		InputMethod_SetEventFilter();
		result = inputMethod.init();
		if (result != INPUT_METHOD_SUCCESS) {
			InputMethod_RestoreEventFilter();
			return result;
		}
	}
	
	InputMethod_Use = (SDL_InputMethod*)malloc(sizeof(SDL_InputMethod));
	if (InputMethod_Use == NULL) {
		inputMethod.quit();
		SDL_SetError("Allocate memory failed");
		return INPUT_METHOD_ERROR_ALLOCATED_MEMORY;
	}
	
	memcpy(InputMethod_Use, &inputMethod, sizeof(SDL_InputMethod));
	
	InputMethod_EventQue = NULL;
	InputMethod_EventQueNumber = 0;
	IsOriginalInputMethod = 1;
	IsValid = 1;
	IsInited = 1;
	
	return INPUT_METHOD_SUCCESS;
}


//IMPORTANT: THIS FUNCTION IS NOT USED BUT HAS BEEN KEPT FOR REFERENCE PURPOSES
int InputMethod_SDLEventFilter(const SDL_Event *e)
{
//IMPORTANT: THIS FUNCTION IS NOT USED BUT HAS BEEN KEPT FOR REFERENCE PURPOSES

	Uint8 *keys;
	SDL_keysym keysym;
	char ch;
	
	if (IsValid == 1) {
		keys = SDL_GetKeyState(NULL);
		keys[SDLK_UNKNOWN] = SDL_RELEASED;
		
		switch (e->type) {
		case SDL_KEYDOWN:
			keysym = ((SDL_KeyboardEvent*)e)->keysym;
			if ( (keysym.unicode & 0xFF80) == 0 ) {
				ch = keysym.unicode & 0x007F;
				if (isprint(ch)) {
					InputMethod_PostCharEvent(keysym.unicode);
					keys[keysym.sym] = SDL_RELEASED;
					return 0;
				}
			}
			break;
		default:
			break;
		}
	}
	
	if (OriginalFilterSDL != NULL) {
		return (*OriginalFilterSDL)(e);
	}
	return 1;
}
//---------------------------------------------end of SDL_inputmethod.c--------------------------------------------------------------------

//---------------------------------------------start of SDL_inputmethod for Windows--------------------------------------------------------------------
/*
    SDL_inputmethod
    Copyright (C) 2004  Kazunori Itoyanagi

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    ITOYANAGI Kazunori
    itkz@users.sourceforge.jp
*/

/*
#include <windows.h>
#include <winuser.h>
#include "SDL.h"
*/
#include "SDL_syswm.h"
/*
#include "SDL_inputmethod.h"
*/

static SDL_bool InputMethod_AvailableWin32(void);
static InputMethod_Result InputMethod_InitWin32(void);
static void InputMethod_QuitWin32(void);
static void InputMethod_ResetWin32(void);
static InputMethod_Result InputMethod_ValidateWin32(void);
static InputMethod_Result InputMethod_InvalidateWin32(void);
static Uint16 *InputMethod_GetNameWin32(void);


SDL_InputMethod InputMethod_win32 =
{
	InputMethod_AvailableWin32,
	InputMethod_InitWin32,
	InputMethod_QuitWin32,
	InputMethod_ResetWin32,
	InputMethod_ValidateWin32,
	InputMethod_InvalidateWin32,
	InputMethod_GetNameWin32
};


static WNDPROC OrigEditProc;
//static int32 IsValid;
static BOOL SaveOpenStatus;

#define SYSTEM_SPECIFIC_NAME_STRING " on IMM"

static LRESULT CALLBACK WrapProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static int CompositionProcessing(HWND hWnd, LPARAM lParam);
static int ResultProcessing(HWND hWnd);
static int SetOpenStatusProcessing(HWND hWnd);
static int EndCompositionProcessing(HWND hWnd);
static int GetUnicodePosition(char *orgString, int cursorPosition);



int GetUnicodePosition(char *orgString, int cursorPosition)
{
	char *splitString;
	int unicodePosition;
	
	splitString = (char *)malloc(cursorPosition + 1);
	if (splitString == NULL) {
		return 0;
	}
	strncpy(splitString, orgString, cursorPosition);
	splitString[cursorPosition] = '\0';
	
	/* (char_num - 0x0000) * char_size */
	unicodePosition =
		MultiByteToWideChar(CP_ACP, 0, splitString, -1, NULL, 0) - 1;
	
	free(splitString);
	
	return unicodePosition;
}


int CompositionProcessing(HWND hWnd, LPARAM lParam)
{
	char *string;
	Uint16 *unicodeString;
	HIMC hImc;
	int needSize;
	int orgCursorPosition;
	int cursorPosition;
	int orgCompositionPosition;
	int compositionPosition;
	int orgCompositionLength;
	int compositionLength;
	DWORD *clauses;
	BYTE *attrs;
	int i;
	int clausesNum;

	//exit(898);



	hImc = ImmGetContext(hWnd);
	//InputMethod_PostEvent
//note: ImmGetCompositionString changed to ImmGetCompositionStringW to ensure UNICODE compatible method is used
	needSize = ImmGetCompositionStringW(hImc, GCS_COMPSTR, NULL, 0) + 1;
	string = (char*)calloc(needSize+2,1);
	if (string == NULL){
		ImmReleaseContext(hWnd, hImc);
		return -1;
	}
	ImmGetCompositionStringW(hImc, GCS_COMPSTR, string, needSize);
	//string[needSize - 1] = '\0';
//	fwrite(string, strlen(string), 1, stdout);
//	fputc('\n', stdout);

/*
	needSize =
		MultiByteToWideChar(CP_ACP, 0, string, -1, NULL, 0)
		* sizeof(Uint16);
	unicodeString = (Uint16*)calloc(needSize, 1);
	if (unicodeString == NULL) {
		ImmReleaseContext(hWnd, hImc);
		free(string);
		return -1;
	}
	MultiByteToWideChar(CP_ACP, 0, string, -1, (LPWSTR)unicodeString, needSize);
*/
unicodeString=(Uint16*)string;

	orgCompositionPosition = 0;
	orgCompositionLength = 0;
	compositionPosition = 0;
	compositionLength = 0;
	if ( (lParam & GCS_COMPCLAUSE) && (lParam & GCS_COMPATTR) ) {

		needSize = ImmGetCompositionStringW(hImc, GCS_COMPCLAUSE, NULL, 0);
		clauses = (DWORD*)calloc(needSize+4,1);
		ImmGetCompositionStringW(hImc, GCS_COMPCLAUSE, clauses, needSize);
		/* (clauses list - total) - 1 */
		clausesNum = needSize / sizeof(DWORD) - 1;

		needSize = ImmGetCompositionStringW(hImc, GCS_COMPATTR, NULL, 0);
		attrs = (BYTE*)calloc(needSize+2,1);
		ImmGetCompositionStringW(hImc, GCS_COMPATTR, attrs, needSize);
		
		for (i = 0; i < clausesNum; i++) {
			if (
				attrs[clauses[i]] == ATTR_TARGET_CONVERTED ||
				attrs[clauses[i]] == ATTR_TARGET_NOTCONVERTED
			) {
				orgCompositionPosition = clauses[i];
				orgCompositionLength = clauses[i + 1] - clauses[i];
				break;
			}
		}
		
		free(clauses);
		free(attrs);
	}
	
	orgCursorPosition =
		ImmGetCompositionStringW(hImc, GCS_CURSORPOS, NULL, 0);
	cursorPosition = 0;
	if (orgCursorPosition > 0) {
		cursorPosition = GetUnicodePosition(string, orgCursorPosition);
	}
	if (orgCompositionPosition > 0) {
		compositionPosition = GetUnicodePosition(string, orgCompositionPosition);
	}
	if (orgCompositionLength > 0) {
		compositionLength =
			GetUnicodePosition(
				string, orgCompositionPosition + orgCompositionLength)
			- compositionPosition;
	}
	InputMethod_PostEvent(
		INPUT_METHOD_MESSAGE_CHANGE,
		unicodeString,
		cursorPosition, compositionPosition, compositionLength);
	//free(unicodeString);
	free(string);
	
	ImmReleaseContext(hWnd, hImc);
	
	return 0;
}


int ResultProcessing(HWND hWnd)
{
	char *string;
	Uint16 *unicodeString;
	int needSize;
	HIMC hImc;
	
	hImc = ImmGetContext(hWnd);
	
//note: ImmGetCompositionString changed to ImmGetCompositionStringW to ensure UNICODE compatible method is used
	needSize = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, NULL, 0) + 1;
	string = (char*)calloc(needSize+2,1);
	if (string == NULL) {
		ImmReleaseContext(hWnd, hImc);
		return -1;
	}
	ImmGetCompositionStringW(hImc, GCS_RESULTSTR, string, needSize);

//note: commented to avoid garbling UNICODE16 string returned by ImmGetCompositionStringW
/*
	needSize =		
MultiByteToWideChar(CP_ACP, 0, string, -1, NULL, 0)
		* sizeof(Uint16);
	unicodeString = (Uint16*)malloc(needSize);
	if (unicodeString == NULL) {
		ImmReleaseContext(hWnd, hImc);
		free(string);
		return -1;
	}

	MultiByteToWideChar(CP_ACP, 0, string, -1, (LPWSTR)unicodeString, needSize);

*/

	InputMethod_PostEvent(
		INPUT_METHOD_MESSAGE_RESULT,
		(Uint16*)string,
		0, 0, 0);
	//free(unicodeString);
	free(string);
	
	ImmReleaseContext(hWnd, hImc);
	
	return 0;
}


int EndCompositionProcessing(HWND hWnd)
{
	HIMC hImc;
	Uint16 dummy;
	
	hImc = ImmGetContext(hWnd);
	
	if (ImmGetCompositionString(hImc, GCS_RESULTSTR, NULL, 0) == 0) {
		dummy = 0x0000;
		InputMethod_PostEvent(
			INPUT_METHOD_MESSAGE_CHANGE,
			&dummy,
			0, 0, 0);
	}
	
	ImmReleaseContext(hWnd, hImc);
	
	return 0;
}


int SetOpenStatusProcessing(HWND hWnd)
{
	HIMC hImc;
	
	hImc = ImmGetContext(hWnd);
	
	if (ImmGetOpenStatus(hImc)) {
		InputMethod_PostEvent(INPUT_METHOD_MESSAGE_ON, NULL, 0, 0, 0);
	} else {
		InputMethod_PostEvent(INPUT_METHOD_MESSAGE_OFF, NULL, 0, 0, 0);
	}
	
	ImmReleaseContext(hWnd, hImc);
	
	return 0;
}


static int32 deadchar_use=0;
static int32 deadchar_code=0;
static int32 ignore=0;

LRESULT CALLBACK WrapProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	MSG m;
	HIMC hImc;
	
	if (IsValid == SDL_FALSE) {
		switch (msg) {
		case WM_IME_NOTIFY:
			if (wParam == IMN_SETOPENSTATUS) {
				hImc = ImmGetContext(hWnd);
				ImmSetOpenStatus(hImc, FALSE);
				ImmReleaseContext(hWnd, hImc);
			}
			break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:


			m.hwnd = hWnd;
			m.message = msg;
			m.wParam = wParam;
			m.lParam = lParam;
			m.time = 0;
			TranslateMessage(&m);
			break;
		default:
			break;
		}
		return CallWindowProc(OrigEditProc, hWnd, msg, wParam, lParam);
	}




	switch (msg) {
	case WM_IME_COMPOSITION:
		if (lParam & GCS_RESULTSTR) {
			ResultProcessing(hWnd);
		}
		if (
			(lParam & GCS_COMPSTR) ||
			((lParam & GCS_COMPCLAUSE) && (lParam & GCS_COMPATTR))
		) {
			CompositionProcessing(hWnd, lParam);
		}
		return 0;
	case WM_IME_STARTCOMPOSITION:
qb64_ime_reading=1;
		return 0;
	case WM_IME_ENDCOMPOSITION:
		EndCompositionProcessing(hWnd);
qb64_ime_reading=2;
		return 0;
	case WM_IME_NOTIFY:
		switch (wParam) {
		case IMN_SETOPENSTATUS:
			SetOpenStatusProcessing(hWnd);
			break;
		case IMN_OPENSTATUSWINDOW:
		case IMN_CLOSESTATUSWINDOW:
		case IMN_OPENCANDIDATE:
		case IMN_CHANGECANDIDATE:
		case IMN_CLOSECANDIDATE:
			return 0;
		default:
			break;
		}
		break;
	case WM_IME_SETCONTEXT:
		lParam &= ~ISC_SHOWUIALL;
		break;
	case WM_SYSKEYDOWN:
/*
//note: translating SYSKEYDOWN messages cause windows to beep on ALT+? combinations
		m.hwnd = hWnd;
		m.message = msg;
		m.wParam = wParam;
		m.lParam = lParam;
		m.time = 0;
		TranslateMessage(&m);
*/
break;

	case WM_KEYUP:
/*
MEDIA_PLAY_PAUSE 0x2200
MEDIA_STOP 0x2400
MEDIA_NEXT_TRACK 0x1900
MEDIA_PREV_TRACK 0x1000
*/
if (wParam==VK_MEDIA_PLAY_PAUSE){keyup_vk(0x2200);return 0;}
if (wParam==VK_MEDIA_STOP){keyup_vk(0x2400);return 0;}
if (wParam==VK_MEDIA_NEXT_TRACK){keyup_vk(0x1900);return 0;}
if (wParam==VK_MEDIA_PREV_TRACK){keyup_vk(0x1000);return 0;}
//note: volume keys are ignored by QB64
if (wParam==VK_VOLUME_MUTE) return 0;
if (wParam==VK_VOLUME_DOWN) return 0;
if (wParam==VK_VOLUME_UP) return 0;
//note: On early keyboards without a Pause key (before the introduction of 101-key keyboards) the Pause function was assigned to Ctrl+NumLock, and the Break function to Ctrl+ScrLock; these key-combinations still work with most programs, even on modern PCs with modern keyboards.
if (wParam==VK_PAUSE){keyup_vk(VK+SDLK_PAUSE);return 0;}
if (wParam==VK_CANCEL){keyup_vk(VK+SDLK_BREAK);return 0;}
/*
m.hwnd = hWnd;
m.message = msg;
m.wParam = wParam;
m.lParam = lParam;
m.time = 1000;
TranslateMessage(&m);
*/
break;

case WM_CHAR:
if (deadchar_use){
deadchar_use=0;
if (wParam==32){
keydown_unicode(deadchar_code);
keyup_unicode(deadchar_code);
return 0;//don't allow message to be sent on to SDL
}
static long i,d,a,r;
i=0;
while (d=deadchar_lookup[i++]){
a=deadchar_lookup[i++];
r=deadchar_lookup[i++];
if (deadchar_code==d){ if (wParam==a){
keydown_unicode(r);
keyup_unicode(r);
return 0;//don't allow message to be sent on to SDL
}}
}//while
keydown_unicode(deadchar_code);
keyup_unicode(deadchar_code);
keydown_unicode(wParam);
keyup_unicode(wParam);
return 0;//don't allow message to be sent on to SDL
}//dead_char
break;

case WM_DEADCHAR:
//showvalue(wParam);
if (deadchar_use){//double dead-char press
deadchar_use=0;
keydown_unicode(deadchar_code);
keyup_unicode(deadchar_code);
keydown_unicode(wParam);
keyup_unicode(wParam);
return 0;//don't allow message to be sent on to SDL
}
deadchar_use=1;
deadchar_code=wParam;
return 0;//don't allow message to be sent on to SDL
break;

	case WM_KEYDOWN:


ignore=0;
if (deadchar_use) ignore=1;

//showvalue(wParam);

/*
MEDIA_PLAY_PAUSE 0x2200
MEDIA_STOP 0x2400
MEDIA_NEXT_TRACK 0x1900
MEDIA_PREV_TRACK 0x1000
*/
if (wParam==VK_MEDIA_PLAY_PAUSE){keydown_vk(0x2200);return 0;}
if (wParam==VK_MEDIA_STOP){keydown_vk(0x2400);return 0;}
if (wParam==VK_MEDIA_NEXT_TRACK){keydown_vk(0x1900);return 0;}
if (wParam==VK_MEDIA_PREV_TRACK){keydown_vk(0x1000);return 0;}
//note: volume keys are ignored by QB64
if (wParam==VK_VOLUME_MUTE) return 0;
if (wParam==VK_VOLUME_DOWN) return 0;
if (wParam==VK_VOLUME_UP) return 0;
//note: On early keyboards without a Pause key (before the introduction of 101-key keyboards) the Pause function was assigned to Ctrl+NumLock, and the Break function to Ctrl+ScrLock; these key-combinations still work with most programs, even on modern PCs with modern keyboards.
if (wParam==VK_PAUSE){keydown_vk(VK+SDLK_PAUSE);return 0;}
if (wParam==VK_CANCEL){keydown_vk(VK+SDLK_BREAK);return 0;}

//note: IME input (eg. hiragana) will not work correctly without calling TranslateMessage
m.hwnd = hWnd;
m.message = msg;
m.wParam = wParam;
m.lParam = lParam;
m.time = 1000;
TranslateMessage(&m);

if (ignore) return 0;//don't allow message to be sent on to SDL

break;

	default:
		break;
	}
	
	return CallWindowProc(OrigEditProc, hWnd, msg, wParam, lParam);
}


SDL_bool InputMethod_AvailableWin32(void)
{
	SDL_SysWMinfo info;
	HWND hWnd;
	HIMC hImc;
	
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	hWnd = info.window;
	hImc = ImmGetContext(hWnd);
	if (hImc) {
		ImmReleaseContext(hWnd, hImc);
		return SDL_TRUE;
	} else {
		return SDL_FALSE;
	}
}


InputMethod_Result InputMethod_InitWin32(void)
{

	SDL_SysWMinfo info;
	HWND hWnd;
	
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	hWnd = info.window;
	OrigEditProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WrapProc);
	IsValid = 1;
	
	return INPUT_METHOD_SUCCESS;
}


void InputMethod_QuitWin32(void)
{
	SDL_SysWMinfo info;
	HWND hWnd;
	
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	hWnd = info.window;
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WrapProc);
}


void InputMethod_ResetWin32(void)
{
	SDL_SysWMinfo info;
	HWND hWnd;
	HIMC hImc;
	Uint16 dummy;
	
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	hWnd = info.window;
	
	hImc = ImmGetContext(hWnd);
	
	ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
	
	ImmReleaseContext(hWnd, hImc);
	
	dummy = 0x0000;
	InputMethod_PostEvent(INPUT_METHOD_MESSAGE_CHANGE, &dummy, 0, 0, 0);
}


InputMethod_Result InputMethod_ValidateWin32(void)
{
	SDL_SysWMinfo info;
	HWND hWnd;
	HIMC hImc;
	
	IsValid = 1;
	
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	hWnd = info.window;
	hImc = ImmGetContext(hWnd);
	
	ImmSetOpenStatus(hImc, SaveOpenStatus);
	
	ImmReleaseContext(hWnd, hImc);
	
	InputMethod_ResetWin32();
	
	if (SaveOpenStatus == TRUE) {
		InputMethod_PostEvent(INPUT_METHOD_MESSAGE_ON, NULL, 0, 0, 0);
	}
	
	return INPUT_METHOD_SUCCESS;
}


InputMethod_Result InputMethod_InvalidateWin32(void)
{
	SDL_SysWMinfo info;
	HWND hWnd;
	HIMC hImc;
	
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	hWnd = info.window;
	hImc = ImmGetContext(hWnd);
	
	SaveOpenStatus = ImmGetOpenStatus(hImc);
	ImmSetOpenStatus(hImc, FALSE);
	
	ImmReleaseContext(hWnd, hImc);
	
	IsValid = SDL_FALSE;
	InputMethod_ResetWin32();
	
	if (SaveOpenStatus == TRUE) {
		InputMethod_PostEvent(INPUT_METHOD_MESSAGE_OFF, NULL, 0, 0, 0);
	}
	
	return INPUT_METHOD_SUCCESS;
}


Uint16 *InputMethod_GetNameWin32(void)
{
	SDL_SysWMinfo info;
	HWND hWnd;
	HIMC hImc;
	HKL hKl;
	char *tempString;
	int length;
	int needSize;
	
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	hWnd = info.window;
	hImc = ImmGetContext(hWnd);
	if (hImc) {
		hKl = GetKeyboardLayout(0);
		
		length = ImmGetDescription(hKl, NULL, 0);
		needSize =
			length
			+ strlen("\"\"")
			+ strlen(SYSTEM_SPECIFIC_NAME_STRING)
			+ 1; /* '\0' */
		tempString = (char*)malloc(needSize);
		strcpy(tempString, "\"");
		/* get string after '\"' */
		ImmGetDescription(hKl, tempString + 1, length);
		strcat(tempString, "\"");
		strcat(tempString, SYSTEM_SPECIFIC_NAME_STRING);
		
		MultiByteToWideChar(
			CP_ACP, 0, tempString, -1,
			(LPWSTR)InputMethod_win32.InputMethodName, INPUT_METHOD_NAME_STRING_LENGTH);
		
		free(tempString);
		ImmReleaseContext(hWnd, hImc);
		
		return InputMethod_win32.InputMethodName;
	} else {
		return NULL;
	}

}
//---------------------------------------------end of SDL_inputmethod for Windows--------------------------------------------------------------------
#endif


int32 exit_ok=0;

//substitute Windows functionality
#ifndef QB64_WINDOWS
 //messagebox defines
 #define IDOK                1
 #define IDCANCEL            2
 #define IDABORT             3
 #define IDRETRY             4
 #define IDIGNORE            5
 #define IDYES               6
 #define IDNO                7
 #define MB_OK                       0x00000000L
 #define MB_OKCANCEL                 0x00000001L
 #define MB_ABORTRETRYIGNORE         0x00000002L
 #define MB_YESNOCANCEL              0x00000003L
 #define MB_YESNO                    0x00000004L
 #define MB_RETRYCANCEL              0x00000005L
 #define MB_SYSTEMMODAL              0x00001000L
//alternate implementations of MessageBox
#ifdef QB64_MACOSX
int MessageBox(int ignore,char* message, char* header, int type )
{
    CFStringRef header_ref      = CFStringCreateWithCString( NULL, header,     kCFStringEncodingASCII );
    CFStringRef message_ref  = CFStringCreateWithCString( NULL, message, kCFStringEncodingASCII );
    CFOptionFlags result;
 if (type|MB_SYSTEMMODAL) type-=MB_SYSTEMMODAL;
 if (type==MB_YESNO){ 
    CFUserNotificationDisplayAlert(
                                                          0, // no timeout
                                                          kCFUserNotificationNoteAlertLevel,
                                                          NULL,
                                                          NULL, 
                                                          NULL,
                                                          header_ref,
                                                          message_ref,
                                                          CFSTR("No"),
                                                          CFSTR("Yes"),
                                                          NULL,
                                                          &result
                                                          );
    CFRelease( header_ref );
    CFRelease( message_ref );
    if( result == kCFUserNotificationDefaultResponse )
        return IDNO;
    else
        return IDYES;
 }
 if (type==MB_OK){ 
    CFUserNotificationDisplayAlert(
                                                          0, // no timeout
                                                          kCFUserNotificationNoteAlertLevel,
                                                          NULL,
                                                          NULL, 
                                                          NULL,
                                                          header_ref,
                                                          message_ref,
                                                          CFSTR("OK"),
                                                          NULL,
                                                          NULL,
                                                          &result
                                                          );
    CFRelease( header_ref );
    CFRelease( message_ref );
    return IDOK;
 }
 return IDCANCEL;
 }
#else
 int MessageBox(int ignore,char* message,char* title,int type){
 static qbs *s=NULL; if (!s) s=qbs_new(0,0);
 if (type|MB_SYSTEMMODAL) type-=MB_SYSTEMMODAL;
 if (type==MB_YESNO){
 qbs_set(s,qbs_new_txt("xmessage -center -title "));
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=34;
 qbs_set(s,qbs_add(s,qbs_new_txt(title)));
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=34;
 qbs_set(s,qbs_add(s,qbs_new_txt(" -buttons Yes:2,No:1 ")));
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=34;
 qbs_set(s,qbs_add(s,qbs_new_txt(message)));
 qbs_set(s,qbs_add(s,qbs_new_txt("                         ")));
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=34;
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=0;
 static int status;
 status=system((char*)s->chr);
 if (-1!=status){
 static int32 r;
 r=WEXITSTATUS(status);
 if (r==2) return IDYES;
 if (r==1) return IDNO;
 }
 return IDNO;
 }//MB_YESNO
 if (type==MB_OK){
 qbs_set(s,qbs_new_txt("xmessage -center -title "));
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=34;
 qbs_set(s,qbs_add(s,qbs_new_txt(title)));
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=34;
 qbs_set(s,qbs_add(s,qbs_new_txt(" -buttons OK:1 ")));
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=34;
 qbs_set(s,qbs_add(s,qbs_new_txt(message)));
 qbs_set(s,qbs_add(s,qbs_new_txt("                         ")));
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=34;
 qbs_set(s,qbs_add(s,qbs_new_txt("?"))); s->chr[s->len-1]=0;
 system((char*)s->chr);
 return IDOK;
 }//MB_OK
 return IDCANCEL;
 }
#endif

 void AllocConsole(){
 return;
 }
 void FreeConsole(){
 return;
 }

 int32 errno;
#endif

//vc->project->properties->configuration properties->general->configuration type->application(.exe)
//vc->project->properties->configuration properties->general->configuration type->static library(.lib)
extern int QBMAIN (void *);
extern int TIMERTHREAD(void *);
extern qbs* WHATISMYIP();

//directory access defines
#define EPERM           1
#define ENOENT          2
#define ESRCH           3
#define EINTR           4
#define EIO             5
#define ENXIO           6
#define E2BIG           7
#define ENOEXEC         8
#define EBADF           9
#define ECHILD          10
#define EAGAIN          11
#define ENOMEM          12
#define EACCES          13
#define EFAULT          14
#define EBUSY           16
#define EEXIST          17
#define EXDEV           18
#define ENODEV          19
#define ENOTDIR         20
#define EISDIR          21
#define EINVAL          22
#define ENFILE          23
#define EMFILE          24
#define ENOTTY          25
#define EFBIG           27
#define ENOSPC          28
#define ESPIPE          29
#define EROFS           30
#define EMLINK          31
#define EPIPE           32
#define EDOM            33
#define ERANGE          34
#define EDEADLK         36
#define ENAMETOOint32    38
#define ENOLCK          39
#define ENOSYS          40
#define ENOTEMPTY       41
#define EILSEQ          42


int32 lprint=0;//set to 1 during LPRINT operations
int32 lprint_image=0;
double lprint_last=0;//TIMER(0.001) value at last time LPRINT was used
int32 lprint_buffered=0;//set to 1 if content is pending to print
int32 lprint_locked=0;//set to 1 to deny access by QB64 program

/* Generic File System (GFS)
GFS allows OS specific access whilst still maintaining 'pure' C-based routines for
multiplatform compatibility. 'Pure' C-based routines may not allow certain functionality,
such as partial file locking.
GFS handles/indexes are independent of QB64 handles/indexes to allow for internal files
to be open but not intefere with the QB64 file handle numbers.

GFS error codes:
-1 non-specific fail
-2 invalid handle
-3 bad/incorrect file mode
-4 illegal function call (input is out of range)
-5 file not found (win:2)
-6 path not found (win:3)
-7 access/permission denied (win:5,19)
-8 device unavailable/drive invalid (win:15,21)
-9 path/file access error
-10 read past eof
-11 bad file name
*/

#ifdef QB64_WINDOWS
 #define GFS_WINDOWS
#endif
#ifndef GFS_WINDOWS
 #define GFS_C
#endif

struct gfs_file_struct{//info applicable to all files
int64 id;//a unique ID given to all files (currently only referenced by the FIELD statement to remove old field conditions)
uint8 open;
uint8 read;
uint8 write;
uint8 lock_read;
uint8 lock_write;
int64 pos;//-1=unknown
uint8 eof_reached;//read last character of file (set/reset by gfs_read only)
uint8 eof_passed;//attempted to read past eof (set/reset by gfs_read only)
int32 fileno;//link to fileno index
uint8 type;//qb access method (1=RANDOM,2=BINARY,3=INPUT,4=OUTPUT)
int64 record_length;//used by RANDOM
uint8 *field_buffer;
qbs **field_strings;//list of qbs pointers linked to this file
int32 field_strings_n;//number of linked strings
int64 column;//used by OUTPUT/APPEND to tab correctly (base 0)
//GFS_C data follows: (unused by custom GFS interfaces)
fstream *file_handle;
ofstream *file_handle_o;
//COM port data follows (*=default)
uint8 com_port;//0=not a com port
int32 com_baud_rate;//(bits per second)75,110,150,300*,600,1200,1800,2400,9600,?
int8 com_parity;//[0]N,[1]E*,[2]O,[3]S,[4]M,[5]PE(none,even*,odd,space,mark,error-checking)
int8 com_data_bits_per_byte;//5,6,7*,8
int8 com_stop_bits;//[10]1,[15]1.5,[20]2
//The default value is 1 for baud rates greater than 110. For
//baud rates less than or equal to 110, the default value is
//1.5 when data is 5; otherwise, the value is 2.
int8 com_bin_asc;//[0]=BIN*,[1]=ASC
int8 com_asc_lf;//[0]omit*,[1]LF(only valid with ASC)
//note: rb_x and tb_x are ignored by QB64 (receive and transmit buffer sizes)
int8 com_rs;//[0]detect*,[1]dont-detect
int32 com_cd_x;//0*-65535
int32 com_cs_x;//1000*,0-65535
int32 com_ds_x;//1000*,0-65535
int32 com_op_x;
//                 OP not used:          x omitted:     x specified:
//                 10 times the CD or    10000 ms       0 - 65,535 milliseconds
//                 DS timeout value,
//                 whichever is greater
};

 #ifdef GFS_WINDOWS
 struct gfs_file_win_struct{//info applicable to WINDOWS OS files
 HANDLE file_handle;
 };
 gfs_file_win_struct *gfs_file_win=(gfs_file_win_struct*)malloc(1);
 #endif

int64 gfs_nextid=1;

gfs_file_struct *gfs_file=(gfs_file_struct*)malloc(1);

int32 gfs_n=0;
int32 gfs_freed_n=0;
int32 *gfs_freed=(int32*)malloc(1);
int32 gfs_freed_size=0;

int32 *gfs_fileno=(int32*)malloc(1);
int32 gfs_fileno_n=0;

static int32 file_qb64icon_bmp_len=2102;
static const uint8 file_qb64icon_bmp[]={
66,77,54,8,0,0,0,0,0,0,54,4,0,0,40,0,0,0,32,0,0,0,32,0,0,0,1,0,8,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,168,0,0,0,0,168,0,0,168,168,0,0,0,0,168,0,168,0,168,0,0,84,168,0,168,168,168,0,84,84,84,0,252,84,84,0,84,252,84,0,252,252,84,0,84,84,252,0,252,84,252,0,84,252,252,0,252,252,252,0,0,0,0,0,20,20,20,0,32,32,32,0,44,44,44,0,56,56,56,0,68,68,68,0,80,80,80,0,96,96,96,0,112,112,112,0,128,128,128,0,144,144,144,0,160,160,160,0,180,180,180,0,200,200,200,0,224,224,224,0,252,252,252,0,252,0,0,0,252,0,64,0,252,0,124,0,252,0,188,0,252,0,252,0,188,0,252,0,124,0,252,0,64,0,252,0,0,0,252,0,0,64,252,0,0,124,252,0,0,188,252,0,0,252,252,0,0,252,188,0,0,252,124,0,0,252,64,0,0,252,0,0,64,252,0,0,124,252,0,0,188,252,0,0,252,252,0,0,252,188,0,0,252,124,0,0,252,64,0,0,252,124,124,0,252,124,156,0,252,124,188,0,252,124,220,0,252,124,252,0,220,124,252,0,188,124,252,0,156,124,252,0,124,124,252,0,124,156,252,0,124,188,252,0,124,220,252,0,124,252,252,0,124,252,220,0,124,252,188,0,124,252,156,
0,124,252,124,0,156,252,124,0,188,252,124,0,220,252,124,0,252,252,124,0,252,220,124,0,252,188,124,0,252,156,124,0,252,180,180,0,252,180,196,0,252,180,216,0,252,180,232,0,252,180,252,0,232,180,252,0,216,180,252,0,196,180,252,0,180,180,252,0,180,196,252,0,180,216,252,0,180,232,252,0,180,252,252,0,180,252,232,0,180,252,216,0,180,252,196,0,180,252,180,0,196,252,180,0,216,252,180,0,232,252,180,0,252,252,180,0,252,232,180,0,252,216,180,0,252,196,180,0,112,0,0,0,112,0,28,0,112,0,56,0,112,0,84,0,112,0,112,0,84,0,112,0,56,0,112,0,28,0,112,0,0,0,112,0,0,28,112,0,0,56,112,0,0,84,112,0,0,112,112,0,0,112,84,0,0,112,56,0,0,112,28,0,0,112,0,0,28,112,0,0,56,112,0,0,84,112,0,0,112,112,0,0,112,84,0,0,112,56,0,0,112,28,0,0,112,56,56,0,112,56,68,0,112,56,84,0,112,56,96,0,112,56,112,0,96,56,112,0,84,56,112,0,68,56,112,0,56,56,112,0,56,68,112,0,56,84,112,0,56,96,112,0,56,112,112,0,56,112,96,0,56,112,84,0,56,112,68,0,56,112,56,0,68,112,56,0,84,112,56,0,96,112,56,0,112,112,56,0,112,96,56,0,112,84,56,0,112,
68,56,0,112,80,80,0,112,80,88,0,112,80,96,0,112,80,104,0,112,80,112,0,104,80,112,0,96,80,112,0,88,80,112,0,80,80,112,0,80,88,112,0,80,96,112,0,80,104,112,0,80,112,112,0,80,112,104,0,80,112,96,0,80,112,88,0,80,112,80,0,88,112,80,0,96,112,80,0,104,112,80,0,112,112,80,0,112,104,80,0,112,96,80,0,112,88,80,0,64,0,0,0,64,0,16,0,64,0,32,0,64,0,48,0,64,0,64,0,48,0,64,0,32,0,64,0,16,0,64,0,0,0,64,0,0,16,64,0,0,32,64,0,0,48,64,0,0,64,64,0,0,64,48,0,0,64,32,0,0,64,16,0,0,64,0,0,16,64,0,0,32,64,0,0,48,64,0,0,64,64,0,0,64,48,0,0,64,32,0,0,64,16,0,0,64,32,32,0,64,32,40,0,64,32,48,0,64,32,56,0,64,32,64,0,56,32,64,0,48,32,64,0,40,32,64,0,32,32,64,0,32,40,64,0,32,48,64,0,32,56,64,0,32,64,64,0,32,64,56,0,32,64,48,0,32,64,40,0,32,64,32,0,40,64,32,0,48,64,32,0,56,64,32,0,64,64,32,0,64,56,32,0,64,48,32,0,64,40,32,0,64,44,44,0,64,44,48,0,64,44,52,0,64,44,60,0,64,44,64,0,60,44,64,0,52,44,64,0,48,44,64,0,44,44,64,0,44,48,64,0,44,52,64,0,44,60,64,0,44,64,64,0,44,64,60,0,44,64,52,0,44,64,48,0,44,64,44,0,48,64,
44,0,52,64,44,0,60,64,44,0,64,64,44,0,64,60,44,0,64,52,44,0,64,48,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,104,104,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,104,104,1,43,43,43,43,43,43,43,43,43,43,43,43,43,1,32,1,32,1,32,1,32,1,32,1,42,42,42,42,32,104,104,32,43,43,43,43,43,43,43,43,43,43,43,43,43,32,1,32,1,32,1,32,1,32,1,32,42,42,42,42,1,104,104,1,43,43,43,43,1,1,1,1,1,43,43,43,43,1,32,1,32,1,32,1,32,1,32,1,42,42,42,42,32,104,104,32,43,43,43,43,1,1,1,1,1,43,43,43,43,32,1,32,1,32,1,32,1,32,1,32,42,42,42,42,1,104,104,1,43,43,43,43,43,43,43,43,43,43,43,43,43,1,32,42,42,42,42,42,42,42,42,42,42,42,42,42,32,104,104,32,43,43,43,43,43,43,43,43,43,43,43,43,43,32,1,42,42,42,42,42,42,42,42,42,42,42,42,42,1,104,104,1,43,43,43,43,32,
1,32,1,32,1,32,1,32,1,32,42,42,42,42,1,32,1,32,1,42,42,42,42,32,104,104,32,43,43,43,43,1,32,1,32,1,32,1,32,1,32,1,42,42,42,42,32,1,32,1,32,42,42,42,42,1,104,104,1,43,43,43,43,32,1,32,1,32,1,32,1,32,1,32,42,42,42,42,1,32,1,32,1,42,42,42,42,32,104,104,32,43,43,43,43,43,43,43,43,43,43,43,43,43,32,1,42,42,42,42,32,1,32,1,32,42,42,42,42,1,104,104,1,43,43,43,43,43,43,43,43,43,43,43,43,43,1,32,42,42,42,42,1,32,1,32,1,42,42,42,42,32,104,104,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,104,104,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,104,104,32,1,32,1,32,53,53,53,53,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,104,104,1,32,1,32,1,53,53,53,53,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,104,104,32,53,53,53,53,53,53,53,53,53,53,53,53,1,32,1,54,54,54,54,54,54,54,54,54,54,54,54,54,1,104,104,1,53,53,53,53,53,53,53,53,53,53,53,53,32,1,32,54,54,54,54,54,54,54,54,54,54,54,54,54,32,104,104,32,53,53,53,53,1,1,1,1,53,53,53,53,
1,32,1,54,54,54,54,1,1,1,1,1,54,54,54,54,1,104,104,1,53,53,53,53,1,1,1,1,53,53,53,53,32,1,32,54,54,54,54,1,1,1,1,1,54,54,54,54,32,104,104,32,53,53,53,53,1,1,1,1,53,53,53,53,1,32,1,54,54,54,54,54,54,54,54,54,54,54,1,32,1,104,104,1,53,53,53,53,1,1,1,1,53,53,53,53,32,1,32,54,54,54,54,54,54,54,54,54,54,54,32,1,32,104,104,32,53,53,53,53,1,1,1,1,53,53,53,53,1,32,1,54,54,54,54,54,54,54,54,54,54,54,1,32,1,104,104,1,53,53,53,53,1,1,1,1,1,53,53,53,32,1,32,54,54,54,54,1,1,1,1,1,54,54,54,54,32,104,104,32,53,53,53,53,1,1,1,1,1,53,53,53,1,32,1,54,54,54,54,1,1,1,1,1,54,54,54,54,1,104,104,1,53,53,53,53,53,53,53,53,53,53,53,53,32,1,32,54,54,54,54,54,54,54,54,54,54,54,54,54,32,104,104,32,53,53,53,53,53,53,53,53,53,53,53,53,1,32,1,54,54,54,54,54,54,54,54,54,54,54,54,54,1,104,104,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,104,104,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,
104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104
};

static int32 file_charset8_raw_len=16384;
static const uint8 file_charset8_raw[]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,255,0,0,0,0,0,0,255,255,0,255,0,0,255,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,0,255,255,255,255,255,255,0,0,255,255,255,255,255,255,0,255,255,255,255,255,255,255,255,255,255,0,255,255,0,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,255,255,255,255,255,255,0,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,255,
255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,0,0,0,0,0,255,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,0,0,0,0,255,0,0,255,0,0,0,0,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,255,255,255,0,0,255,255,0,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,
0,255,255,0,0,255,255,0,0,255,255,255,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,0,255,255,255,255,255,0,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,0,0,0,255,0,0,255,255,0,0,255,0,255,0,255,255,0,255,0,0,0,255,255,255,255,0,0,255,255,255,0,0,255,255,255,255,255,255,0,0,255,255,255,0,0,255,255,
255,255,0,0,0,255,0,255,255,0,255,0,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,0,0,0,255,255,255,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,255,255,255,0,0,0,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,
255,0,0,255,255,0,0,0,255,255,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,255,255,255,255,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,255,255,255,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,255,0,0,0,255,255,0,0,255,255,0,255,255,255,255,255,255,255,255,0,255,255,0,0,255,255,0,0,0,255,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,255,255,255,255,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,255,255,0,0,255,255,0,0,0,
255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,255,255,255,255,255,255,255,255,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,0,255,255,0,255,255,255,255,0,255,255,255,255,0,255,255,0,255,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,
255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,
0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,
255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,0,0,255,255,
255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,255,255,0,0,0,255,0,0,255,255,0,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,0,0,0,0,255,255,0,0,0,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,255,255,0,0,0,255,0,0,255,255,0,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,
0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,0,0,255,255,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,255,0,255,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,0,255,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,
0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,
255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,0,255,255,0,255,255,255,255,255,255,255,0,255,255,255,0,255,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,0,
0,255,255,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,
0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,0,255,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,255,0,0,0,255,255,255,0,255,
255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,255,0,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,255,255,
0,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,255,255,0,0,0,0,255,255,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,
0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,255,0,255,255,0,0,0,0,255,255,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,
0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,
255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,255,255,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,
0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,255,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,255,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,255,255,
0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,255,255,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,255,255,0,255,
255,0,0,255,255,0,0,255,255,0,255,255,0,255,255,0,0,255,255,0,255,255,255,0,255,255,0,255,255,255,255,255,255,0,0,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,255,255,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,0,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,0,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,255,255,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,255,255,
0,255,255,0,255,255,0,255,255,255,0,255,255,255,255,255,0,255,255,0,255,255,255,255,255,0,255,255,255,0,255,255,0,255,255,0,255,255,0,255,255,255,0,255,255,255,255,255,0,255,255,0,255,255,255,255,255,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,
255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,
0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,
0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,0,255,255,255,0,0,255,255,0,255,255,0,
0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,
255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,255,255,0,0,255,0,0,0,255,255,0,255,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,
0,255,255,0,255,255,0,0,255,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,
255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,
0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,255,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,
255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static int32 file_chrset16_raw_len=32768;
static const uint8 file_chrset16_raw[]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,255,0,0,0,0,0,0,255,255,0,255,0,0,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,255,255,255,255,255,255,255,255,255,255,0,255,255,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,255,255,255,0,0,255,255,255,255,255,255,0,0,255,255,255,255,255,255,0,0,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,0,0,0,0,255,0,0,255,0,0,0,0,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,255,255,255,0,0,255,255,0,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,255,255,0,0,255,255,255,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,0,0,0,255,255,0,0,255,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,
255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,
0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,0,0,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,255,255,0,0,255,255,255,255,0,0,255,255,255,0,0,255,255,255,0,0,255,255,255,255,0,0,255,255,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,0,0,0,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,0,255,255,255,
255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,
0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,
255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,255,255,255,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,255,0,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,0,255,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,
255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,0,255,255,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,
255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,255,0,255,255,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,255,255,255,255,255,255,255,255,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,
255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,0,0,
0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,
0,255,255,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,
255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,
255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,
255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,
0,255,255,0,0,0,255,0,0,255,255,0,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,0,0,255,255,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,0,0,255,255,0,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,
255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,0,0,
0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,0,0,255,255,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,255,0,255,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,0,255,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,255,255,0,255,255,0,255,255,
255,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,
255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,255,255,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,
255,255,255,255,0,0,255,0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,
255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,255,0,255,255,0,255,255,255,255,255,255,255,0,255,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,
0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,
0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,
0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,
0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,
0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,0,255,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,
255,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,
255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,255,0,0,0,255,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,255,0,255,255,0,255,255,0,255,0,255,255,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,
0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,
0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,
0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,
255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,255,255,255,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,
255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,
255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,
255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,0,0,0,255,255,0,0,0,0,0,255,255,255,
255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,0,0,255,255,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,255,255,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,
0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,
255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,255,255,
255,0,255,255,0,255,255,255,255,0,255,255,0,0,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,

255,255,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,255,255,255,0,0,255,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,255,0,255,255,0,0,255,255,255,0,255,0,0,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,255,255,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,255,255,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,0,0,255,0,0,0,255,0,255,0,0,0,255,0,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,0,255,0,255,0,255,0,255,255,0,255,0,255,0,255,0,255,255,0,255,255,255,0,255,0,255,255,255,0,255,255,
255,255,255,0,255,255,255,0,255,0,255,255,255,0,255,255,255,255,255,0,255,255,255,0,255,0,255,255,255,0,255,255,255,255,255,0,255,255,255,0,255,0,255,255,255,0,255,255,255,255,255,0,255,255,255,0,255,0,255,255,255,0,255,255,255,255,255,0,255,255,255,0,255,0,255,255,255,0,255,255,255,255,255,0,255,255,255,0,255,0,255,255,255,0,255,255,255,255,255,0,255,255,255,0,255,0,255,255,255,0,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,
0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,
0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,
255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,
255,0,0,0,255,255,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,
255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,
0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,
255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,0,0,255,255,0,0,0,0,0,0,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,
255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,0,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,
255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,
0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,
255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,0,0,0,255,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,255,255,0,0,0,255,255,
0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,255,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,255,255,255,255,255,0,255,
255,0,255,255,0,255,255,255,255,0,255,255,0,255,255,255,255,255,255,0,0,255,255,0,255,255,255,255,255,255,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,
255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,255,255,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,255,255,0,255,255,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,0,0,255,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,0,0,0,255,255,0,0,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static int32 file_qb64_pal_len=1024;
static const uint8 file_qb64_pal[]={
0,0,0,0,170,0,0,0,0,170,0,0,170,170,0,0,0,0,170,0,170,0,170,0,0,85,170,0,170,170,170,0,85,85,85,0,255,85,85,0,85,255,85,0,255,255,85,0,85,85,255,0,255,85,255,0,85,255,255,0,255,255,255,0,0,0,0,0,20,20,20,0,32,32,32,0,45,45,45,0,57,57,57,0,69,69,69,0,81,81,81,0,97,97,97,0,113,113,113,0,130,130,130,0,146,146,146,0,162,162,162,0,182,182,182,0,202,202,202,0,227,227,227,0,255,255,255,0,255,0,0,0,255,0,65,0,255,0,125,0,255,0,190,0,255,0,255,0,190,0,255,0,125,0,255,0,65,0,255,0,0,0,255,0,0,65,255,0,0,125,255,0,0,190,255,0,0,255,255,0,0,255,190,0,0,255,125,0,0,255,65,0,0,255,0,0,65,255,0,0,125,255,0,0,190,255,0,0,255,255,0,0,255,190,0,0,255,125,0,0,255,65,0,0,255,125,125,0,255,125,158,0,255,125,190,0,255,125,223,0,255,125,255,0,223,125,255,0,190,125,255,0,158,125,255,0,125,125,255,0,125,158,255,0,125,190,255,0,125,223,255,0,125,255,255,0,125,255,223,0,125,255,190,0,125,255,158,0,125,255,125,0,158,255,125,0,190,255,125,0,223,255,125,0,255,255,125,0,255,223,125,0,255,190,125,0,255,158,125,0,255,
182,182,0,255,182,198,0,255,182,219,0,255,182,235,0,255,182,255,0,235,182,255,0,219,182,255,0,198,182,255,0,182,182,255,0,182,198,255,0,182,219,255,0,182,235,255,0,182,255,255,0,182,255,235,0,182,255,219,0,182,255,198,0,182,255,182,0,198,255,182,0,219,255,182,0,235,255,182,0,255,255,182,0,255,235,182,0,255,219,182,0,255,198,182,0,113,0,0,0,113,0,28,0,113,0,57,0,113,0,85,0,113,0,113,0,85,0,113,0,57,0,113,0,28,0,113,0,0,0,113,0,0,28,113,0,0,57,113,0,0,85,113,0,0,113,113,0,0,113,85,0,0,113,57,0,0,113,28,0,0,113,0,0,28,113,0,0,57,113,0,0,85,113,0,0,113,113,0,0,113,85,0,0,113,57,0,0,113,28,0,0,113,57,57,0,113,57,69,0,113,57,85,0,113,57,97,0,113,57,113,0,97,57,113,0,85,57,113,0,69,57,113,0,57,57,113,0,57,69,113,0,57,85,113,0,57,97,113,0,57,113,113,0,57,113,97,0,57,113,85,0,57,113,69,0,57,113,57,0,69,113,57,0,85,113,57,0,97,113,57,0,113,113,57,0,113,97,57,0,113,85,57,0,113,69,57,0,113,81,81,0,113,81,89,0,113,81,97,0,113,81,105,0,113,81,113,0,105,81,113,0,97,81,113,0,89,81,113,0,81,81,113,0,
81,89,113,0,81,97,113,0,81,105,113,0,81,113,113,0,81,113,105,0,81,113,97,0,81,113,89,0,81,113,81,0,89,113,81,0,97,113,81,0,105,113,81,0,113,113,81,0,113,105,81,0,113,97,81,0,113,89,81,0,65,0,0,0,65,0,16,0,65,0,32,0,65,0,49,0,65,0,65,0,49,0,65,0,32,0,65,0,16,0,65,0,0,0,65,0,0,16,65,0,0,32,65,0,0,49,65,0,0,65,65,0,0,65,49,0,0,65,32,0,0,65,16,0,0,65,0,0,16,65,0,0,32,65,0,0,49,65,0,0,65,65,0,0,65,49,0,0,65,32,0,0,65,16,0,0,65,32,32,0,65,32,40,0,65,32,49,0,65,32,57,0,65,32,65,0,57,32,65,0,49,32,65,0,40,32,65,0,32,32,65,0,32,40,65,0,32,49,65,0,32,57,65,0,32,65,65,0,32,65,57,0,32,65,49,0,32,65,40,0,32,65,32,0,40,65,32,0,49,65,32,0,57,65,32,0,65,65,32,0,65,57,32,0,65,49,32,0,65,40,32,0,65,45,45,0,65,45,49,0,65,45,53,0,65,45,61,0,65,45,65,0,61,45,65,0,53,45,65,0,49,45,65,0,45,45,65,0,45,49,65,0,45,53,65,0,45,61,65,0,45,65,65,0,45,65,61,0,45,65,53,0,45,65,49,0,45,65,45,0,49,65,45,0,53,65,45,0,61,65,45,0,65,65,45,0,65,61,45,0,65,53,45,0,65,49,45,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0
};

static int32 file_qb64ega_pal_len=256;
static const uint8 file_qb64ega_pal[]={
0,0,0,0,170,0,0,0,0,170,0,0,170,170,0,0,0,0,170,0,170,0,170,0,0,170,170,0,170,170,170,0,85,0,0,0,255,0,0,0,85,170,0,0,255,170,0,0,85,0,170,0,255,0,170,0,85,170,170,0,255,170,170,0,0,85,0,0,170,85,0,0,0,255,0,0,170,255,0,0,0,85,170,0,170,85,170,0,0,255,170,0,170,255,170,0,85,85,0,0,255,85,0,0,85,255,0,0,255,255,0,0,85,85,170,0,255,85,170,0,85,255,170,0,255,255,170,0,0,0,85,0,170,0,85,0,0,170,85,0,170,170,85,0,0,0,255,0,170,0,255,0,0,170,255,0,170,170,255,0,85,0,85,0,255,0,85,0,85,170,85,0,255,170,85,0,85,0,255,0,255,0,255,0,85,170,255,0,255,170,255,0,0,85,85,0,170,85,85,0,0,255,85,0,170,255,85,0,0,85,255,0,170,85,255,0,0,255,255,0,170,255,255,0,85,85,85,0,255,85,85,0,85,255,85,0,255,255,85,0,85,85,255,0,255,85,255,0,85,255,255,0,255,255,255,0
};

uint16 *unicode16_buf=(uint16*)malloc(1);
int32 unicode16_buf_size=1;
void convert_text_to_utf16(int32 fonthandle,void *buf,int32 size){
//expand buffer if necessary
if (unicode16_buf_size<(size*4+4)){
unicode16_buf_size=size*4+4;
free(unicode16_buf);
unicode16_buf=(uint16*)malloc(unicode16_buf_size);
}
//convert text
if ((fontflags[fonthandle]&32)&&(fonthandle!=NULL)){//unicode font
if (size==1) size=4;
convert_unicode(32,buf,size,16,unicode16_buf);
}else{
convert_unicode(1,buf,size,16,unicode16_buf);
}
}



qbs *unknown_opcode_mess;

extern uint32 ercl;

int32 exit_blocked=0;
int32 exit_value=0;
	//1=X-button
	//2=CTRL-BREAK
	//3=X-button and CTRL-BREAK

//MLP
//int32 qbshlp1=0;

//special file handle (sfh) system:
struct sfh_struct{
uint8 type;//what type of handle it is
};

sfh_struct *sfh=(sfh_struct*)calloc(1,sizeof(sfh_struct));
uint32 sfh_bufsize=0;

uint32 *sfh_freed=(uint32*)calloc(1,4);
uint32 sfh_freed_bufsize=0;
int32 sfh_nfreed=0;

uint32 sfh_new(){
uint32 i;
//check freed list
if (sfh_nfreed){
sfh_nfreed--;
i=sfh_freed[sfh_nfreed];
}else{
//expand buffer
sfh_bufsize++;
sfh=(sfh_struct*)realloc(sfh,sizeof(sfh_struct)*sfh_bufsize);
i=sfh_bufsize-1;
}
sfh[i].type=255;//undefined
return i;
}

void sfh_free(int32 i){
if ((i<0)||(i>=sfh_bufsize)) return;
if (sfh[i].type){//allocated
sfh[i].type=0;
if (sfh_freed_bufsize<=sfh_nfreed){
sfh_freed_bufsize++;
sfh_freed=(uint32*)realloc(sfh_freed,sfh_freed_bufsize*4);
}
sfh_freed[sfh_nfreed++]=i;
}
}

//network prototype:

//network handles
struct net_tcp_struct{
IPaddress ip;
TCPsocket socket;
uint16 portused;
SDLNet_SocketSet set;
uint8 error;//0=no communication error has occurred
uint8 eof;
	uint8 *buffer;
	uint32 buffer_size;
	uint32 buffer_space;
};
net_tcp_struct *net_tcp=(net_tcp_struct*)calloc(1,sizeof(net_tcp_struct));
uint32 net_tcp_bufsize=1;

uint8 *revert_input_buffer=NULL;
uint32 revert_input_bufsize=0;
int32 revert_input_x=-1;//invalid

void net_tcp_updatebuffer(uint32 i){//i is a valid index in the net_tcp 
static int32 x;
static uint8 byte;

if (net_tcp[i].error) return;

getdata:

x=SDLNet_CheckSockets(net_tcp[i].set,0);
if (x==-1){//error
net_tcp[i].error=1;
return;
}

if (x==0) return;//no change (no new data)

x=SDLNet_TCP_Recv(net_tcp[i].socket,&byte,1);

if (x<=0){//error/no data when there should have been 1+ bytes
net_tcp[i].error=1;
return;
}

//expand buffer if necessary
if (net_tcp[i].buffer_size==net_tcp[i].buffer_space){
if (net_tcp[i].buffer==NULL){
net_tcp[i].buffer=(uint8*)malloc(net_tcp[i].buffer_space+1024);//1K increments
}else{
net_tcp[i].buffer=(uint8*)realloc(net_tcp[i].buffer,net_tcp[i].buffer_space+1024);//1K increments
}
net_tcp[i].buffer_space+=1024;
}

net_tcp[i].buffer[net_tcp[i].buffer_size++]=byte;

goto getdata;
}

uint8 *tcp_feed_ucbuf;
int32 tcp_feed_ucbufsiz;
int32 tcp_feed_offset;











char *fixdir(qbs *filename){
//note: changes the slashes in a filename to make it compatible with the OS
//applied to QB commands: open, bload/bsave, loadfont, loadimage, sndopen/sndplayfile
static int32 i;
for (i=0;i<filename->len;i++){
#ifdef QB64_WINDOWS
if (filename->chr[i]==47) filename->chr[i]=92;
#else
if (filename->chr[i]==92) filename->chr[i]=47;
#endif
}
return (char*)filename->chr;
}


int32 width8050switch=1;//if set, can automatically switch to WIDTH 80,50 if LOCATE'ing beyond row 26

uint32 pal[256];

extern qbs* nothingstring;

static uint32 sdl_shiftstate=0;

static uint32 sdl_scroll_lock=0;
static uint32 sdl_insert=0;
static uint32 sdl_scroll_lock_prepared=1;
static uint32 sdl_insert_prepared=1;

int32 sub_screen_height_in_characters=-1;//-1=undefined
int32 sub_screen_width_in_characters=-1;//-1=undefined
int32 sub_screen_font=-1;//-1=undefined
int32 sub_screen_keep_page0=0;

int32 key_repeat_on=0;



void error(int32 error_number);//for forward references
uint32 palette_256[256];
uint32 palette_64[64];

//QB64 2D PROTOTYPE 1.0
SDL_Surface *ts,*ts2;
SDL_PixelFormat pixelformat32;
SDL_PixelFormat pixelformat8;

int32 pages=1;
int32 *page=(int32*)calloc(1,4);

#define IMG_BUFFERSIZE 4096
img_struct *img=(img_struct*)malloc(IMG_BUFFERSIZE*sizeof(img_struct));
int32 nimg=IMG_BUFFERSIZE;
int32 nextimg=0;

uint32 *fimg=(uint32*)malloc(IMG_BUFFERSIZE*4);//a list to recover freed indexes
int32 nfimg=IMG_BUFFERSIZE;
int32 lastfimg=-1;//-1=no freed indexes exist

uint8 *cblend=NULL;
uint8 *ablend=NULL;
uint8 *ablend127;
uint8 *ablend128;
//to save 16MB of RAM, software blend tables are only allocated if a 32-bit image is created
void init_blend(){
uint8 *cp;
int32 i,x2,x3,i2,z;
float f,f2,f3;
cblend=(uint8*)malloc(16777216);
cp=cblend;
for (i=0;i<256;i++){//source alpha
for (x2=0;x2<256;x2++){//source
for (x3=0;x3<256;x3++){//dest
f=i;
f2=x2;
f3=x3;
f/=255.0;//0.0-1.0
*cp++=qbr_float_to_long((f*f2)+((1.0-f)*f3));//CINT(0.0-255.0)
}}}
/*
"60%+60%=84%" formula
imagine a 60% opaque lens, you can see 40% of whats behind
now put another 60% opaque lens on top of it
you can now see 40% of the previous lens of which 40% is of the original scene
40% of 40% is 16%
100%-16%=84%
 V1=60, V2=60
 v1=V1/100, v2=V2/100
 iv1=1-v1, iv2=1-v2
 iv3=iv1*iv2
 v3=1-iv3
 V3=v3*100
*/
ablend=(uint8*)malloc(65536);
cp=ablend;
for (i=0;i<256;i++){//first alpha value
for (i2=0;i2<256;i2++){//second alpha value
f=i; f2=i2;
f/=255.0; f2/=255.0;
f=1.0-f; f2=1.0-f2;
f3=f*f2;
z=qbr_float_to_long((1.0-f3)*255.0);
*cp++=z;
}}
ablend127=ablend+(127<<8);
ablend128=ablend+(128<<8);
}


uint32 display_page_index=0;
uint32 write_page_index=0;
uint32 read_page_index=0;
//use of non-indexed forms assumes valid indexes (may not be suitable for all commands)
img_struct *write_page=NULL;
img_struct *read_page=NULL;
img_struct *display_page=NULL;
SDL_Surface *display_surface=NULL;
uint32 *display_surface_offset=0;

void restorepalette(img_struct* im){
static uint32 *pal;
if (im->bytes_per_pixel==4) return;
pal=im->pal;

switch(im->compatible_mode){

case 1:
/*
SCREEN Mode 1 Syntax:  COLOR [background][,palette]
   ¦ background is the screen color (range = 0-15)
   ¦ palette is a three-color palette (range = 0-1)
     0 = green, red, and brown         1 = cyan, magenta, and bright white
Note: option 1 is the default, palette can override these though
OPTION 1:*DEFAULT*
0=black(color 0)
1=cyan(color 3)
2=purple(color 5)
3=light grey(color 7)
OPTION 0:
0=black(color 0)
1=green(color 2)
2=red(color 4)
3=brown(color 6)
*/
pal[0]=palette_256[0];
pal[1]=palette_256[3];
pal[2]=palette_256[5];
pal[3]=palette_256[7];
return;
break;

case 2://black/white 2 color palette
pal[0]=0;
pal[1]=0xFFFFFF;
return;
break;

case 9://16 colors selected from 64 possibilities
pal[0]=palette_64[0];
pal[1]=palette_64[1];
pal[2]=palette_64[2];
pal[3]=palette_64[3];
pal[4]=palette_64[4];
pal[5]=palette_64[5];
pal[6]=palette_64[20];
pal[7]=palette_64[7];
pal[8]=palette_64[56];
pal[9]=palette_64[57];
pal[10]=palette_64[58];
pal[11]=palette_64[59];
pal[12]=palette_64[60];
pal[13]=palette_64[61];
pal[14]=palette_64[62];
pal[15]=palette_64[63];
return;
break;

case 10://4 colors selected from 9 possibilities (indexes held in array pal[4-7])
pal[4]=0;
pal[5]=4;
pal[6]=6;
pal[7]=8;
return;
break;

case 11://black/white 2 color palette
pal[0]=0;
pal[1]=0xFFFFFF;
return;
break;

case 13:
memcpy(pal,palette_256,1024);
return;
break;

case 256:
memcpy(pal,palette_256,1024);
return;
break;

default:
//default 16 color palette
memcpy(pal,palette_256,64);

};//switch

}//restorepalette




void pset(int32 x,int32 y,uint32 col){
static uint8 *cp;
static uint32 *o32;
static uint32 destcol;
if (write_page->bytes_per_pixel==1){
 write_page->offset[y*write_page->width+x]=col&write_page->mask;
 return;
}else{
 if (write_page->alpha_disabled){
 write_page->offset32[y*write_page->width+x]=col;
 return;
 }
switch(col&0xFF000000){
case 0xFF000000://100% alpha, so regular pset (fast)
 write_page->offset32[y*write_page->width+x]=col;
 return;
break;
case 0x0://0%(0) alpha, so no pset (very fast)
 return;
break;
case 0x80000000://~50% alpha (optomized)

 o32=write_page->offset32+(y*write_page->width+x);
 *o32=(((*o32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend128[*o32>>24]<<24);
 return;
break; 
case 0x7F000000://~50% alpha (optomized)
 o32=write_page->offset32+(y*write_page->width+x);
 *o32=(((*o32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend127[*o32>>24]<<24);
 return;
break;
default://other alpha values (uses a lookup table)
 o32=write_page->offset32+(y*write_page->width+x);
 destcol=*o32;
 cp=cblend+(col>>24<<16);
 *o32=
   cp[(col<<8&0xFF00)+(destcol&255)    ]
 +(cp[(col&0xFF00)   +(destcol>>8&255) ]<<8)
 +(cp[(col>>8&0xFF00)+(destcol>>16&255)]<<16)
 +(ablend[(col>>24)+(destcol>>16&0xFF00)]<<24);
};
}
}



/*
img_struct *img=(img_struct*)malloc(1024*sizeof(img_struct));
uint32 nimg=1024;
uint32 nextimg=0;//-1=none have been assigned

uint32 *freeimg=(uint32*)malloc(1024*4);//a list to recover freed indexes
uint32 nfreeimg=1024;
uint32 lastfreeimg=-1;//-1=no freed indexes exist
*/

//returns an index to free img structure
uint32 newimg(){
static int32 i;
if (lastfimg!=-1){
i=fimg[lastfimg--];
goto gotindex;
}
if (nextimg<nimg){
i=nextimg++;
goto gotindex;
}
img=(img_struct*)realloc(img,(nimg+IMG_BUFFERSIZE)*sizeof(img_struct));
if (!img) error(502);
//update existing img pointers to new locations
display_page=&img[display_page_index];
write_page=&img[write_page_index];
read_page=&img[read_page_index];
memset(&img[nimg],0,IMG_BUFFERSIZE*sizeof(img_struct));
nimg+=IMG_BUFFERSIZE;
i=nextimg++;
gotindex:
img[i].valid=1;
return i;
}

int32 freeimg(uint32 i){
//returns: 0=failed, 1=success
if (i>=nimg) return 0;
if (!img[i].valid) return 0;
if (lastfimg>=(nfimg-1)){//extend
fimg=(uint32*)realloc(fimg,(nfimg+IMG_BUFFERSIZE)*4);
if (!fimg) error(503);
nfimg+=IMG_BUFFERSIZE;
}
memset(&img[i],0,sizeof(img_struct));
lastfimg++;
fimg[lastfimg]=i;
return 1;
}


void imgrevert(int32 i){
static int32 bpp;
static img_struct *im;

im=&img[i];
bpp=im->compatible_mode;

//revert to assumed default values
im->bytes_per_pixel=1;
im->font=16;
im->color=15;
im->print_mode=3;
im->background_color=0;
im->draw_ta=0.0; im->draw_scale=1.0;

//revert to mode's set values
switch (bpp){
case 0:
im->bits_per_pixel=16; im->bytes_per_pixel=2;
im->color=7;
im->text=1;
im->cursor_show=0; im->cursor_firstvalue=4; im->cursor_lastvalue=4;
break;
case 1:
im->bits_per_pixel=2;
im->font=8;
im->color=3;
break;
case 2:
im->bits_per_pixel=1; 
im->font=8;//it gets stretched from 8 to 16 later
im->color=1;
break;
case 7:
im->bits_per_pixel=4;
im->font=8;
break;
case 8:
im->bits_per_pixel=4;
im->font=8;
break;
case 9:
im->bits_per_pixel=4;
im->font=14;
break;
case 10:
im->bits_per_pixel=2;
im->font=14;
im->color=3;
break;
case 11:
im->bits_per_pixel=1;
im->color=1;
break;
case 12:
im->bits_per_pixel=4;
break;
case 13:
im->bits_per_pixel=8;
im->font=8;
break;
case 256:
im->bits_per_pixel=8;
break;
case 32:
im->bits_per_pixel=32; im->bytes_per_pixel=4;
im->color=0xFFFFFFFF;
im->background_color=0xFF000000;
break;
};
im->draw_color=im->color;

//revert palette
if (bpp!=32){
restorepalette(im);
im->transparent_color=-1;
}

//revert calculatable values
if (im->bits_per_pixel<32) im->mask=(1<<im->bits_per_pixel)-1; else im->mask=0xFFFFFFFF;
//text
im->cursor_x=1; im->cursor_y=1;
im->top_row=1;
if (bpp) im->bottom_row=(im->height/im->font); else im->bottom_row=im->height;
im->bottom_row--; if (im->bottom_row<=0) im->bottom_row=1;
if (!bpp) return;
//graphics
//clipping/scaling
im->x=((double)im->width)/2.0; im->y=((double)im->height)/2.0;
im->view_x2=im->width-1; im->view_y2=im->height-1;
im->scaling_x=1; im->scaling_y=1;
im->window_x2=im->view_x2; im->window_y2=im->view_y2;

//clear
if (bpp){//graphics
memset(im->offset,0,im->width*im->height*im->bytes_per_pixel);
}else{//text
static int32 i2,i3;
static uint16 *sp;
i3=im->width*im->height; sp=(uint16*)im->offset; for (i2=0;i2<i3;i2++){*sp++=0x0720;}
}

}//imgrevert

int32 imgframe(uint8 *o,int32 x,int32 y,int32 bpp){
static int32 i;
static img_struct *im;
if (x<=0||y<=0) return 0;
i=newimg();
im=&img[i];
im->offset=o;
im->width=x; im->height=y;

//assume default values
im->bytes_per_pixel=1;
im->font=16;
im->color=15;
im->compatible_mode=bpp;
im->print_mode=3;
im->draw_ta=0.0; im->draw_scale=1.0;

//set values
switch (bpp){
case 0:
im->bits_per_pixel=16; im->bytes_per_pixel=2;
im->color=7;
im->text=1;
im->cursor_show=0; im->cursor_firstvalue=4; im->cursor_lastvalue=4;
break;
case 1:
im->bits_per_pixel=2;
im->font=8;
im->color=3;
break;
case 2:
im->bits_per_pixel=1; 
im->font=8;//it gets stretched from 8 to 16 later
im->color=1;
break;
case 7:
im->bits_per_pixel=4;
im->font=8;
break;
case 8:
im->bits_per_pixel=4;
im->font=8;
break;
case 9:
im->bits_per_pixel=4;
im->font=14;
break;
case 10:
im->bits_per_pixel=2;
im->font=14;
im->color=3;
break;
case 11:
im->bits_per_pixel=1;
im->color=1;
break;
case 12:
im->bits_per_pixel=4;
break;
case 13:
im->bits_per_pixel=8;
im->font=8;
break;
case 256:
im->bits_per_pixel=8;
break;
case 32:
im->bits_per_pixel=32; im->bytes_per_pixel=4;
im->color=0xFFFFFFFF;
im->background_color=0xFF000000;
break;
default:
return 0;
};
im->draw_color=im->color;

//attach palette
if (bpp!=32){
im->pal=(uint32*)calloc(256,4);
if (!im->pal){
freeimg(i);
return 0;
}
im->flags|=IMG_FREEPAL;
restorepalette(im);
im->transparent_color=-1;
}

//set calculatable values
if (im->bits_per_pixel<32) im->mask=(1<<im->bits_per_pixel)-1; else im->mask=0xFFFFFFFF;
//text
im->cursor_x=1; im->cursor_y=1;
im->top_row=1;
if (bpp) im->bottom_row=(im->height/im->font); else im->bottom_row=im->height;
im->bottom_row--; if (im->bottom_row<=0) im->bottom_row=1;
if (!bpp) return i;
//graphics
//clipping/scaling
im->x=((double)im->width)/2.0; im->y=((double)im->height)/2.0;
im->view_x2=im->width-1; im->view_y2=im->height-1;
im->scaling_x=1; im->scaling_y=1;
im->window_x2=im->view_x2; im->window_y2=im->view_y2;

return i;
}

void sub__freeimage(int32 i,int32 passed);//forward ref

int32 imgnew(int32 x,int32 y,int32 bpp){
static int32 i,i2,i3;
static img_struct *im;
static uint16 *sp;
static uint32 *lp;
i=imgframe(NULL,x,y,bpp);
if (!i) return 0;
im=&img[i];
if (bpp){//graphics
if (bpp==32){
if (!cblend) init_blend();
im->offset=(uint8*)calloc(x*y,4);
if (!im->offset){sub__freeimage(-i,1); return 0;}
//i3=x*y; lp=im->offset32; for (i2=0;i2<i3;i2++){*lp++=0xFF000000;}
}else{
im->offset=(uint8*)calloc(x*y*im->bytes_per_pixel,1);
if (!im->offset){sub__freeimage(-i,1); return 0;}
}
}else{//text
im->offset=(uint8*)malloc(x*y*im->bytes_per_pixel);
if (!im->offset){sub__freeimage(-i,1); return 0;}
i3=x*y; sp=(uint16*)im->offset; for (i2=0;i2<i3;i2++){*sp++=0x0720;}
}
im->flags|=IMG_FREEMEM;
return i;
}

void sub__font(int32 f,int32 i,int32 passed);//foward def


int32 imgload(char *filename,int32 bpp){
static int32 i,i2,x,y,i3,z2,z3,v,v2,v3,r,g,b,a,t,needt,t2;
static uint8 *cp,*cp2;
static uint32 c;
static uint32 *lp;

static uint8 *sr=(uint8*)malloc(256);
static uint8 *sg=(uint8*)malloc(256);
static uint8 *sb=(uint8*)malloc(256);
static uint8 *dr=(uint8*)malloc(256);
static uint8 *dg=(uint8*)malloc(256);
static uint8 *db=(uint8*)malloc(256);
static uint8 *link=(uint8*)malloc(256);
static int32 *usedcolor=(int32*)malloc(1024);

ts=IMG_Load(filename);
if (!ts) return 0;

if (bpp==-1){

if (write_page->bytes_per_pixel==1){
if (ts->format->BytesPerPixel==1) goto compatible;

//32-->8 bit (best possible color selection)
ts2=SDL_ConvertSurface(ts,&pixelformat32,NULL);
if (!ts2){SDL_FreeSurface(ts); return 0;}
i=imgnew(ts2->w,ts2->h,write_page->compatible_mode);
if (!i){SDL_FreeSurface(ts); return 0;}
//copy write_page's palette
memcpy(img[i].pal,write_page->pal,1024);
//find number of colors
z3=write_page->mask+1;
//build color value table
for (i3=0;i3<z3;i3++){
c=write_page->pal[i3];
db[i3]=c&0xFF; dg[i3]=c>>8&0xFF; dr[i3]=c>>16&0xFF;
}

//reset color used flags
memset(usedcolor,0,1024);
needt=0;
//copy/change colors
cp=(uint8*)ts2->pixels; cp2=img[i].offset;
for (y=0;y<img[i].height;y++){
for (x=0;x<img[i].width;x++){
c=*((uint32*)(cp+y*ts2->pitch+x*4));
a=c>>24;
if (a==0){
needt=1;
}else{
b=c&0xFF; g=c>>8&0xFF; r=c>>16&0xFF; v=1000; v3=0;
for (i3=0;i3<z3;i3++){
v2=abs(r-(int32)dr[i3])+abs(g-(int32)dg[i3])+abs(b-(int32)db[i3]);
if (v2<v){v3=i3; v=v2;}
}//i3
cp2[y*img[i].width+x]=v3;
usedcolor[v3]++;
}//a==0
}}
//add transparency
if (needt){
//find best transparent color
v=0x7FFFFFFF;
for (x=0;x<z3;x++){
if (usedcolor[x]<=v){
v=usedcolor[x];
t=x;
}
}
//remake with transparency
img[i].transparent_color=t;
//copy/change colors
cp=(uint8*)ts2->pixels; cp2=img[i].offset;
for (y=0;y<img[i].height;y++){ for (x=0;x<img[i].width;x++){
c=*((uint32*)(cp+y*ts2->pitch+x*4));
a=c>>24; if (a==0){cp2[y*img[i].width+x]=t; goto usedtranscol;}
b=c&0xFF; g=c>>8&0xFF; r=c>>16&0xFF; v=1000; v3=0;
for (i3=0;i3<z3;i3++){
if (i3!=t){
v2=abs(r-(int32)dr[i3])+abs(g-(int32)dg[i3])+abs(b-(int32)db[i3]);
if (v2<v){v3=i3; v=v2;}
}
}//i3
cp2[y*img[i].width+x]=v3;
usedtranscol:;
}}
}//needt
//adopt font
sub__font(write_page->font,-i,1);
//adopt colors
img[i].color=write_page->color;
img[i].background_color=write_page->background_color;
//adopt print mode
img[i].print_mode=write_page->print_mode;
SDL_FreeSurface(ts2);
SDL_FreeSurface(ts);
return i;
}//write_page->bytes_per_pixel==1
}//-1

if (bpp==256){
if (ts->format->BytesPerPixel!=1){SDL_FreeSurface(ts); return 0;}
compatible:
ts2=ts;
//check for transparent color in palette
ts=SDL_ConvertSurface(ts2,&pixelformat32,NULL);
if (!ts){SDL_FreeSurface(ts2); return 0;}
//prepare image to write to
if (bpp==-1){
i=imgnew(ts2->w,ts2->h,write_page->compatible_mode);
}else{
i=imgnew(ts2->w,ts2->h,256);
}
if (!i){SDL_FreeSurface(ts2); SDL_FreeSurface(ts); return 0;}
//does a transparent pixel exist?
t=-1;
for (y=0;y<img[i].height;y++){
lp=(uint32*)(((char*)ts->pixels)+ts->pitch*y);
for (x=0;x<img[i].width;x++){
if (!(*lp++&0xFF000000)){//alpha==0
//find equivalent 8-bit index
c=*(((uint8*)ts2->pixels)+ts2->pitch*y+x);
if (c<ts2->format->palette->ncolors){
img[i].transparent_color=c;
t=c;
goto found_transparent_color;
}
}
}}
found_transparent_color:

//8-->8 bit (best color match)
if (bpp==-1){
img[i].transparent_color=-1;//this will be set later if necessary
//copy write_page's palette
memcpy(img[i].pal,write_page->pal,1024);
//map image's palette to actual palette
//reset color used flags
memset(usedcolor,0,1024);
//find number of colors
z2=ts2->format->palette->ncolors;
z3=write_page->mask+1;
//build color value tables
for (i2=0;i2<z2;i2++){
c=*(uint32*)&ts2->format->palette->colors[i2];
sr[i2]=c&0xFF; sg[i2]=c>>8&0xFF; sb[i2]=c>>16&0xFF;
}
for (i3=0;i3<z3;i3++){
c=write_page->pal[i3];
db[i3]=c&0xFF; dg[i3]=c>>8&0xFF; dr[i3]=c>>16&0xFF;
}
//link colors to best matching color
for (i2=0;i2<z2;i2++){
v=1000; link[i2]=0;
for (i3=0;i3<z3;i3++){
v2=abs((int32)sr[i2]-(int32)dr[i3])+abs((int32)sg[i2]-(int32)dg[i3])+abs((int32)sb[i2]-(int32)db[i3]);
if (v2<v){
link[i2]=i3; v=v2;
}
}//i3
}//i2
//change colors
needt=0;
cp=(uint8*)ts2->pixels; cp2=img[i].offset;
for (y=0;y<img[i].height;y++){
for (x=0;x<img[i].width;x++){
c=cp[y*ts2->pitch+x];
if (c==t){
needt=1;
}else{
c=link[c];
cp2[y*img[i].width+x]=c;
usedcolor[c]++;
}
}}
//add transparency
if (needt){
t2=t;//backup
//find best transparent color
v=0x7FFFFFFF;
for (x=0;x<z3;x++){
if (usedcolor[x]<=v){
v=usedcolor[x];
t=x;
}
}
//remake with transparency
img[i].transparent_color=t;
//relink colors to best matching color (avoiding t)
for (i2=0;i2<z2;i2++){
v=1000; link[i2]=0;
for (i3=0;i3<z3;i3++){
if (i3!=t){
v2=abs((int32)sr[i2]-(int32)dr[i3])+abs((int32)sg[i2]-(int32)dg[i3])+abs((int32)sb[i2]-(int32)db[i3]);
if (v2<v){
link[i2]=i3; v=v2;
}
}
}//i3
}//i2
//change colors
cp=(uint8*)ts2->pixels; cp2=img[i].offset;
for (y=0;y<img[i].height;y++){
for (x=0;x<img[i].width;x++){
c=cp[y*ts2->pitch+x];
if (c==t2){
cp2[y*img[i].width+x]=t;
}else{
cp2[y*img[i].width+x]=link[c];
}
}}
}//needt
//adopt font
sub__font(write_page->font,-i,1);
//adopt colors
img[i].color=write_page->color;
img[i].background_color=write_page->background_color;
//adopt print mode
img[i].print_mode=write_page->print_mode;
SDL_FreeSurface(ts2);
SDL_FreeSurface(ts);
return i;
}//bpp==-1

//copy pixel data
cp=(uint8*)ts2->pixels; cp2=img[i].offset;
for (i2=0;i2<img[i].height;i2++){
memcpy(cp2,cp,ts2->w);
cp+=ts2->pitch;
cp2+=img[i].width;
}
//update palette
for (i2=ts2->format->palette->ncolors;i2<256;i2++){img[i].pal[i2]=0xFF000000;}
for (i2=0;i2<ts2->format->palette->ncolors;i2++){
c=*(uint32*)&ts2->format->palette->colors[i2];
c=0xFF000000+((c>>16)&255)+(c&0xFF00)+((c&255)<<16);
img[i].pal[i2]=c;
}
SDL_FreeSurface(ts2);
SDL_FreeSurface(ts);
return i;
}

ts2=SDL_ConvertSurface(ts,&pixelformat32,NULL);
if (!ts2){SDL_FreeSurface(ts); return 0;}
i=imgnew(ts2->w,ts2->h,32);
if (!i){SDL_FreeSurface(ts2); SDL_FreeSurface(ts); return 0;}
memcpy(img[i].offset,ts2->pixels,ts2->w*ts2->h*4);
SDL_FreeSurface(ts2); SDL_FreeSurface(ts);
return i;
}

void sub__putimage(double f_dx1,double f_dy1,double f_dx2,double f_dy2,int32 src,int32 dst,double f_sx1,double f_sy1,double f_sx2,double f_sy2,int32 passed){
//format & passed bits:

/* 'passed' flag map
_PUTIMAGE , , , (1, 1)-STEP(1, 1) '64+256+[512]
_PUTIMAGE , , , (1, 1)-(1, 1) '64+[256]
_PUTIMAGE , , , STEP(1, 1) '64+[128]
_PUTIMAGE , , , (1, 1) '[64]
_PUTIMAGE , , 1 '[32]
_PUTIMAGE STEP(0, 0)-STEP(1, 1), 1 '1+2+4+8+[16]
_PUTIMAGE STEP(0, 0)-(1, 1), 1 '1+2+4+[8]
_PUTIMAGE STEP(0, 0)-(1, 1) '1+2+[4]
_PUTIMAGE STEP(0, 0) '1+[2]
_PUTIMAGE (0, 0) '[1]
*/

//old 'passed' flag map:
//[(dx1,dy1)[-(dx2,dy2)]][,[src][,[dst][,[(sx1,sy1)[-(sx2,sy2)]][,...?...]]]]
//  1          2            4      8       16         32          

//changes to 'passed' flag mapping:
//&1->&1
//dstep1->&2
//&2->&4
//&4->&8
//dstep2->&16
//&8->&32
//&16->&64
//sstep1->&128
//&32->&256
//sstep2->&512

static int32 w,h,sskip,dskip,x,y,xx,yy,z,x2,y2,dbpp,sbpp;
static img_struct *s,*d;
static uint32 *soff32,*doff32,col,clearcol,destcol;
static uint8 *soff,*doff;
static uint8 *cp;
static int32 xdir,ydir,no_stretch,no_clip,no_reverse,flip,mirror;
static double mx,my,fx,fy,fsx1,fsy1,fsx2,fsy2,dv,dv2;
static int32 sx1,sy1,sx2,sy2,dx1,dy1,dx2,dy2;
static int32 sw,sh,dw,dh;
static uint32 *pal;
static uint32 *ulp;

no_stretch=0; no_clip=0; no_reverse=1;

flip=0; mirror=0;

if (passed&8){//src
 //validate
 if (src>=0){
 validatepage(src); s=&img[page[src]];
 }else{
 src=-src;
 if (src>=nextimg){error(258); return;}
 s=&img[src];
 if (!s->valid){error(258); return;}
 }
}else{
 s=read_page;
}//src
if (s->text){error(5); return;}
sbpp=s->bytes_per_pixel;

if (passed&32){//dst
 //validate
 if (dst>=0){
 validatepage(dst); d=&img[page[dst]];
 }else{
 dst=-dst;
 if (dst>=nextimg){error(258); return;}
 d=&img[dst];
 if (!d->valid){error(258); return;}
 }
}else{
 d=write_page;
}//dst
if (d->text){error(5); return;}
dbpp=d->bytes_per_pixel;
if ((sbpp==4)&&(dbpp==1)){error(5); return;}
if (s==d){error(5); return;}//cannot put source onto itself!

//quick references
sw=s->width; sh=s->height; dw=d->width; dh=d->height;

//resolve coordinates
if (passed&1){//dx1,dy1
if (d->clipping_or_scaling){
if (d->clipping_or_scaling==2){
dx1=qbr_float_to_long(f_dx1*d->scaling_x+d->scaling_offset_x)+d->view_offset_x;
dy1=qbr_float_to_long(f_dy1*d->scaling_y+d->scaling_offset_y)+d->view_offset_y;
}else{
dx1=qbr_float_to_long(f_dx1)+d->view_offset_x; dy1=qbr_float_to_long(f_dy1)+d->view_offset_y;
}
}else{
dx1=qbr_float_to_long(f_dx1); dy1=qbr_float_to_long(f_dy1);
}
 //note: dx2 & dy2 cannot be passed if dx1 & dy1 weren't passed
 if (passed&4){//dx2,dy2
 if (d->clipping_or_scaling){
 if (d->clipping_or_scaling==2){
 dx2=qbr_float_to_long(f_dx2*d->scaling_x+d->scaling_offset_x)+d->view_offset_x;
 dy2=qbr_float_to_long(f_dy2*d->scaling_y+d->scaling_offset_y)+d->view_offset_y;
 }else{
 dx2=qbr_float_to_long(f_dx2)+d->view_offset_x; dy2=qbr_float_to_long(f_dy2)+d->view_offset_y;
 }
 }else{
 dx2=qbr_float_to_long(f_dx2); dy2=qbr_float_to_long(f_dy2);
 }
 }else{//dx2,dy2
 dx2=0; dy2=0;
 }//dx2,dy2
}else{//dx1,dy1
dx1=0; dy1=0; dx2=0; dy2=0;
}//dx1,dy1

if (passed&64){//sx1,sy1
if (s->clipping_or_scaling){
if (s->clipping_or_scaling==2){
sx1=qbr_float_to_long(f_sx1*s->scaling_x+s->scaling_offset_x)+s->view_offset_x;
sy1=qbr_float_to_long(f_sy1*s->scaling_y+s->scaling_offset_y)+s->view_offset_y;
}else{
sx1=qbr_float_to_long(f_sx1)+s->view_offset_x; sy1=qbr_float_to_long(f_sy1)+s->view_offset_y;
}
}else{
sx1=qbr_float_to_long(f_sx1); sy1=qbr_float_to_long(f_sy1);
}
 //note: sx2 & sy2 cannot be passed if sx1 & sy1 weren't passed
 if (passed&256){//sx2,sy2
 if (s->clipping_or_scaling){
 if (s->clipping_or_scaling==2){
 sx2=qbr_float_to_long(f_sx2*s->scaling_x+s->scaling_offset_x)+s->view_offset_x;
 sy2=qbr_float_to_long(f_sy2*s->scaling_y+s->scaling_offset_y)+s->view_offset_y;
 }else{
 sx2=qbr_float_to_long(f_sx2)+s->view_offset_x; sy2=qbr_float_to_long(f_sy2)+s->view_offset_y;
 }
 }else{
 sx2=qbr_float_to_long(f_sx2); sy2=qbr_float_to_long(f_sy2);
 }
 }else{//sx2,sy2
 sx2=0; sy2=0;
 }//sx2,sy2
}else{//sx1,sy1
sx1=0; sy1=0; sx2=0; sy2=0;
}//sx1,sy1

if ((passed&4)&&(passed&256)){//all co-ords given
//could be stretched
 if ( (abs(dx2-dx1)==abs(sx2-sx1)) && (abs(dy2-dy1)==abs(sy2-sy1)) ){//non-stretched
 //could be flipped/reversed
 //could need clipping
 goto reverse;
 }
goto stretch;
}

if (passed&4){//(dx1,dy1)-(dx2,dy2)...
if (passed&64){//(dx1,dy1)-(dx2,dy2),...,(sx1,sy1)
sx2=sx1+abs(dx2-dx1); sy2=sy1+abs(dy2-dy1);
//can't be stretched
//could be flipped/reversed
//could need clipping
goto reverse;
}else{//(dx1,dy1)-(dx2,dy2)
sx2=sw-1; sy2=sh-1;
//could be stretched
 if ( ((abs(dx2-dx1)+1)==sw) && ((abs(dy2-dy1)+1)==sh) ){//non-stretched
 //could be flipped/reversed
 //could need clipping
 goto reverse;
 }
goto stretch;
}//16
}//2

if (passed&256){//...(sx1,sy1)-(sx2,sy2)
if (passed&1){//(dx1,dy1),,(sx1,sy1)-(sx2,sy2)
dx2=dx1+abs(sx2-sx1); dy2=dy1+abs(sy2-sy1);
//can't be stretched
//could be flipped/reversed
//could need clipping
goto reverse;
}else{//(sx1,sy1)-(sx2,sy2)
dx2=dw-1; dy2=dh-1;
//could be stretched
 if ( ((abs(sx2-sx1)+1)==dw) && ((abs(sy2-sy1)+1)==dh) ){//non-stretched
 //could be flipped/reversed
 //could need clipping
 goto reverse;
 }
goto stretch;
}//1
}//32

if (passed&64){error(5); return;}//Invalid: NULL-NULL,?,?,(sx1,sy1)-NULL

if (passed&1){//(dx1,dy1)
sx2=s->width-1; sy2=s->height-1;
dx2=dx1+sx2; dy2=dy1+sy2;
goto clip;
}

//no co-ords given
sx2=s->width-1; sy2=s->height-1;
dx2=d->width-1; dy2=d->height-1;
if ((sx2==dx2)&&(sy2==dy2)){//non-stretched
//note: because 0-size image is illegal, no null size check is necessary
goto noflip;//cannot be reversed
}
 //precalculate required values
 w=dx2-dx1; h=dy2-dy1;
 fsx1=sx1; fsy1=sy1; fsx2=sx2; fsy2=sy2;
 //"pull" corners so all source pixels are evenly represented in dest rect
 if (fsx1<=fsx2){fsx1-=0.499999; fsx2+=0.499999;}else{fsx1+=0.499999; fsx2-=0.499999;}
 if (fsy1<=fsy2){fsy1-=0.499999; fsy2+=0.499999;}else{fsy1+=0.499999; fsy2-=0.499999;}
 //calc source gradients
 if (w) mx=(fsx2-fsx1)/((double)w); else mx=0.0;
 if (h) my=(fsy2-fsy1)/((double)h); else my=0.0;
 //note: mx & my represent the amount of change per dest pixel
goto stretch_noreverse_noclip;

stretch:
//stretch is required

//mirror?
if (dx2<dx1){
if (sx2>sx1) mirror=1;
}
if (sx2<sx1){
if (dx2>dx1) mirror=1;
}
if (dx2<dx1){x=dx1; dx1=dx2; dx2=x;}
if (sx2<sx1){x=sx1; sx1=sx2; sx2=x;}
//flip?
if (dy2<dy1){
if (sy2>sy1) flip=1;
}
if (sy2<sy1){
if (dy2>dy1) flip=1;
}
if (dy2<dy1){y=dy1; dy1=dy2; dy2=y;}
if (sy2<sy1){y=sy1; sy1=sy2; sy2=y;}

w=dx2-dx1; h=dy2-dy1;
fsx1=sx1; fsy1=sy1; fsx2=sx2; fsy2=sy2;
//"pull" corners so all source pixels are evenly represented in dest rect
if (fsx1<=fsx2){fsx1-=0.499999; fsx2+=0.499999;}else{fsx1+=0.499999; fsx2-=0.499999;}
if (fsy1<=fsy2){fsy1-=0.499999; fsy2+=0.499999;}else{fsy1+=0.499999; fsy2-=0.499999;}
//calc source gradients
if (w) mx=(fsx2-fsx1)/((double)w); else mx=0.0;
if (h) my=(fsy2-fsy1)/((double)h); else my=0.0;
//note: mx & my represent the amount of change per dest pixel

//crop dest offscreen pixels
if (dx1<0){
if (mirror) fsx2+=((double)dx1)*mx; else fsx1-=((double)dx1)*mx;
dx1=0;
}
if (dy1<0){
if (flip) fsy2+=((double)dy1)*my; else fsy1-=((double)dy1)*my;
dy1=0;
}
if (dx2>=dw){
if (mirror) fsx1+=((double)(dx2-dw+1))*mx; else fsx2-=((double)(dx2-dw+1))*mx;
dx2=dw-1;
}
if (dy2>=dh){
if (flip) fsy1+=((double)(dy2-dh+1))*my; else fsy2-=((double)(dy2-dh+1))*my;
dy2=dh-1;
}
//crop source offscreen pixels
if (w){//gradient cannot be 0
if (fsx1<-0.4999999){
x=(-fsx1-0.499999)/mx+1.0;
if (mirror) dx2-=x; else dx1+=x;
fsx1+=((double)x)*mx;
}
if (fsx2>(((double)sw)-0.5000001)){
x=(fsx2-(((double)sw)-0.500001))/mx+1.0;
if (mirror) dx1+=x; else dx2-=x;
fsx2-=(((double)x)*mx);
}
}//w
if (h){//gradient cannot be 0
if (fsy1<-0.4999999){
y=(-fsy1-0.499999)/my+1.0;
if (flip) dy2-=y; else dy1+=y;
fsy1+=((double)y)*my;
}
if (fsy2>(((double)sh)-0.5000001)){
y=(fsy2-(((double)sh)-0.500001))/my+1.0;
if (flip) dy1+=y; else dy2-=y;
fsy2-=(((double)y)*my);
}
}//h
//<0-size/offscreen?
//note: <0-size will cause reversal of dest
//      offscreen values will result in reversal of dest
if (dx1>dx2) return;
if (dy1>dy2) return;
//all values are now within the boundries of the source & dest

stretch_noreverse_noclip:
w=dx2-dx1+1; h=dy2-dy1+1;//recalculate based on actual number of pixels

if (sbpp==4){
if (s->alpha_disabled||d->alpha_disabled) goto put_32_noalpha_stretch;
goto put_32_stretch;
}
if (dbpp==1){
if (s->transparent_color==-1) goto put_8_stretch;
goto put_8_clear_stretch;
}
if (s->transparent_color==-1) goto put_8_32_stretch;
goto put_8_32_clear_stretch;

put_32_stretch:
//calc. starting points & change values
if (flip){
 if (mirror){
  doff32=d->offset32+(dy2*dw+dx2);
  dskip=-dw+w;
 }else{
  doff32=d->offset32+(dy2*dw+dx1);
  dskip=-dw-w;
 }
}else{
 if (mirror){
  doff32=d->offset32+(dy1*dw+dx2);
  dskip=dw+w;
 }else{
  doff32=d->offset32+(dy1*dw+dx1);
  dskip=dw-w;
 }
}
if (mirror) xdir=-1; else xdir=1;
//plot rect
yy=h;
fy=fsy1;
fsx1-=mx;//prev value is moved on from
do{
xx=w;
ulp=s->offset32+sw*qbr_double_to_long(fy);
fx=fsx1;
do{
//--------plot pixel--------
switch((col=*(ulp+qbr_double_to_long(fx+=mx)))&0xFF000000){
case 0xFF000000:
 *doff32=col;
break;
case 0x0:
break;
case 0x80000000:
 *doff32=(((*doff32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend128[*doff32>>24]<<24);
break; 
case 0x7F000000:
 *doff32=(((*doff32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend127[*doff32>>24]<<24);
break;
default:
 destcol=*doff32;
 cp=cblend+(col>>24<<16);
 *doff32=
   cp[(col<<8&0xFF00)+(destcol&255)    ]
 +(cp[(col&0xFF00)   +(destcol>>8&255) ]<<8)
 +(cp[(col>>8&0xFF00)+(destcol>>16&255)]<<16)
 +(ablend[(col>>24)+(destcol>>16&0xFF00)]<<24);
};//switch
//--------done plot pixel--------
doff32+=xdir;
}while(--xx);
doff32+=dskip;
fy+=my;
}while(--yy);
return;

put_32_noalpha_stretch:
//calc. starting points & change values
if (flip){
 if (mirror){
  doff32=d->offset32+(dy2*dw+dx2);
  dskip=-dw+w;
 }else{
  doff32=d->offset32+(dy2*dw+dx1);
  dskip=-dw-w;
 }
}else{
 if (mirror){
  doff32=d->offset32+(dy1*dw+dx2);
  dskip=dw+w;
 }else{
  doff32=d->offset32+(dy1*dw+dx1);
  dskip=dw-w;
 }
}
if (mirror) xdir=-1; else xdir=1;
//plot rect
yy=h;
fy=fsy1;
fsx1-=mx;//prev value is moved on from
doff32-=xdir;
do{
xx=w;
ulp=s->offset32+sw*qbr_double_to_long(fy);
fx=fsx1;
do{
*(doff32+=xdir)=*(ulp+qbr_double_to_long(fx+=mx));
}while(--xx);
doff32+=dskip;
fy+=my;
}while(--yy);
return;

put_8_stretch:
//calc. starting points & change values
if (flip){
 if (mirror){
  doff=d->offset+(dy2*dw+dx2);
  dskip=-dw+w;
 }else{
  doff=d->offset+(dy2*dw+dx1);
  dskip=-dw-w;
 }
}else{
 if (mirror){
  doff=d->offset+(dy1*dw+dx2);
  dskip=dw+w;
 }else{
  doff=d->offset+(dy1*dw+dx1);
  dskip=dw-w;
 }
}
if (mirror) xdir=-1; else xdir=1;
//plot rect
yy=h;
fy=fsy1;
fsx1-=mx;//prev value is moved on from
doff-=xdir;
do{
xx=w;
cp=s->offset+sw*qbr_double_to_long(fy);
fx=fsx1;
do{
*(doff+=xdir)=*(cp+qbr_double_to_long(fx+=mx));
}while(--xx);
doff+=dskip;
fy+=my;
}while(--yy);
return;

put_8_clear_stretch:
clearcol=s->transparent_color;
//calc. starting points & change values
if (flip){
 if (mirror){
  doff=d->offset+(dy2*dw+dx2);
  dskip=-dw+w;
 }else{
  doff=d->offset+(dy2*dw+dx1);
  dskip=-dw-w;
 }
}else{
 if (mirror){
  doff=d->offset+(dy1*dw+dx2);
  dskip=dw+w;
 }else{
  doff=d->offset+(dy1*dw+dx1);
  dskip=dw-w;
 }
}
if (mirror) xdir=-1; else xdir=1;
//plot rect
yy=h;
fy=fsy1;
fsx1-=mx;//prev value is moved on from
do{
xx=w;
cp=s->offset+sw*qbr_double_to_long(fy);
fx=fsx1;
do{
if ((col=*(cp+qbr_double_to_long(fx+=mx)))!=clearcol){
*doff=col;
}
doff+=xdir;
}while(--xx);
doff+=dskip;
fy+=my;
}while(--yy);
return;

put_8_32_stretch:
pal=s->pal;
//calc. starting points & change values
if (flip){
 if (mirror){
  doff32=d->offset32+(dy2*dw+dx2);
  dskip=-dw+w;
 }else{
  doff32=d->offset32+(dy2*dw+dx1);
  dskip=-dw-w;
 }
}else{
 if (mirror){
  doff32=d->offset32+(dy1*dw+dx2);
  dskip=dw+w;
 }else{
  doff32=d->offset32+(dy1*dw+dx1);
  dskip=dw-w;
 }
}
if (mirror) xdir=-1; else xdir=1;
//plot rect
yy=h;
fy=fsy1;
fsx1-=mx;//prev value is moved on from
doff32-=xdir;
do{
xx=w;
cp=s->offset+sw*qbr_double_to_long(fy);
fx=fsx1;
do{
*(doff32+=xdir)=pal[*(cp+qbr_double_to_long(fx+=mx))];
}while(--xx);
doff32+=dskip;
fy+=my;
}while(--yy);
return;

put_8_32_clear_stretch:
clearcol=s->transparent_color;
pal=s->pal;
//calc. starting points & change values
if (flip){
 if (mirror){
  doff32=d->offset32+(dy2*dw+dx2);
  dskip=-dw+w;
 }else{
  doff32=d->offset32+(dy2*dw+dx1);
  dskip=-dw-w;
 }
}else{
 if (mirror){
  doff32=d->offset32+(dy1*dw+dx2);
  dskip=dw+w;
 }else{
  doff32=d->offset32+(dy1*dw+dx1);
  dskip=dw-w;
 }
}
if (mirror) xdir=-1; else xdir=1;
//plot rect
yy=h;
fy=fsy1;
fsx1-=mx;//prev value is moved on from
do{
xx=w;
cp=s->offset+sw*qbr_double_to_long(fy);
fx=fsx1;
do{
if ((col=*(cp+qbr_double_to_long(fx+=mx)))!=clearcol){
*doff32=pal[col];
}
doff32+=xdir;
}while(--xx);
doff32+=dskip;
fy+=my;
}while(--yy);
return;

reverse:
//mirror?
if (dx2<dx1){
if (sx2>sx1) mirror=1;
}
if (sx2<sx1){
if (dx2>dx1) mirror=1;
}
if (dx2<dx1){x=dx1; dx1=dx2; dx2=x;}
if (sx2<sx1){x=sx1; sx1=sx2; sx2=x;}
//flip?
if (dy2<dy1){
if (sy2>sy1) flip=1;
}
if (sy2<sy1){
if (dy2>dy1) flip=1;
}
if (dy2<dy1){y=dy1; dy1=dy2; dy2=y;}
if (sy2<sy1){y=sy1; sy1=sy2; sy2=y;}

clip:
//crop dest offscreen pixels
if (dx1<0){
if (mirror) sx2+=dx1; else sx1-=dx1;
dx1=0;
}
if (dy1<0){
if (flip) sy2+=dy1; else sy1-=dy1;
dy1=0;
}
if (dx2>=dw){
if (mirror) sx1+=(dx2-dw+1); else sx2-=(dx2-dw+1);
dx2=dw-1;
}
if (dy2>=dh){
if (flip) sy1+=(dy2-dh+1); else sy2-=(dy2-dh+1);
dy2=dh-1;
}
//crop source offscreen pixels
if (sx1<0){
if (mirror) dx2+=sx1; else dx1-=sx1;
sx1=0;
}
if (sy1<0){
if (flip) dy2+=sy1; else dy1-=sy1;
sy1=0;
}
if (sx2>=sw){
if (mirror) dx1+=(sx2-sw+1); else dx2-=(sx2-sw+1);
sx2=sw-1;
}
if (sy2>=sh){
if (flip) dy1+=(sy2-sh+1); else dy2-=(sy2-sh+1);
sy2=sh-1;
}
//<0-size/offscreen?
//note: <0-size will cause reversal of dest
//      offscreen values will result in reversal of dest
if (dx1>dx2) return;
if (dy1>dy2) return;
//all values are now within the boundries of the source & dest

//mirror put
if (mirror){
if (sbpp==4){
if (s->alpha_disabled||d->alpha_disabled) goto put_32_noalpha_mirror;
goto put_32_mirror;
}
if (dbpp==1){
if (s->transparent_color==-1) goto put_8_mirror;
goto put_8_clear_mirror;
}
if (s->transparent_color==-1) goto put_8_32_mirror;
goto put_8_32_clear_mirror;
}//mirror put

noflip:
if (sbpp==4){
if (s->alpha_disabled||d->alpha_disabled) goto put_32_noalpha;
goto put_32;
}
if (dbpp==1){
if (s->transparent_color==-1) goto put_8;
goto put_8_clear;
}
if (s->transparent_color==-1) goto put_8_32;
goto put_8_32_clear;

put_32:
w=dx2-dx1+1;
doff32=d->offset32+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff32=s->offset32+(sy2*sw+sx1);
sskip=-w-sw;
}else{
soff32=s->offset32+(sy1*sw+sx1);
sskip=sw-w;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
//--------plot pixel--------
switch((col=*soff32++)&0xFF000000){
case 0xFF000000:
 *doff32++=col;
break;
case 0x0:
 doff32++;
break;
case 0x80000000:
 *doff32++=(((*doff32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend128[*doff32>>24]<<24);
break; 
case 0x7F000000:
 *doff32++=(((*doff32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend127[*doff32>>24]<<24);
break;
default:
 destcol=*doff32;
 cp=cblend+(col>>24<<16);
 *doff32++=
   cp[(col<<8&0xFF00)+(destcol&255)    ]
 +(cp[(col&0xFF00)   +(destcol>>8&255) ]<<8)
 +(cp[(col>>8&0xFF00)+(destcol>>16&255)]<<16)
 +(ablend[(col>>24)+(destcol>>16&0xFF00)]<<24);
};//switch
//--------done plot pixel--------
}while(--xx);
soff32+=sskip; doff32+=dskip;
}while(--h);
return;

put_32_noalpha:
doff32=d->offset32+(dy1*dw+dx1);
if (flip){
soff32=s->offset32+(sy2*sw+sx1);
sskip=-sw;
}else{
soff32=s->offset32+(sy1*sw+sx1);
sskip=sw;
}
h=dy2-dy1+1;
w=(dx2-dx1+1)*4;
while(h--){
memcpy(doff32,soff32,w);
soff32+=sskip; doff32+=dw;
}
return;

put_8:
doff=d->offset+(dy1*dw+dx1);
if (flip){
soff=s->offset+(sy2*sw+sx1);
sskip=-sw;
}else{
soff=s->offset+(sy1*sw+sx1);
sskip=sw;
}
h=dy2-dy1+1;
w=dx2-dx1+1;
while(h--){
memcpy(doff,soff,w);
soff+=sskip; doff+=dw;
}
return;

put_8_clear:
clearcol=s->transparent_color;
w=dx2-dx1+1;
doff=d->offset+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff=s->offset+(sy2*sw+sx1);
sskip=-w-sw;
}else{
soff=s->offset+(sy1*sw+sx1);
sskip=sw-w;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
if ((col=*soff++)!=clearcol){
*doff=col;
}
doff++;
}while(--xx);
soff+=sskip; doff+=dskip;
}while(--h);
return;

put_8_32:
pal=s->pal;
w=dx2-dx1+1;
doff32=d->offset32+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff=s->offset+(sy2*sw+sx1);
sskip=-w-sw;
}else{
soff=s->offset+(sy1*sw+sx1);
sskip=sw-w;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
*doff32++=pal[*soff++];
}while(--xx);
soff+=sskip; doff32+=dskip;
}while(--h);
return;

put_8_32_clear:
pal=s->pal;
clearcol=s->transparent_color;
w=dx2-dx1+1;
doff32=d->offset32+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff=s->offset+(sy2*sw+sx1);
sskip=-w-sw;
}else{
soff=s->offset+(sy1*sw+sx1);
sskip=sw-w;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
if ((col=*soff++)!=clearcol){
*doff32=pal[col];
}
doff32++;
}while(--xx);
soff+=sskip; doff32+=dskip;
}while(--h);
return;

put_32_mirror:
w=dx2-dx1+1;
doff32=d->offset32+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff32=s->offset32+(sy2*sw+sx2);
sskip=-sw+w;
}else{
soff32=s->offset32+(sy1*sw+sx2);
sskip=w+sw;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
//--------plot pixel--------
switch((col=*soff32--)&0xFF000000){
case 0xFF000000:
 *doff32++=col;
break;
case 0x0:
 doff32++;
break;
case 0x80000000:
 *doff32++=(((*doff32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend128[*doff32>>24]<<24);
break; 
case 0x7F000000:
 *doff32++=(((*doff32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend127[*doff32>>24]<<24);
break;
default:
 destcol=*doff32;
 cp=cblend+(col>>24<<16);
 *doff32++=
   cp[(col<<8&0xFF00)+(destcol&255)    ]
 +(cp[(col&0xFF00)   +(destcol>>8&255) ]<<8)
 +(cp[(col>>8&0xFF00)+(destcol>>16&255)]<<16)
 +(ablend[(col>>24)+(destcol>>16&0xFF00)]<<24);
};//switch
//--------done plot pixel--------
}while(--xx);
soff32+=sskip; doff32+=dskip;
}while(--h);
return;

put_32_noalpha_mirror:
w=dx2-dx1+1;
doff32=d->offset32+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff32=s->offset32+(sy2*sw+sx2);
sskip=-sw+w;
}else{
soff32=s->offset32+(sy1*sw+sx2);
sskip=w+sw;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
*doff32++=*soff32--;
}while(--xx);
soff32+=sskip; doff32+=dskip;
}while(--h);
return;

put_8_mirror:
w=dx2-dx1+1;
doff=d->offset+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff=s->offset+(sy2*sw+sx2);
sskip=-sw+w;
}else{
soff=s->offset+(sy1*sw+sx2);
sskip=w+sw;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
*doff++=*soff--;
}while(--xx);
soff+=sskip; doff+=dskip;
}while(--h);
return;

put_8_clear_mirror:
clearcol=s->transparent_color;
w=dx2-dx1+1;
doff=d->offset+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff=s->offset+(sy2*sw+sx2);
sskip=-sw+w;
}else{
soff=s->offset+(sy1*sw+sx2);
sskip=w+sw;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
if ((col=*soff--)!=clearcol){
*doff=col;
}
doff++;
}while(--xx);
soff+=sskip; doff+=dskip;
}while(--h);
return;

put_8_32_mirror:
pal=s->pal;
w=dx2-dx1+1;
doff32=d->offset32+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff=s->offset+(sy2*sw+sx2);
sskip=-sw+w;
}else{
soff=s->offset+(sy1*sw+sx2);
sskip=w+sw;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
*doff32++=pal[*soff--];
}while(--xx);
soff+=sskip; doff32+=dskip;
}while(--h);
return;

put_8_32_clear_mirror:
pal=s->pal;
clearcol=s->transparent_color;
w=dx2-dx1+1;
doff32=d->offset32+(dy1*dw+dx1);
dskip=dw-w;
if (flip){
soff=s->offset+(sy2*sw+sx2);
sskip=-sw+w;
}else{
soff=s->offset+(sy1*sw+sx2);
sskip=w+sw;
}
//plot rect
h=dy2-dy1+1;
do{
xx=w;
do{
if ((col=*soff--)!=clearcol){
*doff32=pal[col];
}
doff32++;
}while(--xx);
soff+=sskip; doff32+=dskip;
}while(--h);
return;

}//_putimage



int32 fontopen(char *name,double d_height,int32 flags){
//flags:
//1 bold TTF_STYLE_BOLD
//2 italic TTF_STYLE_ITALIC
//4 underline TTF_STYLE_UNDERLINE
//8 dontblend (blending is the default in 32-bit alpha-enabled modes)
//16 monospace
//32 unicode
static double d,d2;
static int32 i,y,z,height;
static TTF_Font *tf,*tf2;
static SDL_Surface *ts;
static SDL_Color c;
for (i=32;i<=lastfont;i++){
if (!font[i]){
if (d_height<1.0){
height=d_height*1000;
tf=TTF_OpenFont(name,height);
if (tf==NULL) return 0;
height=TTF_FontHeight(tf);
height-=2; if (height<=0){TTF_CloseFont(tf); return 0;}
}else{
height=qbr_double_to_long(d_height);
height+=2;
tf=TTF_OpenFont(name,1000);
if (tf==NULL) return 0;
d=TTF_FontHeight(tf);
d=1000.0/d;
d2=height;
d2*=d;
y=d2;
TTF_CloseFont(tf);
tf=TTF_OpenFont(name,y);
if (tf==NULL) return 0;
if (TTF_FontHeight(tf)>height){
TTF_CloseFont(tf);
y--; tf=TTF_OpenFont(name,y);
if (tf==NULL) return 0;
}
if (TTF_FontHeight(tf)<height){
TTF_CloseFont(tf);
y++; tf=TTF_OpenFont(name,y);
if (tf==NULL) return 0;
}
height-=2;
}//d_height
TTF_SetFontStyle(tf,flags&7);//returns void, so cannot be error checked
font[i]=tf;
fontflags[i]=flags;
fontheight[i]=height;
fontwidth[i]=0;
if (flags&16){
if (TTF_FontFaceIsFixedWidth(tf)){
//render a glyph to test
z=32;
c.r=255; c.g=255; c.b=255; c.unused=0;//dummy values

convert_text_to_utf16(NULL,&z,1);
ts=TTF_RenderUNICODE_Solid(tf,(Uint16*)unicode16_buf,c);
//ts=TTF_RenderText_Solid(tf,(char*)&z,c);//8-bit, 0=clear, 1=text

fontwidth[i]=ts->w;
SDL_FreeSurface(ts);
}else{
TTF_CloseFont(tf);//not a monospace font
return 0;
}
}
return i;
}
}
return 0;//no valid index, will result in ILLEGAL FUNCTION CALL
}

int32 selectfont(int32 f,img_struct *im){
im->font=f;
im->cursor_x=1; im->cursor_y=1;
im->top_row=1;
if (im->compatible_mode) im->bottom_row=im->height/fontheight[f]; else im->bottom_row=im->height;
im->bottom_row--; if (im->bottom_row<=0) im->bottom_row=1;
return 1;//success
}















SDL_Rect *modes=NULL;
SDL_Rect **sdl_modes;
int32 nmodes=0;
int32 anymode=0;

int32 x_scale=1,y_scale=1;
int32 x_offset=0,y_offset=0;
int32 x_monitor=0,y_monitor=0;

int32 conversion_required=0;
uint32 *conversion_layer=(uint32*)malloc(8);

#define AUDIO_CHANNELS 256

#define sndqueue_lastindex 9999
uint32 sndqueue[sndqueue_lastindex+1];
int32 sndqueue_next=0;
int32 sndqueue_first=0;
int32 sndqueue_wait=-1;
int32 sndqueue_played=0;

uint32 func__sndraw(uint8* data,uint32 bytes);//called by sndsetup
void sndsetup(){
static int32 sndsetup_called=0;
if (!sndsetup_called){
sndsetup_called=1;
if (Mix_OpenAudio(22050,AUDIO_S16,2,1024)==-1) exit(10001);
atexit(Mix_CloseAudio);
Mix_AllocateChannels(AUDIO_CHANNELS);
 //fix "part of first sound missed" problem in SDL_MIXER
 static uint8 *cp=(uint8*)calloc(8192,2*2);
 static int32 i;
 i=func__sndraw(cp,8192*2*2);
 if (i){
 sndqueue[sndqueue_next]=i;
 sndqueue_next++; if (sndqueue_next>sndqueue_lastindex) sndqueue_next=0;
 }
}
}



void call_int(int32 i);

uint32 frame=0;


extern uint8 cmem[1114099];//16*65535+65535+3 (enough for highest referencable dword in conv memory)

struct mouse_message{
int16 x;
int16 y;
uint32 buttons;
int16 movementx;
int16 movementy;
};
mouse_message mouse_messages[65536];//a circular buffer of mouse messages
int32 last_mouse_message=0;
int32 current_mouse_message=0;
int32 mouse_hideshow_called=0;




//x86 Virtual CMEM emulation
//Note: x86 CPU emulation is still experimental and is not available in QB64 yet.
struct cpu_struct{
 //al,ah,ax,eax (unsigned & signed)
 union{
  struct{
   union{
   uint8 al;
   int8 al_signed;
  };
  union{
   uint8 ah;
   int8 ah_signed;
  };
 };
 uint16 ax;
 int16 ax_signed;
 uint32 eax;
 int32 eax_signed;
 };
 //bl,bh,bx,ebx (unsigned & signed)
 union{
  struct{
   union{
   uint8 bl;
   int8 bl_signed;
  };
  union{
   uint8 bh;
   int8 bh_signed;
  };
 };
 uint16 bx;
 int16 bx_signed;
 uint32 ebx;
 int32 ebx_signed;
 };
 //cl,ch,cx,ecx (unsigned & signed)
 union{
  struct{
   union{
   uint8 cl;
   int8 cl_signed;
  };
  union{
   uint8 ch;
   int8 ch_signed;
  };
 };
 uint16 cx;
 int16 cx_signed;
 uint32 ecx;
 int32 ecx_signed;
 };
 //dl,dh,dx,edx (unsigned & signed)
 union{
  struct{
   union{
   uint8 dl;
   int8 dl_signed;
  };
  union{
   uint8 dh;
   int8 dh_signed;
  };
 };
 uint16 dx;
 int16 dx_signed;
 uint32 edx;
 int32 edx_signed;
 };
 //si,esi (unsigned & signed)
 union{
 uint16 si;
 int16 si_signed;
 uint32 esi;
 int32 esi_signed;
 };
 //di,edi (unsigned & signed)
 union{
 uint16 di;
 int16 di_signed;
 uint32 edi;
 int32 edi_signed;
 };
 //bp,ebp (unsigned & signed)
 union{
 uint16 bp;
 int16 bp_signed;
 uint32 ebp;
 int32 ebp_signed;
 };
 //sp,esp (unsigned & signed)
 union{
 uint16 sp;
 int16 sp_signed;
 uint32 esp;
 int32 esp_signed;
 };
 //cs,ss,ds,es,fs,gs (unsigned & signed)
 union{
 uint16 cs;
 uint16 cs_signed;
 };
 union{
 uint16 ss;
 uint16 ss_signed;
 };
 union{
 uint16 ds;
 uint16 ds_signed;
 };
 union{
 uint16 es;
 uint16 es_signed;
 };
 union{
 uint16 fs;
 uint16 fs_signed;
 };
 union{
 uint16 gs;
 uint16 gs_signed;
 };
 //ip,eip (unsigned & signed)
 union{
 uint16 ip;
 uint16 ip_signed;
 uint32 eip;
 uint32 eip_signed;
 };
 //flags
 uint8 overflow_flag;
 uint8 direction_flag;
 uint8 interrupt_flag;
 uint8 trap_flag;
 uint8 sign_flag;
 uint8 zero_flag;
 uint8 auxiliary_flag;
 uint8 parity_flag;
 uint8 carry_flag;
};
cpu_struct cpu;

uint8 *ip;
uint8 *seg;//default segment (DS unless overridden)
uint8 *seg_bp;//the segment bp will be accessed from (SS unless overridden)

uint8 *reg8[8];
uint16 *reg16[8];
uint32 *reg32[8];
uint16 *segreg[8];

int32 a32;
int32 b32;//size of data to read/write in bits is 32


uint32 sib(){
static uint32 i;//sib byte
i=*ip++;
switch(i>>6){
case 0:
return *reg32[i&7]+*reg32[i>>3&7];
break;
case 1:
return *reg32[i&7]+(*reg32[i>>3&7]<<1);
break;
case 2:
return *reg32[i&7]+(*reg32[i>>3&7]<<2);
break;
case 3:
return *reg32[i&7]+(*reg32[i>>3&7]<<3);
break;
}
}

uint32 sib_mod0(){
//Note: Called when top 2 bits of rm byte before sib byte were 0, base register is ignored
//      and replaced with an int32 following the sib byte
static uint32 i;//sib byte
i=*ip++;
if ((i&7)==5){
 switch(i>>6){
 case 0:
 return (*(uint32*)((ip+=4)-4))+*reg32[i>>3&7];
 break;
 case 1:
 return (*(uint32*)((ip+=4)-4))+(*reg32[i>>3&7]<<1);
 break;
 case 2:
 return (*(uint32*)((ip+=4)-4))+(*reg32[i>>3&7]<<2);
 break;
 case 3:
 return (*(uint32*)((ip+=4)-4))+(*reg32[i>>3&7]<<3);
 break;
 }
}
switch(i>>6){
case 0:
return *reg32[i&7]+*reg32[i>>3&7];
break;
case 1:
return *reg32[i&7]+(*reg32[i>>3&7]<<1);
break;
case 2:
return *reg32[i&7]+(*reg32[i>>3&7]<<2);
break;
case 3:
return *reg32[i&7]+(*reg32[i>>3&7]<<3);
break;
}
}

uint8 *rm8(){
static uint32 i;//r/m byte
i=*ip++;
switch(i>>6){
case 3:
 return reg8[i&7];
break;
case 0:
 if (a32){
  switch(i&7){
   case 0: return seg+cpu.ax; break;
   case 1: return seg+cpu.cx; break;
   case 2: return seg+cpu.dx; break;
   case 3: return seg+cpu.bx; break;
   case 4: return seg+(uint16)sib_mod0(); break;
   case 5: return seg+(*(uint16*)((ip+=4)-4)); break;
   case 6: return seg+cpu.si; break;
   case 7: return seg+cpu.di; break;
  }
 }else{
  switch(i&7){
   case 0: return seg+((uint16)(cpu.bx+cpu.si)); break;
   case 1: return seg+((uint16)(cpu.bx+cpu.di)); break;
   case 2: return seg_bp+((uint16)(cpu.bp+cpu.si)); break;
   case 3: return seg_bp+((uint16)(cpu.bp+cpu.di)); break;
   case 4: return seg+cpu.si; break;
   case 5: return seg+cpu.di; break;
   case 6: return seg+(*(uint16*)((ip+=2)-2)); break;
   case 7: return seg+cpu.bx; break;
  }
 }
break;
case 1:
 if (a32){
  switch(i&7){
   case 0: return seg+((uint16)(cpu.eax+*(int8*)ip++)); break;
   case 1: return seg+((uint16)(cpu.ecx+*(int8*)ip++)); break;
   case 2: return seg+((uint16)(cpu.edx+*(int8*)ip++)); break;
   case 3: return seg+((uint16)(cpu.ebx+*(int8*)ip++)); break;
   case 4: i=sib(); return seg+((uint16)(i+*(int8*)ip++)); break;
   case 5: return seg_bp+((uint16)(cpu.ebp+*(int8*)ip++)); break;
   case 6: return seg+((uint16)(cpu.esi+*(int8*)ip++)); break;
   case 7: return seg+((uint16)(cpu.edi+*(int8*)ip++)); break;
  }
 }else{
  switch(i&7){
   case 0: return seg+((uint16)(cpu.bx+cpu.si+*(int8*)ip++)); break;
   case 1: return seg+((uint16)(cpu.bx+cpu.di+*(int8*)ip++)); break;
   case 2: return seg_bp+((uint16)(cpu.bp+cpu.si+*(int8*)ip++)); break;
   case 3: return seg_bp+((uint16)(cpu.bp+cpu.di+*(int8*)ip++)); break;
   case 4: return seg+((uint16)(cpu.si+*(int8*)ip++)); break;
   case 5: return seg+((uint16)(cpu.di+*(int8*)ip++)); break;
   case 6: return seg_bp+((uint16)(cpu.bp+*(int8*)ip++)); break;
   case 7: return seg+((uint16)(cpu.bx+*(int8*)ip++)); break;
  }
 }
break;
case 2:
 if (a32){ 
  switch(i&7){
   case 0: return seg+((uint16)(cpu.eax+*(uint32*)((ip+=4)-4))); break;
   case 1: return seg+((uint16)(cpu.ecx+*(uint32*)((ip+=4)-4))); break;
   case 2: return seg+((uint16)(cpu.edx+*(uint32*)((ip+=4)-4))); break;
   case 3: return seg+((uint16)(cpu.ebx+*(uint32*)((ip+=4)-4))); break;
   case 4: i=sib(); return seg+((uint16)(i+*(uint32*)((ip+=4)-4))); break;
   case 5: return seg_bp+((uint16)(cpu.ebp+*(uint32*)((ip+=4)-4))); break;
   case 6: return seg+((uint16)(cpu.esi+*(uint32*)((ip+=4)-4))); break;
   case 7: return seg+((uint16)(cpu.edi+*(uint32*)((ip+=4)-4))); break;
  }
 }else{
  switch(i&7){
   case 0: return seg+((uint16)(cpu.bx+cpu.si+*(uint16*)((ip+=2)-2))); break;
   case 1: return seg+((uint16)(cpu.bx+cpu.di+*(uint16*)((ip+=2)-2))); break;
   case 2: return seg_bp+((uint16)(cpu.bp+cpu.si+*(uint16*)((ip+=2)-2))); break;
   case 3: return seg_bp+((uint16)(cpu.bp+cpu.di+*(uint16*)((ip+=2)-2))); break;
   case 4: return seg+((uint16)(cpu.si+*(uint16*)((ip+=2)-2))); break;
   case 5: return seg+((uint16)(cpu.di+*(uint16*)((ip+=2)-2))); break;
   case 6: return seg_bp+((uint16)(cpu.bp+*(uint16*)((ip+=2)-2))); break;
   case 7: return seg+((uint16)(cpu.bx+*(uint16*)((ip+=2)-2))); break;
  }
 }
break;
}
}

uint16 *rm16(){
static int32 i;//r/m byte
i=*ip;
switch(i>>6){
case 3:
 ip++; 
 return reg16[i&7];
break;
case 0:
 ip++;
 if (a32){
  switch(i&7){
   case 0: return (uint16*)(seg+cpu.ax); break;
   case 1: return (uint16*)(seg+cpu.cx); break;
   case 2: return (uint16*)(seg+cpu.dx); break;
   case 3: return (uint16*)(seg+cpu.bx); break;   
   case 4: return (uint16*)(seg+(uint16)sib_mod0()); break;
   case 5: return (uint16*)(seg+(*(uint16*)((ip+=4)-4))); break;
   case 6: return (uint16*)(seg+cpu.si); break;
   case 7: return (uint16*)(seg+cpu.di); break;
  }
 }else{
  switch(i&7){
   case 0: return (uint16*)(seg+((uint16)(cpu.bx+cpu.si))); break;
   case 1: return (uint16*)(seg+((uint16)(cpu.bx+cpu.di))); break;
   case 2: return (uint16*)(seg_bp+((uint16)(cpu.bp+cpu.si))); break;
   case 3: return (uint16*)(seg_bp+((uint16)(cpu.bp+cpu.di))); break;
   case 4: return (uint16*)(seg+cpu.si); break;
   case 5: return (uint16*)(seg+cpu.di); break;
   case 6: return (uint16*)(seg+(*(uint16*)((ip+=2)-2))); break;
   case 7: return (uint16*)(seg+cpu.bx); break;
  }
 }
break;
case 1:
 ip++;
 if (a32){ 
  switch(i&7){
   case 0: return (uint16*)(seg+((uint16)(cpu.eax+*(int8*)ip++))); break;
   case 1: return (uint16*)(seg+((uint16)(cpu.ecx+*(int8*)ip++))); break;
   case 2: return (uint16*)(seg+((uint16)(cpu.edx+*(int8*)ip++))); break;
   case 3: return (uint16*)(seg+((uint16)(cpu.ebx+*(int8*)ip++))); break;
   case 4: i=sib(); return (uint16*)(seg+((uint16)(i+*(int8*)ip++))); break;
   case 5: return (uint16*)(seg_bp+((uint16)(cpu.ebp+*(int8*)ip++))); break;
   case 6: return (uint16*)(seg+((uint16)(cpu.esi+*(int8*)ip++))); break;
   case 7: return (uint16*)(seg+((uint16)(cpu.edi+*(int8*)ip++))); break;
  }
 }else{
  switch(i&7){
   case 0: return (uint16*)(seg+((uint16)(cpu.bx+cpu.si+*(int8*)ip++))); break;
   case 1: return (uint16*)(seg+((uint16)(cpu.bx+cpu.di+*(int8*)ip++))); break;
   case 2: return (uint16*)(seg_bp+((uint16)(cpu.bp+cpu.si+*(int8*)ip++))); break;
   case 3: return (uint16*)(seg_bp+((uint16)(cpu.bp+cpu.di+*(int8*)ip++))); break;
   case 4: return (uint16*)(seg+((uint16)(cpu.si+*(int8*)ip++))); break;
   case 5: return (uint16*)(seg+((uint16)(cpu.di+*(int8*)ip++))); break;
   case 6: return (uint16*)(seg_bp+((uint16)(cpu.bp+*(int8*)ip++))); break;
   case 7: return (uint16*)(seg+((uint16)(cpu.bx+*(int8*)ip++))); break;
  }
 }
break;
case 2:
 ip++;
 if (a32){ 
  switch(i&7){
   case 0: return (uint16*)(seg+((uint16)(cpu.eax+*(uint32*)((ip+=4)-4)))); break;
   case 1: return (uint16*)(seg+((uint16)(cpu.ecx+*(uint32*)((ip+=4)-4)))); break;
   case 2: return (uint16*)(seg+((uint16)(cpu.edx+*(uint32*)((ip+=4)-4)))); break;
   case 3: return (uint16*)(seg+((uint16)(cpu.ebx+*(uint32*)((ip+=4)-4)))); break;
   case 4: i=sib(); return (uint16*)(seg+((uint16)(i+*(uint32*)((ip+=4)-4)))); break;
   case 5: return (uint16*)(seg_bp+((uint16)(cpu.ebp+*(uint32*)((ip+=4)-4)))); break;
   case 6: return (uint16*)(seg+((uint16)(cpu.esi+*(uint32*)((ip+=4)-4)))); break;
   case 7: return (uint16*)(seg+((uint16)(cpu.edi+*(uint32*)((ip+=4)-4)))); break;
  }
 }else{
  switch(i&7){
   case 0: return (uint16*)(seg+((uint16)(cpu.bx+cpu.si+*(uint16*)((ip+=2)-2)))); break;
   case 1: return (uint16*)(seg+((uint16)(cpu.bx+cpu.di+*(uint16*)((ip+=2)-2)))); break;
   case 2: return (uint16*)(seg_bp+((uint16)(cpu.bp+cpu.si+*(uint16*)((ip+=2)-2)))); break;
   case 3: return (uint16*)(seg_bp+((uint16)(cpu.bp+cpu.di+*(uint16*)((ip+=2)-2)))); break;
   case 4: return (uint16*)(seg+((uint16)(cpu.si+*(uint16*)((ip+=2)-2)))); break;
   case 5: return (uint16*)(seg+((uint16)(cpu.di+*(uint16*)((ip+=2)-2)))); break;
   case 6: return (uint16*)(seg_bp+((uint16)(cpu.bp+*(uint16*)((ip+=2)-2)))); break;
   case 7: return (uint16*)(seg+((uint16)(cpu.bx+*(uint16*)((ip+=2)-2)))); break;
  }
 }
break;
}
}

uint32 *rm32(){
static int32 i;//r/m byte
i=*ip;
switch(i>>6){
case 3:
 ip++; 
 return reg32[i&7];
break;
case 0:
 ip++;
 if (a32){
  switch(i&7){
   case 0: return (uint32*)(seg+cpu.ax); break;
   case 1: return (uint32*)(seg+cpu.cx); break;
   case 2: return (uint32*)(seg+cpu.dx); break;
   case 3: return (uint32*)(seg+cpu.bx); break;
   case 4: return (uint32*)(seg+(uint16)sib_mod0()); break;
   case 5: return (uint32*)(seg+(*(uint16*)((ip+=4)-4))); break;
   case 6: return (uint32*)(seg+cpu.si); break;
   case 7: return (uint32*)(seg+cpu.di); break;
  }
 }else{
  switch(i&7){
   case 0: return (uint32*)(seg+((uint16)(cpu.bx+cpu.si))); break;
   case 1: return (uint32*)(seg+((uint16)(cpu.bx+cpu.di))); break;
   case 2: return (uint32*)(seg_bp+((uint16)(cpu.bp+cpu.si))); break;
   case 3: return (uint32*)(seg_bp+((uint16)(cpu.bp+cpu.di))); break;
   case 4: return (uint32*)(seg+cpu.si); break;
   case 5: return (uint32*)(seg+cpu.di); break;
   case 6: return (uint32*)(seg+(*(uint16*)((ip+=2)-2))); break;
   case 7: return (uint32*)(seg+cpu.bx); break;
  }
 }
break;
case 1:
 ip++;
 if (a32){ 
  switch(i&7){
   case 0: return (uint32*)(seg+((uint16)(cpu.eax+*(int8*)ip++))); break;
   case 1: return (uint32*)(seg+((uint16)(cpu.ecx+*(int8*)ip++))); break;
   case 2: return (uint32*)(seg+((uint16)(cpu.edx+*(int8*)ip++))); break;
   case 3: return (uint32*)(seg+((uint16)(cpu.ebx+*(int8*)ip++))); break;
   case 4: i=sib(); return (uint32*)(seg+((uint16)(i+*(int8*)ip++))); break;
   case 5: return (uint32*)(seg_bp+((uint16)(cpu.ebp+*(int8*)ip++))); break;
   case 6: return (uint32*)(seg+((uint16)(cpu.esi+*(int8*)ip++))); break;
   case 7: return (uint32*)(seg+((uint16)(cpu.edi+*(int8*)ip++))); break;
  }
 }else{
  switch(i&7){
   case 0: return (uint32*)(seg+((uint16)(cpu.bx+cpu.si+*(int8*)ip++))); break;
   case 1: return (uint32*)(seg+((uint16)(cpu.bx+cpu.di+*(int8*)ip++))); break;
   case 2: return (uint32*)(seg_bp+((uint16)(cpu.bp+cpu.si+*(int8*)ip++))); break;
   case 3: return (uint32*)(seg_bp+((uint16)(cpu.bp+cpu.di+*(int8*)ip++))); break;
   case 4: return (uint32*)(seg+((uint16)(cpu.si+*(int8*)ip++))); break;
   case 5: return (uint32*)(seg+((uint16)(cpu.di+*(int8*)ip++))); break;
   case 6: return (uint32*)(seg_bp+((uint16)(cpu.bp+*(int8*)ip++))); break;
   case 7: return (uint32*)(seg+((uint16)(cpu.bx+*(int8*)ip++))); break;
  }
 }
break;
case 2:
 ip++;
 if (a32){ 
  switch(i&7){
   case 0: return (uint32*)(seg+((uint16)(cpu.eax+*(uint32*)((ip+=4)-4)))); break;
   case 1: return (uint32*)(seg+((uint16)(cpu.ecx+*(uint32*)((ip+=4)-4)))); break;
   case 2: return (uint32*)(seg+((uint16)(cpu.edx+*(uint32*)((ip+=4)-4)))); break;
   case 3: return (uint32*)(seg+((uint16)(cpu.ebx+*(uint32*)((ip+=4)-4)))); break;
   case 4: i=sib(); return (uint32*)(seg+((uint16)(i+*(uint32*)((ip+=4)-4)))); break;
   case 5: return (uint32*)(seg_bp+((uint16)(cpu.ebp+*(uint32*)((ip+=4)-4)))); break;
   case 6: return (uint32*)(seg+((uint16)(cpu.esi+*(uint32*)((ip+=4)-4)))); break;
   case 7: return (uint32*)(seg+((uint16)(cpu.edi+*(uint32*)((ip+=4)-4)))); break;
  }
 }else{
  switch(i&7){
   case 0: return (uint32*)(seg+((uint16)(cpu.bx+cpu.si+*(uint16*)((ip+=2)-2)))); break;
   case 1: return (uint32*)(seg+((uint16)(cpu.bx+cpu.di+*(uint16*)((ip+=2)-2)))); break;
   case 2: return (uint32*)(seg_bp+((uint16)(cpu.bp+cpu.si+*(uint16*)((ip+=2)-2)))); break;
   case 3: return (uint32*)(seg_bp+((uint16)(cpu.bp+cpu.di+*(uint16*)((ip+=2)-2)))); break;
   case 4: return (uint32*)(seg+((uint16)(cpu.si+*(uint16*)((ip+=2)-2)))); break;
   case 5: return (uint32*)(seg+((uint16)(cpu.di+*(uint16*)((ip+=2)-2)))); break;
   case 6: return (uint32*)(seg_bp+((uint16)(cpu.bp+*(uint16*)((ip+=2)-2)))); break;
   case 7: return (uint32*)(seg+((uint16)(cpu.bx+*(uint16*)((ip+=2)-2)))); break;
  }
 }
break;
}
}

uint8* seg_es_ptr;
uint8* seg_cs_ptr;
uint8* seg_ss_ptr;
uint8* seg_ds_ptr;
uint8* seg_fs_ptr;
uint8* seg_gs_ptr;

#define seg_es 0
#define seg_cs 1
#define seg_ss 2
#define seg_ds 3
#define seg_fs 4
#define seg_gs 5


#define op_r i&7
void cpu_call(){

static int32 i,i2,i3,x,x2,x3,y,y2,y3;
static uint8 b,b2,b3;
static uint8 *uint8p;
static uint16 *uint16p;
static uint32 *uint32p;
static uint8* dseg;
static int32 r;
ip=(uint8*)&cmem[cpu.cs*16+cpu.ip];

seg_es_ptr=(uint8*)cmem+cpu.es*16;
seg_cs_ptr=(uint8*)cmem+cpu.cs*16;
seg_ss_ptr=(uint8*)cmem+cpu.ss*16;
seg_ds_ptr=(uint8*)cmem+cpu.ds*16;
seg_fs_ptr=(uint8*)cmem+cpu.fs*16;
seg_gs_ptr=(uint8*)cmem+cpu.gs*16;

next_opcode:
b32=0; a32=0; seg=seg_ds_ptr; seg_bp=seg_ss_ptr;

i=*ip++;

//read any prefixes
if (i==0x66){b32=1; i=*ip++;}
if (i==0x26){seg_bp=seg=seg_es_ptr; i=*ip++;}
if (i==0x2E){seg_bp=seg=seg_cs_ptr; i=*ip++;}
if (i==0x36){seg=seg_ss_ptr; i=*ip++;}
if (i==0x3E){seg_bp=seg_ds_ptr; i=*ip++;}
if (i==0x64){seg_bp=seg=seg_fs_ptr; i=*ip++;}
if (i==0x65){seg_bp=seg=seg_gs_ptr; i=*ip++;}
if (i==0x67){a32=1; i=*ip++;}

if (i==0x0F) goto opcode_0F;

r=*ip>>3&7;

//mov
if (i!=0x8D){
if (i>=0x88&&i<=0x8E){
switch(i){
case 0x88:// /r r/m8,r8
*rm8()=*reg8[r];
break;
case 0x89:// /r r/m16(32),r16(32)
if (b32) *rm32()=*reg32[r]; else *rm16()=*reg16[r];
break;
case 0x8A:// /r r8,r/m8
*reg8[r]=*rm8();
break;
case 0x8B:// /r r16(32),r/m16(32)
if (b32) *reg32[r]=*rm32(); else *reg16[r]=*rm16();
break;
case 0x8C:// /r r/m16,Sreg
*rm16()=*segreg[r];
break;
case 0x8E:// /r Sreg,r/m16
*segreg[r]=*rm16();
if (r==0) seg_es_ptr=(uint8*)cmem+*segreg[r]*16;
//CS (r==1) cannot be set
if (r==2) seg_ss_ptr=(uint8*)cmem+*segreg[r]*16;
if (r==3) seg_ds_ptr=(uint8*)cmem+*segreg[r]*16;
if (r==4) seg_fs_ptr=(uint8*)cmem+*segreg[r]*16;
if (r==5) seg_gs_ptr=(uint8*)cmem+*segreg[r]*16;
break;
}
goto done;
}
}
if (i>=0xA0&&i<=0xA3){
switch(i){
case 0xA0:// al,moffs8
cpu.al=*(seg+*(uint16*)ip); ip+=2;
break;
case 0xA1:// (e)ax,moffs16(32)
if (b32){cpu.eax=*(uint32*)(seg+*(uint16*)ip); ip+=2;}else{cpu.ax=*(uint16*)(seg+*(uint16*)ip); ip+=2;}
break;
case 0xA2:// moffs8,al
*(seg+*(uint16*)ip)=cpu.al; ip+=2;
break;
case 0xA3:// moffs16(32),(e)ax
if (b32){*(uint32*)(seg+*(uint16*)ip)=cpu.eax; ip+=2;}else{*(uint16*)(seg+*(uint16*)ip)=cpu.ax; ip+=2;}
break;
}
goto done;
}
if (i>=0xB0&&i<=0xB7){// +rb reg8,imm8
*reg8[op_r]=*ip++;
goto done;
}
if (i>=0xB8&&i<=0xBF){// +rw(rd) reg16(32),imm16(32)
if (b32){*reg32[op_r]=*(uint32*)ip; ip+=4;}else{*reg16[op_r]=*(uint16*)ip; ip+=2;}
goto done;
}
if (i==0xC6){// r/m8,imm8
uint8p=rm8(); *uint8p=*ip++;
goto done;
}
if (i==0xC7){// r/m16(32),imm16(32)
if (b32){uint32p=rm32(); *uint32p=*(uint32*)ip; ip+=4;}else{uint16p=rm16(); *uint16p=*(uint16*)ip; ip+=2;}
goto done;
}

//ret (todo)
if (i==0xCB){//(far)
//assume return control (revise later)
return;
}
if (i==0xCA){//imm16 (far)
//assume return control (revise later)
return;
}

//int (todo)
if (i==0xCD){
call_int(*ip++);//assume interrupt table is 0xFFFF
goto done;
}

//push
if (i==0xFF){
if (b32){*((uint32*)(seg_ss_ptr+(cpu.sp-=4)))=*rm32();}else{*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*rm16();}
goto done;
}
if (i>=0x50&&i<=0x57){//+ /r r16(32)
if (b32){*((uint32*)(seg_ss_ptr+(cpu.sp-=4)))=*reg32[op_r];}else{*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*reg16[op_r];}
goto done;
}
if (i==0x6A){//imm8 (sign extended to 16 bits)
*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=((int8)*ip++);
goto done;
}
if (i==0x68){//imm16(32)
if (b32){*((uint32*)(seg_ss_ptr+(cpu.sp-=4)))=*(uint32*)ip; ip+=4;}else{*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*(uint16*)ip; ip+=2;}
goto done;
}
if (i==0x0E){//CS
*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*segreg[seg_cs];
goto done;
}
if (i==0x16){//SS
*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*segreg[seg_ss];
goto done;
}
if (i==0x1E){//DS
*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*segreg[seg_ds];
goto done;
}
if (i==0x06){//ES
*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*segreg[seg_es];
goto done;
}

//pop
if (i==0x8F){
if (b32){*rm32()=*((uint32*)(seg_ss_ptr-4+(cpu.sp+=4)));}else{*rm16()=*((uint16*)(seg_ss_ptr-2+(cpu.sp+=2)));}
goto done;
}
if (i>=0x58&&i<=0x5F){//+rw(d) r16(32)
if (b32){*reg32[op_r]=*((uint32*)(seg_ss_ptr-4+(cpu.sp+=4)));}else{*reg16[op_r]=*((uint16*)(seg_ss_ptr-2+(cpu.sp+=2)));}
goto done;
}
if (i==0x1F){//DS
*segreg[seg_ds]=*((uint16*)(seg_ss_ptr-2+(cpu.sp+=2)));
goto done;
}
if (i==0x07){//ES

*segreg[seg_es]=*((uint16*)(seg_ss_ptr-2+(cpu.sp+=2)));
goto done;
}
if (i==0x17){//SS
*segreg[seg_ss]=*((uint16*)(seg_ss_ptr-2+(cpu.sp+=2)));
goto done;
}

goto skip_0F_opcodes;
opcode_0F:
i=*ip++;
r=*ip>>3&7; //required???

//push
if (i==0xA0){
*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*segreg[seg_fs];
goto done;
}
if (i==0xA8){
*((uint16*)(seg_ss_ptr+(cpu.sp-=2)))=*segreg[seg_gs];
goto done;
}

//pop
if (i==0xA1){//FS
*segreg[seg_fs]=*((uint16*)(seg_ss_ptr-2+(cpu.sp+=2)));
goto done;
}
if (i==0xA9){//GS
*segreg[seg_gs]=*((uint16*)(seg_ss_ptr-2+(cpu.sp+=2)));
goto done;
}

skip_0F_opcodes:

i2=((i>>4)&15); if (i2<=9) i2+=48; else i2=i2-10+65;
unknown_opcode_mess->chr[16]=i2;
i2=i&15; if (i2<=9) i2+=48; else i2=i2-10+65;
unknown_opcode_mess->chr[17]=i2;
MessageBox(NULL,(char*)unknown_opcode_mess->chr,"X86 Error",MB_OK|MB_SYSTEMMODAL);
exit(86);
done:
if (*ip) goto next_opcode;

exit(cmem[0]);

}



int32 screen_last_valid=0;
uint8 *screen_last=(uint8*)malloc(1);
uint32 screen_last_size=1;
uint32 pal_last[256];

uint64 asciicode_value=0;
int32 asciicode_reading=0;



int32 lock_display=0;
int32 lock_display_required=0;

SDL_Thread *thread;
SDL_Thread *thread2;

//cost delay, made obselete by managing thread priorities (consider removal)
#define cost_limit 10000
#define cost_delay 0
uint32 cost=0;

#include "msbin.c"

//#include "time64.c"
//#include "time64.h"

//int QBMAIN (void *unused);

int64 build_int64(uint32 val2,uint32 val1){
static int64 val;
val=val2;
val<<=32;
val|=val1;
return val;
}

uint64 build_uint64(uint32 val2,uint32 val1){
static uint64 val;
val=val2;
val<<=32;
val|=val1;
return val;
}

//nb. abreviations are used in variable names to save typing, here are some of the expansions
//cmem=conventional memory
//qbs=qbick basic string (refers to the emulation of quick basic strings)
//sp=stack pointer
//dblock=a 64K memory block in conventional memory holding single variables and strings
extern uint8 *cmem_static_pointer;
uint8 *cmem_static_base=&cmem[0]+1280+65536;
extern uint8 *cmem_dynamic_base;
//[1280][DBLOCK][STATIC-><-DYNAMIC][A000-]

uint32 qbs_cmem_descriptor_space=256; //enough for 64 strings before expansion

extern uint32 qbs_cmem_sp; //=256;
extern uint32 cmem_sp; //=65536;
extern ptrszint dblock; //32bit offset of dblock
extern uint64 *nothingvalue;

uint32 qb64_firsttimervalue;
uint32 sdl_firsttimervalue;


extern uint32 qbevent;

uint8 wait_needed=1;

SDL_Surface * screen;
int32 full_screen=0;//0,1(stretched/closest),2(1:1)
int32 full_screen_toggle=0;//increments each time ALT+ENTER is pressed
int32 full_screen_set=-1;//0(windowed),1(stretched/closest),2(1:1)


int32 vertical_retrace_in_progress=0;
int32 vertical_retrace_happened=0;





static const char *arrow[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
"X                               ",
"XX                              ",
"X.X                             ",
"X..X                            ",
"X...X                           ",
"X....X                          ",
"X.....X                         ",
"X......X                        ",
"X.......X                       ",
"X........X                      ",
"X.........X                     ",
"X......XXXXX                    ",
"X...X..X                        ",
"X..XX..X                        ",
"X.X  X..X                       ",
"XX   X..X                       ",
"X     X..X                      ",
"      X..X                      ",
"       X..X                     ",
"       X..X                     ",
"        XX                      ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
  "0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[])
{
  int32 i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int32 hot_x, hot_y;

  i = -1;
  for ( row=0; row<32; ++row ) {
    for ( col=0; col<32; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;//?
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}



uint8 lock_subsystem=0;

extern uint8 close_program; //=0;
uint8 program_wait=0;

extern uint8 suspend_program;
extern uint8 stop_program;


int32 global_counter=0;
extern double last_line;



void end(void);



extern uint32 new_error;
extern uint32 error_err; //=0;
extern double error_erl; //=0;
extern uint32 error_occurred;
extern uint32 error_goto_line;
extern uint32 error_handling;
extern uint32 error_retry;

const char fixerr_strline[]="Line:";
const char fixerr_strcont[]="\nContinue?";
const char fixerr_strunhan[]="Unhandled Error #";
void fix_error(){
static char errtitle[256];//builds message
static char errmess[256];//builds message
static char *cp;
if ((!error_goto_line)||error_handling){
int32 v,i,i2;//stores response
v=0;

cp=NULL;
if (new_error==1) cp="NEXT without FOR";
if (new_error==2) cp="Syntax error";
if (new_error==3) cp="RETURN without GOSUB";
if (new_error==4) cp="Out of DATA";
if (new_error==5) cp="Illegal function call";
if (new_error==6) cp="Overflow";
if (new_error==7) cp="Out of memory";
if (new_error==8) cp="Label not defined";
if (new_error==9) cp="Subscript out of range";
if (new_error==10) cp="Duplicate definition";
if (new_error==11) cp="Division by zero";
if (new_error==12) cp="Illegal in direct mode";
if (new_error==13) cp="Type mismatch";
if (new_error==14) cp="Out of string space";
//error 15 undefined
if (new_error==16) cp="String formula too complex";
if (new_error==17) cp="Cannot continue";
if (new_error==18) cp="Function not defined";
if (new_error==19) cp="No RESUME";
if (new_error==20) cp="RESUME without error";
//error 21-23 undefined
if (new_error==24) cp="Device timeout";
if (new_error==25) cp="Device fault";
if (new_error==26) cp="FOR without NEXT";
if (new_error==27) cp="Out of paper";
//error 28 undefined
if (new_error==29) cp="WHILE without WEND";
if (new_error==30) cp="WEND without WHILE";
//error 31-32 undefined
if (new_error==33) cp="Duplicate label";
//error 34 undefined
if (new_error==35) cp="Subprogram not defined";
if (new_error==37) cp="Argument-count mismatch";
if (new_error==38) cp="Array not defined";
if (new_error==40) cp="Variable required";
if (new_error==50) cp="FIELD overflow";
if (new_error==51) cp="Internal error";
if (new_error==52) cp="Bad file name or number";
if (new_error==53) cp="File not found";
if (new_error==54) cp="Bad file mode";
if (new_error==55) cp="File already open";
if (new_error==56) cp="FIELD statement active";
if (new_error==57) cp="Device I/O error";
if (new_error==58) cp="File already exists";
if (new_error==59) cp="Bad record length";
if (new_error==61) cp="Disk full";
if (new_error==62) cp="Input past end of file";
if (new_error==63) cp="Bad record number";
if (new_error==64) cp="Bad file name";
if (new_error==67) cp="Too many files";
if (new_error==68) cp="Device unavailable";
if (new_error==69) cp="Communication-buffer overflow";
if (new_error==70) cp="Permission denied";
if (new_error==71) cp="Disk not ready";
if (new_error==72) cp="Disk-media error";
if (new_error==73) cp="Feature unavailable";
if (new_error==74) cp="Rename across disks";
if (new_error==75) cp="Path/File access error";
if (new_error==76) cp="Path not found";
if (new_error==258) cp="Invalid handle";
if (!cp) cp="Unprintable error";

i=0;
memcpy(&errmess[i],&fixerr_strline[0],strlen(fixerr_strline)); i=i+strlen(fixerr_strline);
i2=sprintf(&errmess[i],"%u\n",ercl); i=i+i2;
memcpy(&errmess[i],cp,strlen(cp)); i=i+strlen(cp);
memcpy(&errmess[i],&fixerr_strcont[0],strlen(fixerr_strcont)); i=i+strlen(fixerr_strcont);
errmess[i]=0;

i=0;
memcpy(&errtitle[i],&fixerr_strunhan[0],strlen(fixerr_strunhan)); i=i+strlen(fixerr_strunhan);
i2=sprintf(&errtitle[i],"%u",new_error); i=i+i2;
errtitle[i]=0;
v=MessageBox(NULL,errmess,errtitle,MB_YESNO|MB_SYSTEMMODAL);
#ifdef QB64_WINDOWS
 if (v==IDYES){
 if (full_screen){
 static SDL_SysWMinfo info;
 static HWND hwnd;
 SDL_VERSION(&info.version);
 SDL_GetWMInfo(&info);
 hwnd = info.window;
 ShowWindow(hwnd,SW_SHOWMAXIMIZED);
 }
 }
#endif
if ((v==IDNO)||(v==IDOK)){close_program=1; end();}
new_error=0;
return;
}
error_err=new_error;
new_error=0;
error_erl=last_line;
error_occurred=1;
QBMAIN (NULL);
return;
}



void error(int32 error_number){

//critical errors
if (error_number==256){MessageBox(NULL,"Out of stack space","Critical Error",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==257){MessageBox(NULL,"Out of memory","Critical Error #1",MB_OK|MB_SYSTEMMODAL); exit(0);}//generic "Out of memory" error
if (error_number==259){MessageBox(NULL,"Cannot find dynamic library file","Critical Error",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==260){MessageBox(NULL,"Sub/Function does not exist in dynamic library","Critical Error",MB_OK|MB_SYSTEMMODAL); exit(0);}
//trackable "Out of memory" errors
if (error_number==502){MessageBox(NULL,"Out of memory","Critical Error #2",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==503){MessageBox(NULL,"Out of memory","Critical Error #3",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==504){MessageBox(NULL,"Out of memory","Critical Error #4",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==505){MessageBox(NULL,"Out of memory","Critical Error #5",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==506){MessageBox(NULL,"Out of memory","Critical Error #6",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==507){MessageBox(NULL,"Out of memory","Critical Error #7",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==508){MessageBox(NULL,"Out of memory","Critical Error #8",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==509){MessageBox(NULL,"Out of memory","Critical Error #9",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==510){MessageBox(NULL,"Out of memory","Critical Error #10",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==511){MessageBox(NULL,"Out of memory","Critical Error #11",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==512){MessageBox(NULL,"Out of memory","Critical Error #12",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==513){MessageBox(NULL,"Out of memory","Critical Error #13",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==514){MessageBox(NULL,"Out of memory","Critical Error #14",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==515){MessageBox(NULL,"Out of memory","Critical Error #15",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==516){MessageBox(NULL,"Out of memory","Critical Error #16",MB_OK|MB_SYSTEMMODAL); exit(0);}
if (error_number==517){MessageBox(NULL,"Out of memory","Critical Error #17",MB_OK|MB_SYSTEMMODAL); exit(0);}

if (!new_error){
if ((new_error==256)||(new_error==257)) fix_error();//critical error!
if (error_number<=0) error_number=5;//Illegal function call
new_error=error_number;
qbevent=1;
}
}

double get_error_erl(){
return error_erl;
}

uint32 get_error_err(){
return error_err;
}

void end(){
exit_ok|=1;
while(!stop_program) Sleep(16);
SDL_KillThread(thread);
while(1) Sleep(16);
}



//MEM_STATIC memory manager
/*
mem_static uses a pointer called mem_static_pointer to allocate linear memory.
It can also change mem_static_pointer back to a previous location, effectively erasing
any memory after that point.
Because memory cannot be guaranteed to be allocated in exactly the same location
after realloc which QB64 requires to keep functionality of previous pointers when
the current block of memory is full QB64 creates an entirely new block, much larger
than the previous block (at least 2x), and "writes-off" the previous block as un-
reclaimable memory. This tradeoff is worth the speed it recovers.
This allocation strategy can be shown as follows: (X=1MB)
X
XX
XXXX
XXXXXXXX
XXXXXXXXXXXXXXXX
etc.
*/
uint32 mem_static_size;
extern uint8 *mem_static;
extern uint8 *mem_static_pointer;
extern uint8 *mem_static_limit;

uint8 *mem_static_malloc(uint32 size){
if ((mem_static_pointer+=size)<mem_static_limit) return mem_static_pointer-size;
mem_static_size=(mem_static_size<<1)+size;
mem_static=(uint8*)malloc(mem_static_size);
if (!mem_static) error(504);
mem_static_pointer=mem_static+size;
mem_static_limit=mem_static+mem_static_size;
return mem_static_pointer-size;
}
void mem_static_restore(uint8* restore_point){
if ((restore_point>=mem_static)&&(restore_point<=mem_static_limit)){
mem_static_pointer=restore_point;
}else{
//if restore_point is not in the current block, use t=start of current block as a new base
mem_static_pointer=mem_static;
}
}

//CMEM_FAR_DYNAMIC memory manager
/*
(uses a custom "links" based memory manager)
*/
//           &HA000    DBLOCK SIZE        DBLOCK OFFSET
//           655360 - (65536            + 1280         )=588544 links possible
//links limited to 588544/4=147136 (do not have enough links if avg. block size less than 4 bytes)
//stores blocks, not free memory, because blocks are easier to identify
//always scanned from beginning to end, so prev. pointer is unnecessary
struct cmem_dynamic_link_type{
uint8 *offset;
uint8 *top;
uint32 size;
uint32 i;
cmem_dynamic_link_type *next;
};
cmem_dynamic_link_type cmem_dynamic_link[147136+1]; //+1 is added because array is used from index 1

//i=cmem_dynamic_next_link++; if (i>=147136) error(257);//not enough blocks
//newlink=(cmem_dynamic_link_type*)&cmem_dynamic_link[i];

cmem_dynamic_link_type *cmem_dynamic_link_first=NULL;
int32 cmem_dynamic_next_link=0;
int32 cmem_dynamic_free_link=0;
uint32 cmem_dynamic_free_list[147136];
uint8 *cmem_dynamic_malloc(uint32 size){
static int32 i;
static uint8 *top;
static cmem_dynamic_link_type *link;
static cmem_dynamic_link_type *newlink;
static cmem_dynamic_link_type *prev_link;
if (size>65536) error(505);//>64K
//to avoid mismatches between offsets, all 0-byte blocks are given the special offset A000h (the top of the heap)
if (!size) return(&cmem[0]+655360);//top of heap
//forces blocks to be multiples of 16 bytes so they align with segment boundaries
if (size&15) size=size-(size&15)+16;
//is a space large enough between existing blocks available?
//(if not, memory will be allocated at bottom of heap)
top=&cmem[0]+655360;//top is the base of the higher block
prev_link=NULL;
if (link=cmem_dynamic_link_first){
cmem_dynamic_findspace:
if ((top-link->top)>=size){ //gpf
//found free space
goto cmem_dynamic_make_new_link;
}
prev_link=link; top=link->offset;//set top to the base of current block for future comparisons
if (link=link->next) goto cmem_dynamic_findspace;
}
//no space between existing blocks is large enough, alloc below 'top'
if ((top-cmem_static_pointer)<size) error(506);//a large enough block cannot be created!
cmem_dynamic_base=top-size;
//get a new link index
cmem_dynamic_make_new_link:
if (cmem_dynamic_free_link){
i=cmem_dynamic_free_list[cmem_dynamic_free_link--];
}else{
i=cmem_dynamic_next_link++; if (i>=147136) error(507);//not enough blocks
}
newlink=(cmem_dynamic_link_type*)&cmem_dynamic_link[i];
//set link info
newlink->i=i;
newlink->offset=top-size;
newlink->size=size;
newlink->top=top;
//attach below prev_link
if (prev_link){
newlink->next=prev_link->next;//NULL if none
prev_link->next=newlink;
}else{
newlink->next=cmem_dynamic_link_first;//NULL if none
cmem_dynamic_link_first=newlink;
}
return newlink->offset;
}

void cmem_dynamic_free(uint8 *block){
static cmem_dynamic_link_type *link;
static cmem_dynamic_link_type *prev_link;
if (!cmem_dynamic_link_first) return;
if (!block) return;
if (block==(&cmem[0]+655360)) return;//to avoid mismatches between offsets, all 0-byte blocks are given the special offset A000h (the top of the heap)
prev_link=NULL;
link=cmem_dynamic_link_first;
check_next:
if (link->offset==block){
//unlink
if (prev_link){
prev_link->next=link->next;
}else{
cmem_dynamic_link_first=link->next;
}
//free link
cmem_dynamic_free_link++;
cmem_dynamic_free_list[cmem_dynamic_free_link]=link->i;
//memory freed successfully!
return;
}
prev_link=link;
if (link=link->next) goto check_next;
return;
}

uint8 *defseg=&cmem[1280];//set to base of DBLOCK

void sub_defseg(int32 segment,int32 passed){
if (new_error) return;
if (!passed){
defseg=&cmem[1280];
return;
}

if ((segment<-65536)||(segment>65535)){//same range as QB checks
error(6);
}else{
defseg=&cmem[0]+((uint16)segment)*16;
}
}

int32 func_peek(int32 offset){
if ((offset<-65536)||(offset>65535)){//same range as QB checks
error(6);
return 0;
}
return defseg[(uint16)offset];
}

void sub_poke(int32 offset,int32 value){
if (new_error) return;
if ((offset<-65536)||(offset>65535)){//same range as QB checks
error(6);
return;
}
defseg[(uint16)offset]=value;
}

int32 array_ok=1;//kept to compile legacy versions

//gosub-return handling
extern uint32 next_return_point; //=0;
extern uint32 *return_point; //=(uint32*)malloc(4*16384);
extern uint32 return_points; //=16384;
void more_return_points(){
if (return_points>2147483647) error(256);
return_points*=2;
return_point=(uint32*)realloc(return_point,return_points*4);
if (return_point==NULL) error(256);
}




void sub__sndplay(uint32);

uint8 *soundwave(double frequency,double length,double volume,double fadein,double fadeout);
int32 soundwave_bytes=0;

void qb64_generatesound(double f,double l,uint8 wait){
sndsetup();
static uint8* data;
static uint32 handle;
data=soundwave(f,l,1,0,0);
handle=func__sndraw(data,soundwave_bytes);
if (handle){
if (wait){
sndqueue_wait=sndqueue_next;
suspend_program|=2;
qbevent=1;
}
sndqueue[sndqueue_next]=handle;
sndqueue_next++; if (sndqueue_next>sndqueue_lastindex) sndqueue_next=0;
}else free(data);
}

uint8 *soundwave(double frequency,double length,double volume,double fadein,double fadeout){
sndsetup();
static uint8 *data;
static int32 i;
static int16 x,lastx;
static int16* sp;
static double samples_per_second=22050.0;

//calculate total number of samples required
static double samples;
static int32 samplesi;
samples=length*samples_per_second;
samplesi=samples; if (!samplesi) samplesi=1;

soundwave_bytes=samplesi*4;
data=(uint8*)malloc(soundwave_bytes);
sp=(int16*)data;

static int32 direction;
direction=1;

static double value;
value=0;

static double volume_multiplier;
volume_multiplier=volume*32767.0;

static int32 waveend;
waveend=0;

static double gradient;
//frequency*4.0*length is the total distance value will travel (+1,-2,+1[repeated])
//samples is the number of steps to do this in
if (samples) gradient=(frequency*4.0*length)/samples; else gradient=0;//avoid division by 0

lastx=1;//set to 1 to avoid passing initial comparison
for (i=0;i<samplesi;i++){
x=value*volume_multiplier;
*sp++=x;
*sp++=x;
if (x>0){
if (lastx<=0){
waveend=i;
}
}
lastx=x;
if (direction){
if ((value+=gradient)>=1.0){direction=0; value=2.0-value;}
}else{
if ((value-=gradient)<=-1.0){direction=1; value=-2.0-value;}
}
}//i

if (waveend) soundwave_bytes=waveend*4;

return (uint8*)data;
}

int32 wavesize(double length){
static int32 samples;
samples=length*22050.0; if (samples==0) samples=1;
return samples*4;
}



uint8 keyon[65536];

qbs* singlespace;


qbs *qbs_malloc=(qbs*)calloc(sizeof(qbs)*65536,1);//~1MEG
uint32 qbs_malloc_next=0;//the next idex in qbs_malloc to use
ptrszint *qbs_malloc_freed=(ptrszint*)malloc(ptrsz*65536);
uint32 qbs_malloc_freed_size=65536;
uint32 qbs_malloc_freed_num=0;//number of freed qbs descriptors

/*MLP
uint32 *dbglist=(uint32*)malloc(4*10000000);
uint32 dbglisti=0;
uint32 dbgline=0;
*/

qbs *qbs_new_descriptor(){
//MLP //qbshlp1++;
if (qbs_malloc_freed_num){
/*MLP
static qbs *s;
s=(qbs*)memset((void *)qbs_malloc_freed[--qbs_malloc_freed_num],0,sizeof(qbs));
s->dbgl=dbgline;
return s;
*/
return (qbs*)memset((void *)qbs_malloc_freed[--qbs_malloc_freed_num],0,sizeof(qbs));
}
if (qbs_malloc_next==65536){
qbs_malloc=(qbs*)calloc(sizeof(qbs)*65536,1);//~1MEG
qbs_malloc_next=0;
}
/*MLP
dbglist[dbglisti]=(uint32)&qbs_malloc[qbs_malloc_next];
static qbs* s;
s=(qbs*)&qbs_malloc[qbs_malloc_next++];
s->dbgl=dbgline;
dbglisti++;
return s;
*/
return &qbs_malloc[qbs_malloc_next++];
}

void qbs_free_descriptor(qbs *str){
//MLP //qbshlp1--;
if (qbs_malloc_freed_num==qbs_malloc_freed_size){
 qbs_malloc_freed_size*=2;
 qbs_malloc_freed=(ptrszint*)realloc(qbs_malloc_freed,qbs_malloc_freed_size*ptrsz);
 if (!qbs_malloc_freed) error(508);
}
qbs_malloc_freed[qbs_malloc_freed_num]=(ptrszint)str;
qbs_malloc_freed_num++;
return;
}

//Used to track strings in 16bit memory
ptrszint *qbs_cmem_list=(ptrszint*)malloc(65536*ptrsz);
uint32  qbs_cmem_list_lasti=65535;
uint32  qbs_cmem_list_nexti=0;
//Used to track strings in 32bit memory
ptrszint *qbs_list=(ptrszint*)malloc(65536*ptrsz);
uint32  qbs_list_lasti=65535;
uint32  qbs_list_nexti=0;
//Used to track temporary strings for later removal when they fall out of scope
//*Some string functions delete a temporary string automatically after they have been
// passed one to save memory. In this case qbstring_templist[?]=0xFFFFFFFF
ptrszint *qbs_tmp_list=(ptrszint*)calloc(65536*ptrsz,1);//first index MUST be 0
uint32  qbs_tmp_list_lasti=65535;
extern uint32 qbs_tmp_list_nexti;
//entended string memory

uint8 *qbs_data=(uint8*)malloc(1048576);
uint32 qbs_data_size=1048576;
uint32 qbs_sp=0;


void field_free(qbs* str);

void qbs_free(qbs *str){

if (str->field) field_free(str);

if (str->tmplisti){
 qbs_tmp_list[str->tmplisti]=-1;
 while (qbs_tmp_list[qbs_tmp_list_nexti-1]==-1){
 qbs_tmp_list_nexti--;
 }
}
if (str->fixed||str->readonly){
 qbs_free_descriptor(str);
 return;
}
if (str->in_cmem){
 qbs_cmem_list[str->listi]=-1;
 if ((qbs_cmem_list_nexti-1)==str->listi) qbs_cmem_list_nexti--;
}else{
 qbs_list[str->listi]=-1;
 retry:
 if (qbs_list[qbs_list_nexti-1]==-1){
 qbs_list_nexti--;
 if (qbs_list_nexti) goto retry;
 }
 if (qbs_list_nexti){
 qbs_sp=((qbs*)qbs_list[qbs_list_nexti-1])->chr-qbs_data+((qbs*)qbs_list[qbs_list_nexti-1])->len+32;
 if (qbs_sp>qbs_data_size) qbs_sp=qbs_data_size;//adding 32 could overflow buffer!
 }else{
 qbs_sp=0;
 }
}
qbs_free_descriptor(str);
return;
}

void qbs_cmem_concat_list(){
uint32 i;
uint32 d;
qbs *tqbs;
d=0;
for (i=0;i<qbs_cmem_list_nexti;i++){
 if (qbs_cmem_list[i]!=-1){ 
  if (i!=d){  
   tqbs=(qbs*)qbs_cmem_list[i];
   tqbs->listi=d;
   qbs_cmem_list[d]=(ptrszint)tqbs;
  }
 d++;
 }
}
qbs_cmem_list_nexti=d;
//if string listings are taking up more than half of the list array double the list array's size
if (qbs_cmem_list_nexti>=(qbs_cmem_list_lasti/2)){
qbs_cmem_list_lasti*=2;
qbs_cmem_list=(ptrszint*)realloc(qbs_cmem_list,(qbs_cmem_list_lasti+1)*ptrsz);
if (!qbs_cmem_list) error(509);
}
return;
}

void qbs_concat_list(){
uint32 i;
uint32 d;
qbs *tqbs;
d=0;
for (i=0;i<qbs_list_nexti;i++){
 if (qbs_list[i]!=-1){
  if (i!=d){
   tqbs=(qbs*)qbs_list[i];
   tqbs->listi=d;
   qbs_list[d]=(ptrszint)tqbs;
  }
 d++;
 }
}
qbs_list_nexti=d;
//if string listings are taking up more than half of the list array double the list array's size
if (qbs_list_nexti>=(qbs_list_lasti/2)){
qbs_list_lasti*=2;
qbs_list=(ptrszint*)realloc(qbs_list,(qbs_list_lasti+1)*ptrsz);
if (!qbs_list) error(510);
}
return;
}

void qbs_tmp_concat_list(){
if (qbs_tmp_list_nexti>=(qbs_tmp_list_lasti/2)){
qbs_tmp_list_lasti*=2;
qbs_tmp_list=(ptrszint*)realloc(qbs_tmp_list,(qbs_tmp_list_lasti+1)*ptrsz);
if (!qbs_tmp_list) error(511);
}
return;
}




void qbs_concat(uint32 bytesrequired){
//this does not change indexing, only ->chr pointers and the location of their data
static int32 i;
static uint8 *dest;
static qbs *tqbs;
dest=(uint8*)qbs_data;
if (qbs_list_nexti){
qbs_sp=0;
 for (i=0;i<qbs_list_nexti;i++){
  if (qbs_list[i]!=-1){
   tqbs=(qbs*)qbs_list[i];
   if ((tqbs->chr-dest)>32){
   if (tqbs->len) {memmove(dest,tqbs->chr,tqbs->len);}
   tqbs->chr=dest;       
   }
   dest=tqbs->chr+tqbs->len;
   qbs_sp=dest-qbs_data;
  }
 }
}

if (((qbs_sp*2)+(bytesrequired+32))>=qbs_data_size){
static uint8 *oldbase;
oldbase=qbs_data;
qbs_data_size=qbs_data_size*2+bytesrequired;
qbs_data=(uint8*)realloc(qbs_data,qbs_data_size);
if (qbs_data==NULL) error(512);//realloc failed!
for (i=0;i<qbs_list_nexti;i++){
if (qbs_list[i]!=-1){
tqbs=(qbs*)qbs_list[i];
tqbs->chr=tqbs->chr-oldbase+qbs_data;
}
}
}
return;
}

//as the cmem stack has a limit if bytesrequired cannot be met this exits and returns an error
//the cmem stack cannot after all be extended!
//so bytesrequired is only passed to possibly generate an error, or not generate one
void qbs_concat_cmem(uint32 bytesrequired){
//this does not change indexing, only ->chr pointers and the location of their data
int32 i;
uint8 *dest;
qbs *tqbs;
dest=(uint8*)dblock;
qbs_cmem_sp=qbs_cmem_descriptor_space;
if (qbs_cmem_list_nexti){
 for (i=0;i<qbs_cmem_list_nexti;i++){
  if (qbs_cmem_list[i]!=-1){
   tqbs=(qbs*)qbs_cmem_list[i];
   if (tqbs->chr!=dest){
   if (tqbs->len) {memmove(dest,tqbs->chr,tqbs->len);}
   tqbs->chr=dest;
      //update cmem_descriptor [length][offset]
      if (tqbs->cmem_descriptor){tqbs->cmem_descriptor[0]=tqbs->len; tqbs->cmem_descriptor[1]=(uint16)(ptrszint)(tqbs->chr-dblock);}
   }
   dest+=tqbs->len;
   qbs_cmem_sp+=tqbs->len;
  }
 }
}
if ((qbs_cmem_sp+bytesrequired)>cmem_sp) error(513);
return;
}

qbs *qbs_new_cmem(int32 size,uint8 tmp){
if ((qbs_cmem_sp+size)>cmem_sp) qbs_concat_cmem(size);
qbs *newstr;
newstr=qbs_new_descriptor();
newstr->len=size;
if ((qbs_cmem_sp+size)>cmem_sp) qbs_concat_cmem(size);
newstr->chr=(uint8*)dblock+qbs_cmem_sp;
qbs_cmem_sp+=size;
newstr->in_cmem=1;
if (qbs_cmem_list_nexti>qbs_cmem_list_lasti) qbs_cmem_concat_list();
newstr->listi=qbs_cmem_list_nexti; qbs_cmem_list[newstr->listi]=(ptrszint)newstr; qbs_cmem_list_nexti++;
if (tmp){
if (qbs_tmp_list_nexti>qbs_tmp_list_lasti) qbs_tmp_concat_list();
newstr->tmplisti=qbs_tmp_list_nexti; qbs_tmp_list[newstr->tmplisti]=(ptrszint)newstr; qbs_tmp_list_nexti++;
newstr->tmp=1;
}else{
//alloc string descriptor in DBLOCK (4 bytes)
cmem_sp-=4; newstr->cmem_descriptor=(uint16*)(dblock+cmem_sp); if (cmem_sp<qbs_cmem_sp) error(514);
newstr->cmem_descriptor_offset=cmem_sp;
  //update cmem_descriptor [length][offset]
  newstr->cmem_descriptor[0]=newstr->len; newstr->cmem_descriptor[1]=(uint16)(ptrszint)(newstr->chr-dblock);
}
return newstr;
}

qbs *qbs_new(int32,uint8);

qbs *qbs_new_txt(const char *txt){
qbs *newstr;
newstr=qbs_new_descriptor();
if (!txt){//NULL pointer is converted to a 0-length string
newstr->len=0;
}else{
newstr->len=strlen(txt);
}
newstr->chr=(uint8*)txt;
if (qbs_tmp_list_nexti>qbs_tmp_list_lasti) qbs_tmp_concat_list();
newstr->tmplisti=qbs_tmp_list_nexti; qbs_tmp_list[newstr->tmplisti]=(ptrszint)newstr; qbs_tmp_list_nexti++;
newstr->tmp=1;
newstr->readonly=1;
return newstr;
}

qbs *qbs_new_txt_len(const char *txt,int32 len){
qbs *newstr;
newstr=qbs_new_descriptor();
newstr->len=len;
newstr->chr=(uint8*)txt;
if (qbs_tmp_list_nexti>qbs_tmp_list_lasti) qbs_tmp_concat_list();
newstr->tmplisti=qbs_tmp_list_nexti; qbs_tmp_list[newstr->tmplisti]=(ptrszint)newstr; qbs_tmp_list_nexti++;
newstr->tmp=1;
newstr->readonly=1;
return newstr;
}







//note: qbs_new_fixed detects if string is in DBLOCK
qbs *qbs_new_fixed(uint8 *offset,uint32 size,uint8 tmp){
qbs *newstr;
newstr=qbs_new_descriptor();
newstr->len=size;
newstr->chr=offset;
newstr->fixed=1;
if (tmp){
if (qbs_tmp_list_nexti>qbs_tmp_list_lasti) qbs_tmp_concat_list();
newstr->tmplisti=qbs_tmp_list_nexti; qbs_tmp_list[newstr->tmplisti]=(ptrszint)newstr; qbs_tmp_list_nexti++;
newstr->tmp=1;
}else{
//is it in DBLOCK?
if ((offset>(cmem+1280))&&(offset<(cmem+66816))){
//alloc string descriptor in DBLOCK (4 bytes)
cmem_sp-=4; newstr->cmem_descriptor=(uint16*)(dblock+cmem_sp); if (cmem_sp<qbs_cmem_sp) error(515);
newstr->cmem_descriptor_offset=cmem_sp;
  //update cmem_descriptor [length][offset]
  newstr->cmem_descriptor[0]=newstr->len; newstr->cmem_descriptor[1]=(uint16)(ptrszint)(newstr->chr-dblock);
}
}
return newstr;
}

qbs *qbs_new(int32 size,uint8 tmp){
static qbs *newstr;
if ((qbs_sp+size+32)>qbs_data_size) qbs_concat(size+32);
newstr=qbs_new_descriptor();
newstr->len=size;
newstr->chr=qbs_data+qbs_sp;
qbs_sp+=size+32;
if (qbs_list_nexti>qbs_list_lasti) qbs_concat_list();
newstr->listi=qbs_list_nexti; qbs_list[newstr->listi]=(ptrszint)newstr; qbs_list_nexti++;
if (tmp){
if (qbs_tmp_list_nexti>qbs_tmp_list_lasti) qbs_tmp_concat_list();
newstr->tmplisti=qbs_tmp_list_nexti; qbs_tmp_list[newstr->tmplisti]=(ptrszint)newstr; qbs_tmp_list_nexti++;
newstr->tmp=1;
}
return newstr;
}

void qbs_maketmp(qbs *str){
//WARNING: assumes str is a non-tmp string in non-cmem
if (qbs_tmp_list_nexti>qbs_tmp_list_lasti) qbs_tmp_concat_list();
str->tmplisti=qbs_tmp_list_nexti; qbs_tmp_list[str->tmplisti]=(ptrszint)str; qbs_tmp_list_nexti++;
str->tmp=1;
}

qbs *qbs_set(qbs *deststr,qbs *srcstr){
int32 i;
qbs *tqbs;
//fixed deststr
if (deststr->fixed){
 if (srcstr->len>=deststr->len){
  memcpy(deststr->chr,srcstr->chr,deststr->len);
 }else{
  memcpy(deststr->chr,srcstr->chr,srcstr->len);
  memset(deststr->chr+srcstr->len,32,deststr->len-srcstr->len);//pad with spaces
 }
 goto qbs_set_return;
}
//non-fixed deststr

//can srcstr be aquired by deststr?
if (srcstr->tmp){
if (srcstr->fixed==0){
if (srcstr->readonly==0){
if (srcstr->in_cmem==deststr->in_cmem){
if (deststr->in_cmem){
 //unlist deststr and acquire srcstr's list index
 qbs_cmem_list[deststr->listi]=-1;
 qbs_cmem_list[srcstr->listi]=(ptrszint)deststr;
 deststr->listi=srcstr->listi;
}else{
 //unlist deststr and acquire srcstr's list index
 qbs_list[deststr->listi]=-1;
 qbs_list[srcstr->listi]=(ptrszint)deststr;
 deststr->listi=srcstr->listi;
}

qbs_tmp_list[srcstr->tmplisti]=-1;
if (srcstr->tmplisti==(qbs_tmp_list_nexti-1)) qbs_tmp_list_nexti--;//correct last tmp index for performance

deststr->chr=srcstr->chr;
deststr->len=srcstr->len;
qbs_free_descriptor(srcstr);
  //update cmem_descriptor [length][offset]
  if (deststr->cmem_descriptor){deststr->cmem_descriptor[0]=deststr->len; deststr->cmem_descriptor[1]=(uint16)(ptrszint)(deststr->chr-dblock);}
return deststr;//nb. This return cannot be changed to a goto qbs_set_return!
}}}}

//srcstr is equal length or shorter
if (srcstr->len<=deststr->len){
 memcpy(deststr->chr,srcstr->chr,srcstr->len);
 deststr->len=srcstr->len;
 goto qbs_set_return;
}

//srcstr is longer
if (deststr->in_cmem){
 if (deststr->listi==(qbs_cmem_list_nexti-1)){//last index
  if (((ptrszint)deststr->chr+srcstr->len)<=(dblock+cmem_sp)){//space available
   memcpy(deststr->chr,srcstr->chr,srcstr->len);
   deststr->len=srcstr->len;  
   qbs_cmem_sp=((ptrszint)deststr->chr)+(ptrszint)deststr->len-dblock;
   goto qbs_set_return;
  }
  goto qbs_set_cmem_concat_required;
 }
 //deststr is not the last index so locate next valid index
 i=deststr->listi+1;
 qbs_set_nextindex:
 if (qbs_cmem_list[i]!=-1){
  tqbs=(qbs*)qbs_cmem_list[i];
  if (tqbs==srcstr){
   if (srcstr->tmp==1) goto skippedtmpsrcindex;
  }
  if ((deststr->chr+srcstr->len)>tqbs->chr) goto qbs_set_cmem_concat_required;
  memcpy(deststr->chr,srcstr->chr,srcstr->len);
  deststr->len=srcstr->len;
  goto qbs_set_return;
 }
 skippedtmpsrcindex:
 i++;
 if (i!=qbs_cmem_list_nexti) goto qbs_set_nextindex;
 //all next indexes invalid!
 qbs_cmem_list_nexti=deststr->listi+1;//adjust nexti
 if (((ptrszint)deststr->chr+srcstr->len)<=(dblock+cmem_sp)){//space available
   memmove(deststr->chr,srcstr->chr,srcstr->len);//overlap possible due to sometimes aquiring srcstr's space
   deststr->len=srcstr->len;
   qbs_cmem_sp=((ptrszint)deststr->chr)+(ptrszint)deststr->len-dblock;
   goto qbs_set_return;
 }
qbs_set_cmem_concat_required:
//srcstr could not fit in deststr
//"realloc" deststr
qbs_cmem_list[deststr->listi]=-1;//unlist
if ((qbs_cmem_sp+srcstr->len)>cmem_sp){//must concat!
qbs_concat_cmem(srcstr->len);
}
if (qbs_cmem_list_nexti>qbs_cmem_list_lasti) qbs_cmem_concat_list();
deststr->listi=qbs_cmem_list_nexti;
qbs_cmem_list[qbs_cmem_list_nexti]=(ptrszint)deststr; qbs_cmem_list_nexti++; //relist
deststr->chr=(uint8*)dblock+qbs_cmem_sp;
deststr->len=srcstr->len;
qbs_cmem_sp+=deststr->len;
memcpy(deststr->chr,srcstr->chr,srcstr->len);
goto qbs_set_return;
}


//not in cmem
 if (deststr->listi==(qbs_list_nexti-1)){//last index
  if (((ptrszint)deststr->chr+srcstr->len)<=((ptrszint)qbs_data+qbs_data_size)){//space available
   memcpy(deststr->chr,srcstr->chr,srcstr->len);
   deststr->len=srcstr->len;
   qbs_sp=((ptrszint)deststr->chr)+(ptrszint)deststr->len-(ptrszint)qbs_data;
   goto qbs_set_return;
  }
  goto qbs_set_concat_required;
 }
 //deststr is not the last index so locate next valid index
 i=deststr->listi+1;
 qbs_set_nextindex2:
 if (qbs_list[i]!=-1){
  tqbs=(qbs*)qbs_list[i];
  if (tqbs==srcstr){
   if (srcstr->tmp==1) goto skippedtmpsrcindex2;
  }
  if ((deststr->chr+srcstr->len)>tqbs->chr) goto qbs_set_concat_required;
  memcpy(deststr->chr,srcstr->chr,srcstr->len);
  deststr->len=srcstr->len;
  goto qbs_set_return;
 }
 skippedtmpsrcindex2:
 i++;
 if (i!=qbs_list_nexti) goto qbs_set_nextindex2;
 //all next indexes invalid!

 qbs_list_nexti=deststr->listi+1;//adjust nexti 
 if (((ptrszint)deststr->chr+srcstr->len)<=((ptrszint)qbs_data+qbs_data_size)){//space available
   memmove(deststr->chr,srcstr->chr,srcstr->len);//overlap possible due to sometimes aquiring srcstr's space
   deststr->len=srcstr->len;
   qbs_sp=((ptrszint)deststr->chr)+(ptrszint)deststr->len-(ptrszint)qbs_data;
   goto qbs_set_return;
 }

qbs_set_concat_required:
//srcstr could not fit in deststr
//"realloc" deststr
qbs_list[deststr->listi]=-1;//unlist
if ((qbs_sp+srcstr->len)>qbs_data_size){//must concat!
qbs_concat(srcstr->len);
}
if (qbs_list_nexti>qbs_list_lasti) qbs_concat_list();
deststr->listi=qbs_list_nexti;
qbs_list[qbs_list_nexti]=(ptrszint)deststr; qbs_list_nexti++; //relist

deststr->chr=qbs_data+qbs_sp;
deststr->len=srcstr->len;
qbs_sp+=deststr->len;
memcpy(deststr->chr,srcstr->chr,srcstr->len);

//(fall through to qbs_set_return)
qbs_set_return:
if (srcstr->tmp){//remove srcstr if it is a tmp string
qbs_free(srcstr);
}
  //update cmem_descriptor [length][offset]
  if (deststr->cmem_descriptor){deststr->cmem_descriptor[0]=deststr->len; deststr->cmem_descriptor[1]=(uint16)(ptrszint)(deststr->chr-dblock);}
return deststr;
}

qbs *qbs_add(qbs *str1,qbs *str2){
qbs *tqbs;
if (!str2->len) return str1;//pass on
if (!str1->len) return str2;//pass on
//may be possible to acquire str1 or str2's space but...
//1. check if dest has enough space (because its data is already in the correct place)
//2. check if source has enough space
//3. give up
//nb. they would also have to be a tmp, var. len str in ext memory!
//brute force method...
tqbs=qbs_new(str1->len+str2->len,1);
memcpy(tqbs->chr,str1->chr,str1->len);
memcpy(tqbs->chr+str1->len,str2->chr,str2->len);

//exit(qbs_sp);
if (str1->tmp) qbs_free(str1);
if (str2->tmp) qbs_free(str2);
return tqbs;
}

qbs *qbs_ucase(qbs *str){
uint32 i;
uint8 *c;
if (!str->len) return str;//pass on
qbs *tqbs=NULL;
if (str->tmp){ if (!str->fixed){ if (!str->readonly){ if (!str->in_cmem){ tqbs=str; }}}}
if (!tqbs){
//also pass on if already uppercase
c=str->chr;
for (i=0;i<str->len;i++){
 if ((*c>=97)&&(*c<=122)) goto qbs_ucase_cantpass;
 c++;
}
return str;
qbs_ucase_cantpass:
tqbs=qbs_new(str->len,1); memcpy(tqbs->chr,str->chr,str->len);
}
c=tqbs->chr;
for (i=0;i<tqbs->len;i++){
 if ((*c>=97)&&(*c<=122)) *c-=32;
 c++;
}
if (tqbs!=str) if (str->tmp) qbs_free(str);
return tqbs;
}

qbs *qbs_lcase(qbs *str){
uint32 i;
uint8 *c;
if (!str->len) return str;//pass on
qbs *tqbs=NULL;
if (str->tmp){ if (!str->fixed){ if (!str->readonly){ if (!str->in_cmem){ tqbs=str; }}}}
if (!tqbs){
//also pass on if already lowercase
c=str->chr;
for (i=0;i<str->len;i++){
 if ((*c>=65)&&(*c<=90)) goto qbs_lcase_cantpass;
 c++;
}
return str;
qbs_lcase_cantpass:
tqbs=qbs_new(str->len,1); memcpy(tqbs->chr,str->chr,str->len);
}
c=tqbs->chr;
for (i=0;i<tqbs->len;i++){
 if ((*c>=65)&&(*c<=90)) *c+=32;
 c++;
}
if (tqbs!=str) if (str->tmp) qbs_free(str);
return tqbs;
}

qbs *func_chr(int32 value){
qbs *tqbs;
if ((value<0)||(value>255)){
tqbs=qbs_new(0,1);
error(5);
}else{
tqbs=qbs_new(1,1);
tqbs->chr[0]=value;
}
return tqbs;
}


qbs *func_varptr_helper(uint8 type,uint16 offset){
//*creates a 3 byte string using the values given
qbs *tqbs;
tqbs=qbs_new(3,1);
tqbs->chr[0]=type;
tqbs->chr[1]=offset&255;
tqbs->chr[2]=offset>>8;
return tqbs;
}

qbs *qbs_left(qbs *str,int32 l){
if (l>str->len) l=str->len;
if (l<0) l=0;
if (l==str->len) return str;//pass on
if (str->tmp){ if (!str->fixed){ if (!str->readonly){ if (!str->in_cmem){ str->len=l; return str; }}}}
qbs *tqbs;
tqbs=qbs_new(l,1);
if (l) memcpy(tqbs->chr,str->chr,l);
if (str->tmp) qbs_free(str);
return tqbs;
}

qbs *qbs_right(qbs *str,int32 l){
if (l>str->len) l=str->len;
if (l<0) l=0;
if (l==str->len) return str;//pass on
if (str->tmp){ if (!str->fixed){ if (!str->readonly){ if (!str->in_cmem){
str->chr=str->chr+(str->len-l);
str->len=l; return str;
}}}}
qbs *tqbs;
tqbs=qbs_new(l,1);
if (l) memcpy(tqbs->chr,str->chr+str->len-l,l);
tqbs->len=l;
if (str->tmp) qbs_free(str);
return tqbs;
}

qbs *func_mksmbf(float val){
static qbs *tqbs;
tqbs=qbs_new(4,1);
if (_fieeetomsbin(&val,(float*)tqbs->chr)==1) {error(5); tqbs->len=0;}
return tqbs;
}
qbs *func_mkdmbf(double val){
static qbs *tqbs;
tqbs=qbs_new(8,1);
if (_dieeetomsbin(&val,(double*)tqbs->chr)==1) {error(5); tqbs->len=0;}
return tqbs;
}

float func_cvsmbf(qbs *str){
static float val;
if (str->len<4) {error(5); return 0;}
if (_fmsbintoieee((float*)str->chr,&val)==1) {error(5); return 0;}
return val;
}
double func_cvdmbf(qbs *str){
static double val;
if (str->len<8) {error(5); return 0;}
if (_dmsbintoieee((double*)str->chr,&val)==1) {error(5); return 0;}
return val;
}

qbs *b2string(char v){ static qbs *tqbs; tqbs=qbs_new(1,1); *((char*)(tqbs->chr))=v; return tqbs;}
qbs *ub2string(char v){ static qbs *tqbs; tqbs=qbs_new(1,1); *((uint8*)(tqbs->chr))=v; return tqbs;}
qbs *i2string(int16 v){ static qbs *tqbs; tqbs=qbs_new(2,1); *((int16*)(tqbs->chr))=v; return tqbs;}
qbs *ui2string(int16 v){ static qbs *tqbs; tqbs=qbs_new(2,1); *((uint16*)(tqbs->chr))=v; return tqbs;}
qbs *l2string(int32 v){ static qbs *tqbs; tqbs=qbs_new(4,1); *((int32*)(tqbs->chr))=v; return tqbs;}
qbs *ul2string(uint32 v){ static qbs *tqbs; tqbs=qbs_new(4,1); *((uint32*)(tqbs->chr))=v; return tqbs;}
qbs *i642string(int64 v){ static qbs *tqbs; tqbs=qbs_new(8,1); *((int64*)(tqbs->chr))=v; return tqbs;}
qbs *ui642string(uint64 v){ static qbs *tqbs; tqbs=qbs_new(8,1); *((uint64*)(tqbs->chr))=v; return tqbs;}
qbs *s2string(float v){ static qbs *tqbs; tqbs=qbs_new(4,1); *((float*)(tqbs->chr))=v; return tqbs;}
qbs *d2string(double v){ static qbs *tqbs; tqbs=qbs_new(8,1); *((double*)(tqbs->chr))=v; return tqbs;}
qbs *f2string(long double v){ static qbs *tqbs; tqbs=qbs_new(32,1); memset(tqbs->chr,0,32); *((long double*)(tqbs->chr))=v; return tqbs;}
qbs *bit2string(uint32 bsize,int64 v){
static qbs* tqbs;
tqbs=qbs_new(8,1);
bmask=~(-(((int64)1)<<bsize));
*((int64*)(tqbs->chr))=v&bmask;
tqbs->len=(bsize+7)>>3;
return tqbs;
}
qbs *ubit2string(uint32 bsize,uint64 v){
static qbs* tqbs;

tqbs=qbs_new(8,1);
bmask=~(-(((int64)1)<<bsize));
*((uint64*)(tqbs->chr))=v&bmask;
tqbs->len=(bsize+7)>>3;
return tqbs;
}

char string2b(qbs*str){ if (str->len<1) {error(5); return 0;} else {return *((char*)str->chr);} }
uint8 string2ub(qbs*str){ if (str->len<1) {error(5); return 0;} else {return *((uint8*)str->chr);} }
int16 string2i(qbs*str){ if (str->len<2) {error(5); return 0;} else {return *((int16*)str->chr);} }
uint16 string2ui(qbs*str){ if (str->len<2) {error(5); return 0;} else {return *((uint16*)str->chr);} }
int32 string2l(qbs*str){ if (str->len<4) {error(5); return 0;} else {return *((int32*)str->chr);} }
uint32 string2ul(qbs*str){ if (str->len<4) {error(5); return 0;} else {return *((uint32*)str->chr);} }
int64 string2i64(qbs*str){ if (str->len<8) {error(5); return 0;} else {return *((int64*)str->chr);} }
uint64 string2ui64(qbs*str){ if (str->len<8) {error(5); return 0;} else {return *((uint64*)str->chr);} }
float string2s(qbs*str){ if (str->len<4) {error(5); return 0;} else {return *((float*)str->chr);} }
double string2d(qbs*str){ if (str->len<8) {error(5); return 0;} else {return *((double*)str->chr);} }
long double string2f(qbs*str){ if (str->len<32) {error(5); return 0;} else {return *((long double*)str->chr);} }
uint64 string2ubit(qbs*str,uint32 bsize){
if (str->len<((bsize+7)>>3)) {error(5); return 0;}
bmask=~(-(((int64)1)<<bsize));
return (*(uint64*)str->chr)&bmask;
}
int64 string2bit(qbs*str,uint32 bsize){
if (str->len<((bsize+7)>>3)) {error(5); return 0;}
bmask=~(-(((int64)1)<<bsize));
bval64=(*(uint64*)str->chr)&bmask;
if (bval64&(((int64)1)<<(bsize-1))) return (bval64|(~bmask));
return bval64;
}

void lrset_field(qbs *str);

void sub_lset(qbs *dest,qbs *source){
if (new_error) return;
if (source->len>=dest->len){
if (dest->len) memcpy(dest->chr,source->chr,dest->len);
goto field_check;
}
if (source->len) memcpy(dest->chr,source->chr,source->len);
memset(dest->chr+source->len,32,dest->len-source->len);
field_check:
if (dest->field) lrset_field(dest);
}

void sub_rset(qbs *dest,qbs *source){
if (new_error) return;
if (source->len>=dest->len){
if (dest->len) memcpy(dest->chr,source->chr,dest->len);
goto field_check;
}
if (source->len) memcpy(dest->chr+dest->len-source->len,source->chr,source->len);
memset(dest->chr,32,dest->len-source->len);
field_check:
if (dest->field) lrset_field(dest);
}




qbs *func_space(int32 spaces){
static qbs *tqbs;
if (spaces<0) spaces=0;
tqbs=qbs_new(spaces,1);
if (spaces) memset(tqbs->chr,32,spaces);
return tqbs;
}

qbs *func_string(int32 characters,int32 asciivalue){
static qbs *tqbs;
if (characters<0) characters=0;
tqbs=qbs_new(characters,1);
if (characters) memset(tqbs->chr,asciivalue&0xFF,characters);
return tqbs;
}

int32 func_instr(int32 start,qbs *str,qbs *substr,int32 passed){
//QB64 difference: start can be 0 or negative
//justification-start could be larger than the length of string to search in QBASIC
static uint8 *limit,*base;
static uint8 firstc;
if (!passed) start=1;
if (!str->len) return 0;
if (start<1){
start=1;
if (!substr->len) return 0;
}
if (start>str->len) return 0;
if (!substr->len) return start;
if ((start+substr->len-1)>str->len) return 0;
limit=str->chr+str->len;
firstc=substr->chr[0];
base=str->chr+start-1;
nextchar:
base=(uint8*)memchr(base,firstc,limit-base);
if (!base) return 0;
if ((base+substr->len)>limit) return 0;
if (!memcmp(base,substr->chr,substr->len)) return base-str->chr+1;
base++;
if ((base+substr->len)>limit) return 0;
goto nextchar;
}

void sub_mid(qbs *dest,int32 start,int32 l,qbs* src,int32 passed){
if (new_error) return;
static int32 src_offset;
if (!passed) l=src->len;
src_offset=0;
if (dest==nothingstring) return;//quiet exit, error has already been reported!
if (start<1){
l=l+start-1;
src_offset=-start+1;//src_offset is a byte offset with base 0!
start=1;
}
if (l<=0) return;
if (start>dest->len) return;
if ((start+l-1)>dest->len) l=dest->len-start+1;
//start and l are now reflect a valid region within dest
if (src_offset>=src->len) return;
if (l>(src->len-src_offset)) l=src->len-src_offset;
//src_offset and l now reflect a valid region within src
if (dest==src){
if ((start-1)!=src_offset) memmove(dest->chr+start-1,src->chr+src_offset,l);
}else{
memcpy(dest->chr+start-1,src->chr+src_offset,l);
}
}

qbs *func_mid(qbs *str,int32 start,int32 l,int32 passed){
static qbs *tqbs;
if (passed){
 if (start<1) {l=l-1+start; start=1;}
 if ((l>=1)&&(start<=str->len)){
 if ((start+l)>str->len) l=str->len-start+1;
 }else{
 l=0; start=1;//nothing!
 }
}else{
 if (start<1) start=1;
 l=str->len-start+1;
 if (l<1){
 l=0; start=1;//nothing!
 }
}
if ((start==1)&&(l==str->len)) return str;//pass on
if (str->tmp){ if (!str->fixed){ if (!str->readonly){ if (!str->in_cmem){//acquire
str->chr=str->chr+(start-1);
str->len=l;
return str;
}}}}
tqbs=qbs_new(l,1);
if (l) memcpy(tqbs->chr,str->chr+start-1,l);
if (str->tmp) qbs_free(str);
return tqbs;
}

qbs *qbs_ltrim(qbs *str){
if (!str->len) return str;//pass on
if (*str->chr!=32) return str;//pass on
if (str->tmp){ if (!str->fixed){ if (!str->readonly){ if (!str->in_cmem){//acquire?
qbs_ltrim_nextchar:
if (*str->chr==32){
str->chr++;
if (--str->len) goto qbs_ltrim_nextchar;
}
return str;
}}}}
int32 i;
i=0;
qbs_ltrim_nextchar2: if (str->chr[i]==32) {i++; if (i<str->len) goto qbs_ltrim_nextchar2;}
qbs *tqbs;
tqbs=qbs_new(str->len-i,1);
if (tqbs->len) memcpy(tqbs->chr,str->chr+i,tqbs->len);
if (str->tmp) qbs_free(str);
return tqbs;
}

qbs *qbs_rtrim(qbs *str){
if (!str->len) return str;//pass on
if (str->chr[str->len-1]!=32) return str;//pass on
if (str->tmp){ if (!str->fixed){ if (!str->readonly){ if (!str->in_cmem){//acquire?
qbs_rtrim_nextchar:
if (str->chr[str->len-1]==32){
if (--str->len) goto qbs_rtrim_nextchar;
}
return str;
}}}}
int32 i;
i=str->len;
qbs_rtrim_nextchar2: if (str->chr[i-1]==32) {i--; if (i) goto qbs_rtrim_nextchar2;}
//i is the number of characters to keep
qbs *tqbs;
tqbs=qbs_new(i,1);
if (i) memcpy(tqbs->chr,str->chr,i);
if (str->tmp) qbs_free(str);
return tqbs;
}

qbs *qbs_inkey(){
if (new_error) return qbs_new(0,1);
qbs *tqbs;
Sleep(0);
tqbs=qbs_new(2,1);
if (cmem[0x41a]!=cmem[0x41c]){
tqbs->chr[0]=cmem[0x400+cmem[0x41a]];
tqbs->chr[1]=cmem[0x400+cmem[0x41a]+1];
if (tqbs->chr[0]){
tqbs->len=1;
}else{
if (tqbs->chr[1]==0) tqbs->len=1;
}
cmem[0x41a]+=2;
if (cmem[0x41a]==62) cmem[0x41a]=30;
}else{
tqbs->len=0;
}
return tqbs;
}

//STR() functions
//singed integers
qbs *qbs_str(int64 value){
qbs *tqbs;
tqbs=qbs_new(20,1);
#ifdef QB64_WINDOWS
 tqbs->len=sprintf((char*)tqbs->chr,"% I64i",value);
#else
 tqbs->len=sprintf((char*)tqbs->chr,"% lli",value);
#endif
return tqbs;
}
qbs *qbs_str(int32 value){
qbs *tqbs;
tqbs=qbs_new(11,1);
tqbs->len=sprintf((char*)tqbs->chr,"% i",value);
return tqbs;
}
qbs *qbs_str(int16 value){
qbs *tqbs;
tqbs=qbs_new(6,1);
tqbs->len=sprintf((char*)tqbs->chr,"% i",value);
return tqbs;
}
qbs *qbs_str(int8 value){
qbs *tqbs;
tqbs=qbs_new(4,1);
tqbs->len=sprintf((char*)tqbs->chr,"% i",value);
return tqbs;
}
//unsigned integers
qbs *qbs_str(uint64 value){
qbs *tqbs;
tqbs=qbs_new(21,1);
#ifdef QB64_WINDOWS
 tqbs->len=sprintf((char*)tqbs->chr," %I64u",value);
#else
 tqbs->len=sprintf((char*)tqbs->chr," %llu",value);
#endif
return tqbs;
}
qbs *qbs_str(uint32 value){
qbs *tqbs;
tqbs=qbs_new(11,1);
tqbs->len=sprintf((char*)tqbs->chr," %u",value);
return tqbs;
}
qbs *qbs_str(uint16 value){
qbs *tqbs;
tqbs=qbs_new(6,1);
tqbs->len=sprintf((char*)tqbs->chr," %u",value);
return tqbs;
}
qbs *qbs_str(uint8 value){
qbs *tqbs;
tqbs=qbs_new(4,1);
tqbs->len=sprintf((char*)tqbs->chr," %u",value);
return tqbs;
}



uint8 func_str_fmt[7];
uint8 qbs_str_buffer[32];
uint8 qbs_str_buffer2[32];

qbs *qbs_str(float value){
static qbs *tqbs;
tqbs=qbs_new(16,1);
static int32 l,i,i2,i3,digits,exponent;
l=sprintf((char*)&qbs_str_buffer,"% .6E",value);
//IMPORTANT: assumed l==14
if (l==13){memmove(&qbs_str_buffer[12],&qbs_str_buffer[11],2); qbs_str_buffer[11]=48; l=14;}

digits=7;
for (i=8;i>=1;i--){
if (qbs_str_buffer[i]==48){
digits--;
}else{
if (qbs_str_buffer[i]!=46) break;
}
}//i
//no significant digits? simply return 0
if (digits==0){
tqbs->len=2; tqbs->chr[0]=32; tqbs->chr[1]=48;//tqbs=[space][0]
return tqbs;
}
//calculate exponent
exponent=(qbs_str_buffer[11]-48)*100+(qbs_str_buffer[12]-48)*10+(qbs_str_buffer[13]-48);
if (qbs_str_buffer[10]==45) exponent=-exponent;
if ((exponent<=6)&&((exponent-digits)>=-8)) goto asdecimal;
//fix up exponent to conform to QBASIC standards
//i. cull trailing 0's after decimal point (use digits to help)
//ii. cull leading 0's of exponent

i3=0;
i2=digits+2;
if (digits==1) i2--;//don't include decimal point
for (i=0;i<i2;i++) {tqbs->chr[i3]=qbs_str_buffer[i]; i3++;}
for (i=9;i<=10;i++) {tqbs->chr[i3]=qbs_str_buffer[i]; i3++;}
exponent=abs(exponent);
//i2=13;
//if (exponent>9) i2=12;
i2=12;//override: if exponent is less than 10 still display a leading 0
if (exponent>99) i2=11;
for (i=i2;i<=13;i++) {tqbs->chr[i3]=qbs_str_buffer[i]; i3++;}
tqbs->len=i3;
return tqbs;
/////////////////////
asdecimal:
//calculate digits after decimal point in var. i
i=-(exponent-digits+1);
if (i<0) i=0;
func_str_fmt[0]=37;//"%"
func_str_fmt[1]=32;//" "
func_str_fmt[2]=46;//"."
func_str_fmt[3]=i+48;
func_str_fmt[4]=102;//"f"
func_str_fmt[5]=0;
tqbs->len=sprintf((char*)tqbs->chr,(const char*)&func_str_fmt,value);
if (tqbs->chr[1]==48){//must manually cull leading 0
memmove(tqbs->chr+1,tqbs->chr+2,tqbs->len-2);
tqbs->len--;
}
return tqbs;
}

qbs *qbs_str(double value){
static qbs *tqbs;
tqbs=qbs_new(32,1);
static int32 l,i,i2,i3,digits,exponent;

l=sprintf((char*)&qbs_str_buffer,"% .15E",value);
//IMPORTANT: assumed l==23
if (l==22){memmove(&qbs_str_buffer[21],&qbs_str_buffer[20],2); qbs_str_buffer[20]=48; l=23;}

//check if the 16th significant digit is 9, if it is round to 15 significant digits
if (qbs_str_buffer[17]==57){
sprintf((char*)&qbs_str_buffer2,"% .14E",value);
memmove(&qbs_str_buffer,&qbs_str_buffer2,17);
qbs_str_buffer[17]=48;
}
qbs_str_buffer[18]=68; //change E to D (QBASIC standard)
digits=16;
for (i=17;i>=1;i--){
if (qbs_str_buffer[i]==48){
digits--;
}else{
if (qbs_str_buffer[i]!=46) break;
}
}//i
//no significant digits? simply return 0
if (digits==0){
tqbs->len=2; tqbs->chr[0]=32; tqbs->chr[1]=48;//tqbs=[space][0]
return tqbs;
}
//calculate exponent
exponent=(qbs_str_buffer[20]-48)*100+(qbs_str_buffer[21]-48)*10+(qbs_str_buffer[22]-48);
if (qbs_str_buffer[19]==45) exponent=-exponent;
//OLD if ((exponent<=15)&&((exponent-digits)>=-16)) goto asdecimal;
if ((exponent<=15)&&((exponent-digits)>=-17)) goto asdecimal;
//fix up exponent to conform to QBASIC standards
//i. cull trailing 0's after decimal point (use digits to help)
//ii. cull leading 0's of exponent
i3=0;
i2=digits+2;
if (digits==1) i2--;//don't include decimal point
for (i=0;i<i2;i++) {tqbs->chr[i3]=qbs_str_buffer[i]; i3++;}
for (i=18;i<=19;i++) {tqbs->chr[i3]=qbs_str_buffer[i]; i3++;}
exponent=abs(exponent);
//i2=22;
//if (exponent>9) i2=21;
i2=21;//override: if exponent is less than 10 still display a leading 0
if (exponent>99) i2=20;
for (i=i2;i<=22;i++) {tqbs->chr[i3]=qbs_str_buffer[i]; i3++;}
tqbs->len=i3;
return tqbs;
/////////////////////
asdecimal:
//calculate digits after decimal point in var. i
i=-(exponent-digits+1);
if (i<0) i=0;
func_str_fmt[0]=37;//"%"
func_str_fmt[1]=32;//" "
func_str_fmt[2]=46;//"."
if (i>9){
func_str_fmt[3]=49;//"1"
func_str_fmt[4]=(i-10)+48;
}else{
func_str_fmt[3]=48;//"0"
func_str_fmt[4]=i+48;
}
func_str_fmt[5]=102;//"f"
func_str_fmt[6]=0;
tqbs->len=sprintf((char*)tqbs->chr,(const char*)&func_str_fmt,value);
if (tqbs->chr[1]==48){//must manually cull leading 0
memmove(tqbs->chr+1,tqbs->chr+2,tqbs->len-2);
tqbs->len--;
}
return tqbs;
}

qbs *qbs_str(long double value){
//not fully implemented
return qbs_str((double)value);
}


int32 qbs_equal(qbs *str1,qbs *str2){
if (str1->len!=str2->len) return 0;
if (memcmp(str1->chr,str2->chr,str1->len)==0) return -1;
return 0;
}
int32 qbs_notequal(qbs *str1,qbs *str2){
if (str1->len!=str2->len) return -1;
if (memcmp(str1->chr,str2->chr,str1->len)==0) return 0;
return -1;
}
int32 qbs_greaterthan(qbs *str1,qbs *str2){
static int32 i;
if (str1->len<=str2->len){
i=memcmp(str1->chr,str2->chr,str1->len);
if (i>0) return -1;
return 0;
}else{
i=memcmp(str1->chr,str2->chr,str2->len);
if (i<0) return 0;
return -1;
}
}
int32 qbs_lessthan(qbs *str1,qbs *str2){
static int32 i;
if (str1->len<=str2->len){
if (!str1->len) if (str2->len) return -1; else return 0;
i=memcmp(str1->chr,str2->chr,str1->len);
if (i<0) return -1;
return 0;
}else{
i=memcmp(str1->chr,str2->chr,str2->len);
if (i>=0) return 0;
return -1;
}
}
int32 qbs_lessorequal(qbs *str1,qbs *str2){
static int32 i;
if (str1->len<=str2->len){
i=memcmp(str1->chr,str2->chr,str1->len);
if (i<=0) return -1;
return 0;
}else{
i=memcmp(str1->chr,str2->chr,str2->len);
if (i>=0) return 0;
return -1;
}
}
int32 qbs_greaterorequal(qbs *str1,qbs *str2){
static int32 i;
//greater?
if (str1->len<=str2->len){
i=memcmp(str1->chr,str2->chr,str1->len);
if (i>0) return -1;
if (i==0) if (str1->len==str2->len) return -1;//equal?
return 0;
}else{
i=memcmp(str1->chr,str2->chr,str2->len);
if (i<0) return 0;
return -1;
}
}

int32 qbs_asc(qbs *str,uint32 i){//uint32 speeds up checking for negative
i--;
if (i<str->len){
return str->chr[i];
}
error(5);
return 0;
}


int32 qbs_asc(qbs *str){
if (str->len) return str->chr[0];
error(5);
return 0;
}

int32 qbs_len(qbs *str){
return str->len;
}


//QBG BLOCK
int32 qbg_mode=-1;//-1 means not initialized!
int32 qbg_text_only;
//text & graphics modes
int32 qbg_height_in_characters, qbg_width_in_characters;
int32 qbg_top_row, qbg_bottom_row;
int32 qbg_cursor_x, qbg_cursor_y;
int32 qbg_character_height, qbg_character_width;
uint32 qbg_color, qbg_background_color;
//text mode ONLY
int32 qbg_cursor_show;
int32 qbg_cursor_firstvalue, qbg_cursor_lastvalue;//these values need revision
//graphics modes ONLY
int32 qbg_width, qbg_height;
float qbg_x, qbg_y;
int32 qbg_bits_per_pixel, qbg_pixel_mask; //for monochrome modes 1b, for 16 color 1111b, for 256 color 11111111b
int32 qbg_bytes_per_pixel;
int32 qbg_clipping_or_scaling;//1=clipping, 2=clipping and scaling
int32 qbg_view_x1, qbg_view_y1, qbg_view_x2, qbg_view_y2;
int32 qbg_view_offset_x, qbg_view_offset_y;
float qbg_scaling_x, qbg_scaling_y;
float qbg_scaling_offset_x, qbg_scaling_offset_y;
float qbg_window_x1, qbg_window_y1, qbg_window_x2, qbg_window_y2;
int32 qbg_pages;
uint32 *qbg_pageoffsets;
int32 *qbg_cursor_x_previous; //used to recover old cursor position
int32 *qbg_cursor_y_previous;
int32 qbg_active_page;
uint8 *qbg_active_page_offset;
int32 qbg_visual_page;
uint8 *qbg_visual_page_offset;
int32 qbg_color_assign[256];//for modes with quasi palettes!
uint32 pal_mode10[2][9];













uint8 charset8x8[256][8][8];
uint8 charset8x16[256][16][8];

int32 lineclip_draw;//1=draw, 0=don't draw
int32 lineclip_x1,lineclip_y1,lineclip_x2,lineclip_y2;
int32 lineclip_skippixels;//the number of pixels from x1,y1 which won't be drawn

void lineclip(int32 x1,int32 y1,int32 x2,int32 y2,int32 xmin,int32 ymin,int32 xmax,int32 ymax){
static double mx,my,y,x,d;
static int32 xdis,ydis;
lineclip_skippixels=0;


if (x1>=xmin){ if (x1<=xmax){ if (y1>=ymin){ if (y1<=ymax){//(x1,y1) onscreen?
if (x1==x2) if (y1==y2) goto singlepoint;//is it a single point? (needed to avoid "division by 0" errors)
goto gotx1y1;
}}}}

//(x1,y1) offscreen...

if (x1==x2) if (y1==y2){lineclip_draw=0; return;}//offscreen single point

//ignore entirely offscreen lines requiring no further calculations
if (x1<xmin) if (x2<xmin){lineclip_draw=0; return;}
if (x1>xmax) if (x2>xmax){lineclip_draw=0; return;}
if (y1<ymin) if (y2<ymin){lineclip_draw=0; return;}
if (y1>ymax) if (y2>ymax){lineclip_draw=0; return;}

mx=(x2-x1)/fabs((double)(y2-y1)); my=(y2-y1)/fabs((double)(x2-x1));
//right wall from right
if (x1>xmax){
if (mx<0){
y=(double)y1+((double)x1-(double)xmax)*my;
if (y>=ymin){ if (y<=ymax){
  //double space indented values calculate pixels to skip
  xdis=x1; ydis=y1;
x1=xmax; y1=qbr_float_to_long(y);
  xdis=abs(xdis-x1); ydis=abs(ydis-y1);
  if (xdis>=ydis) lineclip_skippixels=xdis; else lineclip_skippixels=ydis;
goto gotx1y1;
}}
}
}
//left wall from left
if (x1<xmin){
if (mx>0){
y=(double)y1+((double)xmin-(double)x1)*my;
if (y>=ymin){ if (y<=ymax){
  //double space indented values calculate pixels to skip
  xdis=x1; ydis=y1;
x1=xmin; y1=qbr_float_to_long(y);
  xdis=abs(xdis-x1); ydis=abs(ydis-y1);

  if (xdis>=ydis) lineclip_skippixels=xdis; else lineclip_skippixels=ydis;
goto gotx1y1;
}}
}
}
//top wall from top
if (y1<ymin){
if (my>0){
x=(double)x1+((double)ymin-(double)y1)*mx;
if (x>=xmin){ if (x<=xmax){
  //double space indented values calculate pixels to skip
  xdis=x1; ydis=y1;
x1=qbr_float_to_long(x); y1=ymin;
  xdis=abs(xdis-x1); ydis=abs(ydis-y1);
  if (xdis>=ydis) lineclip_skippixels=xdis; else lineclip_skippixels=ydis;
goto gotx1y1;
}}
}
}
//bottom wall from bottom
if (y1>ymax){
if (my<0){
x=(double)x1+((double)y1-(double)ymax)*mx;
if (x>=xmin){ if (x<=xmax){
  //double space indented values calculate pixels to skip
  xdis=x1; ydis=y1;
x1=qbr_float_to_long(x); y1=ymax;
  xdis=abs(xdis-x1); ydis=abs(ydis-y1);
  if (xdis>=ydis) lineclip_skippixels=xdis; else lineclip_skippixels=ydis;
goto gotx1y1;
}}
}
}
lineclip_draw=0;
return;
gotx1y1:

if (x2>=xmin){ if (x2<=xmax){ if (y2>=ymin){ if (y2<=ymax){
goto gotx2y2;
}}}}


mx=(x1-x2)/fabs((double)(y1-y2)); my=(y1-y2)/fabs((double)(x1-x2));
//right wall from right
if (x2>xmax){
if (mx<0){
y=(double)y2+((double)x2-(double)xmax)*my;
if (y>=ymin){ if (y<=ymax){
x2=xmax; y2=qbr_float_to_long(y);
goto gotx2y2;
}}
}
}
//left wall from left
if (x2<xmin){
if (mx>0){
y=(double)y2+((double)xmin-(double)x2)*my;
if (y>=ymin){ if (y<=ymax){
x2=xmin; y2=qbr_float_to_long(y);
goto gotx2y2;
}}
}
}
//top wall from top
if (y2<ymin){
if (my>0){
x=(double)x2+((double)ymin-(double)y2)*mx;
if (x>=xmin){ if (x<=xmax){
x2=qbr_float_to_long(x); y2=ymin;
goto gotx2y2;
}}
}
}
//bottom wall from bottom
if (y2>ymax){
if (my<0){
x=(double)x2+((double)y2-(double)ymax)*mx;
if (x>=xmin){ if (x<=xmax){
x2=qbr_float_to_long(x); y2=ymax;
goto gotx2y2;
}}
}
}
lineclip_draw=0;
return;
gotx2y2:
singlepoint:
lineclip_draw=1;
lineclip_x1=x1; lineclip_y1=y1; lineclip_x2=x2; lineclip_y2=y2;


return;
}

void qbg_palette(uint32 attribute,uint32 col,int32 passed){
static int32 r,g,b;
if (new_error) return;
if (!passed){restorepalette(write_page); return;}

//32-bit
if (write_page->bytes_per_pixel==4) goto error;

attribute&=255;//patch to support QBASIC overflow "bug"

if ((write_page->compatible_mode==13)||(write_page->compatible_mode==256)){
if (col&0xFFC0C0C0) goto error;//11111111110000001100000011000000b
r=col&63; g=(col>>8)&63; b=(col>>16)&63;
r=qbr((double)r*4.063492f-0.4999999f); g=qbr((double)g*4.063492f-0.4999999f); b=qbr((double)b*4.063492f-0.4999999f);
write_page->pal[attribute]=b+g*256+r*65536;
//Upgraded from (((col<<2)&0xFF)<<16)+(((col>>6)&0xFF)<<8)+((col>>14)&0xFF)
return;
}

if (write_page->compatible_mode==12){
if (attribute>15) goto error;
if (col&0xFFC0C0C0) goto error;//11111111110000001100000011000000b
r=col&63; g=(col>>8)&63; b=(col>>16)&63;
r=qbr((double)r*4.063492f-0.4999999f); g=qbr((double)g*4.063492f-0.4999999f); b=qbr((double)b*4.063492f-0.4999999f);
write_page->pal[attribute]=b+g*256+r*65536;
return;
}

if (write_page->compatible_mode==11){
if (attribute>1) goto error;
if (col&0xFFC0C0C0) goto error;//11111111110000001100000011000000b
r=col&63; g=(col>>8)&63; b=(col>>16)&63;
r=qbr((double)r*4.063492f-0.4999999f); g=qbr((double)g*4.063492f-0.4999999f); b=qbr((double)b*4.063492f-0.4999999f);
write_page->pal[attribute]=b+g*256+r*65536;
return;
}

if (write_page->compatible_mode==10){
if (attribute>3) goto error;
if ((col<0)||(col>8)) goto error;
write_page->pal[attribute+4]=col;
return;
}

if (write_page->compatible_mode==9){
if (attribute>15) goto error;
if ((col<0)||(col>63)) goto error;
write_page->pal[attribute]=palette_64[col];
return;
}

if (write_page->compatible_mode==8){
if (attribute>15) goto error;
if ((col<0)||(col>15)) goto error;
write_page->pal[attribute]=palette_256[col];
return;
}

if (write_page->compatible_mode==7){
if (attribute>15) goto error;
if ((col<0)||(col>15)) goto error;
write_page->pal[attribute]=palette_256[col];
return;
}

if (write_page->compatible_mode==2){
if (attribute>1) goto error;
if ((col<0)||(col>15)) goto error;
write_page->pal[attribute]=palette_256[col];
return;
}

if (write_page->compatible_mode==1){
if (attribute>15) goto error;
if ((col<0)||(col>15)) goto error;
write_page->pal[attribute]=palette_256[col];
return;
}

if (write_page->compatible_mode==0){
if (attribute>15) goto error;
if ((col<0)||(col>63)) goto error;
write_page->pal[attribute]=palette_64[col];
return;
}

error:
error(5);
return;

}




void qbg_sub_color(uint32 col1,uint32 col2,uint32 bordercolor,int32 passed){
if (new_error) return;
if (!passed){
//performs no action if nothing passed (as in QBASIC for some modes)
return;
}

if (write_page->compatible_mode==32){
if (passed&4) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->background_color=col2;
return;
}
if (write_page->compatible_mode==256){
if (passed&4) goto error;
if (passed&1) if (col1>255) goto error;
if (passed&2) if (col2>255) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->background_color=col2;
return;
}
if (write_page->compatible_mode==13){
//if (passed&6) goto error;
//if (col1>255) goto error;
//write_page->color=col1;
if (passed&4) goto error;
if (passed&1) if (col1>255) goto error;
if (passed&2) if (col2>255) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->background_color=col2;
return;
}
if (write_page->compatible_mode==12){
//if (passed&6) goto error;
//if (col1>15) goto error;
//write_page->color=col1;
if (passed&4) goto error;
if (passed&1) if (col1>15) goto error;
if (passed&2) if (col2>15) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->background_color=col2;
return;
}
if (write_page->compatible_mode==11){
//if (passed&6) goto error;
//if (col1>1) goto error;
//write_page->color=col1;
if (passed&4) goto error;
if (passed&1) if (col1>1) goto error;
if (passed&2) if (col2>1) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->background_color=col2;
return;
}
if (write_page->compatible_mode==10){
if (passed&4) goto error;
if (passed&1) if (col1>3) goto error;
if (passed&2) if (col2>8) goto error;
if (passed&1) write_page->color=col1;
//if (passed&2) ..._color_assign[0]=col2;
if (passed&2) write_page->pal[4]=col2;
return;
}
if (write_page->compatible_mode==9){
if (passed&4) goto error;
if (passed&1) if (col1>15) goto error;
if (passed&2) if (col2>63) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->pal[0]=palette_64[col2];
return;
}
if (write_page->compatible_mode==8){
if (passed&4) goto error;
if (passed&1) if (col1>15) goto error;
if (passed&2) if (col2>15) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->pal[0]=palette_256[col2];
return;
}
if (write_page->compatible_mode==7){
if (passed&4) goto error;
if (passed&1) if (col1>15) goto error;
if (passed&2) if (col2>15) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->pal[0]=palette_256[col2];
return;
}
if (write_page->compatible_mode==2){
if (passed&4) goto error;
if (passed&1) if (col1>1) goto error;
if (passed&2) if (col2>15) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->pal[0]=palette_256[col2];
return;
}
if (write_page->compatible_mode==1){
if (passed&4) goto error;
if (passed&1){
if (col1>15) goto error;
write_page->pal[0]=palette_256[col1];
}
if (passed&2){
if (col2&1){
write_page->pal[1]=palette_256[3];
write_page->pal[2]=palette_256[5];
write_page->pal[3]=palette_256[7];
}else{
write_page->pal[1]=palette_256[2];
write_page->pal[2]=palette_256[4];
write_page->pal[3]=palette_256[6];
}
}
return;
}
if (write_page->compatible_mode==0){
if (passed&1) if (col1>31) goto error;
if (passed&2) if (col2>15) goto error;
if (passed&1) write_page->color=col1;
if (passed&2) write_page->background_color=col2&7;
return;
}
error:
error(5);
return;
}

void defaultcolors(){
write_page->color=15; write_page->background_color=0;
if (write_page->compatible_mode==0){write_page->color=7; write_page->background_color=0;}
if (write_page->compatible_mode==1){write_page->color=3; write_page->background_color=0;}
if (write_page->compatible_mode==2){write_page->color=1; write_page->background_color=0;}
if (write_page->compatible_mode==10){write_page->color=3; write_page->background_color=0;}
if (write_page->compatible_mode==11){write_page->color=1; write_page->background_color=0;}
if (write_page->compatible_mode==32){write_page->color=0xFFFFFFFF; write_page->background_color=0xFF000000;}
write_page->draw_color=write_page->color;
return;
}

//Note: Cannot be used to setup page 0, just to validate it
void validatepage(int32 n){
static int32 i,i2;
//add new page indexes if necessary
if (n>=pages){
i=n+1;
page=(int32*)realloc(page,i*4);
memset(page+pages,0,(i-pages)*4);
pages=i;
}
//create page at index n if none exists
if (!page[n]){
//graphical (assumed)
 i=page[0];
 i2=imgnew(img[i].width,img[i].height,img[i].compatible_mode);
 //modify based on page 0's attributes
 //i. link palette to page 0's palette (if necessary)
 if (img[i2].bytes_per_pixel!=4){
 free(img[i2].pal); img[i2].flags^=IMG_FREEPAL;
 img[i2].pal=img[i].pal;
 }
 //ii. set flags
 img[i2].flags|=IMG_SCREEN;
 //iii. inherit font
 selectfont(img[i].font,&img[i2]);
 //text
 //...
 page[n]=i2;
}
return;
}//validate_page



void qbg_screen(int32 mode,int32 color_switch,int32 active_page,int32 visual_page,int32 refresh,int32 passed){
if (new_error) return;

if (width8050switch){
if ((passed!=1)||mode) width8050switch=0;
}

static int32 i,i2,i3,x,y,f,p;
static img_struct *im;
static int32 prev_width_in_characters,prev_height_in_characters;

static int32 last_active_page=0;//used for active page settings migration

i=0;//update flags
    //1=mode change required
    //2=page change required (used only to see if an early exit without locking is possible)

i2=page[0];
if (passed&1){//mode
if (mode<0){//custom screen
 i3=-mode;
 if (i3>=nextimg){error(258); return;}//within valid range?
 if (!img[i3].valid){error(258); return;}//valid? 
 if (i3!=i2) i=1; //is mode changing?
}else{
 if (mode==3) goto error;
 if (mode==4) goto error;
 if (mode==5) goto error;
 if (mode==6) goto error;
 if (mode>13) goto error;
 //is mode changing?
 if (i2){
 if (img[i2].compatible_mode!=mode) i=1;
 }else i=1;
 //force update if special parameters passed
 //(at present, only SCREEN 0 is ever called with these overrides, so handling
 // of these is done only in the SCREEN 0 section of the SCREEN sub)
 if ((sub_screen_width_in_characters!=-1)||(sub_screen_height_in_characters!=-1)||(sub_screen_font!=-1)) i=1;
}
}

if (passed&4){//active page
 if (active_page<0) goto error;
 if (!(passed&8)){//if visual page not specified, set it to the active page
 passed|=8;
 visual_page=active_page;
 }
 if (!(i&1)){//mode not changing
 //validate the passed active page, then see if it is the currently selected page
 validatepage(active_page); i2=page[active_page];
 if ((i2!=read_page_index)||(i2!=write_page_index)) i|=2;
 }
}//passed&4

if (passed&8){//visual page
 i3=visual_page;
 if (i3<0) goto error;
 if (!(i&1)){//mode not changing
 validatepage(visual_page); i2=page[visual_page];
 if (i2!=display_page_index) i|=2;
 }
}//passed&8

//if no changes need to be made exit before locking
if (!i) return;

if (autodisplay){
if (lock_display_required){//on init of main (), attempting a lock would create an infinite loop
if (i&1){//avoid locking when only changing the screen page
if (lock_display==0) lock_display=1;//request lock
while (lock_display!=2){
Sleep(0);
}
}
}
}

screen_last_valid=0;//ignore cache used to update the screen on next update

if (passed&1){//mode
if (i&1){//mode change necessary

//calculate previous width & height if possible
prev_width_in_characters=0; prev_height_in_characters=0; 
if (i=page[0]){//currently in a screen mode?
im=&img[i];
if (!im->compatible_mode){
prev_width_in_characters=im->width; prev_height_in_characters=im->height;
}else{
x=fontwidth[im->font]; if (!x) x=1;
prev_width_in_characters=im->width/x;
prev_height_in_characters=im->height/fontheight[im->font];
}
}//currently in a screen mode


//free any previously allocated surfaces
//free pages in reverse order
if (page[0]){//currently in a screen mode?
for (i=1;i<pages;i++){
if(i2=page[i]){
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}//i2
}//i
i=page[0];
if (sub_screen_keep_page0){
img[i].flags^=IMG_SCREEN;
}else{
if (img[i].flags&IMG_FREEMEM) free(img[i].offset);//free pixel data
if (img[i].flags&IMG_FREEPAL) free(img[i].pal);//free palette
freeimg(i);
}
}//currently in a screen mode
sub_screen_keep_page0=0;//reset to default status

pages=1; page[0]=0;

if (mode<0){//custom screen
i=-mode;
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
sub_screen_keep_page0=1;
}

//320 x 200 graphics
//40 x 25 text format, character box size of 8 x 8
//Assignment of up to 256K colors to up to 256 attributes
if (mode==13){
i=imgframe(&cmem[655360],320,200,13);
memset(img[i].offset,0,320*200);
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//13

//640 x 480 graphics
//80 x 30 or 80 x 60 text format, character box size of 8 x 16 or 8 x 8
//Assignment of up to 256K colors to 16 attributes
if (mode==12){
i=imgnew(640,480,12);
if ((prev_width_in_characters==80)&&(prev_height_in_characters==60)) selectfont(8,&img[i]);//override default font
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//12

/*
Screen 11
  ¦ 640 x 480 graphics
  ¦ 80 x 30 or 80 x 60 text format, character box size of 8 x 16 or 8 x 8
  ¦ Assignment of up to 256K colors to 2 attributes
*/
if (mode==11){
i=imgnew(640,480,11);
if ((prev_width_in_characters==80)&&(prev_height_in_characters==60)) selectfont(8,&img[i]);//override default font
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//11

//SCREEN 10: 640 x 350 graphics, monochrome monitor only
//  ¦ 80 x 25 or 80 x 43 text format, 8 x 14 or 8 x 8 character box size
//  ¦ 128K page size, page range is 0 (128K) or 0-1 (256K)
//  ¦ Up to 9 pseudocolors assigned to 4 attributes
/*
'colors swap every half second!
'using PALETTE does NOT swap color indexes
'0 black-black
'1 black-grey
'2 black-white
'3 grey-black
'4 grey-grey
'5 grey-white
'6 white-black
'7 white-grey
'8 white-white
'*IMPORTANT* QB sets initial values up different to default palette!
'0 block-black(0)
'1 grey-grey(4)
'2 white-black(6)
'3 white-white(8)
*/
if (mode==10){
i=imgnew(640,350,10);
if ((prev_width_in_characters==80)&&(prev_height_in_characters==43)) selectfont(8,&img[i]);//override default font
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];

}//10

/*
SCREEN 9: 640 x 350 graphics
  ¦ 80 x 25 or 80 x 43 text format, 8 x 14 or 8 x 8 character box size
  ¦ 64K page size, page range is 0 (64K);
    128K page size, page range is 0 (128K) or 0-1 (256K)
  ¦ 16 colors assigned to 4 attributes (64K adapter memory), or
    64 colors assigned to 16 attributes (more than 64K adapter memory)
*/
if (mode==9){
i=imgnew(640,350,9);
if ((prev_width_in_characters==80)&&(prev_height_in_characters==43)) selectfont(8,&img[i]);//override default font
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//9

/*
SCREEN 8: 640 x 200 graphics
  ¦ 80 x 25 text format, 8 x 8 character box
  ¦ 64K page size, page ranges are 0 (64K), 0-1 (128K), or 0-3 (246K)
  ¦ Assignment of 16 colors to any of 16 attributes
*/
if (mode==8){
i=imgnew(640,200,8);
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//8

/*
SCREEN 7: 320 x 200 graphics
  ¦ 40 x 25 text format, character box size 8 x 8
  ¦ 32K page size, page ranges are 0-1 (64K), 0-3 (128K), or 0-7 (256K)
  ¦ Assignment of 16 colors to any of 16 attributes
*/
if (mode==7){
i=imgnew(320,200,7);
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//7

/*
SCREEN 4:
  ¦ Supports Olivetti (R) Personal Computers models M24, M240, M28,
    M280, M380, M380/C, M380/T and AT&T (R) Personal Computers 6300
    series
  ¦ 640 x 400 graphics
  ¦ 80 x 25 text format, 8 x 16 character box
  ¦ 1 of 16 colors assigned as the foreground color (selected by the
    COLOR statement); background is fixed at black.
*/
//Note: QB64 will not support SCREEN 4

/*
SCREEN 3: Hercules adapter required, monochrome monitor only
  ¦ 720 x 348 graphics
  ¦ 80 x 25 text format, 9 x 14 character box
  ¦ 2 screen pages (1 only if a second display adapter is installed)
  ¦ PALETTE statement not supported
*/
//Note: QB64 will not support SCREEN 3

/*
SCREEN 2: 640 x 200 graphics
  ¦ 80 x 25 text format with character box size of 8 x 8
  ¦ 16 colors assigned to 2 attributes with EGA or VGA
*/
if (mode==2){
i=imgnew(640,200,2);
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//2

/*
SCREEN 1: 320 x 200 graphics
  ¦ 40 x 25 text format, 8 x 8 character box
  ¦ 16 background colors and one of two sets of 3 foreground colors assigned
    using COLOR statement with CGA
  ¦ 16 colors assigned to 4 attributes with EGA or VGA
*/
if (mode==1){
i=imgnew(320,200,1);
page[0]=i; img[i].flags|=IMG_SCREEN; display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//1

/*
                MDPA, CGA, EGA, or VGA Adapter Boards
SCREEN 0: Text mode only
  ¦ Either 40 x 25, 40 x 43, 40 x 50, 80 x 25, 80 x 43, or 80 x 50 text format
    with 8 x 8 character box size (8 x 14, 9 x 14, or 9 x 16 with EGA or VGA)
  ¦ 16 colors assigned to 2 attributes
  ¦ 16 colors assigned to any of 16 attributes (with CGA or EGA)
  ¦ 64 colors assigned to any of 16 attributes (with EGA or VGA)
*/
/*
granularity from &HB800
4096 in 80x25
2048 in 40x25
6880 in 80x43 (80x43x2=6880)
3440 in 40x43 (40x43x2=3440)
8000 in 80x50 (80x50x2=8000)
4000 in 40x50 (40x50x2=4000)
*/
if (mode==0){

if ((sub_screen_width_in_characters!=-1)&&(sub_screen_height_in_characters!=-1)&&(sub_screen_font!=-1)){
x=sub_screen_width_in_characters; y=sub_screen_height_in_characters; f=sub_screen_font;
sub_screen_width_in_characters=-1; sub_screen_height_in_characters=-1; sub_screen_font=-1;
goto gotwidth;
}
if (sub_screen_width_in_characters!=-1){
x=sub_screen_width_in_characters; sub_screen_width_in_characters=-1;
y=25; f=16;//default
if (prev_height_in_characters==43){y=43; f=14;}
if (prev_height_in_characters==50){y=50; f=8;}
if (x==40) f++;
goto gotwidth;
}
if (sub_screen_height_in_characters!=-1){
y=sub_screen_height_in_characters; sub_screen_height_in_characters=-1;
f=16;//default
if (y==43) f=14;
if (y==50) f=8;
x=80;//default
if (prev_width_in_characters==40){f++; x=40;}
goto gotwidth;
}

if ((prev_width_in_characters==80)&&(prev_height_in_characters==50)){
x=80; y=50; f=8; goto gotwidth;
}
if ((prev_width_in_characters==40)&&(prev_height_in_characters==50)){
x=40; y=50; f=8+1; goto gotwidth;
}
if ((prev_width_in_characters==80)&&(prev_height_in_characters==43)){
x=80; y=43; f=8; goto gotwidth;
}
if ((prev_width_in_characters==40)&&(prev_height_in_characters==43)){
x=40; y=43; f=8+1; goto gotwidth;
}
if ((prev_width_in_characters==40)&&(prev_height_in_characters==25)){
x=40; y=25; f=16+1; goto gotwidth;
}
x=80; y=25; f=16;
gotwidth:;
i2=x*y*2;//default granularity
//specific granularities which cannot be calculated
if ((x==40)&&(y==25)&&(f=(16+1))) i2=2048;
if ((x==80)&&(y==25)&&(f=16)) i2=4096;
p=65536/i2;//number of pages to allocate in cmem
if (p>8) p=8;//limit cmem pages to 8
 //make sure 8 page indexes exist
 if (7>=pages){
 i=7+1;
 page=(int32*)realloc(page,i*4);
 memset(page+pages,0,(i-pages)*4);
 pages=i;
 }
for (i3=0;i3<8;i3++){
if (i3<p){
i=imgframe(&cmem[753664+i2*i3],x,y,0);
}else{
i=imgnew(x,y,0);
}
selectfont(f,&img[i]);
img[i].flags|=IMG_SCREEN;
page[i3]=i;
}
//text-clear 64K after seg. &HB800
for (i=0;i<65536;i+=2){cmem[753664+i]=32; cmem[753664+i+1]=7;}//init. 64K of memory after B800
i=page[0];
display_page_index=i; display_page=&img[i]; write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
}//0

write_page->draw_ta=0.0; write_page->draw_scale=1.0; //reset DRAW attributes (of write_page)

last_active_page=0;

key_display_redraw=1; key_update();

}//setmode
}//passed MODE

//note: changing the active or visual page reselects the default colors!
if (passed&4){//SCREEN ?,?,X,? (active_page)
i=active_page; validatepage(i); i=page[i];
if ((write_page_index!=i)||(read_page_index!=i)){
write_page_index=i; write_page=&img[i]; read_page_index=i; read_page=&img[i];
defaultcolors();
//reset VIEW PRINT state
write_page->top_row=1;
if (!write_page->text) write_page->bottom_row=(write_page->height/write_page->font); else write_page->bottom_row=write_page->height;
if (write_page->cursor_y>write_page->bottom_row) write_page->cursor_y=1;
write_page->bottom_row--; if (write_page->bottom_row<=0) write_page->bottom_row=1;

//active page migration
//note: transfers any screen settings which are maintained during a QBASIC active page switch
if (last_active_page!=active_page){
static img_struct *old_page;
old_page=&img[page[last_active_page]];
//WINDOW settings
/*
SCREEN 7
WINDOW (0, 0)-(1, 1)
SCREEN 7, , 1, 1
PSET (.5, .5), 14
*/
//VIEW settings
/*
SCREEN 7
VIEW SCREEN (50, 50)-(100, 100)
SCREEN 7, , 1, 1
LINE (0, 0)-(1000, 1000), 1, BF
*/
//GRAPHICS CURSOR LOCATION
//(proven)
//NOT MAINTAINED:
// X color settings (for both text and graphics)
// X text cursor location
// X draw color (reset, as in QBASIC, by defaultcolors())
if (!write_page->text){
memcpy(&write_page->apm_p1,&old_page->apm_p1,(uint32)(&write_page->apm_p2-&write_page->apm_p1));
}
last_active_page=active_page;
}//active page migration

}

}//passed&4

if (passed&8){//SCREEN ?,?,?,X (visual_page)
i=visual_page; validatepage(i); i=page[i];
if (display_page_index!=i){
display_page_index=i; display_page=&img[i];
defaultcolors();
//reset VIEW PRINT state
write_page->top_row=1;
if (!write_page->text) write_page->bottom_row=(write_page->height/write_page->font); else write_page->bottom_row=write_page->height;
if (write_page->cursor_y>write_page->bottom_row) write_page->cursor_y=1;
write_page->bottom_row--; if (write_page->bottom_row<=0) write_page->bottom_row=1;

}
}//passed&8

if (autodisplay){
if (lock_display_required) lock_display=0;//release lock
}

return;
error:
error(5);
return;
}//screen (end)

void sub_pcopy(int32 src,int32 dst){
if (new_error) return;
static img_struct *s,*d;
//validate
if (src>=0){
validatepage(src); s=&img[page[src]];
}else{
src=-src;
if (src>=nextimg) goto error;
s=&img[src];
if (!s->valid) goto error;
}
if (dst>=0){
validatepage(dst); d=&img[page[dst]];
}else{
dst=-dst;
if (dst>=nextimg) goto error;
d=&img[dst];
if (!d->valid) goto error;
}
if (s==d) return;
if (s->bytes_per_pixel!=d->bytes_per_pixel) goto error;
if ((s->height!=d->height)||(s->width!=d->width)) goto error;
if (s->bytes_per_pixel==1){
if (d->mask<s->mask) goto error;//cannot copy onto a palette image with less colors
}
memcpy(d->offset,s->offset,d->width*d->height*d->bytes_per_pixel);
return;
error:

error(5);
return;
}

void qbsub_width(int32 option,int32 value1,int32 value2,int32 passed){
//[{#|LPRINT}][?],[?]
static int32 i,i2;

if (new_error) return;

if (option==0){//WIDTH [?][,?]

width8050switch=0;

static uint32 col,col2;

//used to restore scaling after simple font changes
//QBASIC/4.5/7.1: PMAP uses old scaling values after WIDTH change
static float window_x1,window_y1,window_x2,window_y2;

//Specifics:
//MODE 0: Changes the resolution based on the desired width
//        Horizontal width of 1 to 40 uses a double width font
//        Heights from 1 to 42 use font height 16 pixels
//        Heights from 43 to 49 use font height 14 pixels
//        Heights from 50 to ? use font height 8 pixels
//MODES 1-13: The resolution IS NOT CHANGED
//            The font is changed to a font usually available for that screen
//            mode, if available, that fits the given dimensions EXACTLY
//            If not possible, it may jump back to SCREEN 0 in some instances
//            just as it did in QBASIC
//256/32 BIT MODES: The font is unchanged
//                  The resolution is changed using the currently selected font
//note:
//COLOR selection is kept, all other values are lost (if staying in same "mode")
static int32 f,f2,width,height;

if ((!(passed&1))&&(!(passed&2))) goto error;//cannot omit both arguments

width=value1; height=value2;

if ((write_page->compatible_mode==32)||(write_page->compatible_mode==256)){

if (!(passed&1)){//width ommited
width=write_page->width;
}else{
if (width<=0) goto error;
i=fontwidth[write_page->font]; if (!i) i=1;
width*=i;
}

if (!(passed&2)){//height ommited
height=write_page->height;
}else{
if (height<=0) goto error;
height*=fontheight[write_page->font];
}

//width & height are now the desired dimensions

if ((width==write_page->width)&&(height==write_page->height)) return;//no change required

if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}

if (autodisplay){
if (write_page->flags&IMG_SCREEN){
 if (lock_display_required){
 if (lock_display==0) lock_display=1;
 while (lock_display!=2){
 Sleep(0);
 }
 }
}
}

col=write_page->color; col2=write_page->background_color;
f=write_page->font;
//change resolution
write_page->width=width; write_page->height=height;
if (write_page->flags&IMG_FREEMEM){
free(write_page->offset);//free pixel data
write_page->offset=(uint8*)calloc(width*height*write_page->bytes_per_pixel,1);
}else{//frame?
memset(write_page->offset,0,width*height*write_page->bytes_per_pixel);
}
imgrevert(write_page_index);
write_page->color=col; write_page->background_color=col2;
selectfont(f,write_page);

if (autodisplay){
if (write_page->flags&IMG_SCREEN){
 if (lock_display_required) lock_display=0;//release lock
}
}

return;

}//32/256

if (!(passed&1)){//width ommited
if (height<=0) goto error;

if (!write_page->compatible_mode){//0
f=8;
if (height<=49) f=14;
if (height<=42) f=16;
width=write_page->width;
if (width<=40) f++;
if ((write_page->font==f)&&(write_page->height==height)) return;//no change
sub_screen_height_in_characters=height; sub_screen_width_in_characters=width;
sub_screen_font=f;
qbg_screen(0,0,0,0,0,1);
return;
}//0

if (((write_page->compatible_mode>=1)&&(write_page->compatible_mode<=8))||(write_page->compatible_mode==13)){
if (write_page->height==height*8){//correct resolution
if (write_page->font==8) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
imgrevert(write_page_index);
write_page->color=col; write_page->background_color=col2;
selectfont(8,write_page);
return;
}//correct resolution
//fall through
}//modes 1-8

/*
SCREEN 9: 640 x 350 graphics
  ¦ 80 x 25 or 80 x 43 text format, 8 x 14 or 8 x 8 character box size
  ¦ 64K page size, page range is 0 (64K);
    128K page size, page range is 0 (128K) or 0-1 (256K)
  ¦ 16 colors assigned to 4 attributes (64K adapter memory), or
    64 colors assigned to 16 attributes (more than 64K adapter memory)
SCREEN 10: 640 x 350 graphics, monochrome monitor only
  ¦ 80 x 25 or 80 x 43 text format, 8 x 14 or 8 x 8 character box size
  ¦ 128K page size, page range is 0 (128K) or 0-1 (256K)
  ¦ Up to 9 pseudocolors assigned to 4 attributes
*/
if ((write_page->compatible_mode>=9)&&(write_page->compatible_mode<=10)){
f=0;
if(write_page->height==height*8) f=8;
if(write_page->height==height*14) f=14;
if((height==43)&&(write_page->height==350)) f=8;//?x350,8x8
if(f){//correct resolution
if (write_page->font==f) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
window_x1=write_page->window_x1; window_x2=write_page->window_x2; window_y1=write_page->window_y1; window_y2=write_page->window_y2;
imgrevert(write_page_index);
qbg_sub_window(window_x1,window_y1,window_x2,window_y2,1+2); write_page->clipping_or_scaling=0;
write_page->color=col; write_page->background_color=col2;
selectfont(f,write_page);
return;
}//correct resolution
//fall through
}//modes 9,10

if ((write_page->compatible_mode>=11)&&(write_page->compatible_mode<=12)){
f=0;
if(write_page->height==height*8) f=8;
if(write_page->height==height*16) f=16;
if(f){//correct resolution
if (write_page->font==f) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
window_x1=write_page->window_x1; window_x2=write_page->window_x2; window_y1=write_page->window_y1; window_y2=write_page->window_y2;
imgrevert(write_page_index);
qbg_sub_window(window_x1,window_y1,window_x2,window_y2,1+2); write_page->clipping_or_scaling=0;
write_page->color=col; write_page->background_color=col2;
selectfont(f,write_page);
return;
}//correct resolution
//fall through
}//modes 11,12

//fall through:
if ((height==25)||(height==50)||(height==43)){
sub_screen_height_in_characters=height; qbg_screen(0,0,0,0,0,1);
return;
}

goto error;

}//width omitted

if (!(passed&2)){//height omitted

if (width<=0) goto error;

if (!write_page->compatible_mode){//0
height=write_page->height;
f=8;
if (height<=49) f=14;
if (height<=42) f=16;
if (width<=40) f++;
if ((write_page->font==f)&&(write_page->width==width)) return;//no change
sub_screen_height_in_characters=height; sub_screen_width_in_characters=width;
sub_screen_font=f;
qbg_screen(0,0,0,0,0,1);
return;
}//0

if (((write_page->compatible_mode>=1)&&(write_page->compatible_mode<=8))||(write_page->compatible_mode==13)){
if (write_page->width==width*8){//correct resolution
if (write_page->font==8) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
imgrevert(write_page_index);
write_page->color=col; write_page->background_color=col2;
selectfont(8,write_page);
return;
}//correct resolution
//fall through
}//modes 1-8

/*
SCREEN 9: 640 x 350 graphics
  ¦ 80 x 25 or 80 x 43 text format, 8 x 14 or 8 x 8 character box size
  ¦ 64K page size, page range is 0 (64K);
    128K page size, page range is 0 (128K) or 0-1 (256K)
  ¦ 16 colors assigned to 4 attributes (64K adapter memory), or
    64 colors assigned to 16 attributes (more than 64K adapter memory)
SCREEN 10: 640 x 350 graphics, monochrome monitor only
  ¦ 80 x 25 or 80 x 43 text format, 8 x 14 or 8 x 8 character box size
  ¦ 128K page size, page range is 0 (128K) or 0-1 (256K)
  ¦ Up to 9 pseudocolors assigned to 4 attributes
*/
if ((write_page->compatible_mode>=9)&&(write_page->compatible_mode<=10)){
f=0;
if (write_page->width==width*8) f=8;
if (f){//correct resolution
f2=fontheight[write_page->font]; if (f2>8) f=14;
if (write_page->font==f) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
window_x1=write_page->window_x1; window_x2=write_page->window_x2; window_y1=write_page->window_y1; window_y2=write_page->window_y2;
imgrevert(write_page_index);
qbg_sub_window(window_x1,window_y1,window_x2,window_y2,1+2); write_page->clipping_or_scaling=0;
write_page->color=col; write_page->background_color=col2;
selectfont(f,write_page);
return;
}//correct resolution
//fall through
}//modes 9,10

if ((write_page->compatible_mode>=11)&&(write_page->compatible_mode<=12)){
f=0;
if (write_page->width==width*8) f=8;
if (f){//correct resolution
f2=fontheight[write_page->font]; if (f2>8) f=16;
if (write_page->font==f) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
window_x1=write_page->window_x1; window_x2=write_page->window_x2; window_y1=write_page->window_y1; window_y2=write_page->window_y2;
imgrevert(write_page_index);
qbg_sub_window(window_x1,window_y1,window_x2,window_y2,1+2); write_page->clipping_or_scaling=0;
write_page->color=col; write_page->background_color=col2;
selectfont(f,write_page);
return;
}//correct resolution
//fall through
}//modes 11,12

//fall through:
if ((width==40)||(width==80)){
sub_screen_width_in_characters=width;
qbg_screen(0,0,0,0,0,1);
return;
}

goto error;

}//height omitted

//both height & width passed

if ((width<=0)||(height<=0)) goto error;

if (!write_page->compatible_mode){//0
f=8;
if (height<=49) f=14;
if (height<=42) f=16;
if (width<=40) f++;
if ((write_page->font==f)&&(write_page->width==width)&&(write_page->height==height)) return;//no change
sub_screen_height_in_characters=height; sub_screen_width_in_characters=width;
sub_screen_font=f;
qbg_screen(0,0,0,0,0,1);
return;
}//0

if (((write_page->compatible_mode>=1)&&(write_page->compatible_mode<=8))||(write_page->compatible_mode==13)){
if ((write_page->width==width*8)&&(write_page->height==height*8)){//correct resolution
if (write_page->font==8) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
imgrevert(write_page_index);
write_page->color=col; write_page->background_color=col2;
selectfont(8,write_page);
return;
}//correct resolution
//fall through
}//modes 1-8

/*
SCREEN 9: 640 x 350 graphics
  ¦ 80 x 25 or 80 x 43 text format, 8 x 14 or 8 x 8 character box size
  ¦ 64K page size, page range is 0 (64K);
    128K page size, page range is 0 (128K) or 0-1 (256K)
  ¦ 16 colors assigned to 4 attributes (64K adapter memory), or
    64 colors assigned to 16 attributes (more than 64K adapter memory)
SCREEN 10: 640 x 350 graphics, monochrome monitor only
  ¦ 80 x 25 or 80 x 43 text format, 8 x 14 or 8 x 8 character box size
  ¦ 128K page size, page range is 0 (128K) or 0-1 (256K)
  ¦ Up to 9 pseudocolors assigned to 4 attributes
*/
if ((write_page->compatible_mode>=9)&&(write_page->compatible_mode<=10)){
f=0;
if (write_page->width==width*8){
if (write_page->height==height*8) f=8;
if (write_page->height==height*14) f=14;
if ((height==43)&&(write_page->height==350)) f=8;//?x350,8x8
}
if (f){//correct resolution
if (write_page->font==f) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
window_x1=write_page->window_x1; window_x2=write_page->window_x2; window_y1=write_page->window_y1; window_y2=write_page->window_y2;
imgrevert(write_page_index);
qbg_sub_window(window_x1,window_y1,window_x2,window_y2,1+2); write_page->clipping_or_scaling=0;
write_page->color=col; write_page->background_color=col2;
selectfont(f,write_page);
return;
}//correct resolution
//fall through
}//modes 9,10

if ((write_page->compatible_mode>=11)&&(write_page->compatible_mode<=12)){
f=0;
if (write_page->width==width*8){

if (write_page->height==height*8) f=8;

if (write_page->height==height*16) f=16;
}
if (f){//correct resolution
if (write_page->font==f) return;//correct font, no change required
if (write_page->flags&IMG_SCREEN){
//delete pages 1-?
for(i=1;i<pages;i++){
if(i2=page[i]){
if (display_page_index==i2){display_page_index=page[0]; display_page=&img[display_page_index];}
if (read_page_index==i2){read_page_index=display_page_index; read_page=display_page;}
if (write_page_index==i2){write_page_index=display_page_index; write_page=display_page;}
//manual delete, freeing video pages is usually illegal
if (img[i2].flags&IMG_FREEMEM) free(img[i2].offset);//free pixel data
freeimg(i2);
}
}//i
}
col=write_page->color; col2=write_page->background_color;
window_x1=write_page->window_x1; window_x2=write_page->window_x2; window_y1=write_page->window_y1; window_y2=write_page->window_y2;
imgrevert(write_page_index);
qbg_sub_window(window_x1,window_y1,window_x2,window_y2,1+2); write_page->clipping_or_scaling=0;
write_page->color=col; write_page->background_color=col2;
selectfont(f,write_page);
return;
}//correct resolution
//fall through
}//modes 11,12

//fall through:
if ((width==40)||(width==80)){
if ((height==25)||(height==50)||(height==43)){
sub_screen_width_in_characters=width; sub_screen_height_in_characters=height;
f=16;
if (height==43) f=14;
if (height==50) f=8;
if (width==40) f++;
sub_screen_font=f;
qbg_screen(0,0,0,0,0,1);
return;
}

goto error;

}//WIDTH [?][,?]

}//option==0

if (option==2){//LPRINT
if (passed!=1) goto error;
if ((value1<1)||(value1>255)) goto error;
width_lprint=value1;
return;
}//option==2

//file/device?
//...

error:
error(5);
return;
}

void pset_and_clip(int32 x,int32 y,uint32 col){

if ((x>=write_page->view_x1)&&(x<=write_page->view_x2)&&(y>=write_page->view_y1)&&(y<=write_page->view_y2)){

static uint8 *cp;
static uint32 *o32;
static uint32 destcol;
if (write_page->bytes_per_pixel==1){
 write_page->offset[y*write_page->width+x]=col&write_page->mask;
 return;
}else{

 if (write_page->alpha_disabled){
 write_page->offset32[y*write_page->width+x]=col;
 return;
 }
switch(col&0xFF000000){
case 0xFF000000://100% alpha, so regular pset (fast)
 write_page->offset32[y*write_page->width+x]=col;
 return;
break;
case 0x0://0%(0) alpha, so no pset (very fast)
 return;
break;
case 0x80000000://~50% alpha (optomized)
 o32=write_page->offset32+(y*write_page->width+x);
 *o32=(((*o32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend128[*o32>>24]<<24);
 return;
break; 
case 0x7F000000://~50% alpha (optomized)
 o32=write_page->offset32+(y*write_page->width+x);
 *o32=(((*o32&0xFEFEFE)+(col&0xFEFEFE))>>1)+(ablend127[*o32>>24]<<24);
 return;
break;
default://other alpha values (uses a lookup table)
 o32=write_page->offset32+(y*write_page->width+x);
 destcol=*o32;
 cp=cblend+(col>>24<<16);
 *o32=
   cp[(col<<8&0xFF00)+(destcol&255)    ]
 +(cp[(col&0xFF00)   +(destcol>>8&255) ]<<8)
 +(cp[(col>>8&0xFF00)+(destcol>>16&255)]<<16)
 +(ablend[(col>>24)+(destcol>>16&0xFF00)]<<24);
};
}

}//within viewport
return;
}

void qb32_boxfill(float x1f,float y1f,float x2f,float y2f,uint32 col){
static int32 x1,y1,x2,y2,i,width,img_width,x,y,d_width,a,a2,v1,v2,v3;
static uint8 *p,*cp,*cp2,*cp3;
static uint32 *lp,*lp_last,*lp_first;
static uint32 *doff32,destcol;

//resolve coordinates
if (write_page->clipping_or_scaling){
if (write_page->clipping_or_scaling==2){
x1=qbr_float_to_long(x1f*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
y1=qbr_float_to_long(y1f*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
x2=qbr_float_to_long(x2f*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
y2=qbr_float_to_long(y2f*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
}else{
x1=qbr_float_to_long(x1f)+write_page->view_offset_x; y1=qbr_float_to_long(y1f)+write_page->view_offset_y;
x2=qbr_float_to_long(x2f)+write_page->view_offset_x; y2=qbr_float_to_long(y2f)+write_page->view_offset_y;
}
}else{
x1=qbr_float_to_long(x1f); y1=qbr_float_to_long(y1f);
x2=qbr_float_to_long(x2f); y2=qbr_float_to_long(y2f);
}

//swap coordinates (if necessary)
if (x1>x2){i=x1; x1=x2; x2=i;}
if (y1>y2){i=y1; y1=y2; y2=i;}

//exit without rendering if necessary
if (x2<write_page->view_x1) return;
if (x1>write_page->view_x2) return;
if (y2<write_page->view_y1) return;
if (y1>write_page->view_y2) return;

//crop coordinates
if (x1<write_page->view_x1) x1=write_page->view_x1;
if (y1<write_page->view_y1) y1=write_page->view_y1;
if (x1>write_page->view_x2) x1=write_page->view_x2;
if (y1>write_page->view_y2) y1=write_page->view_y2;
if (x2<write_page->view_x1) x2=write_page->view_x1;
if (y2<write_page->view_y1) y2=write_page->view_y1;
if (x2>write_page->view_x2) x2=write_page->view_x2;
if (y2>write_page->view_y2) y2=write_page->view_y2;

if (write_page->bytes_per_pixel==1){
col&=write_page->mask;
width=x2-x1+1;
img_width=write_page->width;
p=write_page->offset+y1*write_page->width+x1;
i=y2-y1+1;
loop:
memset(p,col,width);
p+=img_width;
if (--i) goto loop;
return;
}//1

//assume 32-bit
//optomized
//alpha disabled or full alpha?
a=col>>24;
if ((write_page->alpha_disabled)||(a==255)){
width=x2-x1+1;
y=y2-y1+1;
img_width=write_page->width;
//build first line pixel by pixel
lp_first=write_page->offset32+y1*img_width+x1;
lp=lp_first-1; lp_last=lp+width;
while (lp++<lp_last) *lp=col;
//copy remaining lines
lp=lp_first;
width*=4;
while(y--){
memcpy(lp,lp_first,width);
lp+=img_width;
}
return;
}
//no alpha?
if (!a) return;
//half alpha?
img_width=write_page->width;
doff32=write_page->offset32+y1*img_width+x1;
width=x2-x1+1;
d_width=img_width-width;
if (a==128){
col&=0xFEFEFE;
y=y2-y1+1;
while(y--){
x=width;
while(x--){
*doff32++=(((*doff32&0xFEFEFE)+col)>>1)+(ablend128[*doff32>>24]<<24);
}
doff32+=d_width;
}
return;
}
if (a==127){
col&=0xFEFEFE;
y=y2-y1+1;
while(y--){
x=width;
while(x--){
*doff32++=(((*doff32&0xFEFEFE)+col)>>1)+(ablend127[*doff32>>24]<<24);
}
doff32+=d_width;
}
return;
}
//ranged alpha
cp=cblend+(a<<16);
a2=a<<8;
cp3=cp+(col>>8&0xFF00);
cp2=cp+(col&0xFF00);
cp+=(col<<8&0xFF00);
y=y2-y1+1;
while(y--){
x=width;
while(x--){
 destcol=*doff32;
 *doff32++=
   cp[destcol&255]
 +(cp2[destcol>>8&255]<<8)
 +(cp3[destcol>>16&255]<<16)
 +(ablend[(destcol>>24)+a2]<<24);
}
doff32+=d_width;
}
return;
}


void fast_boxfill(int32 x1,int32 y1,int32 x2,int32 y2,uint32 col){
//assumes:
//actual coordinates passed
//left->right, top->bottom order
//on-screen
static int32 i,width,img_width,x,y,d_width,a,a2,v1,v2,v3;
static uint8 *p,*cp,*cp2,*cp3;
static uint32 *lp,*lp_last,*lp_first;
static uint32 *doff32,destcol;

if (write_page->bytes_per_pixel==1){
col&=write_page->mask;
width=x2-x1+1;
img_width=write_page->width;
p=write_page->offset+y1*write_page->width+x1;
i=y2-y1+1;
loop:
memset(p,col,width);
p+=img_width;
if (--i) goto loop;
return;
}//1

//assume 32-bit
//optomized
//alpha disabled or full alpha?
a=col>>24;
if ((write_page->alpha_disabled)||(a==255)){

width=x2-x1+1;
y=y2-y1+1;
img_width=write_page->width;
//build first line pixel by pixel
lp_first=write_page->offset32+y1*img_width+x1;
lp=lp_first-1; lp_last=lp+width;
while (lp++<lp_last) *lp=col;
//copy remaining lines
lp=lp_first;
width*=4;
while(y--){
memcpy(lp,lp_first,width);
lp+=img_width;
}
return;
}
//no alpha?
if (!a) return;
//half alpha?
img_width=write_page->width;
doff32=write_page->offset32+y1*img_width+x1;
width=x2-x1+1;
d_width=img_width-width;
if (a==128){
col&=0xFEFEFE;
y=y2-y1+1;
while(y--){
x=width;
while(x--){
*doff32++=(((*doff32&0xFEFEFE)+col)>>1)+(ablend128[*doff32>>24]<<24);
}
doff32+=d_width;
}
return;
}
if (a==127){
col&=0xFEFEFE;
y=y2-y1+1;
while(y--){
x=width;
while(x--){
*doff32++=(((*doff32&0xFEFEFE)+col)>>1)+(ablend127[*doff32>>24]<<24);
}
doff32+=d_width;
}
return;
}
//ranged alpha
cp=cblend+(a<<16);
a2=a<<8;
cp3=cp+(col>>8&0xFF00);
cp2=cp+(col&0xFF00);
cp+=(col<<8&0xFF00);
y=y2-y1+1;
while(y--){
x=width;
while(x--){
 destcol=*doff32;
 *doff32++=
   cp[destcol&255]
 +(cp2[destcol>>8&255]<<8)
 +(cp3[destcol>>16&255]<<16)
 +(ablend[(destcol>>24)+a2]<<24);
}
doff32+=d_width;
}
return;
}






//copied from qb32_line with the following modifications
//i. pre-WINDOW'd & VIEWPORT'd int32 co-ordinates
//ii. all references to style & lineclip_skippixels commented
//iii. declaration of x1,y1,x2,y2,x1f,y1f changed, some declarations removed
void fast_line(int32 x1,int32 y1,int32 x2,int32 y2,uint32 col){
static int32 l,l2,mi;
static float m,x1f,y1f;

lineclip(x1,y1,x2,y2,write_page->view_x1,write_page->view_y1,write_page->view_x2,write_page->view_y2);

//style=(style&65535)+(style<<16);
//lineclip_skippixels&=15;
//style=_lrotl(style,lineclip_skippixels);

if (lineclip_draw){
l=abs(lineclip_x1-lineclip_x2);
l2=abs(lineclip_y1-lineclip_y2);
if (l>l2){

//x-axis distance is larger
y1f=lineclip_y1;
if (l){//following only applies if drawing more than one pixel
m=((float)lineclip_y2-(float)lineclip_y1)/(float)l;
if (lineclip_x2>=lineclip_x1) mi=1; else mi=-1;//direction of change
}
l++;
while (l--){
if (y1f<0) lineclip_y1=y1f-0.5f; else lineclip_y1=y1f+0.5f;

//if ((style=_lrotl(style,1))&1){
pset(lineclip_x1,lineclip_y1,col);
//}

lineclip_x1+=mi;
y1f+=m;
}

}else{

//y-axis distance is larger
x1f=lineclip_x1;
if (l2){//following only applies if drawing more than one pixel
m=((float)lineclip_x2-(float)lineclip_x1)/(float)l2;
if (lineclip_y2>=lineclip_y1) mi=1; else mi=-1;//direction of change
}
l2++;
while (l2--){
if (x1f<0) lineclip_x1=x1f-0.5f; else lineclip_x1=x1f+0.5f;
//if ((style=_lrotl(style,1))&1){
pset(lineclip_x1,lineclip_y1,col);
//}
lineclip_y1+=mi;
x1f+=m;
}

}

}//lineclip_draw
return;
}























void qb32_line(float x1f,float y1f,float x2f,float y2f,uint32 col,uint32 style){
static int32 x1,y1,x2,y2,l,l2,mi;
static float m;

//resolve coordinates
if (write_page->clipping_or_scaling){
if (write_page->clipping_or_scaling==2){
x1=qbr_float_to_long(x1f*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
y1=qbr_float_to_long(y1f*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
x2=qbr_float_to_long(x2f*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
y2=qbr_float_to_long(y2f*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
}else{
x1=qbr_float_to_long(x1f)+write_page->view_offset_x; y1=qbr_float_to_long(y1f)+write_page->view_offset_y;
x2=qbr_float_to_long(x2f)+write_page->view_offset_x; y2=qbr_float_to_long(y2f)+write_page->view_offset_y;
}
}else{
x1=qbr_float_to_long(x1f); y1=qbr_float_to_long(y1f);
x2=qbr_float_to_long(x2f); y2=qbr_float_to_long(y2f);
}

lineclip(x1,y1,x2,y2,write_page->view_x1,write_page->view_y1,write_page->view_x2,write_page->view_y2);

style=(style&65535)+(style<<16);
lineclip_skippixels&=15;
style=_lrotl(style,lineclip_skippixels);

if (lineclip_draw){
l=abs(lineclip_x1-lineclip_x2);
l2=abs(lineclip_y1-lineclip_y2);
if (l>l2){

//x-axis distance is larger
y1f=lineclip_y1;
if (l){//following only applies if drawing more than one pixel
m=((float)lineclip_y2-(float)lineclip_y1)/(float)l;
if (lineclip_x2>=lineclip_x1) mi=1; else mi=-1;//direction of change
}
l++;
while (l--){
if (y1f<0) lineclip_y1=y1f-0.5f; else lineclip_y1=y1f+0.5f;

if ((style=_lrotl(style,1))&1){
pset(lineclip_x1,lineclip_y1,col);
}

lineclip_x1+=mi;
y1f+=m;
}

}else{

//y-axis distance is larger
x1f=lineclip_x1;
if (l2){//following only applies if drawing more than one pixel
m=((float)lineclip_x2-(float)lineclip_x1)/(float)l2;
if (lineclip_y2>=lineclip_y1) mi=1; else mi=-1;//direction of change
}
l2++;
while (l2--){
if (x1f<0) lineclip_x1=x1f-0.5f; else lineclip_x1=x1f+0.5f;
if ((style=_lrotl(style,1))&1){
pset(lineclip_x1,lineclip_y1,col);
}
lineclip_y1+=mi;
x1f+=m;
}

}

}//lineclip_draw
return;
}



void sub_line(float x1,float y1,float x2,float y2,uint32 col,int32 bf,uint32 style,int32 passed){
if (new_error) return;
if (write_page->text){error(5); return;}
/*
'"[[{STEP}](?,?)]-[{STEP}](?,?)[,[?][,[{B|BF}][,?]]]"
LINE -(10, 10) 'flags: 0
LINE (0, 0)-(10, 10) 'flags: 1
LINE -STEP(10, 10) 'flags: 2
LINE STEP(0, 0)-(10, 10) 'flags: 1+4
*/

//adjust coordinates and qb graphics cursor position based on STEP
if (passed&1){
if (passed&4){x1=write_page->x+x1; y1=write_page->y+y1;}
write_page->x=x1; write_page->y=y1;
}else{
x1=write_page->x; y1=write_page->y;
}
if (passed&2){x2=write_page->x+x2; y2=write_page->y+y2;}
write_page->x=x2; write_page->y=y2;

if (bf==0){//line
if ((passed&16)==0) style=0xFFFF;
if ((passed&8)==0) col=write_page->color;
write_page->draw_color=col;
qb32_line(x1,y1,x2,y2,col,style);
return;
}

if (bf==1){//rectangle
if ((passed&16)==0) style=0xFFFF;
if ((passed&8)==0) col=write_page->color;
write_page->draw_color=col;
qb32_line(x1,y1,x2,y1,col,style);
qb32_line(x2,y1,x2,y2,col,style);
qb32_line(x2,y2,x1,y2,col,style);
qb32_line(x1,y2,x1,y1,col,style);
return;
}

if (bf==2){//filled box
if ((passed&8)==0) col=write_page->color;
write_page->draw_color=col;
qb32_boxfill(x1,y1,x2,y2,col);
return;
}

}//sub_line

































//3 paint routines exist for color (not textured) filling
//i) 8-bit
//ii) 32-bit no-alpha
//iii) 32-bit
//simple comparisons are used, the alpha value is part of that comparison in all cases
//even if blending is disabled (a fixed color is likely to have a fixed alpha value anyway),
//and this allows for filling alpha regions

//32-bit WITH BENDING
void sub_paint32(float x,float y,uint32 fillcol,uint32 bordercol,int32 passed){

//uses 2 buffers, a and b, and swaps between them for reading and creating
static uint32 a_n=0;
static uint16 *a_x=(uint16*)malloc(2*65536),*a_y=(uint16*)malloc(2*65536);
static uint8 *a_t=(uint8*)malloc(65536);
static uint32 b_n=0;
static uint16 *b_x=(uint16*)malloc(2*65536),*b_y=(uint16*)malloc(2*65536);
static uint8 *b_t=(uint8*)malloc(65536);
static uint8 *done=(uint8*)calloc(640*480,1);
static int32 ix,iy,i,t,x2,y2;
static uint32 offset;
static uint8 *cp;
static uint16 *sp;
//overrides
static int32 done_size=640*480;
static uint32 *qbg_active_page_offset;//override
static int32 qbg_width,qbg_view_x1,qbg_view_y1,qbg_view_x2,qbg_view_y2;//override
static uint32 *doff32,destcol;

if ((passed&2)==0) fillcol=write_page->color;
if ((passed&4)==0) bordercol=fillcol;
write_page->draw_color=fillcol;

if (passed&1){write_page->x+=x; write_page->y+=y;}else{write_page->x=x; write_page->y=y;}

if (write_page->clipping_or_scaling){
if (write_page->clipping_or_scaling==2){
ix=qbr_float_to_long(write_page->x*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
iy=qbr_float_to_long(write_page->y*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
}else{
ix=qbr_float_to_long(write_page->x)+write_page->view_offset_x; iy=qbr_float_to_long(write_page->y)+write_page->view_offset_y;
}
}else{
ix=qbr_float_to_long(write_page->x); iy=qbr_float_to_long(write_page->y);
}

//return if offscreen
if ((ix<write_page->view_x1)||(iy<write_page->view_y1)||(ix>write_page->view_x2)||(iy>write_page->view_y2)){
return;
}

//overrides
qbg_active_page_offset=write_page->offset32;
qbg_width=write_page->width;
qbg_view_x1=write_page->view_x1;
qbg_view_y1=write_page->view_y1;
qbg_view_x2=write_page->view_x2;
qbg_view_y2=write_page->view_y2;
i=write_page->width*write_page->height;
if (i>done_size){
free(done);
done=(uint8*)calloc(i,1);
}

//return if first point is the bordercolor
if (qbg_active_page_offset[iy*qbg_width+ix]==bordercol) return;

//create first node
a_x[0]=ix; a_y[0]=iy;
a_t[0]=15;
//types:
//&1=check left
//&2=check right
//&4=check above
//&8=check below


a_n=1;
//qbg_active_page_offset[iy*qbg_width+ix]=fillcol;
offset=iy*qbg_width+ix;
 //--------plot pixel--------
 doff32=qbg_active_page_offset+offset;
 switch(fillcol&0xFF000000){
 case 0xFF000000:
  *doff32=fillcol;
 break;
 case 0x0:
  doff32;
 break;
 case 0x80000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend128[*doff32>>24]<<24);
 break; 
 case 0x7F000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend127[*doff32>>24]<<24);
 break;
 default:
  destcol=*doff32;
  cp=cblend+(fillcol>>24<<16);
  *doff32=
    cp[(fillcol<<8&0xFF00)+(destcol&255)    ]
  +(cp[(fillcol&0xFF00)   +(destcol>>8&255) ]<<8)
  +(cp[(fillcol>>8&0xFF00)+(destcol>>16&255)]<<16)
  +(ablend[(fillcol>>24)+(destcol>>16&0xFF00)]<<24);
 };//switch
 //--------done plot pixel--------
done[iy*qbg_width+ix]=1;

nextpass:
b_n=0;
for (i=0;i<a_n;i++){
t=a_t[i]; ix=a_x[i]; iy=a_y[i];

//left
if (t&1){
x2=ix-1; y2=iy;
if (x2>=qbg_view_x1){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
 //--------plot pixel--------
 doff32=qbg_active_page_offset+offset;
 switch(fillcol&0xFF000000){
 case 0xFF000000:
  *doff32=fillcol;
 break;
 case 0x0:
  doff32;
 break;
 case 0x80000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend128[*doff32>>24]<<24);
 break; 
 case 0x7F000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend127[*doff32>>24]<<24);
 break;
 default:
  destcol=*doff32;
  cp=cblend+(fillcol>>24<<16);
  *doff32=
    cp[(fillcol<<8&0xFF00)+(destcol&255)    ]
  +(cp[(fillcol&0xFF00)   +(destcol>>8&255) ]<<8)
  +(cp[(fillcol>>8&0xFF00)+(destcol>>16&255)]<<16)
  +(ablend[(fillcol>>24)+(destcol>>16&0xFF00)]<<24);
 };//switch
 //--------done plot pixel--------
b_t[b_n]=13; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//right
if (t&2){
x2=ix+1; y2=iy;
if (x2<=qbg_view_x2){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
 //--------plot pixel--------
 doff32=qbg_active_page_offset+offset;
 switch(fillcol&0xFF000000){
 case 0xFF000000:
  *doff32=fillcol;
 break;
 case 0x0:
  doff32;
 break;
 case 0x80000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend128[*doff32>>24]<<24);
 break; 
 case 0x7F000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend127[*doff32>>24]<<24);
 break;
 default:
  destcol=*doff32;
  cp=cblend+(fillcol>>24<<16);
  *doff32=
    cp[(fillcol<<8&0xFF00)+(destcol&255)    ]
  +(cp[(fillcol&0xFF00)   +(destcol>>8&255) ]<<8)
  +(cp[(fillcol>>8&0xFF00)+(destcol>>16&255)]<<16)
  +(ablend[(fillcol>>24)+(destcol>>16&0xFF00)]<<24);
 };//switch
 //--------done plot pixel--------
b_t[b_n]=14; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//above
if (t&4){
x2=ix; y2=iy-1;
if (y2>=qbg_view_y1){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
 //--------plot pixel--------
 doff32=qbg_active_page_offset+offset;
 switch(fillcol&0xFF000000){
 case 0xFF000000:
  *doff32=fillcol;
 break;
 case 0x0:
  doff32;
 break;
 case 0x80000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend128[*doff32>>24]<<24);
 break; 
 case 0x7F000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend127[*doff32>>24]<<24);
 break;
 default:
  destcol=*doff32;
  cp=cblend+(fillcol>>24<<16);
  *doff32=
    cp[(fillcol<<8&0xFF00)+(destcol&255)    ]
  +(cp[(fillcol&0xFF00)   +(destcol>>8&255) ]<<8)
  +(cp[(fillcol>>8&0xFF00)+(destcol>>16&255)]<<16)
  +(ablend[(fillcol>>24)+(destcol>>16&0xFF00)]<<24);
 };//switch
 //--------done plot pixel--------
b_t[b_n]=7; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//below
if (t&8){
x2=ix; y2=iy+1;
if (y2<=qbg_view_y2){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
 //--------plot pixel--------
 doff32=qbg_active_page_offset+offset;
 switch(fillcol&0xFF000000){
 case 0xFF000000:
  *doff32=fillcol;
 break;
 case 0x0:
  doff32;
 break;
 case 0x80000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend128[*doff32>>24]<<24);
 break; 
 case 0x7F000000:
  *doff32=(((*doff32&0xFEFEFE)+(fillcol&0xFEFEFE))>>1)+(ablend127[*doff32>>24]<<24);
 break;
 default:
  destcol=*doff32;
  cp=cblend+(fillcol>>24<<16);
  *doff32=
    cp[(fillcol<<8&0xFF00)+(destcol&255)    ]
  +(cp[(fillcol&0xFF00)   +(destcol>>8&255) ]<<8)
  +(cp[(fillcol>>8&0xFF00)+(destcol>>16&255)]<<16)
  +(ablend[(fillcol>>24)+(destcol>>16&0xFF00)]<<24);
 };//switch
 //--------done plot pixel--------
b_t[b_n]=11; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

}//i

//no new nodes?
if (b_n==0){
memset(done,0,write_page->width*write_page->height);//cleanup
return;//finished!
}

//swap a & b arrays
sp=a_x; a_x=b_x; b_x=sp;
sp=a_y; a_y=b_y; b_y=sp;
cp=a_t; a_t=b_t; b_t=cp;
a_n=b_n;

goto nextpass;
}


//32-bit NO ALPHA BENDING
void sub_paint32x(float x,float y,uint32 fillcol,uint32 bordercol,int32 passed){

//uses 2 buffers, a and b, and swaps between them for reading and creating
static uint32 a_n=0;
static uint16 *a_x=(uint16*)malloc(2*65536),*a_y=(uint16*)malloc(2*65536);
static uint8 *a_t=(uint8*)malloc(65536);
static uint32 b_n=0;
static uint16 *b_x=(uint16*)malloc(2*65536),*b_y=(uint16*)malloc(2*65536);
static uint8 *b_t=(uint8*)malloc(65536);
static uint8 *done=(uint8*)calloc(640*480,1);
static int32 ix,iy,i,t,x2,y2;
static uint32 offset;
static uint8 *cp;
static uint16 *sp;
//overrides
static int32 done_size=640*480;
static uint32 *qbg_active_page_offset;//override
static int32 qbg_width,qbg_view_x1,qbg_view_y1,qbg_view_x2,qbg_view_y2;//override

if ((passed&2)==0) fillcol=write_page->color;
if ((passed&4)==0) bordercol=fillcol;
write_page->draw_color=fillcol;

if (passed&1){write_page->x+=x; write_page->y+=y;}else{write_page->x=x; write_page->y=y;}

if (write_page->clipping_or_scaling){
if (write_page->clipping_or_scaling==2){
ix=qbr_float_to_long(write_page->x*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
iy=qbr_float_to_long(write_page->y*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
}else{
ix=qbr_float_to_long(write_page->x)+write_page->view_offset_x; iy=qbr_float_to_long(write_page->y)+write_page->view_offset_y;
}
}else{
ix=qbr_float_to_long(write_page->x); iy=qbr_float_to_long(write_page->y);
}

//return if offscreen
if ((ix<write_page->view_x1)||(iy<write_page->view_y1)||(ix>write_page->view_x2)||(iy>write_page->view_y2)){
return;
}

//overrides
qbg_active_page_offset=write_page->offset32;
qbg_width=write_page->width;
qbg_view_x1=write_page->view_x1;
qbg_view_y1=write_page->view_y1;
qbg_view_x2=write_page->view_x2;
qbg_view_y2=write_page->view_y2;
i=write_page->width*write_page->height;
if (i>done_size){
free(done);
done=(uint8*)calloc(i,1);
}

//return if first point is the bordercolor
if (qbg_active_page_offset[iy*qbg_width+ix]==bordercol) return;

//create first node
a_x[0]=ix; a_y[0]=iy;
a_t[0]=15;
//types:
//&1=check left
//&2=check right
//&4=check above
//&8=check below

a_n=1;
qbg_active_page_offset[iy*qbg_width+ix]=fillcol;
done[iy*qbg_width+ix]=1;

nextpass:
b_n=0;
for (i=0;i<a_n;i++){
t=a_t[i]; ix=a_x[i]; iy=a_y[i];

//left
if (t&1){
x2=ix-1; y2=iy;
if (x2>=qbg_view_x1){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=13; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//right
if (t&2){
x2=ix+1; y2=iy;
if (x2<=qbg_view_x2){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=14; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//above
if (t&4){
x2=ix; y2=iy-1;
if (y2>=qbg_view_y1){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=7; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//below
if (t&8){
x2=ix; y2=iy+1;
if (y2<=qbg_view_y2){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=11; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

}//i

//no new nodes?
if (b_n==0){
memset(done,0,write_page->width*write_page->height);//cleanup
return;//finished!
}

//swap a & b arrays
sp=a_x; a_x=b_x; b_x=sp;
sp=a_y; a_y=b_y; b_y=sp;
cp=a_t; a_t=b_t; b_t=cp;
a_n=b_n;

goto nextpass;
}



//8-bit (default entry point)
void sub_paint(float x,float y,uint32 fillcol,uint32 bordercol,qbs *backgroundstr,int32 passed){
if (new_error) return;
if (write_page->text){error(5); return;}
if (passed&8){error(5); return;}

if (write_page->bytes_per_pixel==4){
if (write_page->alpha_disabled){
sub_paint32x(x,y,fillcol,bordercol,passed);
return;
}else{
sub_paint32(x,y,fillcol,bordercol,passed);
return;
}
}

//uses 2 buffers, a and b, and swaps between them for reading and creating
static uint32 a_n=0;
static uint16 *a_x=(uint16*)malloc(2*65536),*a_y=(uint16*)malloc(2*65536);
static uint8 *a_t=(uint8*)malloc(65536);
static uint32 b_n=0;
static uint16 *b_x=(uint16*)malloc(2*65536),*b_y=(uint16*)malloc(2*65536);
static uint8 *b_t=(uint8*)malloc(65536);
static uint8 *done=(uint8*)calloc(640*480,1);
static int32 ix,iy,i,t,x2,y2;
static uint32 offset;
static uint8 *cp;
static uint16 *sp;
//overrides
static int32 done_size=640*480;
static uint8 *qbg_active_page_offset;//override
static int32 qbg_width,qbg_view_x1,qbg_view_y1,qbg_view_x2,qbg_view_y2;//override

if ((passed&2)==0) fillcol=write_page->color;
if ((passed&4)==0) bordercol=fillcol;
fillcol&=write_page->mask;
bordercol&=write_page->mask;
write_page->draw_color=fillcol;

if (passed&1){write_page->x+=x; write_page->y+=y;}else{write_page->x=x; write_page->y=y;}

if (write_page->clipping_or_scaling){
if (write_page->clipping_or_scaling==2){
ix=qbr_float_to_long(write_page->x*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
iy=qbr_float_to_long(write_page->y*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
}else{
ix=qbr_float_to_long(write_page->x)+write_page->view_offset_x; iy=qbr_float_to_long(write_page->y)+write_page->view_offset_y;
}
}else{
ix=qbr_float_to_long(write_page->x); iy=qbr_float_to_long(write_page->y);
}

//return if offscreen
if ((ix<write_page->view_x1)||(iy<write_page->view_y1)||(ix>write_page->view_x2)||(iy>write_page->view_y2)){
return;
}

//overrides
qbg_active_page_offset=write_page->offset;
qbg_width=write_page->width;
qbg_view_x1=write_page->view_x1;
qbg_view_y1=write_page->view_y1;
qbg_view_x2=write_page->view_x2;
qbg_view_y2=write_page->view_y2;
i=write_page->width*write_page->height;
if (i>done_size){
free(done);
done=(uint8*)calloc(i,1);
}

//return if first point is the bordercolor
if (qbg_active_page_offset[iy*qbg_width+ix]==bordercol) return;

//create first node
a_x[0]=ix; a_y[0]=iy;
a_t[0]=15;
//types:
//&1=check left
//&2=check right
//&4=check above
//&8=check below

a_n=1;
qbg_active_page_offset[iy*qbg_width+ix]=fillcol;
done[iy*qbg_width+ix]=1;

nextpass:
b_n=0;
for (i=0;i<a_n;i++){
t=a_t[i]; ix=a_x[i]; iy=a_y[i];

//left
if (t&1){
x2=ix-1; y2=iy;
if (x2>=qbg_view_x1){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=13; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//right
if (t&2){
x2=ix+1; y2=iy;
if (x2<=qbg_view_x2){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=14; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//above
if (t&4){
x2=ix; y2=iy-1;
if (y2>=qbg_view_y1){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=7; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//below
if (t&8){
x2=ix; y2=iy+1;
if (y2<=qbg_view_y2){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=11; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

}//i

//no new nodes?
if (b_n==0){
memset(done,0,write_page->width*write_page->height);//cleanup
return;//finished!
}

//swap a & b arrays
sp=a_x; a_x=b_x; b_x=sp;
sp=a_y; a_y=b_y; b_y=sp;
cp=a_t; a_t=b_t; b_t=cp;
a_n=b_n;

goto nextpass;

}






void sub_paint(float x,float y,qbs *fillstr,uint32 bordercol,qbs *backgroundstr,int32 passed){
if (new_error) return;

//uses 2 buffers, a and b, and swaps between them for reading and creating
static uint32 fillcol=0;//stub
static uint32 a_n=0;
static uint16 *a_x=(uint16*)malloc(2*65536),*a_y=(uint16*)malloc(2*65536);
static uint8 *a_t=(uint8*)malloc(65536);
static uint32 b_n=0;
static uint16 *b_x=(uint16*)malloc(2*65536),*b_y=(uint16*)malloc(2*65536);
static uint8 *b_t=(uint8*)malloc(65536);
static uint8 *done=(uint8*)calloc(640*480,1);
static int32 ix,iy,i,t,x2,y2;
static uint32 offset;
static uint8 *cp;
static uint16 *sp;
static uint32 backgroundcol;

if (qbg_text_only){error(5); return;}
if ((passed&2)==0){error(5); return;}//must be called with this parameter!

//STEP 1: create the tile in a buffer (tile) using the source string
static uint8 tilestr[256];
static uint8 tile[8][64];
static int32 sx,sy;
static int32 bytesperrow;
static int32 row2offset;
static int32 row3offset;
static int32 row4offset;
static int32 byte;
static int32 bitvalue;
static int32 c;
if (fillstr->len==0){error(5); return;}
if (qbg_bits_per_pixel==4){
if (fillstr->len>256){error(5); return;}
}else{
if (fillstr->len>64){error(5); return;}
}
memset(&tilestr[0],0,256);
memcpy(&tilestr[0],fillstr->chr,fillstr->len);
sx=8; sy=fillstr->len; //defaults
if (qbg_bits_per_pixel==8) sx=1;
if (qbg_bits_per_pixel==4){
if (fillstr->len&3){
sy=(fillstr->len-(fillstr->len&3)+4)>>2;
}else{
sy=fillstr->len>>2;
}
bytesperrow=sx>>3; if (sx&7) bytesperrow++;
row2offset=bytesperrow;
row3offset=bytesperrow*2;
row4offset=bytesperrow*3;
}
if (qbg_bits_per_pixel==2) sx=4;
//use modified "PUT" routine to create the tile
cp=&tilestr[0];
{//layer
static int32 x,y;
for (y=0;y<sy;y++){
if (qbg_bits_per_pixel==4){
bitvalue=128;
byte=0;
}
for (x=0;x<sx;x++){
//get colour
if (qbg_bits_per_pixel==8){
 c=*cp;
 cp++;
}
if (qbg_bits_per_pixel==4){
byte=x>>3;
c=0;
if (cp[byte]&bitvalue) c|=1;
if (cp[row2offset+byte]&bitvalue) c|=2;
if (cp[row3offset+byte]&bitvalue) c|=4;
if (cp[row4offset+byte]&bitvalue) c|=8;
bitvalue>>=1; if (bitvalue==0) bitvalue=128;
}
if (qbg_bits_per_pixel==1){
 if (!(x&7)){
  byte=*cp;
  cp++;
 }
 c=(byte&128)>>7; byte<<=1;
}
if (qbg_bits_per_pixel==2){
 if (!(x&3)){
  byte=*cp;
  cp++;
 }
 c=(byte&192)>>6; byte<<=2;
}
//"pset" color
tile[x][y]=c;
}//x
if (qbg_bits_per_pixel==4) cp+=(bytesperrow*4);
if (qbg_bits_per_pixel==1){
 if (sx&7) cp++;
}
if (qbg_bits_per_pixel==2){
 if (sx&3) cp++;
}
}//y
}//unlayer
//tile created!

//STEP 2: establish border and background colors
if ((passed&4)==0) bordercol=qbg_color;
bordercol&=qbg_pixel_mask;

backgroundcol=0;//default
if (passed&8){
if (backgroundstr->len==0){error(5); return;}
if (backgroundstr->len>255){error(5); return;}
if (qbg_bits_per_pixel==1){
c=backgroundstr->chr[0];
if ((c>0)&&(c<255)) backgroundcol=-1;//unclear definition
if (c==255) backgroundcol=1;
}
if (qbg_bits_per_pixel==2){
backgroundcol=-1;//unclear definition
x2=backgroundstr->chr[0];
y2=x2&3;
x2>>=2; if ((x2&3)!=y2) goto uncleardef;
x2>>=2; if ((x2&3)!=y2) goto uncleardef;

x2>>=2; if ((x2&3)!=y2) goto uncleardef;

backgroundcol=y2;
}
if (qbg_bits_per_pixel==4){
backgroundcol=-1;//unclear definition
y2=0;
x2=4; if (backgroundstr->len<4) x2=backgroundstr->len;
c=0; memcpy(&c,backgroundstr->chr,x2);
x2=c&255; c>>=8; if ((x2!=0)&&(x2!=255)) goto uncleardef;
y2|=(x2&1);
x2=c&255; c>>=8; if ((x2!=0)&&(x2!=255)) goto uncleardef;
y2|=((x2&1)<<1);
x2=c&255; c>>=8; if ((x2!=0)&&(x2!=255)) goto uncleardef;
y2|=((x2&1)<<2);
x2=c&255; c>>=8; if ((x2!=0)&&(x2!=255)) goto uncleardef;
y2|=((x2&1)<<3);
backgroundcol=y2;
}
if (qbg_bits_per_pixel==8){
backgroundcol=backgroundstr->chr[0];
}
}
uncleardef:

//STEP 3: perform tile'd fill
if (passed&1){qbg_x+=x; qbg_y+=y;}else{qbg_x=x; qbg_y=y;}
if (qbg_clipping_or_scaling){
if (qbg_clipping_or_scaling==2){
ix=qbr_float_to_long(qbg_x*qbg_scaling_x+qbg_scaling_offset_x)+qbg_view_offset_x;
iy=qbr_float_to_long(qbg_y*qbg_scaling_y+qbg_scaling_offset_y)+qbg_view_offset_y;
}else{
ix=qbr_float_to_long(qbg_x)+qbg_view_offset_x; iy=qbr_float_to_long(qbg_y)+qbg_view_offset_y;
}
}else{
ix=qbr_float_to_long(qbg_x); iy=qbr_float_to_long(qbg_y);
}

//return if offscreen
if ((ix<qbg_view_x1)||(iy<qbg_view_y1)||(ix>qbg_view_x2)||(iy>qbg_view_y2)){
return;
}

offset=iy*qbg_width+ix;

//return if first point is the bordercolor
if (qbg_active_page_offset[offset]==bordercol) return;

//return if first point is the same as the tile color used and is not the background color
fillcol=tile[ix%sx][iy%sy];
if ((fillcol==qbg_active_page_offset[offset])&&(fillcol!=backgroundcol)) return;
qbg_active_page_offset[offset]=fillcol;




//create first node
a_x[0]=ix; a_y[0]=iy;
a_t[0]=15;
//types:
//&1=check left
//&2=check right
//&4=check above
//&8=check below

a_n=1;
qbg_active_page_offset[iy*qbg_width+ix]=fillcol;
done[iy*qbg_width+ix]=1;

nextpass:
b_n=0;
for (i=0;i<a_n;i++){
t=a_t[i]; ix=a_x[i]; iy=a_y[i];

//left
if (t&1){
x2=ix-1; y2=iy;
if (x2>=qbg_view_x1){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
fillcol=tile[x2%sx][y2%sy];
//no tile check required when moving horizontally!
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=13; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//right
if (t&2){
x2=ix+1; y2=iy;
if (x2<=qbg_view_x2){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
fillcol=tile[x2%sx][y2%sy];
//no tile check required when moving horizontally!
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=14; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}}}}

//above
if (t&4){
x2=ix; y2=iy-1;
if (y2>=qbg_view_y1){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
fillcol=tile[x2%sx][y2%sy];
if ((fillcol!=qbg_active_page_offset[offset])||(fillcol==backgroundcol)){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=7; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}
}}}}

//below
if (t&8){
x2=ix; y2=iy+1;
if (y2<=qbg_view_y2){
offset=y2*qbg_width+x2;
if (!done[offset]){
done[offset]=1;
if (qbg_active_page_offset[offset]!=bordercol){
fillcol=tile[x2%sx][y2%sy];
if ((fillcol!=qbg_active_page_offset[offset])||(fillcol==backgroundcol)){
qbg_active_page_offset[offset]=fillcol;
b_t[b_n]=11; b_x[b_n]=x2; b_y[b_n]=y2; b_n++;//add new node
}
}}}}

}//i

//no new nodes?
if (b_n==0){
memset(done,0,qbg_width*qbg_height);//cleanup
return;//finished!
}

//swap a & b arrays
sp=a_x; a_x=b_x; b_x=sp;
sp=a_y; a_y=b_y; b_y=sp;
cp=a_t; a_t=b_t; b_t=cp;
a_n=b_n;

goto nextpass;

}










void sub_circle(double x,double y,double r,uint32 col,double start,double end,double aspect,int32 passed){
//                                                &2         &4           &8         &16
//[{STEP}](?,?),?[,[?][,[?][,[?][,?]]]]
if (new_error) return;



//data
static double pi= 3.1415926535897932,pi2=6.2831853071795865;
static int32 line_to_start,line_from_end;
static int32 ix,iy;//integer screen co-ordinates of circle's centre
static double xspan,yspan;
static double c;//circumference
static double px,py;
static double sinb,cosb;//second angle used in double-angle-formula
static int32 pixels;
static double tmp;
static int32 tmpi;
static int32 i;
static int32 exclusive;
static double arc1,arc2,arc3,arc4,arcinc;
static double px2,py2;
static int32 x2,y2;
static int32 x3,y3;
static int32 lastplotted_x2,lastplotted_y2;
static int32 lastchecked_x2,lastchecked_y2;

if (write_page->text){error(5); return;}

//lines to & from centre
if (!(passed&4)) start=0;
if (!(passed&8)) end=pi2;
line_to_start=0; if (start<0) {line_to_start=1; start=-start;}
line_from_end=0; if (end<0) {line_from_end=1; end=-end;}

//error checking
if (start>pi2){error(5); return;}
if (end>pi2){error(5); return;}

//when end<start, the arc of the circle that wouldn't have been drawn if start & end
//were swapped is drawn
exclusive=0;
if (end<start){
tmp=start; start=end; end=tmp;
tmpi=line_to_start; line_to_start=line_from_end; line_from_end=tmpi;
exclusive=1;
}

//calc. centre
if (passed&1){x=write_page->x+x; y=write_page->y+y;}
write_page->x=x; write_page->y=y;//set graphics cursor position to circle's centre



r=x+r;//the differece between x & x+r in pixels will be the radius in pixels
//resolve coordinates (but keep as floats)
if (write_page->clipping_or_scaling){
if (write_page->clipping_or_scaling==2){
x=x*write_page->scaling_x+write_page->scaling_offset_x+write_page->view_offset_x;
y=y*write_page->scaling_y+write_page->scaling_offset_y+write_page->view_offset_y;
r=r*write_page->scaling_x+write_page->scaling_offset_x+write_page->view_offset_x;
}else{
x=x+write_page->view_offset_x;
y=y+write_page->view_offset_y;
r=r+write_page->view_offset_x;
}
}
if (x<0) ix=x-0.5; else ix=x+0.5;
if (y<0) iy=y-0.5; else iy=y+0.5;
r=fabs(r-x);//r is now a radius in pixels



//adjust vertical and horizontal span of the circle based on aspect ratio
xspan=r; yspan=r;
if (!(passed&16)){
aspect=1;//Note: default aspect ratio is 1:1 for QB64 specific modes (256/32)
if (write_page->compatible_mode==1) aspect=4.0*(200.0/320.0)/3.0;
if (write_page->compatible_mode==2) aspect=4.0*(200.0/640.0)/3.0;
if (write_page->compatible_mode==7) aspect=4.0*(200.0/320.0)/3.0;
if (write_page->compatible_mode==8) aspect=4.0*(200.0/640.0)/3.0;
if (write_page->compatible_mode==9) aspect=4.0*(350.0/640.0)/3.0;
if (write_page->compatible_mode==10) aspect=4.0*(350.0/640.0)/3.0;
if (write_page->compatible_mode==11) aspect=4.0*(480.0/640.0)/3.0;
if (write_page->compatible_mode==12) aspect=4.0*(480.0/640.0)/3.0;
if (write_page->compatible_mode==13) aspect=4.0*(200.0/320.0)/3.0;
//Old method: aspect=4.0*((double)write_page->height/(double)write_page->width)/3.0;
}
if (aspect>=0){
 if (aspect<1){
  //aspect: 0 to 1
  yspan*=aspect;
 }
 if (aspect>1){
  //aspect: 1 to infinity
  xspan/=aspect;
 }
}else{
 if (aspect>-1){
  //aspect: -1 to 0
  yspan*=(1+aspect);
 }
 //if aspect<-1 no change is required
}

//skip everything if none of the circle is inside current viwport
if ((x+xspan+0.5)<write_page->view_x1) return;
if ((y+yspan+0.5)<write_page->view_y1) return;
if ((x-xspan-0.5)>write_page->view_x2) return;
if ((y-yspan-0.5)>write_page->view_y2) return;

if (!(passed&2)) col=write_page->color;
write_page->draw_color=col;

//pre-set/pre-calculate values
c=pi2*r;
pixels=c/4.0+0.5;
arc1=0;
arc2=pi;
arc3=pi;
arc4=pi2;
arcinc=(pi/2)/(double)pixels;
sinb=sin(arcinc);
cosb=cos(arcinc);
lastplotted_x2=-1;
lastchecked_x2=-1;
i=0;

if (line_to_start){
px=cos(start); py=sin(start);
x2=px*xspan+0.5; y2=py*yspan-0.5;
fast_line(ix,iy,ix+x2,iy-y2,col);
}

px=1;
py=0;

drawcircle:
x2=px*xspan+0.5;
y2=py*yspan-0.5;

if (i==0) {lastchecked_x2=x2; lastchecked_y2=y2; goto plot;}

if ( (abs(x2-lastplotted_x2)>=2)||(abs(y2-lastplotted_y2)>=2) ){
plot:
if (exclusive){
if ((arc1<=start)||(arc1>=end)){pset_and_clip(ix+lastchecked_x2,iy+lastchecked_y2,col);}
if ((arc2<=start)||(arc2>=end)){pset_and_clip(ix-lastchecked_x2,iy+lastchecked_y2,col);}
if ((arc3<=start)||(arc3>=end)){pset_and_clip(ix-lastchecked_x2,iy-lastchecked_y2,col);}
if ((arc4<=start)||(arc4>=end)){pset_and_clip(ix+lastchecked_x2,iy-lastchecked_y2,col);}
}else{//inclusive
if ((arc1>=start)&&(arc1<=end)){pset_and_clip(ix+lastchecked_x2,iy+lastchecked_y2,col);}
if ((arc2>=start)&&(arc2<=end)){pset_and_clip(ix-lastchecked_x2,iy+lastchecked_y2,col);}
if ((arc3>=start)&&(arc3<=end)){pset_and_clip(ix-lastchecked_x2,iy-lastchecked_y2,col);}
if ((arc4>=start)&&(arc4<=end)){pset_and_clip(ix+lastchecked_x2,iy-lastchecked_y2,col);}
}
if (i>pixels) goto allplotted;
lastplotted_x2=lastchecked_x2; lastplotted_y2=lastchecked_y2;
}
lastchecked_x2=x2; lastchecked_y2=y2;

if (i<=pixels){
i++;
if (i>pixels) goto plot;
px2=px*cosb+py*sinb;
py=py*cosb-px*sinb;
px=px2;
if (i) {arc1+=arcinc; arc2-=arcinc; arc3+=arcinc; arc4-=arcinc;}
goto drawcircle;
}
allplotted:

if (line_from_end){
px=cos(end); py=sin(end);
x2=px*xspan+0.5; y2=py*yspan-0.5;
fast_line(ix,iy,ix+x2,iy-y2,col);
}

}//sub_circle



uint32 point(int32 x,int32 y){//does not clip!
if (read_page->bytes_per_pixel==1){
return read_page->offset[y*read_page->width+x]&read_page->mask;
}else{
return read_page->offset32[y*read_page->width+x];
}
return NULL;
}




double func_point(float x,float y,int32 passed){
static int32 x2,y2,i;

if (!passed){
if (write_page->text){error(5); return 0;}
i=qbr_float_to_long(x);
if ((i<0)||(i>3)){error(5); return 0;}
switch(i){
case 0:
	if (write_page->clipping_or_scaling==2){
	return qbr_float_to_long(write_page->x*write_page->scaling_x+write_page->scaling_offset_x);
	}
	return qbr_float_to_long(write_page->x);
	break;
case 1:
	if (write_page->clipping_or_scaling==2){
	return qbr_float_to_long(write_page->y*write_page->scaling_y+write_page->scaling_offset_y);
	}
	return qbr_float_to_long(write_page->y);
	break;
case 2:
	return write_page->x;
	break;
case 3:
	return write_page->y;
	break;
default:
	error(5);
	return 0;
}
}//!passed

if (read_page->text){error(5); return 0;}
if (read_page->clipping_or_scaling){
 if (read_page->clipping_or_scaling==2){
 x2=qbr_float_to_long(x*read_page->scaling_x+read_page->scaling_offset_x)+read_page->view_offset_x;
 y2=qbr_float_to_long(y*read_page->scaling_y+read_page->scaling_offset_y)+read_page->view_offset_y;
 }else{
 x2=qbr_float_to_long(x)+read_page->view_offset_x; y2=qbr_float_to_long(y)+read_page->view_offset_y;
 }
}else{
x2=qbr_float_to_long(x); y2=qbr_float_to_long(y);
}
if (x2>=read_page->view_x1){ if (x2<=read_page->view_x2){
if (y2>=read_page->view_y1){ if (y2<=read_page->view_y2){
return point(x2,y2);
}}}}
return -1;
}








void sub_pset(float x,float y,uint32 col,int32 passed){
if (new_error) return;
static int32 x2,y2;
if (!write_page->compatible_mode){error(5); return;}
//Special Format: [{STEP}](?,?),[?]
if (passed&1){write_page->x+=x; write_page->y+=y;}else{write_page->x=x; write_page->y=y;}
if (!(passed&2)) col=write_page->color;
write_page->draw_color=col;
if (write_page->clipping_or_scaling){
if (write_page->clipping_or_scaling==2){
x2=qbr(write_page->x*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
y2=qbr(write_page->y*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
}else{
x2=qbr(write_page->x)+write_page->view_offset_x; y2=qbr(write_page->y)+write_page->view_offset_y;
}
if (x2>=write_page->view_x1){ if (x2<=write_page->view_x2){
if (y2>=write_page->view_y1){ if (y2<=write_page->view_y2){
 pset(x2,y2,col);
}}}}
return;
}else{
 x2=qbr(write_page->x); if (x2>=0){ if (x2<write_page->width){
 y2=qbr(write_page->y); if (y2>=0){ if (y2<write_page->height){
  pset(x2,y2,col);
 }}}}
}
return;
}

void sub_preset(float x,float y,uint32 col,int32 passed){
if (new_error) return;
if (!(passed&2)){
col=write_page->background_color;
passed|=2;
}
sub_pset(x,y,col,passed);
return;
}


int32 img_printchr=0;
int32 img_printchr_i;
int32 img_printchr_x;
int32 img_printchr_y;
char *img_printchr_offset;

void printchr(int32 character){
static uint32 x,x2,y,y2,w,h,z,z2,z3,a,a2,a3,color,background_color,f;
static uint32 *lp;
static uint8 *cp;
static img_struct *im;
static SDL_Surface *ts;
static SDL_Color c,c2;

im=write_page;
color=im->color;
background_color=im->background_color;


if (im->text){
im->offset[(((im->cursor_y-1)*im->width+im->cursor_x-1))<<1]=character;
im->offset[((((im->cursor_y-1)*im->width+im->cursor_x-1))<<1)+1]=(color&0xF)+background_color*16+(color&16)*8;
return;
}

//precalculations

f=im->font;
x=fontwidth[f]; if (x) x*=(im->cursor_x-1); else x=im->cursor_x-1;
y=(im->cursor_y-1)*fontheight[f];
h=fontheight[f];
if ((fontflags[f]&32)==0) character&=255;//unicodefontsupport

c.r=255; c.g=255; c.b=255; c.unused=0;//dummy values
c2.r=255; c2.g=255; c2.b=255; c2.unused=0;//dummy values


//if (mode==1) img[i].print_mode=3;//fill
//if (mode==2) img[i].print_mode=1;//keep
//if (mode==3) img[i].print_mode=2;//only

if (f>=32){//custom font

//8-bit / alpha-disabled 32-bit / dont-blend(alpha may still be applied)
if ((im->bytes_per_pixel==1)||((im->bytes_per_pixel==4)&&(im->alpha_disabled))||(fontflags[f]&8)){

convert_text_to_utf16(f,&character,1);
ts=TTF_RenderUNICODE_Solid(font[f],(Uint16*)unicode16_buf,c);


//ts=TTF_RenderText_Solid(font[f],(char*)&character,c);//8-bit, 0=clear, 1=text

if (ts==NULL) return;
w=ts->w;
if (x2=fontwidth[f]) if (x2!=w) w=x2;//render width too large!  
switch(im->print_mode){
case 3:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 if (*cp++) pset(x+x2,y+y2,color); else pset(x+x2,y+y2,background_color);
 }}
 break;
case 1:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 if (*cp++) pset(x+x2,y+y2,color);
 }}
 break;
case 2:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 if (!(*cp++)) pset(x+x2,y+y2,background_color);
 }}
break;
default:
break;
}//z
SDL_FreeSurface(ts);
return;
}//1-8 bit

//assume 32-bit blended
a=(color>>24)+1;
a2=(background_color>>24)+1;
z=color&0xFFFFFF;
z2=background_color&0xFFFFFF;
//8 bit, 0=background -> 255=foreground

convert_text_to_utf16(f,&character,1);
ts=TTF_RenderUNICODE_Shaded(font[f],(Uint16*)unicode16_buf,c,c2);
//ts=TTF_RenderText_Shaded(font[f],(char*)&character,c,c2);

if (ts==NULL) return;
w=ts->w;
if (x2=fontwidth[f]) if (x2!=w) w=x2;//render width too large!  
switch(im->print_mode){
case 3:

 static float r1,g1,b1,alpha1,r2,g2,b2,alpha2;
 alpha1=(color>>24)&255; r1=(color>>16)&255; g1=(color>>8)&255; b1=color&255;
 alpha2=(background_color>>24)&255; r2=(background_color>>16)&255; g2=(background_color>>8)&255; b2=background_color&255;
 static float dr,dg,db,da;
 dr=r2-r1;
 dg=g2-g1;
 db=b2-b1;
 da=alpha2-alpha1;
 static float cw;//color weight multiplier, avoids seeing black when transitioning from RGBA(?,?,?,255) to RGBA(0,0,0,0)
 if (alpha1) cw=alpha2/alpha1; else cw=100000;
 static float d;
 
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){

d=*cp++;
d=255-d;
d/=255.0;
static float r3,g3,b3,alpha3;
alpha3=alpha1+da*d;
d*=cw; if (d>1.0) d=1.0;
r3=r1+dr*d;
g3=g1+dg*d;
b3=b1+db*d;
static int32 r4,g4,b4,alpha4;
r4=qbr_float_to_long(r3);
g4=qbr_float_to_long(g3);
b4=qbr_float_to_long(b3);
alpha4=qbr_float_to_long(alpha3);
pset(x+x2,y+y2,b4+(g4<<8)+(r4<<16)+(alpha4<<24));

 }}
break;
case 1:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 z3=*cp++;
 if (z3) pset(x+x2,y+y2,((z3*a)>>8<<24)+z);
 }}
break;
case 2:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 z3=*cp++;
 if (z3!=255) pset(x+x2,y+y2,(((255-z3)*a2)>>8<<24)+z2);
 }}
break;
default:
break;
}
SDL_FreeSurface(ts);
return;

}//custom font

//default fonts
if (im->font==8) cp=&charset8x8[character][0][0];
if (im->font==14) cp=&charset8x16[character][1][0];
if (im->font==16) cp=&charset8x16[character][0][0];
switch(im->print_mode){
case 3:
 for (y2=0;y2<h;y2++){ for (x2=0;x2<8;x2++){
 if (*cp++) pset(x+x2,y+y2,color); else pset(x+x2,y+y2,background_color);
 }}
 break;
case 1:
 for (y2=0;y2<h;y2++){ for (x2=0;x2<8;x2++){
 if (*cp++) pset(x+x2,y+y2,color);
 }}
 break;
case 2:
 for (y2=0;y2<h;y2++){ for (x2=0;x2<8;x2++){
 if (!(*cp++)) pset(x+x2,y+y2,background_color);
 }}
 break;
default:
break;
}//z
return;

}//printchr


//prints a character at pixel offset x,y of image surface i
//returns the width in pixels of the character printed
//this is used for printing un-kerned characters
//cannot be used on a text surface!
int32 printchr2(int32 x,int32 y,uint32 character,int32 i){
static uint32 x2,y2,w,h,z,z2,z3,a,a2,a3,color,background_color,f;
static uint32 *lp;
static uint8 *cp;
static img_struct *im;
static SDL_Surface *ts;
static SDL_Color c,c2;

im=&img[i];
color=im->color;
background_color=im->background_color;

//precalculations
character&=255;
f=im->font;
h=fontheight[f];
c.r=255; c.g=255; c.b=255; c.unused=0;//dummy values
c2.r=255; c2.g=255; c2.b=255; c2.unused=0;//dummy values

if (f>=32){//custom font

//8-bit / alpha-disabled 32-bit / dont-blend(alpha may still be applied)
if ((im->bytes_per_pixel==1)||((im->bytes_per_pixel==4)&&(im->alpha_disabled))||(fontflags[f]&8)){

convert_text_to_utf16(f,&character,1);
ts=TTF_RenderUNICODE_Solid(font[f],(Uint16*)unicode16_buf,c);
//ts=TTF_RenderText_Solid(font[f],(char*)&character,c);//8-bit, 0=clear, 1=text

if (ts==NULL) return 0;
w=ts->w;
if (x2=fontwidth[f]) if (x2!=w) w=x2;//render width too large!  
switch(im->print_mode){
case 3:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 if (*cp++) pset_and_clip(x+x2,y+y2,color); else pset_and_clip(x+x2,y+y2,background_color);
 }}
 break;
case 1:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 if (*cp++) pset_and_clip(x+x2,y+y2,color);
 }}
 break;
case 2:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 if (!(*cp++)) pset_and_clip(x+x2,y+y2,background_color);
 }}
break;
default:
break;
}//z
SDL_FreeSurface(ts);
return w;
}//1-8 bit

//assume 32-bit blended
a=(color>>24)+1;
a2=(background_color>>24)+1;
z=color&0xFFFFFF;
z2=background_color&0xFFFFFF;
//8 bit, 0=background -> 255=foreground

convert_text_to_utf16(f,&character,1);
ts=TTF_RenderUNICODE_Shaded(font[f],(Uint16*)unicode16_buf,c,c2);
//ts=TTF_RenderText_Shaded(font[f],(char*)&character,c,c2);

if (ts==NULL) return 0;
w=ts->w;
if (x2=fontwidth[f]) if (x2!=w) w=x2;//render width too large!  
switch(im->print_mode){
case 3:

static float r1,g1,b1,alpha1,r2,g2,b2,alpha2;
 alpha1=(color>>24)&255; r1=(color>>16)&255; g1=(color>>8)&255; b1=color&255;
 alpha2=(background_color>>24)&255; r2=(background_color>>16)&255; g2=(background_color>>8)&255; b2=background_color&255;
 static float dr,dg,db,da;
 dr=r2-r1;
 dg=g2-g1;
 db=b2-b1;
 da=alpha2-alpha1;
 static float cw;//color weight multiplier, avoids seeing black when transitioning from RGBA(?,?,?,255) to RGBA(0,0,0,0)
 if (alpha1) cw=alpha2/alpha1; else cw=100000;
 static float d;

 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){

d=*cp++;
d=255-d;
d/=255.0;
static float r3,g3,b3,alpha3;
alpha3=alpha1+da*d;
d*=cw; if (d>1.0) d=1.0;
r3=r1+dr*d;
g3=g1+dg*d;
b3=b1+db*d;
static int32 r4,g4,b4,alpha4;
r4=qbr_float_to_long(r3);
g4=qbr_float_to_long(g3);
b4=qbr_float_to_long(b3);
alpha4=qbr_float_to_long(alpha3);
pset_and_clip(x+x2,y+y2,b4+(g4<<8)+(r4<<16)+(alpha4<<24));

 }}
break;
case 1:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 z3=*cp++;
 //if (z3!=255) pset_and_clip(x+x2,y+y2,(((255-z3)*a2)>>8<<24)+z2);
 if (z3) pset_and_clip(x+x2,y+y2,((z3*a)>>8<<24)+z);
 }}
break;
case 2:
 for (y2=0;y2<h;y2++){
 cp=((uint8*)ts->pixels)+(y2+1)*ts->pitch;
 for (x2=0;x2<w;x2++){
 z3=*cp++;
 if (z3!=255) pset_and_clip(x+x2,y+y2,(((255-z3)*a2)>>8<<24)+z2);
 //if (z3) pset_and_clip(x+x2,y+y2,((z3*a)>>8<<24)+z);
 }}
break;
default:
break;
}
SDL_FreeSurface(ts);
return w;
}//custom font

//default fonts
if (im->font==8) cp=&charset8x8[character][0][0];
if (im->font==14) cp=&charset8x16[character][1][0];
if (im->font==16) cp=&charset8x16[character][0][0];
switch(im->print_mode){
case 3:
 for (y2=0;y2<h;y2++){ for (x2=0;x2<8;x2++){
 if (*cp++) pset_and_clip(x+x2,y+y2,color); else pset_and_clip(x+x2,y+y2,background_color);
 }}
 break;
case 1:
 for (y2=0;y2<h;y2++){ for (x2=0;x2<8;x2++){
 if (*cp++) pset_and_clip(x+x2,y+y2,color);
 }}
 break;
case 2:
 for (y2=0;y2<h;y2++){ for (x2=0;x2<8;x2++){
 if (!(*cp++)) pset_and_clip(x+x2,y+y2,background_color);
 }}
 break;
default:
break;
}//z
return 8;

}//printchr2



int32 chrwidth(int32 character){
static uint32 x;
static img_struct *im;
static SDL_Surface *ts;
static SDL_Color c;
im=write_page;
if (x=fontwidth[im->font]) return x;
if ((fontflags[im->font]&32)==NULL) character&=255;
//int minx,maxx,miny,maxy,advance;
//TTF_GlyphMetrics(font[im->font], character,  &minx,  &maxx,  &miny,  &maxy,  &advance);
c.r=255; c.g=255; c.b=255; c.unused=0;//dummy values
if (im->alpha_disabled||(fontflags[im->font]&8)||(im->bytes_per_pixel==1)){

convert_text_to_utf16(im->font,&character,1);
ts=TTF_RenderUNICODE_Solid(font[im->font],(Uint16*)unicode16_buf,c);
//ts=TTF_RenderText_Solid(font[im->font],(char*)&character,c);//8-bit, 0=clear, 1=text

}else{

convert_text_to_utf16(im->font,&character,1);
ts=TTF_RenderUNICODE_Shaded(font[im->font],(Uint16*)unicode16_buf,c,c);
//ts=TTF_RenderText_Shaded(font[im->font],(char*)&character,c,c);//8-bit 0-255

}
if (ts==NULL) return 0;
x=ts->w;
SDL_FreeSurface(ts);
return x;
}//chrwidth


void newline(){
static uint32 *lp;
static uint16 *sp;
static int32 z,z2;

//move cursor to new line
write_page->cursor_y++; write_page->cursor_x=1;

//scroll up screen if necessary
if (write_page->cursor_y>write_page->bottom_row){

if (lprint){
sub__printimage(lprint_image);
sub_cls(NULL,15,2);
lprint_buffered=0;
return;
}

if (write_page->text){
//text
//move lines up
memmove(
 write_page->offset+(write_page->top_row-1)*2*write_page->width,
 write_page->offset+ write_page->top_row   *2*write_page->width,
 (write_page->bottom_row-write_page->top_row)*2*write_page->width
);
//erase bottom line
z2=(write_page->color&0xF)+(write_page->background_color&7)*16+(write_page->color&16)*8;
z2<<=8;
z2+=32;
sp=((uint16*)(write_page->offset+(write_page->bottom_row-1)*2*write_page->width));
z=write_page->width;
while(z--) *sp++=z2;
}else{
//graphics
//move lines up
memmove(
 write_page->offset+(write_page->top_row-1)*write_page->bytes_per_pixel*write_page->width*fontheight[write_page->font],
 write_page->offset+ write_page->top_row   *write_page->bytes_per_pixel*write_page->width*fontheight[write_page->font],
 (write_page->bottom_row-write_page->top_row)*write_page->bytes_per_pixel*write_page->width*fontheight[write_page->font]
);
//erase bottom line
if (write_page->bytes_per_pixel==1){
memset(write_page->offset+(write_page->bottom_row-1)*write_page->width*fontheight[write_page->font],write_page->background_color,write_page->width*fontheight[write_page->font]);
}else{
//assume 32-bit
z2=write_page->background_color;
lp=write_page->offset32+(write_page->bottom_row-1)*write_page->width*fontheight[write_page->font];
z=write_page->width*fontheight[write_page->font];
while(z--) *lp++=z2;
}
}//graphics
write_page->cursor_y=write_page->bottom_row;
}//scroll up

}//newline

void makefit(qbs *text){
static int32 w,x,x2,x3;
if (write_page->holding_cursor) return;
if (write_page->cursor_x!=1){//if already at left-most, nothing more can be done
if (write_page->text){
 if ((write_page->cursor_x+text->len-1)>write_page->width) newline();
}else{
 w=func__printwidth(text,NULL,NULL);
 x=fontwidth[write_page->font]; if (!x) x=1; x=x*(write_page->cursor_x-1);
 if ((x+w)>write_page->width) newline();
}
}
}

void lprint_makefit(qbs *text){
//stub
//makefit(text);
}

void tab(){
static int32 x,x2,w;

//tab() on a held-cursor only sets the cursor to the left hand position of the next line
if (write_page->holding_cursor){
newline(); write_page->holding_cursor=0;
return;
}

//text
if (write_page->text){
qbs_print(singlespace,0);
text:
if (write_page->cursor_x!=1){
if (((write_page->cursor_x-1)%14)||(write_page->cursor_x>(write_page->width-13))){
if (write_page->cursor_x<write_page->width){qbs_print(singlespace,0); goto text;}
}
}//!=1
return;
}

x=fontwidth[write_page->font]; 
if (!x){

//variable width
x=write_page->cursor_x-1;
x2=(x/112+1)*112;//next position
if (x2>=write_page->width){//it doesn't fit on line
//box fill x to end of line with background color
fast_boxfill(x,(write_page->cursor_y-1)*fontheight[write_page->font],write_page->width-1,write_page->cursor_y*fontheight[write_page->font]-1,write_page->background_color);
newline();
}else{//fits on line
//box fill x to x2-1 with background color
fast_boxfill(x,(write_page->cursor_y-1)*fontheight[write_page->font],x2-1,write_page->cursor_y*fontheight[write_page->font]-1,write_page->background_color);
write_page->cursor_x=x2;
}


}else{

//fixed width
w=write_page->width/x;

qbs_print(singlespace,0);
fixwid:
if (write_page->cursor_x!=1){
if (((write_page->cursor_x-1)%14)||(write_page->cursor_x>(w-13))){
if (write_page->cursor_x<w){qbs_print(singlespace,0); goto fixwid;}
}
}//!=1

}
return;
}

int32 func_lpos(int32 lpt){
//lpt values: 0 = LPT1, 1 = LPT1, 2 = LPT2, 3 = LPT3
if ((lpt<0)||(lpt>3)){error(5); return 0;}
return lpos;
}

void qbs_lprint(qbs* str,int32 finish_on_new_line){
static int32 old_dest;
while (lprint_locked) Sleep(64);
lprint=1;
old_dest=func__dest();
if (!lprint_image){
lprint_image=func__newimage(640,960,13,1);
sub__dest(lprint_image);
sub_cls(NULL,15,2);
sub__font(16,NULL,0);
qbg_sub_color(0,15,NULL,3);
qbg_sub_view_print(1,60,1);
}else{
sub__dest(lprint_image);
}
lprint_buffered=1;
lprint_last=func_timer(0.001,1);
qbs_print(str,finish_on_new_line);
sub__dest(old_dest);
lprint=0;
}

int32 no_control_characters=0;

void qbs_print(qbs* str,int32 finish_on_new_line){
if (new_error) return;
int32 i,i2,entered_new_line,x,x2,y,y2,z,z2,w;
entered_new_line=0;
static uint32 character;

/*
if (!str->len){
if (!newline) return;//no action required
if (write_page->holding_cursor){//extra CR required before return
write_page->holding_cursor=0;
i=-1;
write_page->cursor_x++;
goto print_unhold_cursor;
}
}

if (!str->len) goto null_length;

if (write_page->holding_cursor){
write_page->holding_cursor=0;
i=-1;
write_page->cursor_x++;
goto print_unhold_cursor;
}
*/

//holding cursor?
if (write_page->holding_cursor){
if (str->len){
write_page->holding_cursor=0;
newline();
}else{
//null length print string
if (finish_on_new_line) write_page->holding_cursor=0;//new line will be entered automatically
}
}


for (i=0;i<str->len;i++){

character=str->chr[i];

if (fontflags[write_page->font]&32){//unicode font
if (i>(str->len-4)) break;//not enough data for a utf32 encoding
character=*((int32*)(&str->chr[i]));
i+=3;
}

if (lprint) lprint_buffered=1;
entered_new_line=0;//beginning a new line was the last action (so don't add a new one)



//###special characters

if (no_control_characters) goto skip_control_characters;

if (character==28){
//advance one cursor position
if (lprint){
 if (lpos<width_lprint) lpos++;
}
//can cursor advance?
if (write_page->cursor_y>=write_page->bottom_row){
 if (write_page->text){
  if (write_page->cursor_x>=write_page->width) goto skip;
 }else{
  if (fontwidth[write_page->font]){
   if (write_page->cursor_x>=(write_page->width/fontwidth[write_page->font])) goto skip;
  }else{
   if (write_page->cursor_x>=write_page->width) goto skip;
  }
 } 
}
write_page->cursor_x++;
 if (write_page->text){
  if (write_page->cursor_x>write_page->width){write_page->cursor_y++; write_page->cursor_x=1;}
 }else{
  if (fontwidth[write_page->font]){
   if (write_page->cursor_x>(write_page->width/fontwidth[write_page->font])){write_page->cursor_y++; write_page->cursor_x=1;}
  }else{
   if (write_page->cursor_x>write_page->width){write_page->cursor_y++; write_page->cursor_x=1;}
  }
 } 
goto skip;
}

if (character==29){
//go back one cursor position
if (lprint){
if (lpos>1) lpos--;
}
//can cursor go back?
if ((write_page->cursor_y==write_page->top_row)||(write_page->cursor_y>write_page->bottom_row)){
if (write_page->cursor_x==1) goto skip;
}
write_page->cursor_x--;
if (write_page->cursor_x<1){
write_page->cursor_y--;
 if (write_page->text){
  write_page->cursor_x=write_page->width;
 }else{
  if (fontwidth[write_page->font]){
   write_page->cursor_x=write_page->width/fontwidth[write_page->font];
  }else{
   write_page->cursor_x=write_page->width;
  }
 } 
}
goto skip;
}

if (character==30){
//previous row, same column
//no change if cursor not within view print boundries
if ((write_page->cursor_y>write_page->top_row)&&(write_page->cursor_y<=write_page->bottom_row)){
write_page->cursor_y--;
}
goto skip;
}

if (character==31){
//next row, same column
//no change if cursor not within view print boundries
if ((write_page->cursor_y>=write_page->top_row)&&(write_page->cursor_y<write_page->bottom_row)){
write_page->cursor_y++;
}
goto skip;
}

if (character==12){//aka form feed
if (lprint){sub__printimage(lprint_image); lprint_buffered=0;}
//clears text viewport
//clears bottom row
//moves cursor to top-left of text viewport
sub_cls(NULL,NULL,0);
if (lprint) lpos=1;
goto skip;
}

if (character==11){
write_page->cursor_x=1; write_page->cursor_y=write_page->top_row;
if (lprint) lpos=1;
goto skip;
}

if (character==9){
//moves to next multiple of 8 (always advances at least one space)
if (!fontwidth[write_page->font]){
 //variable width!
 x=write_page->cursor_x-1;
 x2=(x/64+1)*64;//next position
 if (x2>=write_page->width){//it doesn't fit on line
 //box fill x to end of line with background color
 fast_boxfill(x,(write_page->cursor_y-1)*fontheight[write_page->font],write_page->width-1,write_page->cursor_y*fontheight[write_page->font]-1,write_page->background_color);
 newline();
 entered_new_line=1;
 }else{//fits on line
 //box fill x to x2-1 with background color
 fast_boxfill(x,(write_page->cursor_y-1)*fontheight[write_page->font],x2-1,write_page->cursor_y*fontheight[write_page->font]-1,write_page->background_color);
 write_page->cursor_x=x2;
 }
 goto skip;
}else{
 if (write_page->cursor_x%8){//next cursor position not a multiple of 8
 i--;//more spaces will be required
 }
 character=32;//override character 9
}
}//9

if (character==7){
qb64_generatesound(783.99,0.2,0);
Sleep(250);
goto skip;
}

if ((character==10)||(character==13)){
newline();
if (lprint) lpos=1;
//note: entered_new_line not set because these carriage returns compound on each other
goto skip;
}

skip_control_characters:

//###check if character fits on line, if not move to next line
//(only applies to non-fixed width fonts)
if (!fontwidth[write_page->font]){//unpredictable width
w=chrwidth(character);
if ((write_page->cursor_x+w)>write_page->width){
newline();
//entered_new_line not set, a character will follow
}
}

//###print the character
printchr(character);

//###advance lpos, begin new line if necessary
if (lprint){
 lpos++;
 if (lpos>width_lprint){
  newline();
  entered_new_line=1;
  lpos=1;
  goto skip;//skip cursor advancement and checking because new line entered
 }
}

//###advance cursor
if (fontwidth[write_page->font]){
write_page->cursor_x++;
}else{
write_page->cursor_x+=w;
}

//###check if another character could fit at cursor_x's location
if (write_page->compatible_mode){//graphics
 x=fontwidth[write_page->font]; if (!x) x=1;
 x2=x*(write_page->cursor_x-1);
 if (x2>(write_page->width-x)){
  if (!finish_on_new_line){
  if (i==(str->len-1)){//last character
  //move horizontal cursor back to right-most valid position
  write_page->cursor_x=write_page->width/x;
  write_page->holding_cursor=1;
  goto held_cursor;
  }
  }
 newline();
 entered_new_line=1;
 }
}else{//text
 if (write_page->cursor_x>write_page->width){
  if (!finish_on_new_line){
  if (i==(str->len-1)){//last character
  write_page->cursor_x--;//move horizontal cursor back to right-most valid position
  write_page->holding_cursor=1;
  goto held_cursor;
  }
  }
 newline();
 entered_new_line=1;
 }
}
held_cursor:

skip:;

/*
tabbing1:


write_page->cursor_x++;



//hold cursor?
if (write_page->cursor_x>qbg_width_in_characters){//past last x position
if (!newline){//don't need a new line
if (i==(str->len-1)){//last character
write_page->cursor_x--;
write_page->holding_cursor=1;
goto hold_cursor;
}
}
}




qbs_print_skipchar:;

print_unhold_cursor:

if (write_page->cursor_x>qbg_width_in_characters){
qbs_print_newline:
newlineadded=1;

if (write_page->cursor_y==qbg_height_in_characters) write_page->cursor_y=qbg_bottom_row;

write_page->cursor_y++;
write_page->cursor_x=1;



if (write_page->cursor_y>qbg_bottom_row){
//move screen space within view print up 1 row
//if (qbg_mode==13){

///memmove(&cmem[655360+(qbg_top_row-1)*2560],&cmem[655360+qbg_top_row*2560],(qbg_bottom_row-qbg_top_row)*2560);
///memset(&cmem[655360+(qbg_bottom_row-1)*2560],0,2560);
if (qbg_text_only){

memmove(qbg_active_page_offset+(qbg_top_row-1)*qbg_width_in_characters*2,
        qbg_active_page_offset+(qbg_top_row)*qbg_width_in_characters*2,
        (qbg_bottom_row-qbg_top_row)*qbg_width_in_characters*2);
for (i2=0;i2<qbg_width_in_characters;i2++){
qbg_active_page_offset[(qbg_bottom_row-1)*qbg_width_in_characters*2+i2*2]=32;
qbg_active_page_offset[(qbg_bottom_row-1)*qbg_width_in_characters*2+i2*2+1]=7;

}

}else{
memmove(qbg_active_page_offset+(qbg_top_row-1)*qbg_bytes_per_pixel*qbg_width*qbg_character_height,
        qbg_active_page_offset+qbg_top_row*qbg_bytes_per_pixel*qbg_width*qbg_character_height,
        (qbg_bottom_row-qbg_top_row)*qbg_bytes_per_pixel*qbg_width*qbg_character_height);
memset(qbg_active_page_offset+(qbg_bottom_row-1)*qbg_bytes_per_pixel*qbg_width*qbg_character_height,0,qbg_bytes_per_pixel*qbg_width*qbg_character_height);
}




write_page->cursor_y=qbg_bottom_row;
}



}
*/

}//i

null_length:
if (finish_on_new_line&&(!entered_new_line)) newline();
if (lprint){
 if (finish_on_new_line) lpos=1;
}


/*
null_length:

//begin new line?
if (newline&&(!newlineadded)) {newline=0; goto qbs_print_newline;}

//hold cursor
hold_cursor:
*/

return;

}


int32 qbs_cleanup(uint32 base,int32 passvalue){
while (qbs_tmp_list_nexti>base) { qbs_tmp_list_nexti--; if(qbs_tmp_list[qbs_tmp_list_nexti]!=-1)qbs_free((qbs*)qbs_tmp_list[qbs_tmp_list_nexti]); }//clear any temp. strings created
return passvalue;
}



void qbg_sub_window(float x1,float y1,float x2,float y2,int32 passed){
//                  &1
//(passed&2)->SCREEN
if (new_error) return;
static float i;
static float old_x,old_y;

if (write_page->text) goto qbg_sub_window_error;
if ((!(passed&1))&&(passed&2)) goto qbg_sub_window_error;//SCREEEN passed without any other arguements!

//backup current qbg_x & qbg_y coordinates relative to viewport, not window
if (write_page->clipping_or_scaling==2){
old_x=write_page->x*write_page->scaling_x+write_page->scaling_offset_x;
old_y=write_page->y*write_page->scaling_y+write_page->scaling_offset_y;
}else{
old_x=write_page->x;
old_y=write_page->y;
}

if (passed&1){
if (x1==x2) goto qbg_sub_window_error;
if (y1==y2) goto qbg_sub_window_error;
//sort so x1 & y1 contain the lower values
if (x1>x2){i=x1; x1=x2; x2=i;}
if (y1>y2){i=y1; y1=y2; y2=i;}
if (!(passed&2)){
i=y1; y1=y2; y2=i;
}
//Note: Window's coordinates are not based on prev. WINDOW values
write_page->clipping_or_scaling=2;
write_page->scaling_x=((float)(write_page->view_x2-write_page->view_x1))/(x2-x1);
write_page->scaling_y=((float)(write_page->view_y2-write_page->view_y1))/(y2-y1);
write_page->scaling_offset_x=-x1*write_page->scaling_x; //scaling offset should be applied before scaling
write_page->scaling_offset_y=-y1*write_page->scaling_y;
if (!(passed&2)){
write_page->scaling_offset_y=-y2*write_page->scaling_y+(write_page->view_y2-write_page->view_y1);
}
write_page->window_x1=x1; write_page->window_x2=x2; write_page->window_y1=y1; write_page->window_y2=y2;


if (x1==0){ if (y1==0){ if (x2==(write_page->width-1)){ if (y2==(write_page->height-1)){
if ((write_page->scaling_x==1)&&(write_page->scaling_y==1)){
if ((write_page->scaling_offset_x==0)&&(write_page->scaling_offset_y==0)){
goto qbg_sub_window_restore_default;
}
}
}}}}

//adjust qbg_x & qbg_y according to new window
write_page->x=(old_x-write_page->scaling_offset_x)/write_page->scaling_x;
write_page->y=(old_y-write_page->scaling_offset_y)/write_page->scaling_y;

return;
}else{
//restore default WINDOW coordinates
qbg_sub_window_restore_default:
write_page->clipping_or_scaling=1;
write_page->scaling_x=1;
write_page->scaling_y=1;
write_page->scaling_offset_x=0;
write_page->scaling_offset_y=0;
write_page->window_x1=0; write_page->window_x2=write_page->width-1; write_page->window_y1=0; write_page->window_y2=write_page->height-1;
if (write_page->view_x1==0){ if (write_page->view_y1==0){ if (write_page->view_x2==(write_page->width-1)){ if (write_page->view_y2==(write_page->height-1)){
if (write_page->view_offset_x==0){ if (write_page->view_offset_y==0){
write_page->clipping_or_scaling=0;
}}
}}}}

//adjust qbg_x & qbg_y according to new window
write_page->x=old_x;
write_page->y=old_y;

return;
}
qbg_sub_window_error:
error(5);
return;
}



void qbg_sub_view_print(int32 topline,int32 bottomline,int32 passed){
if (new_error) return;

static int32 maxrows;
maxrows=write_page->height; if (!write_page->text) maxrows/=fontheight[write_page->font];

if (!passed){//topline and bottomline not passed
write_page->top_row=1; write_page->bottom_row=maxrows;
write_page->cursor_y=1; write_page->cursor_x=1;
write_page->holding_cursor=0;
return;
}

if (topline<=0) goto error;
if (topline>maxrows) goto error;
if (bottomline<topline) goto error;
if (bottomline>maxrows) goto error;

write_page->top_row=topline;
write_page->bottom_row=bottomline;
write_page->cursor_y=write_page->top_row;
write_page->cursor_x=1;
write_page->holding_cursor=0;
return;

error:
error(5);
return;
}

void qbg_sub_view(int32 x1,int32 y1,int32 x2,int32 y2,int32 fillcolor,int32 bordercolor,int32 passed){
//   &1		    						  &4			  &8
//    (passed&2)->coords_relative_to_screen
if (new_error) return;
//format: [{SCREEN}][(?,?)-(?,?)],[?],[?]
//bordercolor draws a line AROUND THE OUTSIDE of the specified viewport
//the current WINDOW settings do not affect inputted x,y values
//the current VIEW settings do not affect inputted x,y values
//REMEMBER! Recalculate WINDOW values based on new viewport dimensions
int32 i;

//PRE-ERROR CHECKING
if (passed&1){
if (x1<0) goto error;
if (x1>=write_page->width) goto error;
if (y1<0) goto error;
if (y1>=write_page->height) goto error;
if (x2<0) goto error;
if (x2>=write_page->width) goto error;
if (y2<0) goto error;
if (y2>=write_page->height) goto error;
}else{
if (passed&2) goto error;
if (passed&4) goto error;
if (passed&8) goto error;
}

//reset DRAW attributes
write_page->draw_ta=0.0; write_page->draw_scale=1.0;

if (passed&1){
//force x1,y1 to be the top left corner
if (x2<x1){i=x1;x1=x2;x2=i;}
if (y2<y1){i=y1;y1=y2;y2=i;}

write_page->view_x1=x1; write_page->view_y1=y1; write_page->view_x2=x2; write_page->view_y2=y2;
if ((passed&2)==0){
write_page->view_offset_x=x1; write_page->view_offset_y=y1;
}else{
write_page->view_offset_x=0; write_page->view_offset_y=0;
}
if (!write_page->clipping_or_scaling) write_page->clipping_or_scaling=1;
}else{
//no argurments passed
write_page->view_x1=0; write_page->view_y1=0; write_page->view_x2=write_page->width-1; write_page->view_y2=write_page->height-1;
write_page->view_offset_x=0; write_page->view_offset_y=0;
if (write_page->clipping_or_scaling==1) write_page->clipping_or_scaling=0;
}

//recalculate window values based on new viewport (if necessary)
if (write_page->clipping_or_scaling==2){//WINDOW'ing in use
write_page->scaling_x=((float)(write_page->view_x2-write_page->view_x1))/(write_page->window_x2-write_page->window_x1);
write_page->scaling_y=((float)(write_page->view_y2-write_page->view_y1))/(write_page->window_y2-write_page->window_y1);
write_page->scaling_offset_x=-write_page->window_x1*write_page->scaling_x;
write_page->scaling_offset_y=-write_page->window_y1*write_page->scaling_y;
if (write_page->window_y2<write_page->window_y1) write_page->scaling_offset_y=-write_page->window_y2*write_page->scaling_y+write_page->view_y2;
}

if (passed&4){//fillcolor
qb32_boxfill(write_page->window_x1,write_page->window_y1,write_page->window_x2,write_page->window_y2,fillcolor);
}

if (passed&8){//bordercolor
static int32 bx,by;
by=write_page->view_y1-1;
if ((by>=0)&&(by<write_page->height)){
for (bx=write_page->view_x1-1;bx<=write_page->view_x2;bx++){
if ((bx>=0)&&(bx<write_page->width)){
pset(bx,by,bordercolor);
}}}
by=write_page->view_y2+1;
if ((by>=0)&&(by<write_page->height)){
for (bx=write_page->view_x1-1;bx<=write_page->view_x2;bx++){
if ((bx>=0)&&(bx<write_page->width)){
pset(bx,by,bordercolor);
}}}
bx=write_page->view_x1-1;
if ((bx>=0)&&(bx<write_page->width)){
for (by=write_page->view_y1-1;by<=write_page->view_y2;by++){
if ((by>=0)&&(by<write_page->height)){
pset(bx,by,bordercolor);
}}}
bx=write_page->view_x2+1;
if ((bx>=0)&&(bx<write_page->width)){
for (by=write_page->view_y1-1;by<=(write_page->view_y2+1);by++){
if ((by>=0)&&(by<write_page->height)){
pset(bx,by,bordercolor);
}}}
}

return;
error:
error(5);
return;
}



void sub_cls(int32 method,uint32 use_color,int32 passed){
if (new_error) return;
static int32 characters,i;
static uint16 *sp;
static uint16 clearvalue;

//validate
if (passed&2){
if (write_page->bytes_per_pixel!=4){
if (use_color>write_page->mask) goto error;
}
}else{
use_color=write_page->background_color;
}

if (passed&1){
if ((method>2)||(method<0)) goto error;
}



//all CLS methods reset the cursor position
write_page->cursor_y=write_page->top_row;
write_page->cursor_x=1;

//all CLS methods reset DRAW attributes
write_page->draw_ta=0.0; write_page->draw_scale=1.0;

if (write_page->text){
//precalculate a (int16) value which can be used to clear the screen
clearvalue=(write_page->color&0xF)+(use_color&7)*16+(write_page->color&16)*8;
clearvalue<<=8;
clearvalue+=32;
}

if ((passed&1)==0){//no method specified
//video mode: clear only graphics viewport
//text mode: clear text view port AND the bottom line
if (write_page->text){
 //text view port
 characters=write_page->width*(write_page->bottom_row-write_page->top_row+1);
 sp=(uint16*)&write_page->offset[(write_page->top_row-1)*write_page->width*2];
 for (i=0;i<characters;i++){sp[i]=clearvalue;}
 //bottom line
 characters=write_page->width;
 sp=(uint16*)&write_page->offset[(write_page->height-1)*write_page->width*2];
 for (i=0;i<characters;i++){sp[i]=clearvalue;}
 key_display_redraw=1; key_update();
 return;
}else{//graphics
 //graphics view port
 if (write_page->bytes_per_pixel==1){//8-bit
  if (write_page->clipping_or_scaling){
  qb32_boxfill(write_page->window_x1,write_page->window_y1,write_page->window_x2,write_page->window_y2,use_color);
  }else{//fast method (no clipping/scaling)
  memset(write_page->offset,use_color,write_page->width*write_page->height);
  }
 }else{//32-bit
  i=write_page->alpha_disabled; write_page->alpha_disabled=1;  
  if (write_page->clipping_or_scaling){
  qb32_boxfill(write_page->window_x1,write_page->window_y1,write_page->window_x2,write_page->window_y2,use_color);
  }else{//fast method (no clipping/scaling)
  fast_boxfill(0,0,write_page->width-1,write_page->height-1,use_color);
  }
  write_page->alpha_disabled=i;
 }
}

if (write_page->clipping_or_scaling==2){
write_page->x=((float)(write_page->view_x2-write_page->view_x1+1))/write_page->scaling_x/2.0f+write_page->scaling_offset_x;
write_page->y=((float)(write_page->view_y2-write_page->view_y1+1))/write_page->scaling_y/2.0f+write_page->scaling_offset_y;
}else{
write_page->x=((float)(write_page->view_x2-write_page->view_x1+1))/2.0f;
write_page->y=((float)(write_page->view_y2-write_page->view_y1+1))/2.0f;
}

key_display_redraw=1; key_update();
return;
}

if (method==0){//clear everything
if (write_page->text){
 characters=write_page->height*write_page->width;
 sp=(uint16*)write_page->offset;
 for (i=0;i<characters;i++){sp[i]=clearvalue;}
 key_display_redraw=1; key_update();
 return;
}else{
 if (write_page->bytes_per_pixel==1){
 memset(write_page->offset,use_color,write_page->width*write_page->height);
 }else{ 
 i=write_page->alpha_disabled; write_page->alpha_disabled=1;  
 fast_boxfill(0,0,write_page->width-1,write_page->height-1,use_color);
 write_page->alpha_disabled=i;
 }
}

if (write_page->clipping_or_scaling==2){
write_page->x=((float)(write_page->view_x2-write_page->view_x1+1))/write_page->scaling_x/2.0f+write_page->scaling_offset_x;
write_page->y=((float)(write_page->view_y2-write_page->view_y1+1))/write_page->scaling_y/2.0f+write_page->scaling_offset_y;
}else{
write_page->x=((float)(write_page->view_x2-write_page->view_x1+1))/2.0f;
write_page->y=((float)(write_page->view_y2-write_page->view_y1+1))/2.0f;
}

key_display_redraw=1; key_update();
return;
}

if (method==1){//ONLY clear the graphics viewport
if (write_page->text) return;
 //graphics view port
 if (write_page->bytes_per_pixel==1){//8-bit
  if (write_page->clipping_or_scaling){
  qb32_boxfill(write_page->window_x1,write_page->window_y1,write_page->window_x2,write_page->window_y2,use_color);
  }else{//fast method (no clipping/scaling)
  memset(write_page->offset,use_color,write_page->width*write_page->height);
  }
 }else{//32-bit
  i=write_page->alpha_disabled; write_page->alpha_disabled=1;  
  if (write_page->clipping_or_scaling){
  qb32_boxfill(write_page->window_x1,write_page->window_y1,write_page->window_x2,write_page->window_y2,use_color);
  }else{//fast method (no clipping/scaling)
  fast_boxfill(0,0,write_page->width-1,write_page->height-1,use_color);
  }
  write_page->alpha_disabled=i;
 }

if (write_page->clipping_or_scaling==2){
write_page->x=((float)(write_page->view_x2-write_page->view_x1+1))/write_page->scaling_x/2.0f+write_page->scaling_offset_x;
write_page->y=((float)(write_page->view_y2-write_page->view_y1+1))/write_page->scaling_y/2.0f+write_page->scaling_offset_y;
}else{
write_page->x=((float)(write_page->view_x2-write_page->view_x1+1))/2.0f;
write_page->y=((float)(write_page->view_y2-write_page->view_y1+1))/2.0f;
}

key_display_redraw=1; key_update();
return;
}

if (method==2){//ONLY clear the VIEW PRINT range text viewport
if (write_page->text){
 //text viewport
 characters=write_page->width*(write_page->bottom_row-write_page->top_row+1);
 sp=(uint16*)&write_page->offset[(write_page->top_row-1)*write_page->width*2];
 for (i=0;i<characters;i++){sp[i]=clearvalue;}
 //should not and does not redraw KEY bar
 return;
}else{
 //text viewport
 if (write_page->bytes_per_pixel==1){//8-bit
  memset(&write_page->offset[write_page->width*fontheight[write_page->font]*(write_page->top_row-1)],use_color,write_page->width*fontheight[write_page->font]*(write_page->bottom_row-write_page->top_row+1));
 }else{//32-bit
  i=write_page->alpha_disabled; write_page->alpha_disabled=1;  
  fast_boxfill(0,fontheight[write_page->font]*(write_page->top_row-1),write_page->width-1,fontheight[write_page->font]*write_page->bottom_row-1,use_color);
  write_page->alpha_disabled=i;
 }
 //should not and does not redraw KEY bar
 return;
}
}

return;
error:
error(5);
return;
}



void qbg_sub_locate(int32 row,int32 column,int32 cursor,int32 start,int32 stop,int32 passed){
static int32 h,w,i;
if (new_error) return;

//calculate height & width in characters
if (write_page->compatible_mode){
h=write_page->height/fontheight[write_page->font];
if (fontwidth[write_page->font]){
 w=write_page->width/fontwidth[write_page->font];
}else{
 w=write_page->width;
}
}else{
h=write_page->height;
w=write_page->width;
}

//PRE-ERROR CHECKING
if (passed&1){
if (row<write_page->top_row) goto error;
if ((row!=h)&&(row>write_page->bottom_row)){
 if (width8050switch){//note: can assume WIDTH 80,25 as no SCREEN or WIDTH statements have been called
 width8050switch=0;
 if (row<=50){
 if (passed&2){
 if (column<1) goto error;
 if (column>w) goto error;
 }
 char *buffer;
 uint32 c,c2;
 buffer=(char*)malloc(80*25*2);
 c=write_page->color; c2=write_page->background_color;
 memcpy(buffer,&cmem[0xB8000],80*25*2);
 qbsub_width(0,80,50,3);
 memcpy(&cmem[0xB8000],buffer,80*25*2);
 write_page->color=c; write_page->background_color=c2;
 free(buffer);
 goto width8050switch_done;
 }
 }
goto error;
}
}
width8050switch_done:
if (passed&2){
if (column<1) goto error;
if (column>w) goto error;
}
if (passed&4){
if (cursor<0) goto error;
if (cursor>1) goto error;
}
if (passed&8){
if (start<0) goto error;
if (start>31) goto error;
if (stop<0) goto error;
if (stop>31) goto error;
}

if (passed&1) {write_page->cursor_y=row; write_page->holding_cursor=0;}
if (passed&2) {write_page->cursor_x=column; write_page->holding_cursor=0;}

if (passed&4){
if (cursor) cursor=1;
write_page->cursor_show=cursor;
 if (write_page->flags&IMG_SCREEN){//page-linked attribute
 for (i=0;i<pages;i++){
 if (page[i]) img[i].cursor_show=cursor;
 }
 }//IMG_SCREEN
}//passed&4

if (passed&8){
write_page->cursor_firstvalue=start;
write_page->cursor_lastvalue=stop;
 if (write_page->flags&IMG_SCREEN){//page-linked attribute
 for (i=0;i<pages;i++){
 if (page[i]){
 img[i].cursor_firstvalue=start;
 img[i].cursor_lastvalue=stop;
 }
 }//i
 }//IMG_SCREEN
}

return;

error:
error(5);
return;
}









//input helper functions:
uint64 hexoct2uint64_value;
int32 hexoct2uint64(qbs* h){
//returns 0=failed
//        1=HEX value (default if unspecified)
//        2=OCT value
static int32 i,i2;
static uint64 result;
result=0;
static int32 type;
type=0;
hexoct2uint64_value=0;
if (!h->len) return 1;
if (h->chr[0]!=38) return 0;//not "&"
if (h->len==1) return 1;//& received, but awaiting further input
i=h->chr[1];
if ((i==72)||(i==104)) type=1;//"H"or"h"
if ((i==79)||(i==111)) type=2;//"O"or"o"
if (!type) return 0;
if (h->len==2) return type;

if (type==1){
if (h->len>18) return 0;//larger than int64
for (i=2;i<h->len;i++){
result<<=4;
i2=h->chr[i];
//          0  -      9             A  -      F             a  -      f
if ( ((i2>=48)&&(i2<=57)) || ((i2>=65)&&(i2<=70)) || ((i2>=97)&&(i2<=102)) ){
if (i2>=97) i2-=32;
if (i2>=65) i2-=7;
i2-=48;
//i2 is now a values between 0 and 15
result+=i2;
}else return 0;//invalid character
}//i
hexoct2uint64_value=result;
return 1;
}//type==1

if (type==2){
//unsigned _int64 max=18446744073709551615 (decimal, 20 chars)
//                   =1777777777777777777777 (octal, 22 chars)
//                   =FFFFFFFFFFFFFFFF (hex, 16 chars)
if (h->len>24) return 0;//larger than int64
if (h->len==24){
if ((h->chr[2]!=48)&&(h->chr[2]!=49)) return 0;//larger than int64
}
for (i=2;i<h->len;i++){
result<<=3;
i2=h->chr[i];
if ((i2>=48)&&(i2<=55)){//0-7
i2-=48;
result+=i2;
}else return 0;//invalid character
}//i
hexoct2uint64_value=result;
return 2;
}//type==2

}



//input method (complex, calls other qbs functions)
const char *uint64_max[] =    {"18446744073709551615"};
const char *int64_max[] =     {"9223372036854775807"};
const char *int64_max_neg[] = {"9223372036854775808"};
const char *single_max[] = {"3402823"};
const char *single_max_neg[] = {"1401298"};
const char *double_max[] = {"17976931"};
const char *double_max_neg[] = {"4940656"};
uint8 significant_digits[1024];
int32 num_significant_digits;

extern void *qbs_input_variableoffsets[257];
extern int32 qbs_input_variabletypes[257];
qbs *qbs_input_arguements[257];
int32 cursor_show_last;


void qbs_input(int32 numvariables,uint8 newline){
if (new_error) return;

static int32 autodisplay_backup;
autodisplay_backup=autodisplay;
autodisplay=1;

static int32 source_backup;
source_backup=func__source();
sub__source(func__dest());

//duplicate dest image so any changes can be reverted
static int32 dest_image,dest_image_temp,dest_image_holding_cursor;
dest_image=func__copyimage(NULL,NULL);
if (dest_image==-1) error(516);//out of memory
dest_image_temp=func__copyimage(NULL,NULL);
if (dest_image_temp==-1) error(517);//out of memory
static int32 dest_image_cursor_x,dest_image_cursor_y;
dest_image_cursor_x=write_page->cursor_x;
dest_image_cursor_y=write_page->cursor_y;
dest_image_holding_cursor=write_page->holding_cursor;

uint32 qbs_tmp_base=qbs_tmp_list_nexti;

static int32 lineinput;
lineinput=0;
if (qbs_input_variabletypes[1]&ISSTRING){
if (qbs_input_variabletypes[1]&512){
qbs_input_variabletypes[1]=-512;
lineinput=1;
}}

cursor_show_last=write_page->cursor_show;
write_page->cursor_show=1;

int32 i,i2,i3,i4,i5,i6;
int32 addspaces;
addspaces=0;
qbs* inpstr=qbs_new(0,0);//not temp so must be freed
qbs* inpstr2=qbs_new(0,0);//not temp so must be freed
qbs* key=qbs_new(0,0);//not temp so must be freed
qbs* c=qbs_new(1,0);//not temp so must be freed

for (i=1;i<=numvariables;i++) qbs_input_arguements[i]=qbs_new(0,0);

//init all passed variables to 0 or ""
for (i=1;i<=numvariables;i++){

if (qbs_input_variabletypes[i]&ISSTRING){//STRING
if (((qbs*)qbs_input_variableoffsets[i])->fixed){
memset(((qbs*)qbs_input_variableoffsets[i])->chr,32,((qbs*)qbs_input_variableoffsets[i])->len);
}else{
((qbs*)qbs_input_variableoffsets[i])->len=0;
}
}

if ((qbs_input_variabletypes[i]&ISOFFSETINBITS)==0){//reg. numeric variable
memset(qbs_input_variableoffsets[i],0,(qbs_input_variabletypes[i]&511)>>3);
}

//bit referenced?

}//i




qbs_input_next:

int32 argn,firstchr,toomany;
toomany=0;
argn=1;
i=0;
i2=0;
qbs_input_arguements[1]->len=0;
firstchr=1;
qbs_input_sep_arg:

if (i<inpstr->len){

if (inpstr->chr[i]==44){//","
if (i2!=1){//not in the middle of a string
if (!lineinput){
i2=0;
argn=argn+1;
if (argn>numvariables){toomany=1; goto qbs_input_sep_arg_done;}
qbs_input_arguements[argn]->len=0;
firstchr=1;
goto qbs_input_next_arg;
}
}
}

if (inpstr->chr[i]==34){//"
if (firstchr){
if (!lineinput){
i2=1;//requires closure
firstchr=0;
goto qbs_input_next_arg;
}
}
if (i2==1){
i2=2;
goto qbs_input_next_arg;
}
}

if (i2==2){
goto backspace;//INVALID! Cannot have any characters after a closed "..."
}

c->chr[0]=inpstr->chr[i];
qbs_set(qbs_input_arguements[argn],qbs_add(qbs_input_arguements[argn],c));

firstchr=0;
qbs_input_next_arg:;
i++;
goto qbs_input_sep_arg;
}
qbs_input_sep_arg_done:
if (toomany) goto backspace;

//validate current arguements
//ASSUME LEADING & TRALING SPACES REMOVED!
uint8 valid;
uint8 neg;
int32 completewith;
int32 l;
uint8 *cp,*cp2;
uint64 max,max_neg,multiple,value;
uint64 hexvalue;

completewith=-1;
valid=1;
l=qbs_input_arguements[argn]->len;
cp=qbs_input_arguements[argn]->chr;
neg=0;

if ((qbs_input_variabletypes[argn]&ISSTRING)==0){
if ((qbs_input_variabletypes[argn]&ISFLOAT)==0){
if ((qbs_input_variabletypes[argn]&511)<=32){//cannot handle INTEGER64 variables using this method!
int64 finalvalue;
//it's an integer variable!
finalvalue=0;
if (l==0){completewith=48; goto typechecked_integer;}
//calculate max & max_neg (i4 used to store number of bits)
i4=qbs_input_variabletypes[argn]&511;
max=1;
max<<=i4;
max--;

//check for hex/oct
if (i3=hexoct2uint64(qbs_input_arguements[argn])){
hexvalue=hexoct2uint64_value;
if (hexvalue>max){valid=0; goto typechecked;}
//i. check max num of "digits" required to represent a value, if more exist cull excess
//ii. set completewith value (if necessary)
if (i3==1){
 value=max;
 i=0;
 for (i2=1;i2<=16;i2++){
 if (value&0xF) i=i2;
 value>>=4;
 }
 if (l>(2+i)){valid=0; goto typechecked;}
 if (l==1) completewith=72;//"H"
 if (l==2) completewith=48;//"0"
}
if (i3==2){
 value=max;
 i=0;
 for (i2=1;i2<=22;i2++){
 if (value&0x7) i=i2;
 value>>=3;
 }
 if (l>(2+i)){valid=0; goto typechecked;}
 if (l==1) completewith=111;//"O"
 if (l==2) completewith=48;//"0"
}
finalvalue=hexvalue;
goto typechecked_integer;
}

//max currently contains the largest UNSIGNED value possible, adjust as necessary
if (qbs_input_variabletypes[argn]&ISUNSIGNED){ 
max_neg=0;
}else{
max>>=1;
max_neg=max+1;
}
//check for - sign
i2=0;
 if ((qbs_input_variabletypes[argn]&ISUNSIGNED)==0){ 
 if (cp[i2]==45){//"-"
 if (l==1) {completewith=48; goto typechecked_integer;}
 i2++; neg=1;
 }
 }
//after a leading 0 no other digits are possible, return an error if this is the case
if (cp[i2]==48){
if (l>(i2+1)){valid=0; goto typechecked;}
}
//scan the "number"...
multiple=1;
value=0;
for (i=l-1;i>=i2;i--){
i3=cp[i]-48;
if ((i3>=0)&&(i3<=9)){
value+=multiple*i3;
 if (qbs_input_variabletypes[argn]&ISUNSIGNED){ 
 if (value>max){valid=0; goto typechecked;}
 }else{
 if (neg){
 if (value>max_neg){valid=0; goto typechecked;}
 }else{
 if (value>max){valid=0; goto typechecked;}
 }
 }
}else{valid=0; goto typechecked;}
multiple*=10;
}//next i
if (neg) finalvalue=-value; else finalvalue=value;
typechecked_integer:
//set variable to finalvalue
if ((qbs_input_variabletypes[argn]&ISOFFSETINBITS)==0){//reg. numeric variable
memcpy(qbs_input_variableoffsets[argn],&finalvalue,(qbs_input_variabletypes[argn]&511)>>3);
}
goto typechecked;
}
}
}

if (qbs_input_variabletypes[argn]&ISSTRING){
if (((qbs*)qbs_input_variableoffsets[argn])->fixed){
if (l>((qbs*)qbs_input_variableoffsets[argn])->len) {valid=0; goto typechecked;}
}
qbs_set((qbs*)qbs_input_variableoffsets[argn],qbs_input_arguements[argn]);
goto typechecked;
}

//INTEGER64 type
//int64 range:          9223372036854775808 to  9223372036854775807
//uint64 range: 0                    to 18446744073709551615
if ((qbs_input_variabletypes[argn]&ISSTRING)==0){
if ((qbs_input_variabletypes[argn]&ISFLOAT)==0){
if ((qbs_input_variabletypes[argn]&511)==64){
if (l==0){completewith=48; *(int64*)qbs_input_variableoffsets[argn]=0; goto typechecked;}

//check for hex/oct
if (i3=hexoct2uint64(qbs_input_arguements[argn])){
hexvalue=hexoct2uint64_value;
if (hexvalue>max){valid=0; goto typechecked;}
//set completewith value (if necessary)
if (i3==1) if (l==1) completewith=72;//"H"
if (i3==2) if (l==1) completewith=111;//"O"
if (l==2) completewith=48;//"0"
*(uint64*)qbs_input_variableoffsets[argn]=hexvalue;
goto typechecked;
}

//check for - sign
i2=0;
 if ((qbs_input_variabletypes[argn]&ISUNSIGNED)==0){ 
 if (cp[i2]==45){//"-"
 if (l==1) {completewith=48; *(int64*)qbs_input_variableoffsets[argn]=0; goto typechecked;}
 i2++; neg=1;
 }
 }
//after a leading 0 no other digits are possible, return an error if this is the case
if (cp[i2]==48){
if (l>(i2+1)){valid=0; goto typechecked;}
}
//count how many digits are in the number
i4=0;
for (i=l-1;i>=i2;i--){
i3=cp[i]-48;
if ((i3<0)||(i3>9)) {valid=0; goto typechecked;}
i4++;
}//i
if (qbs_input_variabletypes[argn]&ISUNSIGNED){
if (i4<20) goto typechecked_int64;
if (i4>20) {valid=0; goto typechecked;}
cp2=(uint8*)uint64_max[0];
}else{
if (i4<19) goto typechecked_int64;
if (i4>19) {valid=0; goto typechecked;}
if (neg) cp2=(uint8*)int64_max_neg[0]; else cp2=(uint8*)int64_max[0];
}
//number of digits valid, but exact value requires checking
cp=qbs_input_arguements[argn]->chr;
for (i=0;i<i4;i++){
if (cp[i+i2]<cp2[i]) goto typechecked_int64;
if (cp[i+i2]>cp2[i]) {valid=0; goto typechecked;}
}
typechecked_int64:
//add character 0 to end to make it a null terminated string
c->chr[0]=0; qbs_set(qbs_input_arguements[argn],qbs_add(qbs_input_arguements[argn],c));
if (qbs_input_variabletypes[argn]&ISUNSIGNED){
#ifdef QB64_WINDOWS
 sscanf((char*)qbs_input_arguements[argn]->chr,"%I64u",(uint64*)qbs_input_variableoffsets[argn]);
#else
 sscanf((char*)qbs_input_arguements[argn]->chr,"%llu",(uint64*)qbs_input_variableoffsets[argn]);
#endif
}else{
#ifdef QB64_WINDOWS
 sscanf((char*)qbs_input_arguements[argn]->chr,"%I64i",(int64*)qbs_input_variableoffsets[argn]);
#else
 sscanf((char*)qbs_input_arguements[argn]->chr,"%lli",(int64*)qbs_input_variableoffsets[argn]);
#endif
}
goto typechecked;
}
}
}

//check ISFLOAT type?
//[-]9999[.]9999[E/D][+/-]99999
if (qbs_input_variabletypes[argn]&ISFLOAT){
static int32 digits_before_point;
static int32 digits_after_point;
static int32 zeros_after_point;
static int32 neg_power;
digits_before_point=0;
digits_after_point=0;
neg_power=0;
value=0;
zeros_after_point=0;
num_significant_digits=0;

//set variable to 0
if ((qbs_input_variabletypes[argn]&511)==32) *(float*)qbs_input_variableoffsets[argn]=0;
if ((qbs_input_variabletypes[argn]&511)==64) *(double*)qbs_input_variableoffsets[argn]=0;
if ((qbs_input_variabletypes[argn]&511)==256) *(long double*)qbs_input_variableoffsets[argn]=0;

//begin with a generic assessment, regardless of whether it is single, double or float
if (l==0){completewith=48; goto typechecked;}

//check for hex/oct
if (i3=hexoct2uint64(qbs_input_arguements[argn])){
hexvalue=hexoct2uint64_value;
//set completewith value (if necessary)
if (i3==1) if (l==1) completewith=72;//"H"
if (i3==2) if (l==1) completewith=111;//"O"
if (l==2) completewith=48;//"0"
//nb. because VC6 didn't support...
//error C2520: conversion from uint64 to double not implemented, use signed int64
//I've implemented a work-around so correct values will be returned
static int64 transfer;
transfer=0x7FFFFFFF;
transfer<<=32;
transfer|=0xFFFFFFFF;
while(hexvalue>transfer){
hexvalue-=transfer;
if ((qbs_input_variabletypes[argn]&511)==32) *(float*)qbs_input_variableoffsets[argn]+=transfer;
if ((qbs_input_variabletypes[argn]&511)==64) *(double*)qbs_input_variableoffsets[argn]+=transfer;
if ((qbs_input_variabletypes[argn]&511)==256) *(long double*)qbs_input_variableoffsets[argn]+=transfer;
}
transfer=hexvalue;
if ((qbs_input_variabletypes[argn]&511)==32) *(float*)qbs_input_variableoffsets[argn]+=transfer;
if ((qbs_input_variabletypes[argn]&511)==64) *(double*)qbs_input_variableoffsets[argn]+=transfer;
if ((qbs_input_variabletypes[argn]&511)==256) *(long double*)qbs_input_variableoffsets[argn]+=transfer;
goto typechecked;
}

//check for - sign
i2=0;
 if (cp[i2]==45){//"-"
 if (l==1) {completewith=48; goto typechecked;}
 i2++; neg=1;
 }
//if it starts with 0, it may only have one leading 0
if (cp[i2]==48){
if (l>(i2+1)){
i2++;
if (cp[i2]==46) goto decimal_point;
valid=0; goto typechecked;//expected a decimal point
//nb. of course, user could have typed D or E BUT there is no point
//    calculating 0 to the power of anything!
}else goto typechecked;//validate, as no other data is required
}
//scan digits before decimal place
for (i=i2;i<l;i++){
i3=cp[i];
if ((i3==68)||(i3==(68+32))||(i3==69)||(i3==(69+32))){//d,D,e,E?
if (i==i2){valid=0; goto typechecked;}//cannot begin with d,D,e,E!
i2=i;
goto exponent;
}
if (i3==46){i2=i; goto decimal_point;}//nb. it can begin with a decimal point!
i3-=48;
if ((i3<0)||(i3>9)){valid=0; goto typechecked;}
digits_before_point++;
//nb. because leading 0 is handled differently, all digits are significant
significant_digits[num_significant_digits]=i3+48; num_significant_digits++;
}
goto assess_float;
////////////////////////////////
decimal_point:;
i4=1;
if (i2==(l-1)) {completewith=48; goto assess_float;}
i2++;
for (i=i2;i<l;i++){
i3=cp[i];
if ((i3==68)||(i3==(68+32))||(i3==69)||(i3==(69+32))){//d,D,e,E?
if (num_significant_digits){
if (i==i2){valid=0; goto typechecked;}//cannot begin with d,D,e,E just after a decimal point!
i2=i;
goto exponent;
}
}
i3-=48;
if ((i3<0)||(i3>9)){valid=0; goto typechecked;}
if (i3) i4=0;
if (i4) zeros_after_point++;
digits_after_point++;
if ((num_significant_digits)||i3){
significant_digits[num_significant_digits]=i3+48; num_significant_digits++;
}
}//i
goto assess_float;
////////////////////////////////
exponent:;

//ban d/D for SINGLE precision input
if ((qbs_input_variabletypes[argn]&511)==32){//SINGLE
i3=cp[i2];
if ((i3==68)||(i3==(68+32))){//d/D
valid=0; goto typechecked;
}
}
//correct "D" notation for c++ scanf
i3=cp[i2];
if ((i3==68)||(i3==(68+32))){//d/D
cp[i2]=69;//"E"
}
if (i2==(l-1)) {completewith=48; goto assess_float;}
i2++;
//check for optional + or -
i3=cp[i2];
if (i3==45){//"-"
if (i2==(l-1)) {completewith=48; goto assess_float;}
neg_power=1;
i2++;
}
if (i3==43){//"+"
if (i2==(l-1)) {completewith=48; goto assess_float;}
i2++;
}
//nothing valid after a leading 0
if (cp[i2]==48){//0
if (l>(i2+1)) {valid=0; goto typechecked;}
}
multiple=1;
value=0;
for (i=l-1;i>=i2;i--){
i3=cp[i]-48;
if ((i3>=0)&&(i3<=9)){

value+=multiple*i3;
}else{
valid=0; goto typechecked;
}
multiple*=10;
}//i
//////////////////////////
assess_float:;
//nb. 0.???? means digits_before_point==0

if ((qbs_input_variabletypes[argn]&511)==32){//SINGLE
//QB:           ±3.402823    E+38 to ±1.401298    E-45
//WIKIPEDIA:    ±3.4028234   E+38 to ?
//OTHER SOURCE: ±3.402823466 E+38 to ±1.175494351 E-38
if (neg_power) value=-value;
//special case->single 0 after point
if ((zeros_after_point==1)&&(digits_after_point==1)){
digits_after_point=0;
zeros_after_point=0;
}
//upper overflow check
//i. check that value doesn't consist solely of 0's
if (zeros_after_point>43){valid=0; goto typechecked;}//cannot go any further without reversal by exponent
if ((digits_before_point==0)&&(digits_after_point==zeros_after_point)) goto nooverflow_float;
//ii. calculate the position of the first WHOLE digit (in i)
i=digits_before_point;
if (!i) i=-zeros_after_point;
/*EXAMPLES:
1.0			i=1
12.0		i=2
0.1			i=0
0.01		i=-1
*/
i=i+value;//apply exponent
if (i>39){valid=0; goto typechecked;}
//nb. the above blocks the ability to type a long-int32 number and use a neg exponent
//    to validate it
//********IMPORTANT: if i==39 then the first 7 digits MUST be scanned!!!
if (i==39){
cp2=(uint8*)single_max[0];
i2=num_significant_digits;
if (i2>7) i2=7;
for (i3=0;i3<i2;i3++){
if (significant_digits[i3]>*cp2){valid=0; goto typechecked;}
if (significant_digits[i3]<*cp2) break;
cp2++;
}
}
//check for pointless levels of precision (eg. 1.21351273512653625116212!)
if (digits_after_point){
if (digits_before_point){
if ((digits_after_point+digits_before_point)>8){valid=0; goto typechecked;}
}else{
if ((digits_after_point-zeros_after_point)>8){valid=0; goto typechecked;}
}
}
//check for "under-flow"
if (i<-44){valid=0; goto typechecked;}
//********IMPORTANT: if i==-44 then the first 7 digits MUST be scanned!!!
if (i==-44){
cp2=(uint8*)single_max_neg[0];
i2=num_significant_digits;
if (i2>7) i2=7;
for (i3=0;i3<i2;i3++){
if (significant_digits[i3]<*cp2){valid=0; goto typechecked;}
if (significant_digits[i3]>*cp2) break;
cp2++;
}
}
nooverflow_float:;
c->chr[0]=0; qbs_set(qbs_input_arguements[argn],qbs_add(qbs_input_arguements[argn],c));
sscanf((char*)qbs_input_arguements[argn]->chr,"%f",(float*)qbs_input_variableoffsets[argn]);
goto typechecked;
}

if ((qbs_input_variabletypes[argn]&511)==64){//DOUBLE
//QB: Double (15-digit) precision ±1.7976931 D+308 to ±4.940656 D-324
//WIKIPEDIA:    ±1.7976931348623157 D+308 to ???
//OTHER SOURCE: ±1.7976931348623157 D+308 to ±2.2250738585072014E-308



if (neg_power) value=-value;
//special case->single 0 after point
if ((zeros_after_point==1)&&(digits_after_point==1)){
digits_after_point=0;
zeros_after_point=0;
}
//upper overflow check
//i. check that value doesn't consist solely of 0's
if (zeros_after_point>322){valid=0; goto typechecked;}//cannot go any further without reversal by exponent
if ((digits_before_point==0)&&(digits_after_point==zeros_after_point)) goto nooverflow_double;
//ii. calculate the position of the first WHOLE digit (in i)
i=digits_before_point;
if (!i) i=-zeros_after_point;
i=i+value;//apply exponent
if (i>309){valid=0; goto typechecked;}
//nb. the above blocks the ability to type a long-int32 number and use a neg exponent
//    to validate it
//********IMPORTANT: if i==309 then the first 8 digits MUST be scanned!!!
if (i==309){
cp2=(uint8*)double_max[0];
i2=num_significant_digits;
if (i2>8) i2=8;
for (i3=0;i3<i2;i3++){
if (significant_digits[i3]>*cp2){valid=0; goto typechecked;}
if (significant_digits[i3]<*cp2) break;
cp2++;
}
}
//check for pointless levels of precision (eg. 1.21351273512653625116212!)
if (digits_after_point){
if (digits_before_point){
if ((digits_after_point+digits_before_point)>16){valid=0; goto typechecked;}
}else{
if ((digits_after_point-zeros_after_point)>16){valid=0; goto typechecked;}
}
}
//check for "under-flow"
if (i<-323){valid=0; goto typechecked;}
//********IMPORTANT: if i==-323 then the first 7 digits MUST be scanned!!!
if (i==-323){
cp2=(uint8*)double_max_neg[0];
i2=num_significant_digits;
if (i2>7) i2=7;
for (i3=0;i3<i2;i3++){
if (significant_digits[i3]<*cp2){valid=0; goto typechecked;}
if (significant_digits[i3]>*cp2) break;
cp2++;
}
}
nooverflow_double:;
c->chr[0]=0; qbs_set(qbs_input_arguements[argn],qbs_add(qbs_input_arguements[argn],c));
sscanf((char*)qbs_input_arguements[argn]->chr,"%lf",(double*)qbs_input_variableoffsets[argn]);
goto typechecked;
}

if ((qbs_input_variabletypes[argn]&511)==256){//FLOAT
//at present, there is no defined limit for FLOAT type numbers, so no restrictions
//are applied!
c->chr[0]=0; qbs_set(qbs_input_arguements[argn],qbs_add(qbs_input_arguements[argn],c));

//sscanf((char*)qbs_input_arguements[argn]->chr,"%lf",(long double*)qbs_input_variableoffsets[argn]);
static double sscanf_fix;
sscanf((char*)qbs_input_arguements[argn]->chr,"%lf",&sscanf_fix);
*(long double*)qbs_input_variableoffsets[argn]=sscanf_fix;

}


}//ISFLOAT

//undefined/uncheckable types fall through as valid!
typechecked:;
if (!valid) goto backspace;



qbs_set(inpstr2,inpstr);


//input a key



qbs_input_invalidinput:

static int32 showing_cursor;
showing_cursor=0;

qbs_input_wait_for_key:

//toggle box cursor
if (!write_page->text){
i=1;
if ((write_page->font>=32)||(write_page->compatible_mode==256)||(write_page->compatible_mode==32)){
if (SDL_GetTicks()&512) i=0;
}
if (i!=showing_cursor){
showing_cursor^=1;
static int32 x,y,x2,y2,fx,fy,alpha,cw;
static uint32 c;
alpha=write_page->alpha_disabled; write_page->alpha_disabled=1;
fy=fontheight[write_page->font];
fx=fontwidth[write_page->font]; if (!fx) fx=1;
cw=fx; if ((write_page->font>=32)||(write_page->compatible_mode==256)||(write_page->compatible_mode==32)) cw=1;
y2=(write_page->cursor_y-1)*fy;
for (y=0;y<fy;y++){
x2=(write_page->cursor_x-1)*fx;
for (x=0;x<cw;x++){
pset (x2,y2,point(x2,y2)^write_page->color);
x2++;
}
y2++;
}
write_page->alpha_disabled=alpha;
}
}//!write_page->text

if (addspaces){
addspaces--;
c->chr[0]=32; qbs_set(key,c);
}else{
SDL_Delay(10);
qbs_set(key,qbs_inkey());
qbs_cleanup(qbs_tmp_base,0);
}
if (stop_program) return;
if (key->len!=1) goto qbs_input_wait_for_key;

//remove box cursor
if (!write_page->text){
if (showing_cursor){
showing_cursor=0;
static int32 x,y,x2,y2,fx,fy,cw,alpha;
static uint32 c;
alpha=write_page->alpha_disabled; write_page->alpha_disabled=1;
fy=fontheight[write_page->font];
fx=fontwidth[write_page->font]; if (!fx) fx=1;
cw=fx; if ((write_page->font>=32)||(write_page->compatible_mode==256)||(write_page->compatible_mode==32)) cw=1;
y2=(write_page->cursor_y-1)*fy;
for (y=0;y<fy;y++){
x2=(write_page->cursor_x-1)*fx;
for (x=0;x<cw;x++){
pset (x2,y2,point(x2,y2)^write_page->color);
x2++;
}
y2++;
}
write_page->alpha_disabled=alpha;
}
}//!write_page->text

//input should disallow certain characters
if (key->chr[0]==7) {qbs_print(key,0); goto qbs_input_next;}//beep!
if (key->chr[0]==10) goto qbs_input_next;//linefeed
if (key->chr[0]==9){//tab
i=8-(inpstr2->len&7);
addspaces=i;
goto qbs_input_next;
}
//other ASCII chars that cannot be printed
if (key->chr[0]==11) goto qbs_input_next;
if (key->chr[0]==12) goto qbs_input_next;
if (key->chr[0]==28) goto qbs_input_next;
if (key->chr[0]==29) goto qbs_input_next;
if (key->chr[0]==30) goto qbs_input_next;
if (key->chr[0]==31) goto qbs_input_next;

if (key->chr[0]==13){
//assume input is valid

//autofinish (if necessary)

//assume all parts entered

for (i=1;i<=numvariables;i++){
qbs_free(qbs_input_arguements[i]);
}

if (newline){
c->len=0;
qbs_print(c,1);
}
qbs_free(c);
qbs_free(inpstr);
qbs_free(inpstr2);
qbs_free(key);

write_page->cursor_show=cursor_show_last;

sub__freeimage(dest_image,1); sub__freeimage(dest_image_temp,1);

if (autodisplay_backup==0){
autodisplay=-1;//toggle request
while(autodisplay) Sleep(1);
}

sub__source(source_backup);

return;
}

if (key->chr[0]==8){//backspace
backspace:
if (!inpstr->len) goto qbs_input_invalidinput;
inpstr->len--;
i2=func__dest();//backup current dest
sub_pcopy(dest_image,dest_image_temp);//copy original background to temp
//write characters to temp
sub__dest(dest_image_temp);
write_page->cursor_x=dest_image_cursor_x; write_page->cursor_y=dest_image_cursor_y; write_page->holding_cursor=dest_image_holding_cursor;
for (i=0;i<inpstr->len;i++){key->chr[0]=inpstr->chr[i]; qbs_print(key,0);}
sub__dest(i2);
//copy temp to dest
sub_pcopy(dest_image_temp,i2);
//update cursor
write_page->cursor_x=img[-dest_image_temp].cursor_x; write_page->cursor_y=img[-dest_image_temp].cursor_y;
goto qbs_input_next;
}

if (inpstr2->len>=255) goto qbs_input_next;

//affect inpstr2 with key
qbs_set(inpstr2,qbs_add(inpstr2,key));

//perform actual update
qbs_print(key,0);

qbs_set(inpstr,inpstr2);

goto qbs_input_next;

}//qbs_input

long double func_val(qbs *s){
static int32 i,i2,step,c,num_significant_digits,most_significant_digit_position;
static int32 num_exponent_digits;
static int32 negate,negate_exponent;
static uint8 significant_digits[256];
static int64 exponent_value;
static uint8 built_number[256];
static long double return_value;
static int64 value;
static int64 hex_value;
static int32 hex_digits;
if (!s->len) return 0;
value=0;
negate_exponent=0;
num_exponent_digits=0;
num_significant_digits=0;
most_significant_digit_position=0;
step=0;
exponent_value=0;
negate=0;

i=0;
for (i=0;i<s->len;i++){
c=s->chr[i];

if ((c==32)||(c==9)) goto whitespace;

if (c==38){//&
if (step==0) goto hex;
goto finish;
}

if (c==45){//-
if (step==0){negate=1; step=1; goto checked;}
if (step==3){negate_exponent=1; step=4; goto checked;}
goto finish;
}

if (c==43){//+
if (step==0){step=1; goto checked;}
if (step==3){step=4; goto checked;}
goto finish;
}

if ((c>=48)&&(c<=57)){//0-9

if (step<=1){//before decimal point
step=1;
if ((num_significant_digits)||(c>48)){
most_significant_digit_position++;
significant_digits[num_significant_digits]=c;
num_significant_digits++;
value=value*10+c-48;
}
}

if (step==2){//after decimal point
if ((num_significant_digits==0)&&(c==48)) most_significant_digit_position--;
if ((num_significant_digits)||(c>48)){
significant_digits[num_significant_digits]=c;
num_significant_digits++;
}
}

if (step>=3){//exponent
step=4;
if ((num_exponent_digits)||(c>48)){
if (num_exponent_digits>=18) goto finish;
exponent_value*=10; exponent_value=exponent_value+c-48;//precalculate
num_exponent_digits++;
}
}

goto checked;
}

if (c==46){//.
if (step>1) goto finish;
step=2; goto checked;
}

if ((c==68)||(c==69)||(c==100)||(c==101)){//D,E,d,e
if (step>2) goto finish;
step=3; goto checked;
}

goto finish;//invalid character
checked:
whitespace:;
}
finish:;

if (num_significant_digits==0) return 0;

if (exponent_value==0){
if (num_significant_digits==most_significant_digit_position){
if (negate) value=-value;
return value;
}
}

if (negate_exponent) exponent_value=-exponent_value;
i=0;
if (negate) {built_number[i]=45; i++;}//-
if (num_significant_digits){
 for (i2=0;i2<num_significant_digits;i2++){
 if (i2==1){built_number[i]=46; i++;}
 built_number[i]=significant_digits[i2]; i++;
 }
 built_number[i]=69; i++;//E
 //adjust exponent value appropriately
 //if most_significant_digit_position=1, then no change need occur
 exponent_value=exponent_value+most_significant_digit_position-1;
 //add exponent's value
 #ifdef QB64_WINDOWS
  i2=sprintf((char*)&built_number[i],"%I64i",exponent_value);
 #else
  i2=sprintf((char*)&built_number[i],"%lli",exponent_value);
 #endif
 i=i+i2;
}else{
 built_number[i]=48; i++;//0
}
built_number[i]=0;//NULL terminate

#ifdef QB64_MINGW
__mingw_sscanf((char*)&built_number[0],"%Lf",&return_value);
#else 
sscanf((char*)&built_number[0],"%Lf",&return_value);
#endif
return return_value;

hex://hex/oct
if (i>=(s->len-2)) return 0;
c=s->chr[i+1];
if ((c==79)||(c==111)){//"O"or"o"
 hex_digits=0;
 hex_value=0;
 for (i=i+2;i<s->len;i++){
 c=s->chr[i];
 if ((c>=48)&&(c<=55)){//0-7
 c-=48;
 hex_value<<=3;
 hex_value|=c;
 if (hex_digits||c) hex_digits++; 
  if (hex_digits>=22){
  if ((hex_digits>22)||(s->chr[i-21]>49)){error(6); return 0;}
  }
 }else break;
 }//i
 return hex_value;
}
if ((c==72)||(c==104)){//"H"or"h"
 hex_digits=0;
 hex_value=0;
 for (i=i+2;i<s->len;i++){
 c=s->chr[i];
 if ( ((c>=48)&&(c<=57)) || ((c>=65)&&(c<=70)) || ((c>=97)&&(c<=102)) ){//0-9 or A-F or a-f
 if ((c>=48)&&(c<=57)) c-=48;
 if ((c>=65)&&(c<=70)) c-=55;

 if ((c>=97)&&(c<=102)) c-=87;
 hex_value<<=4;
 hex_value|=c;
 if (hex_digits||c) hex_digits++;
 if (hex_digits>16) {error(6); return 0;}
 }else break;
 }//i
 return hex_value;
}
return 0;//& followied by unknown
}






int32 unsupported_port_accessed=0;

int32 H3C7_palette_register_read_index=0;
int32 H3C8_palette_register_index=0;
int32 H3C9_next=0;
int32 H3C9_read_next=0;

void sub_out(int32 port,int32 data){
if (new_error) return;
unsupported_port_accessed=0;
port=port&65535;
data=data&255;

if (port==0x3C7){//&H3C7, set palette register read index
H3C7_palette_register_read_index=data;
H3C9_read_next=0;
goto done;
}
if (port==968){//&H3C8, set palette register write index
H3C8_palette_register_index=data;
H3C9_next=0;
goto done;
}
if (port==969){//&H3C9, set palette color
data=data&63;
if (write_page->pal){//avoid NULL pointer
if (H3C9_next==0){//red
write_page->pal[H3C8_palette_register_index]&=0xFF00FFFF;
write_page->pal[H3C8_palette_register_index]+=(qbr((double)data*4.063492f-0.4999999f)<<16);
}
if (H3C9_next==1){//green
write_page->pal[H3C8_palette_register_index]&=0xFFFF00FF;
write_page->pal[H3C8_palette_register_index]+=(qbr((double)data*4.063492f-0.4999999f)<<8);
}
if (H3C9_next==2){//blue
write_page->pal[H3C8_palette_register_index]&=0xFFFFFF00;
write_page->pal[H3C8_palette_register_index]+=qbr((double)data*4.063492f-0.4999999f);
}
}
H3C9_next=H3C9_next+1;
if (H3C9_next==3){
H3C9_next=0;
H3C8_palette_register_index=H3C8_palette_register_index+1;
H3C8_palette_register_index&=0xFF;
}
goto done;
}

unsupported_port_accessed=1;
done:
return;
error:
error(5);
}

uint32 rnd_seed=327680;
uint32 rnd_seed_first=327680;//Note: must contain the same value as rnd_seed

void sub_randomize (double seed,int32 passed){
if (new_error) return;

if (passed==3){//USING
//Dim As Uinteger m = cptr(Uinteger Ptr, @n)[1]
static uint32 m;
m=((uint32*)&seed)[1];
//m Xor= (m Shr 16)
m^=(m>>16);
//rnd_seed = (m And &hffff) Shl 8 Or (rnd_seed And &hff)
rnd_seed=((m&0xffff)<<8)|(rnd_seed_first&0xff);//Note: rnd_seed changed to rnd_seed_first
return;
}

if (passed==1){
//Dim As Uinteger m = cptr(Uinteger Ptr, @n)[1]
static uint32 m;
m=((uint32*)&seed)[1];
//m Xor= (m Shr 16)
m^=(m>>16);
//rnd_seed = (m And &hffff) Shl 8 Or (rnd_seed And &hff)
rnd_seed=((m&0xffff)<<8)|(rnd_seed&0xff);
return;
}

qbs_print(qbs_new_txt("Random-number seed (-32768 to 32767)? "),0);
static int16 integerseed;
qbs_input_variabletypes[1]=16;//id.t=16 'a signed 16 bit integer
qbs_input_variableoffsets[1]=&integerseed;
qbs_input(1,1);
//rnd_seed = (m And &hffff) Shl 8 Or (rnd_seed And &hff) 'nb. same as above
rnd_seed=((integerseed&0xffff)<<8)|(rnd_seed&0xff);
return;
}

float func_rnd(float n,int32 passed){
if (new_error) return 0;

static uint32 m;
if (!passed) n=1.0f;
if (n!=0.0){
if (n<0.0){
m=*((uint32*)&n);
rnd_seed=(m&0xFFFFFF)+((m&0xFF000000)>>24);
}
rnd_seed=(rnd_seed*16598013+12820163)&0xFFFFFF;
}     
return (double)rnd_seed/0x1000000;
}

double func_timer(double accuracy,int32 passed){
if (new_error) return 0;
static uint32 x;
static double d;
static float f;
x=SDL_GetTicks();
x-=sdl_firsttimervalue;
x+=qb64_firsttimervalue;
//make timer value loop after midnight
//note: there are 86400000 milliseconds in 24hrs(1 day)
x%=86400000;
d=x;//convert to double
d/=1000.0;//convert from ms to sec
//reduce accuracy
if (!passed){
accuracy=18.2;
}else{
if (accuracy<=0.0){error(5); return 0;}
accuracy=1.0/accuracy;
}
d*=accuracy;
d=qbr(d);
d/=accuracy;
if (!passed){f=d; d=f;}
return d;
}

void sub__limit(double fps){
if (new_error) return;

static double prev=0,ms,now,x;

if (fps<=0.0){error(5); return;}
ms=1000.0/fps;
if (ms>60000.0){error(5); return;}//max. 1 min delay between frames allowed to avoid accidental lock-up of program

now=SDL_GetTicks();

if (prev==0.0){//first call?
prev=now;
return;
}
if (now<prev){//value looped?
prev=now;
return;
}

//calculate wait time required in ms
x=now-prev;//elapsed ms since last call
if (x<ms){//delay required
	Sleep(ms-x+1);//+1 added to avoid rounding down which erodes accuracy leading to overspeed
	prev=prev+ms;
}else{//took too long, adjust prev to current time
//note: a minor overshoot of up to 16ms is recoverable
if (x<=(ms+16.0)) prev=prev+ms; else prev=now;
}

}


void sub_sound(double frequency,double lengthinclockticks){
sndsetup();
if (new_error) return;
//note: there are 18.2 clock ticks per second
if ((frequency<37.0)&&(frequency!=0)) goto error;
if (frequency>32767.0) goto error;
if (lengthinclockticks<0.0) goto error;
if (lengthinclockticks>65535.0) goto error;
if (lengthinclockticks==0.0) return;
qb64_generatesound(frequency,lengthinclockticks/18.2,1);
return;
error:
error(5);
}



int32 generic_put(int32 i,int32 offset,uint8 *cp,int32 bytes){
//note: generic_put & generic_get have been made largely redundant by gfs_read & gfs_write
//      offset is a byte-offset from base 0 (-1=current pos)
//      generic_put has been kept 32-bit for compatibility
//      the return value of generic_put is always 0
//      though errors are handled, generic_put should only be called in error-less situations
if (new_error) return 0;
if (gfs_fileno_valid(i)!=1){error(52); return 0;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if (!gfs->write){error(75); return 0;}//Path/file access error
static int32 e;
e=gfs_write(i,offset,(uint8*)cp,bytes);
if (e){
if (e==-2){error(258); return 0;}//invalid handle
if (e==-3){error(54); return 0;}//bad file mode
if (e==-4){error(5); return 0;}//illegal function call
if (e==-7){error(70); return 0;}//permission denied
error(75); return 0;//assume[-9]: path/file access error
}
return 0;
}

int32 generic_get_bytes_read;
int32 generic_get(int32 i,int32 offset,uint8 *cp,int32 bytes){
//note: generic_put & generic_get have been made largely redundant by gfs_read & gfs_write
//      offset is a byte-offset from base 0 (-1=current pos)
//      generic_get has been kept 32-bit for compatibility
//      the return value of generic_get is always 0
//      though errors are handled, generic_get should only be called in error-less situations
generic_get_bytes_read=0;
if (new_error) return 0;
if (gfs_fileno_valid(i)!=1){error(52); return 0;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if (!gfs->read){error(75); return 0;}//Path/file access error
static int32 e;
e=gfs_read(i,offset,(uint8*)cp,bytes);
generic_get_bytes_read=gfs_read_bytes();
if (e){
if (e!=-10){//note: on eof, unread buffer area becomes NULL
if (e==-2){error(258); return 0;}//invalid handle
if (e==-3){error(54); return 0;}//bad file mode
if (e==-4){error(5); return 0;}//illegal function call
if (e==-7){error(70); return 0;}//permission denied
error(75); return 0;//assume[-9]: path/file access error
}
}
return 0;
}



void sub_open(qbs *name,int32 type,int32 access,int32 sharing,int32 i,int64 record_length,int32 passed){
if (new_error) return;
//?[{FOR RANDOM|FOR BINARY|FOR INPUT|FOR OUTPUT|FOR APPEND}]
//1 2
//[{ACCESS READ WRITE|ACCESS READ|ACCESS WRITE}]
//  3
//[{SHARED|LOCK READ WRITE|LOCK READ|LOCK WRITE}]{AS}[#]?[{LEN =}?]
//  4                                                   5        6[1]
static int32 x;
static int32 g_access,g_restrictions,g_how;

if (type==0) type=1;
if (passed) if ((record_length==0)||(record_length<-1)){error(5); return;}//Illegal function call
//note: valid record_length values are allowable but ignored by QB for non-RANDOM modes too!

x=gfs_fileno_valid(i);
if (x==-2){error(52); return;}//Bad file name or number
if (x==1){error(55); return;}//File already open

if (type<=2){g_access=3; g_restrictions=0; g_how=3;}
if (type==3){g_access=1; g_restrictions=0; g_how=0;}
if (type==4){g_access=2; g_restrictions=0; g_how=2;}
if (type==5){g_access=2; g_restrictions=0; g_how=1;}

if (access==1) g_access=3;
if (access==2) g_access=1;
if (access==3) g_access=2;
if (access&&(g_how==3)) g_how=1;//undefined access not possible when ACCESS is explicitly specified

if (sharing==1) g_restrictions=0;
if (sharing==2) g_restrictions=3;
if (sharing==3) g_restrictions=1;
if (sharing==4) g_restrictions=2;
//note: In QB, opening a file already open for OUTPUT/APPEND created the 'file already open' error.

//      However, from a new cmd window (or a SHELLed QB program) it can be opened!
//      So it is not a true OS restriction/lock, just a block applied internally by QB.
//		This is currently unsupported by QB64.

x=gfs_open(name,g_access,g_restrictions,g_how);
if (x<0){
if (x==-5){error(53); return;}
if (x==-6){error(76); return;}
if (x==-7){error(70); return;}
if (x==-8){error(68); return;}
if (x==-11){error(64); return;}
error(53); return;//default assumption: 'file not found'
}

gfs_fileno_use(i,x);

static gfs_file_struct *f; f=&gfs_file[x];

f->type=type; if (type==5) f->type=4;

f->column=1;

if (type==1){//set record length
f->record_length=128;
if (passed) if (record_length!=-1) f->record_length=record_length;
f->field_buffer=(uint8*)calloc(record_length,1);
}

if (type==5){//seek eof
static int64 x64;
x64=gfs_lof(x);
if (x64>0) gfs_setpos(x,x64);//not an error and not null length
}

if (type==3){//check if eof character, CHR$(26), is the first byte and set EOF accordingly
static int64 x64;
x64=gfs_lof(x);
 if (x64){
 //read first byte 
 static uint8 c;
 static int32 e;
 if (e=gfs_read(x,-1,&c,1)){
 //if (e==-10) return -1;
 //if (e==-2){error(258); return -2;}//invalid handle
 //if (e==-3){error(54); return -2;}//bad file mode
 //if (e==-4){error(5); return -2;}//illegal function call
 if (e==-7){error(70); return;}//permission denied
 error(75); return;//assume[-9]: path/file access error
 }
 if (c==26){
 gfs_file[x].eof_passed=1;//set EOF flag
 }
 gfs_setpos(x,0);
 }
}//type==3

}

void sub_open_gwbasic(qbs *typestr,int32 i,qbs *name,int64 record_length,int32 passed){
if (new_error) return;
static int32 a,type;
if (!typestr->len){error(54); return;}//bad file mode
a=typestr->chr[0]&223;
type=0;
if (a==82) type=1;//R
if (a==66) type=2;//B
if (a==73) type=3;//I
if (a==79) type=4;//O
if (a==65) type=5;//A
if (!type){error(54); return;}//bad file mode
if (passed){
sub_open(name,type,NULL,NULL,i,record_length,1);
}else{
sub_open(name,type,NULL,NULL,i,NULL,0);
}
}


void sub_close(int32 i2,int32 passed){
if (new_error) return;
int32 i;

if (passed){

//tcp/ip
if (i2<0){
i=-(i2+1);
if ((i>=0)&&(i<sfh_bufsize)){
if ((sfh[i].type>=1)&&(sfh[i].type<=3)){	
if (sfh[i].type!=1){
SDLNet_TCP_DelSocket(net_tcp[i].set,net_tcp[i].socket);
SDLNet_FreeSocketSet(net_tcp[i].set);
}
SDLNet_TCP_Close(net_tcp[i].socket);
if (net_tcp[i].buffer) free(net_tcp[i].buffer);
sfh_free(i);
}//types 1-3
}
return;
}//i2<0

if (gfs_fileno_valid(i2)==1) gfs_close(gfs_fileno[i2]);
return;

}//passed

//tcp/ip
for (i=0;i<sfh_bufsize;i++){
 if ((sfh[i].type>=1)&&(sfh[i].type<=3)) sub_close(-i-1,1);
}

for (i=1;i<=gfs_fileno_n;i++){
 if (gfs_fileno_valid(i)==1) gfs_close(gfs_fileno[i]);
}

}//close

int32 file_input_chr(int32 i){
//returns the ASCII value of the character (0-255)
//returns -1 if eof reached (error to be externally handled)
//returns -2 for other errors (internally handled), the calling function should abort

if (i<0){//TCP/IP feed buffer
if (tcp_feed_offset>=tcp_feed_ucbufsiz) return -1;
return tcp_feed_ucbuf[tcp_feed_offset++];
}

static uint8 c;
static int32 e;
if (e=gfs_read(i,-1,&c,1)){
if (e==-10) return -1;
if (e==-2){error(258); return -2;}//invalid handle
if (e==-3){error(54); return -2;}//bad file mode
if (e==-4){error(5); return -2;}//illegal function call
if (e==-7){error(70); return -2;}//permission denied
error(75); return -2;//assume[-9]: path/file access error
}
if (c==26){//eof character (go back 1 byte so subsequent reads will re-encounter the eof character)
gfs_setpos(i,gfs_getpos(i)-1);
gfs_file[i].eof_passed=1;//also set EOF flag
return -1;
}
return c;

}

void file_input_skip1310(int32 i,int32 c){
//assumes a character of value 13 or 10 has just been read (passed)
//peeks next character and skips it too if it is a corresponding 13 or 10 pair
static int32 nextc;
nextc=file_input_chr(i);
if (nextc==-2) return;
if (nextc==-1) return;
if (((c==10)&&(nextc!=13))||((c==13)&&(nextc!=10))){
gfs_setpos(i,gfs_getpos(i)-1);//go back 1 character
}else{
//check next character for EOF CHR$(26)
nextc=file_input_chr(i);
if (nextc==-2) return;
if (nextc==-1) return;
gfs_setpos(i,gfs_getpos(i)-1);//go back 1 character
}
}

void file_input_nextitem(int32 i,int32 lastc){
if (i<0) return;
//this may require reversing a bit too!
int32 c,nextc;
c=lastc;
nextchr:
if (c==-1) return;
if (c==32){
nextc=file_input_chr(i); if (nextc==-2) return;
if (nextc==-1) return;
if ( (nextc!=32)&&(nextc!=44)&&(nextc!=10)&&(nextc!=13) ){
gfs_setpos(i,gfs_getpos(i)-1);
return;
}else{
c=nextc;
goto nextchr;
}
}
if (c==44) return;//,
if ((c==10)||(c==13)){//lf cr
file_input_skip1310(i,c);
return;
}
c=file_input_chr(i); if (c==-2) return;
goto nextchr;
}

uint8 sub_file_print_spaces[32];
void sub_file_print(int32 i,qbs *str,int32 extraspace,int32 tab,int32 newline){
if (new_error) return;
static int32 x,x2,x3,x4;
static int32 e;

//tcp/ip?
//note: spacing considerations such as 'extraspace' & 'tab' are ignored
if (i<0){
x=-(i+1);
if (x>=sfh_bufsize){error(52); return;}
if ((sfh[x].type!=2)&&(sfh[x].type!=3)){error(52); return;}
//valid tcp/ip connection
if (net_tcp[x].error) return;
//obselete message check
if ((newline==0)&&(str->len==0)) return;
//send formatted data
static uint8 header_byte;
x2=str->len; if (extraspace) x2++;
if (x2<=4){header_byte=x2; x3=0;}
if ((x2>4)&&(x2<=255)){header_byte=5; x3=1;}
if ((x2>255)&&(x2<=65535)){header_byte=6; x3=2;}
if (x2>65535){header_byte=7; x3=4;}
if (!newline){
if (!tab){
header_byte|=128;
}
}
//build message
static uint8 *cp;
cp=(uint8*)malloc(1+4+x2+16);//***fix
*cp=header_byte;
*((uint32*)(&cp[1]))=x2;
memcpy(&cp[1+x3],str->chr,str->len);
if (extraspace) cp[1+x3+x2-1]=32;

x4=SDLNet_TCP_Send(net_tcp[x].socket,cp,1+x3+x2);
free(cp);
//Returns: the number of bytes sent. If the number returned is less than len, then an error occured, such as the client disconnecting. 
if (x4!=(1+x3+x2)) net_tcp[x].error=1;

return;
}


if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if (gfs->type!=4){error(54); return;}//Bad file mode
if (!gfs->write){error(75); return;}//Path/file access error

e=gfs_write(i,-1,str->chr,str->len);
if (e){
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}

//move column if carriage return found in content
static int32 stri,strl;
static uint8 c;
strl=str->len;
for (stri=0;stri<strl;stri++){
c=str->chr[stri];
 if ((c!=13)&&(c!=10)){
  gfs->column++;
 }else{
  if (c==13) gfs->column=1;
 }
}

//add extra spaces as needed
static int32 nspaces;
static int16 cr_lf=13+10*256; 
nspaces=0;
if (extraspace){
nspaces++;
gfs->column++;
}
if (tab){
//a space MUST be added
nspaces++;
gfs->column++;
x=(gfs->column-1)%14;
if (x!=0){
x=14-x;
nspaces+=x;
gfs->column+=x;
}
}
if (nspaces){

e=gfs_write(i,-1,sub_file_print_spaces,nspaces);
if (e){
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}

}
if (newline){

e=gfs_write(i,-1,(uint8*)&cr_lf,2);
if (e){
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}

gfs->column=1;
}
}

//number limits
const char *range_int64_max[]={"9223372036854775807"};//19 digits
const char *range_int64_neg_max[]={"9223372036854775808"};//19 digits
const char *range_uint64_max[]={"18446744073709551615"};//20 digits
const char *range_float_max[]=    {"17976931348623157"};//17 digits
                          
//universal number representation
uint16 n_digits;
uint8 n_digit[256];
int64 n_exp;//if 0, there is one digit in front of the decimal place
uint8 n_neg;//if 1, the number is negative
uint8 n_hex;//if 1, the digits are in hexidecimal and n_exp should be ignored
                    //if 2, the digits are in octal and n_exp should be ignored
                    //(consider revising variable name n_hex)

int32 n_roundincrement(){
static int32 i,i2,i3;
if (n_digits==0) return 0;
if (n_digits>(n_exp+1)){//numbers exist after the decimal point
i=n_digit[n_exp+1]-48;
if (i>=5) return 1;
}
return 0;
}

long double n_float_value;
int32 n_float(){
//return value: Bit 0=successful
//data
static uint8 built[256];
static int64 value;
uint64 uvalue;
static int32 i,i2,i3;
static uint8 *max;
max=(uint8*)range_float_max[0];
n_float_value=0; value=0; uvalue=0;
if (n_digits==0) return 1;
//hex?
if (n_hex==1){
if (n_digits>16) return 0;
for (i=0;i<n_digits;i++){
 i2=n_digit[i];
 if ((i2>=48)&&(i2<=57)) i2-=48;
 if ((i2>=65)&&(i2<=70)) i2-=55;
 if ((i2>=97)&&(i2<=102)) i2-=87;
 value<<=4;
 value|=i2;
}
n_float_value=value;
return 1;
}
//oct?
if (n_hex==2){
if (n_digits>=22){
if ((n_digits>22)||(n_digit[0]>49)) return 0;
}
for (i=0;i<n_digits;i++){
 i2=n_digit[i]-48;
 value<<=3;
 value|=i2;
}
n_float_value=value;
return 1;
}

//max range check (+-1.7976931348623157E308)
if (n_exp>308)return 0;//overflow
if (n_exp==308){
 i2=n_digits; if (i2>17) i2=17;
 for (i=0;i<i2;i++){
  if (n_digit[i]>max[i]) return 0;//overflow
  if (n_digit[i]<max[i]) break;
 }
}
//too close to 0?
if (n_exp<-324) return 1;
//read & return value (via C++ function)
//build number
i=0;
if (n_neg){built[i]=45; i++;}//-
built[i]=n_digit[0]; i++;
built[i]=46; i++;//.
if (n_digits==1){
built[i]=48; i++;//0
}else{
 i3=n_digits; if (i3>17) i3=17;
 for (i2=1;i2<i3;i2++){
 built[i]=n_digit[i2]; i++;
 }
}
built[i]=69; i++;//E
#ifdef QB64_WINDOWS
i2=sprintf((char*)&built[i],"%I64i",n_exp);
#else
i2=sprintf((char*)&built[i],"%lli",n_exp);
#endif
i=i+i2;
built[i]=0;//NULL terminate for sscanf

static double sscanf_fix;
sscanf((char*)&built[0],"%lf",&sscanf_fix);
n_float_value=sscanf_fix;

return 1;
}

int64 n_int64_value;
int32 n_int64(){
//return value: Bit 0=successful
//data
static int64 value;
uint64 uvalue;
static int32 i,i2;
static uint8 *max; static uint8 *neg_max;
static int64 v0=build_int64(0x80000000,0x00000000);
static int64 v1=build_int64(0x7FFFFFFF,0xFFFFFFFF);
max=(uint8*)range_int64_max[0]; neg_max=(uint8*)range_int64_neg_max[0];
n_int64_value=0; value=0; uvalue=0;
if (n_digits==0) return 1;
//hex
if (n_hex==1){
if (n_digits>16) return 0;
for (i=0;i<n_digits;i++){
 i2=n_digit[i];
 if ((i2>=48)&&(i2<=57)) i2-=48;
 if ((i2>=65)&&(i2<=70)) i2-=55;
 if ((i2>=97)&&(i2<=102)) i2-=87;
 value<<=4;
 value|=i2;
}
n_int64_value=value;
return 1;
}
//oct

if (n_hex==2){
if (n_digits>=22){

if ((n_digits>22)||(n_digit[0]>49)) return 0;
}
for (i=0;i<n_digits;i++){
 i2=n_digit[i]-48;
 value<<=3;
 value|=i2;
}
n_int64_value=value;
return 1;
}

//range check: int64 (-9,223,372,036,854,775,808 to 9,223,372,036,854,775,807)
if (n_exp>18)return 0;//overflow
if (n_exp==18){
i2=n_digits; if (i2>19) i2=19;//only scan integeral digits
 for (i=0;i<i2;i++){
  if (n_neg){
  if (n_digit[i]>neg_max[i]) return 0;//overflow
  if (n_digit[i]<neg_max[i]) break;
  }else{
  if (n_digit[i]>max[i]) return 0;//overflow
  if (n_digit[i]<max[i]) break;
  }
 }
}
//calculate integeral value
i2=n_digits; if (i2>(n_exp+1)) i2=n_exp+1;
for (i=0;i<(n_exp+1);i++){
uvalue*=10;
if (i<i2) uvalue=uvalue+(n_digit[i]-48);
}
if (n_neg){
value=-uvalue;
}else{
value=uvalue;
}
//apply rounding
if (n_roundincrement()){
if (n_neg){
if (value==v0) return 0;
value--;
}else{
if (value==v1) return 0;
value++;
}
}
//return value
n_int64_value=value;
return 1;
}

uint64 n_uint64_value;
int32 n_uint64(){
//return value: Bit 0=successful
//data
static int64 value;
uint64 uvalue;
static int32 i,i2;
static uint8 *max;
static int64 v0=build_uint64(0xFFFFFFFF,0xFFFFFFFF);
max=(uint8*)range_uint64_max[0];
n_uint64_value=0; value=0; uvalue=0;
if (n_digits==0) return 1;
//hex
if (n_hex==1){
if (n_digits>16) return 0;
for (i=0;i<n_digits;i++){
 i2=n_digit[i];
 if ((i2>=48)&&(i2<=57)) i2-=48;
 if ((i2>=65)&&(i2<=70)) i2-=55;
 if ((i2>=97)&&(i2<=102)) i2-=87;
 uvalue<<=4;
 uvalue|=i2;
}
n_uint64_value=uvalue;
return 1;
}
//oct
if (n_hex==2){
if (n_digits>=22){
if ((n_digits>22)||(n_digit[0]>49)) return 0;
}
for (i=0;i<n_digits;i++){
 i2=n_digit[i]-48;
 uvalue<<=3;
 uvalue|=i2;
}
n_uint64_value=uvalue;
return 1;
}

//negative?
if (n_neg){
if (n_exp>=0) return 0;//cannot return a negative number!
}
//range check: int64 (0 to 18446744073709551615)
if (n_exp>19)return 0;//overflow
if (n_exp==19){
i2=n_digits; if (i2>20) i2=20;//only scan integeral digits
 for (i=0;i<i2;i++){ 
 if (n_digit[i]>max[i]) return 0;//overflow
 if (n_digit[i]<max[i]) break; 
 }
}
//calculate integeral value
i2=n_digits; if (i2>(n_exp+1)) i2=n_exp+1;
for (i=0;i<(n_exp+1);i++){
uvalue*=10;
if (i<i2) uvalue=uvalue+(n_digit[i]-48);
}
//apply rounding
if (n_roundincrement()){
if (n_neg){
return 0;
}else{
if (uvalue==v0) return 0;
uvalue++;
}
}
//return value
n_uint64_value=uvalue;
return 1;
}

int32 n_inputnumberfromdata(uint8 *data,ptrszint *data_offset,ptrszint data_size){
//return values:
//0=success!
//1=Overflow
//2=Out of DATA
//3=Syntax error
//note: when read fails the data_offset MUST be restored to its old position

//data
static int32 i,i2;
static int32 step,c;
static int32 exponent_digits;
static uint8 negate_exponent;
static int64 exponent_value;
static int32 return_value;

return_value=1;//overflow (default)
step=0;
negate_exponent=0;
exponent_value=0;
exponent_digits=0;

//prepare universal number representation
n_digits=0; n_exp=0; n_neg=0; n_hex=0;

//Out of DATA?
if (*data_offset>=data_size) return 2;

//read character
c=data[*data_offset];

//hex/oct
if (c==38){//&
(*data_offset)++; if (*data_offset>=data_size) goto gotnumber;
c=data[*data_offset];
if (c==44){(*data_offset)++; goto gotnumber;}
if ((c==72)||(c==104)){//"H"or"h"
 nexthexchr:
 (*data_offset)++; if (*data_offset>=data_size) goto gotnumber;
 c=data[*data_offset];
 if (c==44){(*data_offset)++; goto gotnumber;}
 if ( ((c>=48)&&(c<=57)) || ((c>=65)&&(c<=70)) || ((c>=97)&&(c<=102)) ){//0-9 or A-F or a-f
 if (n_digits==256) return 1;//Overflow
 n_digit[n_digits]=c;
 n_digits++;
 n_hex=1;
 goto nexthexchr;
 }
 return 3;//Syntax error
}
if ((c==79)||(c==111)){//"O"or"o"
 nexthexchr2:
 (*data_offset)++; if (*data_offset>=data_size) goto gotnumber;
 c=data[*data_offset];
 if (c==44){(*data_offset)++; goto gotnumber;}
 if ((c>=48)&&(c<=55)){//0-7
 if (n_digits==256) return 1;//Overflow
 n_digit[n_digits]=c;
 n_digits++;
 n_hex=2;
 goto nexthexchr2;
 }
 return 3;//Syntax error
}
return 3;//Syntax error
}//&

readnextchr:
if (c==44){(*data_offset)++; goto gotnumber;}

if (c==45){//-
if (step==0){n_neg=1; step=1; goto nextchr;}
if (step==3){negate_exponent=1; step=4; goto nextchr;}
return 3;//Syntax error
}

if (c==43){//+
if (step==0){step=1; goto nextchr;}
if (step==3){step=4; goto nextchr;}
return 3;//Syntax error
}

if ((c>=48)&&(c<=57)){//0-9

if (step<=1){//before decimal point
step=1;
if (n_digits||(c>48)){
if (n_digits) n_exp++;
if (n_digits==256) return 1;//Overflow
n_digit[n_digits]=c;
n_digits++;
}
}

if (step==2){//after decimal point
if ((n_digits==0)&&(c==48)) n_exp--;
if ((n_digits)||(c>48)){
if (n_digits==256) return 1;//Overflow
n_digit[n_digits]=c;
n_digits++;
}
}

if (step>=3){//exponent
step=4;
if ((exponent_digits)||(c>48)){
if (exponent_digits==18) return 1;//Overflow
exponent_value*=10;
exponent_value=exponent_value+(c-48);
exponent_digits++;
}
}

goto nextchr;
}

if (c==46){//.
if (step>1) return 3;//Syntax error
if (n_digits==0) n_exp=-1;
step=2; goto nextchr;
}

if ((c==68)||(c==69)||(c==100)||(c==101)){//D,E,d,e
if (step>2) return 3;//Syntax error
step=3; goto nextchr;
}

return 3;//Syntax error
nextchr:
(*data_offset)++; if (*data_offset>=data_size) goto gotnumber;
c=data[*data_offset];
goto readnextchr;

gotnumber:;
if (negate_exponent) n_exp-=exponent_value; else n_exp+=exponent_value;//complete exponent
if (n_digits==0) {n_exp=0; n_neg=0;}//clarify number
return 0;//success
}





















int32 n_inputnumberfromfile(int32 fileno){
//return values:
//0=success
//1=overflow
//2=eof
//3=failed (no further errors)

//data
static int32 i,i2;
static int32 step,c;
static int32 exponent_digits;
static uint8 negate_exponent;
static int64 exponent_value;
static int32 return_value;

//tcp/ip specific data
static qbs *str,*character;
int32 nextc,x,x2,x3,x4;
int32 i1;
int32 inspeechmarks;
static uint8 *ucbuf;
static uint32 ucbufsiz;
static int32 info;

//tcp/ip?
if (fileno<0){
x=-(fileno+1);
if (x>=sfh_bufsize){error(52); return 3;}
if ((sfh[x].type!=2)&&(sfh[x].type!=3)){error(52); return 3;}
//valid tcp/ip connection
revert_input_x=x;
if (net_tcp[x].eof==2) return 3;
if (net_tcp[x].error){net_tcp[x].eof=2; return 3;}
net_tcp_updatebuffer(x);
x2=net_tcp[x].buffer_size; if (!x2){net_tcp[x].eof=2; return 3;}
i=0;
ucbuf=NULL;
ucbufsiz=0;
append_message:
x3=net_tcp[x].buffer[i];
info=x3;
i++;
if (x3&120){
if (ucbufsiz) free(ucbuf);
//deststr->len=0;
net_tcp[x].eof=2;
return 3;
}//invalid bits set [01111000]
x3&=7;
/*
0 0 byte message
1 1 byte message
2 2 byte message
3 3 byte message
4 4 byte message
5 1 byte length descriptor + ? byte message
6 2 byte length descriptor + ? byte message
7 4 byte length descriptor + ? byte message
*/
if (x3<=4) x4=0;
if (x3==5) x4=1;
if (x3==6) x4=2;
if (x3==7) x4=4;
if (x4){
if (x2<(i+x4)){
if (ucbufsiz) free(ucbuf);
net_tcp[x].eof=2;
return 3;
}//not enough data (for length descriptor)
if (x4==1) x3=*((uint8*)(net_tcp[x].buffer+i));
if (x4==2) x3=*((uint16*)(net_tcp[x].buffer+i));
if (x4==4) x3=*((uint32*)(net_tcp[x].buffer+i));
i+=x4;
}
if (x2<(i+x3)){
if (ucbufsiz) free(ucbuf);
net_tcp[x].eof=2;
return 3;
}//not enough data (for message)
//add message to buffer ucbuf
if (!ucbufsiz) ucbuf=(uint8*)malloc(x3); else ucbuf=(uint8*)realloc(ucbuf,ucbufsiz+x3);
memcpy(ucbuf+ucbufsiz,net_tcp[x].buffer+i,x3);
ucbufsiz+=x3;
i+=x3;
if (info&128) goto append_message; //append another message?
//share the 'feed' buffer
tcp_feed_ucbuf=ucbuf;
tcp_feed_ucbufsiz=ucbufsiz;
tcp_feed_offset=0;
x4=i;
}

if (fileno>=0){
if (gfs_fileno_valid(fileno)!=1){error(52); return 3;}//Bad file name or number
fileno=gfs_fileno[fileno];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[fileno];
if (gfs->type!=3){error(54); return 3;}//Bad file mode
if (!gfs->read){error(75); return 3;}//Path/file access error
}

return_value=1;//overflow (default)
step=0;
negate_exponent=0;
exponent_value=0;
exponent_digits=0;

//prepare universal number representation
n_digits=0; n_exp=0; n_neg=0; n_hex=0;

//skip any leading spaces
do{
c=file_input_chr(fileno); if (c==-2) return 3;
if (c==-1){return_value=2; goto error;}//input past end of file
}while(c==32);

//hex/oct
if (c==38){//&
c=file_input_chr(fileno); if (c==-2) return 3;
if (c==-1) goto gotnumber;
if ((c==72)||(c==104)){//"H"or"h"
 nexthexchr:
 c=file_input_chr(fileno); if (c==-2) return 3;
 if ( ((c>=48)&&(c<=57)) || ((c>=65)&&(c<=70)) || ((c>=97)&&(c<=102)) ){//0-9 or A-F or a-f
 if (n_digits==256) goto error;//overflow
 n_digit[n_digits]=c;
 n_digits++;
 n_hex=1;
 goto nexthexchr;
 }
 goto gotnumber;
}
if ((c==79)||(c==111)){//"O"or"o"
 nexthexchr2:
 c=file_input_chr(fileno); if (c==-2) return 3;
 if ((c>=48)&&(c<=55)){//0-7
 if (n_digits==256) goto error;//overflow
 n_digit[n_digits]=c;
 n_digits++;
 n_hex=2;
 goto nexthexchr2;
 }
 goto gotnumber;
}
goto gotnumber;
}//&

readnextchr:
if (c==-1) goto gotnumber;

if (c==45){//-
if (step==0){n_neg=1; step=1; goto nextchr;}
if (step==3){negate_exponent=1; step=4; goto nextchr;}
goto gotnumber;
}

if (c==43){//+
if (step==0){step=1; goto nextchr;}
if (step==3){step=4; goto nextchr;}
goto gotnumber;
}

if ((c>=48)&&(c<=57)){//0-9

if (step<=1){//before decimal point
step=1;
if (n_digits||(c>48)){
if (n_digits) n_exp++;
if (n_digits==256) goto error;//overflow
n_digit[n_digits]=c;
n_digits++;
}
}

if (step==2){//after decimal point
if ((n_digits==0)&&(c==48)) n_exp--;
if ((n_digits)||(c>48)){
if (n_digits==256) goto error;//overflow
n_digit[n_digits]=c;
n_digits++;
}
}

if (step>=3){//exponent
step=4;
if ((exponent_digits)||(c>48)){
if (exponent_digits==18) goto error;//overflow
exponent_value*=10;
exponent_value=exponent_value+(c-48);
exponent_digits++;
}
}

goto nextchr;
}

if (c==46){//.
if (step>1) goto gotnumber;
if (n_digits==0) n_exp=-1;
step=2; goto nextchr;
}

if ((c==68)||(c==69)||(c==100)||(c==101)){//D,E,d,e
if (step>2) goto gotnumber;
step=3; goto nextchr;
}

goto gotnumber;//invalid character
nextchr:
c=file_input_chr(fileno); if (c==-2) return 3;
goto readnextchr;

gotnumber:;
if (negate_exponent) n_exp-=exponent_value; else n_exp+=exponent_value;//complete exponent
if (n_digits==0) {n_exp=0; n_neg=0;}//clarify number
file_input_nextitem(fileno,c);
if (fileno<0){
	//adjust tcp[].buffer
		//revert_input_check (tag) add data that might need to be reverted to the buffer
		if (revert_input_bufsize==0){
		revert_input_buffer=(uint8*)malloc(x4);
		}else{
		revert_input_buffer=(uint8*)realloc(revert_input_buffer,revert_input_bufsize+x4);
		}
		memmove(revert_input_buffer+revert_input_bufsize,net_tcp[x].buffer,x4);
		revert_input_bufsize+=x4;
	if (x2-x4) memmove(net_tcp[x].buffer,net_tcp[x].buffer+x4,x2-x4);
	net_tcp[x].buffer_size-=x4;
	//free ucbuf
	free(ucbuf);
	//set eof to 0 if previous input has not failed (success)
	net_tcp[x].eof=0;
	}
return 0;//success

error:
file_input_nextitem(fileno,c);
	if (fileno<0){
	//free ucbuf
	free(ucbuf);
	//set eof
	net_tcp[x].eof=2;
	return 3;
	}
return return_value;
}

void sub_file_line_input_string(int32 fileno,qbs *deststr){
if (new_error) return;
static qbs *str,*character;
int32 c,nextc;
int32 inspeechmarks;

if (gfs_fileno_valid(fileno)!=1){error(52); return;}//Bad file name or number
fileno=gfs_fileno[fileno];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[fileno];
if (gfs->type!=3){error(54); return;}//Bad file mode
if (!gfs->read){error(75); return;}//Path/file access error

str=qbs_new(0,0);
c=file_input_chr(fileno); if (c==-2) return;
if (c==-1){
qbs_set(deststr,str);
qbs_free(str);
error(62);//input past end of file
return;
}
character=qbs_new(1,0);
nextchr:
if (c==-1) goto gotstr;
if (c==10) goto gotstr;
if (c==13) goto gotstr;
character->chr[0]=c; qbs_set(str,qbs_add(str,character));
c=file_input_chr(fileno); if (c==-2) return;
goto nextchr;
gotstr:
nextstr:
//scan until next item (or eof) reached
if (c==-1) goto returnstr;
if ((c==10)||(c==13)){//lf cr
file_input_skip1310(fileno,c);
goto returnstr;
}
c=file_input_chr(fileno); if (c==-2) return;
goto nextstr;
//return string
returnstr:
qbs_set(deststr,str);
qbs_free(str);
qbs_free(character);
return;
}

void revert_input_check(){
static uint32 x;
x=revert_input_x;
if (x==-1) return;
revert_input_x=-1;//reset for next time

if (net_tcp[x].eof==2){
//assign correct eof value
net_tcp[x].eof=1;
//revert tcp[x] buffer
if (revert_input_bufsize){
	//expand buffer?
	if ((net_tcp[x].buffer_size+revert_input_bufsize)>net_tcp[x].buffer_space){
	if (net_tcp[x].buffer==NULL){
	net_tcp[x].buffer=(uint8*)malloc(net_tcp[x].buffer_size+revert_input_bufsize);
	}else{
	net_tcp[x].buffer=(uint8*)realloc(net_tcp[x].buffer,net_tcp[x].buffer_size+revert_input_bufsize);
	}
	net_tcp[x].buffer_space=net_tcp[x].buffer_size+revert_input_bufsize;
	}
if (net_tcp[x].buffer_size){
memmove(net_tcp[x].buffer+revert_input_bufsize,net_tcp[x].buffer,net_tcp[x].buffer_size);
}
memcpy(net_tcp[x].buffer,revert_input_buffer,revert_input_bufsize);
net_tcp[x].buffer_size+=revert_input_bufsize;
}//bufsize>0
}//eof==2

//free the buffer
if (revert_input_bufsize){revert_input_bufsize=0; free(revert_input_buffer);}

}

void sub_file_input_string(int32 fileno,qbs *deststr){
if (new_error) return;
static qbs *str,*character;
int32 c,nextc,x,x2,x3,x4;
int32 i,i1;
int32 inspeechmarks;
static uint8 *ucbuf;
static uint32 ucbufsiz;
static int32 info;

//tcp/ip?
//note: spacing considerations are ignored
if (fileno<0){
x=-(fileno+1);
if (x>=sfh_bufsize){error(52); return;}
if ((sfh[x].type!=2)&&(sfh[x].type!=3)){error(52); return;}
//valid tcp/ip connection
revert_input_x=x;
if (net_tcp[x].eof==2) return;
if (net_tcp[x].error){net_tcp[x].eof=2; return;}

net_tcp_updatebuffer(x);


x2=net_tcp[x].buffer_size;
if (!x2){deststr->len=0; net_tcp[x].eof=2; return;}

i=0;
ucbuf=NULL;
ucbufsiz=0;
append_message:
x3=net_tcp[x].buffer[i];
info=x3;
i++;
if (x3&120){
if (ucbufsiz) free(ucbuf);
deststr->len=0;
net_tcp[x].eof=2;
return;
}//invalid bits set [01111000]
x3&=7;
/*
0 0 byte message
1 1 byte message
2 2 byte message
3 3 byte message
4 4 byte message
5 1 byte length descriptor + ? byte message
6 2 byte length descriptor + ? byte message
7 4 byte length descriptor + ? byte message
*/
if (x3<=4) x4=0;
if (x3==5) x4=1;
if (x3==6) x4=2;
if (x3==7) x4=4;
if (x4){
if (x2<(i+x4)){
if (ucbufsiz) free(ucbuf);
deststr->len=0;
net_tcp[x].eof=2;
return;
}//not enough data (for length descriptor)
if (x4==1) x3=*((uint8*)(net_tcp[x].buffer+i));
if (x4==2) x3=*((uint16*)(net_tcp[x].buffer+i));
if (x4==4) x3=*((uint32*)(net_tcp[x].buffer+i));
i+=x4;
}
if (x2<(i+x3)){
if (ucbufsiz) free(ucbuf);
deststr->len=0;
net_tcp[x].eof=2;
return;
}//not enough data (for message)
//add message to buffer ucbuf
if (!ucbufsiz) ucbuf=(uint8*)malloc(x3); else ucbuf=(uint8*)realloc(ucbuf,ucbufsiz+x3);
memcpy(ucbuf+ucbufsiz,net_tcp[x].buffer+i,x3);
ucbufsiz+=x3;
i+=x3;
if (info&128) goto append_message; //append another message?
//adjust tcp[].buffer
	//revert_input_check (tag) add data that might need to be reverted to the buffer
	if (revert_input_bufsize==0){
	revert_input_buffer=(uint8*)malloc(i);
	}else{
	revert_input_buffer=(uint8*)realloc(revert_input_buffer,revert_input_bufsize+i);
	}
	memmove(revert_input_buffer+revert_input_bufsize,net_tcp[x].buffer,i);
	revert_input_bufsize+=i;
if (x2-i) memmove(net_tcp[x].buffer,net_tcp[x].buffer+i,x2-i);
net_tcp[x].buffer_size-=i;

//set return string
str=qbs_new(ucbufsiz,0);
memcpy(str->chr,ucbuf,ucbufsiz);
qbs_set(deststr,str);
free(ucbuf); //free ucbuf
net_tcp[x].eof=0;
return;
}

if (gfs_fileno_valid(fileno)!=1){error(52); return;}//Bad file name or number
fileno=gfs_fileno[fileno];//convert fileno to gfs index

static gfs_file_struct *gfs;
gfs=&gfs_file[fileno];
if (gfs->type!=3){error(54); return;}//Bad file mode
if (!gfs->read){error(75); return;}//Path/file access error

str=qbs_new(0,0);
//skip whitespace (spaces & tabs)
do{
c=file_input_chr(fileno); if (c==-2) return;
if (c==-1){
qbs_set(deststr,str);
qbs_free(str);
error(62);//input past end of file
return;
}
}while((c==32)||(c==9));
//quoted string?
inspeechmarks=0;
if (c==34){//"
inspeechmarks=1;
c=file_input_chr(fileno);
}
//read string
character=qbs_new(1,0);
nextchr:
if (c==-2) return;
if (c==-1) goto gotstr;
if (inspeechmarks){
if (c==34) goto gotstr;//"
}else{
if (c==44) goto gotstr;//,
if (c==10) goto gotstr;
if (c==13) goto gotstr;
}
character->chr[0]=c; qbs_set(str,qbs_add(str,character));
c=file_input_chr(fileno);
goto nextchr;
gotstr:
//cull trailing whitespace
if (!inspeechmarks){
cullstr:
if (str->len){
if ((str->chr[str->len-1]==32)||(str->chr[str->len-1]==9)){str->len--; goto cullstr;}
}
}
nextstr:
//scan until next item (or eof) reached
if (c==-2) return;
if (c==-1) goto returnstr;
if (c==44) goto returnstr;
if ((c==10)||(c==13)){//lf cr
file_input_skip1310(fileno,c);
goto returnstr;
}
c=file_input_chr(fileno);
goto nextstr;
//return string
returnstr:
qbs_set(deststr,str);
qbs_free(str);
qbs_free(character);
return;
}

int64 func_file_input_int64(int32 fileno){
if (new_error) return 0;
static int32 i;
i=n_inputnumberfromfile(fileno);
if (i==1){error(6); return 0;}//overflow
if (i==2){error(62); return 0;}//input past end of file
if (i==3) return 0;//failed
if (n_int64()) return n_int64_value;
error(6);//overflow
return 0;
}

uint64 func_file_input_uint64(int32 fileno){
if (new_error) return 0;
static int32 i;
i=n_inputnumberfromfile(fileno);
if (i==1){error(6); return 0;}//overflow
if (i==2){error(62); return 0;}//input past end of file
if (i==3) return 0;//failed
if (n_uint64()) return n_uint64_value;
error(6);//overflow
return 0;
}




void sub_read_string(uint8 *data,ptrszint *data_offset,ptrszint data_size,qbs *deststr){
if (new_error) return;
static qbs *str,*character;
static int32 c,inspeechmarks;
str=qbs_new(0,0);
character=qbs_new(1,0);
inspeechmarks=0;

if (*data_offset>=data_size){error(4); goto gotstr;}//Out of DATA

c=data[*data_offset];
nextchr:

if (c==44){//,
if (inspeechmarks!=1){
(*data_offset)++;
goto gotstr;
}
}
if (inspeechmarks==2){error(4); str->len=0; goto gotstr;}//syntax error (expected , after closing " unless at end of data in which " is assumed by QB)

if (c==34){//"
if (inspeechmarks) {inspeechmarks=2; goto skipchr;}
if (!str->len){inspeechmarks=1; goto skipchr;}
}

character->chr[0]=c; qbs_set(str,qbs_add(str,character));
skipchr:

(*data_offset)++; if (*data_offset>=data_size) goto gotstr;
c=data[*data_offset];
goto nextchr;

gotstr:
qbs_set(deststr,str);
qbs_free(str);
qbs_free(character);
return;
}

long double func_read_float(uint8 *data,ptrszint *data_offset,ptrszint data_size,int32 typ){
if (new_error) return 0;
static int32 i;
static int64 maxval,minval;
static int64 value;
static ptrszint old_data_offset;
old_data_offset=*data_offset;
i=n_inputnumberfromdata(data,data_offset,data_size);


//return values:
//0=success!
//1=Overflow
//2=Out of DATA
//3=Syntax error
//note: when read fails the data_offset MUST be restored to its old position
if (i==1){//Overflow
goto overflow;
}
if (i==2){//Out of DATA
error(4);
return 0;
}
if (i==3){//Syntax error
*data_offset=old_data_offset;
error(2); 
return 0;
}



if (!n_float()) goto overflow; //returns n_float_value

//range test & return value
if (typ&ISFLOAT){
 if ((typ&511)>=64) return n_float_value;
 if (n_float_value>3.402823466E+38) goto overflow;
return n_float_value;
}else{
 if (n_float_value<(-(9.2233720368547758E+18)))goto overflow;//too low to store!
 if (n_float_value>   9.2233720368547758E+18)  goto overflow;//too high to store!
 value=qbr(n_float_value);
 if ((typ&ISUNSIGNED)||n_hex){
 maxval=(((int64)1)<<(typ&511))-1;
 minval=0;
 }else{
 maxval=(((int64)1)<<((typ&511)-1))-1;
 minval=-(((int64)1)<<((typ&511)-1));
 }
 if ((value>maxval)||(value<minval)) goto overflow;

 if (((typ&ISUNSIGNED)==0)&&n_hex){//signed hex/oct/...  
  if (  ( ((int64)1) << ((typ&511)-1) )  &value){//if top bit is set, set all bits above it to form a negative value
  value=(maxval^((int64)-1))+value;
  }
 }

 return value;
}

overflow:
*data_offset=old_data_offset;
error(6); 
return 0;
}

int64 func_read_int64(uint8 *data,ptrszint *data_offset,ptrszint data_size){
if (new_error) return 0;
static int32 i;
static ptrszint old_data_offset;
old_data_offset=*data_offset;
i=n_inputnumberfromdata(data,data_offset,data_size);
//return values:
//0=success!
//1=Overflow
//2=Out of DATA
//3=Syntax error
//note: when read fails the data_offset MUST be restored to its old position
if (i==1){//Overflow
goto overflow;
}
if (i==2){//Out of DATA
error(4);
return 0;
}
if (i==3){//Syntax error
*data_offset=old_data_offset;
error(2); 
return 0;
}
if (n_int64()) return n_int64_value;
overflow:
*data_offset=old_data_offset;
error(6); 
return 0;
}

uint64 func_read_uint64(uint8 *data,ptrszint *data_offset,ptrszint data_size){
if (new_error) return 0;
static int32 i;
static ptrszint old_data_offset;
old_data_offset=*data_offset;
i=n_inputnumberfromdata(data,data_offset,data_size);
//return values:
//0=success!
//1=Overflow
//2=Out of DATA
//3=Syntax error
//note: when read fails the data_offset MUST be restored to its old position
if (i==1){//Overflow
goto overflow;
}
if (i==2){//Out of DATA
error(4);
return 0;
}
if (i==3){//Syntax error
*data_offset=old_data_offset;
error(2); 
return 0;
}
if (n_uint64()) return n_uint64_value;
overflow:
*data_offset=old_data_offset;
error(6); 
return 0;
}

long double func_file_input_float(int32 fileno,int32 typ){
if (new_error) return 0;
static int32 i;
static int64 maxval,minval;
static int64 value;
i=n_inputnumberfromfile(fileno);
if (i==1){error(6); return 0;}//overflow
if (i==2){error(62); return 0;}//input past end of file
if (i==3) return 0;//failed
if (!n_float()){error(6); return 0;} //returns n_float_value
//range test & return value
if (typ&ISFLOAT){
 if ((typ&511)>=64) return n_float_value;
 if (n_float_value>3.402823466E+38){error(6); return 0;}
 return n_float_value;
}else{
 if (n_float_value<(-(9.2233720368547758E+18))){error(6); return 0;}//too low to store!
 if (n_float_value>   9.2233720368547758E+18)  {error(6); return 0;}//too high to store!
 value=qbr(n_float_value);
 if (typ&ISUNSIGNED){
 maxval=(((int64)1)<<(typ&511))-1;
 minval=0;
 }else{
 maxval=(((int64)1)<<((typ&511)-1))-1;
 minval=-(((int64)1)<<((typ&511)-1));
 }
 if ((value>maxval)||(value<minval)){error(6); return 0;}

 if (((typ&ISUNSIGNED)==0)&&n_hex){//signed hex/oct/...  
  if (  ( ((int64)1) << ((typ&511)-1) )  &value){//if top bit is set, set all bits above it to form a negative value
  value=(maxval^((int64)-1))+value;
  }
 }

 return value;
}
}//func_file_input_float

void *byte_element(uint64 offset,int32 length){
if (length<0) length=0;//some calculations may result in negative values which mean 0 (no bytes available)
//add structure to xms stack (byte_element structures are never stored in cmem!)
void *p;
if ((mem_static_pointer+=12)<mem_static_limit) p=mem_static_pointer-12; else p=mem_static_malloc(12);
*((uint64*)p)=offset;
((uint32*)p)[2]=length;
return p;
}

void *byte_element(uint64 offset,int32 length,byte_element_struct *info){
if (length<0) length=0;//some calculations may result in negative values which mean 0 (no bytes available)
info->length=length;
info->offset=offset;
return info;
}

void call_interrupt(int32 intno, void *inregs,void *outregs){
if (new_error) return;
static byte_element_struct *ele;
static uint16 *sp;
/* testing only
static qbs* s=NULL;
if (s==NULL) s=qbs_new(0,0);
qbs_set(s,qbs_str(ele->length));
MessageBox(NULL,(char*)s->chr,"CALL INTERRUPT: size",MB_OK|MB_SYSTEMMODAL);
qbs_set(s,qbs_str( ((uint8*)(ele->offset))[0] ));
MessageBox(NULL,(char*)s->chr,"CALL INTERRUPT: value",MB_OK|MB_SYSTEMMODAL);
*/
/* reference
TYPE RegType
   AX AS INTEGER
   BX AS INTEGER
   CX AS INTEGER
   DX AS INTEGER
   BP AS INTEGER
   SI AS INTEGER
   DI AS INTEGER
   FLAGS AS INTEGER
END TYPE
*/
//error checking
ele=(byte_element_struct*)outregs;
if (ele->length<16){error(5); return;}//Illegal function call
ele=(byte_element_struct*)inregs;
if (ele->length<16){error(5); return;}//Illegal function call
//load virtual registers
sp=(uint16*)(ele->offset);
cpu.ax=sp[0];
cpu.bx=sp[1];
cpu.cx=sp[2];
cpu.dx=sp[3];
cpu.bp=sp[4];
cpu.si=sp[5];
cpu.di=sp[6];
//note: flags ignored (revise)
call_int(intno);
//save virtual registers
ele=(byte_element_struct*)outregs;
sp=(uint16*)(ele->offset);
sp[0]=cpu.ax;
sp[1]=cpu.bx;
sp[2]=cpu.cx;
sp[3]=cpu.dx;
sp[4]=cpu.bp;
sp[5]=cpu.si;
sp[6]=cpu.di;
//note: flags ignored (revise)
return;
}

void call_interruptx(int32 intno, void *inregs,void *outregs){
if (new_error) return;
static byte_element_struct *ele;
static uint16 *sp;
/* reference
  TYPE RegTypeX
     AX AS INTEGER
     BX AS INTEGER
     CX AS INTEGER
     DX AS INTEGER
     BP AS INTEGER
     SI AS INTEGER
     DI AS INTEGER
     FLAGS AS INTEGER
     DS AS INTEGER
     ES AS INTEGER
  END TYPE
*/
//error checking
ele=(byte_element_struct*)outregs;
if (ele->length<20){error(5); return;}//Illegal function call
ele=(byte_element_struct*)inregs;
if (ele->length<20){error(5); return;}//Illegal function call
//load virtual registers
sp=(uint16*)(ele->offset);
cpu.ax=sp[0];
cpu.bx=sp[1];
cpu.cx=sp[2];
cpu.dx=sp[3];
cpu.bp=sp[4];
cpu.si=sp[5];
cpu.di=sp[6];
//note: flags ignored (revise)
cpu.ds=sp[8];
cpu.es=sp[9];
call_int(intno);
//save virtual registers
ele=(byte_element_struct*)outregs;
sp=(uint16*)(ele->offset);
sp[0]=cpu.ax;
sp[1]=cpu.bx;
sp[2]=cpu.cx;
sp[3]=cpu.dx;
sp[4]=cpu.bp;
sp[5]=cpu.si;
sp[6]=cpu.di;
//note: flags ignored (revise)
sp[8]=cpu.ds;
sp[9]=cpu.es;
return;
}

void sub_get(int32 i,int64 offset,void *element,int32 passed){
if (new_error) return;
static byte_element_struct *ele;
static int32 x,x2;

//tcp/ip?
if (i<0){
x=-(i+1);
if (x>=sfh_bufsize){error(52); return;}
if ((sfh[x].type!=2)&&(sfh[x].type!=3)){error(52); return;}
net_tcp_updatebuffer(x);
ele=(byte_element_struct*)element;
if (net_tcp[x].buffer_size<ele->length){net_tcp[x].eof=1; return;}
net_tcp[x].eof=0;
memcpy((void*)(ele->offset),net_tcp[x].buffer,ele->length);
x2=net_tcp[x].buffer_size-ele->length;
if (x2) memmove(net_tcp[x].buffer,net_tcp[x].buffer+ele->length,x2);
net_tcp[x].buffer_size-=ele->length;
return;
}

if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if (gfs->type>2){error(54); return;}//Bad file mode
if (!gfs->read){error(75); return;}//Path/file access error

ele=(byte_element_struct*)element;

if (gfs->type==1){//RANDOM
	if (ele->length>gfs->record_length){error(59); return;}//Bad record length
	if (passed){
		offset--;
		if (offset<0){error(63); return;}//Bad record number
		offset*=gfs->record_length;
	}else{
		offset=-1;
	}
}else{//BINARY
	if (passed){
	offset--;
	if (offset<0){error(63); return;}//Bad record number
    }else{offset=-1;}
}

static int32 e;

e=gfs_read(i,offset,(uint8*)ele->offset,ele->length);
if (e){
if (e!=-10){//note: on eof, unread buffer area becomes NULL
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}
}

//seek to beginning of next field
if (gfs->type==1){
if (e!=-10){//note: seek index not advanced if record did not exist
if (ele->length<gfs->record_length){
 if (offset!=-1){
 e=gfs_setpos(i,offset+gfs->record_length);
 }else{
 e=gfs_setpos(i,gfs_getpos(i)-ele->length+gfs->record_length);
 }
 if (e){error(54); return;}//assume[-3]: bad file mode
}
}//e!=-10
}

}//get

void sub_get2(int32 i,int64 offset,qbs *str,int32 passed){
if (new_error) return;
static int32 x,x2,x3,x4;

//tcp/ip?
if (i<0){
x=-(i+1);
if (x>=sfh_bufsize){error(52); return;}
if ((sfh[x].type!=2)&&(sfh[x].type!=3)){error(52); return;}
if (str->fixed){//following method is only for variable length strings
static byte_element_struct tbyte_element_struct;
sub_get(i,offset,byte_element((uint64)str->chr,str->len,&tbyte_element_struct),passed);
return;
}
net_tcp_updatebuffer(x);
static qbs* tqbs;
tqbs=qbs_new(net_tcp[x].buffer_size,1);
if (net_tcp[x].buffer_size){
memcpy(tqbs->chr,net_tcp[x].buffer,net_tcp[x].buffer_size);
}
net_tcp[x].buffer_size=0;
net_tcp[x].eof=0;
qbs_set(str,tqbs);
return;
}

if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if (gfs->type>2){error(54); return;}//Bad file mode
if (!gfs->read){error(75); return;}//Path/file access error

if (gfs->type==2){//BINARY (use normal sub_get)
static byte_element_struct tbyte_element_struct;
sub_get(gfs->fileno,offset,byte_element((uint64)str->chr,str->len,&tbyte_element_struct),passed);
return;
}

if (gfs->record_length<2){error(59); return;}//Bad record length

if (passed){
 offset--;
 if (offset<0){error(63); return;}//Bad record number
 offset*=gfs->record_length;
}else{
 offset=-1;
}

static int32 e;

static uint8 *data;
static uint64 l,bytes;
data=(uint8*)malloc(gfs->record_length);
e=gfs_read(i,offset,data,gfs->record_length);//read the whole record (including header & data)
if (e){
if (e!=-10){//note: on eof, unread buffer area becomes NULL
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}
}

bytes=gfs_read_bytes();//note: any unread part of the buffer is set to NULL (by gfs_read) and is treated as valid record data
if (!bytes){qbs_set(str,qbs_new(0,1)); free(data); return;}//as in QB when 0 bytes read, NULL string returned and (as no bytes read) no seek advancement

//seek to beginning of next field
//note: advancement occurs even if e==-10 (eof reached)
if (bytes<gfs->record_length){
 if (offset!=-1){
 e=gfs_setpos(i,offset+gfs->record_length);
 }else{
 e=gfs_setpos(i,gfs_getpos(i)-bytes+gfs->record_length);
 }
 if (e){error(54); free(data); return;}//assume[-3]: bad file mode
}

x=2;//offset where string data will be read from
l=((uint16*)data)[0];
if (l&32768){
 if (gfs->record_length<8){//record length is too small to read the length!
  //restore seek to original location
  if (offset!=-1){
  e=gfs_setpos(i,offset);
  }else{
  e=gfs_setpos(i,gfs_getpos(i)-gfs->record_length);
  }
 error(59); free(data); return;//Bad record length
 }
x=8;
l=(l&0x7FFF)+( ( (((uint64*)data)[0]) >> 16) << 15 );
}

//can record_length cannot fit the required string data?
if ((gfs->record_length-x2)<l){
 //restore seek to original location
 if (offset!=-1){
 e=gfs_setpos(i,offset);
 }else{
 e=gfs_setpos(i,gfs_getpos(i)-gfs->record_length);
 }
error(59); free(data); return;//Bad record length
}

qbs_set(str,qbs_new_txt_len((char*)(data+x),l));
free(data);
}

void sub_put(int32 i,int64 offset,void *element,int32 passed){
if (new_error) return;
static byte_element_struct *ele;
static int32 x,x2;

//tcp/ip?
if (i<0){
x=-(i+1);
if (x>=sfh_bufsize){error(52); return;}
if ((sfh[x].type!=2)&&(sfh[x].type!=3)){error(52); return;}
//valid tcp/ip connection
if (net_tcp[x].error) return;
//send unformatted data
ele=(byte_element_struct*)element;
x2=SDLNet_TCP_Send(net_tcp[x].socket,(void*)ele->offset,ele->length);
//Returns: the number of bytes sent. If the number returned is less than len, then an error occured, such as the client disconnecting. 
if (x2!=ele->length) net_tcp[x].error=1;
return;
}

if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if (gfs->type>2){error(54); return;}//Bad file mode
if (!gfs->write){error(75); return;}//Path/file access error

ele=(byte_element_struct*)element;

if (gfs->type==1){//RANDOM
	if (ele->length>gfs->record_length){error(59); return;}//Bad record length
	if (passed){
		offset--;
		if (offset<0){error(63); return;}//Bad record number
		offset*=gfs->record_length;
	}else{
		offset=-1;
	}
}else{//BINARY
	if (passed){
	offset--;
	if (offset<0){error(63); return;}//Bad record number
    }else{offset=-1;}
}

static int32 e;

e=gfs_write(i,offset,(uint8*)ele->offset,ele->length);
if (e){
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}

//seek to beginning of next field
if (gfs->type==1){
if (ele->length<gfs->record_length){
 if (offset!=-1){
 e=gfs_setpos(i,offset+gfs->record_length);
 }else{
 e=gfs_setpos(i,gfs_getpos(i)-ele->length+gfs->record_length);
 }
 if (e) error(54); return;//assume[-3]: bad file mode
}
}

}

//put2 adds a 2-4 byte length descriptor to the data
//(used to PUT variable length strings in RANDOM mode)
void sub_put2(int32 i,int64 offset,void *element,int32 passed){
if (new_error) return;
static byte_element_struct *ele;
static int32 x;
static uint8 *data;

//tcp/ip?
if (i<0){
x=-(i+1);
if (x>=sfh_bufsize){error(52); return;}
if ((sfh[x].type!=2)&&(sfh[x].type!=3)){error(52); return;}
//valid tcp/ip connection
sub_put(i,offset,element,passed);//(use standard put call)
return;
}

if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if (gfs->type>2){error(54); return;}//Bad file mode
if (!gfs->write){error(75); return;}//Path/file access error

if (gfs->type==2){//BINARY (use normal sub_put)
sub_put(gfs->fileno,offset,element,passed);
return;
}

//RANDOM
static uint64 l;
static int64 lmask;
lmask=-1;
lmask>>=16;
ele=(byte_element_struct*)element;
l=ele->length;//note: ele->length is currently 32-bit, but sub_put2 is 64-bit compliant
//{15}{1}[{48}]
if (l>32767){
data=(uint8*)malloc(l+8);
memcpy(&data[8],(void*)(ele->offset),l);
((uint64*)data)[0]=0;
((uint16*)data)[0]=(l&32767)+32768;
l=((l>>15)&lmask);
((uint64*)(data+2))[0]|=l;
ele->length+=8;
}else{
data=(uint8*)malloc(l+2);
memcpy(&data[2],(void*)(ele->offset),l);
((uint16*)data)[0]=l;
ele->length+=2;
}
ele->offset=(uint64)&data[0];
sub_put(gfs->fileno,offset,element,passed);
free(data);

}//put2






void sub_graphics_get(float x1f,float y1f,float x2f,float y2f,void *element,uint32 mask,int32 passed){
//"[{STEP}](?,?)-[{STEP}](?,?),?[,?]"
//   &1            &2            &4
if (new_error) return;

static int32 x1,y1,x2,y2,z,w,h,bits,x,y,bytes,sx,sy,x3,y3,z2;
static uint32 col,off,col1,col2,col3,col4,byte;

if (read_page->text){error(5); return;}

//change coordinates according to step
if (passed&1){x1f=read_page->x+x1f; y1f=read_page->y+y1f;}
read_page->x=x1f; read_page->y=y1f;
if (passed&2){x2f=read_page->x+x2f; y2f=read_page->y+y2f;}
read_page->x=x2f; read_page->y=y2f;

//resolve coordinates
if (read_page->clipping_or_scaling){
if (read_page->clipping_or_scaling==2){
x1=qbr_float_to_long(x1f*read_page->scaling_x+read_page->scaling_offset_x)+read_page->view_offset_x;
y1=qbr_float_to_long(y1f*read_page->scaling_y+read_page->scaling_offset_y)+read_page->view_offset_y;
x2=qbr_float_to_long(x2f*read_page->scaling_x+read_page->scaling_offset_x)+read_page->view_offset_x;
y2=qbr_float_to_long(y2f*read_page->scaling_y+read_page->scaling_offset_y)+read_page->view_offset_y;
}else{
x1=qbr_float_to_long(x1f)+read_page->view_offset_x; y1=qbr_float_to_long(y1f)+read_page->view_offset_y;
x2=qbr_float_to_long(x2f)+read_page->view_offset_x; y2=qbr_float_to_long(y2f)+read_page->view_offset_y;
}
}else{
x1=qbr_float_to_long(x1f); y1=qbr_float_to_long(y1f);
x2=qbr_float_to_long(x2f); y2=qbr_float_to_long(y2f);
}

//swap coordinates if reversed
if (x2<x1){z=x1; x1=x2; x2=z;}
if (y2<y1){z=y1; y1=y2; y2=z;}

sx=read_page->width; sy=read_page->height;

//boundry checking (if no mask colour was passed)
if (!(passed&4)){
if ((x1<0)||(y1<0)||(x2>=sx)||(y2>=sy)){error(5); return;}
}

static byte_element_struct *ele;
ele=(byte_element_struct*)element;
static uint16 *dimensions;
dimensions=(uint16*)(ele->offset);
static uint8 *cp,*cp1,*cp2,*cp3,*cp4;
cp=(uint8*)(ele->offset+4);
static uint32 *lp;
lp=(uint32*)(ele->offset+4);

w=x2-x1+1; h=y2-y1+1;
bits=read_page->bits_per_pixel;

if (bits==1){
mask&=1;
z=(w+7)>>3;
bytes=z*h+4;
if (bytes>ele->length){error(5); return;}
dimensions[0]=w; dimensions[1]=h;
for (y=y1;y<=y2;y++){
z2=128;
col2=0;
off=y*sx+x1;
for (x=x1;x<=x2;x++){
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)) col=read_page->offset[off]; else col=mask;
if (col) col2|=z2;
z2>>=1; if (!z2){z2=128; *cp++=col2; col2=0;}
off++;
}
if (z2!=128) *cp++=col2;
}
return;
}//1

if (bits==2){
mask&=3;
z=(w+7)>>3;
bytes=z*h+4;
if (bytes>ele->length){error(5); return;}
dimensions[0]=w*2; dimensions[1]=h;
for (y=y1;y<=y2;y++){
byte=0;
x3=0;
off=y*sx+x1;
for (x=x1;x<=x2;x++){
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)) col=read_page->offset[off]; else col=mask;
byte<<=2;
byte|=col;
if ((x3&3)==3){*cp++=byte; byte=0;}
x3++;
off++;
}
if (x3&3) *cp++=col2;
}
return;
}//2

if (bits==4){
mask&=15;
z=(w+7)>>3;
bytes=z*4*h+4;
if (bytes>ele->length){error(5); return;}
dimensions[0]=w; dimensions[1]=h;
y3=0;
for (y=y1;y<=y2;y++){
z2=128;
off=y*sx+x1;
cp1=cp+y3*z*4;
cp2=cp+y3*z*4+z;
cp3=cp+y3*z*4+z*2;
cp4=cp+y3*z*4+z*3;
col1=0; col2=0; col3=0; col4=0;
for (x=x1;x<=x2;x++){
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)) col=read_page->offset[off]; else col=mask;
if (col&1) col1|=z2;
if (col&2) col2|=z2;
if (col&4) col3|=z2;
if (col&8) col4|=z2;
z2>>=1; if (!z2){z2=128; *cp1++=col1; *cp2++=col2; *cp3++=col3; *cp4++=col4; col1=0; col2=0; col3=0; col4=0;}
off++;
}
if (z2!=128){*cp1=col1; *cp2=col2; *cp3=col3; *cp4=col4;}
y3++;
}
return;
}//4

if (bits==8){
mask&=255;
bytes=w*h+4;
if (bytes>ele->length){error(5); return;}
dimensions[0]=w*8; dimensions[1]=h;
for (y=y1;y<=y2;y++){
off=y*sx+x1;
for (x=x1;x<=x2;x++){
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)) col=read_page->offset[off]; else col=mask;
*cp++=col;
off++;
}}
return;
}//8

if (bits==32){
bytes=w*h*4+4;
if (bytes>ele->length){error(5); return;}
dimensions[0]=w; dimensions[1]=h;//note: width is left unmultiplied
for (y=y1;y<=y2;y++){
off=y*sx+x1;
for (x=x1;x<=x2;x++){
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)) col=read_page->offset32[off]; else col=mask;
*lp++=col;
off++;
}}
return;
}//32

}//sub_graphics_get



void sub_graphics_put(float x1f,float y1f,void *element,int32 option,uint32 mask,int32 passed){
//"[{STEP}](?,?),?[,[{_CLIP}][{PSET|PRESET|AND|OR|XOR}][,?]]"
//step->passed&1
//clip->passed&2
//mask->passed&4

if (new_error) return;

static int32 step,clip;
step=0;
clip=0;
if (passed&1){step=1; passed-=1;}
if (passed&2){clip=1; passed-=2;}

static int32 x1,y1,x2,y2,z,w,h,bits,x,y,bytes,sx,sy,x3,y3,z2;
static uint32 col,off,col1,col2,col3,col4,byte,pixelmask;

if (write_page->text){error(5); return;}

//change coordinates according to step
if (step){
x1f+=write_page->x; y1f+=write_page->y;
write_page->x=x1f; write_page->y=y1f;
}

//resolve coordinates
if (write_page->clipping_or_scaling){
if (write_page->clipping_or_scaling==2){
x1=qbr_float_to_long(x1f*write_page->scaling_x+write_page->scaling_offset_x)+write_page->view_offset_x;
y1=qbr_float_to_long(y1f*write_page->scaling_y+write_page->scaling_offset_y)+write_page->view_offset_y;
}else{
x1=qbr_float_to_long(x1f)+write_page->view_offset_x; y1=qbr_float_to_long(y1f)+write_page->view_offset_y;
}
}else{
x1=qbr_float_to_long(x1f); y1=qbr_float_to_long(y1f);
}

sx=write_page->width; sy=write_page->height;
bits=write_page->bits_per_pixel;

static byte_element_struct *ele;
ele=(byte_element_struct*)element;
static uint16 *dimensions;
dimensions=(uint16*)(ele->offset);
static uint8 *cp,*cp1,*cp2,*cp3,*cp4;
cp=(uint8*)(ele->offset+4);
static uint32 *lp;
lp=(uint32*)(ele->offset+4);

static uint8 *offp;
static uint32 *off32p;

if (4>ele->length){error(5); return;}

//get dimensions
w=dimensions[0]; h=dimensions[1];
z=w;//(used below)
if (bits==2){if (w&1){error(5); return;} else w>>=1;}
if (bits==8){if (w&7){error(5); return;} else w>>=3;}
x2=x1+w-1; y2=y1+h-1;

//boundry checking (if CLIP option was not used)
if (!clip){
if ((x1<0)||(y1<0)||(x2>=sx)||(y2>=sy)){error(5); return;}
}

//array size check (avoid reading unacclocated memory)
if (bits==32) z*=32;
z=(z+7)>>3;//bits per row->bytes per row
bytes=h*z;
if (bits==4) bytes*=4;
if ((bytes+4)>ele->length){error(5); return;}

pixelmask=write_page->mask;

if (bits==1){
mask&=1;
y3=0;
for (y=y1;y<=y2;y++){
offp=(uint8*)write_page->offset+(y*sx+x1);
x3=0;
for (x=x1;x<=x2;x++){
if (!(x3--)){x3=7; col2=*cp++;}
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)){
col=(col2>>x3)&1;
	if ((!passed)||(col!=mask)){
	switch(option){
	case 0: *offp^=col; break;
	case 1: *offp=col; break;
	case 2: *offp=(~col)&pixelmask; break;
	case 3: *offp&=col; break;
	case 4: *offp|=col; break;
	case 5: *offp^=col; break;
	}
	}//mask
}//bounds
offp++;
}
y3++;
}
return;
}//1

if (bits==2){
mask&=3;
y3=0;
for (y=y1;y<=y2;y++){
offp=(uint8*)write_page->offset+(y*sx+x1);
x3=0;
for (x=x1;x<=x2;x++){
if (!(x3--)){x3=3; col2=*cp++;}
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)){
col=(col2>>(x3<<1))&3;
	if ((!passed)||(col!=mask)){
	switch(option){
	case 0: *offp^=col; break;
	case 1: *offp=col; break;
	case 2: *offp=(~col)&pixelmask; break;
	case 3: *offp&=col; break;
	case 4: *offp|=col; break;
	case 5: *offp^=col; break;
	}
	}//mask
}//bounds
offp++;
}
y3++;
}
return;
}//2

if (bits==4){
mask&=15;
y3=0;
for (y=y1;y<=y2;y++){
offp=(uint8*)write_page->offset+(y*sx+x1);
cp1=cp+y3*z*4;
cp2=cp+y3*z*4+z;
cp3=cp+y3*z*4+z*2;
cp4=cp+y3*z*4+z*3;
x3=0;
for (x=x1;x<=x2;x++){
if (!(x3--)){x3=7; col1=*cp1++; col2=(*cp2++)<<1; col3=(*cp3++)<<2; col4=(*cp4++)<<3;}
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)){
col=((col1>>x3)&1)|((col2>>x3)&2)|((col3>>x3)&4)|((col4>>x3)&8);
	if ((!passed)||(col!=mask)){
	switch(option){
	case 0: *offp^=col; break;
	case 1: *offp=col; break;
	case 2: *offp=(~col)&pixelmask; break;
	case 3: *offp&=col; break;
	case 4: *offp|=col; break;
	case 5: *offp^=col; break;
	}
	}//mask
}//bounds
offp++;
}
y3++;
}
return;
}//4

if (bits==8){
mask&=255;
for (y=y1;y<=y2;y++){
offp=(uint8*)write_page->offset+(y*sx+x1);
for (x=x1;x<=x2;x++){
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)){
col=*cp;
	if ((!passed)||(col!=mask)){
	switch(option){
	case 0: *offp^=col; break;
	case 1: *offp=col; break;
	case 2: *offp=(~col)&pixelmask; break;
	case 3: *offp&=col; break;
	case 4: *offp|=col; break;
	case 5: *offp^=col; break;
	}
	}//mask
}//bounds
offp++;
cp++;
}}
return;
}//8

if (bits==32){
for (y=y1;y<=y2;y++){
off32p=(uint32*)write_page->offset32+(y*sx+x1);
for (x=x1;x<=x2;x++){
if ((x>=0)&&(y>=0)&&(x<sx)&&(y<sy)){
col=*lp;
	if ((!passed)||(col!=mask)){
	switch(option){
	case 0: *off32p^=col; break;
	case 1: *off32p=col; break;
	case 2: *off32p=(~col)&pixelmask; break;
	case 3: *off32p&=col; break;
	case 4: *off32p|=col; break;
	case 5: *off32p^=col; break;
	}
	}//mask
}//bounds
off32p++;
lp++;
}}
return;
}//32


















/*
static byte_element_struct *ele;
ele=(byte_element_struct*)element;
static int32 x,y;
static int32 sx,sy,c,px,py;
static uint8 *cp;
static int32 *lp;

sx=((uint16*)ele->offset)[0];
sy=((uint16*)ele->offset)[1];
cp=(uint8*)(ele->offset+4);
lp=(int32*)cp;


static int32 sizeinbytes;
static int32 byte;
static int32 bitvalue;
static int32 bytesperrow;
static int32 row2offset;
static int32 row3offset;
static int32 row4offset;

static int32 longval;

if (write_page->bits_per_pixel==8){
mask&=255;
//create error if not divisible by 8!
sx>>=3;
}

if (write_page->bits_per_pixel==1){
mask&=1;
}

if (write_page->bits_per_pixel==2){
mask&=3;
sx>>=1;
}


if (write_page->bits_per_pixel==4){
mask&=15;
bytesperrow=sx>>3; if (sx&7) bytesperrow++;
row2offset=bytesperrow;
row3offset=bytesperrow*2;
row4offset=bytesperrow*3;
}


for (y=0;y<sy;y++){
py=y1+y;

if (write_page->bits_per_pixel==4){
bitvalue=128;
byte=0;
}

for (x=0;x<sx;x++){
px=x1+x;


//get colour
if (write_page->bits_per_pixel==8){
 c=*cp;
 cp++;
}

if (write_page->bits_per_pixel==4){
byte=x>>3;
c=0;
if (cp[byte]&bitvalue) c|=1;
if (cp[row2offset+byte]&bitvalue) c|=2;
if (cp[row3offset+byte]&bitvalue) c|=4;
if (cp[row4offset+byte]&bitvalue) c|=8;
bitvalue>>=1; if (bitvalue==0) bitvalue=128;
}


if (write_page->bits_per_pixel==1){
 if (!(x&7)){
  byte=*cp;
  cp++;
 }
 c=(byte&128)>>7; byte<<=1;
}

if (write_page->bits_per_pixel==2){
 if (!(x&3)){
  byte=*cp;
  cp++;
 }
 c=(byte&192)>>6; byte<<=2;
}


if ((px>=0)&&(px<write_page->width)&&(py>=0)&&(py<write_page->height)){

//check color
if (passed){
if (c==mask) goto maskpixel;
}


//"pset" color

//PUT[{STEP}](?,?),?[,[{PSET|PRESET|AND|OR|XOR}][,[?]]]
//apply option

if (option==1){
write_page->offset[py*write_page->width+px]=c;
}
if (option==2){
//PRESET=bitwise NOT
write_page->offset[py*write_page->width+px]=(~c)&write_page->mask;
}
if (option==3){
write_page->offset[py*write_page->width+px]&=c;
}
if (option==4){
write_page->offset[py*write_page->width+px]|=c;
}
if ((option==5)||(option==0)){
write_page->offset[py*write_page->width+px]^=c;
}






}
maskpixel:;


}//x


if (write_page->bits_per_pixel==4) cp+=(bytesperrow*4);

//if (_bits_per_pixel==1){
// if (sx&7) cp++;
//}

//if (_bits_per_pixel==2){
// if (sx&3) cp++;
//}


}//y
*/

}



void sub_date(qbs* date){
if (new_error) return;
return;//stub
}

qbs *func_date(){
//mm-dd-yyyy
//0123456789
static time_t qb64_tm_val;
static tm *qb64_tm;
//struct tm {
//        int tm_sec;     /* seconds after the minute - [0,59] */
//        int tm_min;     /* minutes after the hour - [0,59] */
//        int tm_hour;    /* hours since midnight - [0,23] */
//        int tm_mday;    /* day of the month - [1,31] */
//        int tm_mon;     /* months since January - [0,11] */
//        int tm_year;    /* years since 1900 */
//        int tm_wday;    /* days since Sunday - [0,6] */
//        int tm_yday;    /* days since January 1 - [0,365] */
//        int tm_isdst;   /* daylight savings time flag */
//        };
static int32 x,x2,i;
static qbs *str;
str=qbs_new(10,1);
str->chr[2]=45; str->chr[5]=45;//-
time(&qb64_tm_val); if (qb64_tm_val==-1){error(5); str->len=0; return str;}
qb64_tm=localtime(&qb64_tm_val); if (qb64_tm==NULL){error(5); str->len=0; return str;}
x=qb64_tm->tm_mon; x++; 
i=0; str->chr[i]=x/10+48; str->chr[i+1]=x%10+48;
x=qb64_tm->tm_mday;
i=3; str->chr[i]=x/10+48; str->chr[i+1]=x%10+48;
x=qb64_tm->tm_year; x+=1900;
i=6;
x2=x/1000; x=x-x2*1000; str->chr[i]=x2+48; i++;
x2=x/100; x=x-x2*100; str->chr[i]=x2+48; i++;
x2=x/10; x=x-x2*10; str->chr[i]=x2+48; i++;
str->chr[i]=x+48;
return str;
}








void sub_time(qbs* str){
if (new_error) return;
return;//stub
}

qbs *func_time(){
//23:59:59 (hh:mm:ss)
//01234567
static time_t qb64_tm_val;
static tm *qb64_tm;
//struct tm {
//        int tm_sec;     /* seconds after the minute - [0,59] */
//        int tm_min;     /* minutes after the hour - [0,59] */
//        int tm_hour;    /* hours since midnight - [0,23] */
//        int tm_mday;    /* day of the month - [1,31] */
//        int tm_mon;     /* months since January - [0,11] */
//        int tm_year;    /* years since 1900 */
//        int tm_wday;    /* days since Sunday - [0,6] */
//        int tm_yday;    /* days since January 1 - [0,365] */
//        int tm_isdst;   /* daylight savings time flag */
//        };
static int32 x,x2,i;
static qbs *str;
str=qbs_new(8,1);
str->chr[2]=58; str->chr[5]=58;//:
time(&qb64_tm_val); if (qb64_tm_val==-1){error(5); str->len=0; return str;}
qb64_tm=localtime(&qb64_tm_val); if (qb64_tm==NULL){error(5); str->len=0; return str;}
x=qb64_tm->tm_hour;
i=0; str->chr[i]=x/10+48; str->chr[i+1]=x%10+48;
x=qb64_tm->tm_min;
i=3; str->chr[i]=x/10+48; str->chr[i+1]=x%10+48;
x=qb64_tm->tm_sec;
i=6; str->chr[i]=x/10+48; str->chr[i+1]=x%10+48;
return str;
}


int32 func_csrlin(){
if (write_page->holding_cursor){
if (write_page->cursor_y>=write_page->bottom_row) return write_page->bottom_row; else return write_page->cursor_y+1;
}
return write_page->cursor_y;
}
int32 func_pos(int32 ignore){
if (write_page->holding_cursor) return 1;
return write_page->cursor_x;
}



double func_log(double value){
if (value<=0){error(5);return 0;}
return log(value);
}

//FIX
double func_fix_double(double value){
if (value<0) return ceil(value); else return floor(value);
}
long double func_fix_float(long double value){
if (value<0) return ceil(value); else return floor(value);
}

//EXP
double func_exp_single(double value){
if (value<=88.02969){
return exp(value);
}
error(6); return 0;
}
long double func_exp_float(long double value){
if (value<=709.782712893){
return exp(value);
}
error(6); return 0;
}

int32 sleep_break=0;
void sub_sleep(int32 seconds,int32 passed){
if (new_error) return;
static uint64 start;
static uint64 milliseconds;
static uint64 stop;
static uint64 now;
sleep_break=0;

if (!passed) seconds=0;
if (seconds<=0){
while ((!sleep_break)&&(!stop_program)){
SDL_Delay(32);
}
return;
}

start=SDL_GetTicks();
milliseconds=seconds;
milliseconds*=1000;
stop=start+milliseconds;
if (stop>4294966295){error(6); return;}//cannot process wait time correctly!
wait:
if (sleep_break||stop_program) return;
now=SDL_GetTicks();
if (now<(stop-64)){//more than 64 milliseconds remain!
SDL_Delay(32);
goto wait;
}
if (now<stop) goto wait;
}


qbs *func_oct(int64 value,int32 neg_bits){

static int32 i,i2,i3,x,x2,neg;
static int64 value2;
static qbs *str;

str=qbs_new(22,1);

//negative?
if ((value>>63)&1) neg=1; else neg=0;

//calc. most significant bit
i2=0;
value2=value;
if (neg){
	for (i=1;i<=64;i++){
	if (!(value2&1)) i2=i;
	value2>>=1;
	}
	if (i2>=neg_bits){
	//doesn't fit in neg_bits, so expand to next 16/32/64 boundary
	i3=64;
	if (i2<32) i3=32;
	if (i2<16) i3=16;
	i2=i3;
	}else i2=neg_bits;
}else{
	for (i=1;i<=64;i++){
	if (value2&1) i2=i;
	value2>>=1;
	}
}

if (!i2){str->chr[0]=48; str->len=1; return str;}//"0"

//calc. number of characters required in i3
i3=i2/3; if ((i3*3)!=i2) i3++;

//build string
str->len=i3;
i3--;
x=0; x2=0;
for (i=1;i<=i2;i++){
if (value&1) x2|=(1<<x);
value>>=1;
x++;
if (x==3){str->chr[i3--]=x2+48; x2=0; x=0;}
}
if (x) str->chr[i3]=x2+48;

return str;

}

//note: QBASIC uses 8 characters for SINGLE/DOUBLE or generates "OVERFLOW" if this range is exceeded
//      QB64   uses 8 characters for SINGLE/DOUBLE/FLOAT but if this range is exceeded
//      it uses up to 16 characters before generating an "OVERFLOW" error
//performs overflow check before calling func_hex
qbs *func_oct_float(long double value){
static qbs *str;
static int64 ivalue;
static int64 uivalue;
//ref: uint64 0-18446744073709551615
//      int64 9223372036854775808 to 9223372036854775807
if ((value>=9.223372036854776E18)||(value<=-9.223372036854776E18)){
//note: ideally, the following line would be used, however, qbr_longdouble_to_uint64 just does the same as qbr
//if ((value>=1.844674407370956E19)||(value<=-9.223372036854776E18)){
str=qbs_new(0,1); error(6);//Overflow
return str;
}
if (value>=0){
uivalue=qbr_longdouble_to_uint64(value);
ivalue=uivalue;
}else{
ivalue=qbr(value);
}
return func_oct(ivalue,32);
}

qbs *func_hex(int64 value,int32 neg_size){
//note: negative int64 values can be treated as positive uint64 values (and vise versa)

static int32 i,i2,i3,x,neg;
static int64 value2;
static qbs *str;

str=qbs_new(16,1);

value2=value;
i2=0; i3=0;
for (i=1;i<=16;i++){
if (value2&15) i2=i;//most significant digit of positive value
if ((value2&15)!=15){
i3=i;//most significant digit of negative value
if ((((value2&8)==0)&&(i!=16))) i3++;//for a negative number to fit into 4/8 characters, its top bit must be on
}
x=value2&15; if (x>9) x+=55; else x+=48; str->chr[16-i]=x;
value2>>=4;
}
if (!i2){str->chr[0]=48; str->len=1; return str;}//"0"

//negative?
if ((value>>63)&1) neg=1; else neg=0;

//change i2 from sig-digits to string-output-digits
if (neg){
if (i3<=neg_size){
i2=neg_size;//extend to minimum character size
}else{
//didn't fit in recommended size, expand to either 4, 8 or 16 appropriately
i2=16;
if (i3<=8) i2=8;
if (i3<=4) i2=4;
}
}//neg

//adjust string to the left to remove unnecessary characters
if (i2!=16){
memmove(str->chr,str->chr+(16-i2),i2);
str->len=i2;
}

return str;

}

//note: QBASIC uses 8 characters for SINGLE/DOUBLE or generates "OVERFLOW" if this range is exceeded
//      QB64   uses 8 characters for SINGLE/DOUBLE/FLOAT but if this range is exceeded
//      it uses up to 16 characters before generating an "OVERFLOW" error
//performs overflow check before calling func_hex
qbs *func_hex_float(long double value){
static qbs *str;
static int64 ivalue;
static int64 uivalue;
//ref: uint64 0-18446744073709551615
//      int64 9223372036854775808 to 9223372036854775807
if ((value>=9.223372036854776E18)||(value<=-9.223372036854776E18)){
//note: ideally, the following line would be used, however, qbr_longdouble_to_uint64 just does the same as qbr
//if ((value>=1.844674407370956E19)||(value<=-9.223372036854776E18)){
str=qbs_new(0,1); error(6);//Overflow
return str;
}
if (value>=0){
uivalue=qbr_longdouble_to_uint64(value);
ivalue=uivalue;
}else{
ivalue=qbr(value);
}
return func_hex(ivalue,8);
}

ptrszint func_lbound(ptrszint *array,int32 index,int32 num_indexes){
if ((index<1)||(index>num_indexes)||((array[2]&1)==0)){
error(9); return 0;
}
index=num_indexes-index+1;
return array[4*index];
}

ptrszint func_ubound(ptrszint *array,int32 index,int32 num_indexes){
if ((index<1)||(index>num_indexes)||((array[2]&1)==0)){
error(9); return 0;
}
index=num_indexes-index+1;
return array[4*index]+array[4*index+1]-1;
}

static uint8 port60h_event[256];
static int32 port60h_events=0;


int32 func_inp(int32 port){
static int32 value;
unsupported_port_accessed=0;
if ((port>65535)||(port<-65536)){
error(6); return 0;//Overflow
}
port&=0xFFFF;

if (port==0x3C9){//read palette
if (write_page->pal){//avoid NULL pointer
//convert 0-255 value to 0-63 value
if (H3C9_read_next==0){//red
value=qbr_double_to_long((((double)((write_page->pal[H3C7_palette_register_read_index]>>16)&255))/3.984376-0.4999999f));
}
if (H3C9_read_next==1){//green
value=qbr_double_to_long((((double)((write_page->pal[H3C7_palette_register_read_index]>>8)&255))/3.984376-0.4999999f));
}
if (H3C9_read_next==2){//blue
value=qbr_double_to_long((((double)(write_page->pal[H3C7_palette_register_read_index]&255))/3.984376-0.4999999f));
}
H3C9_read_next=H3C9_read_next+1;
if (H3C9_read_next==3){
H3C9_read_next=0;
H3C7_palette_register_read_index=H3C7_palette_register_read_index+1;
H3C7_palette_register_read_index&=0xFF;
}
return value;
}//->pal
return 0;//non-palette modes
}

/*
3dAh (R):  Input Status #1 Register
bit   0  Either Vertical or Horizontal Retrace active if set
      1  Light Pen has triggered if set
      2  Light Pen switch is open if set
      3  Vertical Retrace in progress if set
    4-5  Shows two of the 6 color outputs, depending on 3C0h index 12h.
          Attr: Bit 4-5:   Out bit 4  Out bit 5
                   0          Blue       Red
                   1        I Blue       Green
                   2        I Red      I Green
*/
if (port==0x3DA){
value=0;
if (vertical_retrace_happened||vertical_retrace_in_progress){
vertical_retrace_happened=0;
value|=8;
}
return value;
}

if (port==0x60){
//return last scancode event
if (port60h_events){
value=port60h_event[0];
if (port60h_events>1) memmove(port60h_event,port60h_event+1,255);
port60h_events--;
return value;
}else{
return port60h_event[0];
}

}



unsupported_port_accessed=1;
return 0;//unknown port!
}

void sub_wait(int32 port,int32 andexpression,int32 xorexpression,int32 passed){
if (new_error) return;
//1. read value from port
//2. value^=xorexpression (if passed!)
//3. value^=andexpression
//IMPORTANT: Wait returns immediately if given port is unsupported by QB64 so program
//           can continue
static int32 value;

//error & range checking
if ((port>65535)||(port<-65536)){
error(6); return;//Overflow
}
port&=0xFFFF;
if ((andexpression<-32768)||(andexpression>65535)){
error(6); return;//Overflow
}
andexpression&=0xFF;
if (passed){
if ((xorexpression<-32768)||(xorexpression>65535)){
error(6); return;//Overflow
}
}
xorexpression&=0xFF;

wait:
value=func_inp(port);
if (passed) value^=xorexpression;
value&=andexpression;
if (value||unsupported_port_accessed||stop_program) return;
Sleep(1);
goto wait;
}

extern int32 tab_LPRINT;//1=dest is LPRINT image
extern int32 tab_spc_cr_size; //=1;//default
extern int32 tab_fileno;
qbs *func_tab(int32 pos){
if (new_error) return qbs_new(0,1);

static int32 tab_LPRINT_olddest;
if (tab_LPRINT){
if (!lprint_image) qbs_lprint(qbs_new(0,1),0);//send dummy data to init the LPRINT image
tab_LPRINT_olddest=func__dest();
sub__dest(lprint_image);
}

//returns a string to advance to the horizontal position "pos" on either
//the current line or the next line.
static int32 w,div,cursor;
//calculate width in spaces & current position
if (tab_spc_cr_size==2){
 //print to file
 div=1;
 w=2147483647;
 cursor=1;
 //validate file
 static int32 i;
 i=tab_fileno;
 if (i<0) goto invalid_file;//TCP/IP unsupported
 if (gfs_fileno_valid(i)!=1) goto invalid_file;//Bad file name or number
 i=gfs_fileno[i];//convert fileno to gfs index
 cursor=gfs_file[i].column;
 invalid_file:;
}else{
 //print to surface
 if (write_page->text){
  w=write_page->width;
  div=1;
 }else{
  if (fontwidth[write_page->font]){
  w=write_page->width/fontwidth[write_page->font];
  div=1;
  }else{
  //w=func__printwidth(singlespace,NULL,0);
  w=write_page->width;
  div=func__printwidth(singlespace,NULL,0);
  }
 }
 cursor=write_page->cursor_x;
}

static qbs *tqbs;
if ((pos<-32768)||(pos>32767)){
if (tab_LPRINT) sub__dest(tab_LPRINT_olddest);
tqbs=qbs_new(0,1);
error(7); return tqbs;//Overflow
}
if (pos>w) pos%=w;
if (pos<1) pos=1;
static int32 size,spaces,cr;
size=0; spaces=0; cr=0;
if (cursor>pos){
cr=1;
size=tab_spc_cr_size;
spaces=pos/div; if (pos%div) spaces++;
spaces--;//don't put a space on the dest position
size+=spaces;
}else{
spaces=(pos-cursor)/div; if ((pos-cursor)%div) spaces++;
size=spaces;
}
//build custom string
tqbs=qbs_new(size,1);
if (cr){
tqbs->chr[0]=13; if (tab_spc_cr_size==2) tqbs->chr[1]=10;
memset(&tqbs->chr[tab_spc_cr_size],32,spaces);
}else{
memset(tqbs->chr,32,spaces);
}
if (tab_LPRINT) sub__dest(tab_LPRINT_olddest);
return tqbs;
}

qbs *func_spc(int32 spaces){
if (new_error) return qbs_new(0,1);

static qbs *tqbs;
if ((spaces<-32768)||(spaces>32767)){tqbs=qbs_new(0,1); error(7); return tqbs;}//Overflow
if (spaces<0) spaces=0;

//for files, spc simply adds that many spaces
if (tab_spc_cr_size==2){//files
tqbs=qbs_new(spaces,1);
memset(tqbs->chr,32,spaces);
return tqbs;
}

static int32 tab_LPRINT_olddest;
if (tab_LPRINT){
if (!lprint_image) qbs_lprint(qbs_new(0,1),0);//send dummy data to init the LPRINT image
tab_LPRINT_olddest=func__dest();
sub__dest(lprint_image);
}

//for screens, spc adds that many spaces MOD screen_width_in_characters
//if 2 rows are bridged, the top row's characters are not printed to, just the lower
static int32 x,x2;
//calc spaces remaining on current screen row & MOD
static int32 spaces_left_on_line;
static qbs *onespace=NULL; if (!onespace){onespace=qbs_new(1,0); onespace->chr[0]=32;}
static int32 onespace_width;//for variable width fonts
if (write_page->text){
 spaces_left_on_line=write_page->width-write_page->cursor_x+1;
 spaces%=write_page->width;//MOD
}else{
 x=fontwidth[write_page->font]; 
 if (x){
  x2=write_page->width/x;//characters per row
  spaces_left_on_line=x2-write_page->cursor_x+1;
  spaces%=x2;//MOD
 }else{
  x2=write_page->width-write_page->cursor_x+1;//pixels left on row
  onespace_width=func__printwidth(onespace,NULL,0);
  spaces_left_on_line=x2/onespace_width;
  spaces%=(write_page->width/onespace_width);//MOD
 }
}

//build string
if (spaces_left_on_line>=spaces){
tqbs=qbs_new(spaces,1);
memset(tqbs->chr,32,spaces);
}else{
spaces-=spaces_left_on_line;
tqbs=qbs_new(1+spaces,1);
tqbs->chr[0]=13;
memset(tqbs->chr+1,32,spaces);
}

if (tab_LPRINT) sub__dest(tab_LPRINT_olddest);
return tqbs;
}

float func_pmap(float val,int32 option){
static int32 x,y;
if (new_error) return 0;
if (!write_page->text){
//note: for QBASIC/4.5/7.1 compatibility clipping_or_scaling check is skipped
if (option==0){
x=qbr_float_to_long(val*write_page->scaling_x+write_page->scaling_offset_x);
return x;
}
if (option==1){
y=qbr_float_to_long(val*write_page->scaling_y+write_page->scaling_offset_y);
return y;
}
if (option==2){
return (((double)qbr_float_to_long(val))-write_page->scaling_offset_x)/write_page->scaling_x;
}
if (option==3){
return (((double)qbr_float_to_long(val))-write_page->scaling_offset_y)/write_page->scaling_y;
}
}//!write_page->text
error(5);
return 0;
}



uint32 func_screen(int32 y,int32 x,int32 returncol,int32 passed){

static uint8 chr[65536];
static int32 x2,y2,x3,y3,i,i2,i3;
static uint32 col,firstcol;
uint8 *cp;

if (!passed) returncol=0;

if (read_page->text){
//on screen?
if ((y<1)||(y>read_page->height)){error(5); return 0;}
if ((x<1)||(x>read_page->width)){error(5); return 0;}
if (returncol){
return read_page->offset[((y-1)*read_page->width+x-1)*2+1];
}
return read_page->offset[((y-1)*read_page->width+x-1)*2];
}

//only 8x8,8x14,8x16 supported
if ((read_page->font!=8)&&(read_page->font!=14)&&(read_page->font!=16)){error(5); return 0;}

//on screen?
x2=read_page->width/fontwidth[read_page->font];
y2=read_page->height/fontheight[read_page->font];
if ((y<1)||(y>y2)){error(5); return 0;}
if ((x<1)||(x>x2)){error(5); return 0;}

//create "signature" of character on screen
x--; y--;
i=0;
i3=1;
y3=y*fontheight[read_page->font];
for (y2=0;y2<fontheight[read_page->font];y2++){
x3=x*fontwidth[read_page->font];
for (x2=0;x2<fontwidth[read_page->font];x2++){
col=point(x3,y3);
if (col){
if (i3){i3=0;firstcol=col;}
col=255;
}
chr[i]=col;
i++;
x3++;
}
y3++;
}

if (i3){//assume SPACE, no non-zero pixels were found
if (returncol) return 1;
return 32;
}

i3=i;//number of bytes per character "signature"

//compare signature with all ascii characters
for (i=0;i<=255;i++){
if (read_page->font==8) cp=&charset8x8[i][0][0];
if (read_page->font==14) cp=&charset8x16[i][1][0];
if (read_page->font==16) cp=&charset8x16[i][0][0];
i2=memcmp(cp,chr,i3);
if (!i2){//identical!
if (returncol) return firstcol;
return i;
}
}

//no match found
if (returncol) return 0;
return 32;
}

void sub_bsave(qbs *filename,int32 offset,int32 size){
if (new_error) return;
static ofstream fh;

static qbs *tqbs=NULL;
if (!tqbs) tqbs=qbs_new(0,0);
static qbs *nullt=NULL;
if (!nullt) nullt=qbs_new(1,0);

static int32 x;
nullt->chr[0]=0;
if ((offset<-65536)||(offset>65535)){error(6); return;}//Overflow
offset&=0xFFFF;
//note: QB64 allows a maximum of 65536 bytes, QB only allows 65535
if ((size<-65536)||(size>65536)){error(6); return;}//Overflow
if (size!=65536) size&=0xFFFF;
qbs_set(tqbs,qbs_add(filename,nullt));//prepare null-terminated filename
fh.open(fixdir(tqbs),ios::binary|ios::out);
if (fh.is_open()==NULL){error(64); return;}//Bad file name
x=253; fh.write((char*)&x,1);//signature byte
x=(defseg-&cmem[0])/16; fh.write((char*)&x,2);//segment
x=offset; fh.write((char*)&x,2);//offset
x=size; if (x>65535) x=0;//if filesize>65542 then size=filesize-7
fh.write((char*)&x,2);//size
fh.write((char*)(defseg+offset),size);//data
fh.close();
}

void sub_bload(qbs *filename,int32 offset,int32 passed){
if (new_error) return;
static uint8 header[7];
static ifstream fh;
static qbs *tqbs=NULL;
if (!tqbs) tqbs=qbs_new(0,0);
static qbs *nullt=NULL;
if (!nullt) nullt=qbs_new(1,0);


static int32 x,file_seg,file_off,file_size;
nullt->chr[0]=0;
if (passed){
if ((offset<-65536)||(offset>65535)){error(6); return;}//Overflow
offset&=0xFFFF;
}
qbs_set(tqbs,qbs_add(filename,nullt));//prepare null-terminated filename
fh.open(fixdir(tqbs),ios::binary|ios::in);
if (fh.is_open()==NULL){error(53); return;}//File not found
fh.read((char*)header,7); if (fh.gcount()!=7) goto error;
if (header[0]!=253) goto error;
file_seg=header[1]+header[2]*256;
file_off=header[3]+header[4]*256;
file_size=header[5]+header[6]*256;
if (file_size==0){
fh.seekg(0,ios::end);
file_size=fh.tellg();
fh.seekg(7,ios::beg);
file_size-=7;
if (file_size<65536) file_size=0;
}
if (passed){
fh.read((char*)(defseg+offset),file_size);
}else{
fh.read((char*)(&cmem[0]+file_seg*16+file_off),file_size);
}
if (fh.gcount()!=file_size) goto error;
fh.close();
return;
error:
fh.close();
error(54);//Bad file mode
}

int64 func_lof(int32 i){
static int64 size;
if (gfs_fileno_valid(i)!=1){error(52); return 0;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
size=gfs_lof(i);
if (size<0){
if (size==-2){error(258); return 0;}//invalid handle
if (size==-3){error(54); return 0;}//bad file mode
error(75); return 0;//assume[-9]: path/file access error
}
return size;
}

int32 func_eof(int32 i){
static int32 pos,lof;



//tcp/ip?
static int32 x;
if (i<0){
x=-(i+1);
if (x>=sfh_bufsize){error(52); return 0;}
if ((sfh[x].type!=2)&&(sfh[x].type!=3)){error(52); return 0;}
//valid tcp/ip connection
if (net_tcp[x].eof) return -1;
return 0;
}

if (gfs_fileno_valid(i)!=1){error(52); return 0;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if (gfs->type!=3){  //uint8 type;//qb access method (1=RANDOM,2=BINARY,3=INPUT,4=OUTPUT)
if (gfs_eof_passed(i)==1) return -1;
}else{
if (gfs_eof_reached(i)==1) return -1;
if (gfs_eof_passed(i)==1) return -1;
}
return 0;

}

void sub_seek(int32 i,int64 pos){
if (new_error) return;
if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
	if (gfs->type==1){//RANDOM
	pos--;
	if (pos<0){error(63); return;}//Bad record number
	pos*=gfs->record_length;
	pos++;
	}
pos--;
if (pos<0){error(63); return;}//Bad record number
int32 e;
e=gfs_setpos(i,pos);
if (e<0){
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
error(75); return;//assume[-9]: path/file access error
}
}

int64 func_seek(int32 i){
if (gfs_fileno_valid(i)!=1){error(52); return 0;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
	if (gfs->type==1){//RANDOM
	return gfs_getpos(i)/gfs->record_length+1;
	}
return gfs_getpos(i)+1;
}

int64 func_loc(int32 i){
if (gfs_fileno_valid(i)!=1){error(52); return 0;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];

if (gfs->com_port){
 #ifdef QB64_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 static COMSTAT c;
 ZeroMemory(&c,sizeof(COMSTAT));
 static DWORD ignore;
 if (!ClearCommError(f_w->file_handle,&ignore,&c)) return 0;
 return c.cbInQue;//bytes in COM input buffer
 #endif
}

	if (gfs->type==1){//RANDOM
	return gfs_getpos(i)/gfs->record_length+1;
	}
	if (gfs->type==2){//BINARY
	return gfs_getpos(i);
	}
//APPEND/OUTPUT/INPUT
int64 pos;
pos=gfs_getpos(i);
if (!pos) return 1;
pos--;
pos/=128;
pos++;
return pos;
}

qbs *func_input(int32 n,int32 i,int32 passed){
if (new_error) return qbs_new(0,1);
static qbs *str,*str2;
static int32 x,c;
if (n<0) str=qbs_new(0,1); else str=qbs_new(n,1);
if (passed){

if (gfs_fileno_valid(i)!=1){error(52); return str;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];
if ((gfs->type<2)||(gfs->type>3)){error(62); return str;}//Input past end of file
//note: RANDOM should be supported
//note: Unusually, QB returns "Input past end of file" instead of "Bad file mode"
if (!gfs->read){error(75); return str;}//Path/file access error

if (n<0){error(52); return str;}//Bad file name or number
if (n==0) return str;

//INPUT -> Input past end of file at EOF or CHR$(26)
//         unlike BINARY, partial strings cannot be returned
//         (use input_file_chr and modify it to support CHR$(26)
if (gfs->type==3){
x=0;
do{
c=file_input_chr(i);
if (c==-1){error(62); return str;}//Input past end of file
if (c==-2){error(75); return str;}//path/file access error
str->chr[x]=c;
x++;
}while(x<n);
return str;
}

//BINARY -> If past EOF, returns a NULL length string!
//          or as much of the data that was valid as possible
//          Seek POS is only advanced as far as was read!
//          Binary can read past CHR$(26)
//          (simply call C's read statement and manage via .eof
if (gfs->type==2){
static int32 e;
e=gfs_read(i,-1,str->chr,n);
if (e){
if (e!=-10){//note: on eof, unread buffer area becomes NULL
str->len=0;
if (e==-2){error(258); return str;}//invalid handle
if (e==-3){error(54); return str;}//bad file mode
if (e==-4){error(5); return str;}//illegal function call
if (e==-7){error(70); return str;}//permission denied
error(75); return str;//assume[-9]: path/file access error
}
}
str->len=gfs_read_bytes();//adjust if not enough data was available
return str;
}

//RANDOM (todo)

}else{
//keyboard/piped
//      For extended-two-byte character codes, only the first, CHR$(0), is returned and counts a 1 byte
if (n<0){error(52); return str;}
if (n==0) return str;
x=0;
waitforinput:
str2=qbs_inkey();
if (str2->len){
str->chr[x]=str2->chr[0];
x++;
}
qbs_free(str2);
if (stop_program) return str;
if (x<n){SDL_Delay(1); goto waitforinput;}
return str;
}
}

double func_sqr(double value){
if (value<0){error(5); return 0;}
return sqrt(value);
}

void sub_beep(){
sndsetup();
qb64_generatesound(783.99,0.2,0);
Sleep(250);
}

#define SND_CAPABILITY_SYNC 1
#define SND_CAPABILITY_VOL 2
#define SND_CAPABILITY_PAUSE 4
#define SND_CAPABILITY_LEN 8
#define SND_CAPABILITY_SETPOS 16

struct snd_struct{
uint32 handle;
Mix_Chunk *sample;
uint8 free;
uint8 playing;
uint8 paused;
float volume;
float volume_mult1;
float posx;
float posy;
float posz;
uint32 start_time;
uint32 paused_time;
uint8 looping;
uint32 limit;
double len;
uint8 capability;
};
snd_struct snd[AUDIO_CHANNELS];
Mix_Music *stream=NULL;
int32 stream_free=0;
int32 stream_loaded=0;
int32 stream_playing=0;
uint32 snd_free_handle=2;
int32 stream_type=0;
int32 stream_paused=0;
float stream_volume=1;
float stream_volume_mult1=1;
uint32 stream_start_time=0;
uint32 stream_paused_time=0;
double stream_setpos=0;
int32 stream_looping=0;
double stream_limit=0;
int32 stream_limited=0;
double stream_len=-1;
uint8 stream_capability;

void snd_check(){
sndsetup();
static int32 i,i2,i3;
//check stream
if (stream_loaded){
if (stream_free){
//still playing?
if (stream_playing&&(!stream_looping)) if (!Mix_PlayingMusic()) stream_playing=0;
if (!stream_playing){
Mix_FreeMusic(stream);
stream=NULL;
}
}
}
//check samples
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle){
if (snd[i].free){
//still playing?
if (snd[i].playing&&(!snd[i].looping)) if (!Mix_Playing(i)) snd[i].playing=0;
if (!snd[i].playing){
snd[i].handle=0;
//free sample data if unrequired by any other sample
i3=1;
for (i2=0;i2<AUDIO_CHANNELS;i2++){
if (snd[i2].handle){
if (snd[i2].sample==snd[i].sample){i3=0; break;}
}}
if (i3) Mix_FreeChunk(snd[i].sample);
}//!stream_playing
}//free
}//handle
}//i
}//snd_check






uint32 func__sndraw(uint8* data,uint32 bytes){
sndsetup();
//***unavailable to QB64 user***
static int32 i,i2,i3;
//find available index
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==0){
memset(&snd[i],0,sizeof(snd_struct));//clear structure
Mix_Volume(i,128);//restore full volume
snd[i].volume=1;
snd[i].volume_mult1=1;
snd[i].sample=Mix_QuickLoad_RAW((Uint8*)data,bytes);
//length_in_sec=size_in_bytes/samples_per_sec/bytes_per_sample/channels
snd[i].len=((double)snd[i].sample->alen)/22050.0/2.0/2.0;
snd[i].handle=snd_free_handle; snd_free_handle++; if (snd_free_handle==0) snd_free_handle=2;
snd[i].capability=SND_CAPABILITY_SYNC;
return snd[i].handle;
}
}
return 0;//no free indexes
}







uint32 func__sndopen(qbs* filename,qbs* requirements,int32 passed){
sndsetup();
if (new_error) return 0;

static qbs *s1=NULL;
if (!s1) s1=qbs_new(0,0);
static qbs *req=NULL;
if (!req) req=qbs_new(0,0);
static qbs *s3=NULL;
if (!s3) s3=qbs_new(0,0);

static uint8 r[32];
static int32 i,i2,i3;
//check requirements
memset(r,0,32);
if (passed){
if (requirements->len){
i=1;
qbs_set(req,qbs_ucase(requirements));//convert tmp str to perm str
nextrequirement:
i2=func_instr(i,req,qbs_new_txt(","),1);
if (i2){
qbs_set(s1,func_mid(req,i,i2-i,1));
}else{
qbs_set(s1,func_mid(req,i,req->len-i+1,1));
}
qbs_set(s1,qbs_rtrim(qbs_ltrim(s1)));
if (qbs_equal(s1,qbs_new_txt("SYNC"))){r[0]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("VOL"))){r[1]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("PAUSE"))){r[2]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("LEN"))){r[3]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("SETPOS"))){r[4]++; goto valid;}
error(5); return 0;//invalid requirements
valid:
if (i2){i=i2+1; goto nextrequirement;}
for (i=0;i<32;i++) if (r[i]>1){error(5); return 0;}//cannot define requirements twice
}//->len
}//passed
qbs_set(s1,qbs_add(filename,qbs_new_txt_len("\0",1)));//s1=filename+CHR$(0)
if (r[0]){//"SYNC"
if (r[4]) return 0;//"SETPOS" unsupported
//find available index
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==0){
memset(&snd[i],0,sizeof(snd_struct));//clear structure
Mix_Volume(i,128);//restore full volume
snd[i].volume=1;
snd[i].volume_mult1=1;
snd[i].sample=Mix_LoadWAV(fixdir(s1));
if (snd[i].sample==NULL) return 0;
//length_in_sec=size_in_bytes/samples_per_sec/bytes_per_sample/channels
snd[i].len=((double)snd[i].sample->alen)/22050.0/2.0/2.0;
snd[i].handle=snd_free_handle; snd_free_handle++; if (snd_free_handle==0) snd_free_handle=2;
snd[i].capability=SND_CAPABILITY_SYNC+r[1]*SND_CAPABILITY_VOL+r[2]*SND_CAPABILITY_PAUSE+r[3]*SND_CAPABILITY_LEN;
return snd[i].handle;
}
}
return 0;//no free indexes
}else{//not "SYNC"
//stream
	//dealloc current stream?
	if (stream_loaded){
	if (!stream_free){error(5); return 0;}
  Mix_FreeMusic(stream);
	stream_loaded=0;
	}
stream=NULL;

//check requirements
stream_len=-1;
if (r[3]){//"LEN"
//detect length (if possible)
static Mix_Chunk *tmpsample;
tmpsample=Mix_LoadWAV(fixdir(s1));
if (tmpsample){
stream_len=((double)tmpsample->alen)/22050.0/2.0/2.0;
Mix_FreeChunk(tmpsample);
}else{
return 0;//capability unavailable
}
}

stream=Mix_LoadMUS(fixdir(s1));
if (!stream) return 0;
Mix_VolumeMusic(128);//restore full volume
stream_volume=1;
stream_volume_mult1=1;
stream_paused=0;
stream_setpos=0;
stream_looping=0;
stream_free=0;
//check requirements?
stream_type=0;
i=Mix_GetMusicType(stream);
if (i==6) stream_type=1;//mp3

if (r[4]){//"SETPOS"
if (stream_type!=1){
 Mix_FreeMusic(stream);
 return 0;//capability unavailable
}
}

stream_capability=r[1]*SND_CAPABILITY_VOL+r[2]*SND_CAPABILITY_PAUSE+r[3]*SND_CAPABILITY_LEN+r[4]*SND_CAPABILITY_SETPOS;
stream_loaded=1;
return 1;
}
}

double func__sndlen(uint32 handle){
sndsetup();
static int32 i;
if (handle==0) return 0;//default response
if (handle==1){
if (stream_len==-1){error(5); return 0;}
if (!(stream_capability&SND_CAPABILITY_LEN)){error(5); return 0;}//unrequested capability
return stream_len;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return 0;}
if (!(snd[i].capability&SND_CAPABILITY_LEN)){error(5); return 0;}//unrequested capability
return snd[i].len;
}
}//i
error(5); return 0;//invalid handle
}

void sub__sndlimit(uint32 handle,double limit){
sndsetup();
if (new_error) return;
//limit=0 means no limit
static int32 i;
if (handle==0) return;//default response
if (handle==1){
if (!stream_loaded){error(5); return;}
if (stream_free){error(5); return;}
stream_limit=limit;
return;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return;}
snd[i].limit=limit*1000;
return;
}
}//i
error(5); return;//invalid handle
}

void sub__sndstop(uint32 handle){
sndsetup();
if (new_error) return;
static int32 i;
if (handle==0) return;//default response
if (handle==1){
if (!stream_loaded){error(5); return;}
if (stream_free){error(5); return;}
Mix_HaltMusic();//stop music
stream_paused=0;
stream_playing=0;
stream_setpos=0;
stream_looping=0;
return;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return;}
Mix_HaltChannel(i);
snd[i].playing=0;
snd[i].paused=0;
snd[i].looping=0;
return;
}
}//i
error(5); return;//invalid handle
}

int32 sndloop_call=0;

void sub__sndsetpos(uint32 handle,double sec){
sndsetup();
if (new_error) return;
static uint32 ms;
if (sec<0){error(5); return;}//must be >=0
if (handle==0) return;//default response
if (handle==1){
if (!stream_loaded){error(5); return;}
if (stream_free){error(5); return;}
if (stream_type!=1){error(5); return;}//only mp3 supports setpos
if (!(stream_capability&SND_CAPABILITY_SETPOS)){error(5); return;}//unrequested capability
if (stream_looping){error(5); return;}//setpos unavailable while looping
//still playing?
if (stream_playing&&(!stream_looping)) if (!Mix_PlayingMusic()) stream_playing=0;
if (stream_playing){//could also be paused
Sleep(100);//without this, music sometimes causes a GPF!
Mix_RewindMusic();
Sleep(100);//without this, music sometimes causes a GPF!
Mix_SetMusicPosition(sec);
ms=sec*1000.0+0.5;
stream_start_time=SDL_GetTicks()-ms;
if (stream_paused) stream_paused_time=SDL_GetTicks();
}else{
//music not playing, buffer the request
stream_setpos=sec;
}
return;
}
error(5); return;//samples do not support this function in sdl_mixer
}

double func__sndgetpos(uint32 handle){
sndsetup();
static int32 i;
if (handle==0) return 0;//default response
if (handle==1){
if (!stream_loaded){error(5); return 0;}
if (stream_free){error(5); return 0;}
//still playing?
if (stream_playing&&(!stream_looping)) if (!Mix_PlayingMusic()) stream_playing=0;
if (!stream_playing) return 0;
if (stream_paused) return(((double)(stream_paused_time-stream_start_time))/1000.0);
return(((double)(SDL_GetTicks()-stream_start_time))/1000.0);
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return 0;}
//still playing?
if (snd[i].playing&&(!snd[i].looping)) if (!Mix_Playing(i)) snd[i].playing=0;
if (!snd[i].playing) return 0;
if (snd[i].paused) return(((double)(snd[i].paused_time-snd[i].start_time))/1000.0);
return(((double)(SDL_GetTicks()-snd[i].start_time))/1000.0);
}
}//i
error(5); return 0;//invalid handle
}

void sub__sndbal(uint32 handle,double x,double y,double z,int32 passed){
sndsetup();
if (new_error) return;
//any optional parameter not passed is assumed to be 0 (which is what NULL equates to)
static int32 i,v,i2,i3;
static double d,d2,d3;
if (handle==0) return;//default response
if (handle==1){
if (!stream_loaded){error(5); return;}
if (stream_free){error(5); return;}
if (!(stream_capability&SND_CAPABILITY_VOL)){error(5); return;}//unrequested capability
//unsupported, but emulate distance by using a volume change
d=sqrt(x*x+y*y+z*z);

if (d<1) d=0;
if (d>1000) d=1000;
d=1000-d;
d=d/1000.0;
stream_volume_mult1=d;

v=stream_volume*129; if (v==129) v=128;
Mix_VolumeMusic(v*stream_volume_mult1);
return;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return;}
if (!(snd[i].capability&SND_CAPABILITY_VOL)){error(5); return;}//unrequested capability
if ((x==0)&&(z==0)) z=1;
snd[i].posx=x; snd[i].posy=y; snd[i].posz=z;
d=atan2(x,z)*57.295779513;
if (d<0) d=360+d;
i2=d+0.5; if (i2==360) i2=0;//angle
d2=sqrt(x*x+y*y+z*z);//distance
if (d2<1) d2=1;
if (d2>999.9) d2=999.9;
i3=d2/3.90625;
Mix_SetPosition(i,i2,i3);
return;
}
}//i
error(5); return;//invalid handle
}

void sub__sndplay(uint32 handle){
sndsetup();
if (new_error) return;
static int32 i;
static uint32 ms;
if (handle==0) return;//default response
	//stream?
	if (handle==1){
	if (!stream_loaded){error(5); return;}
	if (stream_free){error(5); return;}
	if (stream_paused){	
  Mix_ResumeMusic();	
  stream_start_time=stream_start_time+(SDL_GetTicks()-stream_paused_time);
  stream_paused=0;
	return;
	}
  stream_looping=0;
  Mix_HaltMusic();//in case music is already playing
	if (sndloop_call){
	if (stream_limit){error(5); return;}//limit invalid
  if (Mix_PlayMusic(stream,-1)==-1) return;
	stream_limited=0;
	stream_looping=1;
  }else{
	if (Mix_PlayMusic(stream,0)==-1) return;
	if (stream_limit) stream_limited=1; else stream_limited=0;
  }
  stream_start_time=SDL_GetTicks();
	if (stream_setpos){
	Sleep(100);	
	Mix_SetMusicPosition(stream_setpos); 
  ms=stream_setpos*1000.0+0.5;
	stream_start_time=SDL_GetTicks()-ms;  
  stream_setpos=0; 
  }
	stream_playing=1;
  return;
	}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return;}
if (snd[i].paused){
Mix_Resume(i);
//augment start_time
snd[i].start_time=snd[i].start_time+(SDL_GetTicks()-snd[i].paused_time);
snd[i].paused=0;
return;
}
snd[i].looping=0;
Mix_HaltChannel(i);//in case sound is already playing
//remind sdl_mixer of sound's position?
if (snd[i].posx||snd[i].posy||snd[i].posz) sub__sndbal(handle,snd[i].posx,snd[i].posy,snd[i].posz,-1);
if (sndloop_call){
	if (snd[i].limit){
	error(5); return;//limit invalid
	}else{
	if(Mix_PlayChannel(i,snd[i].sample,-1)==-1) return;	
	}
snd[i].looping=1;
}else{
	if (snd[i].limit){
	if(Mix_PlayChannelTimed(i,snd[i].sample,0,snd[i].limit)==-1) return;
	}else{
	if(Mix_PlayChannel(i,snd[i].sample,0)==-1) return;
	}
}
snd[i].start_time=SDL_GetTicks();
snd[i].playing=1;
return;
}
}//i
error(5); return;//invalid handle
}

void sub__sndloop(uint32 handle){
sndsetup();
if (new_error) return;
static int32 i;
if (handle==0) return;//default response
if (handle==1){
if (!stream_loaded){error(5); return;}
if (stream_free){error(5); return;}
sub__sndstop(handle);
stream_setpos=0;
sndloop_call=1; sub__sndplay(handle); sndloop_call=0;
return;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return;}
sub__sndstop(handle);
sndloop_call=1; sub__sndplay(handle); sndloop_call=0;
return;
}
}//i
error(5); return;//invalid handle
}

uint32 func__sndcopy(uint32 handle){
sndsetup();
if (new_error) return 0;
static int32 i,i2;
if (handle==0) return 0;//default response
if (handle==1){error(5); return 0;}//cannot copy a stream
//find handle
for (i2=0;i2<AUDIO_CHANNELS;i2++){
if (snd[i2].handle==handle){
	if (snd[i2].free){error(5); return 0;}
  if (!(snd[i2].capability&SND_CAPABILITY_SYNC)){error(5); return 0;}//unrequested capability
  //find free index
  for (i=0;i<AUDIO_CHANNELS;i++){
	if (snd[i].handle==0){
	 memset(&snd[i],0,sizeof(snd_struct));//clear structure
     Mix_Volume(i,128);//restore full volume
	 snd[i].volume=1;
     snd[i].volume_mult1=1;
     snd[i].sample=snd[i2].sample;
     snd[i].capability=snd[i2].capability;
	 //...
     snd[i].handle=snd_free_handle; snd_free_handle++; if (snd_free_handle==0) snd_free_handle=2;
	 return snd[i].handle;
	}
	}
	return 0;//no free indexes
}
}//i2
error(5); return 0;//invalid handle
}

int32 sub__sndvol_error;
void sub__sndvol(uint32 handle,float volume){
sndsetup();
if (new_error) return;
static int32 i,v;
sub__sndvol_error=1;
if ((volume<0)||(volume>1)){error(5); return;}
v=volume*129; if (v==129) v=128;
if (handle==0) {sub__sndvol_error=0;return;}//default response
if (handle==1){
if (!stream_loaded){error(5); return;}//invalid handle
if (stream_free){error(5); return;}
if (!(stream_capability&SND_CAPABILITY_VOL)){error(5); return;}//unrequested capability
Mix_VolumeMusic((float)v*stream_volume_mult1);
stream_volume=volume;
sub__sndvol_error=0;
return;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return;}
if (!(snd[i].capability&SND_CAPABILITY_VOL)){error(5); return;}//unrequested capability
Mix_Volume(i,(float)v*snd[i].volume_mult1);
snd[i].volume=volume;
sub__sndvol_error=0;
return;
}
}//i
error(5); return;//invalid handle
}

void sub__sndpause(uint32 handle){
sndsetup();
if (new_error) return;
static int32 i;
if (handle==0) return;//default response
if (handle==1){
if (!stream_loaded){error(5); return;}
if (stream_free){error(5); return;}
if (!(stream_capability&SND_CAPABILITY_PAUSE)){error(5); return;}//unrequested capability
if (stream_paused) return;
//still playing?
if (stream_playing&&(!stream_looping)) if (!Mix_PlayingMusic()) stream_playing=0;
if (!stream_playing) return;
Mix_PauseMusic();
stream_paused_time=SDL_GetTicks();
stream_paused=1;
return;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return;}
if (!(snd[i].capability&SND_CAPABILITY_PAUSE)){error(5); return;}//unrequested capability
if (snd[i].paused) return;
//still playing?
if (snd[i].playing&&(!snd[i].looping)) if (!Mix_Playing(i)) snd[i].playing=0;
if (!snd[i].playing) return;
Mix_Pause(i);
snd[i].paused_time=SDL_GetTicks();
snd[i].paused=1;
return;
}
}//i
error(5); return;//invalid handle
}

int32 func__sndpaused(uint32 handle){
sndsetup();
static int32 i;
if (handle==0) return 0;//default response
if (handle==1){
if (!stream_loaded){error(5); return 0;}
if (stream_free){error(5); return 0;}
if (!(stream_capability&SND_CAPABILITY_PAUSE)){error(5); return 0;}//unrequested capability
if (stream_paused) return -1;
return 0;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return 0;}
if (!(snd[i].capability&SND_CAPABILITY_PAUSE)){error(5); return 0;}//unrequested capability
if (snd[i].paused) return -1;
return 0;
}
}//i
error(5); return 0;//invalid handle
}

int32 func__sndplaying(uint32 handle){
sndsetup();
static int32 i;
if (handle==0) return 0;//default response
if (handle==1){
if (!stream_loaded){error(5); return 0;}
if (stream_free){error(5); return 0;}
//still playing?
if (stream_playing&&(!stream_looping)) if (!Mix_PlayingMusic()) stream_playing=0;
if (stream_paused) return 0;
if (stream_playing) return -1;
return 0;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return 0;}
//still playing?
if (snd[i].playing&&(!snd[i].looping)) if (!Mix_Playing(i)) snd[i].playing=0;
if (snd[i].paused) return 0;
if (snd[i].playing) return -1;
return 0;
}
}//i
error(5); return 0;//invalid handle
}

void sub__sndclose(uint32 handle){
sndsetup();
if (new_error) return;
static int32 i;
if (handle==0) return;//default response
if (handle==1){
if (!stream_loaded){error(5); return;}//invalid handle
if (stream_free){error(5); return;}//freed already
stream_free=1;
snd_check();
return;
}
//find handle
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle==handle){
if (snd[i].free){error(5); return;}//freed already
snd[i].free=1;
snd_check();
return;
}
}//i
error(5); return;//invalid handle
}

void sndcloseall(){
sndqueue_played=0; sndqueue_first=sndqueue_next;//invalidate items on the current PLAY sound queue
if (stream_loaded){
if (!stream_free){
sub__sndstop(1);
sub__sndclose(1);
}
}
static int32 i;
for (i=0;i<AUDIO_CHANNELS;i++){
if (snd[i].handle){
if (!snd[i].free){
sub__sndstop(snd[i].handle);
sub__sndclose(snd[i].handle);
}
}
}//i
}

//"macros"
void sub__sndplayfile(qbs *filename,int32 sync,double volume,int32 passed){
sndsetup();
if (new_error) return;
static uint32 handle;
static int32 setvolume;
static qbs *syncstr=NULL;
if (!syncstr) syncstr=qbs_new(0,0);
setvolume=0;
if (passed&2){
if ((volume<0)||(volume>1)){error(5); return;}
if (volume!=1) setvolume=1;
}
if ((!setvolume)&&(!sync)) syncstr->len=0;
if ((setvolume)&&(!sync)) qbs_set(syncstr,qbs_new_txt("VOL"));
if ((!setvolume)&&(sync)) qbs_set(syncstr,qbs_new_txt("SYNC"));
if ((setvolume)&&(sync)) qbs_set(syncstr,qbs_new_txt("SYNC,VOL"));
if (syncstr->len){
handle=func__sndopen(filename,syncstr,1);
}else{
handle=func__sndopen(filename,NULL,0);
}
if (handle==0) return;
if (setvolume) sub__sndvol(handle,volume);
sub__sndplay(handle);
sub__sndclose(handle);
}

void sub__sndplaycopy(uint32 handle,double volume,int32 passed){
sndsetup();
if (new_error) return;
uint32 handle2;
handle2=func__sndcopy(handle);
if (!handle2) return;//an error has already happened
if (passed){
sub__sndvol(handle2,volume);
if (sub__sndvol_error){
sub__sndclose(handle2);
return;
}
}
sub__sndplay(handle2);
sub__sndclose(handle2);
}

qbs *func_command_str=NULL;
qbs *func_command(){
static qbs *tqbs;
tqbs=qbs_new(func_command_str->len,1);
memcpy(tqbs->chr,func_command_str->chr,func_command_str->len);
return tqbs;
}

int32 shell_call_in_progress=0;
static int32 cmd_available=-1;

void sub_shell(qbs *str,int32 passed){
if (new_error) return;
 //exit full screen mode if necessary
 static int32 full_screen_mode;
 full_screen_mode=full_screen;
 if (full_screen_mode){
 full_screen_set=0;
 do{
 Sleep(0);
 }while(full_screen);
 }//full_screen_mode
static qbs *strz=NULL;
static int32 i;

if (!strz) strz=qbs_new(0,0);
if (passed) if (str->len==0) passed=0;//"" means launch a CONSOLE
if (passed){

#ifdef QB64_WINDOWS

static STARTUPINFO si;
static PROCESS_INFORMATION pi;

//cmd.exe available?
if (cmd_available==-1){
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
qbs_set(strz,qbs_new_txt("cmd.exe /c ver"));
if(CreateProcess(
        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NO_WINDOW, // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
WaitForSingleObject( pi.hProcess, INFINITE ); CloseHandle( pi.hProcess ); CloseHandle( pi.hThread );
cmd_available=1;
}else{
cmd_available=0;
}
}//cmd_available==-1

if (cmd_available==1){

qbs_set(strz,qbs_add(qbs_new_txt("cmd.exe /c "),str));
qbs_set(strz,qbs_add(strz,qbs_new_txt_len("\0",1)));
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
if(CreateProcess(
        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        DETACHED_PROCESS, // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
shell_call_in_progress=1;
// Wait until child process exits.
WaitForSingleObject( pi.hProcess, INFINITE );
// Close process and thread handles. 
CloseHandle( pi.hProcess );
CloseHandle( pi.hThread );
shell_call_in_progress=0;
goto shell_complete;
}
goto shell_complete;//failed

}else{

qbs_set(strz,qbs_add(qbs_new_txt("command.com /c "),str));
qbs_set(strz,qbs_add(strz,qbs_new_txt_len("\0",1)));
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
if(CreateProcess(
        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE, 		// No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
shell_call_in_progress=1;
// Wait until child process exits.
WaitForSingleObject( pi.hProcess, INFINITE );
// Close process and thread handles. 
CloseHandle( pi.hProcess );
CloseHandle( pi.hThread );
shell_call_in_progress=0;
goto shell_complete;
}
goto shell_complete;//failed

}//cmd_available

#else

qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
shell_call_in_progress=1;
system((char*)strz->chr);
shell_call_in_progress=0;

#endif

}else{

#ifdef QB64_WINDOWS
//SHELL (with no parameters)
AllocConsole();
qbs_set(strz,qbs_new_txt_len("COMMAND\0",8));
shell_call_in_progress=1;
system((char*)strz->chr);
shell_call_in_progress=0;
FreeConsole();
goto shell_complete;
#endif

}

shell_complete:
 //reenter full screen mode if necessary
 if (full_screen_mode){
 full_screen_set=full_screen_mode;
 do{
 Sleep(0);
 }while(!full_screen);
 }//full_screen_mode
}

void sub_shell2(qbs *str){
if (new_error) return;
static qbs *strz=NULL;
static int32 i;
if (!strz) strz=qbs_new(0,0);
if (!str->len){error(5); return;}//cannot launch a hidden console

#ifdef QB64_WINDOWS

static STARTUPINFO si;
static PROCESS_INFORMATION pi;

//cmd.exe available?
if (cmd_available==-1){
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
qbs_set(strz,qbs_new_txt("cmd.exe /c ver"));
if(CreateProcess(
        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NO_WINDOW, // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
WaitForSingleObject( pi.hProcess, INFINITE ); CloseHandle( pi.hProcess ); CloseHandle( pi.hThread );
cmd_available=1;
}else{
cmd_available=0;
}
}//cmd_available==-1

if (cmd_available==1){

qbs_set(strz,qbs_add(qbs_new_txt("cmd.exe /c "),str));
qbs_set(strz,qbs_add(strz,qbs_new_txt_len("\0",1)));
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
if(CreateProcess(
        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NO_WINDOW, // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
shell_call_in_progress=1;
// Wait until child process exits.
WaitForSingleObject( pi.hProcess, INFINITE );
// Close process and thread handles. 
CloseHandle( pi.hProcess );
CloseHandle( pi.hThread );
shell_call_in_progress=0;
goto shell_complete;
}
goto shell_complete;//failed

}else{

qbs_set(strz,qbs_add(qbs_new_txt("command.com /c "),str));
qbs_set(strz,qbs_add(strz,qbs_new_txt_len("\0",1)));
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
if(CreateProcess(

        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE, //note: cannot hide new console, but can preserve existing one
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
shell_call_in_progress=1;
// Wait until child process exits.
WaitForSingleObject( pi.hProcess, INFINITE );
// Close process and thread handles. 
CloseHandle( pi.hProcess );
CloseHandle( pi.hThread );
shell_call_in_progress=0;
goto shell_complete;
}
goto shell_complete;//failed

}//cmd_available

#else

qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
shell_call_in_progress=1;
system((char*)strz->chr);
shell_call_in_progress=0;
return;

#endif

shell_complete:;
}


//shell3 launches 'str' but does not wait for it to complete
void sub_shell3(qbs *str){//SHELL _DONTWAIT
if (new_error) return;
static qbs *strz=NULL;
static int32 i;
if (!strz) strz=qbs_new(0,0);
if (!str->len){error(5); return;}//console unsupported

#ifdef QB64_WINDOWS

static STARTUPINFO si;
static PROCESS_INFORMATION pi;

//cmd.exe available?
if (cmd_available==-1){
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
qbs_set(strz,qbs_new_txt("cmd.exe /c ver"));
if(CreateProcess(
        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NO_WINDOW, // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
WaitForSingleObject( pi.hProcess, INFINITE ); CloseHandle( pi.hProcess ); CloseHandle( pi.hThread );
cmd_available=1;
}else{
cmd_available=0;
}
}//cmd_available==-1

if (cmd_available==1){

qbs_set(strz,qbs_add(qbs_new_txt("cmd.exe /c "),str));
qbs_set(strz,qbs_add(strz,qbs_new_txt_len("\0",1)));
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
if(CreateProcess(
        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        DETACHED_PROCESS, // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
//ref: The created process remains in the system until all threads within the process have terminated and all handles to the process and any of its threads have been closed through calls to CloseHandle. The handles for both the process and the main thread must be closed through calls to CloseHandle. If these handles are not needed, it is best to close them immediately after the process is created. 
CloseHandle( pi.hProcess );
CloseHandle( pi.hThread );
goto shell_complete;
}
goto shell_complete;//failed

}else{

qbs_set(strz,qbs_add(qbs_new_txt("command.com /c "),str));
qbs_set(strz,qbs_add(strz,qbs_new_txt_len("\0",1)));
ZeroMemory( &si, sizeof(si) ); si.cb = sizeof(si); ZeroMemory( &pi, sizeof(pi) );
if(CreateProcess(
        NULL,           // No module name (use command line)
        (char*)&strz->chr[0], // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE, //note: cannot hide new console, but can preserve existing one
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
){
//ref: The created process remains in the system until all threads within the process have terminated and all handles to the process and any of its threads have been closed through calls to CloseHandle. The handles for both the process and the main thread must be closed through calls to CloseHandle. If these handles are not needed, it is best to close them immediately after the process is created. 
CloseHandle( pi.hProcess );
CloseHandle( pi.hThread );
goto shell_complete;
}
goto shell_complete;//failed

}//cmd_available

#else

qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
pid_t pid = fork ();
if (pid == 0)
  _exit (system((char*)strz->chr));
return;

#endif

shell_complete:;
}//SHELL _DONTWAIT


void sub_kill(qbs *str){
//note: file not found returned for non-existant paths too
//      file already open returned if access unavailable
if (new_error) return;
static int32 i;
static qbs *strz=NULL;
if (!strz) strz=qbs_new(0,0);
qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
#ifdef QB64_WINDOWS
 static WIN32_FIND_DATA fd;
 static HANDLE hFind;
 static qbs *strpath=NULL; if (!strpath) strpath=qbs_new(0,0);
 static qbs *strfullz=NULL; if (!strfullz) strfullz=qbs_new(0,0);
 //find path
 qbs_set(strpath,strz);
 for(i=strpath->len;i>0;i--){
 if ((strpath->chr[i-1]==47)||(strpath->chr[i-1]==92)){strpath->len=i; break;}
 }//i
 if (i==0) strpath->len=0;//no path specified
 static int32 count;
 count=0;
 hFind = FindFirstFile(fixdir(strz), &fd);
 if(hFind==INVALID_HANDLE_VALUE){error(53); return;}//file not found
 do{
 if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0){
 qbs_set(strfullz,qbs_add(strpath,qbs_new_txt_len(fd.cFileName,strlen(fd.cFileName)+1)));
 if (!DeleteFile((char*)strfullz->chr)){
 i=GetLastError();
 if ((i==5)||(i==19)||(i==33)||(i==32)){FindClose(hFind); error(55); return;}//file already open
 FindClose(hFind); error(53); return;//file not found
 }
 count++;
 }//not a directory
 }while(FindNextFile(hFind,&fd));
 FindClose(hFind);
 if (!count){error(53); return;}//file not found 
 return;
#else
 if (remove(fixdir(strz))){
 i=errno;
 if (i==ENOENT){error(53); return;}//file not found
 if (i==EACCES){error(75); return;}//path/file access error
 error(64);//bad file name (assumed)
 }
#endif
}

void sub_name(qbs *oldname,qbs *newname){
if (new_error) return;
static qbs *strz=NULL;
if (!strz) strz=qbs_new(0,0);
static qbs *strz2=NULL;
if (!strz2) strz2=qbs_new(0,0);
static int32 i;
qbs_set(strz,qbs_add(oldname,qbs_new_txt_len("\0",1)));
qbs_set(strz2,qbs_add(newname,qbs_new_txt_len("\0",1)));
if (rename(fixdir(strz),fixdir(strz2))){
i=errno;
if (i==ENOENT){error(53); return;}//file not found
if (i==EINVAL){error(64); return;}//bad file name
if (i==EACCES){error(75); return;}//path/file access error
error(5);//Illegal function call (assumed)
}
}

void sub_chdir(qbs *str){



if (new_error) return;
static qbs *strz=NULL;
if (!strz) strz=qbs_new(0,0);
qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
if (chdir(fixdir(strz))==-1){
//assume errno==ENOENT
error(76);//path not found
}
}

void sub_mkdir(qbs *str){
if (new_error) return;
static qbs *strz=NULL;
if (!strz) strz=qbs_new(0,0);
qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
#ifdef QB64_LINUX
 if (mkdir(fixdir(strz),0770)==-1){
#else
 if (mkdir(fixdir(strz))==-1){
#endif
if (errno==EEXIST){error(75); return;}//path/file access error
//assume errno==ENOENT
error(76);//path not found
}

}

void sub_rmdir(qbs *str){
if (new_error) return;
static qbs *strz=NULL;
if (!strz) strz=qbs_new(0,0);
qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
if (rmdir(fixdir(strz))==-1){
if (errno==ENOTEMPTY){error(75); return;}//path/file access error
//assume errno==ENOENT
error(76);//path not found
}
}

long double pow2(long double x,long double y){
if (x<0){
if (y!=floor(y)){error(5); return 0;}
}
return pow(x,y);
}

int32 func_freefile(){
return gfs_fileno_freefile();
}

void sub__mousehide(){
#ifdef QB64_LINUX
lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
#endif
mouse_hideshow_called=1;
static int x,y;
SDL_GetMouseState(&x,&y);
SDL_WarpMouse(x,y);
SDL_ShowCursor(0);
SDL_WarpMouse(x,y);
#ifdef QB64_LINUX
lock_mainloop=0; Sleep(1);//unlock
#endif
}

void sub__mouseshow(){
device_mouse_relative=0;
#ifdef QB64_LINUX
lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
#endif
mouse_hideshow_called=1;
static int x,y;
if (SDL_WM_GrabInput(SDL_GRAB_QUERY)==SDL_GRAB_ON) SDL_WM_GrabInput(SDL_GRAB_OFF);
SDL_GetMouseState(&x,&y);
SDL_WarpMouse(x,y);
SDL_ShowCursor(1);
SDL_WarpMouse(x,y);
#ifdef QB64_LINUX
lock_mainloop=0; Sleep(1);//unlock
#endif
}

int32 mousemovementfix_state=0;
void mousemovementfix(){
#ifdef QB64_LINUX
lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
#endif
if (full_screen){
if (mousemovementfix_state){//disable fix if active
mousemovementfix_state=0;
if (SDL_WM_GrabInput(SDL_GRAB_QUERY)==SDL_GRAB_ON) SDL_WM_GrabInput(SDL_GRAB_OFF);
//note: mouse show state is not reverted
}
return;
}
if (SDL_ShowCursor(-1)==1) SDL_ShowCursor(0);
//note: regrabs input if necessary (happens when app loses focus)
if (SDL_WM_GrabInput(SDL_GRAB_QUERY)==SDL_GRAB_OFF) SDL_WM_GrabInput(SDL_GRAB_ON);
mousemovementfix_state=1;
#ifdef QB64_LINUX
lock_mainloop=0; Sleep(1);//unlock
#endif
}
void mousemovementfix_mainloop(){
if (full_screen){
if (mousemovementfix_state){//disable fix if active
mousemovementfix_state=0;
if (SDL_WM_GrabInput(SDL_GRAB_QUERY)==SDL_GRAB_ON) SDL_WM_GrabInput(SDL_GRAB_OFF);
//note: mouse show state is not reverted
}
return;
}
if (SDL_ShowCursor(-1)==1) SDL_ShowCursor(0);
//note: regrabs input if necessary (happens when app loses focus)
if (SDL_WM_GrabInput(SDL_GRAB_QUERY)==SDL_GRAB_OFF) SDL_WM_GrabInput(SDL_GRAB_ON);
mousemovementfix_state=1;
}


float func__mousemovementx(){
device_mouse_relative=1;
mousemovementfix();
return mouse_messages[current_mouse_message].movementx;
}

float func__mousemovementy(){
device_mouse_relative=1;
mousemovementfix();
return mouse_messages[current_mouse_message].movementy;
}

void sub__mousemove(float x,float y){
int32 x2,y2,sx,sy;
if (display_page->text){
sx=fontwidth[display_page->font]*display_page->width; sy=fontheight[display_page->font]*display_page->height;
if (x<0.5) goto error;
if (y<0.5) goto error;
if (x>((float)display_page->width)+0.5) goto error;
if (y>((float)display_page->height)+0.5) goto error;
x-=0.5; y-=0.5;
x=x*(float)fontwidth[display_page->font]; y=y*(float)fontheight[display_page->font];
x2=qbr_float_to_long(x); y2=qbr_float_to_long(y);
if (x2<0) x2=0;
if (y2<0) y2=0;
if (x2>sx-1) x2=sx-1;
if (y2>sy-1) y2=sy-1;
}else{
sx=display_page->width; sy=display_page->height;
x2=qbr_float_to_long(x); y2=qbr_float_to_long(y);
if (x2<0) goto error;
if (y2<0) goto error;
if (x2>sx-1) goto error;
if (y2>sy-1) goto error;
}
//x2,y2 are pixel co-ordinates
//adjust for fullscreen position as necessary:
x2*=x_scale; y2*=y_scale;
x2+=x_offset; y2+=y_offset;
SDL_WarpMouse(x2,y2);
return;
error:
error(5);
}

float func__mousex(){
static int32 x,x2;
static float f;
x=mouse_messages[current_mouse_message].x;
x-=x_offset;
if (x<0) x=0;
x/=x_scale;
x2=display_page->width; if (display_page->text) x2*=fontwidth[display_page->font];
if (x>=x2) x=x2-1;
if (display_page->text){
f=x;
x2=fontwidth[display_page->font];
x=x/x2+1;
f=f/(float)x2+0.5f;
 //if cint(f)<>x then adjust f so it does
 x2=qbr_float_to_long(f);
 if (x2>x) f-=0.001f;
 if (x2<x) f+=0.001f;
return f;
}
return x;
}

float func__mousey(){
static int32 y,y2;
static float f;
y=mouse_messages[current_mouse_message].y;
y-=y_offset;
if (y<0) y=0;
y/=y_scale;
y2=display_page->height; if (display_page->text) y2*=fontheight[display_page->font];
if (y>=y2) y=y2-1;
if (display_page->text){
f=y;
y2=fontheight[display_page->font];
y=y/y2+1;
f=f/(float)y2+0.5f;
 //if cint(f)<>y then adjust f so it does
 y2=qbr_float_to_long(f);
 if (y2>y) f-=0.001f;
 if (y2<y) f+=0.001f;
return f;
}
return y;
}

int32 func__mouseinput(){
if (current_mouse_message==last_mouse_message) return 0;
current_mouse_message=(current_mouse_message+1)&65535;
return -1;
}

int32 func__mousebutton(int32 i){
if (i<1){error(5); return 0;}
if (i>3) return 0;//current SDL only supports 3 mouse buttons!
//swap indexes 2&3
if (i==2){
i=3;
}else{
if (i==3) i=2;
}
if (mouse_messages[current_mouse_message].buttons&(1<<(i-1))) return -1;
return 0;
}

int32 func__mousewheel(){
static uint32 x;
x=mouse_messages[current_mouse_message].buttons;
if ((x&(8+16))==(8+16)) return 0;//cancelled out change
if (x&8) return -1;//up
if (x&16) return 1;//down
return 0;//no change
}

extern uint16 call_absolute_offsets[256];
void call_absolute(int32 args,uint16 offset){
memset(&cpu,0,sizeof(cpu_struct));//flush cpu
cpu.cs=((defseg-cmem)>>4); cpu.ip=offset;
cpu.ss=0xFFFF; cpu.sp=0;//sp "loops" to <65536 after first push
cpu.ds=80;
//push (near) arg offsets
static int32 i;
for (i=0;i<args;i++){
cpu.sp-=2; *(uint16*)(cmem+cpu.ss*16+cpu.sp)=call_absolute_offsets[i];
}
//push ret segment, then push ret offset (both 0xFFFF to return control to QB64)
cpu.sp-=4; *(uint32*)(cmem+cpu.ss*16+cpu.sp)=0xFFFFFFFF;
cpu_call();
}

void call_int(int32 i){

if (i==0x33){

if (cpu.ax==0){
cpu.ax=0xFFFF;//mouse installed
cpu.bx=2;
return;
}

if (cpu.ax==1){sub__mouseshow(); return;}
if (cpu.ax==2){sub__mousehide(); return;}
if (cpu.ax==3){
//return the current mouse status
//buttons
cpu.bx=mouse_messages[last_mouse_message].buttons&1;
if (mouse_messages[last_mouse_message].buttons&4) cpu.bx+=2;
//x,y offsets
static int32 current_mouse_message_backup;
current_mouse_message_backup=current_mouse_message;
current_mouse_message=last_mouse_message;
static float mx,my;
mx=func__mousex(); my=func__mousey();
current_mouse_message=current_mouse_message_backup;
cpu.cx=mx; cpu.dx=my;
//double x-axis value for modes 1,7,13
if ((display_page->compatible_mode==1)||(display_page->compatible_mode==7)||(display_page->compatible_mode==13)) cpu.cx*=2;
if (display_page->text){
//note: a range from 0 to columns*8-1 is returned regardless of the number of actual pixels
cpu.cx=(mx-0.5)*8.0;
if (cpu.cx>=(display_page->width*8)) cpu.cx=(display_page->width*8)-1;
//note: a range from 0 to rows*8-1 is returned regardless of the number of actual pixels
//obselete line of code: cpu.dx=(((float)cpu.dx)/((float)(display_page->height*fontheight[display_page->font])))*((float)(display_page->height*8));//(mouse_y/height_in_pixels)*(rows*8)
cpu.dx=(my-0.5)*8.0;
if (cpu.dx>=(display_page->height*8)) cpu.dx=(display_page->height*8)-1;
}
return;
}

if (cpu.ax==7){//horizontal min/max
return;
}
if (cpu.ax==8){//vertical min/max
return;
}

//MessageBox(NULL,"Unknown MOUSE Sub-function","Call Interrupt Error",MB_OK|MB_SYSTEMMODAL);
//exit(cpu.ax);

return;
}

}


//uint8 *soundwave(double frequency,double length,double volume,double fadein,double fadeout,uint8 *data);
//uint8 *soundwavesilence(double length,uint8 *data);

int32 func_play(int32 ignore){
return 0;
}

/*
Formats:
A[#|+|-][0-64]
   0-64 is like temp. Lnumber, 0 is whatever the current default is




*/
void sub_play(qbs *str){
sndsetup();
static uint8 *b,*wave,*wave2,*wave3;
static double d;
static int32 i,bytes_left,a,x,x2,x3,x4,x5,wave_bytes,wave_base;
static int32 o=4;
static double t=120;//quarter notes per minute (120/60=2 per second)
static double l=4;
static double pause=1.0/8.0;//ML 0.0, MN 1.0/8.0, MS 1.0/4.0
static double length,length2;//derived from l and t
static double frequency;
static double mb=0;
static double v=50;

static int32 n;//the semitone-intervaled note to be played
static int32 n_changed;//+,#,- applied?
static int64 number;
static int32 number_entered;
static int32 followup;//1=play note
static int32 playit;
static uint32 handle=NULL;
static int32 fullstops=0;
b=str->chr;
bytes_left=str->len;
wave=NULL;
wave_bytes=0;
n_changed=0;
n=0;
number_entered=0;
number=0;
followup=0;
length=1.0/(t/60.0)*(4.0/l);
playit=0;
wave_base=0;//point at which new sounds will be inserted

next_byte:
if ((bytes_left--)||followup){

if (bytes_left<0){i=32; goto follow_up;}

i=*b++;
if (i==32) goto next_byte;
if (i>=97&&i<=122) a=i-32; else a=i;

if (i==61){//= (+VARPTR$)
if (fullstops){error(5); return;}
if (number_entered){error(5); return;}
number_entered=2;
//VARPTR$ reference
/*
'BYTE=1
'INTEGER=2
'STRING=3 SUB-STRINGS must use "X"+VARPTR$(string$)
'SINGLE=4
'INT64=5
'FLOAT=6
'DOUBLE=8
'LONG=20
'BIT=64+n
*/
if (bytes_left<3){error(5); return;}
i=*b++; bytes_left--;//read type byte
x=*(uint16*)b; b+=2; bytes_left-=2;//read offset within DBLOCK
//note: allowable _BIT type variables in VARPTR$ are all at a byte offset and are all
//      padded until the next byte
d=0;
switch(i){
case 1:
d=*(char*)(dblock+x);
break;
case (1+128):
d=*(uint8*)(dblock+x);
break;
case 2:
d=*(int16*)(dblock+x);
break;
case (2+128):
d=*(uint16*)(dblock+x);
break;
case 4:
d=*(float*)(dblock+x);
break;
case 5:
d=*(int64*)(dblock+x);
break;
case (5+128):
d=*(int64*)(dblock+x); //unsigned conversion is unsupported!
break;
case 6:
d=*(long double*)(dblock+x);
break;
case 8:
d=*(double*)(dblock+x);
break;
case 20:
d=*(int32*)(dblock+x);
break;
case (20+128):
d=*(uint32*)(dblock+x);
break;
default:
//bit type?
if ((i&64)==0){error(5); return;}
x2=i&63;
if (x2>56){error(5); return;}//valid number of bits?
//create a mask
static int64 i64num,mask,i64x;
mask=(((int64)1)<<x2)-1;
i64num=(*(int64*)(dblock+x))&mask;
//signed?
if (i&128){
mask=((int64)1)<<(x2-1);
if (i64num&mask){//top bit on?
mask=-1; mask<<=x2; i64num+=mask;
}
}//signed
d=i64num;
}
if (d>2147483647.0||d<-2147483648.0){error(5); return;}//out of range value!
number=qbr_double_to_long(d);
goto next_byte;
}

//read in a number
if ((i>=48)&&(i<=57)){
if (fullstops||(number_entered==2)){error(5); return;}
if (!number_entered){number=0; number_entered=1;}
number=number*10+i-48;
goto next_byte;
}

//read fullstops
if (i==46){
if (followup!=7&&followup!=1&&followup!=4){error(5); return;}
fullstops++;
goto next_byte;
}

follow_up:

if (followup==8){//V...
if (!number_entered){error(5); return;}
number_entered=0;
if (number>100){error(5); return;}
v=number;
followup=0; if (bytes_left<0) goto done;
}//8

if (followup==7){//P...
if (number_entered){
number_entered=0;
if (number<1||number>64){error(5); return;}
length2=1.0/(t/60.0)*(4.0/((double)number));
}else{
length2=length;
}
d=length2; for (x=1;x<=fullstops;x++){d/=2.0; length2=length2+d;} fullstops=0;

soundwave_bytes=wavesize(length2);
if (!wave){
 //create buffer
 wave=(uint8*)calloc(soundwave_bytes,1); wave_bytes=soundwave_bytes;
 wave_base=0;
}else{
 //increase buffer?
 if ((wave_base+soundwave_bytes)>wave_bytes){
 wave=(uint8*)realloc(wave,wave_base+soundwave_bytes);
 memset(wave+wave_base,0,wave_base+soundwave_bytes-wave_bytes);
 wave_bytes=wave_base+soundwave_bytes;
 }
}
if (i!=44){
wave_base+=soundwave_bytes;
}

playit=1;
followup=0;
if (i==44) goto next_byte;
if (bytes_left<0) goto done;
}//7

if (followup==6){//T...
if (!number_entered){error(5); return;}
number_entered=0;
if (number<32||number>255){error(5); return;}
t=number;
length=1.0/(t/60.0)*(4.0/l);
followup=0; if (bytes_left<0) goto done;
}//6

if (followup==5){//M...
if (number_entered){error(5); return;}
switch(a){
case 76://L
pause=0;
break;
case 78://N
pause=1.0/8.0;
break;
case 83://S
pause=1.0/4.0;
break;

case 66://MB
if (!mb){
mb=1;
if (playit){
 handle=func__sndraw(wave,wave_bytes);
 if (handle){
  sndqueue[sndqueue_next]=handle;
  sndqueue_next++; if (sndqueue_next>sndqueue_lastindex) sndqueue_next=0;
 }else{free(wave);}
 wave=(uint8*)calloc(4,1); handle=func__sndraw(wave,4);
 if (handle){
  sndqueue_wait=sndqueue_next; suspend_program|=2; qbevent=1;
  sndqueue[sndqueue_next]=handle;
  sndqueue_next++; if (sndqueue_next>sndqueue_lastindex) sndqueue_next=0;
 }else{free(wave);}
}
playit=0;
wave=NULL;
}
break;
case 70://MF
if (mb){
mb=0;
//preceding MB content incorporated into MF block
}
break;
default:
error(5); return;
}
followup=0; goto next_byte;
}//5

if (followup==4){//N...
if (!number_entered){error(5); return;}
number_entered=0;
if (number>84){error(5); return;}
n=-33+number;
goto followup1;
followup=0; if (bytes_left<0) goto done;
}//4

if (followup==3){//O...
if (!number_entered){error(5); return;}
number_entered=0;
if (number>6){error(5); return;}
o=number;
followup=0; if (bytes_left<0) goto done;
}//3

if (followup==2){//L...
if (!number_entered){error(5); return;}
number_entered=0;
if (number<1||number>64){error(5); return;}
l=number;
length=1.0/(t/60.0)*(4.0/l);
followup=0; if (bytes_left<0) goto done;
}//2

if (followup==1){//A-G...
if (i==45){//-
 if (n_changed||number_entered){error(5); return;}
 n_changed=1; n--;
 goto next_byte;
}
if (i==43||i==35){//+,#
 if (n_changed||number_entered){error(5); return;}
 n_changed=1; n++;
goto next_byte;
}
followup1:
if (number_entered){
 number_entered=0;
 if (number<0||number>64){error(5); return;}
 if (!number) length2=length; else length2=1.0/(t/60.0)*(4.0/((double)number));
}else{
 length2=length;
}//number_entered
d=length2; for (x=1;x<=fullstops;x++){d/=2.0; length2=length2+d;} fullstops=0;
//frequency=(2^(note/12))*440
frequency=pow(2.0,((double)n)/12.0)*440.0;

//create wave
wave2=soundwave(frequency,length2*(1.0-pause),v/100.0,NULL,NULL);
if (pause>0){
wave2=(uint8*)realloc(wave2,soundwave_bytes+wavesize(length2*pause));
memset(wave2+soundwave_bytes,0,wavesize(length2*pause));
soundwave_bytes+=wavesize(length2*pause);
}

if (!wave){
 //adopt buffer
 wave=wave2; wave_bytes=soundwave_bytes;
 wave_base=0;
}else{
 //mix required?
 if (wave_base==wave_bytes) x=0; else x=1;
 //increase buffer?
 if ((wave_base+soundwave_bytes)>wave_bytes){
 wave=(uint8*)realloc(wave,wave_base+soundwave_bytes);
 memset(wave+wave_base,0,wave_base+soundwave_bytes-wave_bytes);
 wave_bytes=wave_base+soundwave_bytes;
 }
 //mix or copy
 if (x){
  //mix
  static int16 *sp,*sp2;
  sp=(int16*)(wave+wave_base);
  sp2=(int16*)wave2;
  x2=soundwave_bytes/2;
  for (x=0;x<x2;x++){
  x3=*sp2++;
  x4=*sp;
  x4+=x3;
  if (x4>32767) x4=32767;
  if (x4<-32767) x4=-32767;
  *sp++=x4;
  }//x 
 }else{
  //copy
  memcpy(wave+wave_base,wave2,soundwave_bytes);  
 }//x
 free(wave2);
}
if (i!=44){
wave_base+=soundwave_bytes;
}

playit=1;
n_changed=0;
followup=0; 
if (i==44) goto next_byte;
if (bytes_left<0) goto done;
}//1

if (a>=65&&a<=71){
//modify a to represent a semitonal note (n) interval
switch(a){
//[c][ ][d][ ][e][f][ ][g][ ][a][ ][b]
// 0  1  2  3  4  5  6  7  8  9  0  1
case 65: n=9; break;
case 66: n=11; break;
case 67: n=0; break;
case 68: n=2; break;
case 69: n=4; break;
case 70: n=5; break;
case 71: n=7; break;
}
n=n+(o-2)*12-9;
followup=1;
goto next_byte;
}//a

if (a==76){//L
followup=2;
goto next_byte;
}

if (a==77){//M
followup=5;
goto next_byte;
}

if (a==78){//N
followup=4;
goto next_byte;
}

if (a==79){//O
followup=3;
goto next_byte;
}

if (a==84){//T
followup=6;
goto next_byte;
}

if (a==60){//<
o--; if (o<0) o=0;
goto next_byte;
}

if (a==62){//>
o++; if (o>6) o=6;
goto next_byte;
}

if (a==80){//P
followup=7;
goto next_byte;
}

if (a==86){//V
followup=8;
goto next_byte;
}

error(5); return;
}//bytes_left
done:
if (number_entered||followup){error(5); return;}//unhandled data




if (playit){
handle=func__sndraw(wave,wave_bytes);
if (handle){
sndqueue[sndqueue_next]=handle;
sndqueue_next++; if (sndqueue_next>sndqueue_lastindex) sndqueue_next=0;
}

//creating a "blocking" sound object
if (!mb){
wave=(uint8*)calloc(4,1); handle=func__sndraw(wave,4);
if (handle){
sndqueue_wait=sndqueue_next; suspend_program|=2; qbevent=1;
sndqueue[sndqueue_next]=handle;
sndqueue_next++; if (sndqueue_next>sndqueue_lastindex) sndqueue_next=0;
}else{free(wave);}

}//!mb




}//playit

}



//2D PROTOTYPE QB64<->C CALLS

//Creating/destroying an image surface:

int32 func__newimage(int32 x,int32 y,int32 bpp,int32 passed){
static int32 i;
if (new_error) return 0;
if (x<=0||y<=0){error(5); return 0;}
if (!passed){
bpp=write_page->compatible_mode;
}else{
i=0;
if (bpp>=0&&bpp<=2) i=1;
if (bpp>=7&&bpp<=13) i=1;
if (bpp==256) i=1;
if (bpp==32) i=1;
if (!i){error(5); return 0;}
}
i=imgnew(x,y,bpp);
if (!i) return -1;
if (!passed){
//adopt palette
if (write_page->pal){
memcpy(img[i].pal,write_page->pal,1024);
}
//adopt font
sub__font(write_page->font,-i,1);
//adopt colors
img[i].color=write_page->color;
img[i].background_color=write_page->background_color;
//adopt transparent color
img[i].transparent_color=write_page->transparent_color;
//adopt blend state
img[i].alpha_disabled=write_page->alpha_disabled;
//adopt print mode
img[i].print_mode=write_page->print_mode;
}
return -i;
}

int32 func__loadimage(qbs *f,int32 bpp,int32 passed){
static qbs *tqbs=NULL,*nullt=NULL;
static int32 i;
if (new_error) return 0;
//validate bpp
if (passed){
if ((bpp!=32)&&(bpp!=256)){error(5); return 0;}
}else{
if (write_page->text){error(5); return 0;}
bpp=-1;
}
if (!f->len) return -1;//return invalid handle if null length string
if (!tqbs) tqbs=qbs_new(0,0);
if (!nullt){nullt=qbs_new(1,0); nullt->chr[0]=0;}
qbs_set(tqbs,qbs_add(f,nullt));
i=imgload(fixdir(tqbs),bpp);
if (!i) return -1;//failed
return -i;
}

int32 func__copyimage(int32 i,int32 passed){
static int32 i2,bytes;
static img_struct *s,*d;
if (new_error) return 0;
if (passed){
  if (i>=0){//validate i
  validatepage(i); i=page[i];
  }else{
  i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
 }else{
 i=write_page_index;
 }
s=&img[i];
//duplicate structure
i2=newimg();
d=&img[i2];
memcpy(d,s,sizeof(img_struct));
//duplicate pixel data
bytes=d->width*d->height*d->bytes_per_pixel;
d->offset=(uint8*)malloc(bytes);
if (!d->offset){freeimg(i2); return -1;}
memcpy(d->offset,s->offset,bytes);
d->flags|=IMG_FREEMEM;
//duplicate palette
if (d->pal){
d->pal=(uint32*)malloc(1024);
if (!d->pal){free(d->offset); freeimg(i2); return -1;}
memcpy(d->pal,s->pal,1024);
d->flags|=IMG_FREEPAL;
}
//adjust flags
if (d->flags&IMG_SCREEN)d->flags^=IMG_SCREEN;
//return new handle
return -i2;
}

void sub__freeimage(int32 i,int32 passed){
if (new_error) return;
 if (passed){
  if (i>=0){//validate i
  error(5); return;//The SCREEN's pages cannot be freed!
  }else{
  i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;}
 }
 }else{
 i=write_page_index;
 }
if (img[i].flags&IMG_SCREEN){error(5); return;}//The SCREEN's pages cannot be freed!
if (write_page_index==i) sub__dest(-display_page_index);
if (read_page_index==i) sub__source(-display_page_index);
if (img[i].flags&IMG_FREEMEM) free(img[i].offset);//free pixel data
if (img[i].flags&IMG_FREEPAL) free(img[i].pal);//free palette
freeimg(i);
}

void freeallimages(){
static int32 i;
//note: handles 0 & -1(1) are reserved
for (i=2;i<nextimg;i++){
if (img[i].valid){
if ((img[i].flags&IMG_SCREEN)==0){//The SCREEN's pages cannot be freed!
sub__freeimage(-i,1);
}
}//valid
}//i
}

//Selecting images:

void sub__source(int32 i){ 
if (new_error) return;
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;} 
 }
read_page_index=i; read_page=&img[i];
}

void sub__dest(int32 i){ 
if (new_error) return;
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;} 
 }
write_page_index=i; write_page=&img[i];
}

int32 func__source(){
return -read_page_index;
}

int32 func__dest(){
return -write_page_index;
}

int32 func__display(){
return -display_page_index;
}

//Changing the settings of an image surface:

void sub__blend(int32 i,int32 passed){
if (new_error) return;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;} 
 }
}else{
i=write_page_index;
}
if (img[i].bytes_per_pixel!=4){error(5); return;}
img[i].alpha_disabled=0;
}

void sub__dontblend(int32 i,int32 passed){
if (new_error) return;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;} 
 }
}else{
i=write_page_index;
}
if (img[i].bytes_per_pixel!=4) return;
img[i].alpha_disabled=1;
}


void sub__clearcolor(uint32 c,int32 i,int32 passed){
//--         _NONE->1       2       4
//id.specialformat = "[{_NONE}][?][,?]"
if (new_error) return;
static img_struct *im;
static int32 z;
static uint32 *lp,*last;
static uint8 b_max,b_min,g_max,g_min,r_max,r_min;
static uint8 *cp,*clast,v;
if (passed&4){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;} 
 }
}else{
 i=write_page_index;
}
im=&img[i];
//text?
if (im->text){
if ((passed&1)&&(!(passed&2))) return; //you can disable clearcolor using _CLEARCOLOR _NONE in text modes
error(5); return;
}
//palette?
if (im->pal){
if (passed&1){
if (passed&2){error(5); return;}//invalid options
im->transparent_color=-1;
return;
}
if (!(passed&2)){error(5); return;}//invalid options
if (c>255){error(5); return;}//invalid color
im->transparent_color=c;
return;
}
//32-bit? (alpha is ignored in this case)
if (passed&1){
if (passed&2){error(5); return;}//invalid options
return;//no action
}
if (!(passed&2)){error(5); return;}//invalid options
c&=0xFFFFFF;
last=im->offset32+im->width*im->height;
for (lp=im->offset32;lp<last;lp++){
if ((*lp&0xFFFFFF)==c) *lp=c;
}
return;
}

//Changing/Using an image surface:

//_PUT "[(?,?)[-(?,?)]][,[?][,[?][,[(?,?)[-(?,?)]]]]]"
//(defined elsewhere)

//_IMGALPHA "?[,[?[{TO}?]][,?]]"
void sub__setalpha(int32 a,uint32 c,uint32 c2,int32 i,int32 passed){
//-->                             1        4        2
static img_struct *im;
static int32 z;
static uint32 *lp,*last;
static uint8 b_max,b_min,g_max,g_min,r_max,r_min,a_max,a_min;
static uint8 *cp,*clast,v;
if (new_error) return;
if (passed&2){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;} 
 }
}else{
i=write_page_index;
}
im=&img[i];
if (im->pal){error(5); return;}//does not work on paletted images!
if (a<0||a>255){error(5); return;}//invalid range
if (passed&4){
//ranged
if (c==c2) goto uniquerange;
b_min=c&0xFF;  g_min=c>>8&0xFF;  r_min=c>>16&0xFF; a_min=c>>24&0xFF;
b_max=c2&0xFF; g_max=c2>>8&0xFF; r_max=c2>>16&0xFF; a_max=c2>>24&0xFF;
if (b_min>b_max) swap(b_min,b_max);

if (g_min>g_max) swap(g_min,g_max);
if (r_min>r_max) swap(r_min,r_max);
if (a_min>a_max) swap(a_min,a_max);
cp=im->offset;
z=im->width*im->height;
setalpha:
if (z--){
v=*cp; if (v<=b_max&&v>=b_min){
v=*(cp+1); if (v<=g_max&&v>=g_min){
v=*(cp+2); if (v<=r_max&&v>=r_min){
v=*(cp+3); if (v<=a_max&&v>=a_min){
*(cp+3)=a;
}}}}
cp+=4;
goto setalpha;
}
return;
}
if (passed&1){
uniquerange:
//alpha of c=a
c2=a<<24;
lp=im->offset32-1;
last=im->offset32+im->width*im->height-1;
while (lp<last){
if (*++lp==c){
*lp=(*lp&0xFFFFFF)|c2;
}
}
return;
}
//all alpha=a
cp=im->offset-1;
clast=im->offset+im->width*im->height*4-4;
while (cp<clast){*(cp+=4)=a;}
return;
}

//Finding infomation about an image surface:

int32 func__width(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
return img[i].width;
}

int32 func__height(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
return img[i].height;
}

int32 func__pixelsize(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
i=img[i].compatible_mode;
if (i==32) return 4;
if (!i) return 0;
return 1;
}

int32 func__clearcolor(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
if (img[i].text) return -1;
if (img[i].compatible_mode==32) return 0;
return img[i].transparent_color;
}

int32 func__blend(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
if (img[i].compatible_mode==32){
if (!img[i].alpha_disabled) return -1;
}
return 0;
}

uint32 func__defaultcolor(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
return img[i].color;
}

uint32 func__backgroundcolor(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
return img[i].background_color;
}

//Working with 256 color palettes:

uint32 func__palettecolor(int32 n,int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
if (!img[i].pal){error(5); return 0;}
if (n<0||n>255){error(5); return 0;}//out of range
return img[i].pal[n]|0xFF000000;
}

void sub__palettecolor(int32 n,uint32 c,int32 i,int32 passed){
if (new_error) return;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;}
 }
}else{
i=write_page_index;
}
if (!img[i].pal){error(5); return;}
if (n<0||n>255){error(5); return;}//out of range
img[i].pal[n]=c;
}

void sub__copypalette(int32 i,int32 i2,int32 passed){
if (new_error) return;
if (passed&1){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;}
 }
}else{
i=read_page_index;
}
if (!img[i].pal){error(5); return;}
swap(i,i2);
if (passed&2){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;}
 }
}else{
i=write_page_index;
}
if (!img[i].pal){error(5); return;}
swap(i,i2);
memcpy(img[i2].pal,img[i].pal,1024);
}



void sub__printstring(float x,float y,qbs* text,int32 i,int32 passed){
if (new_error) return;
if (passed&2){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;}
 }
}else{
i=write_page_index;
}
static img_struct *im;
im=&img[i];
if (im->text){error(5); return;}//graphics modes only
if (!text->len) return;
//Step?
if (passed&1){im->x+=x; im->y+=y;}else{im->x=x; im->y=y;}
//Adjust co-ordinates for viewport?
static int32 x2,y2;
if (im->clipping_or_scaling){
 if (im->clipping_or_scaling==2){
  x2=qbr_float_to_long(im->x*im->scaling_x+im->scaling_offset_x)+im->view_offset_x;
  y2=qbr_float_to_long(im->y*im->scaling_y+im->scaling_offset_y)+im->view_offset_y;
 }else{
  x2=qbr_float_to_long(im->x)+im->view_offset_x; y2=qbr_float_to_long(im->y)+im->view_offset_y;
 }
}else{
 x2=qbr_float_to_long(im->x); y2=qbr_float_to_long(im->y); 
}
static int32 i2,w;
for (i2=0;i2<text->len;i2++){
w=printchr2(x2,y2,text->chr[i2],i);
x2=x2+w;
}
}



int32 func__printwidth(qbs* text,int32 i,int32 passed){
if (new_error) return 0;
static int32 i2,w2;
static uint32 character;
static uint32 x2,w,f;
static SDL_Surface *ts;
static SDL_Color c,c2;
static img_struct *im;

if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
im=&img[i];
if (im->text){error(5); return 0;}//graphics modes only
if (!text->len) return 0;

//default is no kerning, assume kerning is off

w2=0;
for (i2=0;i2<text->len;i2++){
character=text->chr[i2];

//precalculations
character&=255;
f=im->font;
c.r=255; c.g=255; c.b=255; c.unused=0;//dummy values
c2.r=255; c2.g=255; c2.b=255; c2.unused=0;//dummy values

if (f>=32){//custom font

//8-bit / alpha-disabled 32-bit / dont-blend(alpha may still be applied)
if ((im->bytes_per_pixel==1)||((im->bytes_per_pixel==4)&&(im->alpha_disabled))||(fontflags[f]&8)){

convert_text_to_utf16(f,&character,1);
ts=TTF_RenderUNICODE_Solid(font[f],(Uint16*)unicode16_buf,c);
//ts=TTF_RenderText_Solid(font[f],(char*)&character,c);//8-bit, 0=clear, 1=text

if (ts==NULL) return 0;
w=ts->w;
if (x2=fontwidth[f]) if (x2!=w) w=x2;//render width too large!  
SDL_FreeSurface(ts);
w2+=w;
goto nexti2;
}//1-8 bit

//assume 32-bit blended
//8 bit, 0=background -> 255=foreground

convert_text_to_utf16(f,&character,1);
ts=TTF_RenderUNICODE_Shaded(font[f],(Uint16*)unicode16_buf,c,c2);
//ts=TTF_RenderText_Shaded(font[f],(char*)&character,c,c2);

if (ts==NULL) return 0;
w=ts->w;
if (x2=fontwidth[f]) if (x2!=w) w=x2;//render width too large!  
SDL_FreeSurface(ts);
w2+=w;
goto nexti2;

}//custom font

//default fonts
w2+=8;

nexti2:;
}//i2

return w2;
}//printwidth


int32 func__loadfont(qbs *filename,double size,qbs *requirements,int32 passed){
//make a font one line higher than before
//ignore row 0 when rendering

//f=_FONTLOAD(ttf_filename$,height[,"bold,italic,underline,monospace,dontblend,unicode"])
//1 bold TTF_STYLE_BOLD
//2 italic TTF_STYLE_ITALIC
//4 underline TTF_STYLE_UNDERLINE
//8 dontblend (blending is the default in 32-bit alpha-enabled modes)
//16 monospace
//32 unicode
if (new_error) return 0;
static qbs *s1=NULL;
if (!s1) s1=qbs_new(0,0);
static qbs *req=NULL;
if (!req) req=qbs_new(0,0);
static qbs *s3=NULL;
if (!s3) s3=qbs_new(0,0);
static uint8 r[32];
static int32 i,i2,i3;
//validate
if (size<0.001){error(5); return 0;}
if (size>=1000.5) return 0;
memset(r,0,32);
if (passed){
if (requirements->len){
i=1;
qbs_set(req,qbs_ucase(requirements));//convert tmp str to perm str
nextrequirement:
i2=func_instr(i,req,qbs_new_txt(","),1);
if (i2){
qbs_set(s1,func_mid(req,i,i2-i,1));
}else{
qbs_set(s1,func_mid(req,i,req->len-i+1,1));
}
qbs_set(s1,qbs_rtrim(qbs_ltrim(s1)));
if (qbs_equal(s1,qbs_new_txt("BOLD"))){r[0]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("ITALIC"))){r[1]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("UNDERLINE"))){r[2]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("DONTBLEND"))){r[3]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("MONOSPACE"))){r[4]++; goto valid;}
if (qbs_equal(s1,qbs_new_txt("UNICODE"))){r[5]++; goto valid;}
error(5); return 0;//invalid requirements
valid:
if (i2){i=i2+1; goto nextrequirement;}
for (i=0;i<32;i++) if (r[i]>1){error(5); return 0;}//cannot define requirements twice
}//->len
}//passed
qbs_set(s1,qbs_add(filename,qbs_new_txt_len("\0",1)));//s1=filename+CHR$(0)
i=r[0]+(r[1]<<1)+(r[2]<<2)+(r[3]<<3)+(r[4]<<4)+(r[5]<<5);
i=fontopen(fixdir(s1),size,i);
if (!i) return -1;
return i;
}

void sub__font(int32 f,int32 i,int32 passed){
//_FONT "?[,?]"
static int32 i2;
static img_struct *im;
if (new_error) return;
if (passed&1){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;}
 }
}else{
i=write_page_index;
}
im=&img[i];
//validate f
i2=0;
if (f==8) i2=1;
if (f==9) i2=1;
if (f==14) i2=1;
if (f==15) i2=1;
if (f==16) i2=1;
if (f==17) i2=1;
if (f>=32&&f<=lastfont){
if (font[f]) i2=1;
}
if (!i2){error(258); return;}

if (im->text&&((fontflags[f]&16)==0)){error(5); return;}//only monospace fonts can be used on text surfaces
//note: font changes to text screen mode images requires:
//      i) font change across all screen pages
//      ii) locking of the display
//      iii) update of the data being displayed
if (im->text){
if (im->flags&IMG_SCREEN){
//lock display
if (autodisplay){
if (lock_display==0) lock_display=1;//request lock
while (lock_display!=2) Sleep(0);
}
//force update of data
screen_last_valid=0;//ignore cache used to update the screen on next update
//apply change across all video pages
for(i=0;i<pages;i++){
if(page[i]){
im=&img[page[i]];
im->font=f;
//note: moving the cursor is unnecessary
}
}
//unlock
if (autodisplay){
if (lock_display_required) lock_display=0;//release lock
}
return;
}
}//text

im->font=f;
im->cursor_x=1; im->cursor_y=1;
im->top_row=1;
if (im->compatible_mode) im->bottom_row=im->height/fontheight[f]; else im->bottom_row=im->height;
im->bottom_row--; if (im->bottom_row<=0) im->bottom_row=1;
return;
}

int32 func__fontwidth(int32 f,int32 passed){
static int32 i2;
if (new_error) return 0;
if (passed){
//validate f
i2=0;
if (f==8) i2=1;
if (f==14) i2=1;
if (f==16) i2=1;
if (f>=32&&f<=lastfont){
if (font[f]) i2=1;
}
if (!i2){error(258); return 0;}
}else{
f=write_page->font;
}
return fontwidth[f];
}

int32 func__fontheight(int32 f,int32 passed){
static int32 i2;
if (new_error) return 0;
if (passed){
//validate f
i2=0;
if (f==8) i2=1;
if (f==14) i2=1;
if (f==16) i2=1;
if (f>=32&&f<=lastfont){
if (font[f]) i2=1;
}
if (!i2){error(258); return 0;}
}else{
f=write_page->font;
}
return fontheight[f];
}

int32 func__font(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
return img[i].font;
}

void sub__freefont(int32 f){
if (new_error) return;
static int32 i,i2;
//validate f (cannot remove QBASIC built in fonts!)
i2=0;
if (f>=32&&f<=lastfont){
if (font[f]) i2=1;
}
if (!i2){error(258); return;}
//check all surfaces, no surface can be using the font
for (i=1;i<nextimg;i++){
if (img[i].valid){
if (img[i].font==f){error(5); return;}
}
}
//remove font
TTF_CloseFont(font[f]);
font[f]=NULL;
}

void sub__printmode(int32 mode,int32 i,int32 passed){
if (new_error) return;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;}
 }
}else{
i=write_page_index;
}
if (img[i].text){
if (mode!=1){error(5); return;}
}
if (mode==1) img[i].print_mode=3;//fill
if (mode==2) img[i].print_mode=1;//keep
if (mode==3) img[i].print_mode=2;//only
}

int32 func__printmode(int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
}else{
i=write_page_index;
}
return img[i].print_mode;
}


uint32 matchcol(int32 r,int32 g,int32 b){
static int32 v,v2,n,n2,best,c;
static int32 *p;
p=(int32*)write_page->pal;
if (write_page->text) n2=16; else n2=write_page->mask+1;
v=1000;
best=0;
for (n=0;n<n2;n++){
c=*p++;
v2=abs(b-(c&0xFF))+abs(g-(c>>8&0xFF))+abs(r-(c>>16&0xFF));
if (v2<v){
if (!v2) return n;//perfect match
v=v2;
best=n;
}
}//n
return best;
}

uint32 matchcol(int32 r,int32 g,int32 b,int32 i){
static int32 v,v2,n,n2,best,c;
static int32 *p;
p=(int32*)img[i].pal;
if (img[i].text) n2=16; else n2=img[i].mask+1;
v=1000;
best=0;
for (n=0;n<n2;n++){
c=*p++;
v2=abs(b-(c&0xFF))+abs(g-(c>>8&0xFF))+abs(r-(c>>16&0xFF));
if (v2<v){
if (!v2) return n;//perfect match
v=v2;
best=n;
}
}//n
return best;
}

uint32 func__rgb(int32 r,int32 g,int32 b,int32 i,int32 passed){
if (new_error) return 0;
if (r<0) r=0;
if (r>255) r=255;
if (g<0) g=0;
if (g>255) g=255;
if (b<0) b=0;
if (b>255) b=255;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
 if (img[i].bytes_per_pixel==4){
 return (r<<16)+(g<<8)+b|0xFF000000;
 }else{//==4
 return matchcol(r,g,b,i);
 }//==4
}else{
 if (write_page->bytes_per_pixel==4){
 return (r<<16)+(g<<8)+b|0xFF000000;
 }else{//==4
 return matchcol(r,g,b);
 }//==4
}//passed
}//rgb

uint32 func__rgba(int32 r,int32 g,int32 b,int32 a,int32 i,int32 passed){
if (new_error) return 0;
if (r<0) r=0;
if (r>255) r=255;
if (g<0) g=0;
if (g>255) g=255;
if (b<0) b=0;
if (b>255) b=255;
if (a<0) a=0;
if (a>255) a=255;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
 if (img[i].bytes_per_pixel==4){
 return (a<<24)+(r<<16)+(g<<8)+b;
 }else{//==4
 //error(5); return 0;
 if ((!a)&&(img[i].transparent_color!=-1)) return img[i].transparent_color;
 return matchcol(r,g,b,i);
 }//==4
}else{
 if (write_page->bytes_per_pixel==4){
 return (a<<24)+(r<<16)+(g<<8)+b;
 }else{//==4
 //error(5); return 0;
 if ((!a)&&(write_page->transparent_color!=-1)) return write_page->transparent_color;
 return matchcol(r,g,b);
 }//==4
}//passed
}//rgba

int32 func__alpha(uint32 col,int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
 if (img[i].bytes_per_pixel==4){
 return col>>24;
 }else{//==4
 //error(5); return 0; 
 if ((col<0)||(col>(img[i].mask))){error(5); return 0;} 
 if (img[i].transparent_color==col) return 0;
 return 255;
 }//==4
}else{
 if (write_page->bytes_per_pixel==4){
 return col>>24;
 }else{//==4
 //error(5); return 0; 
 if ((col<0)||(col>(write_page->mask))){error(5); return 0;} 
 if (write_page->transparent_color==col) return 0;
 return 255;
 }//==4
}//passed
}

int32 func__red(uint32 col,int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
 if (img[i].bytes_per_pixel==4){
 return col>>16&0xFF;
 }else{//==4
 if ((col<0)||(col>(img[i].mask))){error(5); return 0;}
 return img[i].pal[col]>>16&0xFF;
 }//==4
}else{
 if (write_page->bytes_per_pixel==4){
 return col>>16&0xFF;
 }else{//==4
 if ((col<0)||(col>(write_page->mask))){error(5); return 0;}
 return write_page->pal[col]>>16&0xFF;
 }//==4
}//passed
}

int32 func__green(uint32 col,int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}
 }
 if (img[i].bytes_per_pixel==4){
 return col>>8&0xFF;
 }else{//==4
 if ((col<0)||(col>(img[i].mask))){error(5); return 0;}
 return img[i].pal[col]>>8&0xFF;
 }//==4
}else{
 if (write_page->bytes_per_pixel==4){
 return col>>8&0xFF;
 }else{//==4
 if ((col<0)||(col>(write_page->mask))){error(5); return 0;}
 return write_page->pal[col]>>8&0xFF;
 }//==4
}//passed
}

int32 func__blue(uint32 col,int32 i,int32 passed){
if (new_error) return 0;
if (passed){
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return 0;} if (!img[i].valid){error(258); return 0;}

 }
 if (img[i].bytes_per_pixel==4){
 return col&0xFF;
 }else{//==4
 if ((col<0)||(col>(img[i].mask))){error(5); return 0;}
 return img[i].pal[col]&0xFF;
 }//==4
}else{
 if (write_page->bytes_per_pixel==4){
 return col&0xFF;
 }else{//==4
 if ((col<0)||(col>(write_page->mask))){error(5); return 0;}
 return write_page->pal[col]&0xFF;
 }//==4
}//passed
}

void sub_end(){
sub_close(NULL,0);
exit_blocked=0;//allow exit via X-box or CTRL+BREAK
//1. set the display page as the destination page
sub__dest(func__display());
//2. VIEW PRINT bottomline,bottomline
static int32 y;
if (write_page->text){
 y=write_page->height;
}else{
 y=write_page->height/fontheight[write_page->font];
}
qbg_sub_view_print(y,y,1|2);
//3. PRINT 'clears the line without having to worry about its contents/size
qbs_print(nothingstring,1);
//4. PRINT "Press any key to continue"
qbs_print(qbs_new_txt("Press any key to continue"),0);
//5. Clear any buffered keypresses
static uint32 qbs_tmp_base;
qbs_tmp_base=qbs_tmp_list_nexti;
while(qbs_cleanup(qbs_tmp_base,qbs_notequal(qbs_inkey(),qbs_new_txt("")))){
SDL_Delay(0);
}
//6. Enable autodisplay
autodisplay=1;
//7. Wait for a new keypress
do{
SDL_Delay(0);
if (stop_program) end();
}while(qbs_cleanup(qbs_tmp_base,qbs_equal(qbs_inkey(),qbs_new_txt(""))));
//8. end program
close_program=1;
end();
exit(0);//<-- should never happen
}

uint8 pu_dig[1024];//digits (left justified)
int32 pu_ndig;//number of digits
int32 pu_dp;//decimal place modifier
//note: if dp=0, the number is an integer and can be read as is
//      if dp=1 the number is itself*10
//      if dp=-1 the number is itself/10
int32 pu_neg;
uint8 pu_buf[1024];//a buffer for preprocessing
uint8 pu_exp_char=69; //"E"

int32 print_using(qbs *f, int32 s2, qbs *dest, qbs* pu_str){
//type: 1=numeric, 2=string
if (new_error) return 0;

static int32 x,z,z2,z3,z4,ii;
//x  - current format string read position
//z - used as a temp variable for various calculations and loops
//z2  - used for various calculations involving exponent digits
//z3 - used as a temp variable for various calculations and loops
//z4 - number of 0s between . and digits after point
//ii  - used as a counter for writing the output
static uint8 c;
static int32 stage,len,chrsleft,type,s;
static int32 leading_plus,dollar_sign,asterisk_spaces,digits_before_point,commas;
static int32 decimal_point,digits_after_point,trailing_plus,exponent_digits, trailing_minus;
static int32 cant_fit,extra_sign_space,rounded,digits_and_commas_before_point,leading_zero;
static qbs *qbs1=NULL;

if (qbs1==NULL) qbs1=qbs_new(1,0);

if (pu_str) type=2; else type=1;

s=s2;
len=f->len;

scan:
rounded=0;
rounded_repass:

x=s-1; //subtract one to counter pre-increment later

leading_plus=0; dollar_sign=0; asterisk_spaces=0; digits_before_point=0; commas=0;
decimal_point=0; digits_after_point=0; trailing_plus=0; exponent_digits=0; trailing_minus=0;
digits_and_commas_before_point=0; leading_zero=0;
stage=0;

nextchar:
x++;
if (x<len){
c=f->chr[x];
chrsleft=len-x;

if ((stage>=2)&&(stage<=4)){

if (c==43){//+
trailing_plus=1; x++; goto numeric_spacer;
}

if (c==45){//-
trailing_minus=1; x++; goto numeric_spacer;
}

}//stage>=2 & stage<=4

if ((stage>=2)&&(stage<=3)){

if (chrsleft>=5){
if ((c==94)&&(f->chr[x+1]==94)&&(f->chr[x+2]==94)&&(f->chr[x+3]==94)&&(f->chr[x+4]==94)){//^^^^^
exponent_digits=3; stage=4; x+=4; goto nextchar;
}
}//5

if (chrsleft>=4){
if ((c==94)&&(f->chr[x+1]==94)&&(f->chr[x+2]==94)&&(f->chr[x+3]==94)){//^^^^
exponent_digits=2; stage=4; x+=3; goto nextchar;
}
}//4

}//stage>=2 & stage<=3

if (stage==3){

if (c==35){//#
digits_after_point++; goto nextchar;
}

}//stage==3

if (stage==2){

if (c==44){//,
commas=1; digits_before_point++; goto nextchar;
}

}//stage==2

if (stage<=2){

if (c==35){//#
digits_before_point++; stage=2; goto nextchar;
}

if (c==46){//.
decimal_point=1; stage=3; goto nextchar;
}

}//stage<=2

if (stage<=1){

if (chrsleft>=3){
if ((c==42)&&(f->chr[x+1]==42)&&(f->chr[x+2]==36)){//**$
asterisk_spaces=1; digits_before_point=2; dollar_sign=1; stage=2; x+=2; goto nextchar;
}
}//3

if (chrsleft>=2){
if ((c==42)&&(f->chr[x+1]==42)){//**
asterisk_spaces=1; digits_before_point=2; stage=2; x++; goto nextchar;
}
if ((c==36)&&(f->chr[x+1]==36)){//$$
dollar_sign=1; digits_before_point=1; stage=2; x++; goto nextchar;
}
}//2

}//stage 1

if (stage==0){

if (c==43){//+
leading_plus=1; stage=1; goto nextchar;
}

}//stage 0

//spacer/end encountered
}//x<len
numeric_spacer:

//valid numeric format?
if (stage<=1) goto invalid_numeric_format;
if ((digits_before_point==0)&&(digits_after_point==0)) goto invalid_numeric_format;

if (type==0) return s; //s is the beginning of a new format but item has already been added to dest
if (type==2){//expected string format, not numeric format
error(13);//type mismatch
return 0;
}

//reduce digits before point appropriatly
extra_sign_space=0;
if (exponent_digits){
 if ((leading_plus==0)&&(trailing_plus==0)&&(trailing_minus==0)){digits_before_point--; extra_sign_space=1;}
}else{
 //the following doesn't occur if using an exponent
 if (pu_neg){
 if ((leading_plus==0)&&(trailing_plus==0)&&(trailing_minus==0)){digits_before_point--; extra_sign_space=1;}
 }
 if (commas){
 digits_and_commas_before_point=digits_before_point;
 ii=digits_before_point/4;//for every 4 digits, one digit will be used up by a comma
 digits_before_point-=ii;
 }
}

//'0'->'.0' exception (for when format doesn't allow for any digits_before_point)
if (digits_before_point==0){//no digits allowed before decimal point
//note: pu_ndig=256, pu_dp=-255
if ((pu_ndig+pu_dp)==1){//1 digit exists in front of the decimal point
if (pu_dig[0]==48){//is it 0?
pu_dp--;//moves decimal point left one position
}//0
}
}

//will number fit? if it can't then adjustments will be made
cant_fit=0;
if (exponent_digits){
 //give back extra_sign_space?
 if (extra_sign_space){
 if (!pu_neg){
 if (digits_before_point<=0){
 extra_sign_space=0;
 digits_before_point++;//will become 0 or 1
  //force 0 in recovered digit?
  if ((digits_before_point==1)&&(digits_after_point>0)){
  digits_before_point--;
  extra_sign_space=2;//2=put 0 instead of blank space
  }
 }
 }
 }
 if ((digits_before_point==0)&&(digits_after_point==0)){
 cant_fit=1;
 digits_before_point=1;//give back removed (for extra sign space) digit
 }
 //but does the exponent fit?
 z2=pu_ndig+pu_dp-1;//calc exponent of most significant digit
                //1.0  = 0
                //10.0 = 1
                //0.1  = -1
 //calc exponent of format's most significant position
 if (digits_before_point) z3=digits_before_point-1; else z3=-1;
 z=z2-z3;//combine to calculate actual exponent which will be "printed" 
 z3=abs(z);
 z2=sprintf((char*)pu_buf,"%u",z3);//use pu_buf to convert exponent to a string
 if (z2>exponent_digits){cant_fit=1; exponent_digits=z2;}
}else{
 z2=0;
 z=pu_ndig+pu_dp;//calc number of digits required before decimal places
 if (digits_before_point<z){
 digits_before_point=z; cant_fit=1;
 if (commas) digits_and_commas_before_point=digits_before_point+(digits_before_point-1)/3;
 }
}



static int32 buf_size;//buf_size is an estimation of size required
static uint8 *cp,*buf=NULL;
static int32 count;
if (buf) free(buf);
buf_size=256;//256 bytes to account for calc overflow (such as exponent digits)
buf_size+=9;//%(1)+-(1)$(1)???.(1)???exponent(5)
buf_size+=digits_before_point;
if (commas) buf_size+=((digits_before_point/3)+2);
buf_size+=digits_after_point;
buf=(uint8*)malloc(buf_size);
cp=buf;
count=0;//char count
ii=0;

if (asterisk_spaces) asterisk_spaces=42; else asterisk_spaces=32;//chraracter to fill blanks with

if (cant_fit) {*cp++=37; count++;}//%

//leading +/-
if (leading_plus){
if (pu_neg) *cp++=45; else *cp++=43;
count++;
}

if (exponent_digits){
 z4=0;
 //add $?
 if (dollar_sign) {*cp++=36; count++;}//$
 //add - sign? (as sign space was not specified)
 if (extra_sign_space){
 if (pu_neg){
 *cp++=45;
 }else{
 if (extra_sign_space==2) *cp++=48; else *cp++=32;
 }
 count++;
 }
 //add digits left of decimal point
 for (z3=0;z3<digits_before_point;z3++){
 if (ii<pu_ndig) *cp++=pu_dig[ii++]; else *cp++=48;
 count++;
 }
 //add decimal point
 if (decimal_point){*cp++=46; count++;}
 //add digits right of decimal point
 for (z3=0;z3<digits_after_point;z3++){
 if (ii<pu_ndig) *cp++=pu_dig[ii++]; else *cp++=48;
 count++;
 }
 //round last digit (requires a repass)
 if (!rounded){
 if (ii<pu_ndig){
 if (pu_dig[ii]>=53){//>="5" (round 5 up)
 z=ii-1;
  //round up pu (by adding 1 from digit at character position z)
  //note: pu_dig is rebuilt one character to the right so highest digit can flow over into next character
  rounded=1;
  memmove(&pu_dig[1],&pu_dig[0],pu_ndig); pu_dig[0]=48; z++;
  puround2:
  pu_dig[z]++;
  if (pu_dig[z]>57) {pu_dig[z]=48; z--; goto puround2;}
  if (pu_dig[0]!=48){//was extra character position necessary?
  pu_ndig++; //note: pu_dp does not require any changes  
  }else{
  memmove(&pu_dig[0],&pu_dig[1],pu_ndig);
  }
  goto rounded_repass;
 }
 }
 }
 //add exponent...
 *cp++=pu_exp_char; count++; //add exponent D/E/F (set and restored by calling function as necessary)
 if (z>=0) {*cp++=43; count++;} else {*cp++=45; count++;} //+/- exponent's sign
 //add exponent's leading 0s (if any)
 for (z3=0;z3<(exponent_digits-z2);z3++){
 *cp++=48; count++;
 }
 //add exponent's value
 for (z3=0;z3<z2;z3++){
 *cp++=pu_buf[z3]; count++;
 }
}else{
 //"print" everything before the point
 //calculate digit spaces before the point in number
 if (!commas) digits_and_commas_before_point=digits_before_point;
 z=pu_ndig+pu_dp;//num of character whole portion of number requires
 z4=0; if (z<0) z4=-z;//number of 0s between . and digits after point
 if (commas) z=z+(z-1)/3;//including appropriate amount of commas
 if (z<0) z=0;
 z2=digits_and_commas_before_point-z;
 if ((z==0)&&(z2>0)){leading_zero=1; z2--;}//change .1 to 0.1 if possible
 for (z3=0;z3<z2;z3++){*cp++=asterisk_spaces; count++;}
 //add - sign? (as sign space was not specified)
 if (extra_sign_space){*cp++=45; count++;}
 //add $?
 if (dollar_sign){*cp++=36; count++;}//$ 
 //leading 0?
 if (leading_zero){*cp++=48; count++;}//0
 //add digits left of decimal point
 for (z3=0;z3<z;z3++){
 if ((commas!=0)&&(((z-z3)&3)==0)){
 *cp++=44;
 }else{
 if (ii<pu_ndig) *cp++=pu_dig[ii++]; else *cp++=48;
 }
 count++;
 }
 //add decimal point
 if (decimal_point){*cp++=46; count++;}
 //add digits right of decimal point
 for (z3=0;z3<digits_after_point;z3++){
 if (z4){
 z4--;
 *cp++=48;
 }else{
 if (ii<pu_ndig) *cp++=pu_dig[ii++]; else *cp++=48;
 }
 count++;
 }
 //round last digit (requires a repass)
 if (!rounded){
 if (ii<pu_ndig){
 if (pu_dig[ii]>=53){//>="5" (round 5 up)
 z=ii-1;
  //round up pu (by adding 1 from digit at character position z)
  //note: pu_dig is rebuilt one character to the right so highest digit can flow over into next character
  rounded=1;  
  memmove(&pu_dig[1],&pu_dig[0],pu_ndig); pu_dig[0]=48; z++;
  puround1: 
  pu_dig[z]++;
  if (pu_dig[z]>57) {pu_dig[z]=48; z--; goto puround1;}
  if (pu_dig[0]!=48){//was extra character position necessary?
  pu_ndig++; //note: pu_dp does not require any changes  
  }else{
  memmove(&pu_dig[0],&pu_dig[1],pu_ndig);  
  }
  goto rounded_repass;
 } 
 }
 }
}//exponent_digits

//add trailing sign?
//trailing +/-
if (trailing_plus){
if (pu_neg) *cp++=45; else *cp++=43;
count++;
}
//trailing -
if (trailing_minus){
if (pu_neg) *cp++=45; else *cp++=32;
count++;
}

qbs_set(dest,qbs_add(dest,qbs_new_txt_len((char*)buf,count)));

s=x;
type=0;//passed type added
if (s>=len) return 0;//end of format line encountered and passed item added
goto scan;

invalid_numeric_format:
//string format
static int32 string_size;

x=s;
if (x<len){
c=f->chr[x];
string_size=0;//invalid
if (c==38) string_size=-1; //"&" (all of string)
if (c==33) string_size=1; //"!" (first character only)
if (c==92){ //"\" first n characters
 z=1;
 x++;
 get_str_fmt:
 if (x>=len) goto invalid_string_format;
 c=f->chr[x];
 z++;
 if (c==32){x++; goto get_str_fmt;}
 if (c!=92) goto invalid_string_format;
 string_size=z;
}//c==47
if (string_size){
 if (type==0) return s; //s is the beginning of a new format but item has already been added to dest
 if (type==1){//expected numeric format, not string format
 error(13);//type mismatch
 return 0;
 }
 if (string_size!=-1){
 s+=string_size;
  for (z=0;z<string_size;z++){
  if (z<pu_str->len) qbs1->chr[0]=pu_str->chr[z]; else qbs1->chr[0]=32;
  qbs_set(dest,qbs_add(dest,qbs1));
  }//z 
 }else{
 qbs_set(dest,qbs_add(dest,pu_str));
 s++;
 }
 type=0;//passed type added
 if (s>=len) return 0;//end of format line encountered and passed item added
 goto scan;
}//string_size
}//x<len
invalid_string_format:

//add literal?
if ((f->chr[s]==95)&&(s<(len-1))){//trailing single _ in format is treated as a literal _
s++;
}
//add non-format character
qbs1->chr[0]=f->chr[s]; qbs_set(dest,qbs_add(dest,qbs1));

s++;
if (s>=len){
s=0;
if (type==0) return s;//end of format line encountered and passed item added
 //illegal format? (format has been passed from start (s2=0) to end and has no numeric/string identifiers
 if (s2==0){
 error(5);//illegal function call
 return 0;
 }
}
goto scan;

return 0;
}

int32 print_using_integer64(qbs* format, int64 value, int32 start, qbs *output){
if (new_error) return 0;
#ifdef QB64_WINDOWS
 pu_ndig=sprintf((char*)pu_buf,"% I64i",value);
#else
 pu_ndig=sprintf((char*)pu_buf,"% lli",value);
#endif
if (pu_buf[0]==45) pu_neg=1; else pu_neg=0;
pu_ndig--;//remove sign
memcpy(pu_dig,&pu_buf[1],pu_ndig);
pu_dp=0;
start=print_using(format,start,output,NULL);
return start;
}

int32 print_using_uinteger64(qbs* format, uint64 value, int32 start, qbs *output){
if (new_error) return 0;
#ifdef QB64_WINDOWS
 pu_ndig=sprintf((char*)pu_dig,"%I64u",value);
#else
 pu_ndig=sprintf((char*)pu_dig,"%llu",value);
#endif
pu_neg=0;
pu_dp=0;
start=print_using(format,start,output,NULL);
return start;
}

int32 print_using_single(qbs* format, float value, int32 start, qbs *output){
if (new_error) return 0;
static int32 i,len,neg_exp;
static uint8 c;
static int64 exp;
len=sprintf((char*)&pu_buf,"% .255E",value);//256 character limit ([1].[255])
pu_dp=0;
pu_ndig=0;
//1. sign
if (pu_buf[0]==45) pu_neg=1; else pu_neg=0;
i=1;
//2. digits before decimal place
getdigits:
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if ((c>=48)&&(c<=57)){
pu_dig[pu_ndig++]=c;
i++;
goto getdigits;
}
//3. decimal place
if (c!=46){error(5); return 0;}//expected decimal point
i++;
//4. digits after decimal place
getdigits2:
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if ((c>=48)&&(c<=57)){
pu_dig[pu_ndig++]=c;
pu_dp--;
i++;
goto getdigits2;
}
//assume random character signifying an exponent
i++;
//optional exponent sign
neg_exp=0;
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if (c==45){neg_exp=1; i++;}//-
if (c==43) i++;//+
//assume remaining characters are an exponent
exp=0;
getdigits3:
if (i<len){
c=pu_buf[i];
if ((c<48)||(c>57)){error(5); return 0;}
exp=exp*10;
exp=exp+c-48;
i++;
goto getdigits3;
}
if (neg_exp) exp=-exp;
pu_dp+=exp;
start=print_using(format,start,output,NULL);
return start;
}

int32 print_using_double(qbs* format, double value, int32 start, qbs *output){
if (new_error) return 0;
static int32 i,len,neg_exp;
static uint8 c;
static int64 exp;
len=sprintf((char*)&pu_buf,"% .255E",value);//256 character limit ([1].[255])
pu_dp=0;
pu_ndig=0;
//1. sign
if (pu_buf[0]==45) pu_neg=1; else pu_neg=0;
i=1;
//2. digits before decimal place
getdigits:
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if ((c>=48)&&(c<=57)){
pu_dig[pu_ndig++]=c;
i++;
goto getdigits;
}
//3. decimal place
if (c!=46){error(5); return 0;}//expected decimal point
i++;
//4. digits after decimal place
getdigits2:
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if ((c>=48)&&(c<=57)){
pu_dig[pu_ndig++]=c;
pu_dp--;
i++;
goto getdigits2;
}
//assume random character signifying an exponent
i++;
//optional exponent sign
neg_exp=0;
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if (c==45){neg_exp=1; i++;}//-
if (c==43) i++;//+
//assume remaining characters are an exponent
exp=0;
getdigits3:
if (i<len){
c=pu_buf[i];
if ((c<48)||(c>57)){error(5); return 0;}
exp=exp*10;
exp=exp+c-48;
i++;
goto getdigits3;
}
if (neg_exp) exp=-exp;
pu_dp+=exp;
pu_exp_char=68; //"D"
start=print_using(format,start,output,NULL);
pu_exp_char=69; //"E"
return start;
}

//WARNING: DUE TO MINGW NOT SUPPORTING PRINTF long double, VALUES ARE CONVERTED TO A DOUBLE
//         BUT PRINTED AS IF THEY WERE A long double
int32 print_using_float(qbs* format, double value, int32 start, qbs *output){
if (new_error) return 0;
static int32 i,len,neg_exp;
static uint8 c;
static int64 exp;
len=sprintf((char*)&pu_buf,"% .255E",value);//256 character limit ([1].[255])
pu_dp=0;
pu_ndig=0;
//1. sign
if (pu_buf[0]==45) pu_neg=1; else pu_neg=0;
i=1;
//2. digits before decimal place
getdigits:
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if ((c>=48)&&(c<=57)){
pu_dig[pu_ndig++]=c;
i++;
goto getdigits;
}
//3. decimal place
if (c!=46){error(5); return 0;}//expected decimal point
i++;
//4. digits after decimal place
getdigits2:
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if ((c>=48)&&(c<=57)){
pu_dig[pu_ndig++]=c;
pu_dp--;
i++;
goto getdigits2;
}
//assume random character signifying an exponent
i++;
//optional exponent sign
neg_exp=0;
if (i>=len){error(5); return 0;}
c=pu_buf[i];
if (c==45){neg_exp=1; i++;}//-
if (c==43) i++;//+
//assume remaining characters are an exponent
exp=0;
getdigits3:
if (i<len){
c=pu_buf[i];
if ((c<48)||(c>57)){error(5); return 0;}
exp=exp*10;
exp=exp+c-48;
i++;
goto getdigits3;
}
if (neg_exp) exp=-exp;
pu_dp+=exp;
pu_exp_char=70; //"F"
start=print_using(format,start,output,NULL);
pu_exp_char=69; //"E"
return start;
}

extern ontimer_struct *ontimer;
extern onkey_struct *onkey;
extern int32 onkey_inprogress;
extern onstrig_struct *onstrig;
extern int32 onstrig_inprogress;

void sub_run_init(){
//Reset ON KEY trapping
//note: KEY bar F-key bindings are not affected
static int32 i;
for (i=1;i<=31;i++){
onkey[i].id=0;
onkey[i].active=0;
onkey[i].state=0;
}
onkey_inprogress=0;
//note: if already in screen 0:80x25, screen pages are left intact
//set screen mode to 0 (80x25)
qbg_screen(0,NULL,0,0,NULL,1|4|8);
//make sure WIDTH is 80x25
qbsub_width(NULL,80,25,1|2);
//restore palette
restorepalette(write_page);
//restore default colors
write_page->background_color=0;
write_page->color=7;
//note: cursor state does not appear to be reset by the RUN command
//im->cursor_show=0; im->cursor_firstvalue=4; im->cursor_lastvalue=4;
}



void sub_run(qbs* f){
if (new_error) return;
//run program
static qbs *str=NULL;
if (str==NULL) str=qbs_new(0,0);
static qbs *strz=NULL;
if (!strz) strz=qbs_new(0,0);

qbs_set(str,f);
fixdir(str);

#ifdef QB64_WINDOWS

qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
if (WinExec((char *)strz->chr,SW_SHOWDEFAULT)>31){
goto run_exit;
}else{
//0-out of resources/memory
//ERROR_BAD_FORMAT
//ERROR_FILE_NOT_FOUND
//ERROR_PATH_NOT_FOUND
error(53); return;//file not found
}

#else
qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
system((char*)strz->chr);
//success?
goto run_exit;

#endif

//exit this program
run_exit:
close_program=1;
end();
exit(99);//<--this line should never actually be executed

}

void sub__icon(int32 i){
if (new_error) return;
static int32 i2;
static SDL_Surface *surface=NULL;
static uint32 *o,*o2;
static int32 x,n,c,i3,c2;
//verify image i
 if (i>=0){//validate i
 validatepage(i); i=page[i];
 }else{
 i=-i; if (i>=nextimg){error(258); return;} if (!img[i].valid){error(258); return;}
 }
if (img[i].text){error(5); return;}
//create new SDL 32x32 surface for icon call if necessary
if (!surface){
Uint32 rmask, gmask, bmask, amask;
    bmask = 0x000000ff;
    gmask = 0x0000ff00;
    rmask = 0x00ff0000;
    amask = 0xff000000;
surface = SDL_CreateRGBSurface(SDL_SWSURFACE,32,32,32,rmask,gmask,bmask,amask);
if (!surface) return;//no error required for icon failure
}
//prepare i2
i2=func__newimage(32,32,32,1);
i3=func__dest();
sub__dest(i2);
sub_cls(NULL,0,2);
sub__dest(i3);
sub__dontblend(i2,1);
sub__putimage(NULL,NULL,NULL,NULL,-i,i2,NULL,NULL,NULL,NULL,8+32); //note: ideally, i should have its aspect ratio respected when putting onto i2
//copy i2 onto SDL surface
o=img[-i2].offset32;
o2=(uint32*)surface->pixels;
n=32*32;
for (x=0;x<n;x++){
c=o[x];
	//note: SDL does not support alpha blended pixels in icons yet, this is a work-around
	c2=((c>>24)&255);
	if (c2<127) c2=0; else c2=255;
o2[x]=(c&0xFFFFFF)+(c2<<24);
}
SDL_WM_SetIcon(surface,NULL);
sub__freeimage(i2,1);
}

void sub__autodisplay(){
autodisplay=1;
}

void sub__display(){
//disable autodisplay (if enabled)
if (autodisplay){
autodisplay=-1;//toggle request
while(autodisplay) Sleep(1);
return;//note: autodisplay is set to 0 after display() has been called so a second call to display() is unnecessary
}
display();
}

int32 sub_draw_i;
uint8 *sub_draw_cp;
int32 sub_draw_len;

int32 draw_num_invalid;
int32 draw_num_undefined;
double draw_num(){
static int32 c,dp,vptr,x,offset;
static double d,dp_mult,sgn;

draw_num_invalid=0;
draw_num_undefined=1;
d=0;
dp=0;
sgn=1;
vptr=0;

nextchar:
if (sub_draw_i>=sub_draw_len) return d*sgn;
c=sub_draw_cp[sub_draw_i];

if (vptr){
if ((sub_draw_i+2)>=sub_draw_len) {draw_num_invalid=1; return 0;}//not enough data!
offset=sub_draw_cp[sub_draw_i+2]*256+sub_draw_cp[sub_draw_i+1];
sub_draw_i+=3;
vptr=0;
/*
'BYTE=1
'INTEGER=2
'STRING=3 (unsupported)
'SINGLE=4
'INT64=5
'FLOAT=6
'DOUBLE=8
'LONG=20
'BIT=64+n (unsupported)
*/
if (c==1){d=*((int8*)(&cmem[1280+offset])); goto nextcharv;}
if (c==(1+128)){d=*((uint8*)(&cmem[1280+offset])); goto nextcharv;}
if (c==2){d=*((int16*)(&cmem[1280+offset])); goto nextcharv;}
if (c==(2+128)){d=*((uint16*)(&cmem[1280+offset])); goto nextcharv;}
if (c==4){d=*((float*)(&cmem[1280+offset])); goto nextcharv;}
if (c==5){d=*((int64*)(&cmem[1280+offset])); goto nextcharv;}
if (c==(5+128)){d=*((uint64*)(&cmem[1280+offset])); goto nextcharv;}
if (c==6){d=*((long double*)(&cmem[1280+offset])); goto nextcharv;}
if (c==8){d=*((double*)(&cmem[1280+offset])); goto nextcharv;}
if (c==20){d=*((int32*)(&cmem[1280+offset])); goto nextcharv;}
if (c==(20+128)){d=*((uint32*)(&cmem[1280+offset])); goto nextcharv;}
//unknown/unsupported types(bit/string) return an error
draw_num_invalid=1; return 0;
nextcharv:
draw_num_invalid=0;
draw_num_undefined=0;
return d;
}

if ((c==32)||(c==9)){sub_draw_i++; goto nextchar;}//skip whitespace

if ((c>=48)&&(c<=57)){
c-=48;
if (dp){
 d+=(((double)c)*dp_mult);
 dp_mult/=10.0;
}else{
 d=(d*10)+c;
}
draw_num_undefined=0;
draw_num_invalid=0;
sub_draw_i++; goto nextchar;
}

if (c==45){//-
if (dp||(!draw_num_undefined)) return d*sgn;
sgn=-sgn;
draw_num_invalid=1;
sub_draw_i++; goto nextchar;
}

if (c==43){//+
if (dp||(!draw_num_undefined)) return d*sgn;
draw_num_invalid=1;
sub_draw_i++; goto nextchar;
}

if (c==46){//.
if (dp) return d*sgn;
dp=1; dp_mult=0.1;
if (!draw_num_undefined) draw_num_invalid=1;
sub_draw_i++; goto nextchar;
}

if (c==61){//=
if (draw_num_invalid||dp||(!draw_num_undefined)){draw_num_invalid=1; return 0;}//leading data invalid!
vptr=1;
sub_draw_i++; goto nextchar;
}

return d*sgn;
}

void sub_draw(qbs* s){
if (new_error) return;

/*

Aspect ratio determination:
32/256 modes always assume 1:1 ratio
All other modes (1-13) determine their aspect ratio from the destination surface's dimensions (presuming it is stretched onto a 4:3 ratio monitor)

Reference:
      Line-drawing and cursor-movement commands:
        D[n%]            Moves cursor down n% units.
        E[n%]            Moves cursor up and right n% units.
        F[n%]            Moves cursor down and right n% units.
        G[n%]            Moves cursor down and left n% units.
        H[n%]            Moves cursor up and left n% units.
        L[n%]            Moves cursor left n% units.
        M[{+|-}]x%,y%    Moves cursor to point x%,y%. If x% is preceded
                         by + or -, moves relative to the current point.
						 -+/- relative ONLY if after the M, after comma doesn't affect method
						 -nothing to do with VIEW/WINDOW coordinates (but still clipped)
        R[n%]            Moves cursor right n% units.
        U[n%]            Moves cursor up n% units.
        [B]              Optional prefix that moves cursor without drawing.
        [N]              Optional prefix that draws and returns cursor to
                         its original position.
						 *Prefixes B&N can be used anywhere. They set (not toggle) their respective states. They are only cleared if they are used in a statement. They are forgotten when a new DRAW statement is called.
      Color, rotation, and scale commands:
        An%              Rotates an object n% * 90 degrees (n% can be 0, 1,
                         2, or 3).
        Cn%              Sets the drawing color (n% is a color attribute).

        Pn1%,n2%         Sets the paint fill and border colors of an object
                         (n1% is the fill-color attribute, n2% is the
                         border-color attribute).
        Sn%              Determines the drawing scale by setting the length
                         of a unit of cursor movement. The default n% is 4,
                         which is equivalent to 1 pixel.
        TAn%             Turns an angle n% degrees (-360 through 360).

      -If you omit n% from line-drawing and cursor-movement commands, the
      cursor moves 1 unit.
      -To execute a DRAW command substring from a DRAW command string, use
      the "X" command:
      DRAW "X"+ VARPTR$(commandstring$)
*/

static double r,ir,vx,vy,hx,hy,ex,ey,fx,fy,xx,yy,px,py,px2,py2,d,d2,sin_ta,cos_ta;
static int64 c64,c64b,c64c;
static uint32 col;
static int32 x,c,prefix_b,prefix_n,offset;
static uint8 *stack_s[8192];
static uint16 stack_len[8192];
static uint16 stack_i[8192];
static int32 stacksize;
static double draw_ta;
static double draw_scale;

if (write_page->text){error(5); return;}

draw_ta=write_page->draw_ta; draw_scale=write_page->draw_scale;

if (write_page->compatible_mode<=13){
if (write_page->compatible_mode==1) r=4.0/((3.0/200.0)*320.0);
if (write_page->compatible_mode==2) r=4.0/((3.0/200.0)*640.0);
if (write_page->compatible_mode==7) r=4.0/((3.0/200.0)*320.0);
if (write_page->compatible_mode==8) r=4.0/((3.0/200.0)*640.0);
if (write_page->compatible_mode==9) r=4.0/((3.0/350.0)*640.0);
if (write_page->compatible_mode==10) r=4.0/((3.0/350.0)*640.0);
if (write_page->compatible_mode==11) r=4.0/((3.0/480.0)*640.0); 
if (write_page->compatible_mode==12) r=4.0/((3.0/480.0)*640.0);
if (write_page->compatible_mode==13) r=4.0/((3.0/200.0)*320.0);
//Old method: r=4.0 /( (3.0/((double)write_page->height)) * ((double)write_page->width) ); //calculate aspect ratio of image
ir=1/r; //note: all drawing must multiply the x offset by ir (inverse ratio)
}else{
r=1;
ir=1;
}



vx=0; vy=-1; ex=r; ey=-1; hx=r; hy=0; fx=r; fy=1;//reset vectors
//rotate vectors by ta?
if (draw_ta){
d=draw_ta*0.0174532925199433; sin_ta=sin(d); cos_ta=cos(d);
px2=vx;
py2=vy;
vx=px2*cos_ta+py2*sin_ta;
vy=py2*cos_ta-px2*sin_ta;
px2=hx;
py2=hy;
hx=px2*cos_ta+py2*sin_ta;
hy=py2*cos_ta-px2*sin_ta;
px2=ex;
py2=ey;
ex=px2*cos_ta+py2*sin_ta;
ey=py2*cos_ta-px2*sin_ta;
px2=fx;
py2=fy;
fx=px2*cos_ta+py2*sin_ta;
fy=py2*cos_ta-px2*sin_ta;
}

//convert x,y image position into a pixel coordinate
if (write_page->clipping_or_scaling){
 if (write_page->clipping_or_scaling==2){
 px=write_page->x*write_page->scaling_x+write_page->scaling_offset_x+write_page->view_offset_x;
 py=write_page->y*write_page->scaling_y+write_page->scaling_offset_y+write_page->view_offset_y;
 }else{
 px=write_page->x+write_page->view_offset_x;
 py=write_page->y+write_page->view_offset_y;
 }
}else{
 px=write_page->x;
 py=write_page->y;
}

col=write_page->draw_color;
prefix_b=0; prefix_n=0;

stacksize=0;

sub_draw_cp=s->chr;
sub_draw_len=s->len;
sub_draw_i=0;

nextchar:
if (sub_draw_i>=sub_draw_len){

//revert from X-stack
if (stacksize){
stacksize--; sub_draw_cp=stack_s[stacksize]; sub_draw_len=stack_len[stacksize]; sub_draw_i=stack_i[stacksize];//restore state
//continue
goto nextchar;
}

//revert px,py to image->x,y offsets
if (write_page->clipping_or_scaling){
 if (write_page->clipping_or_scaling==2){
 px=(px-write_page->view_offset_x-write_page->scaling_offset_x)/write_page->scaling_x;
 py=(py-write_page->view_offset_y-write_page->scaling_offset_y)/write_page->scaling_y;
 }else{
 px=px-write_page->view_offset_x;
 py=py-write_page->view_offset_y;
 }
}
write_page->x=px; write_page->y=py;
return;
}
c=sub_draw_cp[sub_draw_i];

if ((c>=97)&&(c<=122)) c-=32;//ucase c

if (c==77){//M
m_nextchar:
sub_draw_i++; if (sub_draw_i>=sub_draw_len){error(5); return;}
c=sub_draw_cp[sub_draw_i];
if ((c==32)||(c==9)) goto m_nextchar;//skip whitespace
//check for absolute/relative positioning
if ((c==43)||(c==45)) x=1; else x=0;
px2=draw_num();
if (draw_num_invalid||draw_num_undefined){error(5); return;}
c=sub_draw_cp[sub_draw_i];
if (c!=44){error(5); return;}//expected ,
sub_draw_i++;
py2=draw_num();
if (draw_num_invalid||draw_num_undefined){error(5); return;}
if (x){//relative positioning
xx=(px2*ir)*hx-(py2*ir)*vx; yy=px2*hy-py2*vy; px2=px+xx*draw_scale; py2=py+yy*draw_scale;
}
if (!prefix_b) fast_line(qbr(px),qbr(py),qbr(px2),qbr(py2),col);
if (!prefix_n){px=px2; py=py2;}//update position
prefix_b=0; prefix_n=0;
goto nextchar;
}

if (c==84){//T(A)
ta_nextchar:
sub_draw_i++; if (sub_draw_i>=sub_draw_len){error(5); return;}
c=sub_draw_cp[sub_draw_i];
if ((c==32)||(c==9)) goto ta_nextchar;//skip whitespace
if ((c!=65)&&(c!=97)){error(5); return;}//not TA
sub_draw_i++;
d=draw_num();
if (draw_num_invalid||draw_num_undefined){error(5); return;}
draw_ta=d;
write_page->draw_ta=draw_ta;
ta_entry:
//note: ta rotation is not relative to previous angle
vx=0; vy=-1; ex=r; ey=-1; hx=r; hy=0; fx=r; fy=1;//reset vectors
//rotate vectors by ta
d=draw_ta*0.0174532925199433; sin_ta=sin(d); cos_ta=cos(d);
px2=vx;
py2=vy;
vx=px2*cos_ta+py2*sin_ta;
vy=py2*cos_ta-px2*sin_ta;
px2=hx;
py2=hy;
hx=px2*cos_ta+py2*sin_ta;
hy=py2*cos_ta-px2*sin_ta;
px2=ex;
py2=ey;
ex=px2*cos_ta+py2*sin_ta;
ey=py2*cos_ta-px2*sin_ta;
px2=fx;
py2=fy;
fx=px2*cos_ta+py2*sin_ta;
fy=py2*cos_ta-px2*sin_ta;
goto nextchar;
}

if (c==85){xx=vx; yy=vy; goto udlr;}//U
if (c==68){xx=-vx; yy=-vy; goto udlr;}//D
if (c==76){xx=-hx; yy=-hy; goto udlr;}//L
if (c==82){xx=hx; yy=hy; goto udlr;}//R

if (c==69){xx=ex; yy=ey; goto udlr;}//E
if (c==70){xx=fx; yy=fy; goto udlr;}//F
if (c==71){xx=-ex; yy=-ey; goto udlr;}//G
if (c==72){xx=-fx; yy=-fy; goto udlr;}//H

if (c==67){//C
sub_draw_i++;
d=draw_num();
if (draw_num_invalid||draw_num_undefined){error(5); return;}
c64=d; xx=c64; if (xx!=d){error(5); return;}//non-integer
//if (c64<0){error(5); return;}
//c64b=1; c64b<<=write_page->bits_per_pixel; c64b--;
//if (c64>c64b){error(5); return;}
col=c64;
write_page->draw_color=col;
goto nextchar;
}

if (c==66){//B (move without drawing prefix)
prefix_b=1;
sub_draw_i++;
goto nextchar;
}

if (c==78){//N (draw without moving)
prefix_n=1;
sub_draw_i++;
goto nextchar;
}

if (c==83){//S
sub_draw_i++;
d=draw_num();
if (draw_num_invalid||draw_num_undefined){error(5); return;}
if (d<0){error(5); return;}
draw_scale=d/4.0;
write_page->draw_scale=draw_scale;
goto nextchar;
}

if (c==80){//P
sub_draw_i++;
d=draw_num();
if (draw_num_invalid||draw_num_undefined){error(5); return;}
 c64=d; xx=c64; if (xx!=d){error(5); return;}//non-integer
 //if (c64<0){error(5); return;}
 //c64b=1; c64b<<=write_page->bits_per_pixel; c64b--;
 //if (c64>c64b){error(5); return;}
c64c=c64;
c=sub_draw_cp[sub_draw_i];
if (c!=44){error(5); return;}//expected ,
sub_draw_i++;
d=draw_num();
if (draw_num_invalid||draw_num_undefined){error(5); return;}
 c64=d; xx=c64; if (xx!=d){error(5); return;}//non-integer
 //if (c64<0){error(5); return;}
 //c64b=1; c64b<<=write_page->bits_per_pixel; c64b--;
 //if (c64>c64b){error(5); return;}
//revert px,py to x,y offsets
if (write_page->clipping_or_scaling){
 if (write_page->clipping_or_scaling==2){
 xx=(px-write_page->view_offset_x-write_page->scaling_offset_x)/write_page->scaling_x;
 yy=(py-write_page->view_offset_y-write_page->scaling_offset_y)/write_page->scaling_y;
 }else{
 xx=px-write_page->view_offset_x;
 yy=py-write_page->view_offset_y;
 }
}else{
xx=px;
yy=py;
}
sub_paint(xx,yy,c64c,c64,NULL,2+4);
col=c64c;
goto nextchar;
}

if (c==65){//A
sub_draw_i++;
d=draw_num();
if (draw_num_invalid||draw_num_undefined){error(5); return;}
if (d==0){draw_ta=0; write_page->draw_ta=draw_ta; goto ta_entry;}
if (d==1){draw_ta=90; write_page->draw_ta=draw_ta; goto ta_entry;}
if (d==2){draw_ta=180; write_page->draw_ta=draw_ta; goto ta_entry;}
if (d==3){draw_ta=270; write_page->draw_ta=draw_ta; goto ta_entry;}
error(5); return;//invalid value
}

if (c==88){//X
sub_draw_i++;
if ((sub_draw_i+2)>=sub_draw_len){error(5); return;}
if (sub_draw_cp[sub_draw_i]!=3){error(5); return;}
offset=sub_draw_cp[sub_draw_i+2]*256+sub_draw_cp[sub_draw_i+1];//offset of string descriptor in DBLOCK
sub_draw_i+=3;
if (stacksize==8192){error(6); return;}//X-stack "OVERFLOW" (should never occur because DBLOCK will overflow first)
stack_s[stacksize]=sub_draw_cp; stack_len[stacksize]=sub_draw_len; stack_i[stacksize]=sub_draw_i; stacksize++;//backup state
//set new state
sub_draw_i=0;
x=cmem[1280+offset+3]*256+cmem[1280+offset+2];
sub_draw_cp=&cmem[1280]+x;
sub_draw_len=cmem[1280+offset+1]*256+cmem[1280+offset+0];
//continue processing
goto nextchar;
}

if ((c==32)||(c==9)||(c==59)){sub_draw_i++; goto nextchar;}//skip whitespace/semicolons

error(5); return;//unknown command encountered!




udlr:
sub_draw_i++;
d=draw_num();
if (draw_num_invalid){error(5); return;}
if (draw_num_undefined) d=1;
xx*=d; yy*=d;
//***apply scaling here***
xx=xx*ir;
px2=px+xx*draw_scale; py2=py+yy*draw_scale;
if (!prefix_b) fast_line(qbr(px),qbr(py),qbr(px2),qbr(py2),col);
if (!prefix_n){px=px2; py=py2;}//update position
prefix_b=0; prefix_n=0;
goto nextchar;
}



#ifdef QB64_LINUX 
extern char** environ;
#define envp environ
#else /* WINDOWS */
//extern char** _environ;
#define envp _environ
#endif
size_t environ_count;

qbs *func_environ(qbs *name)
{
static char *cp;
static qbs *tqbs;
static int32 bytes;
cp=getenv((char*)name->chr);
if (cp){
bytes=strlen(cp);
tqbs=qbs_new(bytes,1);
memcpy(tqbs->chr,cp,bytes); 
}else{
tqbs=qbs_new(0,1);
}
return tqbs;
}

qbs *func_environ(int32 number)
{
static qbs *tqbs;
static char *cp;
static int32 bytes;
if (number<=0){tqbs=qbs_new(0,1); error(5); return tqbs;}
if (number>=environ_count){tqbs=qbs_new(0,1); return tqbs;}
cp=*(envp+number-1);
bytes=strlen(cp);
tqbs=qbs_new(bytes,1);
memcpy(tqbs->chr,cp,bytes);
return tqbs;
}

void sub_environ(qbs *str)
{
static char *cp;
cp=(char*)malloc(str->len+1);
cp[str->len]=0;//add NULL terminator
memcpy(cp,str->chr,str->len);
putenv(cp);
free(cp);
environ_count++;
}

#ifdef QB64_WINDOWS
void showvalue(__int64 v){
static qbs* s=NULL;
if (s==NULL) s=qbs_new(0,0);
qbs_set(s,qbs_str(v));
MessageBox(NULL,(char*)s->chr,"showvalue",MB_OK|MB_SYSTEMMODAL);
}
#endif





//network prototype:

uint8 net_tcp_init_done=0;
void net_tcp_init(){
if (!net_tcp_init_done){
net_tcp_init_done=1;
if(SDLNet_Init()==-1){
//assume success
}
}
}

int32 func__connected(int32 i){
if (new_error) return 0;
//validate
if (i>=0){error(52); return 0;}//Bad file name or number
i=-(i+1);
if (i>=sfh_bufsize){error(52); return 0;}
if ((sfh[i].type<1)||(sfh[i].type>3)){error(52); return 0;}
if (net_tcp[i].error) return 0;
if ((sfh[i].type==2)||(sfh[i].type==3)){
	net_tcp_updatebuffer(i);//attempt to trigger error state by updating buffer
	if (net_tcp[i].error) return 0;
}
return -1;
}

qbs *func__connectionaddress(int32 i){
static qbs *tqbs,*tqbs2,*str=NULL,*str2=NULL;
if (new_error) goto error;
if (!str) str=qbs_new(0,0);
if (!str2) str2=qbs_new(0,0);

//validate
if (i>=0){error(52); goto error;}//Bad file name or number
i=-(i+1);
if (i>=sfh_bufsize){error(52); goto error;}
if ((sfh[i].type<1)||(sfh[i].type>3)){error(52); goto error;}

if (sfh[i].type==1){//host (1) returns its own address
qbs_set(str,qbs_new_txt("TCP/IP:"));//network type
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str(net_tcp[i].portused))));//port
qbs_set(str,qbs_add(str,qbs_new_txt(":")));
tqbs2=WHATISMYIP();
if (tqbs2->len){
qbs_set(str,qbs_add(str,tqbs2));
}else{
//global IP unavailable, use local IP
//IP4
static IPaddress ip;
if(SDLNet_ResolveHost(&ip,"localhost",0)==-1) goto error;
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str((int32)(ip.host)&255))));
qbs_set(str,qbs_add(str,qbs_new_txt(".")));
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str((int32)((ip.host)>>8)&255))));
qbs_set(str,qbs_add(str,qbs_new_txt(".")));
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str((int32)((ip.host)>>16)&255))));
qbs_set(str,qbs_add(str,qbs_new_txt(".")));
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str((int32)((ip.host)>>24)&255))));
}
tqbs=qbs_new(str->len,1);
memmove(tqbs->chr,str->chr,str->len);
return tqbs;
}

//assume connection/client socket (2/3)
static IPaddress *pip;
pip=SDLNet_TCP_GetPeerAddress(net_tcp[i].socket);
qbs_set(str,qbs_new_txt("TCP/IP:"));//network type
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str(net_tcp[i].portused))));//port
qbs_set(str,qbs_add(str,qbs_new_txt(":")));
//IP4
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str((int32)(pip->host)&255))));
qbs_set(str,qbs_add(str,qbs_new_txt(".")));
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str((int32)((pip->host)>>8)&255))));
qbs_set(str,qbs_add(str,qbs_new_txt(".")));
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str((int32)((pip->host)>>16)&255))));
qbs_set(str,qbs_add(str,qbs_new_txt(".")));
qbs_set(str,qbs_add(str,qbs_ltrim(qbs_str((int32)((pip->host)>>24)&255))));
tqbs=qbs_new(str->len,1);
memmove(tqbs->chr,str->chr,str->len);
return tqbs;

error:
tqbs=qbs_new(0,1);
return tqbs;
}



int32 func__openhost(qbs* method){
if (new_error) return 0;

//generic data
static qbs *s=NULL; if (!s){s=qbs_new(0,0);}
static qbs *s2=NULL; if (!s2){s2=qbs_new(0,0);}
static qbs *s3=NULL; if (!s3){s3=qbs_new(0,0);}
static double d;
static int32 i,i2,portused;

qbs_set(s,method);
qbs_set(s2,qbs_ucase(s));

//TCP/IP Network
qbs_set(s3,qbs_new_txt("TCP/IP:"));
if (func_instr(NULL,s2,s3,NULL)==1){
qbs_set(s,func_mid(s,8,NULL,NULL));
net_tcp_init();
//***assume*** string s contains an integer port number
static IPaddress ip;
static TCPsocket socket;
d=func_val(s);
i=qbr_double_to_long(d);
//***assume*** port number is within valid range
portused=i;
if(SDLNet_ResolveHost(&ip,NULL,i)==-1) return 0;
socket=SDLNet_TCP_Open(&ip);
if(!socket) return 0;

i2=sfh_new();
sfh[i2].type=1;//TCPIP host
if (i2>=net_tcp_bufsize) net_tcp=(net_tcp_struct*)realloc(net_tcp,sizeof(net_tcp_struct)*(++net_tcp_bufsize));
net_tcp[i2].ip=ip;
net_tcp[i2].socket=socket;
net_tcp[i2].error=0;
net_tcp[i2].buffer=NULL; net_tcp[i2].buffer_size=0; net_tcp[i2].buffer_space=0;
net_tcp[i2].portused=i;

return -1-i2;

}//TCP/IP

error(5); return 0;

}

int32 func__openconnection(int32 host){



static int32 i2;

//validate host handle
host=-(host+1);
if ((host<0)||(host>=sfh_bufsize)){error(258); return 0;}//invalid handle
if (sfh[host].type!=1){error(258); return 0;}//invalid handle
static TCPsocket socket;
socket=SDLNet_TCP_Accept(net_tcp[host].socket);
if (!socket) return 0;

//create socket set for watching
static SDLNet_SocketSet set;
set=SDLNet_AllocSocketSet(1);
if (!set){
SDLNet_TCP_Close(socket);
return 0;
}
//add socket to set
if (SDLNet_TCP_AddSocket(set,socket)!=1){
SDLNet_FreeSocketSet(set);
SDLNet_TCP_Close(socket);
return 0;
}

i2=sfh_new();
sfh[i2].type=3;//TCPIP connection
if (i2>=net_tcp_bufsize) net_tcp=(net_tcp_struct*)realloc(net_tcp,sizeof(net_tcp_struct)*(++net_tcp_bufsize));
net_tcp[i2].socket=socket;
net_tcp[i2].set=set;
net_tcp[i2].error=0;
net_tcp[i2].buffer=NULL; net_tcp[i2].buffer_size=0; net_tcp[i2].buffer_space=0;
net_tcp[i2].eof=0;
net_tcp[i2].portused=net_tcp[host].portused;

return -1-i2;

}

int32 func__openclient(qbs* method){
if (new_error) return 0;

//generic data
static qbs *z=NULL; if (!z){z=qbs_new(1,0); z->chr[0]=0;}
static qbs *s=NULL; if (!s){s=qbs_new(0,0);}
static qbs *s2=NULL; if (!s2){s2=qbs_new(0,0);}
static qbs *s3=NULL; if (!s3){s3=qbs_new(0,0);}
static double d;
static int32 i,i2,portused;

qbs_set(s,method);
qbs_set(s2,qbs_ucase(s));

//TCP/IP Network
qbs_set(s3,qbs_new_txt("TCP/IP:"));
if (func_instr(NULL,s2,s3,NULL)==1){
qbs_set(s,func_mid(s,8,NULL,NULL));
net_tcp_init();

//read port number
qbs_set(s3,qbs_new_txt(":"));
i=func_instr(NULL,s,s3,NULL);
if (!i){error(5); return 0;}
qbs_set(s3,func_mid(s,1,i-1,1)); qbs_set(s,func_mid(s,i+1,NULL,NULL));
d=func_val(s3);
i2=qbr_double_to_long(d);
//***assume*** port number is within valid range
portused=i2;
//read address
if (!s->len){error(5); return 0;}
//null terminate address
qbs_set(s3,qbs_add(s,z));
static IPaddress ip;
static TCPsocket socket;
if(SDLNet_ResolveHost(&ip,(char*)s3->chr,i2)==-1) return 0;
socket=SDLNet_TCP_Open(&ip);
if (!socket) return 0;

//create socket set for watching
static SDLNet_SocketSet set;
set=SDLNet_AllocSocketSet(1);
if (!set){
SDLNet_TCP_Close(socket);
return 0;
}
//add socket to set
if (SDLNet_TCP_AddSocket(set,socket)!=1){
SDLNet_FreeSocketSet(set);
SDLNet_TCP_Close(socket);
return 0;
}

i2=sfh_new();
sfh[i2].type=2;//TCPIP client
if (i2>=net_tcp_bufsize) net_tcp=(net_tcp_struct*)realloc(net_tcp,sizeof(net_tcp_struct)*(++net_tcp_bufsize));
net_tcp[i2].ip=ip;
net_tcp[i2].socket=socket;
net_tcp[i2].set=set;
net_tcp[i2].error=0;
net_tcp[i2].buffer=NULL; net_tcp[i2].buffer_size=0; net_tcp[i2].buffer_space=0;
net_tcp[i2].eof=0;
net_tcp[i2].portused=portused;
return -1-i2;

}//TCP/IP

error(5); return 0;

}

int32 func__exit(){
exit_blocked=1;
static int32 x;
x=exit_value;
exit_value=0;
return x;
}

#ifdef QB64_LINUX
#ifndef QB64_MACOSX
//X11 clipboard interface for Linux
SDL_SysWMinfo syswminfo;
Atom targets,utf8string,compoundtext,clipboard;
int x11filter(const SDL_Event *sdlevent){
static int i;
static char *cp;
static XEvent x11event;
static XSelectionRequestEvent *x11request;
static XSelectionEvent x11selectionevent;
static Atom mytargets[]={XA_STRING,utf8string,compoundtext};
if (sdlevent->type==SDL_SYSWMEVENT){ 
 x11event=sdlevent->syswm.msg->event.xevent;
 if (x11event.type==SelectionRequest){
  x11request=&x11event.xselectionrequest;
  x11selectionevent.type=SelectionNotify;
  x11selectionevent.serial=x11event.xany.send_event;
  x11selectionevent.send_event=True;
  x11selectionevent.display=syswminfo.info.x11.display;
  x11selectionevent.requestor=x11request->requestor;
  x11selectionevent.selection=x11request->selection;
  x11selectionevent.target=None;
  x11selectionevent.property=x11request->property;
  x11selectionevent.time=x11request->time;
  if (x11request->target==targets){
   XChangeProperty(syswminfo.info.x11.display,x11request->requestor,x11request->property,XA_ATOM,32,PropModeReplace,(unsigned char*)mytargets,3);
  }else{
   if (x11request->target==compoundtext||x11request->target==utf8string||x11request->target==XA_STRING){
    cp=XFetchBytes(syswminfo.info.x11.display,&i);
    XChangeProperty(syswminfo.info.x11.display,x11request->requestor,x11request->property,x11request->target,8,PropModeReplace,(unsigned char*)cp,i);
    XFree(cp);
   }else{
    x11selectionevent.property=None;
   }
  }
  XSendEvent(x11request->display,x11request->requestor,0,NoEventMask,(XEvent*)&x11selectionevent);
  XSync(syswminfo.info.x11.display,False);
 }
}
return 1;
}
void setupx11clipboard(){
static int32 setup=0;
if (!setup){
 setup=1;
 SDL_GetWMInfo(&syswminfo);
 SDL_EventState(SDL_SYSWMEVENT,SDL_ENABLE);
 SDL_SetEventFilter(x11filter);
 syswminfo.info.x11.lock_func();
 targets=XInternAtom(syswminfo.info.x11.display,"TARGETS",True);
 utf8string=XInternAtom(syswminfo.info.x11.display,"UTF8_STRING",True);
 compoundtext=XInternAtom(syswminfo.info.x11.display,"COMPOUND_TEXT",True);
 clipboard=XInternAtom(syswminfo.info.x11.display,"CLIPBOARD",True);
 syswminfo.info.x11.unlock_func();
}
}
void x11clipboardcopy(const char *text){
setupx11clipboard();
syswminfo.info.x11.lock_func();
XStoreBytes(syswminfo.info.x11.display,text,strlen(text)+1);
XSetSelectionOwner(syswminfo.info.x11.display,clipboard,syswminfo.info.x11.window,CurrentTime);
syswminfo.info.x11.unlock_func();
return; 
}
char *x11clipboardpaste(){
static int32 i;
static char *cp;
static unsigned char *cp2;
static Window x11selectionowner;
static SDL_Event sdlevent;
static XEvent x11event;
static unsigned long data_items,bytes_remaining,ignore;
static int format;
static Atom type;
cp=NULL; cp2=NULL;
setupx11clipboard();
syswminfo.info.x11.lock_func();
x11selectionowner=XGetSelectionOwner(syswminfo.info.x11.display,clipboard);
if (x11selectionowner!=None){
 XConvertSelection(syswminfo.info.x11.display,clipboard,utf8string,clipboard,syswminfo.info.x11.window,CurrentTime);
 XFlush(syswminfo.info.x11.display);
 syswminfo.info.x11.unlock_func();
 i=0;
 while (i==0){
  SDL_WaitEvent(&sdlevent);
  if (sdlevent.type==SDL_SYSWMEVENT){
   x11event=sdlevent.syswm.msg->event.xevent;
   if ((x11event.xselection.requestor==syswminfo.info.x11.window)&&(x11event.type==SelectionNotify)) i=1;
  }
 }
 syswminfo.info.x11.lock_func();
 XGetWindowProperty(syswminfo.info.x11.display,syswminfo.info.x11.window,clipboard,0,0,False,AnyPropertyType,&type,&format,&data_items,&bytes_remaining,&cp2);
 if (cp2){XFree(cp2); cp2=NULL;}
 if (bytes_remaining){
  if (XGetWindowProperty(syswminfo.info.x11.display,syswminfo.info.x11.window,clipboard,0,bytes_remaining,False,AnyPropertyType,&type,&format,&data_items, &ignore,&cp2)==Success){
   cp=strdup((char*)cp2);
   XFree(cp2);
  }
 }  
 XDeleteProperty(syswminfo.info.x11.display,syswminfo.info.x11.window,clipboard);
}
syswminfo.info.x11.unlock_func();
return cp;
}
#endif
#endif

qbs *internal_clipboard=NULL;//used only if clipboard services unavailable
int32 linux_clipboard_init=0;

void sub__clipboard(qbs *text){

#ifdef QB64_WINDOWS
static uint8 *textz;
static HGLOBAL h;
if (OpenClipboard(NULL)){
EmptyClipboard();
h=GlobalAlloc(GMEM_MOVEABLE,text->len+1); if (h){
textz=(uint8*)GlobalLock(h); if (textz){
memcpy(textz,text->chr,text->len);
textz[text->len]=0;
GlobalUnlock(h);
SetClipboardData(CF_TEXT,h);
}
}
CloseClipboard();
}
return;
#endif

#ifdef QB64_MACOSX
    PasteboardRef clipboard;
    if (PasteboardCreate(kPasteboardClipboard, &clipboard) != noErr) {
        return;
    }
    if (PasteboardClear(clipboard) != noErr) {
        CFRelease(clipboard);
        return;
    }
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, text->chr, 
                                             text->len, kCFAllocatorNull);
    if (data == NULL) {
        CFRelease(clipboard);
        return;
    }
    OSStatus err;
    err = PasteboardPutItemFlavor(clipboard, NULL, kUTTypeUTF8PlainText, data, 0);
    CFRelease(clipboard);
    CFRelease(data);
return;
#endif

#ifdef QB64_LINUX
#ifndef QB64_MACOSX
while (!display_surface) Sleep(1);
lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
if (!linux_clipboard_init){

linux_clipboard_init=1;
setupx11clipboard();
}
static qbs *textz=NULL; if (!textz) textz=qbs_new(0,0);
qbs_set(textz,qbs_add(text,qbs_new_txt_len("\0",1)));
x11clipboardcopy((char*)textz->chr);
lock_mainloop=0; Sleep(1);//unlock
return;
#endif
#endif

if (internal_clipboard==NULL) internal_clipboard=qbs_new(0,0);
qbs_set(internal_clipboard,text);
}

qbs *func__clipboard(){
#ifdef QB64_WINDOWS
static qbs *text;
static uint8 *textz;
static HGLOBAL h;
if (OpenClipboard(NULL)){
if (IsClipboardFormatAvailable(CF_TEXT)){
h=GetClipboardData(CF_TEXT); if (h){
textz=(uint8*)GlobalLock(h); if (textz){
text=qbs_new(strlen((char*)textz),1);
memcpy(text->chr,textz,text->len);
GlobalUnlock(h);
CloseClipboard();
return text;
}
}
}
CloseClipboard();
}
text=qbs_new(0,1);
return text;
#endif

#ifdef QB64_MACOSX
static qbs *text;
   OSStatus             err = noErr;
   ItemCount            itemCount;
   PasteboardSyncFlags  syncFlags;
   static PasteboardRef inPasteboard = NULL;
   PasteboardCreate( kPasteboardClipboard, &inPasteboard );
   char* data;
   data = "";   
   syncFlags = PasteboardSynchronize( inPasteboard );
   err = badPasteboardSyncErr;
   
    err = PasteboardGetItemCount( inPasteboard, &itemCount );
    require_noerr( err, CantGetPasteboardItemCount );
   
    for( int itemIndex = 1; itemIndex <= itemCount; itemIndex++ ) {
        PasteboardItemID  itemID;
      CFDataRef  flavorData;

        err = PasteboardGetItemIdentifier( inPasteboard, itemIndex, &itemID );
        require_noerr( err, CantGetPasteboardItemIdentifier );

      err = PasteboardCopyItemFlavorData( inPasteboard, itemID, CFSTR("public.utf8-plain-text"), &flavorData );      
      data = (char*)CFDataGetBytePtr(flavorData);

uint32 size;
size=CFDataGetLength( flavorData );



text=qbs_new(size,1);
memcpy(text->chr,data,text->len);
//CFRelease (flavorData);
//CFRelease (flavorTypeArray);
//CFRelease(inPasteboard);
return text;



      
      CantGetPasteboardItemIdentifier:
      ;
    }
   
   CantGetPasteboardItemCount:
   text=qbs_new(0,1);
return text;
    return NULL;



















/*
PasteboardRef inPasteboard;
    if (PasteboardCreate(kPasteboardClipboard, &inPasteboard) != noErr) {
        return NULL;
}


ItemCount  itemCount;

PasteboardSynchronize( inPasteboard );
PasteboardGetItemCount( inPasteboard, &itemCount );
UInt32 itemIndex = 1; // should be 1 or the itemCount?

PasteboardItemID    itemID;
CFArrayRef          flavorTypeArray;
CFIndex             flavorCount;

PasteboardGetItemIdentifier( inPasteboard, itemIndex, &itemID );
PasteboardCopyItemFlavors( inPasteboard, itemID, &flavorTypeArray );

flavorCount = CFArrayGetCount( flavorTypeArray );

for(CFIndex flavorIndex = 0 ; flavorIndex < flavorCount; flavorIndex++ )
{
   CFStringRef  flavorType = (CFStringRef)CFArrayGetValueAtIndex( flavorTypeArray, flavorIndex );

   if (UTTypeConformsTo(flavorType, CFSTR("public.utf8-plain-text")))
   {
       CFDataRef   flavorData;
       PasteboardCopyItemFlavorData( inPasteboard, itemID,flavorType, &flavorData );

       CFIndex  flavorDataSize = CFDataGetLength( flavorData );
       //out.resize( flavorDataSize/2 );
       //memcpy(&out[0], flavorData, flavorDataSize);
       //CFRelease (flavorData);
       //break;

text=qbs_new(flavorDataSize,1);
memcpy(text->chr,flavorData,text->len);
CFRelease (flavorData);
CFRelease (flavorTypeArray);
CFRelease(inPasteboard);
return text;

   }
}
CFRelease (flavorTypeArray);
CFRelease(inPasteboard);
text=qbs_new(0,1);
return text;
*/

/*
PasteboardRef clipboard;
    if (PasteboardCreate(kPasteboardClipboard, &clipboard) != noErr) {
        return NULL;
    }
*/

/*
    PasteboardRef clipboard;
    if (PasteboardCreate(kPasteboardClipboard, &clipboard) != noErr) {
        return NULL;
    }
    if (PasteboardClear(clipboard) != noErr) {
        CFRelease(clipboard);
        return;
    }
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, text->chr, 
                                             text->len, kCFAllocatorNull);
    if (data == NULL) {
        CFRelease(clipboard);
        return;
    }
    OSStatus err;
    err = PasteboardPutItemFlavor(clipboard, NULL, kUTTypeUTF8PlainText, data, 0);
    CFRelease(clipboard);
    CFRelease(data);
*/
#endif

#ifdef QB64_LINUX
#ifndef QB64_MACOSX
static char *cp;
static qbs *text;
while (!display_surface) Sleep(1);
lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
if (!linux_clipboard_init){
linux_clipboard_init=1;
setupx11clipboard();
}
cp=x11clipboardpaste();
if (!cp){
text=qbs_new(0,1);
}else{
text=qbs_new(strlen(cp),1);
memcpy(text->chr,cp,text->len);
free(cp);
}
lock_mainloop=0; Sleep(1);//unlock
return text;
#endif
#endif

if (internal_clipboard==NULL) internal_clipboard=qbs_new(0,0);
return internal_clipboard;
}

void sub__fullscreen(int32 method){
//ref: "[{_OFF|_STRETCH|_SQUAREPIXELS}]"
//          1      2           3
int32 x;
if (method==0) x=1;
if (method==1) x=0;
if (method==2) x=1;
if (method==3) x=2;
if (full_screen==x) return;
full_screen_set=x;
if (autodisplay==1) {while(full_screen_set!=-1) Sleep(32);} else display();
//try alternative if attempted method was unavailable
if (full_screen!=x){
if (x==1){
full_screen_set=2;
if (autodisplay==1) {while(full_screen_set!=-1) Sleep(32);} else display();
}
if (x==2){
full_screen_set=1;
if (autodisplay==1) {while(full_screen_set!=-1) Sleep(32);} else display();
}
}
}

int32 func__fullscreen(){
return full_screen;
}


void chain_restorescreenstate(int32 i){
static int32 i32,i32b,i32c,x,x2;
generic_get(i,-1,(uint8*)&i32,4);

if (i32==256){
generic_get(i,-1,(uint8*)&i32,4);
if (i32!=0) qbg_screen(i32,0,0,0,0,1);
generic_get(i,-1,(uint8*)&i32,4);
	if (i32==258){
	generic_get(i,-1,(uint8*)&i32,4); i32b=i32;
	generic_get(i,-1,(uint8*)&i32,4);
	qbsub_width(0,i32b,i32,1+2);
	generic_get(i,-1,(uint8*)&i32,4);
	}
}

if (i32==257){
generic_get(i,-1,(uint8*)&i32,4); i32c=i32;
generic_get(i,-1,(uint8*)&i32,4); i32b=i32;
generic_get(i,-1,(uint8*)&i32,4);
qbg_screen(func__newimage(i32b,i32,i32c,1),0,0,0,0,1);
generic_get(i,-1,(uint8*)&i32,4);
}

if (i32==259){
generic_get(i,-1,(uint8*)&i32,4);
sub__font(i32,0,0);
generic_get(i,-1,(uint8*)&i32,4);
}

static img_struct *ix;
static img_struct imgs;

while(i32==260){
generic_get(i,-1,(uint8*)&i32,4); x=i32;
qbg_screen(0,0,x,0,0,4+8);//switch to page (allocates the page)
ix=&img[page[x]];
generic_get(i,-1,ix->offset,ix->width*ix->height*ix->bytes_per_pixel);
imgs=*ix;
generic_get(i,-1,(uint8*)ix,sizeof(img_struct));
//revert specific data
if (ix->font>=32) ix->font=imgs.font;
ix->offset=imgs.offset;
ix->pal=imgs.pal;
generic_get(i,-1,(uint8*)&i32,4);
}

if (i32==261){
generic_get(i,-1,(uint8*)&i32,4); i32b=i32;
generic_get(i,-1,(uint8*)&i32,4);
qbg_screen(0,0,i32b,i32,0,4+8);//switch to correct active & visual pages
generic_get(i,-1,(uint8*)&i32,4);
}

if (i32==262){
for (x=0;x<=255;x++){
generic_get(i,-1,(uint8*)&i32,4);
sub__palettecolor(x,i32,0,1);
}
generic_get(i,-1,(uint8*)&i32,4);
}


//assume command #511("finished screen state") in i32
}

void chain_savescreenstate(int32 i){//adds the screen state to file #i
static int32 i32,x,x2;
static img_struct *i0,*ix;
i0=&img[page[0]];

if ( (i0->offset>cmem) && (i0->offset<(cmem+1114099)) ){//cmem?[need to maintain cmem state]
//[256][mode]
i32=256; generic_put(i,-1,(uint8*)&i32,4);
i32=i0->compatible_mode; generic_put(i,-1,(uint8*)&i32,4);
	if (i0->text){
	//[258][WIDTH:X][Y]
	i32=258; generic_put(i,-1,(uint8*)&i32,4);
	i32=i0->width; generic_put(i,-1,(uint8*)&i32,4);
	i32=i0->height; generic_put(i,-1,(uint8*)&i32,4);
	}
}else{
//[257][mode][X][Y]
i32=257; generic_put(i,-1,(uint8*)&i32,4);
i32=i0->compatible_mode; generic_put(i,-1,(uint8*)&i32,4);
i32=i0->width; generic_put(i,-1,(uint8*)&i32,4);
i32=i0->height; generic_put(i,-1,(uint8*)&i32,4);
}
//[259][font] (standard fonts only)
if (i0->font<32){
i32=259; generic_put(i,-1,(uint8*)&i32,4);
i32=i0->font; generic_put(i,-1,(uint8*)&i32,4);
}

//[260][page][rawdata]
//note: write page is done last to avoid having its values undone by the later page switch
x2=-1;
for (x=0;x<pages;x++){
if (page[x]){
if (page[x]!=write_page_index){
save_write_page:
ix=&img[page[x]];
i32=260; generic_put(i,-1,(uint8*)&i32,4);
i32=x; generic_put(i,-1,(uint8*)&i32,4);
generic_put(i,-1,ix->offset,ix->width*ix->height*ix->bytes_per_pixel);
//save structure (specific parts will be reincorporated)
generic_put(i,-1,(uint8*)ix,sizeof(img_struct));
if (x==x2) break;
}else x2=x;
}
}
if ((x2!=-1)&&(x!=x2)){x=x2; goto save_write_page;}

//[261][activepage][visualpage]
i32=261; generic_put(i,-1,(uint8*)&i32,4);
i32=0;//note: activepage could be a non-screenpage
for (x=0;x<pages;x++){
if (page[x]==write_page_index){i32=x;break;}
}
generic_put(i,-1,(uint8*)&i32,4);
i32=0;
for (x=0;x<pages;x++){
if (page[x]==display_page_index){i32=x;break;}
}
generic_put(i,-1,(uint8*)&i32,4);

//[262][256x32-bit color palette]
if (i0->bytes_per_pixel!=4){
i32=262; generic_put(i,-1,(uint8*)&i32,4);
for (x=0;x<=255;x++){
i32=func__palettecolor(x,0,1); generic_put(i,-1,(uint8*)&i32,4);
}
}

//[511](screen state finished)
i32=511; generic_put(i,-1,(uint8*)&i32,4);

}//chain_savescreenstate






int32 gfs_new(){
static int32 i;
if (gfs_freed_n){
 i=gfs_freed[--gfs_freed_n];
}else{
 i=gfs_n;
 gfs_n++;
 gfs_file=(gfs_file_struct*)realloc(gfs_file,gfs_n*sizeof(gfs_file_struct));
  #ifdef GFS_WINDOWS
  gfs_file_win=(gfs_file_win_struct*)realloc(gfs_file_win,gfs_n*sizeof(gfs_file_win_struct));
  #endif
}
memset(&gfs_file[i],0,sizeof(gfs_file_struct));
 #ifdef GFS_WINDOWS
 ZeroMemory(&gfs_file_win[i],sizeof(gfs_file_win_struct));
 #endif
gfs_file[i].id=gfs_nextid++;
return i;
}

int32 gfs_validhandle(int32 i){
if ((i<0)||(i>=gfs_n)) return 0;
if (gfs_file[i].open) return 1;
return 0;
}

int32 gfs_fileno_valid(int32 f){
//returns: -2	invalid handle
//			1	in use
//			0	unused
if (f<=0) return -2;
if (f<=gfs_fileno_n){
if (gfs_fileno[f]==-1) return 0; else return 1;
}
gfs_fileno=(int32*)realloc(gfs_fileno,(f+1)*4);
memset(gfs_fileno+gfs_fileno_n+1,-1,(f-gfs_fileno_n)*4);
gfs_fileno_n=f;
return 0;
}

int32 gfs_fileno_freefile(){//like FREEFILE
//note: for QBASIC compatibility the lowest available file number is returned
static int32 x;
for (x=1;x<=gfs_fileno_n;x++) if (gfs_fileno[x]==-1) return x;
return gfs_fileno_n+1;
}

void gfs_fileno_use(int32 f, int32 i){
//assumes valid handles
gfs_fileno[f]=i;
gfs_file[i].fileno=f;
}

void gfs_fileno_free(int32 f){//note: called by gfs_free (DO NOT CALL THIS FUNCTION)
gfs_fileno[f]=-1;
}

int32 gfs_free(int32 i){
if (!gfs_validhandle(i)) return -2;//invalid handle
if (gfs_freed_size<=gfs_freed_n){
gfs_freed_size++;
gfs_freed=(int32*)realloc(gfs_freed,gfs_freed_size*4);
}
gfs_file[i].open=0;
if (gfs_file[i].fileno) gfs_fileno_free(gfs_file[i].fileno);
gfs_freed[gfs_freed_n++]=i;
return 0;
}

int32 gfs_close(int32 i){
static int32 x;
if (x=gfs_free(i)) return x;

if (gfs_file[i].field_buffer){free(gfs_file[i].field_buffer); gfs_file[i].field_buffer=NULL;}
if (gfs_file[i].field_strings){free(gfs_file[i].field_strings); gfs_file[i].field_strings=NULL;}

 #ifdef GFS_C
 static gfs_file_struct *f;
 f=&gfs_file[i];
 f->file_handle->close();
 delete f->file_handle;
 return 0;
 #endif

 #ifdef GFS_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 CloseHandle(f_w->file_handle);
 return 0;
 #endif

return -1;
}

int64 gfs_lof(int32 i){
if (!gfs_validhandle(i)) return -2;//invalid handle
static gfs_file_struct *f;
f=&gfs_file[i];

 #ifdef GFS_C
 f->file_handle->clear();
 if (f->read){
 static int64 bytes;
 f->file_handle->seekg(0,ios::end);
 bytes=f->file_handle->tellg();
 f->file_handle->seekg(f->pos);
 return bytes;
 }
 if (f->write){
 static int64 bytes;
 f->file_handle->seekp(0,ios::end);
 bytes=f->file_handle->tellp();
 f->file_handle->seekp(f->pos);
 return bytes;
 }
 return -1;
 #endif

 #ifdef GFS_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 static int64 bytes;
 *((int32*)&bytes)=GetFileSize(f_w->file_handle,(DWORD*)(((int32*)&bytes)+1));
 if ((bytes&0xFFFFFFFF)==0xFFFFFFFF){
 if (GetLastError()!=NO_ERROR) return -3;//bad/incorrect file mode
 }
 return bytes;
 #endif

return -1;
}

int32 gfs_open_com_syntax(qbs *fstr,gfs_file_struct *f){
//0=not an open com statement
//-1=syntax error
//1=valid
//check if filename is a COM open command
static int32 x,i;
static int32 v,c,z;
f->com_port=0;
if (fstr->len<=3) return 0;
if ((fstr->chr[0]&223)!=67) return 0;//"C"/"c"
if ((fstr->chr[1]&223)!=79) return 0;//"O"/"o"
if ((fstr->chr[2]&223)!=77) return 0;//"M"/"m"
v=-1;
for (i=3;i<fstr->len-1;i++){
c=fstr->chr[i];
if (c==58) goto comstatment;
if ((c<48)||(c>57)) return 0;//not 0-9
if (v==-1){
if (c==48) return 0;//first digit 0
v=0;
}
v=v*10+(c-48);
}
return 0;//no ":"
comstatment:
if ((x>=7)||(v<=0)||(v>255)) return -1;//invalid port number (1-255)
f->com_port=v;

//COM open confirmed
static qbs *str=NULL;
if (!str) str=qbs_new(0,0);
qbs_set(str,qbs_ucase(fstr));
str->len--;//remove null term.

//set option values to uninitialized
//---group 1
f->com_baud_rate=-1;
f->com_parity=-1;
f->com_data_bits_per_byte=-1;
f->com_stop_bits=-1;
//---group 2
f->com_rs=-1;
f->com_bin_asc=-1;
f->com_asc_lf=-1;
f->com_cd_x=-1;
f->com_cs_x=-1;
f->com_ds_x=-1;
f->com_op_x=-1;

static int32 str_or_num;
static int64 strv;
static int32 stage;
static int32 com_rb_used,com_tb_used;
com_rb_used=0; com_tb_used=0; stage=1;
str_or_num=1; strv=0; v=-1;
for (i=i+1;i<str->len;i++){ c=str->chr[i];
if (c!=44){
if ((c<48)||((c>57)&&(c<65))||(c>90)) return -1;//invalid character
if ((str_or_num==2)&&(c>=65)) return -1;//invalid character
if (c<65) str_or_num=2;//ABC->123
if ((str_or_num==1)||(stage==4)){//note: stage 4 is interpreted as a string
 if (strv&0xFF0000) strv=strv|(c<<24); else
 if (strv&0x00FF00) strv=strv|(c<<16); else
 if (strv&0x0000FF) strv=strv|(c<<8); else
 strv=strv=c;
 if (strv>16777216) return -1;//string option too long (max 3 characters)
}else{
 if ((c>48)&&(c<=57)){
 if (v==-2) return -1;//leading 0s are invalid
 if (v==-1) v=0;
 v=v*10+(c-48);
 }else{//0
 if (v==-2) return -1;//leading 0s are invalid
 if (v==-1) v=-2;//0...
 if (v>0) v=v*10; 
 }
 if (v>2147483647) return -1;//numeric value too large (LONG values only)
}
}//c!=44
if ((c==44)||(i==str->len-1)){
if (v==-2) v=0;
//note: v==-1 means omit
if (stage==1){
if (f->com_baud_rate!=-1) return -1;
if (strv) return -1;
if (v==0) return -1;
if (v==-1) v=300;
f->com_baud_rate=v;
stage++; goto done_stage;
}
if (stage==2){
if (f->com_parity!=-1) return -1;
if (v!=-1) return -1;
x=-1;
if (strv==78) x=0;//N
if (strv==0) x=1;//E*
if (strv==69) x=1;//E
if (strv==79) x=2;//O
if (strv==83) x=3;//S
if (strv==77) x=4;//M
if (strv==17744) x=5;//PE
if (x==-1) return -1;
f->com_parity=x;
stage++; goto done_stage;
}
if (stage==3){
if (f->com_data_bits_per_byte!=-1) return -1;
if (strv) return -1;
x=-1;
if (v==-1) x=7;
if (v==5) x=5;
if (v==6) x=6;
if (v==7) x=7;
if (v==8) x=8;
if (x==-1) return -1;
f->com_data_bits_per_byte=x;
stage++; goto done_stage;
}
if (stage==4){
if (f->com_stop_bits!=-1) return -1;
if (v!=-1) return -1;
x=-1;
if (strv==0){
x=10;
if (f->com_baud_rate<=110){
x=20;
if (f->com_data_bits_per_byte==5) x=15;
}
}//0
if (strv==49) x=10;//"1"
if (strv==3485233) x=15;//"1.5"
if (strv==50) x=20;//"2"
if (x==-1) return -1;
f->com_stop_bits=x;
stage++; goto done_stage;
}
//stage>4
if (!strv) return -1;//all options after 4 require a string
if (strv==21330){ if (f->com_rs!=-1) return -1;//RS
f->com_rs=1; 
goto done_stage;}
if (strv==5130562){ if (f->com_bin_asc!=-1) return -1;//BIN
f->com_bin_asc=0;
goto done_stage;}
if (strv==4412225){ if (f->com_bin_asc!=-1) return -1;//ASC
f->com_bin_asc=1;
goto done_stage;}
if (strv==16980){ if (com_tb_used) return -1;//TB
com_tb_used=1;
goto done_stage;}
if (strv==16978){ if (com_rb_used) return -1;//RB
com_rb_used=1;
goto done_stage;}
if (strv==17996){ if (f->com_asc_lf!=-1) return -1;//LF
f->com_asc_lf=1;
goto done_stage;}
if (strv==17475){ if (f->com_cd_x!=-1) return -1;//CD
if (v==-1) v=0;
if (v>65535) return -1;
f->com_cd_x=v;
goto done_stage;}
if (strv==21315){ if (f->com_cs_x!=-1) return -1;//CS
if (v==-1) v=1000;
if (v>65535) return -1;
f->com_cs_x=v;
goto done_stage;}
if (strv==21316){ if (f->com_ds_x!=-1) return -1;//DS
if (v==-1) v=1000;
if (v>65535) return -1;
f->com_ds_x=v;
goto done_stage;}
if (strv==20559){ if (f->com_op_x!=-1) return -1;//OP
if (v==-1) v=10000;
if (v>65535) return -1;
f->com_op_x=v;
goto done_stage;}
return -1;//invalid option
done_stage:
str_or_num=1; strv=0; v=-1;
}
}//i

//complete omitted options with defaults
if (f->com_baud_rate==-1) f->com_baud_rate=300;
if (f->com_parity==-1) f->com_parity=1;
if (f->com_data_bits_per_byte==-1) f->com_data_bits_per_byte=7;
if (f->com_stop_bits==-1){
x=10;
if (f->com_baud_rate<=110){
x=20;
if (f->com_data_bits_per_byte==5) x=15;
}
f->com_stop_bits=x; 
}
if (f->com_bin_asc==-1) f->com_bin_asc=0;
if (f->com_asc_lf==-1) f->com_asc_lf=0;
if (f->com_asc_lf==1){
if (f->com_bin_asc==0) f->com_asc_lf=0;
}
if (f->com_rs==-1) f->com_rs=0;
if (f->com_cd_x==-1) f->com_cd_x=0;
if (f->com_cs_x==-1) f->com_cs_x=1000;
if (f->com_ds_x==-1) f->com_ds_x=1000;
if (f->com_op_x==-1){
x=f->com_cd_x*10; z=f->com_ds_x*10;
if (z>x) x=z;
if (x>65535) x=65535;
f->com_op_x=x;
}

}


int32 gfs_open(qbs *filename,int32 access,int32 restrictions, int32 how){

//filename - an OS compatible filename (doesn't need to be NULL terminated)
//access - 1=read, 2=write, 3=read and write
//restrictions - 1=others cannot read, 2=others cannot write, 3=others cannot read or write(exclusive access)
//how - 1=create(if it doesn't exist), 2=create(if it doesn't exist) & truncate
//      3=create(if it doesn't exist)+undefined access[get whatever access is available]
static int32 i,x,x2,x3,e;
static qbs *filenamez=NULL;
if (!filenamez) filenamez=qbs_new(0,0);
qbs_set(filenamez,qbs_add(filename,qbs_new_txt_len("\0",1)));
i=gfs_new();
static gfs_file_struct *f;
f=&gfs_file[i];
if (access&1) f->read=1;
if (access&2) f->write=1;
if (restrictions&1) f->lock_read=1;
if (restrictions&2) f->lock_write=1;
f->pos=0;

//check for OPEN COM syntax
if (x=gfs_open_com_syntax(filenamez,f)){
if (x==-1){gfs_free(i); return -11;}//-11 bad file name
//note: each GFS implementation will handle COM communication differently
}

 #ifdef GFS_C
 //note: GFS_C ignores restrictions/locking
 f->file_handle=new fstream();
 //attempt as if it exists:
 if (how==2){
 //with truncate
 if (access==1) f->file_handle->open(fixdir(filenamez),ios::in|ios::binary|ios::trunc);
 if (access==2) f->file_handle->open(fixdir(filenamez),ios::out|ios::binary|ios::trunc);
 if (access==3) f->file_handle->open(fixdir(filenamez),ios::in|ios::out|ios::binary|ios::trunc);
 }else{
 //without truncate
 if (access==1) f->file_handle->open(fixdir(filenamez),ios::in|ios::binary);
 if (access==2) f->file_handle->open(fixdir(filenamez),ios::out|ios::binary|ios::app);
 if (access==3) f->file_handle->open(fixdir(filenamez),ios::in|ios::out|ios::binary);
 }
 if (how){
 if (!f->file_handle->is_open()){//couldn't open file, so attempt creation
 f->file_handle_o=new ofstream();
 f->file_handle_o->open(fixdir(filenamez),ios::out);
 if (f->file_handle_o->is_open()){//created new file
  f->file_handle_o->close();
  //retry open 
  f->file_handle->clear();
  if (how==2){
  //with truncate
  if (access==1) f->file_handle->open(fixdir(filenamez),ios::in|ios::binary|ios::trunc);
  if (access==2) f->file_handle->open(fixdir(filenamez),ios::out|ios::binary|ios::trunc);
  if (access==3) f->file_handle->open(fixdir(filenamez),ios::in|ios::out|ios::binary|ios::trunc);
  }else{
  //without truncate
  if (access==1) f->file_handle->open(fixdir(filenamez),ios::in|ios::binary);
  if (access==2) f->file_handle->open(fixdir(filenamez),ios::out|ios::binary|ios::app);
  if (access==3) f->file_handle->open(fixdir(filenamez),ios::in|ios::out|ios::binary);
  }
 }
 delete f->file_handle_o;
 }
 }//how 
 if (!f->file_handle->is_open()){//couldn't open file
 delete f->file_handle;
 gfs_free(i);
 if (how) return -11;//Bad file name
 return -5;//File not found
 }
 //file opened successfully
 f->open=1;
 return i;
 #endif

 #ifdef GFS_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 x=0;
 if (access&1)x|=GENERIC_READ;
 if (access&2)x|=GENERIC_WRITE;
 x2=FILE_SHARE_READ|FILE_SHARE_WRITE;
 if (restrictions&1)x2^=FILE_SHARE_READ;
 if (restrictions&2)x2^=FILE_SHARE_WRITE;

 if (f->com_port){//open a com port
 static qbs *portname=NULL; if (!portname) portname=qbs_new(0,0);
 qbs_set(portname,qbs_add(qbs_new_txt("CO"),qbs_str(f->com_port)));
 qbs_set(portname,qbs_add(portname,qbs_new_txt_len(":\0",2)));
 portname->chr[2]=77;//replace " " with "M"
 f_w->file_handle=CreateFile((char*)portname->chr,x,0,NULL,OPEN_EXISTING,0,NULL);
 if (f_w->file_handle==INVALID_HANDLE_VALUE){gfs_free(i); return -8;}//device unavailable
 static DCB cs;
 ZeroMemory(&cs,sizeof(DCB));
 cs.DCBlength=sizeof(DCB);
 if (!GetCommState(f_w->file_handle,&cs)){CloseHandle(f_w->file_handle); gfs_free(i); return -8;}//device unavailable
 static COMMTIMEOUTS ct;
 ZeroMemory(&ct,sizeof(COMMTIMEOUTS));
 /*dump port state and return "file not found" (used for debugging only)
 if (!GetCommTimeouts(f_w->file_handle,&ct)){CloseHandle(f_w->file_handle); gfs_free(i); return -8;}//device unavailable
 ofstream mydump("f:\\comdump.bin");
 mydump.write((char*)&cs,sizeof(cs));
 mydump.write((char*)&ct,sizeof(ct));
 mydump.close();
 CloseHandle(f_w->file_handle); gfs_free(i);
 return -4;
 */
 cs.BaudRate=f->com_baud_rate;
 x=f->com_stop_bits;
 if (x==10) x2=0;
 if (x==15) x2=1;
 if (x==20) x2=2;
 cs.StopBits=x2;
 cs.ByteSize=f->com_data_bits_per_byte; 
 x=f->com_parity;
 if (x==0) x2=0;
 if (x==1) x2=2;
 if (x==2) x2=1;
 if (x==3) x2=4;
 if (x==4) x2=3;
 //if (x==5) x2=... ***"PE" will be supported later***
 //0-4=None,Odd,Even,Mark,Space  
 cs.Parity=x2; if (x2==0) cs.fParity=0; else cs.fParity=1;
 if (f->com_rs) cs.fRtsControl=RTS_CONTROL_DISABLE;
 if (f->com_bin_asc==0) cs.fBinary=1; else cs.fBinary=0;
 cs.EofChar=26;
 if(!SetCommState(f_w->file_handle,&cs)){CloseHandle(f_w->file_handle); gfs_free(i); return -8;}//device unavailable
 if (f->com_ds_x==0){
  //A value of MAXDWORD, combined with zero values for both the ReadTotalTimeoutConstant and ReadTotalTimeoutMultiplier members, specifies that the read operation is to return immediately with the characters that have already been received, even if no characters have been received.
  ct.ReadIntervalTimeout=MAXDWORD;
  ct.ReadTotalTimeoutMultiplier=0;
  ct.ReadTotalTimeoutConstant=0;
 }else{
  ct.ReadIntervalTimeout=0;
  ct.ReadTotalTimeoutMultiplier=0;
  ct.ReadTotalTimeoutConstant=f->com_ds_x;
 }
 ct.WriteTotalTimeoutMultiplier=0;
 ct.WriteTotalTimeoutConstant=f->com_cs_x;
 if (!SetCommTimeouts(f_w->file_handle,&ct)){CloseHandle(f_w->file_handle); gfs_free(i); return -8;}//device unavailable
 f->open=1;
 return i;
 }

  /*
  #define CREATE_NEW          1
  #define CREATE_ALWAYS       2
  #define OPEN_EXISTING       3
  #define OPEN_ALWAYS         4
  #define TRUNCATE_EXISTING   5
  */ 
 x3=OPEN_EXISTING;
 if (how) x3=OPEN_ALWAYS;
 undefined_retry: 
 f_w->file_handle=CreateFile(fixdir(filenamez),x,x2,NULL,x3,FILE_ATTRIBUTE_NORMAL,NULL);
 if (f_w->file_handle==INVALID_HANDLE_VALUE){

 if (how==3){
 //attempt read access only
 x=GENERIC_READ;
 f->read=1;
 f->write=0;
 how++;
 goto undefined_retry;
 }

 if (how==4){
 //attempt write access only
 x=GENERIC_WRITE;
 f->read=0;
 f->write=1;
 how++;
 goto undefined_retry;
 }

 gfs_free(i);
 e=GetLastError();
 if (e==3) return -6;
 if ((e==4)||(e==29)||(e==30)) return -9;
 if ((e==5)||(e==19)||(e==33)||(e==32)) return -7;
 if ((e==15)||(e==21)) return -8;
 if (e==2) return -5;
 //showvalue(e);
 return -5;//assume (2)
 }//invalid handle

 if (how==2){
 //truncate file if size is not 0
 static DWORD GetFileSize_low,GetFileSize_high;
 GetFileSize_low=GetFileSize(f_w->file_handle,&GetFileSize_high);
 if (GetFileSize_low||GetFileSize_high){
 CloseHandle(f_w->file_handle);
 x3=TRUNCATE_EXISTING;
 f_w->file_handle=CreateFile(fixdir(filenamez),x,x2,NULL,x3,FILE_ATTRIBUTE_NORMAL,NULL);

 if (f_w->file_handle==INVALID_HANDLE_VALUE){
 gfs_free(i);
 e=GetLastError();
 if (e==3) return -6;
 if ((e==4)||(e==29)||(e==30)) return -9;
 if ((e==5)||(e==19)||(e==33)||(e==32)) return -7;
 if ((e==15)||(e==21)) return -8;
 if (e==2) return -5;
 //showvalue(e);
 return -5;//assume (2)
 }//invalid handle

 }
 }//how==2
 f->open=1;
 return i;
 #endif


return -1;//non-specific fail

}

int32 gfs_setpos(int32 i, int64 position){
if (!gfs_validhandle(i)) return -2;//invalid handle
if (position<0) return -4;//illegal function call

static gfs_file_struct *f;
f=&gfs_file[i];

 #ifdef GFS_C
 if (f->read){
 f->file_handle->clear();
 f->file_handle->seekg(position);
 }
 if (f->write){
 f->file_handle->clear();
 f->file_handle->seekp(position);
 }
 f->pos=position;
 return 0;
 #endif



 #ifdef GFS_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 if (SetFilePointer(f_w->file_handle,(int32)position,(long*)(((int32*)&position)+1),FILE_BEGIN)==0xFFFFFFFF){/*Note that it is not an error to set the file pointer to a position beyond the end of the file. The size of the file does not increase until you call the SetEndOfFile, WriteFile, or WriteFileEx function.*/
 if (GetLastError()!=NO_ERROR){
 return -3;//bad file mode
 }
 }
 f->pos=position;
 return 0;
 #endif

return -1;
}

int64 gfs_getpos(int32 i){
if (!gfs_validhandle(i)) return -2;//invalid handle
static gfs_file_struct *f;
f=&gfs_file[i];
return f->pos;
}

int32 gfs_write(int32 i,int64 position,uint8 *data,int64 size){
if (!gfs_validhandle(i)) return -2;//invalid handle
static int32 e;
static gfs_file_struct *f;
f=&gfs_file[i];
if (!f->write) return -3;//bad file mode
if (size<0) return -4;//illegal function call
static int32 x;
if (position!=-1){
if (x=gfs_setpos(i,position)) return x;//(pass on error)
}

 #ifdef GFS_C
 f->file_handle->clear();
 f->file_handle->write((char*)data,size);
 if (f->file_handle->bad()){
 return -7;//assume: permission denied
 }
 f->pos+=size;
 return 0;
 #endif

 #ifdef GFS_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 static uint32 size2;
 static int64 written=0;
 while(size){
 if (size>4294967295){
 size2=4294967295;
 size-=4294967295;
 }else{
 size2=size; size=0;
 }
 if (!WriteFile(f_w->file_handle,data,size2,(unsigned long*)&written,NULL)){
 e=GetLastError();
 if ((e==5)||(e==33)) return -7;//permission denied
 return -9;//assume: path/file access error
 }
 data+=written; f->pos+=written;
 if (written!=size2) return -1;
 }
 return 0;
 #endif

return -1;
}


int64 gfs_read_bytes_value;
int64 gfs_read_bytes(){
return gfs_read_bytes_value;
}

int32 gfs_read(int32 i,int64 position,uint8 *data,int64 size){
gfs_read_bytes_value=0;
if (!gfs_validhandle(i)) return -2;//invalid handle
static int32 e;
static gfs_file_struct *f;
f=&gfs_file[i];
if (!f->read) return -3;//bad file mode
if (size<0) return -4;//illegal function call
static int32 x;
if (position!=-1){
if (x=gfs_setpos(i,position)) return x;//(pass on error)
}

 #ifdef GFS_C
 f->file_handle->clear();
 f->file_handle->read((char*)data,size); 
 if (f->file_handle->bad()){//note: 'eof' also sets the 'fail' flag, so only the the 'bad' flag is checked
 return -7;//assume: permission denied
 }
 static int64 bytesread;
 bytesread=f->file_handle->gcount();
 gfs_read_bytes_value=bytesread;
 f->pos+=bytesread;
 if (bytesread<size){
 memset(data+bytesread,0,size-bytesread);
 f->eof_passed=1;
 return -10;
 }
 f->eof_passed=0;
 return 0;
 #endif

 #ifdef GFS_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 static uint32 size2;
 static int64 bytesread=0;
 while(size){
 if (size>4294967295){
 size2=4294967295;
 size-=4294967295;
 }else{
 size2=size; size=0;
 }

 if (ReadFile(f_w->file_handle,data,size2,(unsigned long*)&bytesread,NULL)){
 data+=bytesread; f->pos+=bytesread; gfs_read_bytes_value+=bytesread;
  if (bytesread!=size2){   
   ZeroMemory(data,size+(size2-bytesread));//nullify remaining buffer
   f->eof_passed=1; return -10;
  }//eof passed
 }else{
 //error
 e=GetLastError();
 if ((e==5)||(e==33)) return -7;//permission denied
 //showvalue(e);
 return -9;//assume: path/file access error
 }
 }
 f->eof_passed=0;
 return 0;
 #endif

return -1;
}

int32 gfs_eof_reached(int32 i){
if (!gfs_validhandle(i)) return -2;//invalid handle
if (gfs_getpos(i)>=gfs_lof(i)) return 1;
return 0;
}

int32 gfs_eof_passed(int32 i){
if (!gfs_validhandle(i)) return -2;//invalid handle
static gfs_file_struct *f;
f=&gfs_file[i];
if (f->eof_passed) return 1;
return 0;
}

int32 gfs_lock(int32 i,int64 offset_start,int64 offset_end){
//if offset_start==-1, 'from beginning' (typically offset 0) is assumed
//if offset_end==-1, 'to end/infinity' is assumed
//range is inclusive of start and end
if (!gfs_validhandle(i)) return -2;//invalid handle
static gfs_file_struct *f;
f=&gfs_file[i];

if (offset_start==-1) offset_start=0;
if (offset_start<0) return -4;//illegal function call
if (offset_end<-1) return -4;//illegal function call
//note: -1 equates to highest uint64 value (infinity)
//      All other negative end values are illegal

 #ifdef GFS_C
 return 0;
 #endif

 #ifdef GFS_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 uint64 bytes;
 bytes=offset_end;
 if (bytes!=-1) bytes=bytes-offset_start+1;
 if (!LockFile(f_w->file_handle,*((DWORD*)(&offset_start)),*(((DWORD*)(&offset_start))+1),*((DWORD*)(&bytes)),*(((DWORD*)(&bytes))+1))){
 //failed
 static int32 e;
 e=GetLastError();
 if ((e==5)||(e==33)) return -7;//permission denied
 //showvalue(e);
 return -9;//assume: path/file access error
 }
 return 0;
 #endif

return -1;
}

int32 gfs_unlock(int32 i,int64 offset_start,int64 offset_end){
//if offset_start==-1, 'from beginning' (typically offset 0) is assumed
//if offset_end==-1, 'to end/infinity' is assumed
//range is inclusive of start and end
if (!gfs_validhandle(i)) return -2;//invalid handle
static gfs_file_struct *f;
f=&gfs_file[i];

if (offset_start==-1) offset_start=0;
if (offset_start<0) return -4;//illegal function call
if (offset_end<-1) return -4;//illegal function call
//note: -1 equates to highest uint64 value (infinity)
//      All other negative end values are illegal

 #ifdef GFS_C
 return 0;
 #endif

 #ifdef GFS_WINDOWS
 static gfs_file_win_struct *f_w;
 f_w=&gfs_file_win[i];
 uint64 bytes;
 bytes=offset_end;
 if (bytes!=-1) bytes=bytes-offset_start+1;
 if (!UnlockFile(f_w->file_handle,*((DWORD*)(&offset_start)),*(((DWORD*)(&offset_start))+1),*((DWORD*)(&bytes)),*(((DWORD*)(&bytes))+1))){
 //failed
 static int32 e;
 e=GetLastError(); 
 if ((e==5)||(e==33)||(e==158)) return -7;//permission denied
 //showvalue(e);
 return -9;//assume: path/file access error
 }
 return 0;
 #endif

return -1;
}

void sub_lock(int32 i,int64 start,int64 end,int32 passed){
if (new_error) return;
if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];

//If the file has been opened for sequential input or output, LOCK and UNLOCK affect the entire file, regardless of the range specified by start& and end&.
if (gfs->type>2) passed=0;

if (passed&1){
start--;
if (start<0){error(5); return;}
if (gfs->type==1) start*=gfs->record_length;
}else{
start=-1;
}

if (passed&2){
end--;
if (end<0){error(5); return;}
if (gfs->type==1) end=end*gfs->record_length+gfs->record_length-1;
}else{
end=start;
if (gfs->type==1) end=start+gfs->record_length-1;
if (!(passed&1)) end=-1;
}


int32 e;
e=gfs_lock(i,start,end);
if (e){
if (e==-2){error(258); return;}//invalid handle
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}

}

void sub_unlock(int32 i,int64 start,int64 end,int32 passed){
if (new_error) return;
if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
static gfs_file_struct *gfs;
gfs=&gfs_file[i];

//If the file has been opened for sequential input or output, LOCK and UNLOCK affect the entire file, regardless of the range specified by start& and end&.
if (gfs->type>2) passed=0;

if (passed&1){
start--;
if (start<0){error(5); return;}
if (gfs->type==1) start*=gfs->record_length;
}else{
start=-1;
}

if (passed&2){
end--;
if (end<0){error(5); return;}
if (gfs->type==1) end=end*gfs->record_length+gfs->record_length-1;
}else{
end=start;
if (gfs->type==1) end=start+gfs->record_length-1;
if (!(passed&1)) end=-1;
}

int32 e;
e=gfs_unlock(i,start,end);
if (e){
if (e==-2){error(258); return;}//invalid handle
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}

}

int32 func__screenimage(){

#ifdef QB64_WINDOWS

static HWND hwnd;
hwnd=GetDesktopWindow();
static RECT rect;
GetWindowRect(hwnd,&rect);
static int32 x,y;
x=rect.right-rect.left;
y=rect.bottom-rect.top;
static HDC hdc;
hdc=GetDC(hwnd);
static HDC hdc2;
hdc2=CreateCompatibleDC(hdc);
static HBITMAP bitmap;
bitmap=CreateCompatibleBitmap(hdc,x,y);

SelectObject(hdc2,bitmap);
BitBlt(        hdc2, 
               0,0, 
               x,y, 
               hdc, 
               0,0,
               SRCCOPY);


    static BITMAPFILEHEADER   bmfHeader;  
    static BITMAPINFOHEADER   bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = x;    
    bi.biHeight = y;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;  
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;

static int32 i,i2,i3;
i2=func__dest();
i=func__newimage(x,y,32,1);
sub__dest(i);
GetDIBits(hdc2,bitmap,0,y,write_page->offset,(BITMAPINFO*)&bi, DIB_RGB_COLORS);
sub__setalpha(255,NULL,NULL,NULL,0);

i3=func__newimage(x,y,32,1);
sub__dontblend(i,1);
sub__dontblend(i3,1);
sub__putimage(0,y-1,x-1,0,i,i3,NULL,NULL,NULL,NULL,1+4+8+32);
sub__freeimage(i,1);
sub__blend(i3,1);
sub__dest(i2);

DeleteObject(bitmap);
DeleteDC(hdc2);
ReleaseDC(NULL,hdc);

return i3;

/*
//   FUNCTION: CaptureAnImage(HWND hWnd)
//
//   PURPOSE: Captures a screenshot into a window and then saves it in a .bmp file.
//
//   COMMENTS: 
//
//      Note: This sample will attempt to create a file called captureqwsx.bmp 
//        

int CaptureAnImage(HWND hWnd)
{
    HDC hdcScreen;
    HDC hdcWindow;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;

    // Retrieve the handle to a display device context for the client 
    // area of the window. 
    hdcScreen = GetDC(NULL);
    hdcWindow = GetDC(hWnd);

    // Create a compatible DC which is used in a BitBlt from the window DC
    hdcMemDC = CreateCompatibleDC(hdcWindow); 

    if(!hdcMemDC)
    {
        MessageBox(hWnd, L"StretchBlt has failed",L"Failed", MB_OK);
        goto done;
    }

    // Get the client area for size calculation
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    //This is the best stretch mode
    SetStretchBltMode(hdcWindow,HALFTONE);

    //The source DC is the entire screen and the destination DC is the current window (HWND)
    if(!StretchBlt(hdcWindow, 
               0,0, 
               rcClient.right, rcClient.bottom, 
               hdcScreen, 
               0,0,
               GetSystemMetrics (SM_CXSCREEN),
               GetSystemMetrics (SM_CYSCREEN),
               SRCCOPY))
    {
        MessageBox(hWnd, L"StretchBlt has failed",L"Failed", MB_OK);
        goto done;
    }
    
    // Create a compatible bitmap from the Window DC
    hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
    
    if(!hbmScreen)
    {
        MessageBox(hWnd, L"CreateCompatibleBitmap Failed",L"Failed", MB_OK);
        goto done;
    }

    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC,hbmScreen);
    
    // Bit block transfer into our compatible memory DC.
    if(!BitBlt(hdcMemDC, 
               0,0, 
               rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
               hdcWindow, 
               0,0,
               SRCCOPY))
    {
        MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
        goto done;
    }

    // Get the BITMAP from the HBITMAP
    GetObject(hbmScreen,sizeof(BITMAP),&bmpScreen);
     
    BITMAPFILEHEADER   bmfHeader;    
    BITMAPINFOHEADER   bi;
     
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = bmpScreen.bmWidth;    
    bi.biHeight = bmpScreen.bmHeight;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;  
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
    char *lpbitmap = (char *)GlobalLock(hDIB);    

    // Gets the "bits" from the bitmap and copies them into a buffer 
    // which is pointed to by lpbitmap.
    GetDIBits(hdcWindow, hbmScreen, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
    HANDLE hFile = CreateFile(L"captureqwsx.bmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);   
    
    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
 
    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER); 
    
    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB; 
    
    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM   
 
    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
    
    //Unlock and Free the DIB from the heap
    GlobalUnlock(hDIB);    
    GlobalFree(hDIB);

    //Close the handle for the file that was created
    CloseHandle(hFile);
       
    //Clean up
done:
    DeleteObject(hbmScreen);
    ReleaseDC(hWnd, hdcMemDC);
    ReleaseDC(NULL,hdcScreen);
    ReleaseDC(hWnd,hdcWindow);

    return 0;
}
*/

#endif
	
#ifdef QB64_LINUX
#ifndef QB64_MACOSX
    //Code contributed by 'Chronokitsune'
    lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
    static Display *dpy;
    static int x, y;
    static unsigned int width, height;
    static XWindowAttributes attrs;
    static Window w;
    static XImage *img;
    static SDL_SysWMinfo syswminfo;
    static int32 setup=0;
    if (!setup)
    {
      setup=1;
      SDL_VERSION(&syswminfo.version);
      SDL_GetWMInfo (&syswminfo);
    }
    dpy = syswminfo.info.x11.display;
    syswminfo.info.x11.lock_func ();
    w = RootWindow(dpy, DefaultScreen(dpy));
    XGetWindowAttributes (dpy, // Display *display
                          w, // Window w
                          &attrs); //XWindowAttributes *attrs_return);
    x = attrs.x;
    y = attrs.y;
    width = attrs.width;
    height = attrs.height;
    w = attrs.root;
    img = XGetImage (dpy, w, x, y, width, height, AllPlanes, ZPixmap);
    if (img)
      {
        static int32 i,i2;
        i2=func__dest();
        i=func__newimage(width,height,32,1);
        sub__dest(i);
        if (img->bytes_per_line==width*4) memcpy(write_page->offset,img->data,width*height*4);//overflow safeguard                  
        sub__setalpha(255,NULL,NULL,NULL,0);
        sub__dest(i2);
        XDestroyImage (img);
        syswminfo.info.x11.unlock_func ();
        lock_mainloop=0; Sleep(1);//unlock
        return i;        
      }
    syswminfo.info.x11.unlock_func();
    lock_mainloop=0; Sleep(1);//unlock
    return -1;
#endif
#endif

#ifdef QB64_MACOSX
	system("screencapture -x -tbmp /tmp/qb64screencapture.bmp\0");
	return func__loadimage(qbs_new_txt("/tmp/qb64screencapture.bmp"),32,1);
#endif	

return -1;
}

void sub__screenclick(int32 x,int32 y){

#ifdef QB64_WINDOWS

static INPUT input;

ZeroMemory(&input,sizeof(INPUT));
input.type=INPUT_MOUSE;
input.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE;
 static HWND hwnd;
 hwnd=GetDesktopWindow();
 static RECT rect;
 GetWindowRect(hwnd,&rect);
 static double x2,y2,fx,fy;
 x2=rect.right-rect.left;
 y2=rect.bottom-rect.top;
 fx=x*(65535.0f/x2);
 fy=y*(65535.0f/y2);
 input.mi.dx=fx;
 input.mi.dy=fy;
SendInput(1,&input,sizeof(INPUT));

ZeroMemory(&input,sizeof(INPUT));
input.type=INPUT_MOUSE;
input.mi.dwFlags=MOUSEEVENTF_LEFTDOWN;
SendInput(1,&input,sizeof(INPUT));

ZeroMemory(&input,sizeof(INPUT));
input.type=INPUT_MOUSE;
input.mi.dwFlags=MOUSEEVENTF_LEFTUP;
SendInput(1,&input,sizeof(INPUT));

#endif

}

void sub__screenprint(qbs *txt){

#ifdef QB64_WINDOWS

static int32 i,s,x,vk,c;

static INPUT input;

/*VK reference:
http://msdn.microsoft.com/en-us/library/ms927178.aspx
*/

for (i=0;i<txt->len;i++){
c=txt->chr[i];
//custom characters
if (c==9){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_TAB; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT)); goto special_character;}
if (c==8){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_BACK; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT)); goto special_character;}
if (c==13){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_RETURN; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT)); goto special_character;}
//...

/*
CONTROL+{A-Z} 
The following 'x' letters cannot be simulated this way because they map to above functionality:
    ABCDEFGHIJKLMNOPQRSTUVWXYZ
    .......xx...x.............
Common/standard CTRL+? combinations for copying, pasting, undoing, cutting, etc. are available
*/
if ((c>=1)&&(c<=26)){
ZeroMemory(&input,sizeof(INPUT)); input.type=INPUT_KEYBOARD; input.ki.wVk=VK_CONTROL; SendInput(1,&input,sizeof(INPUT));
ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VkKeyScan(64+c)&255; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));
ZeroMemory(&input,sizeof(INPUT)); input.type=INPUT_KEYBOARD; input.ki.wVk=VK_CONTROL; input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));
goto special_character;
}

//custom extended characters
if (c==0){
if (i==(txt->len-1)) goto special_character;
i++;
c=txt->chr[i];
if (c==15){//SHIFT+TAB
ZeroMemory(&input,sizeof(INPUT)); input.type=INPUT_KEYBOARD; input.ki.wVk=VK_SHIFT; SendInput(1,&input,sizeof(INPUT));
ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_TAB; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));
ZeroMemory(&input,sizeof(INPUT)); input.type=INPUT_KEYBOARD; input.ki.wVk=VK_SHIFT; input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));
}
if (c==75){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_LEFT; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==77){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_RIGHT; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==72){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_UP; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==80){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_DOWN; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==82){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_INSERT; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==71){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_HOME; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==83){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_DELETE; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==79){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_END; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==81){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_NEXT; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
if (c==73){ZeroMemory(&input,sizeof(INPUT)); input.ki.wVk=VK_PRIOR; input.type=INPUT_KEYBOARD; SendInput(1,&input,sizeof(INPUT)); input.ki.dwFlags = KEYEVENTF_KEYUP; SendInput(1,&input,sizeof(INPUT));}
//...
//todo: F1-F12, shift/control/alt+above

goto special_character;
}

if ((c>126)||(c<32)) goto special_character;

x=VkKeyScan(txt->chr[i]);
vk=x&255;

s=(x>>8)&255;
//1 Either shift key is pressed. 
//2 Either CTRL key is pressed. 
//4 Either ALT key is pressed. 
if (s&1){
ZeroMemory(&input,sizeof(INPUT));
input.type=INPUT_KEYBOARD;
input.ki.wVk=VK_SHIFT;
SendInput(1,&input,sizeof(INPUT));
}

ZeroMemory(&input,sizeof(INPUT));
input.type=INPUT_KEYBOARD;
input.ki.wVk=vk;
SendInput(1,&input,sizeof(INPUT));

ZeroMemory(&input,sizeof(INPUT));
input.type=INPUT_KEYBOARD;
input.ki.wVk=vk;
input.ki.dwFlags = KEYEVENTF_KEYUP;
SendInput(1,&input,sizeof(INPUT));

if (s&1){
ZeroMemory(&input,sizeof(INPUT));
input.type=INPUT_KEYBOARD;
input.ki.wVk=VK_SHIFT;
input.ki.dwFlags = KEYEVENTF_KEYUP;
SendInput(1,&input,sizeof(INPUT));
}

special_character:;

}//i

#endif

}

void sub__printimage(int32 i){

#ifdef QB64_WINDOWS

static LPSTR szPrinterName=NULL;
DWORD dwNameLen;
HDC dc;
DOCINFO di;
uint32 w,h;
int32 x,y;
int32 i2;
BITMAPFILEHEADER bmfHeader;  
BITMAPINFOHEADER bi;
img_struct *s,*s2;

if (i>=0){
validatepage(i); s=&img[page[i]];
}else{
x=-i;
if (x>=nextimg){error(258); return;}
s=&img[x];
if (!s->valid){error(258); return;}
}

if (!szPrinterName) szPrinterName=(LPSTR)malloc(65536);
dwNameLen=65536;
GetDefaultPrinter(szPrinterName,&dwNameLen);
if((dc=CreateDC(TEXT("WINSPOOL"),szPrinterName,NULL,NULL))==NULL) goto failed;
ZeroMemory(&di,sizeof(DOCINFO));
di.cbSize=sizeof(DOCINFO);
di.lpszDocName=TEXT("Document");
if(StartDoc(dc,&di)<=0){DeleteDC(dc); goto failed;}
if(StartPage(dc)<=0){EndDoc(dc); DeleteDC(dc); goto failed;}

w=GetDeviceCaps(dc,HORZRES);
h=GetDeviceCaps(dc,VERTRES);

i2=func__newimage(w,h,32,1);
if (i2==-1){EndDoc(dc); DeleteDC(dc); goto failed;}
s2=&img[-i2];
sub__dontblend(i2,1);
sub__putimage(NULL,NULL,NULL,NULL,i,i2,NULL,NULL,NULL,NULL,8+32);

ZeroMemory(&bi,sizeof(BITMAPINFOHEADER));
bi.biSize = sizeof(BITMAPINFOHEADER);
bi.biWidth = w;
bi.biHeight = h;  
bi.biPlanes = 1;
bi.biBitCount = 32;
bi.biCompression = BI_RGB;
bi.biSizeImage = 0;  
bi.biXPelsPerMeter = 0;
bi.biYPelsPerMeter = 0;
bi.biClrUsed = 0;
bi.biClrImportant = 0;

for (y=0;y<h;y++){
SetDIBitsToDevice(dc,0,y,w,1,0,0,0,1,s2->offset32+(y*w),(BITMAPINFO*)&bi, DIB_RGB_COLORS);
}

sub__freeimage(i2,1);

if(EndPage(dc)<=0){EndDoc(dc); DeleteDC(dc); goto failed;}
if(EndDoc(dc)<=0){DeleteDC(dc); goto failed;}
DeleteDC(dc);
failed:;

#endif

}

void sub_files(qbs *str,int32 passed){
if (new_error) return;
static int32 i,i2,i3;
static qbs *strz=NULL; if (!strz) strz=qbs_new(0,0);

if (passed){
qbs_set(strz,qbs_add(str,qbs_new_txt_len("\0",1)));
}else{
qbs_set(strz,qbs_new_txt_len("\0",1));
}

#ifdef QB64_WINDOWS
 static WIN32_FIND_DATA fd;
 static HANDLE hFind;
 static qbs *strpath=NULL; if (!strpath) strpath=qbs_new(0,0);
 static qbs *strz2=NULL; if (!strz2) strz2=qbs_new(0,0);

 i=0;
 if (strz->len>=2){
 if (strz->chr[strz->len-2]==92) i=1;
 }else i=1;
 if (i){//add * (and new NULL term.)
 strz->chr[strz->len-1]=42;//"*"
 qbs_set(strz,qbs_add(strz,qbs_new_txt_len("\0",1))); 
 }

 qbs_set(strpath,strz);

 for(i=strpath->len;i>0;i--){
 if ((strpath->chr[i-1]==47)||(strpath->chr[i-1]==92)){strpath->len=i; break;}
 }//i
 if (i==0) strpath->len=0;//no path specified
 
 //print the current path
 //note: for QBASIC compatibility reasons it does not print the directory name of the files being displayed
 static uint8 curdir[4096];
 static uint8 curdir2[4096];
 i2=GetCurrentDirectory(4096,(char*)curdir);
 if (i2){
 i2=GetShortPathName((char*)curdir,(char*)curdir2,4096);
 if (i2){
 qbs_set(strz2,qbs_ucase(qbs_new_txt_len((char*)curdir2,i2)));
 qbs_print(strz2,1);
 }else{
 error(5); return;
 }
 }else{
 error(5); return;
 }

 hFind = FindFirstFile(fixdir(strz), &fd);
 if(hFind==INVALID_HANDLE_VALUE){error(53); return;}//file not found
 do{

 if (!fd.cAlternateFileName[0]){//no alternate filename exists
 qbs_set(strz2,qbs_ucase(qbs_new_txt_len(fd.cFileName,strlen(fd.cFileName))));
 }else{
 qbs_set(strz2,qbs_ucase(qbs_new_txt_len(fd.cAlternateFileName,strlen(fd.cAlternateFileName))));
 }

 if (strz2->len<12){//padding required
 qbs_set(strz2,qbs_add(strz2,func_space(12-strz2->len)));
 i2=0;
 for (i=0;i<12;i++){
 if (strz2->chr[i]==46){
 memmove(&strz2->chr[8],&strz2->chr[i],4);
 memset(&strz2->chr[i],32,8-i);
 break;
 }
 }//i
 }//padding

 //add "      " or "<DIR> "
 if (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){
 qbs_set(strz2,qbs_add(strz2,qbs_new_txt_len("<DIR> ",6)));
 }else{
 qbs_set(strz2,qbs_add(strz2,func_space(6)));
 }

 makefit(strz2);
 qbs_print(strz2,0);
 
 }while(FindNextFile(hFind,&fd));
FindClose(hFind);

static ULARGE_INTEGER FreeBytesAvailableToCaller;
static ULARGE_INTEGER TotalNumberOfBytes;
static ULARGE_INTEGER TotalNumberOfFreeBytes;
static int64 bytes;
static char *cp;
qbs_set(strpath,qbs_add(strpath,qbs_new_txt_len("\0",1)));
cp=(char*)strpath->chr;
if (strpath->len==1) cp=NULL;
if (GetDiskFreeSpaceEx(cp,&FreeBytesAvailableToCaller,&TotalNumberOfBytes,&TotalNumberOfFreeBytes)){
bytes=*(int64*)(void*)&FreeBytesAvailableToCaller;
}else{
bytes=0;
}
if (func_pos(NULL)>1){strz2->len=0; qbs_print(strz2,1);}//new line if necessary
qbs_set(strz2,qbs_add(qbs_str(bytes),qbs_new_txt_len(" Bytes free",11)));
qbs_print(strz2,1);

#endif
}

void mouseinput_flush(){
static int32 i;
static int int_x,int_y;
SDL_GetMouseState(&int_x,&int_y);
//create initial message
i=(last_mouse_message+1)&65535;
if (i==current_mouse_message) current_mouse_message=(current_mouse_message+1)&65535;//if buffer full, skip oldest message
mouse_messages[i].x=int_x;
mouse_messages[i].y=int_y;
mouse_messages[i].movementx=0;
mouse_messages[i].movementy=0;
mouse_messages[i].buttons=mouse_messages[last_mouse_message].buttons;
last_mouse_message=i;
//set current mouse message to new mouse message (skipping any buffered messages)
current_mouse_message=i;
}

int32 mouseinput_ignoremovement=1;





















#ifdef QB64_IME

static int message_loop(
	SDL_Event event,
	SDL_Surface *screen,
	SDL_Surface *back,
	int *inputedWidth,
	Uint16 *inputedString,
	TTF_Font *font);

static int GetUnicodeStringLength(Uint16 *unicodeString);
static Uint16 *CatUnicodeString(Uint16 *dest, Uint16 *src);
/*
static Uint16 *CopyUnicodeString(Uint16 *dest, Uint16 *src);
*/
static Uint16 *CopyNumUnicodeString(Uint16 *dest, Uint16 *src, int num);
static int DrawCursor(SDL_Surface *back, TTF_Font *font, int cursorPosition);
static int MessageChange(SDL_Surface *back, TTF_Font *font, int inputedWidth);


int GetUnicodeStringLength(Uint16 *unicodeString)
{
	int i;
	
	if (unicodeString == NULL) {
		return 0;
	}
	
	i = 0;
	while (unicodeString[i]) {
		i++;
	}
	
	return i;
}


/*
Uint16 *CopyUnicodeString(Uint16 *dest, Uint16 *src)
{
	int i;
	
	i = 0;
	while (src[i]) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0x0000;
	
	return dest;
}
*/


Uint16 *CopyNumUnicodeString(Uint16 *dest, Uint16 *src, int num)
{
	int i;
	
	i = 0;
	while (src[i] && i < num) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0x0000;
	
	return dest;
}


Uint16 *CatUnicodeString(Uint16 *dest, Uint16 *src)
{
	int destCount;
	int srcCount;
	
	destCount = 0;
	srcCount = 0;
	while (dest[destCount]) {
		destCount++;
	}
	
	while (src[srcCount]) {
		dest[destCount] = src[srcCount];
		destCount++;
		srcCount++;
	}
	dest[destCount] = 0x0000;
	
	return dest;
}


int DrawCursor(SDL_Surface *back, TTF_Font *font, int cursorPosition)
{
	SDL_Rect rect;
	///SDL_Color bg = {0x00, 0x00, 0x00};
	///SDL_Color cfg = {0x66, 0xFF, 0x66};

SDL_Color bg = {0xFF, 0xFF, 0xFF, 0xFF};
SDL_Color cfg = {0x00, 0x00, 0x00, 0x00};
	
	int cursorWidth;
	SDL_Surface *surface;
	
	rect.x = cursorPosition;
	rect.y = 0;
	rect.w = 640 - cursorPosition;
	rect.h = 100;
	surface = TTF_RenderUTF8_Shaded(font, "|", cfg, bg);
	SDL_BlitSurface(surface, NULL, back, &rect);
	cursorWidth = surface->w;
	SDL_FreeSurface(surface);
	
	return cursorWidth;
}


int DrawMessageBlock(
	SDL_Surface *back, TTF_Font *font, Uint16 *string,
	int drawStartWidth, SDL_Color fg, SDL_Color bg, int cursorPosition)
{
	SDL_Rect rect;
	SDL_Surface *surface;
	Uint16 *splitString;
	int needSize;
	int stringLength;
	int nowWidth;
	
	/* No cursor */
	if (cursorPosition == -1) {
		rect.x = drawStartWidth;
		rect.y = 0;
		rect.w = 640 - drawStartWidth;
		rect.h = 100;
		surface = TTF_RenderUNICODE_Shaded(font, string, fg, bg);
		
		SDL_BlitSurface(surface, NULL, back, &rect);
		nowWidth = surface->w;
		SDL_FreeSurface(surface);

ime_DrawMessageBlock_lastpixeloffset=drawStartWidth+nowWidth;
		return nowWidth;
	}
	
	/* Split string along cursorPosition */
	if (cursorPosition == 0) {
		nowWidth = 0;
	} else {
		nowWidth = 0;
		needSize = (cursorPosition + 1) * sizeof(Uint16);
		splitString = (Uint16*)calloc(needSize, 1);
		if (splitString == NULL) {
			return -1;
		}
		memcpy(splitString, string, cursorPosition * sizeof(Uint16));
		rect.x = drawStartWidth + nowWidth;
		rect.y = 0;
		rect.w = 640 - (drawStartWidth + nowWidth);
		rect.h = 100;
		surface = TTF_RenderUNICODE_Shaded(font, splitString, fg, bg);
		free(splitString);
		
		SDL_BlitSurface(surface, NULL, back, &rect);
		nowWidth += surface->w;
		SDL_FreeSurface(surface);
	}
	
	nowWidth += DrawCursor(back, font, drawStartWidth + nowWidth);
	
	stringLength =
		GetUnicodeStringLength(string) - cursorPosition;
	if (stringLength > 0) {
		needSize = (stringLength + 1) * sizeof(Uint16);
		splitString = (Uint16*)calloc(needSize, 1);
		if (splitString == NULL) {
			return -1;
		}
		splitString = string + cursorPosition;
		rect.x = drawStartWidth + nowWidth;
		rect.y = 0;
		rect.w = 640 - drawStartWidth + nowWidth;
		rect.h = 100;
		surface = TTF_RenderUNICODE_Shaded(font, splitString, fg, bg);
		SDL_BlitSurface(surface, NULL, back, &rect);
		nowWidth += surface->w;
		SDL_FreeSurface(surface);
	}


ime_DrawMessageBlock_lastpixeloffset=drawStartWidth+nowWidth;
	return nowWidth;
}


int MessageChange(SDL_Surface *back, TTF_Font *font, int inputedWidth)
{
	///SDL_Color fg = {0x66, 0x66, 0xFF};
	///SDL_Color bg = {0x00, 0x00, 0x00};

/*ref
    int DrawMessageBlock(
	SDL_Surface *back, TTF_Font *font, Uint16 *string,
	int drawStartWidth, SDL_Color fg, SDL_Color bg, int cursorPosition)
*/


	SDL_Color fg = {0x00, 0x00, 0x00, 0x00};
	SDL_Color bg = {0xFF, 0xFF, 0xFF, 0xFF};


	int compositionLastPosition;
	Uint16 *firstString;
	Uint16 *middleString;
	Uint16 *lastString;
	int firstStringCursorPosition;
	int middleStringCursorPosition;
	int lastStringCursorPosition;
	int needSize;
	int stringLength;
	int width;
	
	compositionLastPosition =
		InputMethod_GetCurrentCompositionPosition() +
		InputMethod_GetCurrentCompositionLength();

 stringLength =
		GetUnicodeStringLength(InputMethod_GetCurrentEditingString());
	
	if (InputMethod_GetCurrentCompositionLength() == 0) {
		DrawMessageBlock(
			back, font, InputMethod_GetCurrentEditingString(),
			inputedWidth, fg, bg, InputMethod_GetCurrentCursorPosition());



	} else if (
		InputMethod_GetCurrentCompositionPosition() == 0 &&
		compositionLastPosition >= stringLength
	) {
		DrawMessageBlock(
			back, font, InputMethod_GetCurrentEditingString(),
			inputedWidth, bg, fg, InputMethod_GetCurrentCursorPosition());

	} else if (InputMethod_GetCurrentCompositionPosition() == 0) {
		needSize =
			(InputMethod_GetCurrentCompositionLength() + 1)
			* sizeof(Uint16);
		firstString = (Uint16*)malloc(needSize);
		if (firstString == NULL) {
			return 0;
		}
		needSize =
			(stringLength - InputMethod_GetCurrentCompositionLength() + 1)
			* sizeof(Uint16);
		lastString = (Uint16*)malloc(needSize);
		if (lastString == NULL) {
			free(firstString);
			return 0;
		}
		CopyNumUnicodeString(
			firstString,
			InputMethod_GetCurrentEditingString(),
			InputMethod_GetCurrentCompositionLength());
		CopyNumUnicodeString(
			lastString,
			InputMethod_GetCurrentEditingString() +
			InputMethod_GetCurrentCompositionLength(),
			stringLength - InputMethod_GetCurrentCompositionLength());
		if (
			InputMethod_GetCurrentCursorPosition() <=
			InputMethod_GetCurrentCompositionLength()
		) {
			firstStringCursorPosition =
				InputMethod_GetCurrentCursorPosition();
			lastStringCursorPosition = -1;
		} else {
			firstStringCursorPosition = -1;
			lastStringCursorPosition =
				InputMethod_GetCurrentCursorPosition() -
				InputMethod_GetCurrentCompositionPosition();
		}
		width = DrawMessageBlock(
			back, font, firstString,
			inputedWidth, bg, fg, firstStringCursorPosition);
		DrawMessageBlock(
			back, font, lastString,
			inputedWidth + width, fg, bg, lastStringCursorPosition);
		free(firstString);
		free(lastString);
	} else if (compositionLastPosition >= stringLength) {
		needSize =
			(InputMethod_GetCurrentCompositionPosition() + 1)
			* sizeof(Uint16);
		firstString = (Uint16*)malloc(needSize);
		if (firstString == NULL) {
			return 0;
		}
		needSize =
			(InputMethod_GetCurrentCompositionLength() + 1)
			* sizeof(Uint16);
		lastString = (Uint16*)malloc(needSize);
		if (lastString == NULL) {
			free(firstString);
			return 0;
		}
		CopyNumUnicodeString(
			firstString,
			InputMethod_GetCurrentEditingString(),
			InputMethod_GetCurrentCompositionPosition());
		CopyNumUnicodeString(
			lastString,
			InputMethod_GetCurrentEditingString() +
			InputMethod_GetCurrentCompositionPosition(),
			InputMethod_GetCurrentCompositionLength());
		if (
			InputMethod_GetCurrentCursorPosition() <=
			InputMethod_GetCurrentCompositionPosition()
		) {
			firstStringCursorPosition =
				InputMethod_GetCurrentCursorPosition();
			lastStringCursorPosition = -1;
		} else {
			firstStringCursorPosition = -1;
			lastStringCursorPosition =
				InputMethod_GetCurrentCursorPosition() -
				InputMethod_GetCurrentCompositionPosition();
		}
		width = DrawMessageBlock(
			back, font, firstString,
			inputedWidth, fg, bg, firstStringCursorPosition);
		DrawMessageBlock(
			back, font, lastString,
			inputedWidth + width, bg, fg, lastStringCursorPosition);
		free(firstString);
		free(lastString);
	} else {
		needSize =
			(InputMethod_GetCurrentCompositionPosition() + 1)
			* sizeof(Uint16);
		firstString = (Uint16*)malloc(needSize);
		if (firstString == NULL) {
			return 0;
		}
		needSize =
			(InputMethod_GetCurrentCompositionLength() + 1)
			* sizeof(Uint16);
		middleString = (Uint16*)malloc(needSize);
		if (middleString == NULL) {
			free(firstString);
			return 0;
		}
		needSize =
			(stringLength - compositionLastPosition + 1)
			* sizeof(Uint16);
		lastString = (Uint16*)malloc(needSize);
		if (lastString == NULL) {
			free(firstString);
			free(middleString);
			return 0;
		}
		CopyNumUnicodeString(
			firstString,
			InputMethod_GetCurrentEditingString(),
			InputMethod_GetCurrentCompositionPosition());
		CopyNumUnicodeString(
			middleString,
			InputMethod_GetCurrentEditingString() +
			InputMethod_GetCurrentCompositionPosition(),
			InputMethod_GetCurrentCompositionLength());
		CopyNumUnicodeString(
			lastString,
			InputMethod_GetCurrentEditingString() +
			compositionLastPosition,
			stringLength - compositionLastPosition);
		if (
			InputMethod_GetCurrentCursorPosition() <=
			InputMethod_GetCurrentCompositionPosition()
		) {
			firstStringCursorPosition =
				InputMethod_GetCurrentCursorPosition();
			middleStringCursorPosition = -1;
			lastStringCursorPosition = -1;
		} else if (
			InputMethod_GetCurrentCursorPosition() <=
			InputMethod_GetCurrentCompositionPosition() +
			InputMethod_GetCurrentCompositionLength()
		) {
			firstStringCursorPosition = -1;
			middleStringCursorPosition =
				InputMethod_GetCurrentCursorPosition();
			lastStringCursorPosition = -1;
		} else {
			firstStringCursorPosition = -1;
			middleStringCursorPosition = -1;
			lastStringCursorPosition =
				InputMethod_GetCurrentCursorPosition() -
				(InputMethod_GetCurrentCompositionPosition() +
				 InputMethod_GetCurrentCompositionLength());
		}
		width = DrawMessageBlock(
			back, font, firstString,
			inputedWidth, fg, bg, firstStringCursorPosition);
		width += DrawMessageBlock(
			back, font, middleString,
			inputedWidth + width, bg, fg, middleStringCursorPosition);
		DrawMessageBlock(
			back, font, lastString,
			inputedWidth + width, fg, bg, lastStringCursorPosition);
		free(firstString);
		free(lastString);
	}
	
	return 0;
}
#endif


int32 func__keyhit(){
/*
//keyhit cyclic buffer
int64 keyhit[8192];
//    keyhit specific internal flags: (stored in high 32-bits)
//    &4294967296->numpad was used
int32 keyhit_nextfree=0;
int32 keyhit_next=0;
//note: if full, the oldest message is discarded to make way for the new message
*/
if (keyhit_next!=keyhit_nextfree){
static int32 x;
x=*(int32*)&keyhit[keyhit_next];
keyhit_next=(keyhit_next+1)&0x1FFF;
return x;
}
return 0;
}

int32 func__keydown(int32 x){
if (x<=0){error(5); return 0;}
if (keyheld(x)) return -1;
return 0;
}







static int32 field_failed=1;
static int32 field_fileno;
static int32 field_totalsize;
static int32 field_maxsize;

void field_new(int32 fileno){
field_failed=1;
if (new_error) return;
//validate file
static int32 i;
static gfs_file_struct *gfs;
i=fileno;
if (i<0){error(54); return;}//bad file mode (TCP/IP exclusion)
if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
gfs=&gfs_file[i];
if (gfs->type!=1){error(54); return;}//Bad file mode (note: must have RANDOM access)
//set global variables for field_add
field_fileno=fileno;
field_totalsize=0;
field_maxsize=gfs->record_length;
field_failed=0;
return;
}


void field_update(int32 fileno){

//validate file
static int32 i;
static gfs_file_struct *gfs;
i=fileno;
if (i<0){exit(7701);}//bad file mode (TCP/IP exclusion)
if (gfs_fileno_valid(i)!=1){exit(7702);}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
gfs=&gfs_file[i];
if (gfs->type!=1){exit(7703);}//Bad file mode (note: must have RANDOM access)

static qbs *str;
for (i=0;i<gfs->field_strings_n;i++){
str=gfs->field_strings[i];
if (!str) exit(7704);

//fix length if necessary
if (str->len!=str->field->size){
if (str->len>str->field->size) str->len=str->field->size; else qbs_set(str,qbs_new(str->field->size,1));
}

//copy data from field into string
memmove(str->chr,gfs->field_buffer+str->field->offset,str->field->size);

}//i

}

void lrset_field(qbs *str){
//validate file
static int32 i;
static gfs_file_struct *gfs;
i=str->field->fileno;
if (gfs_fileno_valid(i)!=1) goto remove;
i=gfs_fileno[i];//convert fileno to gfs index
gfs=&gfs_file[i];
if (gfs->type!=1) goto remove;
//check file ID
if (gfs->id!=str->field->fileid) goto remove;

//store in field buffer, padding with spaces or truncating data if necessary
if (str->field->size<=str->len){
memmove(gfs->field_buffer+str->field->offset,str->chr,str->field->size);
}else{
memmove(gfs->field_buffer+str->field->offset,str->chr,str->len);
memset(gfs->field_buffer+str->field->offset+str->len,32,str->field->size-str->len);
}

//update field strings for this file
field_update(str->field->fileno);

return;
remove:;
free(str->field);
str->field=NULL;
}

void field_free(qbs* str){

//validate file
static int32 i;
static gfs_file_struct *gfs;
i=str->field->fileno;
if (gfs_fileno_valid(i)!=1) goto remove;
i=gfs_fileno[i];//convert fileno to gfs index
gfs=&gfs_file[i];
if (gfs->type!=1) goto remove;
//check file ID
if (gfs->id!=str->field->fileid) goto remove;

//remove from string list
static qbs *str2;
for (i=0;i<gfs->field_strings_n;i++){
str2=gfs->field_strings[i];
if (str==str2){//match found
//truncate list
memmove(&(gfs->field_strings[i]),&(gfs->field_strings[i+1]),(gfs->field_strings_n-i-1)*ptrsz);
goto remove;
}
}//i

remove:
free(str->field);
str->field=NULL;
}

void field_add(qbs *str,int64 size){
if (field_failed) return;
if (new_error) goto fail;
if (size<0){error(5); goto fail;}
if ((field_totalsize+size)>field_maxsize){error(50); goto fail;}

//revalidate file
static int32 i;
static gfs_file_struct *gfs;
i=field_fileno;
//TCP/IP exclusion (reason: multi-reading from same TCP/IP position would require a more complex implementation)
if (i<0){error(54); goto fail;}//bad file mode
if (gfs_fileno_valid(i)!=1){error(52); goto fail;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
gfs=&gfs_file[i];
if (gfs->type!=1){error(54); goto fail;}//Bad file mode (note: must have RANDOM access)

//1) Remove str from any previous FIELD allocations
if (str->field) field_free(str);

//2) Setup qbs field info
str->field=(qbs_field*)malloc(sizeof(qbs_field));
str->field->fileno=field_fileno;
str->field->fileid=gfs->id;
str->field->size=size;
str->field->offset=field_totalsize;

//3) Add str to qbs list of gfs
if (!gfs->field_strings){
gfs->field_strings_n=1;
gfs->field_strings=(qbs**)malloc(ptrsz);
gfs->field_strings[0]=str;
}else{
gfs->field_strings_n++;
gfs->field_strings=(qbs**)realloc(gfs->field_strings,ptrsz*gfs->field_strings_n);
gfs->field_strings[gfs->field_strings_n-1]=str;
}

//4) Update field strings for this file
field_update(field_fileno);

field_totalsize+=size;
return;
fail:
field_failed=1;
return;
}

void field_get(int32 fileno,int64 offset,int32 passed){
if (new_error) return;

//validate file
static int32 i;
static gfs_file_struct *gfs;
i=fileno;
if (i<0){error(54); return;}//bad file mode (TCP/IP exclusion)
if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
gfs=&gfs_file[i];
if (gfs->type!=1){error(54); return;}//Bad file mode (note: must have RANDOM access)

if (!gfs->read){error(75); return;}//Path/file access error

if (passed){
 offset--;
 if (offset<0){error(63); return;}//Bad record number
 offset*=gfs->record_length;
}else{
 offset=-1;
}

static int32 e;
e=gfs_read(i,offset,gfs->field_buffer,gfs->record_length);
if (e){
if (e!=-10){//note: on eof, unread buffer area becomes NULL
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}
}

field_update(fileno);

}

void field_put(int32 fileno,int64 offset,int32 passed){
if (new_error) return;

//validate file
static int32 i;
static gfs_file_struct *gfs;
i=fileno;
if (i<0){error(54); return;}//bad file mode (TCP/IP exclusion)
if (gfs_fileno_valid(i)!=1){error(52); return;}//Bad file name or number
i=gfs_fileno[i];//convert fileno to gfs index
gfs=&gfs_file[i];
if (gfs->type!=1){error(54); return;}//Bad file mode (note: must have RANDOM access)

if (!gfs->write){error(75); return;}//Path/file access error

if (passed){
 offset--;
 if (offset<0){error(63); return;}//Bad record number
 offset*=gfs->record_length;
}else{
 offset=-1;
}

static int32 e;
e=gfs_write(i,offset,gfs->field_buffer,gfs->record_length);
if (e){
if (e==-2){error(258); return;}//invalid handle
if (e==-3){error(54); return;}//bad file mode
if (e==-4){error(5); return;}//illegal function call
if (e==-7){error(70); return;}//permission denied
error(75); return;//assume[-9]: path/file access error
}

}


void sub__mapunicode(int32 unicode_code, int32 ascii_code){
if (new_error) return;
if ((unicode_code<0)||(unicode_code>65535)){error(5); return;}
if ((ascii_code<0)||(ascii_code>255)){error(5); return;}
codepage437_to_unicode16[ascii_code]=unicode_code;
}

int32 func__mapunicode(int32 ascii_code){
if (new_error) return NULL;
if ((ascii_code<0)||(ascii_code>255)){error(5); return NULL;}
return (codepage437_to_unicode16[ascii_code]);
}



int32 addone(int32 x){return x+1;}//for testing purposes only


qbs *func__os(){
qbs *tqbs;
#ifdef QB64_WINDOWS
tqbs=qbs_new_txt("[WINDOWS][32BIT]");
#else
 #ifdef QB64_MACOSX
  #ifdef QB64_32
  tqbs=qbs_new_txt("[MACOSX][32BIT][LINUX]");
  #else
  tqbs=qbs_new_txt("[MACOSX][64BIT][LINUX]");
  #endif
 #else
  #ifdef QB64_32
  tqbs=qbs_new_txt("[LINUX][32BIT]");
  #else
  tqbs=qbs_new_txt("[LINUX][64BIT]");
  #endif 
 #endif
#endif
return tqbs;
}

int32 func__screenx(){
#ifdef QB64_WINDOWS
static SDL_SysWMinfo info;
static HWND win;
static RECT r;
SDL_VERSION(&info.version);
if (SDL_GetWMInfo(&info)){
 win =info.window;
 GetWindowRect(win,&r);
 return r.left;
}
#endif
return 0;
}

int32 func__screeny(){
#ifdef QB64_WINDOWS
static SDL_SysWMinfo info;
static HWND win;
static RECT r;
SDL_VERSION(&info.version);
if (SDL_GetWMInfo(&info)){
 win =info.window;
 GetWindowRect(win,&r);
 return r.top;
}
#endif
return 0;
}

void sub__screenmove(int32 x,int32 y,int32 passed){
if (new_error) return;
if (!passed) goto error;
if (passed==3) goto error;
if (full_screen) return;

#ifdef QB64_WINDOWS
static SDL_SysWMinfo info;
static HWND win,win2;
static RECT r,r2,r3;
static int32 x2,y2,x3,y3,x4,y4;
SDL_VERSION(&info.version);
if (SDL_GetWMInfo(&info)){
 win=info.window;
 Sleep(100);//IMPORTANT: Release CPU so SDL/OS can fully create window or following calls may not work
 if (passed&2){
  SetWindowPos(win,NULL,x,y,NULL,NULL,SWP_NOSIZE);
 }else{
  //_MIDDLE method
  win2=GetDesktopWindow();
  retry:
  GetWindowRect(win, &r);
  GetClientRect(win, &r3);
  //find the additional size of the window frame
  x2=r.right-r.left; y2=r.bottom-r.top;
  x2=x2-(r3.right-r3.left); y2=y2-(r3.bottom-r3.top);
  if ((x2<0)||(y2<0)) goto retry;//OS was in process of creating window
  //find the offsets to apply so that border & title bar are not included in the centering process
  x3=x2/2; y3=y2-x3;
  //find the size of our display page
  x=display_page->width; y=display_page->height;
  if (display_page->text){x*=fontwidth[display_page->font]; y*=fontheight[display_page->font];}
  GetWindowRect(win2, &r2);
  x=((r2.right-r2.left)/2)-(x/2)-x3;
  y=((r2.bottom-r2.top)/2)-(y/2)-y3;
  SetWindowPos(win,NULL,x,y,NULL,NULL,SWP_NOSIZE);
 }
}
#endif

return;
error:
error(5);
}

void key_update(){

if (key_display_redraw){
key_display_redraw=0;
if (!key_display) return;
}else{
if (key_display==key_display_state) return;
key_display_state=key_display;
}

//use display page 0
static int32 olddest;
olddest=func__dest();
sub__dest(0);
static img_struct *i;
i=write_page;

static int32 f,z,c,x2;

//locate bottom-left
//get current status
static int32 cx,cy,holding,col1,col2;
cx=i->cursor_x; cy=i->cursor_y; holding=i->holding_cursor;
col1=i->color; col2=i->background_color;
static int32 h,w;
//calculate height & width in characters
if (i->compatible_mode){
 h=i->height/fontheight[i->font];
 if (fontwidth[i->font]){
  w=i->width/fontwidth[i->font];
 }else{
  w=write_page->width;
 }
}else{
 h=i->height;
 w=i->width;
}
i->cursor_x=1; i->cursor_y=h; i->holding_cursor=0;

static qbs *str=NULL; if (!str) str=qbs_new(0,0);
static qbs *str2=NULL; if (!str2) str2=qbs_new(1,0);

//clear bottom row using background color
if (i->text){
 for (x2=1;x2<=i->width;x2++){
 str2->chr[0]=32; qbs_print(str2,0);
 }
 i->cursor_x=1; i->cursor_y=h; i->holding_cursor=0;
}else{
 fast_boxfill(0,(i->cursor_y-1)*fontheight[i->font],i->width-1,i->cursor_y*fontheight[i->font]-1,col2|0xFF000000);
}

if (!key_display) goto no_key;

static int32 item_x,limit_x,row_limit,leeway_x;
leeway_x=0;
if (i->compatible_mode){
 if (fontwidth[i->font]){
  item_x=w/12; row_limit=item_x*12;
  if (item_x<8){//cannot fit min. width
   item_x=8; row_limit=(w/8)*8;
   if (item_x>w){item_x=w; row_limit=w;}//can't even fit 1!
  }
 }else{
  leeway_x=fontheight[i->font];
  item_x=w/12; row_limit=item_x*12-leeway_x;
  x2=((float)fontheight[i->font])*0.5;//estimate the average character width (it's OK for this to be wrong)
  if (item_x<(x2*8+leeway_x)){//cannot fit min. width
   item_x=(x2*8+leeway_x); row_limit=(w/(x2*8+leeway_x))*(x2*8+leeway_x)-leeway_x;
   if (item_x>w){item_x=w; row_limit=w-leeway_x;}//can't even fit 1!
  }  
 }
}else{
 item_x=w/12; row_limit=item_x*12;
 if (item_x<8){//cannot fit min. width
  item_x=8; row_limit=(w/8)*8;
  if (item_x>w){item_x=w; row_limit=w;}//can't even fit 1!
 }
}

static int32 final_chr,row_final_chr;

row_final_chr=0;
for (f=1;f<=12;f++){
final_chr=0;
limit_x=f*item_x-leeway_x;//set new limit

//relocate
x2=((f-1)*item_x)+1;
if (x2>=row_limit){row_final_chr=1; goto done_f;}
i->cursor_x=x2;

//number string
if (fontwidth[i->font]){
qbs_set(str,qbs_ltrim(qbs_str(f)));
}else{
qbs_set(str,qbs_add(qbs_ltrim(qbs_str(f)),qbs_new_txt(")")));
}
for (z=0;z<str->len;z++){
if (i->cursor_x>=row_limit) row_final_chr=1;
if (i->cursor_x>limit_x) goto done_f;
if (i->cursor_x>=limit_x) final_chr=1;
str2->chr[0]=str->chr[z]; qbs_print(str2,0);
if (final_chr) goto done_f;
}

//text
static int32 fi;
fi=f; if (f>10) fi=f-11+30;
if (onkey[fi].text){
qbs_set(str,onkey[fi].text);
if (i->text){
 if (i->background_color){
  i->color=7; i->background_color=0;
 }else{
  i->color=0; i->background_color=7;
 }
}
}else{
str->len=0;
}
z=0;
while(i->cursor_x<limit_x){
static int32 c;

if (z>=str->len){
 if (!onkey[fi].text) goto done_f;
 c=32;
}else{
 c=str->chr[z++];
}

if (i->cursor_x>=row_limit) row_final_chr=1;
if (i->cursor_x>limit_x) goto done_f;
if (i->cursor_x>=limit_x) final_chr=1;
/*
7->14
8->254
9->26
10->60
11->127
12->22
13->27
28->16
29->17
30->24
31->25
 KEY LIST puts spaces instead of non-printables
 QBASIC's KEY LIST differs from QBX in this regard
 CHR$(13) is also turned into a space in KEY LIST, even if it is at the end
*/
if (c==7) c=14;
if (c==8) c=254;
if (c==9) c=26;
if (c==10) c=60;
if (c==11) c=127;
if (c==12) c=22;
if (c==13) c=27;
if (c==28) c=16;
if (c==29) c=17;
if (c==30) c=24;
if (c==31) c=25;
str2->chr[0]=c;
no_control_characters=1; qbs_print(str2,0); no_control_characters=0;
if (final_chr) goto done_f;
}

done_f:;
i->color=col1; i->background_color=col2;
if (row_final_chr) goto done_row;
}
done_row:;

//revert status
no_key:
i->cursor_x=cx; i->cursor_y=cy; i->holding_cursor=holding; i->color=col1; i->background_color=col2;
sub__dest(olddest);
}

void key_on(){
key_display=1; key_update();
}

void key_off(){
key_display=0; key_update();
}

void key_list(){
static img_struct *i;
i=write_page;
static int32 mono;
mono=1; if (!fontwidth[i->font]) if (func__printwidth(qbs_new_txt(" "),NULL,NULL)!=func__printwidth(qbs_new_txt(")"),NULL,NULL)) mono=0;
static int32 f,fi;
static qbs *str=NULL; if (!str) str=qbs_new(0,0);
for (f=1;f<=12;f++){

//F-number & spacer
if (fontwidth[i->font]){
 if (f<10){
  qbs_set(str,qbs_add(qbs_ltrim(qbs_str(f)),qbs_new_txt("  ")));
 }else{
  qbs_set(str,qbs_add(qbs_ltrim(qbs_str(f)),qbs_new_txt(" ")));
 }
}else{
 if ((f<10)&&(mono==1)){
  qbs_set(str,qbs_add(qbs_ltrim(qbs_str(f)),qbs_new_txt(")  ")));
 }else{
  qbs_set(str,qbs_add(qbs_ltrim(qbs_str(f)),qbs_new_txt(") ")));
 }
}
qbs_set(str,qbs_add(qbs_new_txt("F"),str));


//text
fi=f; if (f>10) fi=f-11+30;
if (onkey[fi].text){
 qbs_print(str,0);
/*
7->14
8->254
9->26
10->60
11->127
12->22
13->27
28->16
29->17
30->24
31->25
 KEY LIST puts spaces instead of non-printables
 QBASIC's KEY LIST differs from QBX in this regard
 CHR$(13) is also turned into a space in KEY LIST, even if it is at the end
*/
 str->len=1;
 static int32 x,c;
 for (x=0;x<onkey[fi].text->len;x++){
 c=onkey[fi].text->chr[x];
 if ((c>=7)&&(c<=13)) c=32;
 if ((c>=28)&&(c<=31)) c=32;
 str->chr[0]=c;
 qbs_print(str,0);
 }
 str->len=0; qbs_print(str,1);
}else{
 qbs_print(str,1);
}

}//f
}

void key_assign(int32 i,qbs *str){
if (new_error) return;
static int32 x,x2,i2;

if ( ((i>=1)&&(i<=10)) || (i==30) || (i==31) ){//F1-F10,F11,F12
if (str->len>15){error(5); return;}
if (!onkey[i].text) onkey[i].text=qbs_new(0,0);
qbs_set(onkey[i].text,str);
key_display_redraw=1; key_update();
return;
}//F1-F10,F11,F12

if ((i>=15)&&(i<=29)){//user defined key
if (str->len==0){
onkey[i].key_scancode=0;
}else{
x=str->chr[str->len-1];
x2=0; for (i2=0;i2<str->len-1;i2++) x2|=str->chr[i2];
onkey[i].key_scancode=x;
onkey[i].key_flags=x2;
}
return;
}//user defined key

error(5);
return;
}

void sub_paletteusing(void *element,int32 bits){
//note: bits is either 16(INTEGER) or 32(LONG)
if (new_error) return;
static byte_element_struct *ele; ele=(byte_element_struct*)element;
static int16 *i16; i16=(int16*)ele->offset;
static int32 *i32; i32=(int32*)ele->offset;
if (write_page->bits_per_pixel==32) goto error;
static int32 last_color,i,c;
last_color=write_page->mask;
if (ele->length<((bits/8)*(last_color+1))) goto error;
if ((write_page->compatible_mode==11)||(write_page->compatible_mode==12)||(write_page->compatible_mode==13)||(write_page->compatible_mode==256)){
 if (bits==16) goto error;//must be an array of type LONG in these modes
}
for (i=0;i<=last_color;i++){
if (bits==16){c=*i16; i16++;}else{c=*i32; i32++;}
if (c<-1) goto error;
if (c!=-1){
qbg_palette(i,c,1);
if (new_error) return;
}
}
return;
error:

error(5);
}

//#include <cassert>
#define CHUNKSIZE   1024
#define SAMPLE_RATE 22050.0
#define SAMP_BYTES  2
#define CHANNELS    2

static bool raw_playing = 0;
static char *raw_data;
static int raw_size = 0; //size of raw_data buffer (bytes)
static int raw_reader, raw_writer;
static bool raw_dontfree = 0;

//SDL_Mixer callback function
//must populate 'stream' with the next chunk of sound
static void raw_fill(void *dummy, Uint8 *stream, int len)
{
    //assert(len == CHUNKSIZE * CHANNELS);
    if (raw_reader < raw_writer && raw_reader + len > raw_writer) {
        //partial chunk, zero the rest
        memcpy(stream, raw_data + raw_reader, raw_writer - raw_reader);
        memset(stream + (raw_writer - raw_reader), 0, len - (raw_writer - raw_reader));
    } else {
        memcpy(stream, raw_data + raw_reader, len);
    }
    if ((raw_reader < raw_writer && raw_reader + len >= raw_writer) || (raw_writer == 0 && raw_reader + len == raw_size)) {
        //out of data
        Mix_HookMusic(NULL, NULL);
        raw_playing = 0;
        raw_reader = raw_writer = 0;
        if (raw_dontfree == 0) {
            free(raw_data);
            raw_size = 0;
        }
        return;
    }
    raw_reader += len;
    if (raw_reader >= raw_size) {
        raw_reader = 0;
    }
}

//double the space of the raw sound buffer
static void raw_grow()
{
    char *newptr;
    raw_dontfree = 1;
    SDL_UnlockAudio();
    newptr = (char *)malloc(raw_size * 2);
    if (!newptr){error(257); return;}
    memcpy(newptr, raw_data, raw_size);
    memcpy(newptr + raw_size, raw_data, raw_size);
    SDL_LockAudio();
    if (raw_writer <= raw_reader) {
        raw_writer += raw_size;
    }
    free(raw_data);
    raw_data = newptr;
    raw_size *= 2;
    raw_dontfree = 0;
}

//returns the amount of sound data queued, in bytes
//assumes that raw_size is a power of 2!
static inline int raw_bytes()
{
    return (raw_writer - raw_reader) & (raw_size - 1);
}

//queue a sample
void sub__sndraw(double left,double right,int32 passed)
{

    if (!raw_size) {
        sndsetup();
        raw_size = CHUNKSIZE*SAMP_BYTES*CHANNELS * 8;
        raw_data = (char *) malloc(raw_size);
        if (!raw_data){error(257); return;}
        raw_writer = raw_reader = 0;
    }

    static double sample;
    static int16 clipped;

    sample = left;
    if (sample >= 1.0)
        clipped = 32767;
    else if (sample <= -1.0)
        clipped = -32768;
    else
        clipped = floor(sample * 32767.99999);

    //left
    *(Sint16 *)&raw_data[raw_writer] = clipped;
    raw_writer += SAMP_BYTES;

    if (passed){
		sample = right;
		if (sample >= 1.0)
	        clipped = 32767;
	    else if (sample <= -1.0)
			clipped = -32768;
		else
	        clipped = floor(sample * 32767.99999);
	}

    //right
    *(Sint16 *)&raw_data[raw_writer] = clipped;
    raw_writer += SAMP_BYTES;

    if (raw_writer % (CHUNKSIZE * CHANNELS) == 0) {
        SDL_LockAudio();
        if (raw_writer >= raw_size)
            raw_writer = 0;
        if (raw_bytes() == 0)
            raw_grow();

        //ensure a reasonable amount is queued before commencing
        if (!raw_playing && raw_bytes() >= CHUNKSIZE * SAMP_BYTES * CHANNELS * 4) {
            raw_playing = 1;
            Mix_HookMusic(raw_fill, NULL);
        }
        SDL_UnlockAudio();
    }
}

//returns the length of sound queued, in seconds
double func__sndrawlen()
{
    int bytes;
    //This trick ensures that programs queue enough sound to trigger play
    if (!raw_playing)
        return 0;
    SDL_LockAudio();
    bytes = raw_bytes();
    SDL_UnlockAudio();
    return bytes / (SAMP_BYTES * CHANNELS * SAMPLE_RATE);
}

int32 func__sndrate(){
return 22050;//*revise
}


void sub__maptriangle(float sx1,float sy1,float sx2,float sy2,float sx3,float sy3,long si,long dx1,long dy1,long dx2,long dy2,long dx3,long dy3,long di,long passed){
//"[{_SEAMLESS}](?,?)-(?,?)-(?,?)[,?]{TO}(?,?)-(?,?)-(?,?)[,?]"
//   &1                           &2                       &4

static int32 dwidth,dheight,swidth,sheight,swidth2,sheight2;
static int32 lhs,rhs,lhs1,lhs2,top,bottom,temp,flats,flatg,final,tile,no_edge_overlap;
flats=0; final=0; tile=0; no_edge_overlap=0;
static int32 v,i,x,x1,x2,y,y1,y2,z,h,ti,lhsi,rhsi,d; 
static int32 g1x,g2x,g1tx,g2tx,g1ty,g2ty,g1xi,g2xi,g1txi,g2txi,g1tyi,g2tyi,tx,ty,txi,tyi,roff,loff;
static int64 i64;
static img_struct *src,*dst;
static uint8* pixel_offset;
static uint32* pixel_offset32;
static uint8* dst_offset;
static uint32* dst_offset32;
static uint8* src_offset;
static uint32* src_offset32;
static uint32 col,destcol,transparent_color;
static uint8* cp;

//get/validate src/dst images
if (passed&2){
 if (si>=0){//validate si
 validatepage(si); si=page[si];
 }else{
 si=-si; if (si>=nextimg){error(258); return;} if (!img[si].valid){error(258); return;}
 }
 src=&img[si];
}else{
 src=read_page;
}
if (passed&4){
 if (di>=0){//validate di
 validatepage(di); di=page[di];
 }else{
 di=-di; if (di>=nextimg){error(258); return;} if (!img[di].valid){error(258); return;}
 }
 dst=&img[di];
}else{
 dst=write_page;
}
if (src->text||dst->text){error(5);return;}
if (src->bytes_per_pixel!=dst->bytes_per_pixel){error(5);return;}

if (passed&1) no_edge_overlap=1;

dwidth=dst->width; dheight=dst->height;
swidth=src->width; sheight=src->height;
swidth2 = swidth<<16; sheight2 = sheight<<16;

struct PointType{
	int32 x;
	int32 y;
	int32 tx;
	int32 ty;
};
static PointType p[4],*p1,*p2,*tp,*tempp;
struct GradientType{
    int32 x;
    int32 xi;
    int32 tx;
    int32 ty;
    int32 txi;
    int32 tyi;
    int32 y1;
    int32 y2;
    //----
    PointType *p1;
    PointType *p2; //needed for clipping above screen
};
static GradientType g[4],*tg,*g1,*g2,*g3,*tempg;
memset(&g,0,sizeof(GradientType)*4);

/*
'Reference:
'Fixed point division: a/b -> a*65536/b (using intermediate _INTEGER64)
*/

/* debugging method
ofstream f;
char fn[] = "c:\\qb64\\20c.txt";
f.open(fn, ios::app);
f<<"\n";
f<<variablename;
f<<"\n";
f.close();
*/

static int32 limit,limit2,nlimit,nlimit2;

//----------------------------------------------------------------------------------------------------------------------------------------------------


limit = 16383;
limit2 = (limit << 16) + 32678;
nlimit = -limit;
nlimit2 = -limit2;

//convert texture coords to fixed-point & adjust so 0,0 effectively becomes 0.5,0.5 (ie. 32768,32768)
v = ((int32)(sx1 * 65536.0)) + 32768;
if(v < 16 | v >= swidth2 - 16) tile = 1;
if(v < nlimit2 | v > limit2) {error(5); return;}
p[1].tx = v;
v = ((int32)(sx2 * 65536.0)) + 32768;
if(v < 16 | v >= swidth2 - 16) tile = 1;
if(v < nlimit2 | v > limit2) {error(5); return;}
p[2].tx = v;
v = ((int32)(sx3 * 65536.0)) + 32768;
if(v < 16 | v >= swidth2 - 16) tile = 1;
if(v < nlimit2 | v > limit2) {error(5); return;}
p[3].tx = v;
v = ((int32)(sy1 * 65536.0)) + 32768;
if(v < 16 | v >= sheight2 - 16) tile = 1;
if(v < nlimit2 | v > limit2) {error(5); return;}
p[1].ty = v;
v = ((int32)(sy2 * 65536.0)) + 32768;
if(v < 16 | v >= sheight2 - 16) tile = 1;
if(v < nlimit2 | v > limit2) {error(5); return;}
p[2].ty = v;
v = ((int32)(sy3 * 65536.0)) + 32768;
if(v < 0 | v >= sheight2 - 16) tile = 1;
if(v < nlimit2 | v > limit2) {error(5); return;}
p[3].ty = v;

if(tile){
    //shifting to positive range is required for tiling | mod on negative coords will fail
    //shifting may also alleviate the need for tiling if(shifted coords fall within textures normal range
    //does texture extend beyond surface dimensions?
    lhs = 2147483647;
    rhs = -2147483648;
    top = 2147483647;
    bottom = -2147483648;
    for(i=1;i<=3;i++){
        tp=&p[i];
        y = tp->ty;
        if(y > bottom) bottom = y;
        if(y < top) top = y;
        x = tp->tx;
        if(x > rhs) rhs = x;
        if(x < lhs) lhs = x;
    }
    z = 0;
    if(lhs < 0){
        //shift texture coords right
        v = ((lhs + 1) / -swidth2 + 1) * swidth2; //offset to move by
        for(i=1;i<=3;i++){
            tp=&p[i];
            tp->tx = tp->tx + v;
            z = 1;
        }
    }else{
        if(lhs >= swidth2){
            //shift texture coords left
            z = 1;
            v = (lhs / swidth2) * swidth2; //offset to move by
            for(i=1;i<=3;i++){
                tp=&p[i];
                tp->tx = tp->tx - v;
                z = 1;
            }
        }
    }
    if(top < 0){
        //shift texture coords down
        v = ((top + 1) / -sheight2 + 1) * sheight2; //offset to move by
        for(i=1;i<=3;i++){
            tp=&p[i];
            tp->ty = tp->ty + v;
            z = 1;
        }
    }else{
        if(top >= swidth2){
            //shift texture coords up
            v = (top / sheight2) * sheight2; //offset to move by
            for(i=1;i<=3;i++){
                tp=&p[i];
                tp->ty = tp->ty - v;
                z = 1;
            }
            z = 1;
        }
    }
    if(z){
        //reassess need for tiling
        z = 0;
        for(i=1;i<=3;i++){
            tp=&p[i];
            v = tp->tx; if(v < 16 | v >= swidth2 - 16){
                z = 1; break;
            }
            v = tp->ty; if(v < 16 | v >= sheight2 - 16){
                z = 1; break;
            }
        }
        if(z == 0) tile = 0; //remove tiling flag, this will greatly improve blit speed
    }
}

//validate dest points
if(dx1 < nlimit | dx1 > limit) {error(5); return;}
if(dx2 < nlimit | dx2 > limit) {error(5); return;}
if(dx3 < nlimit | dx3 > limit) {error(5); return;}
if(dy1 < nlimit | dy1 > limit) {error(5); return;}
if(dy2 < nlimit | dy2 > limit) {error(5); return;}
if(dy3 < nlimit | dy3 > limit) {error(5); return;}

//setup dest points
p[1].x = (dx1 << 16) + 32768;
p[2].x = (dx2 << 16) + 32768;
p[3].x = (dx3 << 16) + 32768;
p[1].y = dy1;
p[2].y = dy2;
p[3].y = dy3;

//get dest extents
lhs = 2147483647;
rhs = -2147483648;
top = 2147483647;
bottom = -2147483648;
for(i=1;i<=3;i++){
    tp=&p[i];
    y = tp->y;
    if(y > bottom) bottom = y;
    if(y < top) top = y;
    if(tp->x < 0) x = (tp->x - 65535) / 65536;else x = tp->x / 65536;
    if(x > rhs) rhs = x;
    if(x < lhs) lhs = x;
}

if(bottom < 0 | top >= dheight | rhs < 0 | lhs >= dwidth) return; //clip entire triangle

for(i=1;i<=3;i++){
    tg=&g[i]; p1=&p[i]; if (i==3) p2=&p[1]; else p2=&p[i+1];

    if(p1->y > p2->y){
        tempp = p1; p1 = p2; p2 = tempp;
    }
    tg->tx = p1->tx; tg->ty = p1->ty; //starting co-ordinates
    tg->x = p1->x;
    tg->y1 = p1->y; tg->y2 = p2->y; //top & bottom
    h = tg->y2 - tg->y1;
    if(h == 0){
        flats = flats + 1; //number of flat(horizontal) gradients
        flatg = i; //last detected flat gradient
    }else{
        tg->xi = (p2->x - p1->x) / h;
        tg->txi = (p2->tx - p1->tx) / h;
        tg->tyi = (p2->ty - p1->ty) / h;
    }
    tg->p2 = p2;
    tg->p1 = p1;
}

g1=&g[1]; g2=&g[2]; g3=&g[3];
if(flats == 0){
    //select 2 top-most gradients
    if(g3->y1 < g1->y1){
        tempg = g1; g1 = g3; g3 = tempg;
    }
    if(g3->y1 < g2->y1){
        tempg = g2; g2 = g3; g3 = tempg;
    }
}else{
    if(flats == 1){
        //select the only 2 non-flat gradients available
        if(flatg == 1){
            tempg = g1; g1 = g3; g3 = tempg;
        }
        if(flatg == 2){
            tempg = g2; g2 = g3; g3 = tempg;
        }
        final = 1; //don't check for continuation
    }else{
        //3 flats
        //create a row from the leftmost to rightmost point
        //find leftmost-rightmost points
        lhs = 2147483647;
        rhs = -2147483648;
        for(ti=1;ti<=3;ti++){
            x = p[ti].x;
            if(x <= lhs){
                lhs = x; lhsi = ti;
            }
            if(x >= rhs){
                rhs = x; rhsi = ti;
            }
        }
        //build (dummy) gradients
        g[1].x = lhs;
        g[2].x = rhs;
        g[1].tx = p[lhsi].tx; g[1].ty = p[lhsi].ty;
        g[2].tx = p[rhsi].tx; g[2].ty = p[rhsi].ty;
        final = 1; //don't check for continuation
    }
}

y1 = g1->y1; if(g1->y2 > g2->y2) y2 = g2->y2;else y2 = g1->y2;

//compare the mid-point x-coords of both runs to determine which is on the left & right
y = y2 - y1;
lhs1 = g1->x + (g1->xi * y) / 2;
lhs2 = g2->x + (g2->xi * y) / 2;
if(lhs1 > lhs2){
    tempg = g1; g1 = g2; g2 = tempg;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

if (src->bytes_per_pixel==4){
dst_offset32=dst->offset32;
src_offset32=src->offset32;
 if (src->alpha_disabled||dst->alpha_disabled){
  if (tile){
   #include "mtri1t.cpp"
  }
  #include "mtri1.cpp"
 }else{
  if (tile){
   #include "mtri2t.cpp"
  }
  #include "mtri2.cpp"
 }
}//4

//assume 1 byte per pixel
dst_offset=dst->offset;
src_offset=src->offset;
transparent_color=src->transparent_color;
 if (transparent_color==-1){
  if (tile){
   #include "mtri3t.cpp"
  }
  #include "mtri3.cpp"
 }else{
  if (tile){
   #include "mtri4t.cpp"
  }
  #include "mtri4.cpp"
 }//1

error(5); return;
}//sub__maptriangle

extern int32 func__devices();

int32 func_stick(int32 i,int32 axis_group,int32 passed){
//note: range: 1-254 (127=neutral), top-left to bottom-right positive
//             128 returned for unattached devices
//QB64 extension: 'i' allows for joystick selection 0,1->JoyA, 2,3->JoyB, 4,5->JoyC, etc
//                'axis_group' selects the pair of axes to read from, 1 is the default
if (device_last==0) func__devices();//init device interface (if not already setup)
if (passed){
if (axis_group<1||axis_group>65535){error(5); return 0;}
}else{
axis_group=1;
}
if (i<0||i>65535){error(5); return 0;}
static int32 di,axis,i2,v;
static device_struct *d;
static float f;
axis=(i&1)+(axis_group-1)*2;
i=i>>1;
i2=0;
for(di=1;di<=device_last;di++){
d=&devices[di];
if (d->type==1){
if (i==i2){
if (axis<d->lastaxis){
f=*(float*)(d->events+(d->event_size*(d->queued_events-1))+d->lastbutton+axis*4);
if (f>-0.01&&f<=0.01) f=0;
v=qbr_float_to_long(f*127.0)+127;
if (v>254) v=254;
if (v<1) v=1;
return v;
}//axis valid
}
i2++;
}//type==1
}//di
return 128;
}

int32 func_strig(int32 i,int32 controller,int32 passed){
//note: returns 0 or -1(true)
//QB64 extension: 'i' refers to a button (b1,b1,b1,b1,b2,b2,b2,b2,b3,b3,b3,b3,b4,...)
//                'controller' overrides the controller implied by 'i', 1 is the default
if (device_last==0) func__devices();//init device interface (if not already setup)
if (i<0||i>65535){error(5); return 0;}
if (passed){
if (controller<1||controller>65535){error(5); return 0;}
}else{
controller=1; if (i&2){controller=2; i-=2;}
}
static int32 di,button,method,c,v;
static device_struct *d;
button=(i>>2)+1;
method=(i&1)+1;//1=if pressed since last call, 2=currently down
c=1;
for(di=1;di<=device_last;di++){
d=&devices[di];
if (d->type==1){
if (c==controller){
if (button<=d->lastbutton){//button exists
if (method==1){
//method 1: pressed since last call
if (button>0&&button<=256){
if (d->STRIG_button_pressed[button-1]){
d->STRIG_button_pressed[button-1]=0;
return -1;
}
}
return 0;
}else{
//method 2: currently down
v=*(d->events+(d->event_size*(d->queued_events-1))+(button-1));
if (v) return -1; else return 0;
}
}//button exists
}//c==controller
c++;
}//type==1
}//di
return 0;
}

int32 func__fileexists(qbs* file){
if (new_error) return 0;
static qbs *strz=NULL;
if (!strz) strz=qbs_new(0,0);
qbs_set(strz,qbs_add(file,qbs_new_txt_len("\0",1)));
#ifdef QB64_WINDOWS
 static int32 x;
 x=GetFileAttributes(fixdir(strz));
 if (x==INVALID_FILE_ATTRIBUTES) return 0;
 if (x&FILE_ATTRIBUTE_DIRECTORY) return 0;
 return -1;
#endif
#ifdef QB64_LINUX
 struct stat sb;
 if (stat(fixdir(strz),&sb) == 0 && S_ISREG(sb.st_mode)) return -1;
 return 0;
#endif
//generic method (not currently used)
static ifstream fh;
fh.open(fixdir(strz),ios::binary|ios::in);
if (fh.is_open()==NULL){fh.clear(ios::goodbit); return 0;}
fh.clear(ios::goodbit);
fh.close();
return -1;
}

int32 func__direxists(qbs* file){
if (new_error) return 0;
static qbs *strz=NULL;
if (!strz) strz=qbs_new(0,0);
qbs_set(strz,qbs_add(file,qbs_new_txt_len("\0",1)));
#ifdef QB64_WINDOWS
 static int32 x;
 x=GetFileAttributes(fixdir(strz));
 if (x==INVALID_FILE_ATTRIBUTES) return 0;
 if (x&FILE_ATTRIBUTE_DIRECTORY) return -1;
 return 0;
#endif
#ifdef QB64_LINUX
 struct stat sb;
 if (stat(fixdir(strz),&sb) == 0 && S_ISDIR(sb.st_mode)) return -1;
 return 0;
#endif
return 0;//default response
}

int main( int argc, char* argv[] )
{

static int32 i,i2,i3,i4;
static uint8 c,c2,c3,c4;
static int32 x,x2,x3,x4;
static int32 y,y2,y3,y4;
static int32 z,z2,z3,z4;
static float f,f2,f3,f4;
static uint8 *cp,*cp2,*cp3,*cp4;

onkey[1].keycode=59<<8;//F1-F10
onkey[2].keycode=60<<8;
onkey[3].keycode=61<<8;
onkey[4].keycode=62<<8;
onkey[5].keycode=63<<8;
onkey[6].keycode=64<<8;
onkey[7].keycode=65<<8;
onkey[8].keycode=66<<8;
onkey[9].keycode=67<<8;
onkey[10].keycode=68<<8;
onkey[11].keycode=72<<8;//up,left,right,down
onkey[11].keycode_alternate=VK+SDLK_KP8;
onkey[12].keycode=75<<8;
onkey[12].keycode_alternate=VK+SDLK_KP4;
onkey[13].keycode=77<<8;
onkey[13].keycode_alternate=VK+SDLK_KP6;
onkey[14].keycode=80<<8;
onkey[14].keycode_alternate=VK+SDLK_KP2;
onkey[30].keycode=133<<8;//F11,F12
onkey[31].keycode=134<<8;

ontimer[0].allocated=1;
ontimer[0].id=0;
ontimer[0].state=0;
ontimer[0].active=0;

//switch to directory of this EXE file
#ifdef QB64_WINDOWS
#ifndef QB64_MICROSOFT
static char *exepath=(char*)malloc(65536);
GetModuleFileName(NULL,exepath,65536);
i=strlen(exepath);
for (i2=i-1;i2>=0;i2--){
x=exepath[i2];
if ((x==92)||(x==47)||(x==58)){
if (x==58) exepath[i2+1]=0; else exepath[i2]=0;
break;
}
}
chdir(exepath);
#endif
#endif


{
/* For bounds check on numeric ENVIRON$ */
char **p = envp;
while (*p++);
environ_count = p - envp;
}

memset(font,0,sizeof(font));

fontwidth[8]=8; fontwidth[14]=8; fontwidth[16]=8;
fontheight[8]=8; fontheight[14]=14; fontheight[16]=16;
fontflags[8]=16; fontflags[14]=16; fontflags[16]=16;//monospace flag
fontwidth[8+1]=8*2; fontwidth[14+1]=8*2; fontwidth[16+1]=8*2;
fontheight[8+1]=8; fontheight[14+1]=14; fontheight[16+1]=16;
fontflags[8+1]=16; fontflags[14+1]=16; fontflags[16+1]=16;//monospace flag

memset(img,0,IMG_BUFFERSIZE*sizeof(img_struct));
x=newimg();//reserve index 0
img[x].valid=0;
x=newimg();//reserve index 1
img[x].valid=0;








memset(&cpu,0,sizeof(cpu_struct));

//uint8 *asmcodep=(uint8*)&asmcode[0];
//memcpy(&cmem[0],asmcodep,sizeof(asmcode));
reg8[0]=&cpu.al;
reg8[1]=&cpu.cl;
reg8[2]=&cpu.dl;
reg8[3]=&cpu.bl;
reg8[4]=&cpu.ah;
reg8[5]=&cpu.ch;
reg8[6]=&cpu.dh;
reg8[7]=&cpu.bh;

reg16[0]=&cpu.ax;
reg16[1]=&cpu.cx;
reg16[2]=&cpu.dx;
reg16[3]=&cpu.bx;
reg16[4]=&cpu.sp;
reg16[5]=&cpu.bp;
reg16[6]=&cpu.si;
reg16[7]=&cpu.di;

reg32[0]=&cpu.eax;
reg32[1]=&cpu.ecx;
reg32[2]=&cpu.edx;
reg32[3]=&cpu.ebx;
reg32[4]=&cpu.esp;
reg32[5]=&cpu.ebp;
reg32[6]=&cpu.esi;
reg32[7]=&cpu.edi;

segreg[0]=&cpu.es;
segreg[1]=&cpu.cs;
segreg[2]=&cpu.ss;
segreg[3]=&cpu.ds;
segreg[4]=&cpu.fs;
segreg[5]=&cpu.gs;

//WINDOWS SPECIFIC CONTENT
FreeConsole();
//END WINDOWS SPECIFIC CONTENT




for (i=0;i<32;i++) sub_file_print_spaces[i]=32;


port60h_event[0]=128+1;//simulate release of ESC


mem_static_size=1048576;//1MEG
mem_static=(uint8*)malloc(mem_static_size);
mem_static_pointer=mem_static;
mem_static_limit=mem_static+mem_static_size;

memset(&cmem[0],0,sizeof(cmem));










memset(&keyon[0],0,sizeof(keyon));

dblock=(ptrszint)&cmem+1280;//0:500h

//define "nothing"
cmem_sp-=8; nothingvalue=(uint64*)(dblock+cmem_sp);
*nothingvalue=0;
nothingstring=qbs_new_cmem(0,0);
singlespace=qbs_new_cmem(1,0);
singlespace->chr[0]=32;

unknown_opcode_mess=qbs_new(0,0);
qbs_set(unknown_opcode_mess,qbs_new_txt_len("Unknown Opcode (  )\0",20));

i=argc;
if (i>1){
//calculate required size of COMMAND$ string
i2=0;
for (i=1;i<argc;i++){
i2+=strlen(argv[i]);
if (i!=1) i2++;//for a space
}
//create COMMAND$ string
func_command_str=qbs_new(i2,0);
//build COMMAND$ string
i3=0;
for (i=1;i<argc;i++){
if (i!=1){func_command_str->chr[i3]=32; i3++;}
memcpy(&func_command_str->chr[i3],argv[i],strlen(argv[i])); i3+=strlen(argv[i]);
}
}else{
func_command_str=qbs_new(0,0);
}


//init SDL
if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_CDROM|SDL_INIT_TIMER) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(100);
    }
atexit(SDL_Quit);


//struct tm:
//        int tm_sec;     /* seconds after the minute - [0,59] */
//        int tm_min;     /* minutes after the hour - [0,59] */
//        int tm_hour;    /* hours since midnight - [0,23] */
//        int tm_mday;    /* day of the month - [1,31] */
//        int tm_mon;     /* months since January - [0,11] */
//        int tm_year;    /* years since 1900 */
//        int tm_wday;    /* days since Sunday - [0,6] */
//        int tm_yday;    /* days since January 1 - [0,365] */
//        int tm_isdst;   /* daylight savings time flag */
tm *qb64_tm;
time_t qb64_tm_val;
time_t qb64_tm_val_old;
//call both timing routines as close as possible to each other to maximize accuracy
//wait for second "hand" to "tick over"/move
time(&qb64_tm_val_old);
//note: time() returns the time as seconds elapsed since midnight, January 1, 1970, or -1 in the case of an error. 
if (qb64_tm_val_old!=-1){
	do{
	time(&qb64_tm_val);
	}while(qb64_tm_val==qb64_tm_val_old);
}else{
	qb64_tm_val=0;//time unknown! (set to midnight, January 1, 1970)
}
sdl_firsttimervalue=SDL_GetTicks();
//calculate localtime as milliseconds past midnight
qb64_tm=localtime(&qb64_tm_val);
/* re: localtime()
Return a pointer to the structure result, or NULL if the date passed to the function is:
Before midnight, January 1, 1970.
After 03:14:07, January 19, 2038, UTC (using _time32 and time32_t).
After 23:59:59, December 31, 3000, UTC (using _time64 and __time64_t).
*/
if (qb64_tm){
	qb64_firsttimervalue=qb64_tm->tm_hour*3600+qb64_tm->tm_min*60+qb64_tm->tm_sec;
	qb64_firsttimervalue*=1000;
}else{
	qb64_firsttimervalue=0;//time unknown! (set to midnight)
}
/* Used as follows for calculating TIMER value:
x=SDL_GetTicks();
x-=sdl_firsttimervalue;
x+=qb64_firsttimervalue;
*/

//init truetype .ttf/.fon font library
if (TTF_Init()==-1) exit(7000);
atexit(TTF_Quit);

#ifdef QB64_WINDOWS 
//for caps lock, use the state of the lock (=1)
//for held keys check against (=-127)
if (GetKeyState(VK_SCROLL)&1) keyheld_add(QBK+QBK_SCROLL_LOCK_MODE);
if (GetKeyState(VK_SCROLL)<0){bindkey=SDLK_SCROLLOCK; keydown_vk(VK+SDLK_SCROLLOCK);}
if (GetKeyState(VK_LSHIFT)<0){bindkey=SDLK_LSHIFT; keydown_vk(VK+SDLK_LSHIFT);}
if (GetKeyState(VK_RSHIFT)<0){bindkey=SDLK_RSHIFT; keydown_vk(VK+SDLK_RSHIFT);}
if (GetKeyState(VK_LCONTROL)<0){bindkey=SDLK_LCTRL; keydown_vk(VK+SDLK_LCTRL);}
if (GetKeyState(VK_RCONTROL)<0){bindkey=SDLK_RCTRL; keydown_vk(VK+SDLK_RCTRL);}
if (GetKeyState(VK_LMENU)<0){bindkey=SDLK_LALT; keydown_vk(VK+SDLK_LALT);}
if (GetKeyState(VK_RMENU)<0){bindkey=SDLK_RALT; keydown_vk(VK+SDLK_RALT);}
if (GetKeyState(VK_CAPITAL)&1){bindkey=SDLK_CAPSLOCK; keydown_vk(VK+SDLK_CAPSLOCK);}
if (GetKeyState(VK_NUMLOCK)&1){bindkey=SDLK_NUMLOCK; keydown_vk(VK+SDLK_NUMLOCK);}
update_shift_state();
keyhit_next=keyhit_nextfree;//skip hitkey events generated by above code
#endif

#ifdef QB64_IME
//setup
SDL_Event eventExpose;
SDL_Rect rect;
int inputedWidth;
Uint16 *inputedString;
int needSize;
int stringLength;
ime_back=SDL_CreateRGBSurface(SDL_SWSURFACE,2048,32,32,0,0,0,0); if (!ime_back) exit(601);
if (InputMethod_Init()!=INPUT_METHOD_SUCCESS) exit(602);
InputMethod_InitWin32();
imefontname=qbs_new(0,0);
qbs_set(imefontname,qbs_new_txt_len("cyberbit.ttf\0",13));
imefontname2=qbs_new(0,0);
qbs_set(imefontname2,qbs_add(qbs_new_txt(getenv("WinDir")),qbs_new_txt_len("\\fonts\\cyberbit.ttf\0",20)));
imefontname3=qbs_new(0,0);
qbs_set(imefontname3,qbs_add(qbs_new_txt(getenv("WinDir")),qbs_new_txt_len("\\fonts\\arialuni.ttf\0",20)));
imefontname4=qbs_new(0,0);
qbs_set(imefontname4,qbs_add(qbs_new_txt(getenv("WinDir")),qbs_new_txt_len("\\fonts\\msmincho.ttf\0",20)));
inputedString = (Uint16*)malloc(sizeof(Uint16));
inputedString[0] = 0x0000;
inputedWidth = 0;
/* possible exit options
	free(inputedString);
	TTF_CloseFont(ime_font);
	InputMethod_Quit();
	SDL_Quit();
*/	
#endif

sdl_modes=SDL_ListModes(NULL,SDL_FULLSCREEN|SDL_HWSURFACE);
//count modes
if(sdl_modes){
if(sdl_modes==(SDL_Rect **)-1){
anymode=1;//*unhandled!
}else{
for(i=0;sdl_modes[i];++i){
nmodes++;
}
}
}//modes
modes=(SDL_Rect*)malloc(nmodes*sizeof(SDL_Rect));
for(i=0;sdl_modes[i];++i){
modes[i]=*sdl_modes[i];

}


//SDL_Rect *modes=NULL;



memset(snd,0,sizeof(snd));

//set key repeat rate
SDL_EnableKeyRepeat(500,30); key_repeat_on=1;
//safer to use default key repeat rates which aren't dissimilar to those used above

//enable unicode
SDL_EnableUNICODE(1);

//init fake keyb. cyclic buffer
cmem[0x41a]=30; cmem[0x41b]=0; //head
cmem[0x41c]=30; cmem[0x41d]=0; //tail

SDL_WM_SetIcon(SDL_LoadBMP_RW(SDL_RWFromConstMem(&file_qb64icon_bmp[0],file_qb64icon_bmp_len),0),NULL);

SDL_WM_SetCaption("Untitled",NULL);

int int_x,int_y;
SDL_GetMouseState(&int_x,&int_y);
mouse_messages[0].x=int_x;
mouse_messages[0].y=int_y;
mouse_messages[0].buttons=0;
mouse_messages[0].movementx=0;
mouse_messages[0].movementy=0;




//SDL_Cursor * mycursor=SDL_CreateCursor
		//(Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y);
SDL_Cursor * mycursor=init_system_cursor(arrow);
SDL_SetCursor(mycursor);

ifstream fh;

//default 256 color palette
memcpy(&palette_256,&file_qb64_pal[0],file_qb64_pal_len);
for(i=0;i<256;i++) palette_256[i]|=0xFF000000;

//default EGA(64) color palette
memcpy(&palette_64,&file_qb64ega_pal[0],file_qb64ega_pal_len);
for(i=0;i<64;i++) palette_64[i]|=0xFF000000;

//manually set screen 10 palette
pal_mode10[0][0]=0;
pal_mode10[0][1]=0;
pal_mode10[0][2]=0;
pal_mode10[0][3]=0x808080;
pal_mode10[0][4]=0x808080;
pal_mode10[0][5]=0x808080;
pal_mode10[0][6]=0xFFFFFF;
pal_mode10[0][7]=0xFFFFFF;
pal_mode10[0][8]=0xFFFFFF;
pal_mode10[1][0]=0;
pal_mode10[1][1]=0x808080;
pal_mode10[1][2]=0xFFFFFF;

pal_mode10[1][3]=0;
pal_mode10[1][4]=0x808080;
pal_mode10[1][5]=0xFFFFFF;
pal_mode10[1][6]=0;
pal_mode10[1][7]=0x808080;
pal_mode10[1][8]=0xFFFFFF;

//8x8 character set
memcpy(&charset8x8,&file_charset8_raw[0],file_charset8_raw_len);

//8x16 character set
memcpy(&charset8x16,&file_chrset16_raw[0],file_chrset16_raw_len);

//get 32bit argb pixel format
ts=SDL_CreateRGBSurface(NULL,8,8,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
pixelformat32=*ts->format;
SDL_FreeSurface(ts);
ts=SDL_CreateRGBSurface(NULL,8,8,8,NULL,NULL,NULL,NULL);
pixelformat8=*ts->format;
SDL_FreeSurface(ts);

qbg_screen(0,NULL,NULL,NULL,NULL,1);
width8050switch=1;//reaffirm switch reset by above command

    thread = SDL_CreateThread (QBMAIN, NULL  );
    if ( thread == NULL ) {
        fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
        return 0;
    }

    thread2 = SDL_CreateThread (TIMERTHREAD, NULL  );
    if ( thread2 == NULL ) {
        fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
        return 0;
    }


lock_display_required=1;



int32 update=0;//0=update input,1=update display

main_loop:

if (lock_mainloop==1){
lock_mainloop=2;
while (lock_mainloop==2) Sleep(1);
}

//update timer bytes in cmem
static uint32 cmem_ticks;
static double cmem_ticks_double;

cmem_ticks=SDL_GetTicks();
cmem_ticks-=sdl_firsttimervalue;
cmem_ticks+=qb64_firsttimervalue;
//make timer value loop after midnight
//note: there are 86400000 milliseconds in 24hrs(1 day)
cmem_ticks%=86400000;
cmem_ticks=((double)cmem_ticks)*0.0182;
cmem[0x46c]=cmem_ticks&255;
cmem[0x46d]=(cmem_ticks>>8)&255;
cmem[0x46e]=(cmem_ticks>>16)&255;
//note: a discrepancy exists of unknown cause

if (shell_call_in_progress){
if (shell_call_in_progress!=-1){
shell_call_in_progress=-1;
if (key_repeat_on){SDL_EnableKeyRepeat(0,0); key_repeat_on=0;}
goto update_display_only;
}
Sleep(64);
goto main_loop;
}

Sleep(15);
vertical_retrace_happened=1; vertical_retrace_in_progress=1;
Sleep(1);

if (close_program){
lock_mainloop=2;//report mainloop as locked so that any process waiting for a successful lock can continue
goto end_program;
}

update^=1;//toggle update

//buffered sound/play notes
//(safe even if sndsetup not called)
if (sndqueue_first!=sndqueue_next){
if (sndqueue_played==0){
 //note: this only happens the first time a sound is ever played (through this queue)
 sndqueue_played=1;
 if (sndqueue_first==sndqueue_wait){suspend_program^=2; sndqueue_wait=-1;}
 sub__sndplay(sndqueue[sndqueue_first]);
 sndqueue_first++;
}else{
 i=sndqueue_first-1; if (i==-1) i=sndqueue_lastindex;
 if (!func__sndplaying(sndqueue[i])){
 sub__sndclose(sndqueue[i]);
 if (sndqueue_first==sndqueue_wait){suspend_program^=2; sndqueue_wait=-1;}
 sub__sndplay(sndqueue[sndqueue_first]);
 sndqueue_first++; if (sndqueue_first>sndqueue_lastindex) sndqueue_first=0;
 }
}
}

if (!lprint){//not currently performing an LPRINT operation
lprint_locked=1;
if (lprint_buffered){
if (fabs(func_timer(0.001,1)-lprint_last)>=10.0){//10 seconds elapsed since last LPRINT operation
sub__printimage(lprint_image);
lprint_buffered=0;
static int32 old_dest;
old_dest=func__dest();
sub__dest(lprint_image);
sub_cls(NULL,15,2);
sub__dest(old_dest);
}
}
lprint_locked=0;
}




//update display?
if (update==1){
update_display_only:
if (autodisplay) display();
}//update==1

vertical_retrace_in_progress=0;

if (shell_call_in_progress) goto main_loop;

if (update==0){

//correct SDL key repeat error after losing input focus while a key is held
//occurs when switching to another window, SHELL-ing, etc.
//SDL_APPMOUSEFOCUS	The application has mouse focus.
//SDL_APPINPUTFOCUS	The application has keyboard focus
//SDL_APPACTIVE	The application is visible
static int32 state;
state=SDL_GetAppState();
if ( (!(state&SDL_APPINPUTFOCUS)) || (!(state&SDL_APPACTIVE)) ){//lost focus
if (key_repeat_on){SDL_EnableKeyRepeat(0,0); key_repeat_on=0;}
}else{
//active
if (!key_repeat_on){SDL_EnableKeyRepeat(500,30); key_repeat_on=1;}
}

#ifdef QB64_IME
//main loop

if (qb64_ime_reading){//load font when required
if (ime_font==NULL){
static int32 x,failed=0;
if (!failed){
x=fontopen((char*)imefontname->chr,29,0);//note: font is actually 2 pixels higher than stated
if (x==0){
x=fontopen((char*)imefontname2->chr,29,0);
}
if (x==0){
x=fontopen((char*)imefontname3->chr,29,0);
}
if (x==0){
x=fontopen((char*)imefontname4->chr,29,0);
}
if (x) ime_font=font[x]; else failed=1;
}
}
}

			if (InputMethod_GetEventNumber() > 0) {
				rect.x = 0;
				rect.y = 0;
				rect.w = 2048;
				rect.h = 32;				
				SDL_FillRect(ime_back, &rect, 0xFFFFFFFF);

static int32 len;
static uint32 *buf=(uint32*)malloc(2);
static int32 bufsize=1;
				
				switch (InputMethod_GetCurrentMessage()) {
				case INPUT_METHOD_MESSAGE_ON:
/*                    
					rect.x = 0;
					rect.y = 100;
					rect.w = 640;
					rect.h = 100;
					SDL_FillRect(ime_back, &rect, 0x00000000);
					surface = TTF_RenderUTF8_Shaded(
						ime_font, "IME: ON", fg, bg);
					SDL_BlitSurface(surface, NULL, ime_back, &rect);
					SDL_FreeSurface(surface);
					DrawCursor(ime_back, ime_font, inputedWidth);
*/
					break;
				case INPUT_METHOD_MESSAGE_CHANGE:
if (ime_font){
					MessageChange(ime_back, ime_font, inputedWidth);
}
					break;
				case INPUT_METHOD_MESSAGE_RESULT:

//send UNICODE string to qb64 key handler
len=GetUnicodeStringLength(InputMethod_GetCurrentEditingString())*2;
if (len>bufsize){free(buf); buf=(uint32*)malloc(len*4+4);}
len=convert_unicode(16,InputMethod_GetCurrentEditingString(),len,32,buf);
len/=4;
for (i=0;i<len;i++){
keydown_unicode(buf[i]);
keyup_unicode(buf[i]);
}

//don't let it concatenate!
/*



stringLength =
						GetUnicodeStringLength(inputedString) +
						GetUnicodeStringLength(
							InputMethod_GetCurrentEditingString());
					needSize = stringLength * sizeof(Uint16) + sizeof(Uint16);
					inputedString = (Uint16*)realloc(inputedString, needSize);
					CatUnicodeString(
						inputedString,
						InputMethod_GetCurrentEditingString());
					

					{
						long i;
						for (i=0;i<stringLength;i++){
							//showvalue(inputedString[i]);
						}
					}



					surface = TTF_RenderUNICODE_Shaded(
						font, inputedString, gfg, bg);
					inputedWidth = surface->w;
					SDL_BlitSurface(surface, NULL, ime_back, NULL);
					SDL_FreeSurface(surface);
					DrawCursor(ime_back, font, inputedWidth);
*/


					break;
				case INPUT_METHOD_MESSAGE_CHAR:
/*
stringLength =
						GetUnicodeStringLength(inputedString) + 1;
					needSize = stringLength * sizeof(Uint16) + sizeof(Uint16);
					inputedString = (Uint16*)realloc(inputedString, needSize);
					inputedString[stringLength - 1] =
						InputMethod_GetCurrentChar();
					inputedString[stringLength] = 0x0000;
					
					surface = TTF_RenderUNICODE_Shaded(
						ime_font, inputedString, rfg, bg);
					inputedWidth = surface->w;
					SDL_BlitSurface(surface, NULL, ime_back, NULL);
					SDL_FreeSurface(surface);
					DrawCursor(ime_back, ime_font, inputedWidth);
*/
					break;
				case INPUT_METHOD_MESSAGE_OFF:
/*
					rect.x = 0;
					rect.y = 100;
					rect.w = 640;
					rect.h = 100;
					SDL_FillRect(ime_back, &rect, 0x00000000);
					surface = TTF_RenderUTF8_Shaded(
						ime_font, "IME: OFF", fg, bg);
					SDL_BlitSurface(surface, NULL, ime_back, &rect);
					SDL_FreeSurface(surface);
*/
					break;
				default:
					break;
				}
				InputMethod_MoveNextEvent();
				eventExpose.type = SDL_VIDEOEXPOSE;
				SDL_PushEvent(&eventExpose);
			}
#endif

static int32 scancode;
static const uint8 sdlk_2_scancode[] = {
0,0,0,0,0,0,0,0,14,15,0,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,57,0,0,0,0,0,0,40,0,0,0,0,51,12,52,53,11,2,3,4,5,6,7,8,9,10,0,39,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,43,27,0,0,41,30,48,46,32,18,33,34,35,23,36,37,38,50,49,24,25,16,19,31,20,22,47,17,45,21,44,0,0,0,0,83,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,82,79,80,81,75,76,77,71,72,73,83,53,55,74,78,28,0,72,80,77,75,82,71,79,73,81,59,60,61,62,63,64,65,66,67,68,133,134,0,0,0,0,0,0,69,58,70,54,42,29,29,56,56,0,0,91,92,0,0,0,0,55,197,93,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

#ifdef QB64_WINDOWS 
//manage important external keyboard lock/state changes
if ((GetKeyState(VK_SCROLL)&1)!=keyheld(QBK+QBK_SCROLL_LOCK_MODE)){
if (keyheld(QBK+QBK_SCROLL_LOCK_MODE)){
keyheld_remove(QBK+QBK_SCROLL_LOCK_MODE);
}else{
keyheld_add(QBK+QBK_SCROLL_LOCK_MODE);
}
update_shift_state();
}
if ((GetKeyState(VK_CAPITAL)&1)!=keyheld(VK+SDLK_CAPSLOCK)){
if (keyheld(VK+SDLK_CAPSLOCK)){
bindkey=SDLK_CAPSLOCK; keyup_vk(VK+SDLK_CAPSLOCK);
}else{
bindkey=SDLK_CAPSLOCK; keydown_vk(VK+SDLK_CAPSLOCK);
}
update_shift_state();
}
if ((GetKeyState(VK_NUMLOCK)&1)!=keyheld(VK+SDLK_NUMLOCK)){
if (keyheld(VK+SDLK_NUMLOCK)){
bindkey=SDLK_NUMLOCK; keyup_vk(VK+SDLK_NUMLOCK);
}else{
bindkey=SDLK_NUMLOCK; keydown_vk(VK+SDLK_NUMLOCK);
}
update_shift_state();
}
#endif



static SDL_Event event;

    while ( SDL_PollEvent(&event) ) {


        switch (event.type) {




case SDL_JOYBALLMOTION:
{//scope
static int32 di;
for(di=1;di<=device_last;di++){
static device_struct *d;
d=&devices[di];
if (d->used==1){
if (d->type==1){
if (d->SDL_js_index==event.jball.which){
static uint8 *cp,*cp2;
static float f;
static int32 o;

if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
f=event.jball.xrel;
o=d->lastbutton+d->lastaxis*4+event.jball.ball*8;
*(float*)(d->events+(d->queued_events*d->event_size)+o)=f;
f=event.jball.yrel;
o=d->lastbutton+d->lastaxis*4+event.jball.ball*8+4;
*(float*)(d->events+(d->queued_events*d->event_size)+o)=f;
d->queued_events++;

//add dummy message to clear relative motion
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
f=0;
o=d->lastbutton+d->lastaxis*4+event.jball.ball*8;
*(float*)(d->events+(d->queued_events*d->event_size)+o)=f;
f=0;
o=d->lastbutton+d->lastaxis*4+event.jball.ball*8+4;
*(float*)(d->events+(d->queued_events*d->event_size)+o)=f;
d->queued_events++;

}//js index
}//type==1
}//used
}//di
}//scope
break;

case SDL_JOYHATMOTION:
{//scope
static int32 di;
for(di=1;di<=device_last;di++){
static device_struct *d;
d=&devices[di];
if (d->used==1){
if (d->type==1){
if (d->SDL_js_index==event.jaxis.which){
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
static int32 v;
static float fx,fy;
v=event.jhat.value;
fx=0; fy=0;
if (v&SDL_HAT_UP) fy=fy-1.0;
if (v&SDL_HAT_DOWN) fy=fy+1.0;
if (v&SDL_HAT_LEFT) fx=fx-1.0;
if (v&SDL_HAT_RIGHT) fx=fx+1.0;
static int32 o;
o=d->lastbutton+d->SDL_axes*4+event.jhat.hat*8;
*(float*)(d->events+(d->queued_events*d->event_size)+o)=fx;
o=d->lastbutton+d->SDL_axes*4+event.jhat.hat*8+4;
*(float*)(d->events+(d->queued_events*d->event_size)+o)=fy;
d->queued_events++;
}//js index
}//type==1
}//used
}//di
}//scope
break;

case SDL_JOYAXISMOTION:
{//scope
static int32 di;
for(di=1;di<=device_last;di++){
static device_struct *d;
d=&devices[di];
if (d->used==1){
if (d->type==1){
if (d->SDL_js_index==event.jaxis.which){
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
static float f;
f=event.jaxis.value;
if (f==-32768) f=-32767;
f/=32767.0;
if (f>1.0) f=1.0;
if (f<-1.0) f=-1.0;
static int32 o;
o=d->lastbutton+event.jaxis.axis*4;
*(float*)(d->events+(d->queued_events*d->event_size)+o)=f;
d->queued_events++;
}//js index
}//type==1
}//used
}//di
}//scope
break;

case SDL_JOYBUTTONDOWN:
{//scope
static int32 di,controller;
controller=0;
for(di=1;di<=device_last;di++){
static device_struct *d;
d=&devices[di];
if (d->used==1){
if (d->type==1){
controller++;
if (d->SDL_js_index==event.jbutton.which){

//ON STRIG event
static int32 i;
if (controller<=256&&event.jbutton.button<=255){//within supported range
i=(controller-1)*256+event.jbutton.button;
if (onstrig[i].active){
if (onstrig[i].id){
if (onstrig[i].active==1){//(1)ON
onstrig[i].state++;
}else{//(2)STOP
onstrig[i].state=1;
}
qbevent=1;
}
}
}//within supported range

static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
*(d->events+(d->queued_events*d->event_size)+event.jbutton.button)=1;
d->queued_events++;
//set STRIG_button_pressed for button
if (event.jbutton.button>=0&&event.jbutton.button<=255){
d->STRIG_button_pressed[event.jbutton.button]=1;
}
}//js index
}//type==1
}//used
}//di
}//scope
break;

case SDL_JOYBUTTONUP:
{//scope
static int32 di;
for(di=1;di<=device_last;di++){
static device_struct *d;
d=&devices[di];
if (d->used==1){
if (d->type==1){
if (d->SDL_js_index==event.jbutton.which){
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
*(d->events+(d->queued_events*d->event_size)+event.jbutton.button)=0;
d->queued_events++;
}//js index
}//type==1
}//used
}//di
}//scope
break;

/*
case SDL_ACTIVEEVENT:
 #ifdef QB64_WINDOWS
 if (shell_call_in_progress){
 if (event.active.gain==0){//lost focus
 if (full_screen){ 
 SDL_WM_IconifyWindow();
 }//full screen
 }//lost focus
 }//shell
 #endif
break;
*/

case SDL_KEYUP:
{//new scope

if (device_last){//core devices required?
static int32 code,special;
special=0;
code=event.key.keysym.sym;
if ((code==300)||(code==301)) special=1;
if ((code>0)&&(scancode<=512)){
code=(((512-code)^341)&511)+1;//switch indexing
static device_struct *d;
d=&devices[1];//keyboard
if ((*(d->events+((d->queued_events-1)*d->event_size)+(code-1))!=0)||special){
keyup_special:
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=(QUEUED_EVENTS_LIMIT/4)){//note: default event limit divied by 4
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
if (special){*(d->events+(d->queued_events*d->event_size)+(code-1))=1; special=0; d->queued_events++; goto keyup_special;}
*(d->events+(d->queued_events*d->event_size)+(code-1))=0;
d->queued_events++;
}//not 0
}//valid range
}//core devices required

if (qb64_ime_reading){
if (qb64_ime_reading==3) qb64_ime_reading=0;
goto skip_keyup;
}


static uint32 x;
x=event.key.keysym.sym;

//ALT+number(ascii-code)
//note: "Compose" an ASCII code for digits typed on the numeric keypad while ALT is held down. The code is the number (modulo 256).
static int32 altvalue;
altvalue=-1;
if (asciicode_reading){
if ((x==SDLK_LALT)||(x==SDLK_RALT)){
if (asciicode_reading==2){
altvalue=asciicode_value&255;
}
asciicode_reading=0;
}
}

if (x) keyheld_unbind(x);

if (altvalue!=-1){keydown_ascii(altvalue); keyup_ascii(altvalue);}

skip_keyup:;
}//end scope
break;

case SDL_KEYDOWN:
{//new scope

if (device_last){//core devices required?
static int32 code,special;
special=0;
code=event.key.keysym.sym;
if ((code==300)||(code==301)) special=2;
if ((code>0)&&(scancode<=512)){
code=(((512-code)^341)&511)+1;//switch indexing
keydown_special:
static device_struct *d;
d=&devices[1];//keyboard
if ((*(d->events+((d->queued_events-1)*d->event_size)+(code-1))!=1)||special){
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=(QUEUED_EVENTS_LIMIT/4)){//note: default event limit divied by 4
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
*(d->events+(d->queued_events*d->event_size)+(code-1))=1;
if (special==2){special=1; d->queued_events++; goto keydown_special;}
if (special==1) *(d->events+(d->queued_events*d->event_size)+(code-1))=0;
d->queued_events++;
}//not 1
}//valid range
}//core devices required

if (qb64_ime_reading){
if (qb64_ime_reading==2) qb64_ime_reading=3;
goto skip_key;
}


static uint32 x,x2;
x=event.key.keysym.sym;//SDL VK code

//ALT+number(ascii-code)
//note: "Compose" an ASCII code for digits typed on the numeric keypad while ALT is held down. The code is the number (modulo 256).
if ((x==SDLK_LALT)||(x==SDLK_RALT)){
if ((keyheld(VK+SDLK_LALT)==0)&&(keyheld(VK+SDLK_RALT)==0)){
asciicode_reading=1;
asciicode_value=0;
}
}
/* Numeric keypad
  	SDLK_KP0		= 256,
    ...
	SDLK_KP9		= 265,*/
if (asciicode_reading){
if ((x>=SDLK_KP0)&&(x<=SDLK_KP9)){
asciicode_reading=2;
asciicode_value*=10;
asciicode_value+=(x-SDLK_KP0);
}else{
if ((x!=SDLK_LALT)&&(x!=SDLK_RALT)) asciicode_reading=0;//sequence broken
}
}

bindkey=x;

x2=event.key.keysym.unicode;

//qbs_print(qbs_str((int32)x),1);
//qbs_print(qbs_str((int32)x2),1);

#ifdef QB64_MACOSX
if ((x2>=0xF700)&&(x2<=0xF8FF)){//within MacOSX UNICODE reserved range
if (x) x2=0;//ignore unicode value and refer to keysym only
}
#endif
if ((x2==127)&&(x==8)) x2=8;//remap UNICODE 127 to UNICODE 8 when linked to SDL backspace key's keysym
if ((x2==127)&&(x==127)) x2=0;//remap UNICODE 127/KEYSYM 127 to NON-UNICODE KEYSYM 127 (Linux)

if (x2){

if ((x>=SDLK_KP0)&&(x<=SDLK_KP_ENTER)){//NUMPAD specific
keydown_vk(VK+x);
goto skip_key;
}

//revert any CTRL+? keys
if (keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL)){//CTRL held
if (x2==0x7F) if (x==SDLK_BACKSPACE) {keydown_ascii(8); goto skip_key;}
if (x2==0x0A) if (x==SDLK_RETURN) {keydown_ascii(13); goto skip_key;}
if (x2==0x1B) if (x==SDLK_ESCAPE) {keydown_ascii(27); goto skip_key;}
if (x2==0x20) if (x==SDLK_SPACE) {keydown_ascii(32); goto skip_key;}
if (x2==0x1E) if (x==SDLK_6) {keydown_ascii(54); goto skip_key;}
if (x2==0x1B) if (x==SDLK_LEFTBRACKET) {keydown_ascii(91); goto skip_key;}
if (x2==0x1C) if (x==SDLK_BACKSLASH) {keydown_ascii(92); goto skip_key;}
if (x2==0x1D) if (x==SDLK_RIGHTBRACKET) {keydown_ascii(93); goto skip_key;}
if ((x2>=1)&&(x2<=26)){
static int32 x3;
x3=keyheld(VK+SDLK_CAPSLOCK);
x3=x3+(keyheld(VK+SDLK_LSHIFT)|keyheld(VK+SDLK_RSHIFT));
if (x3&1){//uppercase?
keydown_ascii(64+x2); goto skip_key;
}
keydown_ascii(96+x2); goto skip_key;
}
}

keydown_unicode(x2);
if (!x) keyup_unicode(x2);//simulate release for unbindable key
goto skip_key;
}
if (x){

//if ((x!=SDLK_LCTRL)&&(x!=SDLK_LALT)&&(x!=SDLK_LSHIFT)) showvalue(x);

if ((x>=SDLK_KP0)&&(x<=SDLK_KP_PERIOD)){//NUMPAD specific #1
keydown_vk(QBK+(x-SDLK_KP0));
goto skip_key;
}
if ((x>=SDLK_KP_DIVIDE)&&(x<=SDLK_KP_ENTER)){//NUMPAD specific #2
//handles operators (/,-,*,+) which are not set as UNICODE values when CTRL is held
keydown_vk(VK+x);
goto skip_key;
}

//remap SDL_VKs to CHR$(0)+CHR$(x) combination VKs
if ((x>=SDLK_F1)&&(x<=SDLK_F10)){keydown_vk((x-SDLK_F1+59)<<8);goto skip_key;}
if ((x>=SDLK_F11)&&(x<=SDLK_F12)){keydown_vk((x-SDLK_F11+133)<<8);goto skip_key;}
if (x==SDLK_HOME){keydown_vk(71<<8);goto skip_key;}
if (x==SDLK_UP){keydown_vk(72<<8);goto skip_key;}
if (x==SDLK_PAGEUP){keydown_vk(73<<8);goto skip_key;}
if (x==SDLK_LEFT){keydown_vk(75<<8);goto skip_key;}
//??? center  /   76 ,      0x4C,    0x4C00,      0x35,         0,         0,      0x35,    0x4C00,      0x35,    0x4C00,
if (x==SDLK_RIGHT){keydown_vk(77<<8);goto skip_key;}
if (x==SDLK_END){keydown_vk(79<<8);goto skip_key;}
if (x==SDLK_DOWN){keydown_vk(80<<8);goto skip_key;}
if (x==SDLK_PAGEDOWN){keydown_vk(81<<8);goto skip_key;}
if (x==SDLK_INSERT){keydown_vk(82<<8);goto skip_key;}
if (x==SDLK_DELETE){keydown_vk(83<<8);goto skip_key;}

//revert any CTRL+? keys which did not map to UNICODE
if (keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL)){//CTRL held
if ((x>=SDLK_SPACE)&&(x<=SDLK_BACKQUOTE)){//32-96
keydown_ascii(x); goto skip_key;
}//32-96
if (x==8){keydown_ascii(x); goto skip_key;}
if (x==9){keydown_ascii(x); goto skip_key;}
if (x==13){keydown_ascii(13); goto skip_key;}
if (x==27){keydown_ascii(13); goto skip_key;}
}//CTRL

if ((x>=(SDLK_a))&&(x<=SDLK_z)){
//note: handles instances such as CTRL+ALT+"a"
x=x-97;
static int32 x3;
x3=keyheld(VK+SDLK_CAPSLOCK);
x3=x3+(keyheld(VK+SDLK_LSHIFT)|keyheld(VK+SDLK_RSHIFT));
if (x3&1){//uppercase?
keydown_ascii(65+x); goto skip_key;
}
keydown_ascii(97+x); goto skip_key;
}

if ((x>=32)&&(x<=127)){//skip possible dead-key
bindkey=0;
goto skip_key;
}

keydown_vk(VK+x);

}

skip_key:;
}//end scope
break;

case SDL_MOUSEMOTION:

//message #1
i=(last_mouse_message+1)&65535;
if (i==current_mouse_message) current_mouse_message=(current_mouse_message+1)&65535;//if buffer full, skip oldest message
mouse_messages[i].x=event.motion.x;
mouse_messages[i].y=event.motion.y;
if (mouseinput_ignoremovement){
mouseinput_ignoremovement--;
mouse_messages[i].movementx=0;
mouse_messages[i].movementy=0;
}else{
mouse_messages[i].movementx=event.motion.xrel;
mouse_messages[i].movementy=event.motion.yrel;
}
mouse_messages[i].buttons=event.motion.state;
last_mouse_message=i;

//message #2 (clears movement values to avoid confusion)
i=(last_mouse_message+1)&65535;
if (i==current_mouse_message) current_mouse_message=(current_mouse_message+1)&65535;//if buffer full, skip oldest message
mouse_messages[i].x=event.motion.x;
mouse_messages[i].y=event.motion.y;
mouse_messages[i].movementx=0;
mouse_messages[i].movementy=0;
mouse_messages[i].buttons=event.motion.state;
last_mouse_message=i;

if (device_last){//core devices required?
if (!device_mouse_relative){

static device_struct *d;
d=&devices[2];//mouse
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
static float fx,fy;
static int32 z;
fx=event.motion.x;
fx-=x_offset;
z=x_monitor-x_offset*2;
if (fx<0) fx=0;
if (fx>=z) fx=z-1;
fx=fx/(float)(z-1);//0 to 1
fx*=2.0;//0 to 2
fx-=1.0;//-1 to 1
fy=event.motion.y;
fy-=y_offset;
z=y_monitor-y_offset*2;
if (fy<0) fy=0;
if (fy>=z) fy=z-1;
fy=fy/(float)(z-1);//0 to 1
fy*=2.0;//0 to 2
fy-=1.0;//-1 to 1
*(float*)(d->events+(d->queued_events*d->event_size)+d->lastbutton)=fx;
*(float*)(d->events+(d->queued_events*d->event_size)+d->lastbutton+4)=fy;
d->queued_events++;

}else{

static device_struct *d;
d=&devices[2];//mouse
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
static float fx,fy;
static int32 z;
fx=event.motion.xrel;
fy=event.motion.yrel;
*(float*)(d->events+(d->queued_events*d->event_size)+d->lastbutton+d->lastaxis*4)=fx;
*(float*)(d->events+(d->queued_events*d->event_size)+d->lastbutton+d->lastaxis*4+4)=fy;
d->queued_events++;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
fx=0;
fy=0;
*(float*)(d->events+(d->queued_events*d->event_size)+d->lastbutton+d->lastaxis*4)=fx;
*(float*)(d->events+(d->queued_events*d->event_size)+d->lastbutton+d->lastaxis*4+4)=fy;
d->queued_events++;

}
}//core devices required

break;

case SDL_MOUSEBUTTONUP:
i=(last_mouse_message+1)&65535;
if (i==current_mouse_message) current_mouse_message=(current_mouse_message+1)&65535;//if buffer full, skip oldest message
mouse_messages[i].movementx=0;
mouse_messages[i].movementy=0;
mouse_messages[i].x=event.button.x;
mouse_messages[i].y=event.button.y;
mouse_messages[i].buttons=mouse_messages[last_mouse_message].buttons;
if (mouse_messages[i].buttons&(1<<(event.button.button-1))) mouse_messages[i].buttons^=(1<<(event.button.button-1));
last_mouse_message=i;

if (device_last){//core devices required?
static int32 button;
button=event.button.button;
if ((button>=1)&&(button<=3)){
button--;
static device_struct *d;
d=&devices[2];//mouse
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
*(d->events+(d->queued_events*d->event_size)+button)=0;
d->queued_events++;
}//valid range
}//core devices required

break;

case SDL_MOUSEBUTTONDOWN:
i=(last_mouse_message+1)&65535;
if (i==current_mouse_message) current_mouse_message=(current_mouse_message+1)&65535;//if buffer full, skip oldest message
mouse_messages[i].movementx=0;
mouse_messages[i].movementy=0;
mouse_messages[i].x=event.button.x;
mouse_messages[i].y=event.button.y;
mouse_messages[i].buttons=mouse_messages[last_mouse_message].buttons;
mouse_messages[i].buttons|=(1<<(event.button.button-1));
last_mouse_message=i;

if (device_last){//core devices required?
static int32 button;
button=event.button.button;

if ((button>=1)&&(button<=3)){
button--;
static device_struct *d;
d=&devices[2];//mouse
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
*(d->events+(d->queued_events*d->event_size)+button)=1;
d->queued_events++;
//1-3
}else{
//not 1-3
//mouse wheel?
if ((button>=4)&&(button<=5)){
static float f;
if (button==4) f=-1; else f=1;
static device_struct *d;
d=&devices[2];//mouse
static uint8 *cp,*cp2;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
*(float*)(d->events+(d->queued_events*d->event_size)+d->lastbutton+d->lastaxis*4+(3-1)*4)=f;
d->queued_events++;
if (d->queued_events==d->max_events){//expand/shift event buffer
 if (d->max_events>=QUEUED_EVENTS_LIMIT){
  //discard base message
  memmove(d->events,d->events+d->event_size,(d->queued_events-1)*d->event_size);
  d->queued_events--;
 }else{
  cp=(uint8*)calloc(d->max_events*2,d->event_size);
  memcpy(cp,d->events,d->queued_events*d->event_size);//copy existing events
  cp2=d->events;
  d->events=cp;
  free(cp2);
  d->max_events*=2;
 }
}
memmove(d->events+d->queued_events*d->event_size,d->events+(d->queued_events-1)*d->event_size,d->event_size);//duplicate last event
*(int64*)(d->events+(d->queued_events*d->event_size)+(d->event_size-8))=device_event_index++;//store global event index
//make required changes
*(float*)(d->events+(d->queued_events*d->event_size)+d->lastbutton+d->lastaxis*4+(3-1)*4)=0;
d->queued_events++;
}//4-5
}//not 1-3
}//core devices required

break;

case SDL_QUIT:
if (exit_blocked) exit_value|=1; else goto end_program;
break;

}
}



if (shell_call_in_progress) goto main_loop;

//safe even if sndsetup not called
static double pos;
if (stream_limited){
if (stream_loaded){
if (stream_playing){//worth checking?
pos=func__sndgetpos(1);
if (pos>=stream_limit){
sub__sndstop(1);
stream_limited=0;
}}}}

}//update==0

goto main_loop;

end_program:
stop_program=1;
qbevent=1;
#ifndef QB64_MACOSX
 SDL_WaitThread(thread, NULL);
#endif
while (exit_ok!=3) Sleep(16);
if (lprint_buffered){
sub__printimage(lprint_image);//print any pending content
}
sub_close(NULL,0);
SDL_ShowCursor(0);
SDL_FreeCursor(mycursor);
exit(0);
}

//display updates the visual page onto the visible window/monitor
void display(){

//general variables
static int32 i,i2,i3,i4;
static uint8 c,c2,c3,c4;
static int32 x,x2,x3,x4;
static int32 y,y2,y3,y4;
static int32 z,z2,z3,z4;
static float f,f2,f3,f4;
static uint8 *cp,*cp2,*cp3,*cp4;

static uint8 *pixeldata=(uint8*)malloc(1);
static int32 pixeldatasize=1;
static int32 paldata[256];
uint32 *pixel;

frame++;//~32 fps

if (lock_display==1){lock_display=2; Sleep(0);}
if (lock_display==0){


//validate display_page
if (!display_page) goto display_page_invalid;



//check what is possible in full screen
x=display_page->width; y=display_page->height;

if (display_page->compatible_mode==0){
x=display_page->width*fontwidth[display_page->font]; y=display_page->height*fontheight[display_page->font];
}

//check for y-stretch flag?
if (x<=512&&y<=384){
x*=2; y*=2;
}

static int32 mode_square,mode_stretch;

//find best fullscreen mode(s) (eg. square/"1:1", stretched)
mode_square=-1;
mode_stretch=-1;
z2=0x7FFFFFFF; z3=0x7FFFFFFF;
for (i=0;i<nmodes;i++){
if (modes[i].w>=640){
if (modes[i].w>=x&&modes[i].h>=y){
z=modes[i].w-x+modes[i].h-y;
 //square
 if (z<=z2){
 //is it square?
 if (modes[i].w/4==modes[i].h/3){
 mode_square=i; z2=z;
 }//square
 }//z2<=z
 //stretch
 if (z<=z3){
 mode_stretch=i; z3=z;
 }//z<=z3
}//>x,>y
}//ignore?
}//i

static int32 full_screen_change;
full_screen_change=0;

if (full_screen_set!=-1){
if (full_screen_set==0) goto full_screen0;
if (full_screen_set==1) goto full_screen1;
if (full_screen_set==2) goto full_screen2;
}

if (full_screen_toggle){
full_screen_toggle--;

if (full_screen==0){
full_screen1:
if (mode_stretch>-1){//can be displayed
full_screen=1;
if (autodisplay==1) mousemovementfix_mainloop(); else mousemovementfix();
 full_screen_change=1;
 screen_last_valid=0;

 if (!mouse_hideshow_called){
 #ifdef QB64_LINUX
 if (autodisplay!=1){
 lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
 }
 #endif
 SDL_ShowCursor(0);
 #ifdef QB64_LINUX
 if (autodisplay!=1){ 
 lock_mainloop=0; Sleep(1);//unlock
 } 
 #endif
 }

}
goto full_screen_toggle_done;
}

if (full_screen==1){
full_screen2:
if (mode_square>-1&&mode_square!=mode_stretch){//usable 1:1 mode exists (that isn't same as stretched mode)
full_screen=2;
if (autodisplay==1) mousemovementfix_mainloop(); else mousemovementfix();
 full_screen_change=1;
 screen_last_valid=0;

 if (!mouse_hideshow_called){
 #ifdef QB64_LINUX
 if (autodisplay!=1){
 lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
 }
 #endif
 SDL_ShowCursor(0);
 #ifdef QB64_LINUX
 if (autodisplay!=1){ 
 lock_mainloop=0; Sleep(1);//unlock
 } 
 #endif
 }

goto full_screen_toggle_done;
}
}
//back to windowed mode
full_screen0:
full_screen=0;
 full_screen_change=1;
 screen_last_valid=0;

 if (!mouse_hideshow_called){
 #ifdef QB64_LINUX
 if (autodisplay!=1){
 lock_mainloop=1; while (lock_mainloop!=2) Sleep(1);//lock
 }
 #endif
 SDL_ShowCursor(1);
 #ifdef QB64_LINUX
 if (autodisplay!=1){ 
 lock_mainloop=0; Sleep(1);//unlock
 } 
 #endif
 }

}
full_screen_toggle_done:
full_screen_set=-1;


//set the screen mode if necessary

x_offset=0; y_offset=0;
x_scale=1; y_scale=1;

x=display_page->width; y=display_page->height;
if (display_page->compatible_mode==0){
x=display_page->width*fontwidth[display_page->font]; y=display_page->height*fontheight[display_page->font];
}

//check for y-stretch flag?

if (full_screen){

//double res. of "small" surfaces to avoid video driver incompatibilities
if (x<=512&&y<=384){
x*=2; y*=2;
y_scale*=2; x_scale*=2;
}

x2=x; y2=y;
if (full_screen==1){
x=modes[mode_stretch].w; y=modes[mode_stretch].h;
}
if (full_screen==2){
x=modes[mode_square].w; y=modes[mode_square].h;
}

//calculate offsets
x_offset=(x-x2)/2; y_offset=(y-y2)/2;

}//full_screen



//adjust monitor resolution
if ((x!=x_monitor)||(y!=y_monitor)||full_screen_change){
x_monitor=x; y_monitor=y;
if (full_screen) z=SDL_FULLSCREEN; else z=0;
mouseinput_ignoremovement=65536;
display_surface=SDL_SetVideoMode(x,y,32,z);
mouseinput_flush();
mouseinput_ignoremovement=4;
display_surface_offset=(uint32*)display_surface->pixels;
conversion_required=0;
if ((display_surface->format->Bmask!=255)||(display_surface->format->Gmask!=65280)||(display_surface->format->Rmask!=16711680)){
 //non-BGR(A) format!
 //create conversion layer
 conversion_required=1;
 conversion_layer=(uint32*)realloc(conversion_layer,x_monitor*y_monitor*4);
 memset(conversion_layer,0,x_monitor*y_monitor*4);
 display_surface_offset=conversion_layer;//redirect to conversion layer
}
pixel=display_surface_offset;//<-will be made obselete
}

if (!screen_last_valid){
memset(display_surface->pixels,0,x_monitor*y_monitor*4);
}

//update the screen data

if (!display_page->compatible_mode){//text

//removed following line on 22/1/09, questionable performace gain vs visual loss
//if (frame&1) goto screen_refresh_unrequired;//~16fps for text mode

static int32 show_flashing_last=0;
static int32 show_cursor_last=0;
static int32 check_last;
static uint8 *cp,*cp2,*cp_last;
static uint32 *lp;
static int32 cx,cy;
static int32 cx_last=-1,cy_last=-1;
static int32 show_cursor;
static int32 show_flashing;
static uint8 chr,col,chr_last,col_last;
static int32 screen_changed;
static int32 qbg_y_offset;


static int32 f,f_pitch,f_width,f_height;//font info
f=display_page->font; f_width=fontwidth[f]; f_height=fontheight[f];

//realloc buffer if necessary
i=display_page->width*display_page->height*2;
if (screen_last_size!=i){
free(screen_last);
screen_last=(uint8*)malloc(i);
screen_last_size=i;
}

check_last=screen_last_valid;
if (!check_last){
 //set pal_last (no prev pal was avilable to compare to)
 memcpy(&pal_last[0],display_page->pal,256*4);
}else{
 //if palette has changed, update pal_last and draw all characters
 if (memcmp(&pal_last[0],display_page->pal,256*4)){
 memcpy(&pal_last[0],display_page->pal,256*4);
 check_last=0;
 }
}

//calculate cursor position (base 0)
cx=display_page->cursor_x-1; cy=display_page->cursor_y-1;

//show cursor?
if (frame&4) show_cursor=1; else show_cursor=0;

//show flashing?
if (frame&8) show_flashing=1; else show_flashing=0;

qbg_y_offset=y_offset*x_monitor+x_offset;//the screen base offset
cp=display_page->offset;//read from
cp_last=screen_last;//written to for future comparisons

//outer loop
y2=0;
for (y=0;y<display_page->height;y++){
x2=0;
for (x=0;x<display_page->width;x++){

chr=*cp; cp++; col=*cp; cp++;

//can be skipped?
chr_last=*cp_last; cp_last++; col_last=*cp_last; cp_last++;

if (check_last){
if (chr==chr_last){//same character
if (col==col_last){//same colours
if (col&128) if (show_flashing!=show_flashing_last) goto cantskip;//same flash
if (x==cx) if (y==cy) if (show_cursor!=show_cursor_last) goto cantskip;//same cursor
if (x==cx_last){ if (y==cy_last){
if ((cx!=cx_last)||(cy!=cy_last)) goto cantskip;//fixup old cursor's location
}}
goto skip;
}}}
cantskip:
screen_changed=1;
cp_last-=2; *cp_last=chr; cp_last++; *cp_last=col; cp_last++;

//set cp2 to the character's data
z2=0;//double-width
if (f>=32){//custom font
static SDL_Surface *ts=NULL;
static SDL_Color c;
c.r=255; c.g=255; c.b=255; c.unused=0;//dummy values
if (ts) SDL_FreeSurface(ts);//free previously created buffer

z=codepage437_to_unicode16[chr];
ts=TTF_RenderUNICODE_Solid(font[f],(Uint16*)&z,c);
//z=chr;
//ts=TTF_RenderText_Solid(font[f],(char*)&z,c);//8-bit, 0=clear, 1=text

cp2=((uint8*)ts->pixels)+ts->pitch;
f_pitch=ts->pitch-f_width;
}else{//default font
f_pitch=0;
if (f==8) cp2=&charset8x8[chr][0][0];
if (f==14) cp2=&charset8x16[chr][1][0];
if (f==16) cp2=&charset8x16[chr][0][0];
if (f==(8+1)) {cp2=&charset8x8[chr][0][0]; z2=1;}
if (f==(14+1)) {cp2=&charset8x16[chr][1][0]; z2=1;}
if (f==(16+1)) {cp2=&charset8x16[chr][0][0]; z2=1;}
}
c=col&0xF;//foreground col
c2=(col>>4)&7;//background col
c3=col>>7;//flashing?
if (c3&&show_flashing) c=c2;
i2=display_page->pal[c];
i3=display_page->pal[c2];
lp=display_surface_offset+qbg_y_offset+y2*x_monitor+x2;
z=x_monitor-fontwidth[display_page->font];

//inner loop
for (y3=0;y3<f_height;y3++){
for (x3=0;x3<f_width;x3++){
if (*cp2) *lp=i2; else *lp=i3;
if (z2){
if (x3&z2) cp2++;
}else{
cp2++;
}
lp++;
}
lp+=z;
cp2+=f_pitch;
}//y3,x3

//draw cursor
if (display_page->cursor_show&&show_cursor&&(cx==x)&&(cy==y)){
static int32 v1,v2;
static uint8 from_bottom;//bottom is the 2nd bottom scanline in width ?x25
static uint8 half_cursor;//if set, overrides all following values
static uint8 size;//if 0, no cursor is drawn, if 255, from begin to bottom
static uint8 begin;//only relevant if from_bottom was not specified
v1=display_page->cursor_firstvalue;
v2=display_page->cursor_lastvalue;
from_bottom=0;
half_cursor=0;
size=0;
begin=0;
//RULE: IF V2=0, NOTHING (UNLESS V1=0)
if (v2==0){
if (v1==0){size=1; goto cursor_created;}
goto nocursor;//no cursor!
}
//RULE: IF V2<V1, FROM V2 TO BOTTOM
if (v2<v1){begin=v2; size=255; goto cursor_created;}
//RULE: IF V1=V2, SINGLE SCANLINE AT V1 (OR BOTTOM IF V1>=4)
if (v1==v2){
if (v1<=3){begin=v1; size=1; goto cursor_created;}
from_bottom=1; size=1; goto cursor_created;
}
//NOTE: V2 MUST BE LARGER THAN V1!
//RULE: IF V1>=3, CALC. DIFFERENCE BETWEEN V1 & V2
//                IF DIFF=1, 2 SCANLINES AT BOTTOM
//                IF DIFF=2, 3 SCANLINES AT BOTTOM
//                OTHERWISE HALF CURSOR
if (v1>=3){
if ((v2-v1)==1){from_bottom=1; size=2; goto cursor_created;}
if ((v2-v1)==2){from_bottom=1; size=3; goto cursor_created;}
half_cursor=1; goto cursor_created;
}
//RULE: IF V1<=1, IF V2<=3 FROM V1 TO V3 ELSE FROM V1 TO BOTTOM
if (v1<=1){
if (v2<=3){begin=v1;size=v2-v1+1; goto cursor_created;} 
begin=v1;size=255; goto cursor_created;
}
//RULE: IF V1=2, IF V2=3, 2 TO 3
//               IF V2=4, 3 SCANLINES AT BOTTOM
//               IF V2>=5, FROM 2 TO BOTTOM
//(assume V1=2)
if (v2==3){begin=2;size=2; goto cursor_created;}
if (v2==4){from_bottom=1; size=3; goto cursor_created;}
begin=2;size=255;
cursor_created:
static int32 cw,ch;
cw=fontwidth[display_page->font]; ch=fontheight[display_page->font];
if (half_cursor){
//half cursor
y3=ch-1;
size=ch/2;
c=col&0xF;//foreground col
i2=display_page->pal[c];
draw_half_curs:
lp=display_surface_offset+qbg_y_offset+(y2+y3)*x_monitor+x2;
for (x3=0;x3<cw;x3++){
*lp=i2;
lp++;
}
y3--;
size--;
if (size) goto draw_half_curs;
}else{
if (from_bottom){
//cursor from bottom
y3=ch-1;
if (y3==15) y3=14;//leave bottom line blank in 8x16 char set
c=col&0xF;//foreground col
i2=display_page->pal[c];
draw_curs_from_bottom:
lp=display_surface_offset+qbg_y_offset+(y2+y3)*x_monitor+x2;
for (x3=0;x3<cw;x3++){
*lp=i2;
lp++;
}
y3--;
size--;
if (size) goto draw_curs_from_bottom;
}else{
//cursor from begin using size
if (begin<ch){
y3=begin;
c=col&0xF;//foreground col
i2=display_page->pal[c];
if (size==255) size=ch-begin;
draw_curs_from_begin:
lp=display_surface_offset+qbg_y_offset+(y2+y3)*x_monitor+x2;
for (x3=0;x3<cw;x3++){
*lp=i2;
lp++;
}
y3++;
size--;
if (size) goto draw_curs_from_begin;
}
}
}
}//draw cursor?
nocursor:

//outer loop
skip:
x2=x2+fontwidth[display_page->font];
}
y2=y2+fontheight[display_page->font];

}

show_flashing_last=show_flashing;
show_cursor_last=show_cursor;
cx_last=cx;
cy_last=cy;
screen_last_valid=1;
if (!screen_changed) goto screen_refresh_unrequired;
goto screen_refreshed;

}//text

if (display_page->bits_per_pixel==32){
//scaled refresh
if (x_scale==2){
static uint32 *lp;
static uint32 *lp2;
static uint32 c;
lp=display_page->offset32;
lp2=display_surface_offset+x_monitor*y_offset+x_offset;
x2=display_page->width;
z=x2*8;
y2=display_page->height;
for (y=0;y<y2;y++){
for (x=0;x<x2;x++){
c=*lp++; *lp2++=c; *lp2++=c;
}
lp2+=(x_monitor-(x2*2));
if (y_scale==2){memcpy(lp2,lp2-x_monitor,z);lp2+=x_monitor;}
}
goto screen_refreshed;
}//x_scale==2
if (x_scale==1){
static uint32 *lp;
static uint32 *lp2;
lp=display_page->offset32;
lp2=display_surface_offset+x_monitor*y_offset+x_offset;
x2=display_page->width;
z=x2*4;
y2=display_page->height;
for (y=0;y<y2;y++){
memcpy(lp2,lp,z);
lp2+=x_monitor;
if (y_scale==2){memcpy(lp2,lp,z);lp2+=x_monitor;}
lp+=x2;
}
goto screen_refreshed;
}//x_scale==1
exit(48341);
}//32

//assume <=256 colors using palette

if (display_page->compatible_mode==10){//update SCREEN 10 palette
i2=SDL_GetTicks()&512;
if (i2) i2=1;
for (i=0;i<=3;i++){
display_page->pal[i]=pal_mode10[i2][display_page->pal[i+4]];//pal_mode10[0-1][0-8]
}
}


i=display_page->width*display_page->height;
i2=1<<display_page->bits_per_pixel;//unique colors
//data changed?
if (i!=pixeldatasize){
free(pixeldata);
pixeldata=(uint8*)malloc(i);
pixeldatasize=i;
goto update_display;
}
if (memcmp(pixeldata,display_page->offset,i)) goto update_display;
//palette changed?
if (memcmp(paldata,display_page->pal,i2*4)) goto update_display;
//force update because of mode change?
if (!screen_last_valid) goto update_display;
if (qb64_ime_reading==1) goto screen_refreshed;
goto screen_refresh_unrequired;//no need to update display
update_display:
memcpy(pixeldata,display_page->offset,i);
memcpy(paldata,display_page->pal,i2*4);

if (x_scale==2){
static uint8 *cp;
static uint32 *lp2;
static uint32 c;
cp=pixeldata;
lp2=display_surface_offset+x_monitor*y_offset+x_offset;
x2=display_page->width;
y2=display_page->height;
z=x2*8;
for (y=0;y<y2;y++){
for (x=0;x<x2;x++){
c=paldata[*cp++]; *lp2++=c; *lp2++=c;
}//x
lp2+=(x_monitor-(x2*2));
if (y_scale==2){memcpy(lp2,lp2-x_monitor,z);lp2+=x_monitor;}
}//y
goto screen_refreshed;
}

if (x_scale==1){
static uint8 *cp;
static uint32 *lp2;
static uint32 c;
cp=pixeldata;
lp2=display_surface_offset+x_monitor*y_offset+x_offset;
x2=display_page->width;
y2=display_page->height;
z=x2*4;
for (y=0;y<y2;y++){
for (x=0;x<x2;x++){
*lp2++=paldata[*cp++];
}//x
lp2+=(x_monitor-x2);
if (y_scale==2){memcpy(lp2,lp2-x_monitor,z);lp2+=x_monitor;}
}//y
goto screen_refreshed;
}

error(4621);

screen_refreshed:

if (conversion_required){
uint8 *rp1;
uint8 *gp1;
uint8 *bp1;
uint8 *rp2;
uint8 *gp2;
uint8 *bp2;
int32 i;
int32 pixels;
uint32 *d;
bp1=(uint8*)display_surface_offset; gp1=bp1+1; rp1=gp1+1;
d=(uint32*)display_surface->pixels;
bp2=((uint8*)d)+3; gp2=bp2; rp2=bp2;//default 'safe' positions
if (display_surface->format->Bmask==255) bp2=bp2-3;
if (display_surface->format->Bmask==65280) bp2=bp2-2;
if (display_surface->format->Bmask==16711680) bp2=bp2-1;
if (display_surface->format->Gmask==255) gp2=gp2-3;
if (display_surface->format->Gmask==65280) gp2=gp2-2;
if (display_surface->format->Gmask==16711680) gp2=gp2-1;
if (display_surface->format->Rmask==255) rp2=rp2-3;
if (display_surface->format->Rmask==65280) rp2=rp2-2;
if (display_surface->format->Rmask==16711680) rp2=rp2-1;
pixels=x_monitor*y_monitor;
for (i=0;i<pixels;i++){
*bp2=*bp1;
*gp2=*gp1;
*rp2=*rp1;
rp1+=4; gp1+=4; bp1+=4;
rp2+=4; gp2+=4; bp2+=4;
}//i
}//conversion_required

#ifdef QB64_IME
//display
if (qb64_ime_reading==1){
	static SDL_Rect drect;
    drect.x = display_surface->w-ime_DrawMessageBlock_lastpixeloffset;
	drect.y = 0;
	drect.w = ime_DrawMessageBlock_lastpixeloffset;
	drect.h = 32;
	static SDL_Rect srect;
    srect.x = 0;
	srect.y = 0;
	srect.w = ime_DrawMessageBlock_lastpixeloffset;
	srect.h = 32;
SDL_BlitSurface(ime_back, &srect, display_surface, &drect);
}
#endif

SDL_UpdateRect(display_surface,0,0,0,0);

screen_last_valid=1;
if (qb64_ime_reading==1) screen_last_valid=0;

display_page_invalid:

/*
static int32 qbg_y_offset;//obselete?
qbg_y_offset=0;

//Video mode

x_offset=0; y_offset=0;
x_scale=1; y_scale=1;

if (qbg_text_only){
qbg_width=qbg_width_in_characters*qbg_character_width;
qbg_height=qbg_height_in_characters*qbg_character_height;
}
x=qbg_width; y=qbg_height;

//SCREEN 2/8 custom resolution
if (x==640&&y==200){
y_scale=2;
x=640; y=400;
}

x_monitor=x; y_monitor=y;

if (full_screen){

//fix 320x200 full screen resolution (SDL bug/incompatibility in low res. modes)
if (x==320&&y==200){
x_scale=2; y_scale=2;
x=640; y=400;
x_monitor=x; y_monitor=y;
}


//Check against available resolutions by priorities
//1. Exact match?
for (i=0;i<nmodes;i++){
if (modes[i]->w==x&&modes[i]->h==y) goto modeok;
}
//2. Same x, larger y?
y2=1000000;
for (i=0;i<nmodes;i++){
if (modes[i]->w==x){
if (modes[i]->h>y){
if (modes[i]->h<y2) y2=modes[i]->h;
}
}
}
if (y2!=1000000){
y_monitor=y2;
y_offset=(y_monitor-y)/2;
goto modeok;
}

//No mode seemed appropriate! Continue with default window resolution.

}//fullscreen
modeok:

if ((qbg_program_window_width!=x_monitor)||(qbg_program_window_height!=y_monitor)||(full_screen_in_use!=full_screen)){
qbg_program_window_width=x_monitor; qbg_program_window_height=y_monitor;


screen = SDL_SetVideoMode(qbg_program_window_width, qbg_program_window_height, 32, SDL_SWSURFACE|(SDL_FULLSCREEN*full_screen));

pixel=(uint32*)screen->pixels;
full_screen_in_use=full_screen;
}
if (!screen_last_valid){
memset(pixel,0,qbg_program_window_width*qbg_program_window_height);
}

if (qbg_text_only){
//if (frame&1) goto screen_refresh_unrequired;//~16fps for text mode
static int32 show_flashing_last=0;
static int32 show_cursor_last=0;
static int32 check_last;
static uint8 *cp,*cp2,*cp_last;
static uint32 *lp;
static int32 cx,cy;
static int32 cx_last=-1,cy_last=-1;
static int32 show_cursor;
static int32 show_flashing;
static uint8 chr,col,chr_last,col_last;
static int32 screen_changed;

//realloc buffer if necessary
i=qbg_height_in_characters*qbg_width_in_characters*2;
if (screen_last_size!=i){
free(screen_last);
screen_last=(uint8*)malloc(i);
screen_last_size=i;
}

check_last=screen_last_valid;
if (!check_last){
 //set pal_last
 memcpy(&pal_last[0],&pal[0],256*4);
}else{
 //if palette has changed, update pal_last and draw all characters
 if (memcmp(&pal_last[0],&pal[0],256*4)){
 memcpy(&pal_last[0],&pal[0],256*4);
 check_last=0;
 }
}

//calculate cursor position (base 0)
cx=qbg_cursor_x; cy=qbg_cursor_y;
if (qbg_visual_page!=qbg_active_page){
cx=qbg_cursor_x_previous[qbg_visual_page]; cy=qbg_cursor_y_previous[qbg_visual_page];
}
cx--; cy--;

//show cursor?
if (frame&4) show_cursor=1; else show_cursor=0;

//show flashing?
if (frame&8) show_flashing=1; else show_flashing=0;

qbg_y_offset=(qbg_width*x_scale*y_offset);
cp=qbg_visual_page_offset;
cp_last=screen_last;

//outer loop
y2=0;
for (y=0;y<qbg_height_in_characters;y++){
x2=0;
for (x=0;x<qbg_width_in_characters;x++){

chr=*cp; cp++; col=*cp; cp++;

//can be skipped?
chr_last=*cp_last; cp_last++; col_last=*cp_last; cp_last++;
if (check_last){
if (chr==chr_last){//same character
if (col==col_last){//same colours
if (col&128) if (show_flashing!=show_flashing_last) goto cantskip;//same flash
if (x==cx) if (y==cy) if (show_cursor!=show_cursor_last) goto cantskip;//same cursor
if (x==cx_last){ if (y==cy_last){
if ((cx!=cx_last)||(cy!=cy_last)) goto cantskip;//fixup old cursor's location
}}
goto skip;
}}}
cantskip:
screen_changed=1;
cp_last-=2; *cp_last=chr; cp_last++; *cp_last=col; cp_last++;

//draw the character
if (qbg_character_height==8){
cp2=&charset8x8[chr][0][0];
}else{
if (qbg_character_height==16){
cp2=&charset8x16[chr][0][0];
}else{//assume 14
cp2=&charset8x16[chr][1][0];
}
}
if (qbg_character_width==16) z2=1; else z2=0;
//set the correct color
c=col&0xF;//foreground col
c2=(col>>4)&7;//background col
c3=col>>7;//flashing?
if (c3&&show_flashing) c=c2;
i2=pal[c];
i3=pal[c2];
lp=pixel+y2*qbg_width_in_characters*qbg_character_width+x2+qbg_y_offset;
z=qbg_width_in_characters*qbg_character_width-qbg_character_width;

//inner loop
for (y3=0;y3<qbg_character_height;y3++){
for (x3=0;x3<qbg_character_width;x3++){
if (*cp2) *lp=i2; else *lp=i3;
if (z2){
if (x3&z2) cp2++;
}else{
cp2++;
}
lp++;
}
lp+=z;
}//y3,x3



//draw cursor?
if (qbg_cursor_show&&show_cursor&&(cx==x)&&(cy==y)){
static int32 v1,v2;
static uint8 from_bottom;//bottom is the 2nd bottom scanline in width ?x25
static uint8 half_cursor;//if set, overrides all following values
static uint8 size;//if 0, no cursor is drawn, if 255, from begin to bottom
static uint8 begin;//only relevant if from_bottom was not specified
v1=qbg_cursor_firstvalue;
v2=qbg_cursor_lastvalue;
from_bottom=0;
half_cursor=0;
size=0;
begin=0;
//RULE: IF V2=0, NOTHING (UNLESS V1=0)
if (v2==0){
if (v1==0){size=1; goto cursor_created;}
goto nocursor;//no cursor!
}
//RULE: IF V2<V1, FROM V2 TO BOTTOM
if (v2<v1){begin=v2; size=255; goto cursor_created;}
//RULE: IF V1=V2, SINGLE SCANLINE AT V1 (OR BOTTOM IF V1>=4)
if (v1==v2){
if (v1<=3){begin=v1; size=1; goto cursor_created;}
from_bottom=1; size=1; goto cursor_created;
}
//NOTE: V2 MUST BE LARGER THAN V1!
//RULE: IF V1>=3, CALC. DIFFERENCE BETWEEN V1 & V2
//                IF DIFF=1, 2 SCANLINES AT BOTTOM
//                IF DIFF=2, 3 SCANLINES AT BOTTOM
//                OTHERWISE HALF CURSOR
if (v1>=3){
if ((v2-v1)==1){from_bottom=1; size=2; goto cursor_created;}
if ((v2-v1)==2){from_bottom=1; size=3; goto cursor_created;}
half_cursor=1; goto cursor_created;
}
//RULE: IF V1<=1, IF V2<=3 FROM V1 TO V3 ELSE FROM V1 TO BOTTOM
if (v1<=1){
if (v2<=3){begin=v1;size=v2-v1+1; goto cursor_created;} 
begin=v1;size=255; goto cursor_created;
}
//RULE: IF V1=2, IF V2=3, 2 TO 3
//               IF V2=4, 3 SCANLINES AT BOTTOM
//               IF V2>=5, FROM 2 TO BOTTOM
//(assume V1=2)
if (v2==3){begin=2;size=2; goto cursor_created;}
if (v2==4){from_bottom=1; size=3; goto cursor_created;}
begin=2;size=255;
cursor_created:

if (half_cursor){
//half cursor
y3=qbg_character_height-1;
size=qbg_character_height/2;
c=col&0xF;//foreground col
i2=pal[c];
draw_half_curs:
lp=pixel+(y2+y3)*qbg_width_in_characters*qbg_character_width+x2+qbg_y_offset;
for (x3=0;x3<qbg_character_width;x3++){
*lp=i2;
lp++;
}
y3--;
size--;
if (size) goto draw_half_curs;
}else{
if (from_bottom){
//cursor from bottom
y3=qbg_character_height-1;
if (y3==15) y3=14;//leave bottom line blank in 8x16 char set
c=col&0xF;//foreground col
i2=pal[c];
draw_curs_from_bottom:
lp=pixel+(y2+y3)*qbg_width_in_characters*qbg_character_width+x2+qbg_y_offset;
for (x3=0;x3<qbg_character_width;x3++){
*lp=i2;
lp++;
}
y3--;
size--;
if (size) goto draw_curs_from_bottom;
}else{
//cursor from begin using size
if (begin<qbg_character_height){
y3=begin;
c=col&0xF;//foreground col
i2=pal[c];
if (size==255) size=qbg_character_height-begin;
draw_curs_from_begin:
lp=pixel+(y2+y3)*qbg_width_in_characters*qbg_character_width+x2+qbg_y_offset;
for (x3=0;x3<qbg_character_width;x3++){
*lp=i2;
lp++;
}
y3++;
size--;

if (size) goto draw_curs_from_begin;
}
}
}
}//draw cursor?
nocursor:



//outer loop
skip:
x2=x2+qbg_character_width;
}
y2=y2+qbg_character_height;
}

show_flashing_last=show_flashing;
show_cursor_last=show_cursor;
cx_last=cx;
cy_last=cy;
screen_last_valid=1;

if (!screen_changed) goto screen_refresh_unrequired;
goto screen_refreshed;
}//text only









//graphical modes

//update SCREEN 10 palette
if (qbg_mode==10){
//pal_mode10[0-1][0-8]
i2=SDL_GetTicks()&512;
if (i2) i2=1;
for (i=0;i<=3;i++){
pal[i]=pal_mode10[i2][qbg_color_assign[i]];
}
}

i=qbg_width*qbg_height;
i2=1<<qbg_bits_per_pixel;//i2 is number of colors

//data changed?
if (i!=pixeldatasize){
free(pixeldata);
pixeldata=(uint8*)malloc(i);
pixeldatasize=i;
goto update_display;
}
if (memcmp(pixeldata,qbg_visual_page_offset,i)) goto update_display;

//palette changed?
if (memcmp(paldata,pal,i2*4)) goto update_display;

//force update because of mode change?
if (!screen_last_valid) goto update_display;

goto screen_refresh_unrequired;//no need to update display

update_display:

memcpy(pixeldata,qbg_visual_page_offset,i);
memcpy(paldata,pal,i2*4);
screen_last_valid=1;

//scaled refresh

if (x_scale==2){
static uint8 *cp;
static uint32 *lp;
static uint32 c;
cp=pixeldata;
lp=pixel+(qbg_width*x_scale*y_offset);
for (y=0;y<qbg_height;y++){
for (x=0;x<qbg_width;x++){
c=paldata[*cp++]; *lp++=c; *lp++=c;
}//x
 if (y_scale==2){
 memcpy(lp,lp-qbg_width*2,qbg_width<<3);
 lp+=(qbg_width*2);
 }
}//y
goto screen_refreshed;
}

if (x_scale==1){
static uint8 *cp;
static uint32 *lp;
static uint32 c;
cp=pixeldata;
lp=pixel+(qbg_width*x_scale*y_offset);
for (y=0;y<qbg_height;y++){
for (x=0;x<qbg_width;x++){
*lp++=paldata[*cp++];
}//x
 if (y_scale==2){
 memcpy(lp,lp-qbg_width,qbg_width<<2);
 lp+=qbg_width;
 }
}//y

goto screen_refreshed;
}


/*





//full screen 320x200 resolution correction
if (full_screen){
if (qbg_width==320&&qbg_height==200){
static uint8 *cp;
static uint32 *lp,*lp2;
static uint32 c;
cp=qbg_visual_page_offset;
lp=pixel;
lp2=pixel+640;
for (y=0;y<200;y++){
for (x=0;x<320;x++){
c=pal[*cp++];
*lp++=c; *lp++=c;
*lp2++=c; *lp2++=c;
}
lp+=640; lp2+=640;
}
goto screen_refreshed;
}
}

//SCREEN 2/8 double y-axis
if (qbg_width==640&&qbg_height==200){
uint8 *cp;
uint32 *lp;
cp=qbg_visual_page_offset;
lp=pixel;
i2=qbg_program_window_width*qbg_program_window_height;
i3=0;
for (i=0;i<i2;i++){
*lp=pal[*cp];
i3++;
if (i3==640){cp-=640;i3=-640;}
cp++;
lp++;
}
goto screen_refreshed;
}

if (qbg_bytes_per_pixel==1){
static uint8 *cp;
static uint32 *lp;
cp=pixeldata;
lp=pixel+qbg_y_offset*qbg_width;
i2=qbg_width*qbg_height;

for (i=0;i<i2;i++){
*lp=pal[*cp];
cp++;
lp++;
}


}
*/

//screen_refreshed:
//SDL_UpdateRect(screen, 0, 0, 0, 0);
//SDL_UpdateRect(screen_surface,0,0,0,0);

//update display surface
//memcpy(display_page_surface->pixels,display_page->offset,1024*768*4);



//SDL_UpdateRect(display_page_surface,0,0,0,0);
screen_refresh_unrequired:;


}//lock_display==0

if (lock_display==1){lock_display=2; Sleep(0);}

if (autodisplay==-1) autodisplay=0;

}





/*
int message_loop(
	SDL_Event event,
	SDL_Surface *screen,
	SDL_Surface *back,
	int *inputedWidth,
	Uint16 *inputedString,
	TTF_Font *font)
{
	SDL_Color fg = {0x66, 0x66, 0xFF};
	SDL_Color bg = {0x00, 0x00, 0x00};
	SDL_Rect rect;
	SDL_Surface *surface;
	SDL_Event eventExpose;
	
	switch(event.type){
		case SDL_QUIT:
			return 1;
		case SDL_VIDEOEXPOSE:
			SDL_BlitSurface(back, NULL, screen, NULL);
			SDL_UpdateRect(screen, 0, 0, 0, 0);
			break;
		case SDL_KEYDOWN:


			


			if (event.key.keysym.sym == SDLK_F1) {
				InputMethod_Reset();
			}
			if (event.key.keysym.sym == SDLK_F2) {
				*inputedWidth = 0;
				inputedString[0] = 0x0000;
				InputMethod_Reset();
			}
			if (event.key.keysym.sym == SDLK_F3) {
				InputMethod_Validate();
				rect.x = 0;
				rect.y = 200;
				rect.w = 640;
				rect.h = 100;
				SDL_FillRect(back, &rect, 0x00000000);
				surface = TTF_RenderUTF8_Shaded(
					font, "Valid", fg, bg);
				SDL_BlitSurface(surface, NULL, back, &rect);
				SDL_FreeSurface(surface);
				eventExpose.type = SDL_VIDEOEXPOSE;
				SDL_PushEvent(&eventExpose);
			}
			if (event.key.keysym.sym == SDLK_F4) {
				InputMethod_Invalidate();
				rect.x = 0;
				rect.y = 200;
				rect.w = 640;
				rect.h = 100;
				SDL_FillRect(back, &rect, 0x00000000);
				surface = TTF_RenderUTF8_Shaded(
					font, "Invalid", fg, bg);
				SDL_BlitSurface(surface, NULL, back, &rect);
				SDL_FreeSurface(surface);
				eventExpose.type = SDL_VIDEOEXPOSE;
				SDL_PushEvent(&eventExpose);
			}
			break;
		default:
			break;
	}
	return 0;
}
*/

void update_shift_state(){
int32 x;
/*
0:417h                   Shift Status
               7 6 5 4 3 2 1 0
               x . . . . . . .      Insert locked
               . x . . . . . .      Caps Lock locked
               . . x . . . . .      Num Lock locked
               . . . x . . . .      Scroll Lock locked
               . . . . x . . .      Alt key is pressed
               . . . . . x . .      Ctrl key is pressed
               . . . . . . x .      Left Shift key is pressed
               . . . . . . . x      Right Shift key is pressed
*/
x=0;
if (keyheld(VK+SDLK_RSHIFT)) x|=1;
if (keyheld(VK+SDLK_LSHIFT)) x|=2;
if (keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL)) x|=4;
if (keyheld(VK+SDLK_LALT)||keyheld(VK+SDLK_RALT)) x|=8;
if (keyheld(QBK+QBK_SCROLL_LOCK_MODE)) x|=16;
if (keyheld(VK+SDLK_NUMLOCK)) x|=32;
if (keyheld(VK+SDLK_CAPSLOCK)) x|=64;
//note: insert state is emulated (off by default)
if (keyheld(QBK+QBK_INSERT_MODE)) x|=128;
cmem[0x417]=x;
/*
0:418h                   Extended Shift Status
 (interpret the word 'pressed' as "being held down")
               7 6 5 4 3 2 1 0
               x . . . . . . .      Ins key is pressed
               . x . . . . . .      Caps Lock key is pressed (detection not possible, return 0)
               . . x . . . . .      Num Lock key is pressed (detection not possible, return 0)
               . . . x . . . .      Scroll Lock key is pressed
               . . . . x . . .      Pause key locked
               . . . . . x . .      SysReq key is pressed
               . . . . . . x .      Left Alt key is pressed
               . . . . . . . x      Left Ctrl key is pressed
*/
x=0;
if (keyheld(VK+SDLK_LCTRL)) x|=1;
if (keyheld(VK+SDLK_LALT)) x|=2;
if (keyheld(VK+SDLK_SYSREQ)) x|=4;
if (keyheld(VK+SDLK_PAUSE)) x|=8;
if (keyheld(VK+SDLK_SCROLLOCK)) x|=16;
//if (keyheld(VK+SDLK_NUMLOCK)) x|=32;
//if (keyheld(VK+SDLK_CAPSLOCK)) x|=64;
if (keyheld(0x5200)) x|=128;
cmem[0x418]=x;
/*
0:496h                   Keyboard Status and Type Flags
    This byte holds keyboard status information.
                      Keyboard Status Information
            7 6 5 4 3 2 1 0
            x . . . . . . .       Read ID in progress (always 0)
            . x . . . . . .       Last character was first ID character (always 0)
            . . x . . . . .       Force Num Lock if read ID and KBX (always 0)
            . . . x . . . .       101/102-key keyboard installed (always 1)
            . . . . x . . .       Right Alt key is pressed
            . . . . . x . .       Right Ctrl key is pressed
            . . . . . . x .       Last code was E0 Hidden Code (always 0)
            . . . . . . . x       last code was E1 Hidden Code (always 0)
*/
x=0;
if (keyheld(VK+SDLK_RCTRL)) x|=1;
if (keyheld(VK+SDLK_RALT)) x|=2;
x|=16;
cmem[0x496]=x;
}


int32 keyup_mask_last=-1;
uint32 keyup_mask[256];//NULL values indicate removed masks

void keyup(uint32 x){

if (!x) x=QBK+QBK_CHR0;

keyheld_remove(x);

if (asciicode_reading!=2){//hide numpad presses related to ALT+1+2+3 type entries
//identify and revert numpad specific key codes to non-numpad codes
static uint32 x2;
static int64 numpadkey;
numpadkey=0;
x2=x;
//check multimapped NUMPAD keys
if ((x>=(VK+SDLK_KP0))&&(x<=(VK+SDLK_KP_ENTER))){
numpadkey=4294967296ll;
if ((x>=(VK+SDLK_KP0))&&(x<=(VK+SDLK_KP9))){x2=x-(VK+SDLK_KP0)+48; goto onnumpad;}
if (x==(VK+SDLK_KP_PERIOD)){x2=46; goto onnumpad;}
if (x==(VK+SDLK_KP_DIVIDE)){x2=47; goto onnumpad;}
if (x==(VK+SDLK_KP_MULTIPLY)){x2=42; goto onnumpad;}
if (x==(VK+SDLK_KP_MINUS)){x2=45; goto onnumpad;}
if (x==(VK+SDLK_KP_PLUS)){x2=43; goto onnumpad;}
if (x==(VK+SDLK_KP_ENTER)){x2=13; goto onnumpad;}
}
if ((x>=(QBK+0))&&(x<=(QBK+10))){
numpadkey=4294967296ll;
x2=x-QBK;
if (x2==0){x2=82<<8; goto onnumpad;}
if (x2==1){x2=79<<8; goto onnumpad;}
if (x2==2){x2=80<<8; goto onnumpad;}
if (x2==3){x2=81<<8; goto onnumpad;}
if (x2==4){x2=75<<8; goto onnumpad;}
if (x2==5){x2=76<<8; goto onnumpad;}
if (x2==6){x2=77<<8; goto onnumpad;}
if (x2==7){x2=71<<8; goto onnumpad;}
if (x2==8){x2=72<<8; goto onnumpad;}
if (x2==9){x2=73<<8; goto onnumpad;}
if (x2==10){x2=83<<8; goto onnumpad;}
}
onnumpad:;

static int32 i;
for (i=0;i<=keyup_mask_last;i++){
 if (x==keyup_mask[i]){
  keyup_mask[i]=0;
  goto key_handled;
 }
}

//add x2 to keyhit buffer
static int32 z;
z=(keyhit_nextfree+1)&0x1FFF;
if (z==keyhit_next){//remove oldest message when cyclic buffer is full
keyhit_next=(keyhit_next+1)&0x1FFF;
}
static int32 sx;
sx=x2; sx=-sx; x2=sx;//negate x2
keyhit[keyhit_nextfree]=x2|numpadkey;
keyhit_nextfree=z;
}//asciicode_reading!=2


static int32 shift,alt,ctrl,capslock,numlock;
numlock=0; capslock=0;

if (x<=255){
if (scancode_lookup[x*10+2]) scancodeup(scancode_lookup[x*10+1]);
goto key_handled;
}//x<=255

//NUMPAD?
if ((x>=(VK+SDLK_KP0))&&(x<=(VK+SDLK_KP_ENTER))){
if ((x>=(VK+SDLK_KP0))&&(x<=(VK+SDLK_KP_PERIOD))) numlock=1;
x=(x-(VK+SDLK_KP0)+256)*256;
goto numpadkey;
}
if ((x>=(QBK+0))&&(x<=(QBK+0+(SDLK_KP_PERIOD-SDLK_KP0)))){
x=(x-(QBK+0)+256)*256;
goto numpadkey;
}

if (x<=65535){
static int32 r;
numpadkey:
r=(x>>8)+256;
if (scancode_lookup[r*10+2]) scancodeup(scancode_lookup[r*10+1]);

if (x==0x5200){//INSERT lock emulation
update_shift_state();
}

goto key_handled;
}//x<=65536

if (x==(VK+SDLK_LSHIFT)){
scancodeup(42);
update_shift_state();
}
if (x==(VK+SDLK_RSHIFT)){
scancodeup(54);
update_shift_state();
}
if (x==(VK+SDLK_LALT)){
scancodeup(56);
update_shift_state();
}
if (x==(VK+SDLK_RALT)){
scancodeup(56);
update_shift_state();
}
if (x==(VK+SDLK_LCTRL)){
scancodeup(29);
update_shift_state();
}
if (x==(VK+SDLK_RCTRL)){
scancodeup(29);
update_shift_state();
}
if (x==(VK+SDLK_NUMLOCK)){
scancodeup(69);
update_shift_state();
}
if (x==(VK+SDLK_CAPSLOCK)){
scancodeup(58);
update_shift_state();
}
if (x==(VK+SDLK_SCROLLOCK)){
scancodeup(70);
update_shift_state();
}

key_handled:;

}

void keydown(uint32 x){

if (!x) x=QBK+QBK_CHR0;

static int32 glyph;
glyph=keydown_glyph; keydown_glyph=0;

//INSERT lock emulation
static int32 insert_held;
if (x==0x5200) insert_held=keyheld(0x5200);

//SCROLL lock tracking
static int32 scroll_lock_held;
if (x==(VK+SDLK_SCROLLOCK)) scroll_lock_held=keyheld(VK+SDLK_SCROLLOCK);

keyheld_add(x);

//note: On early keyboards without a Pause key (before the introduction of 101-key keyboards) the Pause function was assigned to Ctrl+NumLock, and the Break function to Ctrl+ScrLock; these key-combinations still work with most programs, even on modern PCs with modern keyboards.
//CTRL+BREAK handling
if (
(x==(VK+SDLK_BREAK))
|| ((x==(VK+SDLK_SCROLLOCK))&&(keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL))) 
|| ((x==(VK+SDLK_F15))&&(keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL))) 
){
if (exit_blocked){exit_value|=2; goto key_handled;}
close_program=1;
goto key_handled;
}

//note: On early keyboards without a Pause key (before the introduction of 101-key keyboards) the Pause function was assigned to Ctrl+NumLock, and the Break function to Ctrl+ScrLock; these key-combinations still work with most programs, even on modern PCs with modern keyboards.
//PAUSE handling
if ( (x==(VK+SDLK_PAUSE)) || ((x==(VK+SDLK_NUMLOCK))&&(keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL))) ){
suspend_program|=1;
qbevent=1;
goto key_handled;
}else{
 if (suspend_program&1){
 suspend_program^=1;
 goto key_handled;
 }
}

//ALT+ENTER
if (keyheld(VK+SDLK_RALT)||keyheld(VK+SDLK_LALT)){
if (x==13){
full_screen_toggle++;
goto key_handled;
}
}

if (asciicode_reading!=2){//hide numpad presses related to ALT+1+2+3 type entries

//identify and revert numpad specific key codes to non-numpad codes
static uint32 x2;
static int64 numpadkey;
numpadkey=0;
x2=x;
//check multimapped NUMPAD keys
if ((x>=(VK+SDLK_KP0))&&(x<=(VK+SDLK_KP_ENTER))){
numpadkey=4294967296ll;
if ((x>=(VK+SDLK_KP0))&&(x<=(VK+SDLK_KP9))){x2=x-(VK+SDLK_KP0)+48; goto onnumpad;}
if (x==(VK+SDLK_KP_PERIOD)){x2=46; goto onnumpad;}
if (x==(VK+SDLK_KP_DIVIDE)){x2=47; goto onnumpad;}
if (x==(VK+SDLK_KP_MULTIPLY)){x2=42; goto onnumpad;}
if (x==(VK+SDLK_KP_MINUS)){x2=45; goto onnumpad;}
if (x==(VK+SDLK_KP_PLUS)){x2=43; goto onnumpad;}
if (x==(VK+SDLK_KP_ENTER)){x2=13; goto onnumpad;}
}
if ((x>=(QBK+0))&&(x<=(QBK+10))){
numpadkey=4294967296ll;
x2=x-QBK;
if (x2==0){x2=82<<8; goto onnumpad;}
if (x2==1){x2=79<<8; goto onnumpad;}
if (x2==2){x2=80<<8; goto onnumpad;}
if (x2==3){x2=81<<8; goto onnumpad;}
if (x2==4){x2=75<<8; goto onnumpad;}
if (x2==5){x2=76<<8; goto onnumpad;}
if (x2==6){x2=77<<8; goto onnumpad;}
if (x2==7){x2=71<<8; goto onnumpad;}
if (x2==8){x2=72<<8; goto onnumpad;}
if (x2==9){x2=73<<8; goto onnumpad;}
if (x2==10){x2=83<<8; goto onnumpad;}
}
onnumpad:;





//ON KEY trapping
{//new scope
static int32 block_onkey=0;
static int32 f,x3,scancode,extended,c,flags_mask;
int32 i,i2;//must not be static!

//establish scancode (if any)
scancode=0;
if (x<=255){scancode=scancode_lookup[x*10+1]; goto onkey_gotscancode;}
//*check for 2 byte scancodes here
x3=x;
if ((x3>=(VK+SDLK_KP0))&&(x3<=(VK+SDLK_KP_ENTER))){
x3=(x3-(VK+SDLK_KP0)+256)*256;
goto onkey_numpadkey;
}
if ((x3>=(QBK+0))&&(x3<=(QBK+0+(SDLK_KP_PERIOD-SDLK_KP0)))){
x3=(x3-(QBK+0)+256)*256;
goto onkey_numpadkey;
}
if (x3<=65535){
onkey_numpadkey:
i=(x3>>8)+256;
if (scancode_lookup[i*10+2]) scancode=scancode_lookup[i*10+1];
}
onkey_gotscancode:

//check modifier keys
if (x==(VK+SDLK_LSHIFT)){
scancode=42;
flags_mask=3;
}
if (x==(VK+SDLK_RSHIFT)){
scancode=54;
flags_mask=3;
}
if (x==(VK+SDLK_LALT)){
scancode=56;
flags_mask=8;
}
if (x==(VK+SDLK_RALT)){
scancode=56;
flags_mask=8;
}
if (x==(VK+SDLK_LCTRL)){
scancode=29;
flags_mask=4;
}
if (x==(VK+SDLK_RCTRL)){
scancode=29;
flags_mask=4;
}
if (x==(VK+SDLK_NUMLOCK)){
scancode=69;
flags_mask=32;
}
if (x==(VK+SDLK_CAPSLOCK)){
scancode=58;
flags_mask=64;
}
if (x==(VK+SDLK_SCROLLOCK)){
scancode=70;
//note: no mask required
}

//establish if key is an extended key
extended=0;
//arrow-pad (note: num-pad is ignored because x is a QB64 pure key value and only refers to the arrow-pad)
if (x==0x4B00)extended=1;
if (x==0x4800)extended=1;
if (x==0x4D00)extended=1;
if (x==0x5000)extended=1;
//num-pad extended keys
if (x==VK+SDLK_KP_DIVIDE)extended=1;
if (x==VK+SDLK_KP_ENTER)extended=1;
//ins/del/hom/end/pgu/pgd pad
if (x==0x5200)extended=1;
if (x==0x4700)extended=1;
if (x==0x4900)extended=1;
if (x==0x5300)extended=1;
if (x==0x4F00)extended=1;
if (x==0x5100)extended=1;
//right alt/right control
if (x==VK+SDLK_RCTRL)extended=1;
if (x==VK+SDLK_RALT)extended=1;

if (!block_onkey){

//priority #1: user defined keys
if (scancode){
for (i=0;i<=31;i++){
if (onkey[i].key_scancode==scancode){
if (onkey[i].active){
if (onkey[i].id){
//check keyboard flags
f=onkey[i].key_flags;
//0 No keyboard flag, 1-3 Either Shift key, 4 Ctrl key, 8 Alt key,32 NumLock key,64 Caps Lock key, 128 Extended keys on a 101-key keyboard
//To specify multiple shift states, add the values together. For example, a value of 12 specifies that the user-defined key is used in combination with the Ctrl and Alt keys.
if ((flags_mask&3)==0){
if (f&3){
if (keyheld(VK+SDLK_LSHIFT)==0&&keyheld(VK+SDLK_RSHIFT)==0) goto wrong_flags;
}else{
if (keyheld(VK+SDLK_LSHIFT)||keyheld(VK+SDLK_RSHIFT)) goto wrong_flags;
}
}
if ((flags_mask&4)==0){
if (f&4){
if (keyheld(VK+SDLK_LCTRL)==0&&keyheld(VK+SDLK_RCTRL)==0) goto wrong_flags;
}else{
if (keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL)) goto wrong_flags;
}
}
if ((flags_mask&8)==0){
if (f&8){
if (keyheld(VK+SDLK_LALT)==0&&keyheld(VK+SDLK_RALT)==0) goto wrong_flags;
}else{
if (keyheld(VK+SDLK_LALT)||keyheld(VK+SDLK_RALT)) goto wrong_flags;
}
}
if ((flags_mask&32)==0){
if (f&32){
if (keyheld(VK+SDLK_NUMLOCK)==0) goto wrong_flags;
//*revise
}
}
if ((flags_mask&64)==0){
if (f&64){
if (keyheld(VK+SDLK_CAPSLOCK)==0) goto wrong_flags;
//*revise
}
}
if ((flags_mask&128)==0){
if (((f&128)/128)!=extended) goto wrong_flags;
}
if (onkey[i].active==1){//(1)ON
onkey[i].state++;
}else{//(2)STOP
onkey[i].state=1;
}

qbevent=1;

//mask trigger key
for (i=0;i<=keyup_mask_last;i++){
 if (!keyup_mask[i]){
  keyup_mask[i]=x;  
  break;
 }
}
if (i==keyup_mask_last+1){
if (keyup_mask_last<255){
keyup_mask[i]=x;
keyup_mask_last++;
}
}

goto key_handled;

}//id
}//active
}//scancode==
wrong_flags:;
}//i
}//scancode

//priority #2: fixed index F1-F12, arrows
for (i=0;i<=31;i++){
if (onkey[i].active){
if (onkey[i].id){
if ((x2==onkey[i].keycode)||x==onkey[i].keycode_alternate){
if (onkey[i].active==1){//(1)ON
onkey[i].state++;
}else{//(2)STOP
onkey[i].state=1;
}
qbevent=1;

//mask trigger key
for (i=0;i<=keyup_mask_last;i++){
 if (!keyup_mask[i]){
  keyup_mask[i]=x;  
  break;
 }
}
if (i==keyup_mask_last+1){
if (keyup_mask_last<255){
keyup_mask[i]=x;
keyup_mask_last++;
}
}

goto key_handled;
}//keycode
}//id
}//active
}//i

}//block_onkey

//priority #3: string insertion
for (i=0;i<=31;i++){
if (onkey[i].text){
if (onkey[i].text->len){
if ((x2==onkey[i].keycode)||x==onkey[i].keycode_alternate){

//mask trigger key
{//scope
static int32 i;
for (i=0;i<=keyup_mask_last;i++){
 if (!keyup_mask[i]){
  keyup_mask[i]=x;  
  break;
 }
}
if (i==keyup_mask_last+1){
if (keyup_mask_last<255){
keyup_mask[i]=x;
keyup_mask_last++;
}
}
}//descope

for (i2=0;i2<onkey[i].text->len;i2++){
 block_onkey=1;
 keydown_ascii(onkey[i].text->chr[i2]);
 keyup_ascii(onkey[i].text->chr[i2]);
 block_onkey=0;
}//i2
goto key_handled;
}//keycode
}}//text
}//i

}//descope


/*
//keyhit cyclic buffer
int64 keyhit[8192];
//    keyhit specific internal flags: (stored in high 32-bits)
//    &4294967296->numpad was used
int32 keyhit_nextfree=0;
int32 keyhit_next=0;
//note: if full, the oldest message is discarded to make way for the new message
*/
//add x2 to keyhit buffer
static int32 z;
z=(keyhit_nextfree+1)&0x1FFF;
if (z==keyhit_next){//remove oldest message when cyclic buffer is full
keyhit_next=(keyhit_next+1)&0x1FFF;
}
keyhit[keyhit_nextfree]=x2|numpadkey;
keyhit_nextfree=z;
}//asciicode_reading!=2


static int32 shift,alt,ctrl,capslock,numlock;
numlock=0; capslock=0;

if (x==QBK+QBK_CHR0) x=0;

if (x<=255){
static int32 b1,b2,z,o;
b1=x;
if (b2=scancode_lookup[x*10+1]){//table entry exists
scancodedown(b2);







//check for relevent table modifiers
shift=0; if (keyheld(VK+SDLK_LSHIFT)||keyheld(VK+SDLK_RSHIFT)) shift=1;
ctrl=0; if (keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL)) ctrl=1;
alt=0; if (keyheld(VK+SDLK_LALT)||keyheld(VK+SDLK_RALT)) alt=1;
o=0;
if (shift) o=1;
if (ctrl) o=2;
if (alt) o=3;
if (glyph){
if ((keyheld(VK+SDLK_LALT)==0)&&(keyheld(VK+SDLK_RCTRL)==0)&&keyheld(VK+SDLK_LCTRL)&&keyheld(VK+SDLK_RALT)) o=0;//assume alt-gr combo-key
}
z=scancode_lookup[x*10+2+o];
if (!z) goto key_handled;//not possible
if (z&0xFF00){
b1=0;
b2=z>>8;
}else{
b1=z;
}
}//b2
static int32 i,i2,i3;
i=cmem[0x41a];
i2=cmem[0x41c];
i3=i2+2;
if (i3==62) i3=30;
if (i!=i3){//fits in buffer
cmem[0x400+i2]=b1;
cmem[0x400+i2+1]=b2;//(scancode)
cmem[0x41c]=i3;//fix tail location
}
goto key_handled;
}//x<=255

//NUMPAD?
if ((x>=(VK+SDLK_KP0))&&(x<=(VK+SDLK_KP_ENTER))){
if ((x>=(VK+SDLK_KP0))&&(x<=(VK+SDLK_KP_PERIOD))) numlock=1;
x=(x-(VK+SDLK_KP0)+256)*256;
goto numpadkey;
}
if ((x>=(QBK+0))&&(x<=(QBK+0+(SDLK_KP_PERIOD-SDLK_KP0)))){
x=(x-(QBK+0)+256)*256;
goto numpadkey;
}

if (x<=65535){
static int32 b1,b2,z,o,r;
numpadkey:
b1=0; b2=x>>8;
r=(x>>8)+256;
if (scancode_lookup[r*10+2]){
scancodedown(scancode_lookup[r*10+1]);
//check relevent modifiers
shift=0; if (keyheld(VK+SDLK_LSHIFT)||keyheld(VK+SDLK_RSHIFT)) shift=1;
ctrl=0; if (keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL)) ctrl=1;
alt=0; if (keyheld(VK+SDLK_LALT)||keyheld(VK+SDLK_RALT)) alt=1;

if (x==0x5200){//INSERT lock emulation
if (insert_held==0){//nullify effects of key repeats
if ((alt==0)&&(shift==0)&&(ctrl==0)){
//toggle insert mode
if (keyheld(QBK+QBK_INSERT_MODE)){
keyheld_remove(QBK+QBK_INSERT_MODE);
}else{
keyheld_add(QBK+QBK_INSERT_MODE);
}
update_shift_state();
}
}
}

o=0;
if (shift) o=1;
if (numlock) o=4;
if (numlock&&shift) o=7;
if (ctrl) o=2;
if (alt) o=3;
z=scancode_lookup[r*10+2+o];
if (!z) goto key_handled;//invalid combination
if (z&0xFF00){
b1=0;
b2=z>>8;
}else{
b1=z;
b2=scancode_lookup[r*10+1];
}
}//z
static int32 i,i2,i3;
i=cmem[0x41a];
i2=cmem[0x41c];
i3=i2+2;
if (i3==62) i3=30;
if (i!=i3){//fits in buffer
cmem[0x400+i2]=b1;
cmem[0x400+i2+1]=b2;//(scancode)
cmem[0x41c]=i3;//fix tail location
}
goto key_handled;
}//x<=65536

if (x==(VK+SDLK_LSHIFT)){
scancodedown(42);
update_shift_state();
}
if (x==(VK+SDLK_RSHIFT)){
scancodedown(54);
update_shift_state();
}
if (x==(VK+SDLK_LALT)){
scancodedown(56);
update_shift_state();
}
if (x==(VK+SDLK_RALT)){
scancodedown(56);
update_shift_state();
}
if (x==(VK+SDLK_LCTRL)){
scancodedown(29);
update_shift_state();
}
if (x==(VK+SDLK_RCTRL)){
scancodedown(29);
update_shift_state();
}
if (x==(VK+SDLK_NUMLOCK)){
scancodedown(69);
update_shift_state();
}
if (x==(VK+SDLK_CAPSLOCK)){
scancodedown(58);
update_shift_state();
}
if (x==(VK+SDLK_SCROLLOCK)){
scancodedown(70);

if (scroll_lock_held==0){//nullify effects of key repeats
ctrl=0; if (keyheld(VK+SDLK_LCTRL)||keyheld(VK+SDLK_RCTRL)) ctrl=1;
if (ctrl==0){
//toggle insert mode
if (keyheld(QBK+QBK_SCROLL_LOCK_MODE)){
keyheld_remove(QBK+QBK_SCROLL_LOCK_MODE);
}else{
keyheld_add(QBK+QBK_SCROLL_LOCK_MODE);
}
}
}

update_shift_state();
}

key_handled:
sleep_break=1;

}

void scancodedown(uint8 scancode){
if (port60h_events){
if (port60h_event[port60h_events-1]==scancode) return;//avoid duplicate entries in buffer (eg. from key-repeats)
}
if (port60h_events==256){memmove(port60h_event,port60h_event+1,255); port60h_events=255;}
port60h_event[port60h_events]=scancode;
port60h_events++;
}

void scancodeup(uint8 scancode){
if (port60h_events){
if (port60h_event[port60h_events-1]==(scancode+128)) return;//avoid duplicate entries in buffer
}
if (port60h_events==256){memmove(port60h_event,port60h_event+1,255); port60h_events=255;}
port60h_event[port60h_events]=scancode+128;
port60h_events++;
}
