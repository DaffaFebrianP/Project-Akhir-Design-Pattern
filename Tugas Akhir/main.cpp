#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <raylib.h>

using namespace std;

// =============== Word Class ===============
class Word {
private:
    string text;
    Vector2 position;
    float speed;
    Texture2D* meteorTexture;

public:
    Word(string t, float x, float y, float spd, Texture2D* texture)
        : text(t), position({x, y}), speed(spd), meteorTexture(texture) {}

    void update(float deltaTime) {
        position.y += speed * deltaTime;
    }

    void draw() const {
        if (meteorTexture) {
            DrawTexture(*meteorTexture, (int)position.x, (int)position.y, WHITE);
        }
        DrawText(text.c_str(), (int)position.x + 10, (int)position.y + 10, 20, WHITE);  // Ganti jadi putih
    }

    string getText() const { return text; }
    float getY() const { return position.y; }
};

// =============== Factory Pattern ===============
class WordFactory {
private:
    vector<string> wordPool = {
        "shark", "code", "loop", "fish", "game", "cpp", "raylib", "class", "object",
        "score", "input", "event", "ocean", "swim", "coral", "wave", "deep",
        "seaweed", "whale", "tide", "diver", "current", "reef", "anchor", "keyboard",
        "danger", "type", "speed", "fast", "slow", "blue", "black", "clear", "storm",
        "drop", "float", "turtle", "octopus", "jellyfish", "seal", "dolphin", "crab",
        "starfish", "eel", "penguin", "clam", "lobster", "electric", "velocity",
        "function", "variable", "structure", "algorithm", "inherit", "virtual", "template"
    };

    Texture2D* meteorTexture;

public:
    WordFactory(Texture2D* texture) : meteorTexture(texture) {}

    Word createRandomWord(float speed) {
        int index = rand() % wordPool.size();
        float x = 50 + rand() % 700;
        return Word(wordPool[index], x, 0, speed, meteorTexture);
    }
};

// =============== Observer Pattern ===============
class ScoreObserver {
public:
    void notify(int score) {
        cout << "Score Updated: " << score << endl;
    }
};

// =============== Main Function ===============
int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Typing Shark - Raylib Edition");
    SetTargetFPS(60);

    // Load assets
    Texture2D background = LoadTexture("Asset/background.png");
    Texture2D meteor = LoadTexture("Asset/meteor.png");

    if (background.id == 0) cout << "Failed to load background.png\n";
    if (meteor.id == 0) cout << "Failed to load meteor.png\n";

    // Game State
    WordFactory factory(&meteor);
    ScoreObserver scoreObserver;
    vector<Word> words;

    string currentInput = "";
    int score = 0;
    int lives = 3;
    int level = 1;
    bool gameOver = false;

    float spawnTimer = 0.0f;
    float spawnInterval = 2.0f;
    float wordSpeed = 60.0f;

    srand((unsigned)time(0));

    // Game Loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // === INPUT ===
        if (!gameOver) {
            int key = GetCharPressed();
            while (key > 0) {
                if (isalpha(key)) currentInput += (char)key;
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !currentInput.empty()) {
                currentInput.pop_back();
            }

            if (IsKeyPressed(KEY_ENTER)) {
                for (size_t i = 0; i < words.size(); i++) {
                    if (words[i].getText() == currentInput) {
                        score += 10;
                        scoreObserver.notify(score);
                        words.erase(words.begin() + i);
                        break;
                    }
                }
                currentInput = "";
            }
        }

        // === UPDATE ===
        if (!gameOver) {
            if (score >= level * 50) {
                level++;
                wordSpeed += 20.0f;
                spawnInterval *= 0.9f;
            }

            spawnTimer += dt;
            if (spawnTimer >= spawnInterval) {
                words.push_back(factory.createRandomWord(wordSpeed));
                spawnTimer = 0;
            }

            for (auto& word : words) word.update(dt);

            for (size_t i = 0; i < words.size();) {
                if (words[i].getY() > screenHeight - 20) {
                    lives--;
                    words.erase(words.begin() + i);
                    if (lives <= 0) gameOver = true;
                } else {
                    ++i;
                }
            }
        }

        // === RENDER ===
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ==== Draw Background (tidak stretch) ====
        float windowRatio = (float)screenWidth / (float)screenHeight;
        float textureRatio = (float)background.width / (float)background.height;

        Rectangle sourceRec;
        Rectangle destRec = {0, 0, (float)screenWidth, (float)screenHeight};
        Vector2 origin = {0, 0};

        if (windowRatio > textureRatio) {
            // crop vertikal
            float newHeight = (float)background.width / windowRatio;
            float yOffset = ((float)background.height - newHeight) / 2;
            sourceRec = {0, yOffset, (float)background.width, newHeight};
        } else {
            // crop horizontal
            float newWidth = (float)background.height * windowRatio;
            float xOffset = ((float)background.width - newWidth) / 2;
            sourceRec = {xOffset, 0, newWidth, (float)background.height};
        }

        DrawTexturePro(background, sourceRec, destRec, origin, 0.0f, WHITE);

        if (!gameOver) {
            for (const auto& word : words) word.draw();

            DrawText(("Type: " + currentInput).c_str(), 10, 550, 20, WHITE);
            DrawText(("Score: " + to_string(score)).c_str(), 650, 10, 20, GREEN);
            DrawText(("Lives: " + to_string(lives)).c_str(), 650, 40, 20, RED);
            DrawText(("Level: " + to_string(level)).c_str(), 650, 70, 20, BLUE);
        } else {
            DrawText("GAME OVER!", 280, 250, 40, RED);
            DrawText(("Final Score: " + to_string(score)).c_str(), 300, 300, 25, WHITE);
            DrawText("Press ESC to Exit", 310, 350, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    UnloadTexture(background);
    UnloadTexture(meteor);
    CloseWindow();
    return 0;
}
