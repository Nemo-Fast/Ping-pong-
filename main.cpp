#include "raylib.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// All the colours used in the code
Color Green = Color{50, 255, 50, 255};
Color Orange = Color{255, 200, 120, 255};
Color Blue = Color{173, 216, 230, 255};
Color Red = Color{255, 0, 0, 255}; 
Color Yellow = Color{255, 255, 0, 255}; 
Color White = Color{255, 255, 255, 255}; 

int score = 0;
int oldfps = 60; //staring fps
string playerName;

//Highscore as vector
vector<pair<int, string>> highscores;

// Global Ball variables
float BallXD, BallYD;
int BallSpeedXD, BallSpeedYD;
int BallRadius;

// Global Paddle variables
float PaddleXD, PaddleYD;
float PWidth, PHeight;
int PaddleSpeed;


// Function prototypes at the start
void BallCenter();
void GameRestart();
void MakeBall(int x, int y, int radius);
void MoveBall(bool &BackToMenu);
void MakePaddle(int x, int y, int width, int height);
void MovePaddle();
bool DisplayMenu(Texture2D background);
void IncreaseGameSpeed();
bool GameOverScreen();
void SaveHighscore(int score, const string &name);
void LoadHighscores();
void SortHighscore();
void ShowHighscores();
void EnterName();

void BallCenter() {
    BallXD = 1440 / 2; //reset ball at center
    BallYD = 900 / 2;

    
        BallSpeedXD *= -1;
        BallSpeedYD *= -1;
    
}

void GameRestart() {
    score = 0;
    oldfps = 60; // OG FPS was 60
    SetTargetFPS(oldfps); 
    BallCenter();
}

void MakeBall(int x, int y, int radius) {//as seen from name
    DrawCircle(x,y,radius, Orange);
}

void MoveBall(bool &BackToMenu) {
    BallXD += BallSpeedXD;
    BallYD += BallSpeedYD;

    // Top and bottom reverse
    if (BallYD + BallRadius >= 900 || BallYD - BallRadius <= 0) {
        BallSpeedYD *= -1;
    }

    // Right Wall reverse
    if (BallXD + BallRadius >= 1440) {
        BallSpeedXD *= -1;
    }

    // Game ends when touches left boundary
    if (BallXD - BallRadius <= 0) {
        bool tryAgain = GameOverScreen();

        if (tryAgain) {
            GameRestart(); 
        } else {
            BackToMenu = true; 
        }
    }
}

void MakePaddle(int x, int y, int width, int height) {//as seen from name

    DrawRectangle(x,y,width,height,Blue);
}

void MovePaddle() { //move paddle
    if (IsKeyDown(KEY_UP)) {
        PaddleYD -= PaddleSpeed;
    }
    if (IsKeyDown(KEY_DOWN)) {
        PaddleYD += PaddleSpeed;
    }
    if (PaddleYD <= 0) { //paddle should not go out of window
        PaddleYD = 0;
    }
    if (PaddleYD + PHeight >= 900) {
        PaddleYD = 900 - PHeight;
    }
}

bool DisplayMenu(Texture2D background) {
    int selectedOption = 0; 
    const int numOptions = 3; //3 Numbers for the 3 options
    const int optionHeight = 60;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(White);

        DrawTexture(background, 0, 0, WHITE);
        DrawText("Ping Pong", 1440 / 2 - 150, 300, 50, BLACK);

        for (int i = 0; i < numOptions; i++) {
            int yPosition = 400 + (i * optionHeight);
            Color optionColor = (i == selectedOption) ? White : BLACK; //create contrast between selected options
            const char* text;
            if (i == 0) text = "Play";
            else if (i == 1) text = "Highscores";
            else text = "Exit";

            int xPosition = 1440 / 2 - 50;
            DrawText(text, xPosition, yPosition, 30, optionColor);

        }

        EndDrawing();

        if (IsKeyPressed(KEY_DOWN)) {
            selectedOption = (selectedOption + 1) % numOptions;//mod for infite loop in the options
        }
        if (IsKeyPressed(KEY_UP)) {
            selectedOption = (selectedOption - 1 + numOptions) % numOptions;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (selectedOption == 1) {
                // Highscores selected
                ShowHighscores();
                continue; // Stay in the menu loop during seeing highscores
            } else if (selectedOption == 0) {
                
                return true; // Game start
            } else if (selectedOption == 2) {
                //game exits here
                CloseWindow();
                return false; 
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
            return false; 
        }
    }

    return false;
}

void IncreaseGameSpeed() {
    if (score % 5 == 0 && score > 0 && oldfps < 150) {
        oldfps += 20;
        SetTargetFPS(oldfps);  //make the game fast so its not repetitive
    }
}

bool GameOverScreen() {
    int selectedOption = 0;
    const int numOptions = 3; // each option having a diffrent number
    const int optionHeight = 60;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK); 

        DrawText("Game Over!", 1440 / 2 - 150, 300, 40, RED); 

        // Display menu options
        for (int i = 0; i < numOptions; i++) {
            int yPosition = 400 + (i * optionHeight);
            Color optionColor = (i == selectedOption) ? ORANGE : RED; // Create contrast
            const char* optionText;
           if (i == 0) {
           optionText = "Try Again";
           } else if (i == 1) {
            optionText = "Main Menu";
           } else {
           optionText = "Enter Name";
        }

            DrawText(optionText, 1440 / 2 - 100, yPosition, 30, optionColor);
        }

        EndDrawing();

        // Go through the options
        if (IsKeyPressed(KEY_DOWN)) {
            selectedOption = (selectedOption + 1) % numOptions;//Mod to create infinite loop
        }
        if (IsKeyPressed(KEY_UP)) {
            selectedOption = (selectedOption - 1 + numOptions) % numOptions;
        }

        
        if (IsKeyPressed(KEY_ENTER)) {
            if (selectedOption == 0) {
                // Try Again
                return true;
            } else if (selectedOption == 1) {
                // Main Menu
                return false;
            } else if (selectedOption == 2) {
                // Enter Name
                EnterName();
            }
        }
    }

    CloseWindow();
    exit(0);
    return false;
}

void SaveHighscore(int score, const string &name) {
    ofstream file("Highscores.txt", ios::app);//Open the File in append mode to add new elements
    if (file.is_open()) {
        file << score << " " << name << "\n";
        file.close();
    }
}

void LoadHighscores() {
    highscores.clear();
    ifstream file("Highscores.txt"); //only open for read
    if (file.is_open()) {
        int score;
        string name;
        while (file >> score) { //read scores until end of file is reached
            getline(file, name);
            highscores.emplace_back(score, name); //add in the vector
        }
        file.close();
    }
}

void SortHighscore() {
    
    int size = highscores.size();

    
    for (int i = 0; i < size; i++) {
        
        for (int j = 0; j < size - i - 1; j++) {
            
            if (highscores[j].first < highscores[j + 1].first) {
                
                pair<int, string> temp = highscores[j];  // Store the current element in temp
                highscores[j] = highscores[j + 1];       
                highscores[j + 1] = temp;                
            }
        }
    }
}



void ShowHighscores() {
    LoadHighscores();
    SortHighscore();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Blue); 

        DrawText("Highscores", 1440 / 2 - 100, 100, 40, Yellow); 

       int numScoresToShow = (highscores.size() < 5) ? highscores.size() : 5; //max score should be 5 or less

        for (int i = 0; i < numScoresToShow; i++) {
        string text = to_string(i + 1) + ". " + highscores[i].second + " - " + to_string(highscores[i].first);
        DrawText(text.c_str(), 1440 / 2 - 200, 200 + i * 50, 30, Yellow); 
    }


        DrawText("Press [Enter] to return to Menu", 1440 / 2 - 200, 600, 20, Yellow); // condition for menu

        EndDrawing();

        if (IsKeyPressed(KEY_ENTER)) {
            break; // Return to the main menu
        }
    }
}

void EnterName() {
    playerName.clear();//to make a new entry
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        string displayText = "Enter your name please: " + playerName + "_";
        DrawText(displayText.c_str(), 1440 / 2 - 200, 400, 30, White);

        DrawText("Press Enter to confirm or ESC to cancel", 1440 / 2 - 200, 500, 20, White);

        EndDrawing();

        int key = GetCharPressed();
        if (key >= 32 && key <= 125 && playerName.length() < 12) { // max legth should be 12
            playerName += static_cast<char>(key);
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !playerName.empty()) {
            playerName.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER)) {
            SaveHighscore(score, playerName);
            break;
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            break; 
        }
    }
}
int main() {
    const int screenWidth = 1440;
    const int screenHeight = 900;

    // Game Window
    InitWindow(screenWidth, screenHeight, "Ping Pong Game");
    SetTargetFPS(oldfps);

    // Image Background
    Texture2D background = LoadTexture("menucopy.png");

    // The ball's variables
    BallRadius = 20;
    BallXD = screenWidth / 2;
    BallYD = screenHeight / 2;
    BallSpeedXD = 8;
    BallSpeedYD = 8;

    // The paddle's variables
    PWidth = 20;
    PHeight = 180;
    PaddleXD = 20;
    PaddleYD = screenHeight / 2 - PHeight / 2;
    PaddleSpeed = 12;

    
    while (!WindowShouldClose()) {
        // Display menu
        bool play = DisplayMenu(background);

        if (!play) {
            break; // Exit if the user presses exit
        }

    
        GameRestart();
        bool BackToMenu = false;

        while (!WindowShouldClose() && !BackToMenu) {
            
            MovePaddle();
            MoveBall(BackToMenu);

            // Check collision to increase score
            if (CheckCollisionCircleRec(Vector2{BallXD, BallYD},
                                        BallRadius,
                                        Rectangle{PaddleXD, PaddleYD, PWidth, PHeight})) {
                BallXD = PaddleXD + PWidth + BallRadius; // so ball rebounds appropriately
                BallSpeedXD *= -1;
                score++;
                IncreaseGameSpeed(); // make the game fun by increasing speed after 5 points
            }

            
            BeginDrawing();
            ClearBackground(Green);

            MakeBall(BallXD, BallYD, BallRadius);
            MakePaddle(PaddleXD, PaddleYD, PWidth, PHeight);
            // rectanlge should look neat so diffrence is 30 pixels
            int LX = 30;
            int RX = screenWidth - 30;
            int TY = 30;
            int BY = screenHeight - 30;

            DrawLine(LX, TY, LX, BY, WHITE);

            DrawLine(RX, TY, RX, BY, WHITE);

            DrawLine(LX, TY, RX, TY, WHITE);

            DrawLine(LX, BY, RX, BY, WHITE);
            DrawLine(screenWidth / 2, TY, screenWidth / 2, BY, WHITE);

            DrawText(TextFormat("Score: %i", score), screenWidth - 250, 40, 30, WHITE);

            EndDrawing();
        }
    }

    
    
    CloseWindow();

    return 0;
}