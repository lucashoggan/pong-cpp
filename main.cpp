#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <vector>
#include <cmath>
#include <iostream>


using std::vector;

const double pi = std::acos(-1);

double rad(int deg) {
    return (deg*180)/pi;
}

vector<double> getProjectileSpeed(int speed, double angle) {
    vector<double> out;
    if (0 < angle < pi/2) {
        out.push_back(speed*std::cos(pi/2-angle));
        out.push_back(speed*std::sin(pi/2-angle));
    } else if (pi/2 < angle < pi) {
        out.push_back(speed*std::cos(angle-pi/2));
        out.push_back(-1*speed*std::sin(angle-pi/2));
    } else if (pi < angle < (3/2)*pi) {
        out.push_back(-1*speed*std::cos((3/2)*pi-angle));
        out.push_back(-1*speed*std::sin((3/2)*pi-angle));
    } else if (angle == 0){
        out.push_back(0);
        out.push_back(speed);
    } else if (angle == pi/2) {
        out.push_back(speed);
        out.push_back(0);
    } else if (angle == pi) {
        out.push_back(0);
        out.push_back(-speed);
    } else if (angle == (3/2)*pi) {
        out.push_back(-speed);
        out.push_back(0);
    } else {
        out.push_back(-1*speed*std::cos(angle-(3/2)*pi));
        out.push_back(speed*std::sin(angle-(3/2)*pi));
    }
    
    

    return out;
}

int projDistanceFromPaddleCentre(SDL_Rect paddle, SDL_Rect proj) { return paddle.y+paddle.h/2-proj.y; }


int main(int argc, char* argv[]) {

    SDL_Window *window;                    // Declare a pointer
    bool done = false;
    int paddleY1 = 100;
    int paddleY2 = 100;
    int paddleYDeltaVelocity = 10;
    int prevTime = SDL_GetTicks();
    int deltaTime;
    int score1 = 0;
    int score2 = 0;
    int projSpinK = 30;

    // PROJECTILE
    int projSpeed = 4;
    double projAngle = 0.5*pi;
    vector<double> projXYSpeed;


    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "Pong",                  // window title
        900,                               // width, in pixels
        600,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    SDL_Rect paddle1;
    SDL_FRect paddle1F;
    SDL_Rect paddle2;
    SDL_FRect paddle2F;
    SDL_Rect proj;
    SDL_FRect projF;
    paddle1.x=100;
    paddle2.x=750;
    paddle1.y=275;
    paddle2.y=275;
    paddle1.h=100;
    paddle2.h=100;
    paddle1.w=50;
    paddle2.w=50;
    
    proj.w=5;
    proj.h=5;
    
    proj.x=450;
    proj.y=300;


    while (!done) {
        SDL_Event event;
        deltaTime = SDL_GetTicks() - prevTime;
        prevTime = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }

            
        }

        const bool *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) {
            paddle1.y-=paddleYDeltaVelocity;
        } else if (keys[SDL_SCANCODE_S]) {
            paddle1.y+=paddleYDeltaVelocity;
        }
        if (keys[SDL_SCANCODE_UP]) {
            paddle2.y-=paddleYDeltaVelocity;
        } else if (keys[SDL_SCANCODE_DOWN]) {
            paddle2.y+=paddleYDeltaVelocity;
        }
        if (keys[SDL_SCANCODE_R]) {
            score1=0;
            score2=0;
            paddle1.y=275;
            paddle2.y=275;
            projAngle = 0.5*pi;
            proj.x=450;
            proj.y=300;

        }



        projXYSpeed = getProjectileSpeed(projSpeed, projAngle);
        proj.x+=projXYSpeed[0];
        proj.y-=projXYSpeed[1];
        
        if (proj.y < 0 || proj.y > 595) {
            projAngle = pi-projAngle;
        }
        if (SDL_HasRectIntersection(&proj, &paddle1)) {
            if (projDistanceFromPaddleCentre(paddle1, proj) > 0) {
                projAngle = 2*pi-projAngle - pi/10;
            } else {
                projAngle = 2*pi-projAngle + pi/10;
            }
            
            proj.x+=5;
        } else if (SDL_HasRectIntersection(&proj, &paddle2)) {
            if (projDistanceFromPaddleCentre(paddle2, proj) > 0) {
                projAngle = 2*pi-projAngle + pi/10;
            } else {
                projAngle = 2*pi-projAngle - pi/10;
            }
            
            proj.x-=5;
        }
        
        if (proj.x < 5) {
            score2++;
            std::cout << "1: " << score1 << ", 2: " << score2 << std::endl;
            proj.x=450;
            proj.y=300;
            projAngle = 1.5*pi;
        } 
        if (proj.x > 895) {
            score1++;
            std::cout << "1: " << score1 << ", 2: " << score2 << std::endl;
            proj.x=450;
            proj.y=300;
            projAngle = pi/2;
        }

        // Do game logic, present a frame, etc.
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RectToFRect(&paddle1, &paddle1F);
        SDL_RectToFRect(&paddle2, &paddle2F);
        SDL_RectToFRect(&proj, &projF);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &paddle1F);
        SDL_RenderFillRect(renderer, &paddle2F);
        SDL_RenderFillRect(renderer, &projF);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}