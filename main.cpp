#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <algorithm>
#include <iomanip>

using namespace std;

// Original car designs
const vector<vector<string>> CAR_DESIGNS = {
    // Sports car
    {
        "  _____  ",
        " /|_||_\\`.__",
        "(   _    _ _\\",
        "=`-(_)--(_)-'"
    },
    // Truck
    {
        "    _____",
        " __/__|__\\____",
        "|  _        _`\\",
        "'-(_)------(_)-'"
    },
    // Formula 1
    {
        "    ___    ",
        "  _/___\\_  ",
        " [_\\_@_/_] ",
        "   (_)(_)  "
    },
    // Vintage car
    {
        "     ___     ",
        " ___/___\\____",
        " [_|_\\@/_|_] ",
        "    (_) (_)  "
    },
    // Motorcycle
    {
        "    __o",
        "  _ \\<_",
        " (_)/(_)"
    },
    // SUV
    {
        "  ______  ",
        " /|_||_\\\\",
        "(   _    _\\",
        "=`-(_)--(_)-"
    },
    // Convertible
    {
        "      _______",
        "     //  ||\\ \\",
        " ___//___||_\\ \\___",
        " )  _          _  \\",
        " |_/ \\________/ \\__|"
    },
    // Futuristic car
    {
        "     _-=-_     ",
        " _.-'_____`-._",
        "( .-'_____`-. )",
        " `-.________.-'",
        "     |   |     "
    }
};

class Car {
private:
    int position;
    int lane;
    int speed;
    int maxSpeed;
    int carType;
    string playerName;
    bool isPlayer;
    int score;
    bool hasNitro;

public:
    Car(int lane, int carType, string name = "", bool isPlayer = false) 
        : position(0), lane(lane), speed(isPlayer ? 0 : 1), maxSpeed(isPlayer ? 8 : 4), 
          carType(carType), playerName(name), isPlayer(isPlayer),
          score(0), hasNitro(false) {}

    void move() {
        if (!isPlayer) {
            // AI cars move at random speeds but much slower
            if (rand() % 20 == 0) { // Less frequent speed changes
                speed = rand() % maxSpeed + 1;
            }
            
            // AI cars occasionally change lanes (less frequently)
            if (rand() % 30 == 0) {
                int newLane = lane;
                if (rand() % 2 == 0) {
                    newLane++;
                } else {
                    newLane--;
                }
                // Keep within bounds
                if (newLane >= 0 && newLane < 8) {
                    lane = newLane;
                }
            }
        }

        // Apply nitro if active
        int actualSpeed = speed;
        if (hasNitro) {
            actualSpeed += 3;
            hasNitro = false; // Nitro lasts only one frame
        }

        position += actualSpeed;
        
        // Update score based on position
        score = position;
    }

    void accelerate() { 
        if (speed < maxSpeed) 
            speed++; 
    }
    
    void decelerate() { 
        if (speed > 0) // Allow complete stop for player
            speed--; 
    }
    
    void changeLane(int newLane) { 
        lane = newLane; 
    }

    void useNitro() { 
        hasNitro = true; 
    }

    void addScore(int points) {
        score += points;
    }

    int getPosition() const { return position; }
    int getLane() const { return lane; }
    int getSpeed() const { return speed; }
    int getCarType() const { return carType; }
    string getPlayerName() const { return playerName; }
    bool isPlayerCar() const { return isPlayer; }
    int getScore() const { return score; }
    bool isUsingNitro() const { return hasNitro; }
};

class Game {
private:
    vector<Car> cars;
    int trackLength;
    int numLanes;
    int screenWidth;
    int screenHeight;
    int playerCarIndex;
    bool gameOver;
    bool gameStarted;
    int frameCount;
    int difficulty;
    vector<pair<string, int>> highScores;
    bool practiceMode;
    int countdownTimer;

public:
    Game(int trackLen = 800, int lanes = 8) // Even longer track
        : trackLength(trackLen), numLanes(lanes), screenWidth(120), screenHeight(40),
          gameOver(false), gameStarted(false), frameCount(0), difficulty(1),
          practiceMode(true), countdownTimer(100) { // Start with practice mode
        
        srand(static_cast<unsigned int>(time(nullptr)));
        
        // Display welcome screen and instructions
        system("cls"); // Clear screen (Windows)
        cout << "=== EXTREME ASCII RACING ===\n\n";
        cout << "CONTROLS:\n";
        cout << "  W - Accelerate\n";
        cout << "  S - Decelerate/Brake\n";
        cout << "  A - Move left\n";
        cout << "  D - Move right\n";
        cout << "  SPACE - Use Nitro Boost\n";
        cout << "  Q - Quit game\n\n";
        
        cout << "OBJECTIVE:\n";
        cout << "  Reach the finish line before the AI cars\n";
        cout << "  Avoid collisions with other cars\n\n";
        
        cout << "SPECIAL FEATURES:\n";
        cout << "  - Practice mode at start (no AI cars for 10 seconds)\n";
        cout << "  - Press SPACE for nitro boost\n";
        cout << "  - You can completely stop your car with S\n\n";
        
        cout << "Select difficulty (1-Easy, 2-Medium, 3-Hard): ";
        cin >> difficulty;
        difficulty = max(1, min(3, difficulty)); // Ensure valid range
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // Create player car
        string playerName;
        system("cls");
        cout << "Enter your name: ";
        getline(cin, playerName);
        
        int carChoice;
        cout << "\nChoose your car (0-" << CAR_DESIGNS.size() - 1 << "):\n";
        for (size_t i = 0; i < CAR_DESIGNS.size(); i++) {
            cout << "Car " << i << ":\n";
            for (const auto& line : CAR_DESIGNS[i]) {
                cout << line << endl;
            }
            cout << endl;
        }
        
        cin >> carChoice;
        carChoice = carChoice % CAR_DESIGNS.size();
        
        // Clear input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // Add player car - start at position 0
        cars.push_back(Car(numLanes / 2, carChoice, playerName, true));
        playerCarIndex = 0;
        
        // Add AI cars - number based on difficulty, but start them much further away
        int numAICars = 6 + (difficulty * 2);
        for (int i = 0; i < numAICars; i++) {
            int aiLane = rand() % numLanes;
            int aiCarType = rand() % CAR_DESIGNS.size();
            int aiPosition = 100 + rand() % 200; // Start AI cars 100-300 positions ahead
            
            cars.push_back(Car(aiLane, aiCarType));
            cars.back().changeLane(aiLane);
            
            // Set initial positions for AI cars
            for (int j = 0; j < aiPosition; j++) {
                cars.back().move();
            }
        }
        
        // Ready to start message with detailed instructions
        system("cls");
        cout << "Get ready to race!\n\n";
        cout << "Difficulty: " << (difficulty == 1 ? "Easy" : (difficulty == 2 ? "Medium" : "Hard")) << "\n\n";
        cout << "IMPORTANT TIPS:\n";
        cout << "- You'll start in PRACTICE MODE with no AI cars nearby\n";
        cout << "- Use this time to get familiar with controls\n";
        cout << "- Your car starts at speed 0, press W to accelerate\n";
        cout << "- Press A/D to change lanes\n";
        cout << "- Press SPACE for nitro boost when you need extra speed\n\n";
        
        cout << "Press any key to start the game...";
        _getch();
        gameStarted = true;
    }

    void handleInput() {
        if (_kbhit()) {
            char key = _getch();
            Car& playerCar = cars[playerCarIndex];
            
            switch (key) {
                case 'w': case 'W': playerCar.accelerate(); break;
                case 's': case 'S': playerCar.decelerate(); break;
                case 'a': case 'A': 
                    if (playerCar.getLane() > 0) 
                        playerCar.changeLane(playerCar.getLane() - 1); 
                    break;
                case 'd': case 'D': 
                    if (playerCar.getLane() < numLanes - 1) 
                        playerCar.changeLane(playerCar.getLane() + 1); 
                    break;
                case ' ': playerCar.useNitro(); break; // Space for nitro
                case 'q': case 'Q': gameOver = true; break;
            }
        }
    }

    void update() {
        // Only update every 5 frames to slow down the game
        frameCount++;
        if (frameCount % 5 != 0) return;
        
        // Update practice mode countdown
        if (practiceMode) {
            countdownTimer--;
            if (countdownTimer <= 0) {
                practiceMode = false;
            }
        }
        
        // Move player car
        cars[playerCarIndex].move();
        
        // Only move AI cars if not in practice mode
        if (!practiceMode) {
            for (size_t i = 1; i < cars.size(); i++) {
                cars[i].move();
            }
        }
        
        // Check if player has finished the race
        if (cars[playerCarIndex].getPosition() >= trackLength) {
            gameOver = true;
            system("cls");
            cout << "\n\nCongratulations! You won the race!" << endl;
            cars[playerCarIndex].addScore(1000);
            return;
        }
        
        // Check if any AI car has finished the race
        for (size_t i = 1; i < cars.size(); i++) {
            if (cars[i].getPosition() >= trackLength) {
                gameOver = true;
                system("cls");
                cout << "\n\nAn AI car won the race. Better luck next time!" << endl;
                return;
            }
        }
        
        // Check for collisions between player and AI cars
        Car& playerCar = cars[playerCarIndex];
        for (size_t i = 1; i < cars.size(); i++) {
            if (playerCar.getLane() == cars[i].getLane()) {
                int posDiff = abs(playerCar.getPosition() - cars[i].getPosition());
                if (posDiff < 4) {  // Collision detected
                    system("cls");
                    cout << "\n\nCrash! Game over." << endl;
                    gameOver = true;
                    return;
                }
            }
        }
    }

    void render() {
        system("cls");  // Clear screen (Windows)
        
        // Display track info
        cout << "EXTREME ASCII RACING - W/S: speed, A/D: change lanes, SPACE: nitro, Q: quit\n";
        cout << "Player: " << cars[playerCarIndex].getPlayerName() 
             << " | Speed: " << cars[playerCarIndex].getSpeed() 
             << " | Position: " << cars[playerCarIndex].getPosition() << "/" << trackLength;
        
        // Display practice mode info
        if (practiceMode) {
            cout << " | PRACTICE MODE: " << (countdownTimer / 20) + 1 << " seconds left";
        }
        
        cout << "\n\n";
        
        // Create track view - much bigger
        vector<vector<char>> track(numLanes, vector<char>(screenWidth, ' '));
        
        // Draw lane dividers - simpler, just dots
        for (int i = 0; i < numLanes; i++) {
            for (int j = 0; j < screenWidth; j += 5) {
                track[i][j] = '.';
            }
        }
        
        // Draw cars
        Car& playerCar = cars[playerCarIndex];
        int viewportStart = max(0, playerCar.getPosition() - screenWidth / 4);
        
        for (const auto& car : cars) {
            int carPos = car.getPosition() - viewportStart;
            if (carPos >= 0 && carPos < screenWidth - 15) {
                int carLane = car.getLane();
                int carType = car.getCarType();
                
                // Skip if car is out of visible lanes
                if (carLane < 0 || carLane >= numLanes) continue;
                
                // Draw the car ASCII art
                for (size_t i = 0; i < CAR_DESIGNS[carType].size() && carLane + i < numLanes; i++) {
                    const string& line = CAR_DESIGNS[carType][i];
                    for (size_t j = 0; j < line.size() && carPos + j < screenWidth; j++) {
                        if (line[j] != ' ') {
                            track[carLane + i][carPos + j] = line[j];
                        }
                    }
                }
            }
        }
        
        // Draw finish line
        int finishPos = trackLength - viewportStart;
        if (finishPos >= 0 && finishPos < screenWidth) {
            for (int i = 0; i < numLanes; i++) {
                track[i][finishPos] = '|';
            }
        }
        
        // Draw road edges
        for (int j = 0; j < screenWidth + 2; j++) {
            cout << "#";
        }
        cout << endl;
        
        // Render track
        for (const auto& lane : track) {
            cout << "#";
            for (char c : lane) {
                cout << c;
            }
            cout << "#" << endl;
        }
        
        // Draw road edges
        for (int j = 0; j < screenWidth + 2; j++) {
            cout << "#";
        }
        cout << endl;
        
        // Display race positions
        cout << "\nRACE POSITIONS:\n";
        
        // Create a copy of cars sorted by position
        vector<reference_wrapper<Car>> sortedCars(cars.begin(), cars.end());
        sort(sortedCars.begin(), sortedCars.end(), 
             [](const Car& a, const Car& b) { return a.getPosition() > b.getPosition(); });
        
        // Display top 5 positions
        for (size_t i = 0; i < min(size_t(5), sortedCars.size()); i++) {
            const Car& car = sortedCars[i];
            cout << (i + 1) << ". " 
                 << (car.isPlayerCar() ? car.getPlayerName() : "AI Car " + to_string(i)) 
                 << " - Position: " << car.getPosition() << endl;
        }
        
        // Display controls reminder
        cout << "\nCONTROLS: W-Accelerate, S-Brake, A/D-Change Lanes, SPACE-Nitro\n";
    }

    bool isGameOver() const {
        return gameOver;
    }

    void saveHighScore() {
        int playerScore = cars[playerCarIndex].getScore();
        string playerName = cars[playerCarIndex].getPlayerName();
        
        // Add current score to high scores
        highScores.push_back(make_pair(playerName, playerScore));
        
        // Sort high scores
        sort(highScores.begin(), highScores.end(), 
             [](const pair<string, int>& a, const pair<string, int>& b) { 
                 return a.second > b.second; 
             });
        
        // Keep only top 10
        if (highScores.size() > 10) {
            highScores.resize(10);
        }
    }

    void displayHighScores() {
        system("cls");
        cout << "=== HIGH SCORES ===\n\n";
        
        if (highScores.empty()) {
            cout << "No high scores yet!\n";
        } else {
            cout << setw(4) << "Rank" << setw(20) << "Player" << setw(10) << "Score" << endl;
            cout << string(34, '-') << endl;
            
            for (size_t i = 0; i < highScores.size(); i++) {
                cout << setw(4) << (i + 1) << setw(20) << highScores[i].first 
                     << setw(10) << highScores[i].second << endl;
            }
        }
        
        cout << "\nYour score: " << cars[playerCarIndex].getScore() << endl;
        cout << "\nPress any key to exit...";
        _getch();
    }

    void run() {
        while (!isGameOver()) {
            handleInput();
            update();
            render();
            this_thread::sleep_for(chrono::milliseconds(100)); // Slower refresh rate
        }
        
        // Save high score
        saveHighScore();
        
        // Game over screen
        cout << "\nFinal Results:\n";
        cout << "Your position: " << cars[playerCarIndex].getPosition() << "/" << trackLength << "\n";
        cout << "Your score: " << cars[playerCarIndex].getScore() << "\n\n";
        cout << "Press any key to see high scores...";
        _getch();
        
        // Display high scores
        displayHighScores();
    }
};

int main() {
    bool playAgain = true;
    
    while (playAgain) {
        Game game;
        game.run();
        
        cout << "Play again? (y/n): ";
        char choice;
        cin >> choice;
        playAgain = (choice == 'y' || choice == 'Y');
    }
    
    return 0;
}