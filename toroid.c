#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const size_t WIDTH = 80;
const size_t HEIGHT = 80;
const float STEP = 0.01;
const float R = 2;
const float r = 1;
const float X_AXIS_ROTATION_SPEED = 0.06;
const float Y_AXIS_ROTATION_SPEED = 0.03;
const float SCREEN_DIST = 50;
const char* SHADES = ".,-~:;=!*#$@";
const float DISTANCE = 5;

int main() {
    const int NUMBER_SHADES = strlen(SHADES);
    const float LIGHT_VECTOR[] = { 0, 1 / sqrt(2), -1 / sqrt(2) };


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
                nx = x;
                ny = y * cos(xRotationAngle) - z * sin(xRotationAngle);
                nz = y * sin(xRotationAngle) + z * cos(xRotationAngle);

                // ROTATION Y
                x = nx * cos(yRotationAngle) - nz * sin(yRotationAngle);
                y = ny;
                z = nx * sin(yRotationAngle) + nz * cos(yRotationAngle);



                // APPLY LIGHT
                // norms
                normX = cos(alpha) * cos(betta);
                normY = 0;
                normZ = sin(alpha);

                // ROTATION X
                nx = normX;
                ny = normY * cos(xRotationAngle) - normZ * sin(xRotationAngle);
                nz = normY * sin(xRotationAngle) + normZ * cos(xRotationAngle);

                // ROTATION Y
                normX = nz * cos(yRotationAngle) + nz * sin(yRotationAngle);
                normY = ny;
                normZ = -nx * sin(yRotationAngle) + nz * cos(yRotationAngle);


                light = normX * LIGHT_VECTOR[0] + normY * LIGHT_VECTOR[1] + normZ * LIGHT_VECTOR[2];
                // printf("%d\n", light);
                // ADD TO Z BUFFER
                projected_x = x * SCREEN_DIST / (z + DISTANCE) + WIDTH / 2;
                projected_y = y * SCREEN_DIST / (z + DISTANCE) + HEIGHT / 2;
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