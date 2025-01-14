#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>
using namespace std;

const int totalCards = 52;
const int maxHand = 11;

struct Card {
    string suit;
    string rank;
    int value;
};

struct Player {
    string name;
    Card hand[maxHand];
    int handCount = 0;
    int totalPoints = 0;
    bool stopped = false; 
};

void initializeDeck(Card deck[]) {
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

void shuffleDeck(Card deck[]) {
    srand(time(0));
    for (int i = totalCards - 1; i > 0; i--) {
        int randomIndex = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[randomIndex];
        deck[randomIndex] = temp;
    }
}

void displayCard(const Card& card) {
    cout << "+-------------+" << endl;
    cout << "| " << card.rank << string(11 - card.rank.length(), ' ') << " |" << endl;
    cout << "|             |" << endl;
    cout << "|             |" << endl;
    cout << "|      " << card.suit[0] << "      |" << endl;
    cout << "|             |" << endl;
    cout << "|             |" << endl;
    cout << "| " << string(11 - card.rank.length(), ' ') << card.rank << " |" << endl;
    cout << "+-------------+" << endl;
}

void drawCard(Player& player, Card deck[], bool usedCards[], bool isComputer = false) {
    int cardIndex;
    if (isComputer) {
        while (true) {
            cardIndex = rand() % totalCards;
            if (!usedCards[cardIndex]) {
                break;
            }
        }
        cout << "Computer picked a card." << endl;
    }
    else {
        while (true) {
            cout << player.name << ", choose a card number (1-52): ";
            cin >> cardIndex;
            if (cardIndex < 1 || cardIndex > 52 || usedCards[cardIndex - 1]) {
                cout << "Invalid choice or card already used. Try again!" << endl;
            }
            else {
                break;
            }
        }
        cardIndex--; 
    }

    usedCards[cardIndex] = true;
    player.hand[player.handCount++] = deck[cardIndex];
    player.totalPoints += deck[cardIndex].value;

    if (player.totalPoints > 21) {
        for (int i = 0; i < player.handCount; i++) {
            if (player.hand[i].rank == "Ace" && player.hand[i].value == 11) {
                player.hand[i].value = 1;
                player.totalPoints -= 10;
                break;
            }
        }
    }

    cout << player.name << " drew: " << endl;
    displayCard(deck[cardIndex]);
    cout << "Total points: " << player.totalPoints << endl;
}

void saveResult(string result) {
    static int gameNumber = 1;
    ofstream outFile("game_results.csv", ios::app);
    if (outFile.is_open()) {
        outFile << "Game " << gameNumber++ << endl;
        outFile << result << endl;
        outFile.close();
        cout << "Result saved to game_results.txt!" << endl;
    }
    else {
        cout << "Failed to save result!" << endl;
    }
}

int main() {
    Card deck[totalCards];
    bool usedCards[totalCards] = { false };
    Player player1, player2;
    player1.name = "Player 1";
    player2.name = "Player 2";

    initializeDeck(deck);
    shuffleDeck(deck);

    cout << "Welcome to the Card Picking Game - Blackjack" << endl;
    cout << "1 - Play with Computer" << endl;
    cout << "2 - Play with Player" << endl;
    cout << "3 - Exit" << endl;

    int choice;
    cin >> choice;
    if (choice == 3) {
        cout << "Goodbye!" << endl;
        return 0;
    }

    while (true) {
        player1.handCount = 0;
        player2.handCount = 0;
        player1.totalPoints = 0;
        player2.totalPoints = 0;
        player1.stopped = false;
        player2.stopped = false;

        cout << "Game started!" << endl;

        while (!player1.stopped || !player2.stopped) {
            if (!player1.stopped) {
                drawCard(player1, deck, usedCards);
                if (player1.totalPoints > 21) {
                    cout << player1.name << " busts! " << player2.name << " wins!" << endl;
                    saveResult(player1.name + " | Lose\n" + player2.name + " | Win");
                    break;
                }

                char choice;
                cout << player1.name << ", do you want to draw another card? (y/n): ";
                cin >> choice;
                if (choice == 'n' || choice == 'N') {
                    player1.stopped = true;
                }
            }

            if (!player2.stopped) {
                if (choice == 1) {
                    drawCard(player2, deck, usedCards, true); 
                    if (player2.totalPoints > 21) {
                        cout << player2.name << " busts! " << player1.name << " wins!" << endl;
                        saveResult(player1.name + " | Win\n" + player2.name + " | Lose");
                        break;
                    }
                }
                else {
                    drawCard(player2, deck, usedCards);
                    if (player2.totalPoints > 21) {
                        cout << player2.name << " busts! " << player1.name << " wins!" << endl;
                        saveResult(player1.name + " | Win\n" + player2.name + " | Lose");
                        break;
                    }

                    char choice;
                    cout << player2.name << ", do you want to draw another card? (y/n): ";
                    cin >> choice;
                    if (choice == 'n' || choice == 'N') {
                        player2.stopped = true;
                    }
                }
            }

            if (player1.stopped && player2.stopped) {
                if (player1.totalPoints > player2.totalPoints) {
                    cout << player1.name << " wins!" << endl;
                    saveResult(player1.name + " | Win\n" + player2.name + " | Lose");
                }
                else if (player1.totalPoints < player2.totalPoints) {
                    cout << player2.name << " wins!" << endl;
                    saveResult(player1.name + " | Lose\n" + player2.name + " | Win");
                }
                else {
                    cout << "It's a tie!" << endl;
                    saveResult(player1.name + " | Tie\n" + player2.name + " | Tie");
                }
                break;
            }
        }

        cout << "Play again? (1 for yes or 2 for no): ";
        cin >> choice;
        if (choice == 2) break;
    }

    cout << "Thank you for playing!" << endl;
    return 0;
}