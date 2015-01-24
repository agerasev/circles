#pragma once

/* Event types */
#define MEDIA_QUIT              0x01

#define MEDIA_INIT_SURFACE      0x02
#define MEDIA_TERM_SURFACE      0x03
#define MEDIA_RESIZE_SURFACE    0x04

#define MEDIA_SHOWN             0x05
#define MEDIA_HIDDEN            0x06

#define MEDIA_SAVE_STATE        0x08

#define MEDIA_MOTION            0x10

/* Actions */
#define MEDIA_ACTION_UP         0x01
#define MEDIA_ACTION_DOWN       0x02
#define MEDIA_ACTION_MOVE       0x03

typedef struct Media_IVec2
{
	int x, y;
} 
Media_IVec2;

typedef struct Media_FVec3
{
	float x, y, z;
} 
Media_FVec3;

typedef struct Media_Event
{
	unsigned int type;
	unsigned int action;
	Media_FVec3 value;
	Media_IVec2 rect;
} 
Media_Event;

#ifdef __cplusplus
extern "C" {
#endif

int  Media_init();
void Media_quit();

void Media_setEventListener(void (*listener)(const Media_Event*,void*), void *data);

void Media_handleEvents();
void Media_waitForEvent();

void Media_setRenderer(void (*renderer)(void*), void *data);

void Media_renderFrame();

#ifdef __ANDROID__
int main();
#endif

#ifdef __cplusplus
}
#endif

#ifdef __ANDROID__
#include <android/log.h>
#define printInfo(...) ((void)__android_log_print(ANDROID_LOG_INFO, "mynativeapp", __VA_ARGS__))
#define printWarn(...) ((void)__android_log_print(ANDROID_LOG_WARN, "mynativeapp", __VA_ARGS__))
#else
#include <stdio.h>
#define printInfo(...) (fprintf(stdout,__VA_ARGS__))
#define printWarn(...) (fprintf(stderr,__VA_ARGS__))
#endif


