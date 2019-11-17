#include "include/constant.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static sfTexture *background;
static sfTexture *button;
static sfTexture *game_background;
static sfTexture *titre;

static int x = 0;
static int y = 0;

static sfVector2i holePos[10];
static sfVector2i hitPos[10];
static sfVector2i killPos[10];

static sfVector2i doritos[100];
static sfVector2i mlg[100];
static sfVector2i mountain[100];

static int KILLS = 0;

typedef struct ennemi {
    sfVector2i position;
    sfVector2i initialposition;
    int hp;

} ennemi_t;

static ennemi_t mechan[10];

void draw_square(float x, float y, float size, float r, float g, float b, float a)
{
			glBegin(GL_QUADS);
			glColor4f(r, g, b, a);
			glTexCoord2f((0), (0));glVertex2f(x, y);
			glTexCoord2f((1), (0));glVertex2f(x + size, y);
			glTexCoord2f((1), (1));glVertex2f(x + size, y + size);
			glTexCoord2f((0), (1));glVertex2f(x, y + size);
			glEnd();
}

void draw_rect(float x, float y, float w, float h, float r, float g, float b, float a)
{
			glBegin(GL_QUADS);
			glColor4f(r, g, b, a);
            glTexCoord2f((0), (0));glVertex2f(x, y);
            glTexCoord2f((1), (0));glVertex2f(x + w, y);
            glTexCoord2f((1), (1));glVertex2f(x + w, y + h);
            glTexCoord2f((0), (1));glVertex2f(x, y + h);
			glEnd();
}

void initGL()
{
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void updateGL()
{
    glClearColor(0, 0, 0, 1);
}

void render_menu(sfVector2i mousePosition)
{
    if(mousePosition.x < WIDTH/3 && mousePosition.y > HEIGHT - HEIGHT/3) {
        sfTexture_bind(button);
        draw_rect(0,HEIGHT - HEIGHT/3,WIDTH/3, HEIGHT/3, 1, 1, 1, 1);
        x++;
        y++;
        sfTexture_bind(button);
        draw_rect(50,HEIGHT - HEIGHT/3,WIDTH/3, HEIGHT/3, 1, 1, 1, 0.5f);
    } else {
        x = 0;
        y = 0;
        sfTexture_bind(background);
        draw_rect(0,0,WIDTH, HEIGHT, 1, 1, 1, 1);
        sfTexture_bind(button);
        draw_rect(0,HEIGHT - HEIGHT/3,WIDTH/3, HEIGHT/3, 1, 1, 1, 0.5f);
    }
    glDisable(GL_BLEND);
    sfTexture_bind(titre);
    draw_rect(WIDTH/2 - WIDTH /6 + x,HEIGHT/12 + y,WIDTH/3, HEIGHT/6, 1, 1, 1, 1);
    glEnable(GL_BLEND);
}


void render_game(sfVector2i mousePosition)
{
    y ++;
    if(y > 0) {
        if(mousePosition.x > WIDTH/2 && mousePosition.x < WIDTH/2 + 10) {
            sfTexture_bind(game_background);
            draw_rect(-WIDTH,0,WIDTH*4, HEIGHT, 1, 1, 1, 1);
            y = -100;
        } else {
            sfTexture_bind(game_background);
            draw_rect(0,0,WIDTH, HEIGHT, 1, 1, 1, 1);
            y = 0;
        }
    }
}

int main (void)
{
    sfVideoMode mode = {WIDTH, HEIGHT, 32};
    sfRenderWindow* window;
    sfEvent event;
    int holeindex = 0;
    int hitindex = 0;
    int killindex = 0;
    int inMenu = 1;
    int click = 0;
    int shot = 0;

    int lock = 0;

    sfVector2i mousePosition = {0,0};
    sfSound *music;
    music = sfSound_create();
    sfSoundBuffer *soundBuffer;
    soundBuffer = sfSoundBuffer_createFromFile("res/song.ogg");
    sfSound_setBuffer(music, soundBuffer);
    sfSound *sound;
    sound = sfSound_create();
    soundBuffer = sfSoundBuffer_createFromFile("res/awp.wav");
    sfSound_setBuffer(sound, soundBuffer);

    sfSound *death;
    death = sfSound_create();
    soundBuffer = sfSoundBuffer_createFromFile("res/death1.wav");
    sfSound_setBuffer(death, soundBuffer);

    sfSound *hit;
    hit = sfSound_create();
    soundBuffer = sfSoundBuffer_createFromFile("res/hit.wav");
    sfSound_setBuffer(hit, soundBuffer);

    background = sfTexture_createFromFile("res/menu-background.jpg", NULL);
    sfTexture *cursor;
    game_background = sfTexture_createFromFile("res/ranking-panel.png", NULL);
    sfTexture *bullet_hole;
    bullet_hole = sfTexture_createFromFile("res/cursor-smoke.png", NULL);

    sfTexture *blood;
    blood = sfTexture_createFromFile("res/hit0.png", NULL);

    sfTexture *hit_marker;
    hit_marker = sfTexture_createFromFile("res/lighting.png", NULL);

    titre = sfTexture_createFromFile("res/titre.png", NULL);
    cursor = sfTexture_createFromFile("res/cursor.png", NULL);
    button = sfTexture_createFromFile("res/play-skip.jpg", NULL);

    sfTexture *idle;
    idle = sfTexture_createFromFile("res/fruit-catcher-idle.png", NULL);

    sfTexture *hitted;
    hitted = sfTexture_createFromFile("res/fruit-catcher-fail.png", NULL);

    sfFont *font = sfFont_createFromFile("res/COMIC.TTF");
    sfText *text = sfText_create();
    sfText_setFont(text, font);
    sfText *textkc = sfText_create();
    sfText_setFont(textkc, font);

    window = sfRenderWindow_create(mode, "OpenGL", sfDefaultStyle, NULL);
    sfRenderWindow_setMouseCursorVisible(window, sfFalse);
    sfRenderWindow_setFramerateLimit(window, 60);

    sfVector2f textposition = {0, HEIGHT - 50};
    sfVector2f textkcposition = {70, HEIGHT - 50};

    initGL();
    sfSound_setPitch(music, 0.8f);
    sfSound_setPitch(sound, 0.8f);
    sfSound_setPitch(death, 0.8f);
    sfSound_setVolume (sound, 5.0f);
    sfSound_setPitch(hit, 0.8f);
    sfSound_setVolume (hit, 5.0f);
    sfSound_setVolume (death, 50.0f);
    sfSound_setLoop (music, sfTrue);
    sfSound_play(music);

    srand(time(NULL));

    for(int i = 0; i < 10; i ++) {
        mechan[i].position.x = (rand() % 2) * WIDTH*2 - 500;
        mechan[i].position.y = (rand() % HEIGHT-100);
        mechan[i].initialposition.x = mechan[i].position.x;
        mechan[i].initialposition.y = mechan[i].position.y;
        mechan[i].hp = 2;
    }

    while (sfRenderWindow_isOpen(window)) {
        while (sfRenderWindow_pollEvent(window, &event))
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        updateGL();
        if(inMenu) {
                if(mousePosition.x < WIDTH/3 && mousePosition.y > HEIGHT - HEIGHT/3 && sfMouse_isButtonPressed(sfMouseLeft) && !click) {
                    x = 0;
                    y = 0;
                    for(int i = 0; i < 10; i ++) {
                        holePos[i].x = 0;
                        holePos[i].y = 0;
                    }
                    inMenu = 0;
                }

                render_menu(mousePosition);
            } else {
                render_game(mousePosition);
            }
            //Always draw cursor last
            if(!inMenu) {

            for(int i = 0; i < 10; i ++) {
                if(mechan[i].initialposition.x == -500) {
                    mechan[i].position.x += (rand() % 30) - (rand() % 10);
                } else {
                    mechan[i].position.x -= (rand() % 30)-(rand() % 10);
                }
                mechan[i].position.y += sin(mechan[i].position.x)*20;
                if(mechan[i].position.x < -500 || mechan[i].position.x > 2060) {
                    mechan[i].position.x = mechan[i].initialposition.x;
                    mechan[i].hp = 2;
                }
                if(mousePosition.y < mechan[i].position.y + 200 && mousePosition.y > mechan[i].position.y && mousePosition.x < mechan[i].position.x + 150 && mousePosition.x > mechan[i].position.x && sfMouse_isButtonPressed(sfMouseLeft) && !click) {
                    click = 1;
                    mechan[i].hp--;
                    if(mechan[i].hp == 1) {
                        sfSound_play(hit);
                        hitPos[hitindex].x = mousePosition.x;
                        hitPos[hitindex].y = mousePosition.y;
                        hitindex ++;
                        if(hitindex >= 10) {
                            hitindex = 0;
                        }
                    }
                    if(mechan[i].hp == 0) {
                        sfSound_play(death);
                        KILLS++;
                        killPos[killindex].x = mousePosition.x;
                        killPos[killindex].y = mousePosition.y;
                        killindex ++;
                        if(killindex >= 10) {
                            killindex = 0;
                        }
                        mechan[i].position.x = mechan[i].initialposition.x;
                        mechan[i].hp == 2;
                    }
                }
                if(mechan[i].hp == 2) {
                    sfTexture_bind(idle);
                    draw_rect(mechan[i].position.x + x,mechan[i].position.y + y,150, 200, 1, 1, 1, 1);
                }
                if(mechan[i].hp == 1) {
                    sfTexture_bind(hitted);
                    draw_rect(mechan[i].position.x + x,mechan[i].position.y + y,150, 200, 1, 1, 1, 1);
                }
            }
        }

        sfTexture_bind(bullet_hole);
        for(int i = 0; i < 10; i ++) {
            if(holePos[i].x > 0 && holePos[i].y > 0) {
                draw_rect(holePos[i].x-25 + x,holePos[i].y-25 + y,50, 50, 1, 1, 1, 1);
            }
        }
        glDisable(GL_BLEND);
        sfTexture_bind(blood);
        for(int i = 0; i < 10; i ++) {
            if(killPos[i].x > 0 && killPos[i].y > 0) {
                draw_rect(killPos[i].x-25 + x,killPos[i].y-25 + y,50, 50, 1, 1, 1, 1);
            }
        }
        glEnable(GL_BLEND);
        sfTexture_bind(hit_marker);
        for(int i = 0; i < 10; i ++) {
            if(hitPos[i].x > 0 && hitPos[i].y > 0) {
                draw_rect(hitPos[i].x-25 + x,hitPos[i].y-25 + y,50, 50, 1, 1, 1, 1);
            }
        }


        sfTexture_bind(cursor);
        mousePosition = sfMouse_getPositionRenderWindow(window);
        draw_rect(mousePosition.x - WIDTH / 2, mousePosition.y - HEIGHT / 2, WIDTH, HEIGHT, 1, 1, 1, 1);
        sfTexture_bind(NULL);

        if(sfMouse_isButtonPressed(sfMouseLeft) && !click) {
            sfSound_play(sound);
            holePos[holeindex].x = mousePosition.x;
            holePos[holeindex].y = mousePosition.y;
            holeindex ++;
            if(holeindex >= 10) {
                holeindex = 0;
            }
        }
        if(!inMenu) {

            sfText_setColor (text, sfRed);
            sfText_setPosition (text, textposition);
            sfText_setString (text, "KIL : ");

            sfText_setColor (textkc, sfRed);
            sfText_setPosition (textkc, textkcposition);
            char KILLCOUNT[5];
            sprintf(KILLCOUNT, "%d", KILLS);
            sfText_setString (textkc, KILLCOUNT);
        }

        if(sfMouse_isButtonPressed(sfMouseLeft)) {
            click = 1;
            lock++;
            if(lock >= 7) {
                click = 0;
                lock = 0;
            }
        } else {
            click = 0;
        }
        sfRenderWindow_pushGLStates(window);
        sfRenderWindow_drawText(window, text, NULL);
        sfRenderWindow_drawText(window, textkc, NULL);
        sfRenderWindow_popGLStates(window);
        sfRenderWindow_display(window);
    }
    sfRenderWindow_destroy(window);
    return (0);
}
