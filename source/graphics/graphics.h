#pragma once

/**
 * Library for simple graphics
 */

#define BLACK    0xff000000
#define WHITE    0xffffffff
#define RED      0xff0000ff
#define GREEN    0xff00ff00
#define BLUE     0xffff0000
#define YELLOW   0xff00ffff
#define CYAN     0xffffff00
#define MAGENTA  0xffff00ff

#define ALPHA(c) (((int)(255.0f*c)<<24)|0x00ffffff)

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes graphics subsystem */
int initGraphics();

/* Safely disposes graphics subsystem */
int disposeGraphics();

/* Performs resizing of window */
void resizeGraphics(int width, int height);

/* Transform */
void translate(const float *vector);
void transform(const float *matrix);

/* Color */
void setColor(unsigned color);
void setFloatColor(const float *color);

/* Fill */
void clear();
void fill();

/* Geometry */
void quad();
void circle();

#ifdef __cplusplus
}
#endif

