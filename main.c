#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "input.c"

#define snake '@'
#define apple 'X'
#define empty ' '

#define DIRECTION_LEFT 1
#define DIRECTION_RIGHT 2
#define DIRECTION_UP 3
#define DIRECTION_DOWN 4

const int SCREEN_WIDTH = 16;
const int SCREEN_HEIGHT = 16;

struct Position
{
    int x;
    int y;
};

int apple_coords[2];
int *snake_coords;
int SNAKE_LENGTH = 1;
int DIRECTION = DIRECTION_RIGHT;
int SCORE = 0;
struct Position FOOD;

int setup()
{
    // Create snake
    snake_coords = malloc(sizeof(int) * 1 * 2);
    *(snake_coords + 2 * 0 + 0) = floor((SCREEN_WIDTH - 1) / 2);
    *(snake_coords + 2 * 0 + 1) = floor((SCREEN_WIDTH - 1) / 2);

    DIRECTION = DIRECTION_RIGHT;

    populate_food();

    return 0;
}

struct Position position_change()
{
    struct Position pos;
    pos.x = 0;
    pos.y = 0;

    switch (DIRECTION)
    {
    case DIRECTION_LEFT:
        pos.x = -1;
        break;
    case DIRECTION_RIGHT:
        pos.x = 1;
        break;
    case DIRECTION_UP:
        pos.y = 1;
        break;
    case DIRECTION_DOWN:
        pos.y = -1;
        break;
    default:
        break;
    }

    return pos;
}

int move_snake()
{
    int last_x = -1;
    int last_y = -1;

    struct Position position = position_change();

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {

        if (i == 0)
        {
            last_x = *(snake_coords + i * 2 + 0);
            last_y = *(snake_coords + i * 2 + 1);

            *(snake_coords + i * 2 + 0) += position.x;
            *(snake_coords + i * 2 + 1) += position.y;
        }
        else
        {
            int temp_last_x = last_x;
            int temp_last_y = last_y;
            last_x = *(snake_coords + i * 2 + 0);
            last_y = *(snake_coords + i * 2 + 1);

            *(snake_coords + i * 2 + 0) = temp_last_x;
            *(snake_coords + i * 2 + 1) = temp_last_y;
        }
    }

    return 0;
}

int grow_snake()
{
    snake_coords = realloc(snake_coords, (SNAKE_LENGTH + 1) * 2 * sizeof(int));

    int x = *(snake_coords + 0), y = *(snake_coords + 1);
    struct Position pos_change = position_change();
    *(snake_coords + SNAKE_LENGTH * 2 + 0) = x + pos_change.x;
    *(snake_coords + SNAKE_LENGTH * 2 + 1) = y + pos_change.y;
    SNAKE_LENGTH += 1;

    return 0;
}

int clear_screen(int screen[SCREEN_WIDTH][SCREEN_HEIGHT])
{
    for (int i = 0; i < SCREEN_WIDTH; i++)
    {
        for (int j = 0; j < SCREEN_HEIGHT; j++)
        {
            screen[i][j] = empty;
        }
    }

    return 0;
}

int populate_food()
{
    time_t t;
    srand(time(&t));

    int x = 3 + rand() % (SCREEN_WIDTH - 3);
    int y = 3 + rand() % (SCREEN_HEIGHT - 3);

    FOOD.x = x;
    FOOD.y = y;

    return 0;
}

int draw()
{
    // clear
    system("clear");
    int display[SCREEN_WIDTH][SCREEN_HEIGHT];
    clear_screen(display);

    // show snake
    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        display[*(snake_coords + i * 2 + 0)][*(snake_coords + i * 2 + 1)] = snake;
    }

    // show food
    display[FOOD.x][FOOD.y] = apple;

    for (int y = SCREEN_HEIGHT - 1; y >= 0; y--)
    {
        for (int x = SCREEN_WIDTH - 1; x >= 0; x--)
        {
            printf("%c ", display[x][y]);
        }
        printf("\n");
    }

    printf("\n SCORE: %d \n \n", SCORE);
}

int is_out_of_bounds()
{
    if (*(snake_coords + 0) >= 0 && *(snake_coords + 1) >= 0 && *(snake_coords + 0) < SCREEN_WIDTH && *(snake_coords + 1) < SCREEN_HEIGHT)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int snake_ate_itself()
{
    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        for (int j = 0; j < SNAKE_LENGTH; j++)
        {
            if (i == j)
                continue;

            int x1 = *(snake_coords + i * 2 + 0), y1 = *(snake_coords + i * 2 + 1);
            int x2 = *(snake_coords + j * 2 + 0), y2 = *(snake_coords + j * 2 + 1);

            if (x1 == x2 && y1 == y2)
            {
                return 1;
            }
        }
    }

    return 0;
}

int snake_ate_food()
{
    int head_x = *(snake_coords + 0);
    int head_y = *(snake_coords + 1);

    if (head_x == FOOD.x && head_y == FOOD.y)
    {
        return 1;
    }

    return 0;
}

void exit_game()
{
    input_off();
    printf("\n\n############ GAME OVER ############\n\n");
}

int main()
{
    int display[SCREEN_WIDTH][SCREEN_HEIGHT];
    fd_set rfds;
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    input_on();

    setup();

    while (1)
    {

        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);

        if (select(1, &rfds, NULL, NULL, &timeout) > 0 && FD_ISSET(STDIN_FILENO, &rfds))
        {
            switch (getchar())
            {
            case 'a':
                if (DIRECTION != DIRECTION_LEFT)
                    DIRECTION = DIRECTION_RIGHT;
                break;

            case 's':
                if (DIRECTION != DIRECTION_UP)
                    DIRECTION = DIRECTION_DOWN;
                break;

            case 'd':
                if (DIRECTION != DIRECTION_RIGHT)
                    DIRECTION = DIRECTION_LEFT;
                break;

            case 'w':
                if (DIRECTION != DIRECTION_DOWN)
                    DIRECTION = DIRECTION_UP;
                break;
            default:
                break;
            }
        }

        move_snake();

        if (snake_ate_food())
        {
            populate_food();
            SCORE += 3;
            grow_snake();
        }

        if (is_out_of_bounds() || snake_ate_itself())
        {
            exit_game();
            break;
        }

        draw();
        usleep(200000);
    }

    input_off();
    return 0;
}
