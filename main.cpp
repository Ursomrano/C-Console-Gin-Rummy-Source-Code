#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <unordered_map>
#include <cctype>
#include <array>
#include <map>
#include <chrono>
#include <thread>

using namespace std;

class Deck{
public:
    static unordered_map<string,int> DeckGenerator(){
        unordered_map<string,int> generatingDeck;
        static string cardSuits[4] = {" of Clubs", " of Diamonds", " of Hearts", " of Spades"};

        for (int a = 0; a<13; a++){
            for (const auto &cardSuit: cardSuits){
                ostringstream key;
                switch (a){
                    default:
                        key << (a+1) << cardSuit;
                        generatingDeck[key.str()] = a+1;
                        break;
                    case 0:
                        key << "Ace" << cardSuit;
                        generatingDeck[key.str()] = a+1;
                        break;
                    case 10:
                        key << "Jack" << cardSuit;
                        generatingDeck[key.str()] = a+1;
                        break;
                    case 11:
                        key << "Queen" << cardSuit;
                        generatingDeck[key.str()] = a+1;
                        break;
                    case 12:
                        key << "King" << cardSuit;
                        generatingDeck[key.str()] = a+1;
                        break;
                }
            }
        }
        return generatingDeck;
    }
    static unordered_map<string,int> DeckShuffler(const unordered_map<string,int>& unshuffledDeck){
        random_device rand;
        mt19937  g(rand());

        vector<pair<string,int>> tempVec(unshuffledDeck.begin(),unshuffledDeck.end());
        shuffle(tempVec.begin(),tempVec.end(), g);
        unordered_map<string,int> shuffledDeck(tempVec.begin(),tempVec.end());

        return shuffledDeck;
    }
};

class GameSetup{
public:
    static array<vector<pair<string,int>>,2> DealCards(unordered_map<string,int>& deck){
        vector<string> dealtCards;
        array<vector<pair<string,int>>,2> hands;

        for (auto &hand:hands){
            auto deckLocation = deck.begin();
            hand.resize(10);

            for (int b=0;b<10;b++){
                if (deckLocation == deck.end()){
                    cout << "Deck is empty"<<endl;
                    break;
                }
                else{
                    dealtCards.push_back(deckLocation->first);
                    deckLocation++;
                    hand[b] = make_pair(dealtCards[b],deck[dealtCards[b]]);
                    deck.erase(dealtCards[b]);
                }
            }
            dealtCards.clear();
        }
        return hands;
    }
    static vector<pair<string,int>> GenerateDiscardPile(unordered_map<string,int>& deck){
        vector<pair<string,int>> discardPile;

        if (!deck.empty()){
            discardPile.emplace_back(deck.begin()->first, deck.begin()->second);
            deck.erase(deck.begin());
        }
        else{
            cout << "The deck is empty";
        }
        return discardPile;
    }
};

class Turns{
public:
    static void Player(vector<pair<string,int>>& hand, unordered_map<string,int>& deck, vector<pair<string,int>>& discardPile){
        string cardToRemove;
        int cardToRemoveLocation = -1;
        string replaceFrom;
        pair<string,int> discardCard = make_pair(discardPile.begin()->first, discardPile.begin()->second);

        cout <<endl<< "Choose which card you want to remove"<< endl;
        getline(cin,cardToRemove);

        if (isalpha(cardToRemove[0])){
            cardToRemove[0] = toupper(cardToRemove[0]);
        }
        size_t secondSpace = (cardToRemove.find_first_of(' ')) +3;
        cardToRemove[secondSpace+1] = toupper(cardToRemove[secondSpace+1]);

        for (int c=0; c<hand.size(); c++){
            if (hand[c].first == cardToRemove){
                cardToRemoveLocation = c;
                break;
            }
        }

        if(cardToRemoveLocation != -1){
            discardPile.insert(discardPile.begin(), make_pair(hand[cardToRemoveLocation].first,hand[cardToRemoveLocation].second));
            hand.erase(hand.begin() + cardToRemoveLocation);
        }
        else {
            cout << "card not found in hand, try again" << endl;
            return;
        }

        cout << "Do you wish to draw from the deck or discard pile?: "<<endl;
        getline(cin,replaceFrom);

        for(char d: replaceFrom)
            tolower(d);

        if (replaceFrom.find("deck") != string::npos && !deck.empty()){
            cout << "You drew the card: "<<deck.begin()->first<<endl;
            this_thread::sleep_for(chrono::seconds(3));

            hand.emplace_back(deck.begin()->first,deck.begin()->second);
            deck.erase(deck.begin());
        }
        else if (replaceFrom.find("discard") != string::npos && !discardPile.empty()){
            cout << "You drew the card: "<<discardCard.first<<endl;
            this_thread::sleep_for(chrono::seconds(3));

            hand.emplace_back(discardCard.first, discardCard.second);
            discardPile.erase(discardPile.begin()+1);
        }
        else{
            cout <<"Either you typed the wrong input or the discard pile/deck is empty"<<endl;
        }

        sort(hand.begin(),hand.end(), CompareCards);

        if (DeadwoodCount(hand) == 1){
            cout << "The current player has won!!"<<endl;
            this_thread::sleep_for(chrono::seconds(3));
            exit(0);
        }
    }
    static bool CompareCards(pair<string,int>& a, pair<string,int>& b){
        if (a.second == b.second){
            return a.first < b.first;
        }
        else{
            return a.second<b.second;
        }
    }
private:
    static int DeadwoodCount(vector<pair<string,int>>& hand){
        map<pair<char,int>,int> sameValueOccurrences;
        vector<pair<char,int>> valueSets;
        map<string,vector<int>> sameSuitOccurrences;
        map<string,vector<int>> suitSets;
        int allSuitSets = 0;
        int deadwoodCount;

        for(const auto& card:hand){
            sameValueOccurrences[make_pair(card.first[0], card.second)]++;
            string suit = card.first.substr(card.first.find_last_of(" ")+1);
            sameSuitOccurrences[suit].push_back(card.second);
        }

        for(auto& key:sameValueOccurrences){
            if(key.second>=3){
                for (int e=0;e<key.second;e++){
                    valueSets.emplace_back(key.first);
                }
            }
        }
        for (const auto& key:valueSets){
            sameValueOccurrences.erase(key);
        }

        for(auto& key:sameSuitOccurrences){
            if (key.second.size()>=3){
                for(int f=1; f<key.second.size();f++){
                    if (IsInValueSet(valueSets, key, f)){
                        continue;
                    }
                    else if (key.second[f] == key.second[f+1]-1 || key.second[f] == key.second[f-1]+1){
                        suitSets[key.first].emplace_back(key.second[f]);
                    }
                }
            }
        }
        for (const auto& key:suitSets){
            sameSuitOccurrences.erase(key.first);
        }
        for(auto& key:suitSets){
            allSuitSets+=key.second.size();
        }

        deadwoodCount = hand.size() - (valueSets.size() + allSuitSets);
        return deadwoodCount;
    }
    static bool IsInValueSet(vector<pair<char,int>>& valueSets, pair<const basic_string<char>,vector<int>> key, int& i){
        for (auto& valueSet:valueSets){
            if (valueSet.second == key.second[i]){
                return true;
            }
        }
        return false;
    }
};

static void PrintInfo(vector<pair<string,int>>& hand, vector<pair<string,int>>& discardPile, int& currentPlayer){
    auto handLocation = hand.begin();

    sort(hand.begin(),hand.end(),Turns::CompareCards);

    system("clear");
    cout<<"Current Player: "<<currentPlayer+1<<endl<<"Your hand: ";
    while (handLocation != hand.end()){
        cout <<handLocation->first;
        if(++handLocation != hand.end()){
            cout << ", ";
        }
    }
    cout <<endl<<"Card on top of discard pile: "<<discardPile[0].first;
}

int main() {
    unordered_map<string,int> deck = Deck::DeckShuffler(Deck::DeckGenerator());
    array<vector<pair<string,int>>,2> hands = GameSetup::DealCards(deck);
    vector<pair<string,int>> discardPile = GameSetup::GenerateDiscardPile(deck);

    while (true){
        int currentPlayer = 0;
        PrintInfo(hands[0], discardPile, currentPlayer);
        Turns::Player(hands[0], deck, discardPile);
        currentPlayer++;
        PrintInfo(hands[1], discardPile, currentPlayer);
        Turns::Player(hands[1], deck, discardPile);
    }
    return 0;
}
