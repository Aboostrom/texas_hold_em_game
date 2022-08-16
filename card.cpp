#include <iostream>
#include <string>
#include <vector>

#include "card.h"

// Instantiate the card object with the input card number and card suit
// Also, create a string version of the card to call when necessary
Card::Card(int cardNumber, int suit) {
    this->cardNumber = cardNumber;
    this->suit = suit;
}

// Getter for the card number
int Card::GetCardNumber() {
    return cardNumber;
}

// Getter for the card suit
int Card::GetCardSuit() {
    return suit;
}

// Getter for the card as string
std::string Card::GetCardString() {
    return ConvertToString(cardNumber, suit);
}

// Take in the card suit number and card number and convert it to a string
// For example: (1, 4) becomes "Ace of Clubs"
std::string ConvertToString(int cardNumber, int suit) {
    // Use a switch depending on the suit number given
    switch(suit) {
        case 1: 
            return CombineNumberAndSuit(cardNumber, "Diamonds");
        case 2:
            return CombineNumberAndSuit(cardNumber, "Hearts");
        case 3:
            return CombineNumberAndSuit(cardNumber, "Spades");
        case 4:
            return CombineNumberAndSuit(cardNumber, "Clubs");
        default:
            return "ERROR: No suit found!";
    }
}

// Take the given suit from the ConvertToString function and add the correct associative number
// to the string
std::string CombineNumberAndSuit(int cardNumber, std::string suit) {
    // Use a switch depending on the card number given and attach it to the suit
    switch(cardNumber) {
        case 1:
            return "Ace of " + suit;
        case 11:
            return "Jack of " + suit;
        case 12:
            return "Queen of " + suit;
        case 13:
            return "King of " + suit;
        default:
            return std::to_string(cardNumber) + " of " + suit;
    }
}