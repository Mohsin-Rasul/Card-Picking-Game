#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>

using namespace std;

const int MAX_PLAYERS = 4;
const int MAX_CARDS = 52;
const int SCREEN_WIDTH = 80;

struct Card {
    string suit;
    string rank;
    int value;
};

struct Player {
    string name;
    Card hand[MAX_CARDS];
    int handSize;
    int score;
};

void centerText(const string& text) {
    int padding = (SCREEN_WIDTH - text.length()) / 2;
    for (int i = 0; i < padding; i++) cout << ' ';
    cout << text << endl;
}

void initializeDeck(Card deck[MAX_CARDS]) {
    string suits[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
    string ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };
    int values[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11 };

    int index = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            deck[index] = { suits[i], ranks[j], values[j] };
            index++;
        }
    }
}

void shuffleDeck(Card deck[MAX_CARDS]) {
    srand(static_cast<unsigned>(time(0)));
    for (int i = MAX_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void displayCard(const Card& card) {
    string suitSymbol = (card.suit == "Hearts") ? "♥" : (card.suit == "Diamonds") ? "♦" :
        (card.suit == "Clubs") ? "♣" : "♠";

    string cardLines[9] = {
        "+-------------+",
        "| " + card.rank + string(11 - card.rank.length(), ' ') + " |",
        "|             |",
        "|             |",
        "|      " + suitSymbol + "      |",
        "|             |",
        "|             |",
        "| " + string(11 - card.rank.length(), ' ') + card.rank + " |",
        "+-------------+"
    };

    for (int i = 0; i < 9; i++) {
        centerText(cardLines[i]);
    }
}

int calculateHandValue(const Card hand[], int handSize) {
    int total = 0;
    int aces = 0;
    for (int i = 0; i < handSize; i++) {
        total += hand[i].value;
        if (hand[i].rank == "Ace") aces++;
    }
    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }
    return total;
}

void playerTurn(Player& player, Card deck[], int& deckSize, bool isComputer = false) {
    string turnMessage = "=== " + player.name + "'s Turn ===";
    centerText(turnMessage);
    while (true) {
        Card drawnCard = deck[--deckSize];
        player.hand[player.handSize++] = drawnCard;

        string drawMessage = player.name + " drew:";
        centerText(drawMessage);
        displayCard(drawnCard);
        int total = calculateHandValue(player.hand, player.handSize);
        string totalMessage = "Total so far: " + to_string(total);
        centerText(totalMessage);

        if (total > 21) {
            centerText("Oh no! Total exceeds 21!");
            break;
        }

        if (isComputer) {
            if (total >= 17) {
                centerText("Computer chooses to stop drawing.");
                break;
            }
            centerText("Computer decides to draw again.");
            continue;
        }

        char choice;
        while (true) {
            centerText("Do you want to draw another card? (y/n): ");
            cin >> choice;
            if (choice != 'y' && choice != 'Y' && choice != 'n' && choice != 'N') {
                cin.clear();
                cin.ignore(1000, '\n');
                centerText("Invalid input. Please enter 'y' or 'n'.");
            }
            else {
                break;
            }
        }
        if (choice == 'n' || choice == 'N') break;
    }
}

void displayScores(const Player players[], int playerCount) {
    centerText("=== Current Scores ===");
    for (int i = 0; i < playerCount; i++) {
        string scoreMessage = players[i].name + ": " + to_string(players[i].score);
        centerText(scoreMessage);
    }
}

void saveGameState(const Player players[], int playerCount, const string& filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        for (int i = 0; i < playerCount; i++) {
            outFile << players[i].name << " " << players[i].score << endl;
        }
        outFile.close();
        centerText("Game state saved successfully!");
    }
    else {
        centerText("Unable to save game state.");
    }
}

void loadGameState(Player players[], int& playerCount, const string& filename) {
    ifstream inFile(filename);
    if (inFile.is_open()) {
        playerCount = 0;
        string name;
        int score;
        while (inFile >> name >> score && playerCount < MAX_PLAYERS) {
            players[playerCount] = { name, {}, 0, score };
            playerCount++;
        }
        inFile.close();
        centerText("Game state loaded successfully!");
    }
    else {
        centerText("Unable to load game state. Starting a new game.");
    }
}

int main() {
    Card deck[MAX_CARDS];
    Player players[MAX_PLAYERS];
    int playerCount = 0;
    int deckSize = MAX_CARDS;

    centerText("Enter the number of players (2-4): ");
    cin >> playerCount;
    while (playerCount < 2 || playerCount > MAX_PLAYERS) {
        cin.clear();
        cin.ignore(1000, '\n');
        centerText("Invalid input. Please enter a number between 2 and 4: ");
        cin >> playerCount;
    }

    for (int i = 0; i < playerCount; i++) {
        string name;
        string namePrompt = "Enter name for Player " + to_string(i + 1) + ": ";
        centerText(namePrompt);
        cin >> name;
        players[i] = { name, {}, 0, 0 };
    }

    char loadChoice;
    centerText("Do you want to load a previous game state? (y/n): ");
    cin >> loadChoice;
    if (loadChoice == 'y' || loadChoice == 'Y') {
        loadGameState(players, playerCount, "game_state.txt");
    }

    while (true) {
        initializeDeck(deck);
        shuffleDeck(deck);
        deckSize = MAX_CARDS;

        for (int i = 0; i < playerCount; i++) {
            players[i].handSize = 0;
            playerTurn(players[i], deck, deckSize, players[i].name == "Computer");
        }

        centerText("=== Round Results ===");
        int maxScore = 0;
        int winnerCount = 0;
        int winners[MAX_PLAYERS];
        for (int i = 0; i < playerCount; i++) {
            int score = calculateHandValue(players[i].hand, players[i].handSize);
            string scoreMessage = players[i].name + "'s Total: " + to_string(score);
            centerText(scoreMessage);
            if (score <= 21 && score >= maxScore) {
                if (score > maxScore) {
                    winnerCount = 0;
                    maxScore = score;
                }
                winners[winnerCount++] = i;
            }
        }

        if (winnerCount == 0) {
            centerText("No winners this round. Everyone went over 21!");
        }
        else if (winnerCount == 1) {
            string winMessage = players[winners[0]].name + " wins this round!";
            centerText(winMessage);
            players[winners[0]].score++;
        }
        else {
            string tieMessage = "It's a tie between: ";
            for (int i = 0; i < winnerCount; i++) {
                tieMessage += players[winners[i]].name;
                if (i < winnerCount - 1) tieMessage += ", ";
                players[winners[i]].score++;
            }
            centerText(tieMessage);
        }

        displayScores(players, playerCount);

        char playAgain;
        while (true) {
            centerText("Do you want to play another round? (y/n): ");
            cin >> playAgain;
            if (playAgain != 'y' && playAgain != 'Y' && playAgain != 'n' && playAgain != 'N') {
                cin.clear();
                cin.ignore(1000, '\n');
                centerText("Invalid input. Please enter 'y' or 'n'.");
            }
            else {
                break;
            }
        }
        if (playAgain == 'n' || playAgain == 'N') break;
    }

    centerText("=== Final Results ===");
    displayScores(players, playerCount);

    int maxScore = 0;
    int winnerCount = 0;
    int winners[MAX_PLAYERS];
    for (int i = 0; i < playerCount; i++) {
        if (players[i].score >= maxScore) {
            if (players[i].score > maxScore) {
                winnerCount = 0;
                maxScore = players[i].score;
            }
            winners[winnerCount++] = i;
        }
    }

    if (winnerCount == 1) {
        string finalWinMessage = players[winners[0]].name + " is the overall winner!";
        centerText(finalWinMessage);
    }
    else {
        string tieMessage = "It's an overall tie between: ";
        for (int i = 0; i < winnerCount; i++) {
            tieMessage += players[winners[i]].name;
            if (i < winnerCount - 1) tieMessage += ", ";
        }
        centerText(tieMessage);
    }

    char saveChoice;
    centerText("Do you want to save the game state? (y/n): ");
    cin >> saveChoice;
    if (saveChoice == 'y' || saveChoice == 'Y') {
        saveGameState(players, playerCount, "game_state.txt");
    }

    centerText("Thank you for playing! Goodbye!");
    return 0;
}