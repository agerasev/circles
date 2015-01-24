#include "media.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>

struct __Context
{
	SDL_Window *window;
	SDL_GLContext context;
	
	int width;
	int height;
	
	int mouse[3];
	
	void(*event_listener)(const Media_Event*,void*);
	void *event_listener_data;
	
	int init_event_pushed;
	void(*renderer)(void*);
	void *renderer_data;
};
static struct __Context __context;

static void pushEvent(const Media_Event *event)
{
	if(__context.event_listener != NULL)
	{
		__context.event_listener(event,__context.event_listener_data);
	}
}

int Media_init()
{
	__context.width = 800;
	__context.height = 600;

	__context.window =
		SDL_CreateWindow(
			"MyNativeApp",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			__context.width, __context.height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		);
	
	__context.context =
		SDL_GL_CreateContext(__context.window);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	SDL_GL_SetSwapInterval(1);

	GLenum glew_status = glewInit();
	if(GLEW_OK != glew_status)
	{
		printf("%s\n",glewGetErrorString(glew_status));
		return -1;
	}
	if(!GLEW_VERSION_2_0)
	{
		printf("%s\n","No support for OpenGL 2.0 found");
		return -2;
	}
	
	return 0;
}

void Media_quit()
{
	SDL_GL_DeleteContext(__context.context);
	SDL_DestroyWindow(__context.window);
}

static void handleEvent(const SDL_Event *event)
{
	Media_Event m_event;
	if(!__context.init_event_pushed)
	{
		m_event.type = MEDIA_INIT_SURFACE;
		pushEvent(&m_event);
		m_event.type = MEDIA_RESIZE_SURFACE;
		m_event.rect.x = __context.width;
		m_event.rect.y = __context.height;
		pushEvent(&m_event);
		__context.init_event_pushed = 1;
	}
	int call_back = 1;
	switch(event->type)
	{
	case SDL_QUIT:
		m_event.type = MEDIA_TERM_SURFACE;
		pushEvent(&m_event);
		m_event.type = MEDIA_QUIT;
		pushEvent(&m_event);
		break;
	case SDL_WINDOWEVENT:
		switch(event->window.event) 
		{
			case SDL_WINDOWEVENT_RESIZED:
				__context.width = event->window.data1;
				__context.height = event->window.data2;
				m_event.type = MEDIA_RESIZE_SURFACE;
				m_event.rect.x = __context.width;
				m_event.rect.y = __context.height;
				pushEvent(&m_event);
				break;
			/*
			// Not used due to bad behavior
			case SDL_WINDOWEVENT_MAXIMIZED:
				m_event.type = MEDIA_SHOWN;
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				m_event.type = MEDIA_HIDDEN;
				break;
			default:
				call_back = 0;
				break;
			*/
		}
		break;
	case SDL_MOUSEMOTION:
		if(__context.mouse[0])
		{
			m_event.type = MEDIA_MOTION;
			m_event.action = MEDIA_ACTION_MOVE;
			m_event.rect.x = event->motion.x;
		  m_event.rect.y = event->motion.y;
			pushEvent(&m_event);
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if(event->button.button == SDL_BUTTON_LEFT)
		{
			__context.mouse[0] = 1;
			m_event.type = MEDIA_MOTION;
			m_event.action = MEDIA_ACTION_DOWN;
			m_event.rect.x = event->button.x;
		  m_event.rect.y = event->button.y;
			pushEvent(&m_event);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if(event->button.button == SDL_BUTTON_LEFT)
		{
			__context.mouse[0] = 0;
			m_event.type = MEDIA_MOTION;
			m_event.action = MEDIA_ACTION_UP;
			m_event.rect.x = event->button.x;
		  m_event.rect.y = event->button.y;
			pushEvent(&m_event);
		}
		break;
	default:
		break;
	}
}

void Media_handleEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		handleEvent(&event);
	}
}

void Media_waitForEvent()
{
	SDL_Event event;
	if(SDL_WaitEvent(&event))
	{
		handleEvent(&event);
	}
}

void Media_setEventListener(void (*listener)(const Media_Event*,void*), void *data)
{
	__context.event_listener = listener;
	__context.event_listener_data = data;
}

void Media_setRenderer(void (*renderer)(void*), void *data)
{
	__context.renderer = renderer;
	__context.renderer_data = data;
}

void Media_renderFrame()
{
	if(__context.renderer != NULL)
	{
		__context.renderer(__context.renderer_data);
	}
	SDL_GL_SwapWindow(__context.window);
}
