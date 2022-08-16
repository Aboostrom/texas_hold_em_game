#include <iostream>
#include <string>
#include <vector>

#include "player.h"

// Initializer, creating a new player with 50 chips and no starting bet
// Also assigning the player with the input name
Player::Player(std::string name) {
    this->name = name;
    this->chips = 50;
    totalBet = 0;
    dealer = false;
    folded = false;
}

// A testing function to check to make sure hand analyzing functions are correct
void Player::PrintHandStats() {
    for (size_t i = 0; i < handStats.size(); i++) {
        std::cout << handStats.at(i) << " ";
    }
    // Print the hand to compare with the handStats that the computer assesses
    PrintHand();
}

// A getter for chips
int Player::GetChips() {
    return chips;
}

// A getter for the player's name
std::string Player::GetName() {
    return name;
}

// A getter for the total amount bet within a hand
int Player::GetTotalBet() {
    return totalBet;
}

// A getter for the determined quality of the hand (more detail below)
std::vector<int> Player::GetHandStats() {
    return handStats;
}

// A getter for the hand itself
std::vector<Card> Player::GetHand() {
    return hand;
}

// A getter for whether the player has folded
bool Player::GetFoldedStat() {
    return folded;
}

// A getter for whether the player is all-in
bool Player::GetAllInStat() {
    // If the player doesn't have chips, the player is all-in
    if (!chips) {
        return true;
    }
    // Otherwise the player is not all-in
    return false;
}

// A setter for the folded statement of the player
void Player::SetFolded(bool value) {
    folded = value;
}

// A function used at the end of a hand for a winning player to take
// the total bet from each other player and add it to his own chips
void Player::TakeWinnings(int losingPlayerChips, int winnerShare) {
    /* Check if the losing player had bet less than the winner's bet.
    For example, if the losing player bet 5 and went all-in, but the
    winner had bet more, the winner only gets the loser's 5. If, however,
    the winner bet 5 and went all-in, but the loser bet more, then the winner
    only gets the 5 that matched his bet.
    */
    if (losingPlayerChips <= winnerShare) {
        chips += losingPlayerChips;
    }
    else if (losingPlayerChips > winnerShare) {
        chips += winnerShare;
    }
}

// A function to empty the total bet at the end of each round
// after it has been counted to the winner's pile
void Player::SubtractTotalBet(int lostAmount) {
    /* This checks if the amount taken (in the previous function)
    was the entire bet of the current player or only some portion (described above).
    If some portion remains after the winner takes his share, then another player will, while the pot is still being divvied,
    receive that money until everyone's total bet is back to 0 
    */
    if (totalBet <= lostAmount) {
        totalBet = 0;
    }
    else {
        totalBet -= lostAmount;
    }
}

// Delete all the cards in the player's hand before starting a new round
void Player::EmptyHand() {
    hand.clear();
}

// Delete the previous hand's stats before starting a new round
void Player::EmptyHandStats() {
    handStats = {0, 0, 0, 0, 0, 0};
}

// A getter for determing if the current player is marked as the dealer
bool Player::isDealer() {
    return dealer;
}

// A method for rotating the dealer, by unmarking the current dealer 
// and marking the next player in the order as the dealer
void Player::FlipDealerStat() {
    if (dealer) {
        dealer = false;
    }
    else {
        dealer = true;
    }
}

// Receive the ante amount and subtract the chips necessary and add it to the totalBet
void Player::Ante(int amount) {
    chips -= amount;
    totalBet += amount;
}

// Receive a card, place it in "hand", and then sort the cards as necessary
void Player::TakeCard(Card dealtCard) {
    // If the hand will have more than one card when taking the given card, 
    // start sorting cards
    if (hand.size() >= 1) {
        SortHand(dealtCard);
    }
    // Otherwise, just add the card to the "hand" vector
    else {
        hand.push_back(dealtCard);
    }
}

// Order the cards from lowest to highest
void Player::SortHand(Card dealtCard) {
    // Go through the hand until the dealt card finds a card that is bigger than (or equal to) it
    // If it finds a bigger card, insert the dealt card just before the bigger card and end the function
    for (size_t i = 0; i < hand.size(); i++) {
        if (dealtCard.GetCardNumber() <= hand.at(i).GetCardNumber()) {
            hand.insert(hand.begin() + i, dealtCard);
            return;
        }
    }

    // If it doesn't find a bigger card, just put the dealt card at the back of the hand
    hand.push_back(dealtCard);
}

// A function to interact with the human player when betting chips
int Player::BetChips(int highestBet) {
    std::string bet;
    int callingCost = highestBet - totalBet;

    // Check if the amount to call the current bet is too high for the current player to raise it
    // For example, if the current highest bet is 5, but the player only has 5 (or less),
    // the player cannot raise and bet 6. They are then limited to going all-in or folding
    if (callingCost >= chips) {
        // Explain their situation and offer a new choice
        std::string choice;
        std::cout << "You do not have enough chips to raise the current max bet. Would you like to go (a)ll-in or (f)old?\n";
        getline(std::cin, choice);

        // Loop to make sure the player inputs a readable response
        while (choice != "a" && choice != "a") {
            std::cout << "Select either 'a' for all-in or 'f' for fold: ";

            getline(std::cin, choice);
        }

        // If the player selects "all-in" 
        if (choice == "a") {
            // If the player has less than the calling cost,
            // adjust the calling cost so player only puts in what they have
            if (chips < callingCost) {
                callingCost = chips;
            }
            chips -= callingCost;
            totalBet += callingCost;
            return callingCost;
        }
        // Otherwise return 0, which will be interpreted as folding in the Round function
        else {
            return 0;
        }
    }

    // Otherwise, ask how much the player would like to raise
    std::cout << "How much would you like to raise?";
    std::cout << " (Enter a number between 1 and " << chips - callingCost << "): ";
    getline(std::cin, bet);

    // Loop to ensure the player only raises an amount they have
    while (!isdigit(bet[0]) || (stoi(bet) < 1 || stoi(bet) > chips - callingCost)) {
        std::cout << "Not a valid raise amount.";
        std::cout << " Enter a number between 1 and " << chips - callingCost << ": ";
        getline(std::cin, bet);
    }

    int betInt = stoi(bet);

    // Once the raise amount is determined, subtract that amount from their chips
    // and add that total amount (including the calling cost) to their totalBet
    chips -= betInt + callingCost;
    totalBet += betInt + callingCost;

    // Then return the total amount they are adding (just to make sure it isn't 0)
    return betInt + callingCost;
}

// Function to handle calling as opposed to raising
void Player::CallBet(int highestBet) {
    int callingCost = highestBet - totalBet;

    // If the calling cost is greater than their amount of chips,
    // adjust the calling cost to only take their total chip amount.
    // A function later in Round will remove them from play and mark them as all-in
    if (callingCost > chips) {
        callingCost = chips;
    }
    // Adjust the chip total and total bet as necessary and return
    chips -= callingCost;
    totalBet += callingCost;
    return;
}

// A print function so that cards are printed as words and not the number vector
// Used to show players what they have when betting for each round
void Player::PrintHand() {
    // For each card, print to the console its string version
    for (int i = 0; i < hand.size(); i++) {
        std::cout << hand.at(i).GetCardString();
        
        // Add ", " or ", and " depending on where it is in the sequence 
        if ((hand.size() == 2 && i == 0) || (hand.size() > 2 && i == hand.size()- 2)) {
            std::cout << ", and ";
        }
        else if (i != hand.size() - 1) {
            std::cout << ", ";
        }
    }
}

// Return a string represntation of the winner's handStats
// For example, if they had {10, 14, 0, 0, 0, 0} as the handStats,
// this function would return "a Royal flush, with Ace high"
std::string Player::GetBestHand() {
    // Create the string that all the words will be added to
    std::string handStat = "";

    // Use a switch to address the 10 different winning hand types
    switch(handStats.at(0)) {
        case 10:
            handStat += "a Royal flush";
            break;
        case 9:
            handStat += "a straight flush";
            break;
        case 8:
            // Incorporate the use of StringifyCardNumber to return the highest card number as a string
            return "a four-of-a-kind of " + StringifyCardNumber(handStats.at(1)) + "s";
        case 7:
            return "a full house, " + StringifyCardNumber(handStats.at(1)) + "s over " + StringifyCardNumber(handStats.at(2)) + "s";
        case 6:
            handStat += "a flush";
            break;
        case 5:
            handStat += "a straight";
            break;
        case 4:
            return "a three-of-a-kind of " + StringifyCardNumber(handStats.at(1)) + "s";
        case 3:
            return "two pair, with " + StringifyCardNumber(handStats.at(1)) + "s over " + StringifyCardNumber(handStats.at(2)) + "s";
        case 2:
            return "a pair of " + StringifyCardNumber(handStats.at(1)) + "s";
        case 0:
            return "No hand stats yet!";
    }
        
    // For the winning hands that did not have the addition of the card number before, add the high card here
    // If something has been added via the switch before this point,
    if (handStat.size() > 0) {
        // Append the high card to that existing string
        return handStat + ", " + StringifyCardNumber(handStats.at(1)) + " high";
    }
    // Otherwise just return the high card as the best hand
    else {
        return "a(n) " + StringifyCardNumber(handStats.at(1)) + " high";
    }
}

// Address the string versions of 11(Jack), 12(Queen), 13(King), 14/1(Ace), and 2(Deuce)
// Otherwise just return a string version of the card number
std::string Player::StringifyCardNumber(int card) {
    if (card > 2 && card < 11) {
        return std::to_string(card);
    }
    else if (card == 14 || card == 1) {
        return "Ace";
    }
    switch(card) {
        case 13:
            return "King";
        case 12:
            return "Queen";
        case 11:
            return "Jack";
        case 2:
            return "Deuce";
        default:
            return "ERROR: No card to match!";
    }
}


// This function looks through the entire hand and determines the best 
// possible combination of 5 cards according to the rules of the game
int Player::EvaluateCards(std::vector<Card> &communityHand) {
    // Before evaluating the hand, add the community cards to each player's hand
    // And order each card within the hand from smallest to biggest
    CombineHands(communityHand);

    // Then just check each function until one of them does not return 0
    // The first function that does not return 0 is the best possible hand
    return CheckForStraightFlush() || CheckForFourOfAKind() || 
        CheckForFullHouse() || CheckForFlush() || CheckForStraight() ||
        CheckForThreeOfAKind() || CheckForTwoPair() || CheckForPair() || 
        CheckForHighCard();
}

// For each community hand card, use the TakeCard function to take each card
// one at a time and then sort it into the player's hand
void Player::CombineHands(std::vector<Card> &communityHand) {
    for (size_t i = 0; i < communityHand.size(); i++) {
        TakeCard(communityHand.at(i));
    }
}

// Start the search for the best possible hand, a straight flush 
// For example: 7, 8, 9, 10, and Jack of spades
int Player::CheckForStraightFlush() {
    /* Create trackers for the number of cards currently in the straight, starting at one,
    because each card is a straight of one
    Then create a tracker for the highest card in the straight and the highest count 
    for if there are multiple straights of various lengths
    for example: 2, 3, 7, 8, 9 has two straights, one of length 2 and one of length 3
    */
    int numForStraight = 1;
    int highestCardIndex;
    int highestStraightCount = 0;

    // Then cycle through each suit, searching for a straight using only cards of that suit
    for (size_t suit = 1; suit < 5; suit++) {
        Card previousCard;
        Card currentCard;
        int numCardsInSuit = 1;

        // For each card in the hand, check if the card is of the specified suit
        for (size_t i = 0; i < hand.size(); i++) {
            if (hand.at(i).GetCardSuit() == suit) {   
                currentCard = hand.at(i);
                
                // If this is the first card found in the suit, ignore these steps
                if (numCardsInSuit > 1) {
                    // Otherwise, check if the current card is a step above the previous card found in the suit
                    if (currentCard.GetCardNumber() - 1 != previousCard.GetCardNumber() && currentCard.GetCardNumber() != previousCard.GetCardNumber()) {
                        numForStraight = 1;
                    }
                    // If the currentCard number is bigger by 1, then mark it as part of a straight
                    else if (currentCard.GetCardNumber() - 1 == previousCard.GetCardNumber()) {
                        numForStraight++;
                        if (numForStraight > highestStraightCount) {
                            highestStraightCount = numForStraight;
                            highestCardIndex = i;
                        }
                    }
                }
                // Then mark what was the currentCard as the previousCard and increment the number of cards in the suit
                previousCard = hand.at(i);
                numCardsInSuit++;
            }
        }

        numForStraight = 1;
    }

    // If the highest straight count is 4 or greater and the highest card in the straight is a king and the lowest card in the hand is an ace,
    // check if the ace is of the highest straight suit. If it is, the player has a Royal flush (rated 10)
    if (highestStraightCount >= 4 && hand.at(highestCardIndex).GetCardNumber() == 13 && hand.at(0).GetCardNumber() == 1) {
        size_t i = 0;
        while (hand.at(i).GetCardNumber() == 1) {
            if (hand.at(i).GetCardSuit() == hand.at(highestCardIndex).GetCardSuit()) {
                handStats.at(0) = 10;
                break;
            }

            i++;
        }
    }
    // Otherwise, if the straight is not Royal, but is of 5 or more cards,
    // It is a straight flush (rated 9), and the highest card is marked in the handStats vector
    else if (highestStraightCount >= 5) {
        handStats.at(0) = 9;
        handStats.at(1) = hand.at(highestCardIndex).GetCardNumber();
    }

    // Regardless of what is found, return the first number in the handStats vector
    // If it is 0, then the next possible winning hand type will be checked. If it is not zero,
    // the EvaluateCards function will return the vector showing this as the winning hand type
    return handStats.at(0);
}

// After checking for a straight flush, if necessary, check for 4-of-a-kind
int Player::CheckForFourOfAKind() {
    // Use the LookForSets function, specifying looking for 4 of the same card number
    int cardIndex = LookForSets(4);
    // If LookForSets does not return zero, then a 4-of-a-kind was found and the handStats vector is marked appropriately
    if (cardIndex != 0) {
        handStats.at(0) = 8;
        handStats.at(1) = hand.at(cardIndex).GetCardNumber();
        // Then the remaining card that is not of the 4-of-a-kind is selected as the highest card, in case of a tie
        GrabCardsInOrder(1);
    }

    // Then return the handStats first value
    return handStats.at(0);
}

// If a 4-of-a-kind is not present, check for full house (a pair and a 3-of-a-kind)
int Player::CheckForFullHouse() {
    int trioIndex;
    int pairIndex;

    // Use the LookForSets function looking for higher 3-of-a-kind and then lower pair
    trioIndex = LookForSets(3);
    pairIndex = LookForSets(2, trioIndex);

    // Do a second check in case the pair found consists of a 3-of-a-kind of Aces 
    // that should be marked as the higher set
    if (trioIndex && pairIndex && hand.at(pairIndex).GetCardNumber() == 1) {
        if (LookForSets(3, 3)) {
            handStats.at(0) = 7;
            handStats.at(1) = 14;
            handStats.at(2) = hand.at(trioIndex).GetCardNumber();
        }
    }
    // Do a check in case there is an Ace pair that is not found
    else if (trioIndex && pairIndex && hand.at(0).GetCardNumber() == 1) {
        if (LookForSets(2, 2)) {
            handStats.at(0) = 7;
            handStats.at(1) = hand.at(trioIndex).GetCardNumber();
            handStats.at(2) = 14;
        }
        else {
            handStats.at(0) = 7;
            handStats.at(1) = hand.at(trioIndex).GetCardNumber();
            handStats.at(2) = hand.at(pairIndex).GetCardNumber();
        }
    }
    // If both are found and no Aces present, mark the handStats accordingly
    else if (trioIndex && pairIndex) {
        handStats.at(0) = 7;
        handStats.at(1) = hand.at(trioIndex).GetCardNumber();
        handStats.at(2) = hand.at(pairIndex).GetCardNumber();

    }
    // If they are not found, check for a higher pair and a lower 3-of-a-kind
    else {
        pairIndex = LookForSets(2);
        trioIndex = LookForSets(3, pairIndex);
        if (trioIndex && pairIndex) {
            handStats.at(0) = 7;
            handStats.at(1) = hand.at(trioIndex).GetCardNumber();
            handStats.at(2) = hand.at(pairIndex).GetCardNumber();
        }
    }

    // Return the handStats first value
    return handStats.at(0);
}

// If there is no full house present, check for flush (5 cards of the same suit)
int Player::CheckForFlush() {
    int cardsInFlush = 1;
    int suitToFind;

    // Loop through the last 3 cards, starting with the highest card and going down
    // Only the last 3 cards are necessary because the flush requires 5 cards, and if
    // the 4th from the end is checked, only 4 cards total will be possible to be able to match that card's suit
    for (int i = hand.size() - 1; i >= 4; i--) {
        // Then do a second loop checking against the highest card to see if 4 other cards are of the same suit
        for (int j = i - 1; j >= 0; j--) {
            if (hand.at(i).GetCardSuit() == hand.at(j).GetCardSuit()) {
                cardsInFlush++;
                // If a flush is found with 5 cards, mark the handStats appropriately
                if (cardsInFlush == 5) {
                    handStats.at(0) = 6;
                    handStats.at(1) = hand.at(i).GetCardNumber();
                    suitToFind = hand.at(i).GetCardSuit();
                }
            }
        }
        // If the next card from the back is checked, cardsInFlush returns to 1 card
        cardsInFlush = 1;
    }

    // If a flush of 5 cards was found, 
    // to find the other cards in decending order for the handStats vector,
    // cycle through the cards in the hand again, looking for cards that match the suitToFind
    if (handStats.at(0)) {
        int grabbedCards = 0;
        int i = 0;
        int handStatsIndex = 2;

        // Check first, though, for an Ace of that suit, to mark it as 14 (high card)
        while (hand.at(i).GetCardNumber() == 1) {
            if (hand.at(i).GetCardSuit() == suitToFind) {
                handStats.at(1) = 14;
                break;
            }

            i++;
        }

        // Then set the index at the highest card
        i = hand.size() - 1;

        // Until 4 other cards have been found in decending order of the appropriate suit, keep looping
        while (grabbedCards < 4) {
            // Check if the current card being looked at is part of the handStats vector already, and thus should be ignored
            if (hand.at(i).GetCardNumber() != handStats.at(1) && hand.at(i).GetCardSuit() == suitToFind) {
                // If it isn't, add it to the handStats vector and increment the grabbedCards variable
                handStats.at(handStatsIndex) = hand.at(i).GetCardNumber();
                handStatsIndex++;
                grabbedCards++;
            }

            i--;
        }
    }

    return handStats.at(0);
}

/* If there is no flush, check for a straight (not flush)
   Following similar logic to the straight flush check,
   this looks at the current card and the card after. If those two cards are only 1 apart, 
   mark it as part of a straight
*/
int Player::CheckForStraight() {
    int numForStraight = 1;
    int highestCardIndex;
    int highestStraightCount = 0;

    for (size_t i = 0; i < hand.size() - 1; i++) {
        if (hand.at(i).GetCardNumber() + 1 != hand.at(i + 1).GetCardNumber() && hand.at(i).GetCardNumber() != hand.at(i + 1).GetCardNumber()) {
            numForStraight = 1;
        }
        else if (hand.at(i).GetCardNumber() + 1 == hand.at(i + 1).GetCardNumber()) {
            numForStraight++;
            if (numForStraight > highestStraightCount) {
                highestStraightCount = numForStraight;
                highestCardIndex = i + 1;
            }
        }
    }

    // Check for a straight that has ace high
    if (highestStraightCount >= 4 && hand.at(highestCardIndex).GetCardNumber() == 13 && hand.at(0).GetCardNumber() == 1) {
        handStats.at(0) = 5;
        handStats.at(1) = 14;
    }
    // If it is not a straight with ace high, but still a straight of 5 or more cards, mark the handStats vector appropriately
    else if (highestStraightCount >= 5) {
        handStats.at(0) = 5;
        handStats.at(1) = hand.at(highestCardIndex).GetCardNumber();
    }

    return handStats.at(0);
}

// If there is no straight present, check for a three-of-a-kind
int Player::CheckForThreeOfAKind() {
    // Use the LookForSets function to search for the highest 3-of-a-kind
    int trioIndex = LookForSets(3);

    // If a 3-of-a-kind was found, mark the handStats appropriately
    if (trioIndex) {
        handStats.at(0) = 4;
        handStats.at(1) = hand.at(trioIndex).GetCardNumber();
        // Then get the next two highest cards
        GrabCardsInOrder(2);
    }

    return handStats.at(0);
}

// If there is no 3-of-a-kind, check for 2 pair (2, 2, 5, 5)
int Player::CheckForTwoPair() {
    int firstPairIndex = 0;
    int secondPairIndex = 0;

    // Use the LookForSets function to find the first pair, and then a lower pair afterwards
    firstPairIndex = LookForSets(2);
    secondPairIndex = LookForSets(2, firstPairIndex);

    // If the two pair were found and one of those pair is an ace pair, mark it as high
    if ((firstPairIndex && secondPairIndex) && hand.at(secondPairIndex).GetCardNumber() == 1) {
        handStats.at(0) = 3;
        handStats.at(1) = 14;
        handStats.at(2) = hand.at(firstPairIndex).GetCardNumber();
        GrabCardsInOrder(1);
    }
    // Otherwise just use the usual higher pair first and then lower pair.
    else if (firstPairIndex && secondPairIndex) {
        handStats.at(0) = 3;
        handStats.at(1) = hand.at(firstPairIndex).GetCardNumber();
        handStats.at(2) = hand.at(secondPairIndex).GetCardNumber();
        GrabCardsInOrder(1);
    }

    return handStats.at(0);
}

// If two pair is not present, look for a pair
int Player::CheckForPair() {
    int pairIndex = 0;

    // Use the LookForSets function to find a pair
    pairIndex = LookForSets(2);

    // Again, if the pair is aces, mark them as 14, not 1, as they are high
    if (pairIndex && hand.at(pairIndex).GetCardNumber() == 1) {
        handStats.at(0) = 2;
        handStats.at(1) = 14;
        GrabCardsInOrder(3);
    }
    // Otherwise, just mark the handStats using the number as is
    else if (pairIndex) {
        handStats.at(0) = 2;
        handStats.at(1) = hand.at(pairIndex).GetCardNumber();
        GrabCardsInOrder(3);
    }

    return handStats.at(0);
}

// Finally, if nothing else is found, check for the highest cards
int Player::CheckForHighCard() {
    // Mark it as the lowest scoring hand and then get the top 5 cards
    handStats.at(0) = 1;
    GrabCardsInOrder(5);

    return handStats.at(0);
}

// This grabs the cards from highest to lowest and inputs them into 
// the handStats vector, making sure to ignore card numbers that are already present
void Player::GrabCardsInOrder(int numCards) {
    int grabbedCards = 0;
    int i = hand.size() - 1;
    int handStatsIndex = 1;
    
    // First check for a potential ace high card
    if (hand.at(0).GetCardNumber() == 1) {
        size_t m = 1;
        bool alreadyCounted = false;
        
        // Cycle through the handStats vector to check if numbers have been added previously
        // If not, and there is an ace present, mark the high card as 14
        while (handStats.at(m) != 0) {
            if (handStats.at(m) == 14) {
                bool alreadyCounted = true;
                break;
            }
            m++;
        }
        if (!alreadyCounted) {
            handStats.at(m) = 14;
            grabbedCards++;
        }
    }

    // While the needed amount of cards is still under the total numCards input into the function as what is needed
    while (grabbedCards < numCards) {
        // Check through each index of handStats, and compare against the hand card number
        for (size_t j = 1; j < handStats.size(); j++) {
            // If the value in the handStats vector at j is 0, then it can receive the next highest card
            if (handStats.at(j) == 0) {
                handStatsIndex = j;
                break;
            }

            // However, if the handStats vector at j equals the hand at i, then move to the next hand card
            // and check against the same handStats at j (just in case there is a pair in the hand that needs to be ignored)
            if (hand.at(i).GetCardNumber() == handStats.at(j)) {
                i--;
                j--;
            }
        }

        // Add the current card number to the handStats empty index value and increment (or decrement) everything appropriately
        handStats.at(handStatsIndex) = hand.at(i).GetCardNumber();
        grabbedCards++;
        i--;
    }
}

// Look for sets of various sizes
// The parameter cardAmount states how many cards need to be part of the set (pair (2), 3-of-a-kind (3), 4-of-a-kind (4))
// The parameter previousFind, is the index of a card that is part of an already found set and should be ignored (as it was already found)
int Player::LookForSets(int cardAmount, int previousFind) {
    // If the previousFind value exists (is less than 14), use it to look for cards less than the card at that index
    int totNumOfCard = 1;
    if (previousFind < 14) {
        previousFind = hand.at(previousFind).GetCardNumber();
    }

    // Go through each card in a double loop, checking for cards that are the same
    for (int i = hand.size() - 1; i >= cardAmount - 1; i--) {
        for (int j = i - 1; j >= 0; j--) {
            if (hand.at(i).GetCardNumber() == hand.at(j).GetCardNumber() && hand.at(j).GetCardNumber() < previousFind) {
                totNumOfCard++;
                // If the totNumOfCard reaches the cardAmount desired, return the index of that card
                if (totNumOfCard == cardAmount) {
                    return i;
                }
            }
        }
        // Each time the outer loop increments, start the count of matching cards over
        totNumOfCard = 1;
    }

    return 0;
}