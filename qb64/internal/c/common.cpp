//7/7/2010: modified for 64-bit variable type compatibility

//OS/compiler detection
#ifdef WIN32
 #include "stdafx2.h"
 #include <winbase.h>
 #define QB64_WINDOWS
 #ifdef _MSC_VER
  #define QB64_MICROSOFT
 #else
  #define QB64_GCC
  #define QB64_MINGW
 #endif
#else
 #define QB64_LINUX
 #define QB64_GCC
 #ifdef __APPLE__
  #define QB64_MACOSX
 #endif
#endif

//common includes
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_thread.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_net.h>

//OS/compiler specific includes
#ifndef QB64_MACOSX
 #include <SDL_syswm.h>
#endif
#ifdef QB64_WINDOWS
 #include <direct.h>
 #include <winspool.h>
#else
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <sys/wait.h>
 #include <unistd.h>
 #include <stdint.h> 
 #include <dlfcn.h>
#endif

using namespace std;

//common types
#ifdef QB64_WINDOWS
 #define uint64 unsigned __int64
 #define uint32 unsigned __int32
 #define uint16 unsigned __int16
 #define uint8 unsigned __int8
 #define int64 __int64
 #define int32 __int32
 #define int16 __int16
 #define int8 __int8
#else
 #define int64 int64_t
 #define int32 int32_t
 #define int16 int16_t
 #define int8 int8_t
 #define uint64 uint64_t
 #define uint32 uint32_t
 #define uint16 uint16_t
 #define uint8 uint8_t
#endif
// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define QB64_64
#else
#define QB64_32
#endif
#endif
// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define QB64_64
#else
#define QB64_32
#endif
#endif
#ifdef QB64_64
 #define ptrszint int64
 #define uptrszint uint64
 #define ptrsz 8
#else
 #define ptrszint int32
 #define uptrszint uint32
 #define ptrsz 4
#endif

//QB64 string descriptor structure
struct qbs_field{
int32 fileno;
int64 fileid;
int64 size;
int64 offset;
};
struct qbs{
uint8 *chr;//a 32 bit pointer to the string's data
uint32 len;
uint8 in_cmem;//set to 1 if in the conventional memory DBLOCK
uint16 *cmem_descriptor;
uint16 cmem_descriptor_offset;
uint32 listi;//the index in the list of strings that references it
uint8 tmp;//set to 1 if the string can be deleted immediately after being processed
uint32 tmplisti;//the index in the list of strings that references it
uint8 fixed;//fixed length string
uint8 readonly;//set to 1 if string is read only
qbs_field *field;
};

//substitute functionality
#ifdef QB64_WINDOWS
 inline void SDL_Delay(Uint32 milliseconds){//redefine SDL_Delay to call Sleep
 Sleep(milliseconds);
 }
#else
 inline void Sleep(uint32 milliseconds){
 SDL_Delay(milliseconds);
 }
 inline uint32 _lrotl(uint32 word,uint32 shift){
 return (word << shift) | (word >> (32 - shift));
 }
 inline void ZeroMemory(void *ptr,int64 bytes){
 memset(ptr,0,bytes);
 }
#endif

struct img_struct{
uint8 valid;//0,1 0=invalid
uint8 text;//if set, surface is a text surface
uint16 width,height;
uint8 bytes_per_pixel;//1,2,4
uint8 bits_per_pixel;//1,2,4,8,16(text),32
uint32 mask;//1,3,0xF,0xFF,0xFFFF,0xFFFFFFFF
uint16 compatible_mode;//0,1,2,7,8,9,10,11,12,13,32,256
uint32 color,background_color,draw_color;
uint32 font;//8,14,16,?
int16 top_row,bottom_row;//VIEW PRINT settings, unique (as in QB) to each "page"
int16 cursor_x,cursor_y;//unique (as in QB) to each "page"
uint8 cursor_show, cursor_firstvalue, cursor_lastvalue;
union{
uint8 *offset;
uint32 *offset32;
};
uint32 flags;
uint32 *pal;
int32 transparent_color;//-1 means no color is transparent
uint8 alpha_disabled;
uint8 holding_cursor;
uint8 print_mode;
//BEGIN apm ('active page migration')
//everything between apm points is migrated during active page changes
//note: apm data is only relevent to graphics modes
uint8 apm_p1;
int32 view_x1,view_y1,view_x2,view_y2;
int32 view_offset_x,view_offset_y;
float x,y;
uint8 clipping_or_scaling;
float scaling_x,scaling_y,scaling_offset_x,scaling_offset_y;
float window_x1,window_y1,window_x2,window_y2;
double draw_ta;
double draw_scale;
uint8 apm_p2;
//END apm
};
//img_struct flags
#define IMG_FREEPAL 1 //free palette data before freeing image
#define IMG_SCREEN 2 //img is linked to other screen pages
#define IMG_FREEMEM 4 //if set, it means memory must be freed

//QBASIC compatible rounding via FPU:
#ifdef QB64_MICROSOFT
	inline int64 qbr(long double f){
		static int64 i;
		__asm{
		fld   f
		fistp i
		}
		return i;
	}
	inline uint64 qbr_longdouble_to_uint64(long double f){
		static uint64 i;
		__asm{
		fld   f
		fistp i
		}
		return i;
	}
	inline int32 qbr_float_to_long(float f){
		static int32 i;
		__asm{
		fld   f
		fistp i
		}
		return i;
	}
	inline int32 qbr_double_to_long(double f){
		static int32 i;
		__asm{
		fld   f
		fistp i
		}
		return i;
	}
#else
    //FLDS=load single
    //FLDL=load double
    //FLDT=load long double
	inline int64 qbr(long double f){
	static int64 i;
  	__asm__ (
		"fldt %1;"
		"fistpll %0;"              
		:"=m" (i)
		:"m" (f)
		);
		return i;
	}
	inline uint64 qbr_longdouble_to_uint64(long double f){
		static uint64 i;
		__asm__ (
		"fldt %1;"
		"fistpll %0;"              
		:"=m" (i)
		:"m" (f)
		);
		return i;
	}
	inline int32 qbr_float_to_long(float f){
		static int32 i;
		__asm__ (
		"flds %1;"
		"fistpl %0;"              
		:"=m" (i)
		:"m" (f)
		);
		return i;
	}
	inline int32 qbr_double_to_long(double f){
		static int32 i;
		__asm__ (
		"fldl %1;"
		"fistpl %0;"              
		:"=m" (i)
		:"m" (f)
		);
		return i;
	}
#endif
/* Alternate rounding routines (may have to be used on non-x86 based processors)
    inline int64 qbr(long double f){if (f<0) return(f-0.5f); else return(f+0.5f);}
    inline uint64 qbr_longdouble_to_uint64(long double f){if (f<0) return(f-0.5f); else return(f+0.5f);}
    inline int32 qbr_float_to_long(float f){if (f<0) return(f-0.5f); else return(f+0.5f);}
    inline int32 qbr_double_to_long(double f){if (f<0) return(f-0.5f); else return(f+0.5f);}
*/

//bit-array access functions (note: used to be included through 'bit.cpp')
static int64 bmask;
static uint64 *bptr64;
static int64 bval64;
inline uint64 getubits(uint32 bsize,uint8 *base,ptrszint i){
bmask=~(-(((int64)1)<<bsize));
i*=bsize;
return ((*(uint64*)(base+(i>>3)))>>(i&7))&bmask;
}
inline int64 getbits(uint32 bsize,uint8 *base,ptrszint i){
bmask=~(-(((int64)1)<<bsize));
i*=bsize;
bval64=((*(uint64*)(base+(i>>3)))>>(i&7))&bmask;
if (bval64&(((int64)1)<<(bsize-1))) return bval64|(~bmask);
return bval64;
}
inline void setbits(uint32 bsize,uint8 *base,ptrszint i,int64 val){
bmask=(((uint64)1)<<bsize)-1;
i*=bsize;
bptr64=(uint64*)(base+(i>>3));
*bptr64=(*bptr64&( ( (bmask<<(i&7)) ^-1)  )) | ((val&bmask)<<(i&7));
}

//QB64 internal variable type flags (internally referenced by some C functions)
#define ISSTRING 1073741824
#define ISFLOAT 536870912
#define ISUNSIGNED 268435456
#define ISPOINTER 134217728
#define ISFIXEDLENGTH 67108864 //only set for strings with pointer flag
#define ISINCONVENTIONALMEMORY 33554432
#define ISOFFSETINBITS 16777216

struct ontimer_struct{
uint8 allocated;
uint32 id;//the event ID to trigger (0=no event)
int64 pass;//the value to pass to the triggered event (only applicable to ON ... CALL ...(x)
uint8 active;//0=OFF, 1=ON, 2=STOP
uint8 state;//0=untriggered,1=triggered
double seconds;//how many seconds between events
double last_time;//the last time this event was triggered
};

struct onkey_struct{
uint32 id;//the event ID to trigger (0=no event)
int64 pass;//the value to pass to the triggered event (only applicable to ON ... CALL ...(x)
uint8 active;//0=OFF, 1=ON, 2=STOP
uint8 state;//0=untriggered,1=triggered,2=in progress(TIMER only),2+=multiple events queued(KEY only)
uint32 keycode;//32-bit code, same as what _KEYHIT returns
uint32 keycode_alternate;//an alternate keycode which may also trigger event
uint8 key_scancode;
uint8 key_flags;
//flags:
//0 No keyboard flag, 1-3 Either Shift key, 4 Ctrl key, 8 Alt key,32 NumLock key,64 Caps Lock key, 128 Extended keys on a 101-key keyboard
//To specify multiple shift states, add the values together. For example, a value of 12 specifies that the user-defined key is used in combination with the Ctrl and Alt keys.
qbs *text;
};

struct onstrig_struct{
uint32 id;//the event ID to trigger (0=no event)
int64 pass;//the value to pass to the triggered event (only applicable to ON ... CALL ...(x)
uint8 active;//0=OFF, 1=ON, 2=STOP
uint8 state;//0=untriggered,1=triggered,2=in progress(TIMER only),2+=multiple events queued(KEY only)
};

struct byte_element_struct
{
uint64 offset;
int32 length;
};

struct device_struct{
int32 used;
int32 type;
//0=Unallocated
//1=SDL joystick interface
//2=?
qbs *name;
int32 connected;
int32 lastbutton;
int32 lastaxis;
int32 lastwheel;
//--------------
int32 max_events;
int32 queued_events;
uint8 *events;//the structure and size of the events depends greatly on the device and its capabilities
int32 event_size;
//--------------
uint8 STRIG_button_pressed[256];//checked and cleared by the STRIG function
//--------------
uint8 id[1024];
SDL_Joystick *SDL_js;
int32 SDL_js_index;
int32 SDL_buttons;
int32 SDL_axes;
int32 SDL_balls;
int32 SDL_hats;
};
#define QUEUED_EVENTS_LIMIT 1024
