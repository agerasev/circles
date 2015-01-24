/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#define DEBUG

#include "media.h"

#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "mynativeapp", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "mynativeapp", __VA_ARGS__))

static int  initDisplay();
static void termDisplay();

struct engine 
{
	struct android_app* app;
	
	void(*event_listener)(const Media_Event*,void*);
	void *event_listener_data;
	
	void(*renderer)(void*);
	void *renderer_data;
	
	EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  int32_t width;
  int32_t height;
  int frame_counter;
};
static struct engine engine;

static void pushEvent(const Media_Event *event)
{
	if(engine.event_listener != NULL)
	{
		engine.event_listener(event,engine.event_listener_data);
	}
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	Media_Event m_event;
  switch(AInputEvent_getType(event))
  {
	case AINPUT_EVENT_TYPE_MOTION:
		m_event.type = MEDIA_MOTION;
		switch(AInputEvent_getSource(event))
		{
		case AINPUT_SOURCE_TOUCHSCREEN:
			switch(AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK)
			{
			case AMOTION_EVENT_ACTION_DOWN:
				m_event.action = MEDIA_ACTION_DOWN;
				break;
			case AMOTION_EVENT_ACTION_UP:
				m_event.action = MEDIA_ACTION_UP;
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				m_event.action = MEDIA_ACTION_MOVE;
				break;
			}
			break;
		}
		m_event.rect.x = AMotionEvent_getX(event, 0);
		m_event.rect.y = AMotionEvent_getY(event, 0);
		pushEvent(&m_event);
		return 1;
	/*
	case AINPUT_EVENT_TYPE_KEY:
		// handle key input...
		return 1;
	*/
  }
  
	return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) 
{
	Media_Event event;
	int call_back = 1;
	printInfo("Command %d\n",cmd);
	switch(cmd) 
	{
		case APP_CMD_SAVE_STATE:
			event.type = MEDIA_SAVE_STATE;
			pushEvent(&event);
			break;
		case APP_CMD_INIT_WINDOW:
			initDisplay();
			event.type = MEDIA_INIT_SURFACE;
			pushEvent(&event);
			event.type = MEDIA_RESIZE_SURFACE;
			event.rect.x = engine.width;
			event.rect.y = engine.height;
			pushEvent(&event);
			break;
		case APP_CMD_TERM_WINDOW:
			termDisplay();
			event.type = MEDIA_TERM_SURFACE;
			pushEvent(&event);
			break;
		case APP_CMD_CONFIG_CHANGED:
			engine.frame_counter = 4;
		case APP_CMD_GAINED_FOCUS:
			event.type = MEDIA_SHOWN;
			pushEvent(&event);
			break;
		case APP_CMD_LOST_FOCUS:
			event.type = MEDIA_HIDDEN;
			pushEvent(&event);
			break;
		case APP_CMD_DESTROY:
			event.type = MEDIA_QUIT;
			pushEvent(&event);
			break;
		default:
			break;
	}
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) 
{
	// Make sure glue isn't stripped.
	app_dummy();
	
	memset(&engine,0,sizeof(struct engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;
	
	main();
}

int Media_init()
{
	// LOGI("Application started\n");
}

void Media_quit()
{
	// LOGI("Application exited\n");
}

void Media_setEventListener(void (*listener)(const Media_Event*, void*), void *data)
{
	engine.event_listener = listener;
	engine.event_listener_data = data;
}

static void engine_handle_events(int mode)
{
	if(engine.frame_counter > 0)
	{
		engine.frame_counter--;
		if(engine.frame_counter == 0)
		{
			Media_Event event;
			event.type = MEDIA_RESIZE_SURFACE;
			eglQuerySurface(engine.display, engine.surface, EGL_WIDTH, &(event.rect.x));
			eglQuerySurface(engine.display, engine.surface, EGL_HEIGHT, &(event.rect.y));
			pushEvent(&event);
		}
	}
	
	// Read all pending events.
	int ident;
	int events;
	struct android_poll_source* source;

	// If not animating, we will block forever waiting for events.
	// If animating, we loop until all events are read, then continue
	// to draw the next frame of animation.
	while((ident=ALooper_pollAll(mode, NULL, &events, (void**)&source)) >= 0)
	{
		// LOGI("poll mode %d\n",mode);
	
		// Process this event.
		if(source != NULL) 
		{
			source->process(engine.app, source);
		}
		
		if(mode)
		{
			break;
		}
		
		// Check if we are exiting.
		if(engine.app->destroyRequested != 0) 
		{
			break;
		}
	}
}

void Media_handleEvents()
{
	engine_handle_events(0);
}

void Media_waitForEvent()
{
	engine_handle_events(-1);
}

int initDisplay()
{
	// initialize OpenGL ES and EGL

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint attribs[] = 
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine.app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine.app->window, NULL);
	
	EGLint attrib_list[] = 
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	
	context = eglCreateContext(display, config, NULL, attrib_list);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine.display = display;
	engine.context = context;
	engine.surface = surface;
	engine.width = w;
	engine.height = h;

	// Initialize GL state.
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	return 0;
}

void termDisplay()
{
	if (engine.display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine.context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine.display, engine.context);
		}
		if (engine.surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine.display, engine.surface);
		}
		eglTerminate(engine.display);
	}

	engine.display = EGL_NO_DISPLAY;
	engine.context = EGL_NO_CONTEXT;
	engine.surface = EGL_NO_SURFACE;
}

void Media_setRenderer(void (*renderer)(void*), void *data)
{
	engine.renderer = renderer;
	engine.renderer_data = data;
}

void Media_renderFrame()
{
	if (engine.display == NULL) {
		// No display.
		return;
	}
	if(engine.renderer != NULL)
	{
		engine.renderer(engine.renderer_data);
	}
	eglSwapBuffers(engine.display, engine.surface);
}
