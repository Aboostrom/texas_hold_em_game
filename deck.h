#ifndef DECK_H
#define DECK_H

#include "card.h"

// Class created for building the deck of 52 cards and then shuffling them
class Deck {
    public:
        Deck(int numDecks = 1);
        std::string GetCard();
        Card DealCard();
        int CardsLeft();
        void FillDeck();
        void ShuffleDeck();
    
    private:
        int numCards;
        std::vector<Card> deck;
};

#endif