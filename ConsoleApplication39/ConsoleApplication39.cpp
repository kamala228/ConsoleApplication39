#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <windows.h>

using namespace std;

string suits[] = { "Черви", "Бубни", "Піки", "Трефи" };
string ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "В", "Д", "К", "Т" };

struct Card {
    int rank, suit;
    string toString() const {
        return ranks[rank] + " " + suits[suit];
    }
    bool operator<(const Card& other) const {
        return rank < other.rank;
    }
};

vector<Card> deck;
vector<vector<Card>> playersHands;
vector<Card> tableCards;
vector<int> playerChips;
int numPlayers;
int pot = 0;
const int betAmount = 10;

void createDeck() {
    deck.clear();
    for (int s = 0; s < 4; ++s)
        for (int r = 0; r < 13; ++r)
            deck.push_back({ r, s });
    srand(time(0));
    random_shuffle(deck.begin(), deck.end());
}

void dealToPlayers(int count) {
    for (int i = 0; i < numPlayers; ++i)
        for (int j = 0; j < count; ++j) {
            playersHands[i].push_back(deck.back());
            deck.pop_back();
        }
}

void dealToTable(int count) {
    for (int i = 0; i < count; ++i) {
        tableCards.push_back(deck.back());
        deck.pop_back();
    }
}

void showCardList(const vector<Card>& cards) {
    for (const auto& c : cards)
        cout << c.toString() << "  ";
    cout << endl;
}

bool askPlayersToBet(const string& message) {
    bool allAgree = true;
    for (int i = 0; i < numPlayers; ++i) {
        if (playerChips[i] < betAmount) {
            cout << "Гравець " << i + 1 << " не має достатньо фішок для ставки.\n";
            allAgree = false;
            continue;
        }
        string answer;
        cout << "Гравець " << i + 1 << ", " << message << " (так/ні): ";
        cin >> answer;
        if (answer == "так") {
            playerChips[i] -= betAmount;
            pot += betAmount;
        }
        else {
            allAgree = false;
        }
    }
    return allAgree;
}

Card getBestCard(const vector<Card>& hand) {
    Card best = hand[0];
    for (const Card& c : hand)
        if (c.rank > best.rank)
            best = c;
    return best;
}

int determineWinner() {
    int winner = 0;
    Card bestCard = { -1, 0 };
    for (int i = 0; i < numPlayers; ++i) {
        vector<Card> combined = playersHands[i];
        combined.insert(combined.end(), tableCards.begin(), tableCards.end());
        Card currentBest = getBestCard(combined);
        cout << "Гравець " << i + 1 << " найкраща карта: " << currentBest.toString() << endl;
        if (currentBest.rank > bestCard.rank) {
            bestCard = currentBest;
            winner = i;
        }
    }
    return winner;
}

int main() {
    system("chcp 1251 > nul");
    cout << "Введіть кількість гравців: ";
    cin >> numPlayers;

    playerChips.assign(numPlayers, 100); // Кожен починає з 100 фішками
    playersHands.resize(numPlayers);
    createDeck();
    dealToPlayers(2);

    cout << "\n--- Роздані карти гравцям ---\n";
    for (int i = 0; i < numPlayers; ++i) {
        cout << "Гравець " << i + 1 << ": ";
        showCardList(playersHands[i]);
    }

    if (!askPlayersToBet("хочете зробити ставку?")) {
        cout << "Гра завершена. Не всі гравці поставили ставки.\n";
        return 0;
    }

    dealToTable(3);
    cout << "\n--- На столі відкрито 3 карти ---\n";
    showCardList(tableCards);

    if (askPlayersToBet("хочете підняти ставку?")) {
        dealToTable(1);
        cout << "\n--- Додано 4-а карта ---\n";
        showCardList(tableCards);

        if (askPlayersToBet("ще раз підняти ставку?")) {
            dealToTable(1);
            cout << "\n--- Додано 5-а карта ---\n";
            showCardList(tableCards);

            if (askPlayersToBet("підняти ставку востаннє?")) {
                dealToTable(1);
                cout << "\n--- Додано 6-а карта ---\n";
                showCardList(tableCards);
            }
        }
    }

    cout << "\n--- Кінцевий розклад ---\n";
    cout << "Карти на столі: ";
    showCardList(tableCards);

    for (int i = 0; i < numPlayers; ++i) {
        cout << "Гравець " << i + 1 << ": ";
        showCardList(playersHands[i]);
    }

    int winner = determineWinner();
    cout << "\n*** Переможець: Гравець " << winner + 1 << " ***\n";
    cout << "Виграш: " << pot << " фішок\n";
    playerChips[winner] += pot;
    pot = 0;

    cout << "\n--- Баланс гравців ---\n";
    for (int i = 0; i < numPlayers; ++i)
        cout << "Гравець " << i + 1 << ": " << playerChips[i] << " фішок\n";

    return 0;
}
