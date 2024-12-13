#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // Update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // Update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > 5)
        {
            data.frame = 0;
        }
        return data;
    }

    return data;
}

int main()
{
    // Window dimensions
    const int windowWidth{640};
    const int windowHeight{480};
    int windowDimensions[] = {windowWidth, windowHeight};

    // Initialize the window
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher!");

    // Acceleration due to gravity (pixels/s)/s
    const int gravity{1'000};

    // Nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    // AnimData for nebula
    const int sizeOfNebulae{5};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height / 8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1 / 16;
    }

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].pos.x = windowDimensions[0] + (300 * (i));
    }

    float finishLine{nebulae[sizeOfNebulae - 1].pos.x};

    // Nebula X velocity (pixels/second)
    int nebVel{-200};

    // Scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width / 6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0] / 2 - scarfyData.rec.width / 2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0 / 12.0;
    scarfyData.runningTime = 0.0;

    // Is the rectangle in the air?
    bool isInAir{};
    // Jump velocity (pixels/second)
    const int jumpVel{-600};
    // Game over condition
    bool collision{false};

    int velocity{0};
    // Load background
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};

    // Load foreGround
    Texture2D foreGround = LoadTexture("textures/foreground.png");
    float fgX{};

    // Load midGround
    Texture2D midGround = LoadTexture("textures/back-buildings.png");
    float mgX{};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Delta time (time since last frame)
        const float dT{GetFrameTime()};

        // Start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // Scroll the background
        bgX -= 20 * dT;
        if (bgX <= -background.width * 2.5)
        {
            bgX = 0.0;
        }

        // Scroll the midGround
        mgX -= 40 * dT;
        if (mgX <= -background.width * 2.5)
        {
            mgX = 0.0;
        }

        // Scroll the foreGround
        fgX -= 80 * dT;
        if (fgX <= -foreGround.width * 2.5)
        {
            fgX = 0.0;
        }

        // Update finish line
        finishLine += nebVel * dT;

        // Draw the background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.5, WHITE);
        Vector2 bg2Pos{bgX + background.width * 2.5, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.5, WHITE);

        // Draw midGround
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midGround, mg1Pos, 0.0, 2.5, WHITE);
        Vector2 mg2Pos{mgX + midGround.width * 2.5, 0.0};
        DrawTextureEx(midGround, mg2Pos, 0.0, 2.5, WHITE);

        // Draw foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreGround, fg1Pos, 0.0, 2.5, WHITE);
        Vector2 fg2Pos{fgX + foreGround.width * 2.5, 0.0};
        DrawTextureEx(foreGround, fg2Pos, 0.0, 2.5, WHITE);

        // Perform ground check
        if (isOnGround(scarfyData, windowDimensions[1]))
        {
            // Rectangle is on the ground
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // Rectangle is in the air
            velocity += gravity * dT;
            isInAir = true;
        }

        // Jump check
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel;
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            // Update pos of each nebula
            nebulae[i].pos.x += nebVel * dT;
        }

        // Update scarfy position
        scarfyData.pos.y += velocity * dT;

        // Update scarfy's animation frame
        if (!isInAir)
        {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        // Update nebula animation frame
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{60};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad};
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height};
            if (CheckCollisionRecs(scarfyRec, nebRec))
            {
                collision = true;
            }
        }

        if (collision)
        {
            // Loose game
        }
        else
        {
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

            // Draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        // Stop drawing
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(foreGround);
    UnloadTexture(midGround);

    CloseWindow();
}