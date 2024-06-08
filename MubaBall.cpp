#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>


// imp global variables
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;
const int ICON_WIDTH = 80;
const int ICON_HEIGHT = 130;
const int BALL_SIZE = 20;
const int borderwidth = 20 ;
const int WALL_WIDTH = 13;
const int WALL_HEIGHT = 140;
const float iconspeed = 25.0;
const float GRAVITY = 0.15;
float SHOOTING_ANGLE ;
// for sprite
SDL_Texture* player1Texture = NULL ;
SDL_Texture* player2Texture = NULL ;
// Define the frame dimensions and total number of frames in the sprite sheet
const int frameWidth = 30;
const int frameHeight = 60;
const int totalFrames = 2;
const int animationSpeed = 50; // Delay in milliseconds between frames
int currentFrame1 = 0;
int currentFrame2 = 0;
Uint32 frameStartTime = 0;
// for player movement
int dir1 = 0  ;
int dir2 = 0 ;
//
// coordinates of the icons
int x1 =borderwidth , x2 = WINDOW_WIDTH-borderwidth-ICON_WIDTH ;
int icon1y = WINDOW_HEIGHT-borderwidth-ICON_HEIGHT , icon2y = WINDOW_HEIGHT-ICON_HEIGHT-borderwidth ;
int score1 = 0 , score2 = 0 ;
SDL_Rect icon1 , icon2 ;
int midwallx = WINDOW_WIDTH/2 , midwally = WINDOW_HEIGHT-borderwidth-WALL_HEIGHT  ;
// global variables for ball
SDL_Rect ballRect;
float ballX=+1000, ballY=+1000;
float ballVelX=0, ballVelY=0;
bool ballOnIcon1 = true;
float dx , dy  ;
// global variables for icon jump
float icon2yspeed = 0 ;
float icon1yspeed = 0 ;
// names for icons
char player1name[20] = "player-1" ;
char player2name[20] = "player-2" ;
char point[10] = "0" ;
// start
SDL_Window* window = NULL ;
SDL_Renderer* renderer = NULL ;
SDL_Event e  ;
// rectangles for showing the scores
SDL_Rect rect1 = {25,25,150,40} ;
SDL_Rect rect2 = {WINDOW_WIDTH-borderwidth-150,25,150,40} ;
// score-system
int gamepoint =  0 ;
int overtime =  0 ;
int turn = 0 ;

//texture for rendering the graphics
SDL_Texture* textureText1 = NULL ;
SDL_Texture* textureText2 = NULL ;
SDL_Texture* backgroundTexture = NULL ;
SDL_Texture* wallTexture = NULL ;
SDL_Texture* iconTexture1 = NULL ;
SDL_Texture* iconTexture2 = NULL ;
SDL_Texture* midwallTexture = NULL ;
SDL_Texture* pausebuttonTexture = NULL ;
SDL_Texture* grassTexture = NULL ;
//global variables for mouse-controlled  ball-projectile
int mouseXStart, mouseYStart;
int mouseXEnd, mouseYEnd;
bool isDragging = false;
//for music
bool musicOn = true  ;

// all predeclaraction
void gameloop() ;
void HandleMenuEvents() ;
void gameloop()  ;
void user_input_window() ;
void handle_pauseMenu() ;
void handle_menu_after_result() ;

float min(float a,float b){
    if (a<b) return a ;
    else return b  ;
}

void reset_after_restart(){
    x1 = borderwidth ;
    x2 = WINDOW_WIDTH-borderwidth-ICON_WIDTH ;
    icon1y = WINDOW_HEIGHT-borderwidth-ICON_HEIGHT  ;
    icon2y = WINDOW_HEIGHT-ICON_HEIGHT-borderwidth ;
    score1= 0  ;
    score2= 0  ;
    ballOnIcon1 = false ;
    overtime = 0 ;
    turn = 0 ;
    isDragging = false ;
}
void reset_after_end(){
    x1 = borderwidth ;
    x2 = WINDOW_WIDTH-borderwidth-ICON_WIDTH ;
    icon1y = WINDOW_HEIGHT-borderwidth-ICON_HEIGHT  ;
    icon2y = WINDOW_HEIGHT-ICON_HEIGHT-borderwidth ;
    score1= 0  ;
    score2= 0  ;
    ballOnIcon1 = false ;
    strcpy(player1name,"player-1") ;
    strcpy(player2name,"player-2") ;
    strcpy(point,"0") ;
    overtime = 0 ;
    gamepoint = 0 ;
    turn = 0 ;
    isDragging  = false ;
}
void init(){ // getting the engine started
    // sdl2 initialization
    SDL_Init(SDL_INIT_EVERYTHING) ;
    // ttf format initialization
    TTF_Init() ;
    // image format initialization
    IMG_Init(IMG_INIT_JPG);
    IMG_Init(IMG_INIT_PNG) ;
    // audio format initialization
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    // background setup with custom img
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH,WINDOW_HEIGHT,0,&window,&renderer) ;
    SDL_Surface* backgroundSurface = IMG_Load("game bg2.png");
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);
    // border wall  coloring setup with custom img
    SDL_Surface* wallSurface = IMG_Load("wall.jpg");
    wallTexture = SDL_CreateTextureFromSurface(renderer, wallSurface);
    SDL_FreeSurface(wallSurface) ;
    // midwall /net setup with custom image
    SDL_Surface* midwallSurface = IMG_Load("net.png");
    midwallTexture = SDL_CreateTextureFromSurface(renderer, midwallSurface);
    SDL_Surface* pausebuttonSurface = IMG_Load("pause.png");
    pausebuttonTexture = SDL_CreateTextureFromSurface(renderer, pausebuttonSurface);
    SDL_FreeSurface(pausebuttonSurface);
    // sprite for player
    SDL_Surface* player1surface = IMG_Load("sprite1.png");
    player1Texture = SDL_CreateTextureFromSurface(renderer,player1surface);
    SDL_Surface* player2surface = IMG_Load("sprite2.png");
    player2Texture = SDL_CreateTextureFromSurface(renderer,player2surface);
    SDL_FreeSurface(player1surface) ;
    SDL_FreeSurface(player2surface) ;
}
void update_icon1_pos(){
    if (icon1y+icon1yspeed>WINDOW_HEIGHT-borderwidth-ICON_HEIGHT){
        icon1y = WINDOW_HEIGHT-borderwidth-ICON_HEIGHT ;
        icon1yspeed = 0 ;
        return ;
    }
    icon1yspeed += GRAVITY;
    icon1y += icon1yspeed ;
}
void update_icon2_pos(){
    if (icon2y+icon2yspeed>WINDOW_HEIGHT-borderwidth-ICON_HEIGHT){
        icon2y = WINDOW_HEIGHT-borderwidth-ICON_HEIGHT ;
        icon2yspeed = 0 ;
        return ;
    }
    icon2yspeed += GRAVITY ;
    icon2y += icon2yspeed ;
}

// following couple functions to handle the projectile ball movement
void renderBall() {
    int radius = BALL_SIZE / 2;
    int centerX = ballX + radius;
    int centerY = ballY + radius;
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                if (x < 0) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
                } else if (x == 0) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
                }
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void resetBall() {
    ballX = 1000;
    ballY = 1000;
    ballVelX = 0;
    ballVelY = 0;
}

void updateBallPosition() {
    ballVelY += GRAVITY;

    ballX += ballVelX;
    ballY += ballVelY;

    if (ballX <= borderwidth || ballX >= WINDOW_WIDTH - borderwidth - BALL_SIZE) { // side wall collision
        ballVelX = -ballVelX;
    }
    // net
    if ((ballX >= midwallx - BALL_SIZE && ballX <= midwallx + WALL_WIDTH) &&
        (ballY + BALL_SIZE >= midwally)) {
        if (ballVelX > 0) {
            ballX = midwallx - BALL_SIZE;
        } else {
            ballX = midwallx + WALL_WIDTH;
        }
        ballVelX = -ballVelX;
    }
    // icon collision
    bool hitIcon1 = ballX + BALL_SIZE > x1 && ballX < x1 + ICON_WIDTH &&
                    ballY + BALL_SIZE > icon1y && ballY < icon2y + ICON_HEIGHT;
    bool hitIcon2 = ballX + BALL_SIZE > x2 && ballX < x2 + ICON_WIDTH &&
                    ballY + BALL_SIZE > icon2y && ballY < icon2y + ICON_HEIGHT;

    if (hitIcon1 || hitIcon2) {
        // ball hit sound
        Mix_Chunk* ballhit = Mix_LoadWAV("ballhit.wav");
        Mix_PlayChannel(-1, ballhit, 0);
        float velocityMagnitude = sqrt(ballVelX * ballVelX + ballVelY * ballVelY);
        ballVelX = -ballVelX;
        ballVelY = -velocityMagnitude * sin(SHOOTING_ANGLE);

        if (hitIcon1) { // adjusting
            ballX = x1 + ICON_WIDTH;
        } else {
            ballX = x2 - BALL_SIZE;
        }
    }

    if (ballY <= borderwidth) { // top border hit
        ballVelY = -ballVelY;
    }
}

void shootBall(float speed) {
    // ball hit sound
    Mix_Chunk* ballhit = Mix_LoadWAV("ballhit.wav");
    Mix_PlayChannel(-1, ballhit, 0);
    if (ballOnIcon1) {
        ballX = x1 + ICON_WIDTH / 2 - BALL_SIZE / 2;
        ballY = icon1y - BALL_SIZE;
    } else {
        ballX = x2 + ICON_WIDTH / 2 - BALL_SIZE / 2;
        ballY = icon2y - BALL_SIZE;

    }
    if (dx > 0){
        ballVelX = speed ;
    }
    else{
        ballVelX = -speed ;
    }
    if (dy>0){
        ballVelY = speed * sin(SHOOTING_ANGLE);
    }
    else{
        ballVelY = -speed * sin(SHOOTING_ANGLE);
    }
}


bool isBallOutOfBounds() {
    return ballY > WINDOW_HEIGHT - borderwidth - BALL_SIZE;
}
bool fellonrightside(){
    return (ballX >= midwallx+WALL_WIDTH) ;
}
// ball handling function end just here
void render(){ // frames
    SDL_SetRenderDrawColor(renderer,0,0,0,255) ;
    SDL_RenderClear(renderer) ;
    // borders , midwall and icon declarations
    SDL_Rect leftwall = {0,0,borderwidth, WINDOW_HEIGHT} ;
    SDL_Rect rightwall = {WINDOW_WIDTH-borderwidth,0,borderwidth,WINDOW_HEIGHT} ;
    SDL_Rect upwall = {0,0,WINDOW_WIDTH,borderwidth} ;
    SDL_Rect downwall = {0,WINDOW_HEIGHT-borderwidth,WINDOW_WIDTH,borderwidth} ;
    SDL_Rect midwall = {midwallx,midwally,WALL_WIDTH,WALL_HEIGHT} ;
    SDL_Rect icon1 = {x1,icon1y,ICON_WIDTH,ICON_HEIGHT} ;
    SDL_Rect icon2 = {x2,icon2y,ICON_WIDTH,ICON_HEIGHT} ;
    // rect for pause button
    SDL_Rect r1 = {WINDOW_WIDTH/2-25,25,50,50} ;
    // src -> from sprite and dst -> location to paste
    SDL_Rect srcRect1 = { currentFrame1 * frameWidth*2 , 0, frameWidth, frameHeight };
    SDL_Rect srcRect2 = { currentFrame2 * frameWidth*2 , 0, frameWidth, frameHeight };
    // score-system setup (ttf)
    TTF_Font* ourfont = TTF_OpenFont("8bit.ttf", 32);
    char text1[20];
    snprintf(text1, sizeof(text1), "%s:%d",player1name, score1);
    char text2[20];
    snprintf(text2, sizeof(text2), "%s:%d",player2name, score2);
    SDL_Color textColor = {255, 0, 0}; // red
    SDL_Surface* surfaceText1 = TTF_RenderText_Solid(ourfont, text1, textColor);
    SDL_Surface* surfaceText2 = TTF_RenderText_Solid(ourfont, text2, textColor);
    textureText1 = SDL_CreateTextureFromSurface(renderer, surfaceText1);
    textureText2 = SDL_CreateTextureFromSurface(renderer, surfaceText2);
    SDL_FreeSurface(surfaceText1);
    SDL_FreeSurface(surfaceText2);
    // painting the borders,walls and icons
    SDL_SetRenderDrawColor(renderer,255,255,255,255) ;
    SDL_RenderFillRect(renderer,&leftwall) ;
    SDL_RenderFillRect(renderer,&rightwall) ;
    SDL_RenderFillRect(renderer,&upwall) ;
    SDL_RenderFillRect(renderer,&downwall) ;
    SDL_RenderFillRect(renderer,&midwall) ;
    SDL_RenderFillRect(renderer,&icon1) ;
    SDL_RenderFillRect(renderer,&icon2) ;
    //copying the image texture into background
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    // copying the image texture into the border and midwall rectangles
    SDL_RenderCopy(renderer, wallTexture, NULL, &leftwall);
    SDL_RenderCopy(renderer, wallTexture, NULL, &rightwall);
    SDL_RenderCopy(renderer, wallTexture, NULL, &upwall);
    SDL_RenderCopy(renderer, midwallTexture, NULL, &midwall);
    SDL_RenderCopy(renderer, wallTexture, NULL, &downwall);
    // copying the ttf texture into the rectangles
    SDL_RenderCopy(renderer, textureText1, NULL, &rect1);
    SDL_RenderCopy(renderer, textureText2, NULL, &rect2);
    // copying the pausebutton texture
    SDL_RenderCopy(renderer,pausebuttonTexture,NULL,&r1) ;
    // for sprite
    SDL_RenderCopy(renderer,player1Texture, &srcRect1, &icon1);
    SDL_RenderCopy(renderer,player2Texture, &srcRect2, &icon2);
    renderBall() ;
    SDL_RenderPresent(renderer);

}

bool ballthrown = false ;
void gameloop() {
    bool running = true ;
    // stop previous ones
    Mix_HaltMusic() ;
    // start crowd
    Mix_Music* bgm = Mix_LoadMUS("crowd.mp3");
    Mix_PlayMusic(bgm, -1);
    while (running) {
        if (isBallOutOfBounds() && ballthrown) {
            if (fellonrightside()) {
                score1++;
                ballOnIcon1 = true ;
            } else {
                score2++;
                ballOnIcon1 = false ;
            }
            ballthrown = false;
            resetBall() ;
        }
        if (score1 == score2 && (score1==(gamepoint+overtime-1))){
            overtime++ ;
        }
        if (score1 == gamepoint + overtime || score2 == gamepoint + overtime) {
            handle_menu_after_result() ;
            return ;
        }
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(0);
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case (SDLK_d):
                        if (x1 + iconspeed + ICON_WIDTH <= WINDOW_WIDTH / 2) {
                            dir1 = 1;
                        }
                        break;
                    case (SDLK_a):
                        if (x1 - iconspeed >= borderwidth) {
                            dir1 = -1;
                        }
                        break;
                    case (SDLK_RIGHT):
                        if (x2 + iconspeed + ICON_WIDTH <= WINDOW_WIDTH - borderwidth) {
                            dir2 = 1 ;
                        }
                        break;
                    case (SDLK_LEFT):
                        if (x2 - iconspeed >= WINDOW_WIDTH / 2 + WALL_WIDTH) {
                            dir2 = -1 ;
                        }
                        break;
                    case (SDLK_UP):
                        if (icon2y == WINDOW_HEIGHT-borderwidth-ICON_HEIGHT ){
                            icon2yspeed = -5 ;
                        }
                        break ;
                    case (SDLK_w):
                        if (icon1y == WINDOW_HEIGHT-borderwidth-ICON_HEIGHT ){
                            icon1yspeed = -5 ;
                        }
                        break ;
                }
            }
            else if(e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_a:
                    case SDLK_d:
                        dir1 = 0;
                        currentFrame1 = 0; // Return to frame 1 after key release
                        break;
                    case SDLK_RIGHT :
                    case SDLK_LEFT :
                        dir2 = 0 ;
                        currentFrame2 = 0 ;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    isDragging = true;
                    SDL_GetMouseState(&mouseXStart, &mouseYStart);
                    if (mouseXStart >= WINDOW_WIDTH/2-25 && mouseXStart <=WINDOW_WIDTH/2+25 && mouseYStart>=25 && mouseYStart<=25+50 ){
                        Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                        int channel = Mix_PlayChannel(-1, buttonTap, 0);
                        while (Mix_Playing(channel) != 0) {
                            SDL_Delay(1);
                        }
                        isDragging = false ;
                        handle_pauseMenu() ;
                        return ;
                    }
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT && isDragging) {
                    isDragging = false;
                    if (isBallOutOfBounds()) {
                        SDL_GetMouseState(&mouseXEnd, &mouseYEnd);
                        ballthrown = true;
                        int deltaX = mouseXEnd - mouseXStart;
                        int deltaY = mouseYEnd - mouseYStart;
                        if (deltaX == 0){
                            SHOOTING_ANGLE = M_PI/2 ;
                        }
                        else if (deltaY == 0){
                            SHOOTING_ANGLE = 0  ;
                        }
                        else{
                            SHOOTING_ANGLE = atan2(abs(deltaY), abs(deltaX));  // Angle in radians
                        }
                        float speed = sqrt(deltaX * deltaX + deltaY * deltaY) / 35.0;  // Adjust the divisor to change speed sensitivity
                        dx  = mouseXEnd-mouseXStart ;
                        dy = mouseYEnd-mouseYStart ;
                        shootBall(min(speed,(float)15));
                    }
                }
            }
        }
        Uint32 currentTime = SDL_GetTicks();
        if (dir1 != 0 && currentTime - frameStartTime >= animationSpeed) {
            currentFrame1 = (currentFrame1 + 1) % totalFrames;
            frameStartTime = currentTime ;
            x1 += iconspeed*dir1 ;
            if (x1<borderwidth){
                x1 = borderwidth ;
            }
            else if (x1+ICON_WIDTH>WINDOW_WIDTH / 2){
                x1 = WINDOW_WIDTH / 2 -ICON_WIDTH ;
            }
        }
        if (dir2 != 0 && currentTime - frameStartTime >= animationSpeed) {
            currentFrame2 = (currentFrame2 + 1) % totalFrames;
            frameStartTime = currentTime ;
            x2 += iconspeed*dir2 ;
            if (x2<WINDOW_WIDTH/2+WALL_WIDTH){
                x2 = WINDOW_WIDTH/2 + WALL_WIDTH ;
            }
            else if (x2+ICON_WIDTH>WINDOW_WIDTH-borderwidth){
                x2 = WINDOW_WIDTH-borderwidth -ICON_WIDTH ;
            }
        }
        update_icon1_pos() ;
        update_icon2_pos() ;
        updateBallPosition();
        render();
        // use delay if needed
        SDL_Delay(3);
    }
}

// user window
void user_input_window(){
    SDL_SetRenderDrawColor(renderer,0,0,0,255) ;
    SDL_RenderClear(renderer) ;
    // enter name request
    SDL_Color textColor = {255, 255, 255, 255}; // White color
    TTF_Font* font = TTF_OpenFont("8bit.ttf", 50); // Ensure the font file is in the correct location
    SDL_Surface* reqSurface = NULL ;
    if (turn ==  0 || turn == 1){
       reqSurface = TTF_RenderText_Solid(font,"Enter Name: ", textColor);
    }
    else {
        reqSurface = TTF_RenderText_Solid(font,"Enter Game Point: ", textColor);
    }
    SDL_Texture* reqTexture = SDL_CreateTextureFromSurface(renderer,reqSurface);
    SDL_FreeSurface(reqSurface) ;
    // rect for req text
    SDL_Rect r1 = {150,50,200,100} ;
    // menu background img setup
    SDL_Surface* MenuSurface = IMG_Load("namebg.png");
    SDL_Texture* MenuTexture = SDL_CreateTextureFromSurface(renderer, MenuSurface);
    SDL_FreeSurface(MenuSurface);
    SDL_Surface* textSurface =  NULL ;
    if (turn == 0){
        if (player1name[0] == '\0'){
            textSurface = TTF_RenderText_Solid(font, " ", textColor);
        }
        else textSurface = TTF_RenderText_Solid(font, player1name, textColor);
    }
    else if (turn == 1){
        if (player2name[0] == '\0'){
            textSurface = TTF_RenderText_Solid(font, " ", textColor);
        }
        else textSurface = TTF_RenderText_Solid(font, player2name, textColor);
    }
    else if (turn == 2){
        if (point[0] == '\0'){
            textSurface = TTF_RenderText_Solid(font, " ", textColor);
        }
        else  {
            textSurface = TTF_RenderText_Solid(font, point, textColor);
        }
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    int textWidth = textSurface->w;
    int textHeight = 40 ;
    SDL_Rect r2 = {150, 150, textWidth, textHeight};
    SDL_RenderCopy(renderer,MenuTexture,NULL,NULL) ;
    SDL_RenderCopy(renderer,reqTexture,NULL,&r1) ;
    SDL_RenderCopy(renderer, textTexture, NULL, &r2);
    SDL_RenderPresent(renderer);
}
void handleUserInputWindow(){
    SDL_StartTextInput();
    bool running  = true;
    SDL_Event e;
    user_input_window() ;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(0) ;
            } else if (e.type == SDL_TEXTINPUT) {
                if ( (turn ==  0) && (strlen(player1name) + strlen(e.text.text) < 20 - 1)) {
                    strcat(player1name, e.text.text);

                }
                else if ( (turn ==  1) && (strlen(player2name) + strlen(e.text.text) < 20 - 1)) {
                    strcat(player2name, e.text.text);
                }
                else if ((turn ==  2) && (strlen(point) + strlen(e.text.text) < 10 - 1)){
                    strcat(point, e.text.text);
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    turn++ ;
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    if (turn == 3){
                        gamepoint = atoi(point) ;
                        gameloop() ;
                        return ;
                    }
                }
                else if ( (turn == 0) && (e.key.keysym.sym == SDLK_BACKSPACE && strlen(player1name) > 0)) {
                    player1name[strlen(player1name) - 1] = '\0';
                }
                else if ( (turn == 1) && (e.key.keysym.sym == SDLK_BACKSPACE && strlen(player2name) > 0)) {
                    player2name[strlen(player2name) - 1] = '\0';
                }
                else if ((turn==2)&& (e.key.keysym.sym == SDLK_BACKSPACE && strlen(point) > 0)){
                    point[strlen(point) - 1] = '\0';
                }
            }
        }
        user_input_window() ;
    }
}

// after game is done
void render_menu_after_result(){
    // clearing the frame
    SDL_SetRenderDrawColor(renderer,0,0,0,255) ;
    SDL_RenderClear(renderer) ;
    // menu background img setup
    SDL_Surface* MenuSurface = IMG_Load("after_win.png");
    SDL_Texture* MenuTexture = SDL_CreateTextureFromSurface(renderer, MenuSurface);
    SDL_FreeSurface(MenuSurface);
    //setting up for the buttons
    SDL_Surface* restartSurface = IMG_Load("restart.png") ;
    SDL_Texture* restartTexture = SDL_CreateTextureFromSurface(renderer,restartSurface) ;
    SDL_FreeSurface(restartSurface) ;
    SDL_Surface* mainmenuSurface = IMG_Load("mainmenu.png") ;
    SDL_Texture* mainmenuTexture = SDL_CreateTextureFromSurface(renderer,mainmenuSurface) ;
    SDL_FreeSurface(mainmenuSurface) ;
    // ttf text setup
    TTF_Font* ourfont = TTF_OpenFont("arial.ttf", 32);
    //SDL_Rect rect = {640/2,480/2-100,250,100} ;
    SDL_Rect playerRect = {640/2+115,480/2+200,100,25} ;
    //char text1[50];
    SDL_Color TextColor  = {255,255,255} ;
    SDL_Texture* playerTexture = NULL ;
    if (score1==gamepoint+overtime){
        SDL_Surface* playerSurface = TTF_RenderText_Solid(ourfont,player1name,TextColor) ;
        playerTexture = SDL_CreateTextureFromSurface(renderer,playerSurface) ;
        SDL_FreeSurface(playerSurface) ;
    }
    else{
        SDL_Surface* playerSurface = TTF_RenderText_Solid(ourfont,player2name,TextColor) ;
        playerTexture = SDL_CreateTextureFromSurface(renderer,playerSurface) ;
        SDL_FreeSurface(playerSurface) ;
    }
    // setting up rectangles to render the menu buttons
    SDL_Rect r1 = {WINDOW_WIDTH/2-250,WINDOW_HEIGHT/2+200,150,75} ;
    SDL_Rect r2 = {WINDOW_WIDTH/2+50,WINDOW_HEIGHT/2+200,150,75} ;
    // copying the menu background img texture into menu-window
    SDL_RenderCopy(renderer,MenuTexture,NULL,NULL) ;
    // copying the ttf texture into the rect
    SDL_RenderCopy(renderer,restartTexture,NULL,&r1) ;
    SDL_RenderCopy(renderer,mainmenuTexture,NULL,&r2) ;
    SDL_RenderCopy(renderer,playerTexture,NULL,&playerRect) ;
    SDL_RenderPresent(renderer) ;
}

void handle_menu_after_result(){
    SDL_Event e ;
    bool running   = true  ;
    Mix_HaltMusic() ;
    Mix_Music* bgm = Mix_LoadMUS("after_win.mp3");
    Mix_PlayMusic(bgm, 1) ;
    render_menu_after_result() ;
    while(running){
        while(SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                exit(0) ;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                int mouseX , mouseY ;
                SDL_GetMouseState(&mouseX,&mouseY) ;
                if (mouseX >= WINDOW_WIDTH/2-250 && mouseX <=WINDOW_WIDTH/2-250+150 && mouseY>=WINDOW_HEIGHT/2+150+50 && WINDOW_HEIGHT/2+150+75+50){
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    Mix_HaltMusic();
                    reset_after_restart() ;
                    gameloop() ;
                    return ;
                }
                else if (mouseX >= WINDOW_WIDTH/2+50 && mouseX <=WINDOW_WIDTH/2+200 && mouseY>=WINDOW_HEIGHT/2+200 && mouseY<=WINDOW_HEIGHT/2+200+75){
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    Mix_HaltMusic();
                    reset_after_end() ;
                    HandleMenuEvents() ;
                    return ;
                }
            }
        }
    }
}

void RenderMenu(){ // menu window
    // clearing the frame
    SDL_SetRenderDrawColor(renderer,0,0,0,255) ;
    SDL_RenderClear(renderer) ;
    // menu background img setup
    SDL_Surface* MenuSurface = IMG_Load("background2.png");
    SDL_Texture* MenuTexture = SDL_CreateTextureFromSurface(renderer, MenuSurface);
    SDL_FreeSurface(MenuSurface);
    //setting up for the buttons
    SDL_Surface* startSurface = IMG_Load("startbutton.png") ;
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer,startSurface) ;
    SDL_FreeSurface(startSurface) ;
    SDL_Surface* exitSurface = IMG_Load("exitbutton.png") ;
    SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(renderer,exitSurface) ;
    SDL_FreeSurface(exitSurface) ;
    SDL_Surface* musicSurface = IMG_Load("musicbutton.png") ;
    SDL_Texture* musicTexture = SDL_CreateTextureFromSurface(renderer,musicSurface) ;
    SDL_FreeSurface(musicSurface) ;
    // setting up rectangles to render the menu buttons
    SDL_Rect r1 = {WINDOW_WIDTH/2-250,WINDOW_HEIGHT/2+150,150,75} ;
    SDL_Rect r2 = {WINDOW_WIDTH/2+50,WINDOW_HEIGHT/2+150,150,75} ;
    SDL_Rect r3 = {WINDOW_WIDTH-borderwidth-150,25,125,50} ;
    // copying the menu background img texture into menu-window
    SDL_RenderCopy(renderer,MenuTexture,NULL,NULL) ;
    // copying the ttf texture into the rect
    SDL_RenderCopy(renderer,startTexture,NULL,&r1) ;
    SDL_RenderCopy(renderer,exitTexture,NULL,&r2) ;
    SDL_RenderCopy(renderer,musicTexture,NULL,&r3) ;
    SDL_RenderPresent(renderer) ;
}

void HandleMenuEvents(){ // handling menu user i/p
    bool running = true  ;
    SDL_Event e ;
    if (musicOn){
        Mix_Music* bgm = Mix_LoadMUS("music.mp3");
        Mix_PlayMusic(bgm, -1);
    }
    RenderMenu() ;
    while(running){
        while(SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                exit(0) ;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                int mouseX , mouseY ;
                SDL_GetMouseState(&mouseX,&mouseY) ;
                if (mouseX >= WINDOW_WIDTH/2-250 && mouseX <=WINDOW_WIDTH/2-250+150 && mouseY>=WINDOW_HEIGHT/2+150 && mouseY<=WINDOW_HEIGHT/2+150+75){
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    handleUserInputWindow() ;
                }
                else if (mouseX >= WINDOW_WIDTH/2+50 && mouseX <=WINDOW_WIDTH/2+200 && mouseY>=WINDOW_HEIGHT/2+150 && mouseY<=WINDOW_HEIGHT/2+150+75){
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    exit(0) ;
                }
                else if (mouseX >=WINDOW_WIDTH-borderwidth-150 && mouseX<=WINDOW_WIDTH-borderwidth-150+125 && mouseY>=25 && mouseY<=25+50){
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    if (musicOn){
                         Mix_HaltMusic();
                    }
                    else {
                        Mix_Music* bgm = Mix_LoadMUS("music.mp3");
                        Mix_PlayMusic(bgm, -1);
                    }
                    musicOn = !musicOn ;
                }
            }
        }
    }
}

void renderPauseMenu(){
    // clearing the frame
    SDL_SetRenderDrawColor(renderer,0,0,0,255) ;
    SDL_RenderClear(renderer) ;
    // menu background img setup
    SDL_Surface* MenuSurface = IMG_Load("blankmenu.png");
    SDL_Texture* MenuTexture = SDL_CreateTextureFromSurface(renderer, MenuSurface);
    SDL_FreeSurface(MenuSurface);
    //setting up for the buttons
    SDL_Surface* continuebuttonSurface = IMG_Load("continue.png") ;
    SDL_Texture* continuebuttonTexture = SDL_CreateTextureFromSurface(renderer,continuebuttonSurface) ;
    SDL_FreeSurface(continuebuttonSurface) ;
    SDL_Surface* mainMenubuttonSurface = IMG_Load("mainmenu.png") ;
    SDL_Texture* mainMenubuttonTexture = SDL_CreateTextureFromSurface(renderer,mainMenubuttonSurface) ;
    SDL_FreeSurface(mainMenubuttonSurface) ;
    SDL_Surface* musicSurface = IMG_Load("musicbutton.png") ;
    SDL_Texture* musicTexture = SDL_CreateTextureFromSurface(renderer,musicSurface) ;
    SDL_FreeSurface(musicSurface) ;
    SDL_Rect r1 = {WINDOW_WIDTH/2-100,WINDOW_HEIGHT/2-100,150,75} ;
    SDL_Rect r2 = {WINDOW_WIDTH/2-100,WINDOW_HEIGHT/2,150,75} ;
    SDL_Rect r3 = {WINDOW_WIDTH-borderwidth-150,25,125,50} ;
    SDL_RenderCopy(renderer,MenuTexture,NULL,NULL) ;
    SDL_RenderCopy(renderer,continuebuttonTexture,NULL,&r1) ;
    SDL_RenderCopy(renderer,mainMenubuttonTexture,NULL,&r2) ;
    SDL_RenderCopy(renderer,musicTexture,NULL,&r3) ;
    SDL_RenderPresent(renderer) ;
}

void handle_pauseMenu(){
    bool running = true  ;
    SDL_Event e ;
    if (musicOn){
        Mix_Music* bgm = Mix_LoadMUS("music.mp3");
        Mix_PlayMusic(bgm, -1); // inf loop
    }
    renderPauseMenu() ;
    while(running){
        while(SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                exit(0) ;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                int mouseX , mouseY ;
                SDL_GetMouseState(&mouseX,&mouseY) ;
                if (mouseX >= WINDOW_WIDTH/2-100 && mouseX <=WINDOW_WIDTH/2-100+150 && mouseY>=WINDOW_HEIGHT/2-100 && mouseY<= WINDOW_HEIGHT/2-100+75){
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    gameloop() ;
                    return ;
                }
                else if (mouseX >= WINDOW_WIDTH/2-100 && mouseX <=WINDOW_WIDTH/2-100+150 && mouseY>=WINDOW_HEIGHT/2 && mouseY<=WINDOW_HEIGHT/2+75){
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    reset_after_end() ;
                    HandleMenuEvents() ;
                    return ;
                }
                else if (mouseX >=WINDOW_WIDTH-borderwidth-150 && mouseX<=WINDOW_WIDTH-borderwidth-150+125 && mouseY>=25 && mouseY<=25+50){
                    Mix_Chunk* buttonTap = Mix_LoadWAV("click.wav");
                    int channel = Mix_PlayChannel(-1, buttonTap, 0);
                    while (Mix_Playing(channel) != 0) {
                        SDL_Delay(1);
                    }
                    if (musicOn){
                         Mix_HaltMusic();
                    }
                    else {
                        Mix_Music* bgm = Mix_LoadMUS("music.mp3");
                        Mix_PlayMusic(bgm, -1);
                    }
                    musicOn = !musicOn ;
                }
            }
        }
    }
}

int main(int argc, char* argv[]){ // managing the functions
    init() ;
    HandleMenuEvents() ;
    return 0 ;
}
