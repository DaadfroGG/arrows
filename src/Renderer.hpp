#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <limits.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <functional>
#include <stdbool.h>
#include <cstdio>
#include <string>
#include <math.h>
#include <sstream>
#include <set>
// #define GENERAL_Y 2560 / 2
// #define GENERAL_X 1440 / 2
#define GENERAL_Y 1920
#define GENERAL_X 1080
#define WIDTH GENERAL_Y
#define HEIGHT GENERAL_X
#define POINT_RADIUS 5
#define MAP_WIDTH GENERAL_Y
#define MAP_HEIGHT GENERAL_X
// #define MAP_WIDTH 2580
// #define MAP_HEIGHT 1440

    /* 
        // Example usage:
        // Draw a point on screen
        renderer.drawPoint(renderraw_noflag, 100, 100, 0xFF0000, 0);
        
        // Draw a point on a texture
        renderer.drawPoint(texdraw_noflag, 100, 100, 0xFF0000, 0);

        // Draw a line on screen
        renderer.drawLine(renderdraw_noflag, 100, 100, 200, 200, 0xFF0000, 0); 
        // Draw a line on a texture
        renderer.drawLine(texdraw_noflag, 100, 100, 200, 200, 0xFF0000, 0);
        
        // Draw a circle on screen
        renderer.drawCircle(renderdraw_noflag, 100, 100, 50, 0xFF0000, 0);
        // Draw a circle on a texture
        renderer.drawCircle(texdraw_noflag, 100, 100, 50, 0xFF0000, 0);

        // Fill a triangle on screen
        renderer.fillTriangle(renderdraw_noflag, 100, 100, 200, 200, 300, 100, 0xFF0000, 0);
        // Fill a triangle on a texture
        renderer.fillTriangle(texdraw_noflag, 100, 100, 200, 200, 300, 100, 0xFF0000, 0);
        
        // Fill a polygon on screen
        renderer.fillPolygon(renderdraw_noflag, {Point(100, 100), Point(200, 200), Point(300, 100)}, 0xFF0000, 0);
        // Fill a polygon on a texture
        renderer.fillPolygon(texdraw_noflag, {Point(100, 100), Point(200, 200), Point(300, 100)}, 0xFF0000, 0);
        // Outline a polygon on screen

        renderer.outlinePolygon(renderdraw_noflag, {Point(100, 100), Point(200, 200), Point(300, 100)}, 0xFF0000, 0);
        // Outline a polygon on a texture
        renderer.outlinePolygon(texdraw_noflag, {Point(100, 100), Point(200, 200), Point(300, 100)}, 0xFF0000, 0);
        
        // Draw a texture on screen
        renderer.drawTex(&tex, 100, 100);
    */

class Point {
public:
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

class Edge {
public:
    Point* start;
    Point* end;
    Edge(Point* start, Point* end) : start(start), end(end) {}
};
enum flag{
    NO_OVERLAP = (1 << 0),
    NOISE = (1 << 1),
};

class Tex {
    private:
        Uint32* pixels;

        SDL_Surface* surface;
        SDL_Texture* texture;
        int width;
        int height;
        double noise;
    public:
        Tex(SDL_Renderer* renderer, SDL_Window* window, int width, int height, int noise = 1);
        ~Tex();
        SDL_Texture* getTexture();
        Uint32* getPixels();
        int getWidth();
        int getHeight();
        void setPixel(int x, int y, int color , SDL_Renderer* renderer, int flag);
        void setNoise(double noise);
        void update(SDL_Renderer* renderer, SDL_Window* window);
        void clear();
        void save(const char* filename);
};

typedef void (*drawFunction)(int, int, int, SDL_Renderer*, int);
#define drawFunc std::function<void(int, int, int, SDL_Renderer*, int)>
class Renderer {
    private:
        SDL_Renderer* renderer;
        SDL_Window* window;
    public:
        Renderer(SDL_Renderer* renderer, SDL_Window* window);
        ~Renderer();
        void toggleFullscreen();
        SDL_Renderer* getRenderer();
        SDL_Window* getWindow();
        void drawPoint(drawFunc drawFunction, int x, int y, int color, int flag);
        void drawLine(drawFunc drawFunction, int x1, int y1, int x2, int y2, int color, int flag);
        void drawCircle(drawFunc drawFunction, int x, int y, int radius, int color, int flag);
        void fillTriangle(drawFunc drawFunction, int x1, int y1, int x2, int y2, int x3, int y3, int color, int flag);
        void fillPolygon(drawFunc drawFunction, std::vector<Point> vertices, int color, int flag);
        void outlinePolygon(drawFunc drawFunction, std::vector<Point> vertices, int color, int flag);
        void drawTex(Tex* tex, int x, int y);

        void setPixel(int x, int y, int color , SDL_Renderer* renderer, int flag) ;
        // those are the functions that update the screen
        void update();
        void clear();
};

#define texdraw [&tex](int x, int y, int color, SDL_Renderer* r, int flagn)
#define renderdraw [&renderer](int x, int y, int color, SDL_Renderer* r, int flagn)
#define texdraw_noflag texdraw{tex.setPixel(x, y, color, r, 0);}
#define texdraw_nooverlap texdraw{tex.setPixel(x, y, color, r, NO_OVERLAP);}
#define renderdraw_noflag renderdraw{renderer.setPixel(x, y, color, r, 0);}

#endif
