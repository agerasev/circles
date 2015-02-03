#include <media/media.h>
#include <graphics/graphics.h>

#include <circle.hpp>
#include <pool.hpp>

static const int ITER_PER_FRAME = 0x10;

struct State
{
	int done;
	int wait;
	Pool *pool;
	int width;
	int height;
	bool ready;
	vec2 gravity;
};

void handleAppEvent(Media_App *app, const Media_AppEvent *event)
{
	State *state = static_cast<State*>(app->data);
	switch(event->type)
	{
	case MEDIA_APP_SAVESTATE:
		printInfo("Save state\n");
		break;
	case MEDIA_APP_QUIT:
		state->done = 1;
		printInfo("Quit\n");
		break;
	case MEDIA_APP_SHOW:
		state->wait = 0;
		Media_enableSensor(app,MEDIA_SENSOR_ACCELEROMETER,(1000L/30)*1000);
		printInfo("Show\n");
		break;
	case MEDIA_APP_HIDE:
		state->wait = 1;
		Media_disableSensor(app,MEDIA_SENSOR_ACCELEROMETER);
		printInfo("Hide\n");
		break;
	default:
		break;
	}
}

void handleSurfaceEvent(Media_App *app, const Media_SurfaceEvent *event)
{
	State *state = static_cast<State*>(app->data);
	switch(event->type)
	{
	case MEDIA_SURFACE_INIT:
		printInfo("Init surface\n");
		initGraphics();
		break;
	case MEDIA_SURFACE_TERM:
		printInfo("Term surface\n");
		disposeGraphics();
		break;
	case MEDIA_SURFACE_RESIZE:
		printInfo("Resize surface ( %d, %d )\n",event->w,event->h);
		state->width = event->w;
		state->height = event->h;
		state->pool->setBorders(event->h/2.0,-event->w/2.0,-event->h/2.0,event->w/2.0);
		resizeGraphics(event->w,event->h);
		state->ready = true;
		break;
	default:
		break;
	}
}

void handleMotionEvent(Media_App *app, const Media_MotionEvent *event)
{
	State *state = static_cast<State*>(app->data);
	vec2 pos = vec2(event->x - state->width/2.0, state->height/2.0 - event->y);
	switch(event->action)
	{
	case MEDIA_ACTION_UP:
		//printInfo("Up\n");
		state->pool->dropCircles();
		break;
	case MEDIA_ACTION_DOWN:
		//printInfo("Down\n");
		state->pool->grabCircles(pos);
		break;
	case MEDIA_ACTION_MOVE:
		//printInfo("Move\n");
		state->pool->dragCircles(pos);
		break;
	default:
		break;
	}
	//printInfo("Motion ( %d, %d )\n", static_cast<int>(pos.x()), static_cast<int>(pos.y()));
}

void handleSensorEvent(Media_App *app, const Media_SensorEvent *event)
{
	State *state = static_cast<State*>(app->data);
	switch(event->sensor)
	{
	case MEDIA_SENSOR_ACCELEROMETER:
		state->gravity = -vec2(event->x,event->y)/20.0;
		// printInfo("Accelerometer ( %f, %f, %f)\n",event->x,event->y,event->z);
		break;
	default:
		break;
	}
}

void render(Media_App *app)
{
	State *state = static_cast<State*>(app->data);
	Pool *pool = state->pool;
	clear();
	pool->draw();
}

int Media_main(Media_App *app)
{
	Pool pool(40.0,1.0,1.0,20.0,20.0);
	pool.addCircle(new Circle(60.0f,60.0f,vec2(-70.0,-50.0),BLUE));
	pool.addCircle(new Circle(70.0f,70.0f,vec2(70.0,-50.0),GREEN));
	pool.addCircle(new Circle(80.0f,80.0f,vec2(0.0,100.0),RED));
	
	State state = {0,0,&pool,0,0,false,vec2(0.0,-9.0)/20.0};
	
	app->data = static_cast<void*>(&state);
	
	app->listeners.app = &handleAppEvent;
	app->listeners.surface = &handleSurfaceEvent;
	app->listeners.motion = &handleMotionEvent;
	app->listeners.sensor = &handleSensorEvent;
	
	app->renderer = &render;
	
	while(!state.done)
	{
		if(state.wait)
		{
			Media_waitForEvent(app);
		}
		Media_handleEvents(app);
		
		if(state.ready)
		{
			for(int i = 0; i < ITER_PER_FRAME; ++i)
			{
				pool.accelerate(state.gravity);
				pool.interact();
				pool.collide();
				pool.step(1.0/ITER_PER_FRAME);
			}
		}
		
		// printInfo("Frame\n");
		Media_renderFrame(app);
	}
	
	pool.forEachCircle([](Circle *c){delete c;});
	
	return 0;
}
