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
};

void handleEvent(const Media_Event *event, void *data)
{
	State *state = static_cast<State*>(data);
	vec2 pos;
	switch(event->type)
	{
	case MEDIA_SAVE_STATE:
		printInfo("Save state\n");
		break;
	case MEDIA_QUIT:
		state->done = 1;
		printInfo("Quit\n");
		break;
	case MEDIA_SHOWN:
		state->wait = 0;
		Media_enableSensor(MEDIA_ACCELEROMETER,(1000L/30)*1000);
		printInfo("Shown\n");
		break;
	case MEDIA_HIDDEN:
		state->wait = 1;
		Media_disableSensor(MEDIA_ACCELEROMETER);
		printInfo("Hidden\n");
		break;
	case MEDIA_INIT_SURFACE:
		printInfo("Init surface\n");
		initGraphics();
		break;
	case MEDIA_TERM_SURFACE:
		printInfo("Term surface\n");
		disposeGraphics();
		break;
	case MEDIA_RESIZE_SURFACE:
		printInfo("Resize surface ( %d, %d )\n",event->rect.x,event->rect.y);
		state->width = event->rect.x;
		state->height = event->rect.y;
		state->pool->setBorders(event->rect.y/2.0,-event->rect.x/2.0,-event->rect.y/2.0,event->rect.x/2.0);
		resizeGraphics(event->rect.x,event->rect.y);
		state->ready = true;
		break;
	case MEDIA_MOTION:
		pos = vec2(event->rect.x - state->width/2.0, state->height/2.0 - event->rect.y);
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
		break;
	case MEDIA_SENSOR:
		switch(event->sensor)
		{
		case MEDIA_ACCELEROMETER:
			state->pool->accelerate(-vec2(event->value.x,event->value.y));
			// printInfo("Accelerometer ( %f, %f, %f)\n",event->value.x,event->value.y,event->value.z);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void render(void *data)
{
	Pool *pool = static_cast<Pool*>(data);
	clear();
	pool->draw();
}

int main()
{
	Media_init();
	
	Pool pool(40.0,1.0,20.0,20.0);
	pool.addCircle(new Circle(60.0f,60.0f,vec2(-70.0,-50.0),BLUE));
	pool.addCircle(new Circle(70.0f,70.0f,vec2(70.0,-50.0),GREEN));
	pool.addCircle(new Circle(80.0f,80.0f,vec2(0.0,100.0),RED));
	
	State state = {0,0,&pool,0,0,false};
	Media_setEventListener(handleEvent,static_cast<void*>(&state));
	
	Media_setRenderer(render,&pool);
	
	while(!state.done)
	{
		if(state.wait)
		{
			Media_waitForEvent();
		}
		Media_handleEvents();
		
		if(state.ready)
		{
			for(int i = 0; i < ITER_PER_FRAME; ++i)
			{
				pool.interact();
				pool.collide();
				pool.step(1.0/ITER_PER_FRAME);
			}
		}
		
		// printInfo("Frame\n");
		Media_renderFrame();
	}
	
	pool.forEachCircle([](Circle *c){delete c;});
	
	Media_quit();
	return 0;
}
