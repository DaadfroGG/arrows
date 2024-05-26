#include "Renderer.hpp"
// Constructor
Renderer::Renderer(SDL_Renderer* __attribute__((unused))renderer, SDL_Window* window) {
    this->window = window;
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_SOFTWARE);
    SDL_ShowCursor(SDL_DISABLE);
    if (this->renderer == nullptr) {
        std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
    }
}

    // Function to toggle fullscreen mode
void Renderer::toggleFullscreen() {
    Uint32 fullscreenFlag = SDL_GetWindowFlags(this->window) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(window, fullscreenFlag ? 0 : SDL_WINDOW_FULLSCREEN);
}

void Renderer::setAlwaysOnTop() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(this->window, &wmInfo)) {
        Display* display = wmInfo.info.x11.display;
        Window root = DefaultRootWindow(display);
        Window sdlWindowHandle = wmInfo.info.x11.window;

        Atom wmStateAbove = XInternAtom(display, "_NET_WM_STATE_ABOVE", 0);
        Atom wmNetWmState = XInternAtom(display, "_NET_WM_STATE", 0);
        
        if (wmStateAbove == None || wmNetWmState == None) {
            std::cerr << "ERROR: cannot find atom for _NET_WM_STATE_ABOVE or _NET_WM_STATE" << std::endl;
            return;
        }

        XClientMessageEvent xclient;
        memset(&xclient, 0, sizeof(xclient));
        xclient.type = ClientMessage;
        xclient.window = sdlWindowHandle;
        xclient.message_type = wmNetWmState;
        xclient.format = 32;
        xclient.data.l[0] = _NET_WM_STATE_ADD;
        xclient.data.l[1] = wmStateAbove;
        xclient.data.l[2] = 0;
        xclient.data.l[3] = 1; // Normal window
        xclient.data.l[4] = 0;

        XSendEvent(display, root, False, SubstructureRedirectMask | SubstructureNotifyMask, (XEvent *)&xclient);

        // Set the override_redirect attribute to True to make the window manager ignore it
        XSetWindowAttributes attrs;
        attrs.override_redirect = True;
        XChangeWindowAttributes(display, sdlWindowHandle, CWOverrideRedirect, &attrs);

        // Ensure the window is above others
        XRaiseWindow(display, sdlWindowHandle);

        // Reset the override_redirect attribute to False to regain window manager control
        attrs.override_redirect = False;
        XChangeWindowAttributes(display, sdlWindowHandle, CWOverrideRedirect, &attrs);

        XFlush(display);
    } else {
        std::cerr << "Failed to get SDL window info" << std::endl;
    }
}



// Destructor
Renderer::~Renderer() {
    SDL_DestroyRenderer(this->renderer);
}

// Those are the functions that you can use to draw on the screen
void Renderer::setPixel(int x, int y, int color , SDL_Renderer* renderer, int flag) {
    SDL_SetRenderDrawColor(renderer, color >> 16, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_RenderDrawPoint(renderer, x, y);
    (void)flag;
}

SDL_Renderer* Renderer::getRenderer() {
    return this->renderer;
}

SDL_Window* Renderer::getWindow() {
    return this->window;
}

void Renderer::drawPoint(std::function<void(int, int, int, SDL_Renderer*, int)> drawFunction, int x, int y, int color, int flag) {
    drawFunction(x, y, color, this->renderer, flag);
}

void Renderer::drawLine(std::function<void(int, int, int, SDL_Renderer*, int)> drawFunction, int x1, int y1, int x2, int y2, int color, int flag) {
    SDL_SetRenderDrawColor(this->renderer, color >> 16, (color >> 8) & 0xFF, color & 0xFF, 255);
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;
    for (;;) {
        drawPoint(drawFunction, x1, y1, color, flag);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

void Renderer::drawCircle(std::function<void(int, int, int, SDL_Renderer*, int)> drawFunction, int x, int y, int radius, int color, int flag) {
    SDL_SetRenderDrawColor(this->renderer, color >> 16, (color >> 8) & 0xFF, color & 0xFF, 255);
    int x1 = 0;
    int y1 = radius;
    int d = 3 - 2 * radius;
    while (y1 >= x1) {
        drawPoint(drawFunction, x + x1, y - y1, color, flag);
        drawPoint(drawFunction, x + y1, y - x1, color, flag);
        drawPoint(drawFunction, x + y1, y + x1, color, flag);
        drawPoint(drawFunction, x + x1, y + y1, color, flag);
        drawPoint(drawFunction, x - x1, y + y1, color, flag);
        drawPoint(drawFunction, x - y1, y + x1, color, flag);
        drawPoint(drawFunction, x - y1, y - x1, color, flag);
        drawPoint(drawFunction, x - x1, y - y1, color, flag);
        if (d < 0) {
            d = d + 4 * x1 + 6;
        } else {
            d = d + 4 * (x1 - y1) + 10;
            y1--;
        }
        x1++;
    }
}
void Renderer::fillTriangle(std::function<void(int, int, int, SDL_Renderer*, int)> drawFunction, int x1, int y1, int x2, int y2, int x3, int y3, int color, int flag) {
    SDL_SetRenderDrawColor(this->renderer, color >> 16, (color >> 8) & 0xFF, color & 0xFF, 255);
    int minX = std::min(x1, std::min(x2, x3));
    int minY = std::min(y1, std::min(y2, y3));
    int maxX = std::max(x1, std::max(x2, x3));
    int maxY = std::max(y1, std::max(y2, y3));
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            int w0 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
            int w1 = (x3 - x2) * (y - y2) - (y3 - y2) * (x - x2);
            int w2 = (x1 - x3) * (y - y3) - (y1 - y3) * (x - x3);
            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                drawPoint(drawFunction, x, y, color, flag);
            }
        }
    }
}
void Renderer::fillPolygon(std::function<void(int, int, int, SDL_Renderer*, int)> drawFunction, std::vector<Point> vertices, int color, int flag) {
    int i = 0;
    while (i < (int)vertices.size() - 2) {
        fillTriangle(drawFunction, vertices[0].x, vertices[0].y, vertices[i + 1].x, vertices[i + 1].y, vertices[i + 2].x, vertices[i + 2].y, color, flag);
        i++;
    }
    fillTriangle(drawFunction, vertices[0].x, vertices[0].y, vertices[i + 1].x, vertices[i + 1].y, vertices[1].x, vertices[1].y, color, flag);
}

void Renderer::outlinePolygon(std::function<void(int, int, int, SDL_Renderer*, int)> drawFunction, std::vector<Point> vertices, int color, int flag){
    //Draw multiple lines for thickness
    for (int i = 0; i < (int)vertices.size() - 1; i++) {
        drawLine(drawFunction, vertices[i].x, vertices[i].y, vertices[i + 1].x, vertices[i + 1].y, color, flag);
    }
    drawLine(drawFunction, vertices[vertices.size() - 1].x, vertices[vertices.size() - 1].y, vertices[0].x, vertices[0].y, color, flag);
}
        

void Renderer::drawTex(Tex* tex, int x, int y){
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = tex->getWidth();
    dst.h = tex->getHeight();
    SDL_RenderCopy(this->renderer, tex->getTexture(), NULL, &dst);

}
// Those are the functions that you can use to draw on a texture


// those are the functions that you can use to update the screen
void Renderer::update() {
    SDL_RenderPresent(this->renderer);
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);
}

