#include "src/Renderer.hpp"
#include "src/Event.hpp"
#include <cmath>
#include <math.h>

double clamp(double x, double a, double b) {
    return fmax(a, fmin(b, x));
}
struct HSV {
    double h; // Hue [0, 360]
    double s; // Saturation [0, 1]
    double v; // Value [0, 1]
};

HSV rgbToHsv(int r, int g, int b) {
    double r_ = r / 255.0;
    double g_ = g / 255.0;
    double b_ = b / 255.0;

    double max = std::max({r_, g_, b_});
    double min = std::min({r_, g_, b_});
    double delta = max - min;

    double h = 0;
    if (delta != 0) {
        if (max == r_) {
            h = 60 * fmod(((g_ - b_) / delta), 6);
        } else if (max == g_) {
            h = 60 * (((b_ - r_) / delta) + 2);
        } else if (max == b_) {
            h = 60 * (((r_ - g_) / delta) + 4);
        }
    }

    double s = max == 0 ? 0 : delta / max;
    double v = max;

    if (h < 0) {
        h += 360;
    }

    return {h, s, v};
}

int hsvToRgb(HSV hsv) {
    double c = hsv.v * hsv.s;
    double x = c * (1 - fabs(fmod(hsv.h / 60.0, 2) - 1));
    double m = hsv.v - c;

    double r_ = 0, g_ = 0, b_ = 0;

    if (0 <= hsv.h && hsv.h < 60) {
        r_ = c;
        g_ = x;
        b_ = 0;
    } else if (60 <= hsv.h && hsv.h < 120) {
        r_ = x;
        g_ = c;
        b_ = 0;
    } else if (120 <= hsv.h && hsv.h < 180) {
        r_ = 0;
        g_ = c;
        b_ = x;
    } else if (180 <= hsv.h && hsv.h < 240) {
        r_ = 0;
        g_ = x;
        b_ = c;
    } else if (240 <= hsv.h && hsv.h < 300) {
        r_ = x;
        g_ = 0;
        b_ = c;
    } else if (300 <= hsv.h && hsv.h < 360) {
        r_ = c;
        g_ = 0;
        b_ = x;
    }

    int r = (int)((r_ + m) * 255);
    int g = (int)((g_ + m) * 255);
    int b = (int)((b_ + m) * 255);

    return (r << 16) + (g << 8) + b;
}
int interpolateColor(int color1, int color2, double t) {
    int r1 = (color1 >> 16) & 0xFF;
    int g1 = (color1 >> 8) & 0xFF;
    int b1 = color1 & 0xFF;
    int r2 = (color2 >> 16) & 0xFF;
    int g2 = (color2 >> 8) & 0xFF;
    int b2 = color2 & 0xFF;
    int r = (int) (r1 * (1 - t) + r2 * t);
    int g = (int) (g1 * (1 - t) + g2 * t);
    int b = (int) (b1 * (1 - t) + b2 * t);
    return (r << 16) + (g << 8) + b;
}

int main() {
    Renderer renderer(nullptr, SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0));
    Events events;
    Tex tex(renderer.getRenderer(), renderer.getWindow(), MAP_WIDTH, MAP_HEIGHT);
    int arrow[2] = {MAP_WIDTH / 2, MAP_HEIGHT / 4};

    // Variables for dragging
    bool dragging = false;
    int lastMouseX = 0;
    int lastMouseY = 0;
    double hue = 0.99; // Use this to control the hue
    // Frame rate
    Time frameTime;
    double colorlol;
    double flip_flop = 1;
    int draw_mode = 1;
    int megasave = 1;
    int tracemode = 1;
    events.updateMousePosition();
    double lineRatio = 0.9;
    // Main loop
    while (!events.quit()) {
        // Start the frame time
        frameTime.start();
        // Clear the screen
        renderer.clear();
        // Update mouse position
        events.updateMousePosition();
        int mouseX = events.getMouseX();
        int mouseY = events.getMouseY();

        // Handle events
        while (events.poll()) {
            if (events.quit()) {
                      return 0; 
            }
            if (events.keyDown()) {
                if (events.keyPressed(SDLK_ESCAPE)) {
                    return 0;
                }
                if (events.keyPressed(SDLK_s)) {
                    tex.save("test.ppm");
                }
                if (events.keyPressed(SDLK_r)) {
                    tex.clear();
                }
                if (events.keyPressed(SDLK_SPACE)) {
                    draw_mode = !draw_mode;
                }
                if (events.keyPressed(SDLK_m)) {
                    megasave = !megasave;
                }
                if (events.keyPressed(SDLK_t)){
                    tracemode = !tracemode;
                }
            }
            if (events.mouseWheelY() == 1 || events.mouseWheelY() == -1)
            {
                lineRatio += events.mouseWheelY() * 0.001;
                if (lineRatio < 0.1)
                    lineRatio = 0.2;
                if (lineRatio > 1)
                    lineRatio = 1;   
            }
            if (events.mouseButtonDown())
            {
                if (events.mouseButtonPressed(SDL_BUTTON_LEFT))
                {
                        dragging = true;
                        lastMouseX = mouseX;
                        lastMouseY = mouseY;
                }
            }
            if (events.mouseButtonUp())
        {
                if (events.mouseButtonReleased(SDL_BUTTON_LEFT))
                {
                    dragging = false;
                }

        }
        }

        // Update arrow position if dragging
        if (dragging) {
            int dx = mouseX - lastMouseX;
            int dy = mouseY - lastMouseY;
            arrow[0] += dx;
            arrow[1] += dy;
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }
        // Programmable movememt when mouse is at 0 0
        if (mouseX == 0 && mouseY == 0) {
            double radius = 500.0; // Set the radius of the circle
            double centerX = MAP_WIDTH / 2;
            double centerY = MAP_HEIGHT / 2;
            double angle = colorlol * 2 * M_PI - M_PI / 2; // Convert colorlol to radians

            mouseX = centerX + radius * cos(angle);
            mouseY = centerY + radius * sin(angle);
        }
        if (megasave <= 0) 
        {
            tex.save("buffer.ppm");
        }

        // Drawing logic
            double dx = mouseX - arrow[0];
            double dy = mouseY - arrow[1];
            double len = sqrt(dx * dx + dy * dy);
            double angle = atan2(dx, -dy);
            double angle_increment = angle;
            int num_lines = 200;


            double startX = mouseX;
            double startY = mouseY;
            double prevEndX = startX;
            double prevEndY = startY;

            renderer.drawTex(&tex, WIDTH / 2 - MAP_WIDTH / 2, HEIGHT / 2 - MAP_HEIGHT / 2);
            for (int i = 1; i <= num_lines; ++i) {
                double endX = len * sin(angle_increment * (i + 1)) + startX;
                double endY = -len * cos(angle_increment * (i + 1)) + startY;

                HSV hsv1 = {colorlol * 360, 1.0, 1.0};
                HSV hsv2 = {(-colorlol) * 360, 1.0, 1.0};
                int color1 = hsvToRgb(hsv1);
                int color2 = hsvToRgb(hsv2);

                if (draw_mode) {
                    renderer.outlinePolygon(renderdraw_noflag, {Point(startX, startY), Point(endX, endY), Point(prevEndX, prevEndY)}, color1, 0);
                   // renderer.drawLine(renderdraw_noflag, startX, startY, endX, endY, color2, 0);
                } else {
                    renderer.outlinePolygon(texdraw_noflag, {Point(startX, startY), Point(endX, endY), Point(prevEndX, prevEndY)}, color1, 0);
                    //renderer.drawLine(texdraw_noflag, startX, startY, endX, endY, color2, 0);
                }

                prevEndX = startX;
                prevEndY = startY;
                startX = endX;
                startY = endY;
                len *= lineRatio;
            }
    


        // Draw the texture


        tex.update(renderer.getRenderer(), renderer.getWindow());
        if (tracemode)
            tex.clear();
        if (dragging)
            renderer.drawCircle(renderdraw_noflag, mouseX, mouseY, 10,0x00FF00, 0);
        else
            renderer.drawCircle(renderdraw_noflag, mouseX, mouseY, 10,0xFF00, 0);
        
        renderer.update();

        // Limit the frame rate
        frameTime.limit();
        if (colorlol >= 1) {
            colorlol = 0;
        }
        colorlol += flip_flop / 10000;
    }
    return 0;
}
