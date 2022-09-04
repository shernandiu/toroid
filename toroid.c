#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const size_t WIDTH = 80;
const size_t HEIGHT = 80;
const float STEP = 0.01;
const float R = 2;
const float r = 1;
const float X_AXIS_ROTATION_SPEED = 0.00;
const float Y_AXIS_ROTATION_SPEED = 0.005;
const float SCREEN_DIST = 50;
const char* SHADES = ".,-~:;=!*#$@";
const float DISTANCE = 5;

void rotation_x(float* x, float* y, float* z, float angle);
void rotation_y(float* x, float* y, float* z, float angle);
void rotation_z(float* x, float* y, float* z, float angle);

void rotation_x(float* x, float* y, float* z, float angle) {
    float tempY = *y;

    *y = *y * cos(angle) - *z * sin(angle);
    *z = tempY * sin(angle) - *z * cos(angle);
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


int main() {
    const int NUMBER_SHADES = strlen(SHADES);
    const float LIGHT_VECTOR[] = { 0, 0, -1 };


    float betta, alpha;
    float x, y, z;
    float nx, ny, nz;
    float normX, normY, normZ;
    float light;
    float projected_x, projected_y;
    float xRotationAngle = 0, yRotationAngle = 0;

    float zBuffer[HEIGHT][WIDTH];
    char screen[HEIGHT][WIDTH];

    printf("\x1b[2J");

    while (1) {

        memset(zBuffer, 0, (HEIGHT * WIDTH) * sizeof(float));
        memset(screen, ' ', (HEIGHT * WIDTH) * sizeof(char));

        for (betta = 0; betta < 2 * M_PI; betta += STEP) {
            for (alpha = 0; alpha < 2 * M_PI; alpha += STEP) {
                // Coords of toroid
                x = (R + r * cos(alpha)) * cos(betta);
                y = (R + r * cos(alpha)) * sin(betta);
                z = r * sin(alpha);

                // ROTATION X
                rotation_x(&x, &y, &z, xRotationAngle);

                // ROTATION Y
                rotation_y(&x, &y, &z, yRotationAngle);

                // TRANSLATION
                z += DISTANCE;

                // APPLY LIGHT
                // norms
                normX = cos(alpha);
                normY = 0;
                normZ = sin(alpha);
                rotation_z(&normX, &normY, &normZ, betta);

                // ROTATION X
                rotation_x(&normX, &normY, &normZ, xRotationAngle);

                // ROTATION Y
                rotation_y(&normX, &normY, &normZ, yRotationAngle);


                light = normZ < 0 ? normX * LIGHT_VECTOR[0] + normY * LIGHT_VECTOR[1] + normZ * LIGHT_VECTOR[2] : 0;
                // printf("%d\n", light);
                // ADD TO Z BUFFER
                projected_x = x * SCREEN_DIST / z + WIDTH / 2;
                projected_y = y * SCREEN_DIST / z + HEIGHT / 2;
                if (projected_x >= 0 && projected_x < WIDTH && projected_y >= 0 && projected_y < HEIGHT)
                    zBuffer[(int)projected_y][(int)projected_x] = light >= 0 ? light : 0;

            }
        }
        // CHANGE ANGLE
        xRotationAngle += X_AXIS_ROTATION_SPEED;
        yRotationAngle += Y_AXIS_ROTATION_SPEED;
        xRotationAngle -= xRotationAngle >= 2 * M_PI ? 2 * M_PI : 0;
        yRotationAngle -= yRotationAngle >= 2 * M_PI ? 2 * M_PI : 0;

        printf("\x1b[H");
        // draw screen
        for (size_t i = 0; i < HEIGHT; i++) {
            for (size_t j = 0; j < WIDTH; j++) {
                light = zBuffer[i][j];
                putchar(light > 0 ? SHADES[(int)(NUMBER_SHADES * light)] : ' ');
            }
            putchar('\n');
        }

        // for (size_t i = 0; i < HEIGHT; i++) {
        //     for (size_t j = 0; j < WIDTH; j++) {
        //         printf("%.2f ", zBuffer[i][j]);
        //     }
        //     putchar('\n');
        // }


    }

    return EXIT_SUCCESS;
}