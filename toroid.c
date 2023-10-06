#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#define LIMIT_FPS 24
#define FRAME_TIME 1000.0/LIMIT_FPS
#define DIV 150

const size_t WIDTH = 180;
const size_t HEIGHT = 80;
const float STEP = 0.01;
const float R = 2;
const float r = 1;
const float X_AXIS_ROTATION_SPEED = 0.4;
const float Y_AXIS_ROTATION_SPEED = 0.25;
const float Z_AXIS_ROTATION_SPEED = 0.25;
const float DISTANCE = 15;
const char* SHADES = ".:,!;-~*=#$@";
const float FOV = 90;
// text scale = 156*288

const float LIGHT_VECTOR_OG[] = { 0, -1 , -1 };

void rotation_x(float* x, float* y, float* z, float angle);
void rotation_y(float* x, float* y, float* z, float angle);
void rotation_z(float* x, float* y, float* z, float angle);
const float* normalizate_vector(const float* vector);


void rotation_x(float* x, float* y, float* z, float angle) {
    float tempY = *y;

    *y = *y * cos(angle) - *z * sin(angle);
    *z = tempY * sin(angle) + *z * cos(angle);
}

void rotation_y(float* x, float* y, float* z, float angle) {
    float tempX = *x;

    *x = *x * cos(angle) + *z * sin(angle);
    *z = -tempX * sin(angle) + *z * cos(angle);
}

void rotation_z(float* x, float* y, float* z, float angle) {
    float tempX = *x;

    *x = *x * cos(angle) - *y * sin(angle);
    *y = tempX * sin(angle) + *y * cos(angle);
}

const float* normalize_vector(const float* vector) {
    float* output = calloc(3, sizeof(float));
    float mod = vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
    mod = 1 / sqrt(mod);
    output[0] = vector[0] * mod;
    output[1] = vector[1] * mod;
    output[2] = vector[2] * mod;
    return (const float*)output;
}

int main() {

    const int NUMBER_SHADES = strlen(SHADES);
    const float* LIGHT_VECTOR = normalize_vector(LIGHT_VECTOR_OG);

    const float SCALE = 288.0 / 156.0;

    const float SCREEN_DIST = WIDTH / tan(FOV * M_PI / 180 / 2);


    float betta, alpha;
    float x, y, z;
    float nx, ny, nz;
    float normX, normY, normZ;
    float light;
    float projected_x, projected_y;
    float xRotationAngle = 0, yRotationAngle = 0, zRotationAngle = 0;

    float zBuffer[HEIGHT][WIDTH];
    float screen[HEIGHT][WIDTH];
    char color[HEIGHT][WIDTH];
    time_t start;
    struct timespec tstart = { 0,0 }, tend = { 0,0 };


    printf("\x1b[2J");

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        memset(zBuffer, 0, (HEIGHT * WIDTH) * sizeof(float));
        memset(screen, -1, (HEIGHT * WIDTH) * sizeof(float));
        memset(color, -1, (HEIGHT * WIDTH) * sizeof(char));

        for (betta = 0; betta < 2 * M_PI; betta += STEP) {
            for (alpha = 0; alpha < 2 * M_PI; alpha += STEP) {



                // Coords of toroid
                x = (R + r * cos(alpha));
                y = 0;
                z = r * sin(alpha);
                rotation_z(&x, &y, &z, betta + zRotationAngle);

                // ROTATION X
                rotation_x(&x, &y, &z, xRotationAngle);

                // ROTATION Y
                rotation_y(&x, &y, &z, yRotationAngle);

                // TRANSLATION
                z += DISTANCE;

                // printf("%d\n", light);
                // ADD TO Z BUFFER
                projected_x = x * SCALE * SCREEN_DIST / z + WIDTH / 2;
                projected_y = y * SCREEN_DIST / z + HEIGHT / 2;


                if (z > 0 && projected_x >= 0 && projected_x < WIDTH && projected_y >= 0 && projected_y < HEIGHT) {
                    if (zBuffer[(int)projected_y][(int)projected_x] == 0 || z < zBuffer[(int)projected_y][(int)projected_x]) {
                        zBuffer[(int)projected_y][(int)projected_x] = z;


                        // APPLY LIGHT
                        normX = cos(alpha);
                        normY = 0;
                        normZ = sin(alpha);
                        rotation_z(&normX, &normY, &normZ, betta + zRotationAngle);

                        // ROTATION X
                        rotation_x(&normX, &normY, &normZ, xRotationAngle);

                        // ROTATION Y
                        rotation_y(&normX, &normY, &normZ, yRotationAngle);
                        if (-x * normX - y * normY - z * normZ > 0) {
                            light = normX * LIGHT_VECTOR[0] + normY * LIGHT_VECTOR[1] + normZ * LIGHT_VECTOR[2];
                            screen[(int)projected_y][(int)projected_x] = light > 0 ? light : 0;
                            color[(int)projected_y][(int)projected_x] = (char)(betta / 0.5 / M_PI);
                        }
                    }
                }

            }
        }


        printf("\x1b[H");
        // draw screen
        for (size_t i = 0; i < HEIGHT; i++) {
            for (size_t j = 0; j < WIDTH; j++) {
                light = screen[i][j];
                if (light >= 0) {
                    switch (color[i][j]) {
                        case 0:
                            printf("\x1B[31m");
                            break;
                        case 1:
                            printf("\x1B[32m");
                            break;
                        case 2:
                            printf("\x1B[34m");
                            break;
                        case 3:
                            printf("\x1B[33m");
                            break;
                    }
                    putchar(SHADES[(int)roundf((NUMBER_SHADES - 1) * light)]);
                }
                else {
                    putchar(' ');
                }
            }
            putchar('\n');
        }
        printf("\x1B[0m");

        clock_gettime(CLOCK_MONOTONIC, &tend);
        double timeErased = ((double)1.0e3 * tend.tv_sec + 1.0e-6 * tend.tv_nsec) - ((double)1.0e3 * tstart.tv_sec + 1.0e-6 * tstart.tv_nsec);
        // printf("Erased time: %f\n", timeErased);
        // printf("Objetive: %f\n", FRAME_TIME);
        if (timeErased < FRAME_TIME) {
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = (FRAME_TIME - timeErased) * 1.0e6;
            // printf("Delay: %f\n", ts.tv_nsec / 1.0e6);
            nanosleep(&ts, NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &tend);
        timeErased = ((double)1.0e3 * tend.tv_sec + 1.0e-6 * tend.tv_nsec) - ((double)1.0e3 * tstart.tv_sec + 1.0e-6 * tstart.tv_nsec);


        printf("%f FPS\n", 1000 / timeErased);
        // CHANGE ANGLE
        xRotationAngle += X_AXIS_ROTATION_SPEED * timeErased / DIV;
        yRotationAngle += Y_AXIS_ROTATION_SPEED * timeErased / DIV;
        zRotationAngle += Z_AXIS_ROTATION_SPEED * timeErased / DIV;
        xRotationAngle -= xRotationAngle >= 2 * M_PI ? 2 * M_PI : 0;
        yRotationAngle -= yRotationAngle >= 2 * M_PI ? 2 * M_PI : 0;
        zRotationAngle -= zRotationAngle >= 2 * M_PI ? 2 * M_PI : 0;


    }

    /*
    Did they not teach memory freeing there in that crystal ball known as UEX, or were you too busy brown-nosing the professors or
    focusing on your bobblehead-figurine-like nodding in class to feign attention?
        */
    free((void*)LIGHT_VECTOR);

    return EXIT_SUCCESS;
}
