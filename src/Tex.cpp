#include "Renderer.hpp"

Tex::Tex(SDL_Renderer* renderer, SDL_Window* window, int width, int height, int noise) {
    this->pixels = new Uint32[width * height];
    this->surface = SDL_CreateRGBSurfaceFrom(
        this->pixels,  // The pixel data array
        width,         // The width of the texture
        height,        // The height of the texture
        32,            // The depth of the pixel data   
        width *  4,    // The size of each row in bytes
        SDL_GetWindowSurface(window)->format->Rmask,    // The red mask
        SDL_GetWindowSurface(window)->format->Gmask,    // The green mask
        SDL_GetWindowSurface(window)->format->Bmask,    // The blue mask
        SDL_GetWindowSurface(window)->format->Amask);   // The alpha mask
    this->texture = SDL_CreateTextureFromSurface(renderer, surface);
    this->width = width;
    this->height = height;
}
Tex::~Tex() {
    SDL_FreeSurface(this->surface);
    delete[] this->pixels;
    SDL_DestroyTexture(this->texture);
}
SDL_Texture* Tex::getTexture() {
    return this->texture;
}
Uint32* Tex::getPixels() {
    return this->pixels;
}
int Tex::getWidth() {
    return this->width;
}
int Tex::getHeight() {
    return this->height;
}

void Tex::setPixel(int x, int y, int color , SDL_Renderer* renderer, int flag) {
    if (x < 0 || x >= this->width || y < 0 || y >= this->height ){
        return;
    } else if (flag & NO_OVERLAP && this->pixels[y * this->width + x] != 0) {
        return;
    } else {
        if (flag & NOISE) {
            int r = rand() % 255 / this->noise;
            int g = rand() % 255 / this->noise;
            int b = rand() % 255 / this->noise;
            
            r += (color >> 16) & 0xFF;
            g += (color >> 8) & 0xFF;
            b += color & 0xFF;
            color = r << 16 | g << 8 | b;

        }
        this->pixels[y * this->width + x] = color;
    }
    (void)renderer;
    (void )flag;
}
void Tex::setNoise(double noise) {
    this->noise = noise;
}
void Tex::update(SDL_Renderer* renderer, SDL_Window* window) {
    this->surface = SDL_CreateRGBSurfaceFrom(
        this->pixels,         // The pixel data array
        this->width,  // The width of the texture
        this->height, // The height of the texture
        32,            // The depth of the pixel data   
        this->width *  4, // The size of each row in bytes
        SDL_GetWindowSurface(window)->format->Rmask,    // The red mask
        SDL_GetWindowSurface(window)->format->Gmask,    // The green mask
        SDL_GetWindowSurface(window)->format->Bmask,    // The blue mask
        SDL_GetWindowSurface(window)->format->Amask);   // The alpha mask
    SDL_DestroyTexture(this->texture);
    this->texture = SDL_CreateTextureFromSurface(renderer, surface);
}
void Tex::clear() {
    memset(this->pixels, 0, this->width * this->height * sizeof(Uint32));
}

void Tex::save(const char* filename) {
    std::string textureFolder = "texture/";
    std::string filePath = textureFolder + filename;
    std::ifstream file(filePath);
    int count = 1;

    while (file) {
        file.close();
        std::stringstream ss;
        ss << count;
        std::string numberedFilename = filename + std::string("_") + ss.str();
        filePath = textureFolder + numberedFilename;
        file.open(filePath);
        count++;
    }

    std::ofstream outputFile(filePath, std::ios::binary);
    outputFile << "P6\n" << width << " " << height << " 255\n";
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            Uint32 pixel = this->pixels[y * this->width + x];
            outputFile.put((pixel & 0xFF0000) >> 16);
            outputFile.put((pixel & 0x00FF00) >> 8);
            outputFile.put(pixel & 0x0000FF);
        }
    }
    outputFile.close();
}
