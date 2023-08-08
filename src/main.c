#include "raylib.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int generate_random_number(int lower, int upper)
{
    return rand() % (upper - lower + 1) + lower;
}

double gen_rand_angle(int flag)
{
    if (flag == 1) // p2 scores (left side)
    {
        return generate_random_number(20, 160) - 90.0;
    }
    else if (flag == 2) // p1 scores (right side)
    {
        return generate_random_number(200, 340) - 90.0;
    }
    else if (flag == 3) // p1 paddle hit
    {
        return generate_random_number(40, 140) - 90.0;
    }
    else if (flag == 4) // p2 paddle hit
    {
        return generate_random_number(220, 320) - 90.0;
    }
    else
    {
        if (generate_random_number(0, 1))
        {
            return generate_random_number(20, 160) - 90.0;
        }
        else
        {
            return generate_random_number(200, 340) - 90.0;
        }
    }
}

bool in_range(float num, float r1, float r2)
{
    if (num >= r1 && num <= r2)
    {
        return true;
    }
    return false;
}

double degrees_to_radians(double degrees)
{
    return degrees * (PI / 180.0);
}

Vector2 compute_velocity(float speed, double angle)
{
    double radians = degrees_to_radians(angle);

    Vector2 result = (Vector2){cos(radians) * speed, sin(radians) * speed};

    return result;
}

bool hits_paddle(int ball_size, Vector2 ball_pos, Vector2 paddle_size, Vector2 paddle_pos)
{
    // radius
    float radius = ball_size / 2;

    // ball bounds
    float b_top = ball_pos.y - radius;
    float b_bot = ball_pos.y + radius;
    float b_left = ball_pos.x - radius;
    float b_right = ball_pos.x + radius;

    // rectangle bounds
    float r_top = paddle_pos.y;
    float r_bot = paddle_pos.y + paddle_size.y;
    float r_left = paddle_pos.x;
    float r_right = paddle_pos.x + paddle_size.x;

    // check if a collision is possible
    bool in_x = ((b_left <= r_right) && (b_right >= r_left));
    bool in_y = ((b_bot >= r_top) && (b_top <= r_bot));

    // simple collision
    return in_x && in_y;
}

int main(void)
{
    // initialization
    const int screen_width = 800;
    const int screen_height = 600;

    // initialize window
    InitWindow(screen_width, screen_height, "Pong");

    // TODO: set up icon
    // Image icon = LoadImage('../assets/ball.png');
    // SetWindowIcon(icon);

    // load music
    InitAudioDevice();
    Music background_music;
    // load background music
    if (IsAudioDeviceReady())
    {
        // load background music into initialized var
        background_music = LoadMusicStream("../assets/background_music.mp3");

        // set loop flag to true
        background_music.looping = true;

        // if the music loads successfully, play the music
        if (IsMusicReady(background_music))
        {
            PlayMusicStream(background_music);
            // fprintf(stderr, "music is playing");
        }
    }

    // TODO: set up mute icon

    // ball info
    const float default_ball_speed = 5.0f;
    int ball_size = 64;
    float ball_speed = default_ball_speed;
    double ball_direction = gen_rand_angle(0);
    Vector2 ball_pos = {(float)screen_width / 2, (float)screen_height / 2};
    Vector2 ball_vel = compute_velocity(ball_speed, ball_direction);

    // paddle positions
    float paddle_speed = 7.5f;
    Vector2 paddle_size = {20, 100};
    Vector2 paddle1_pos = {(float)(100.0 - (paddle_size.x / 2)), (float)((screen_height / 2) - (paddle_size.y / 2))};
    Vector2 paddle2_pos = {(float)(700.0 + (paddle_size.x / 2)), (float)((screen_height / 2) - (paddle_size.y / 2))};

    // score trackers for players 1 and 2
    int player_1_score = 0;
    int player_2_score = 0;

    // textures
    Texture2D bg = LoadTexture("../assets/background.png");
    Texture2D ball_texture = LoadTexture("../assets/ball.png");
    Texture2D paddle_texture = LoadTexture("../assets/paddle.png");

    // text
    int font_size = 20;

    char instr1[34] = "Move left paddle with \'W\' and \'S\'";
    int instr1_width = MeasureText(instr1, font_size);

    char instr2[39] = "Move right paddle with \'UP\' and \'DOWN\'";
    int instr2_width = MeasureText(instr2, font_size);

    char instr3[33] = "Press \'F\' to toggle Fullscreen";
    int instr3_width = MeasureText(instr3, font_size);

    char instr4[24] = "Press \'M\' to mute music";
    int instr4_width = MeasureText(instr4, font_size);

    char instr5[25] = "Press \'ESC\' to exit game";
    int instr5_width = MeasureText(instr5, font_size);

    // frames per second
    SetTargetFPS(60);

    // disable cursor
    DisableCursor();

    // game loop
    while (!WindowShouldClose())
    {
        // UPDATE
        // ---------------------------------------------------------

        // toggle mute
        if (IsKeyReleased(KEY_M))
        {
            if (IsMusicStreamPlaying(background_music))
            {
                PauseMusicStream(background_music);
            }
            else
            {
                ResumeMusicStream(background_music);
            }
        }

        // update music
        UpdateMusicStream(background_music);

        // update ball position
        ball_pos.x += ball_vel.x;
        ball_pos.y += ball_vel.y;

        // top and bottom edges
        if ((ball_pos.y - (ball_size / 2) <= 0) || (ball_pos.y + (ball_size / 2) >= screen_height))
        {
            ball_direction *= -1;
            ball_vel = compute_velocity(ball_speed, ball_direction);
        }

        // left and right edges
        if ((ball_pos.x - (ball_size / 2) <= 0))
        {
            // update scores
            player_2_score += 1;

            // reset ball to center
            ball_pos.x = (float)screen_width / 2;
            ball_pos.y = (float)screen_height / 2;
            ball_speed = default_ball_speed;
            ball_direction = gen_rand_angle(2);
            ball_vel = compute_velocity(ball_speed, ball_direction);
        }
        if (ball_pos.x + (ball_size / 2) >= screen_width)
        {
            // update scores
            player_1_score += 1;

            // reset ball to center
            ball_pos.x = (float)screen_width / 2;
            ball_pos.y = (float)screen_height / 2;
            ball_speed = default_ball_speed;
            ball_direction = gen_rand_angle(1);
            ball_vel = compute_velocity(ball_speed, ball_direction);
        }

        // collision with paddle1
        if (hits_paddle(ball_size, ball_pos, paddle_size, paddle1_pos))
        {
            ball_direction = gen_rand_angle(3);
            ball_vel = compute_velocity(ball_speed, ball_direction);
            ball_speed += 0.5;
        }

        // collision with paddle2
        if (hits_paddle(ball_size, ball_pos, paddle_size, paddle2_pos))
        {
            ball_direction = gen_rand_angle(4);
            ball_vel = compute_velocity(ball_speed, ball_direction);
            ball_speed += 0.5;
        }

        // left paddle movement
        if (IsKeyDown(KEY_W))
        {
            if (paddle1_pos.y > 0)
            {
                paddle1_pos.y -= paddle_speed;
            }
        }
        if (IsKeyDown(KEY_S))
        {
            if (paddle1_pos.y < screen_height - paddle_size.y)
            {
                paddle1_pos.y += paddle_speed;
            }
        }

        // right paddle movement
        if (IsKeyDown(KEY_UP))
        {
            if (paddle2_pos.y > 0)
            {
                paddle2_pos.y -= paddle_speed;
            }
        }
        if (IsKeyDown(KEY_DOWN))
        {
            if (paddle2_pos.y < screen_height - paddle_size.y)
            {
                paddle2_pos.y += paddle_speed;
            }
        }

        if (player_1_score >= 10 || player_2_score >= 10)
        {
            break;
        }

        // toggle fullscreen
        if (IsKeyDown(KEY_F))
        {
            ToggleFullscreen();
        }
        // ---------------------------------------------------------

        // DRAW
        // ---------------------------------------------------------
        BeginDrawing();

        // refresh background
        ClearBackground(BLACK);

        // draw background texture
        DrawTexture(bg, 0, 0, WHITE);

        // FPS tracker
        SetWindowTitle(TextFormat("Pong (FPS: %d)", GetFPS()));

        // draw instructions
        DrawText(instr1, (screen_width / 2) - (instr1_width / 2), 20, font_size, WHITE);
        DrawText(instr2, (screen_width / 2) - (instr2_width / 2), 45, font_size, WHITE);
        DrawText(instr3, (screen_width / 2) - (instr3_width / 2), screen_height - font_size, font_size, WHITE);      // FULLSCREEN
        DrawText(instr4, (screen_width / 2) - (instr4_width / 2), screen_height - font_size - 50, font_size, WHITE); // MUSIC
        DrawText(instr5, (screen_width / 2) - (instr5_width / 2), screen_height - font_size - 25, font_size, WHITE); // ESCAPE

        // draw scores
        DrawText(TextFormat("P1: %03i", player_1_score), 5, screen_height - font_size - 5, font_size, WHITE);
        DrawText(TextFormat("P2: %03i", player_2_score), screen_width - MeasureText(TextFormat("P2: %03i", player_2_score), font_size) - 5, screen_height - font_size - 5, font_size, WHITE);

        // TODO: draw mute icon
        // DrawTextureEx()

        // draw ball
        DrawTexture(ball_texture, ball_pos.x - ball_size / 2, ball_pos.y - ball_size / 2, WHITE);

        // draw paddles
        DrawTextureEx(paddle_texture, paddle1_pos, 0.0, 1.0, WHITE);
        DrawTextureEx(paddle_texture, paddle2_pos, 0.0, 1.0, WHITE);

        // debug (centers)
        // DrawRectangle((screen_width / 2) - 1, 0, 2, screen_height, WHITE);
        // DrawRectangle(0, (screen_height / 2) - 1, screen_width, 2, WHITE);

        EndDrawing();
        // ---------------------------------------------------------
    }

    // unload vars
    UnloadMusicStream(background_music);
    UnloadTexture(bg);
    UnloadTexture(ball_texture);
    UnloadTexture(paddle_texture);
    // UnloadImage(icon);

    // deinitialize
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
