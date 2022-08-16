#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

#include "deck.h"

// Initialization of the deck with the appropriate number of cards
Deck::Deck(int numDecks) {
    // If more than one deck is being combined in this set of cards, 
    // multiply the number of decks desired by 52 cards for each deck
    this->numCards = 52 * numDecks;
    
    // Populate the deck
    Deck::FillDeck();
    
    // Make sure the deck is appropriately randomized
    Deck::ShuffleDeck();
}

void Deck::FillDeck() {
    // Outer loop to handle the number of sets of 52 cards
    for (int decks = 0; decks < numCards / 52; decks++) {
        // First inner loop to handle each of the 4 suits in turn
        for (int suits = 0; suits < 4; suits++) {
            // Second inner loop to handle the thirteen different numbers for each suit
            for (int cards = 0; cards < 13; cards++) {
                // Use the suit number and the card number and assign it to the Card object 
                // (with addition so it starts at 1 as opposed to zero)
                Card newCard = Card(cards + 1, suits + 1);
                deck.push_back(newCard);
            }
        }
    }
}

// Getter function for getting the human-readable string version of the card
// "Queen of Hearts" instead of "12, 2"
std::string Deck::GetCard() {
    return deck.at(0).GetCardString();
}

void Deck::ShuffleDeck() {
    // Make the seed assigned to the exact time it is called so each seed will be different, 
    // thus fully randomizing the shuffle
    unsigned seed = std::chrono::system_clock::now()
                        .time_since_epoch()
                        .count();
    
    // Use the std built-in shuffle function to shuffle with the newly created seed 
    std::shuffle(std::begin(deck), std::end(deck), std::default_random_engine(seed));
}

// For each dealing of the cards, erase the dealt card from the deck
Card Deck::DealCard() {
    Card temp = deck.at(0);

    // Erase the top card after assigning it to a temp object
    deck.erase(deck.begin());

    // Return the dealt card so that it can be assigned to the appropriate hand
    return temp;
}