#ifndef CARD_H
#define CARD_H

#include <string>
#include <vector>

// Class created for holding the card's descriptions as a pair (suit and card number)
class Card {
    public:
        Card(int cardNumber = 0, int suit = 0);
        std::string GetCardString();
        int GetCardNumber();
        int GetCardSuit();
    
    private:
        int cardNumber;
        int suit;
};

// Take the numerical value for the card "(1, 4)", and convert it to a string:
// "Ace of clubs"
std::string ConvertToString(int cardNumber, int suit);
// Change the card number to "Ace", "King", etc., as necessary for the the string
std::string CombineNumberAndSuit(int cardNumber, std::string suit);

#endif