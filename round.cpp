#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include "round.h"

// Initializer for a round, creating a deck and importing the vector of players
// The highest bet starts at 2 for the "big blind", and the currentDealer index gets 
// initialized to zero (subject to change later with the AssignDealer function)
Round::Round(std::vector<Player> &players) {
    this->players = players;
    highestBet = 2;
    currentDealer = 0;
    this->tableDeck = Deck();
}

// For each round, all the players are dealt 2 cards to start with 
// This function deals the 2 cards to each player
void Round::DealCards() {
    int handSize = 2;
    // Using a for loop, and cycling through the list of players twice,
    // this ensures that each player will be dealt 2 cards
    for (int i = 0; i < (players.size() * handSize); i++) {
        players.at(i % players.size()).TakeCard(tableDeck.DealCard());
    }
}

// Similar to the DealCards function, this just deals cards to the community hand 
// that everyone sees and can use
void Round::DealCommunityCards(int numCards) {
    // Before dealing the cards to the community, a card must be "burned".
    // This means that the card is removed from the game without being seen.
    // It helps with limiting a player's knowledge of cards to come
    tableDeck.DealCard();
    
    // Then the number of cards specified as the parameter are dealt to the community
    for (size_t i = 0; i < numCards; i++) {
        communityHand.push_back(tableDeck.DealCard());
    }
}

// A helper function to find the current dealer
int Round::FindDealer() {
    int i = 0;

    // While the current dealer isn't found (using the isDealer function), keep incrementing i,
    // and wrap around back to zero if i reaches the end of the loop
    while (!players.at(i).isDealer()) {
        i++;

        if (i % players.size() == 0) {
            i = 0;
        }
    }

    // Return the location of the dealer once it is found
    return i;
}

// A helper function used only when everyone but one person has folded
// It searches for the one player who has not been marked as having folded
// and returns that player's index
int Round::FindNotFolded() {
    for (int i = 0; i < players.size(); i++) {
        if (!players.at(i).GetFoldedStat()) {
            return i;
        }
    }
    return 0;
}

// This function takes the player that was the dealer, marks them as not the dealer
// and then marks the player after them (1 number higher) as the dealer
// This ensures everyone is the dealer at any given time
void Round::AssignDealer() {
    
    // Find previous round's dealer using the FindDealer helper function
    int previousDealer = FindDealer();

    // Unmark previous dealer and mark new dealer using the player's setter NewDealer
    for (int j = 0; j < 2; j++) {
        players.at((previousDealer + j) % players.size()).FlipDealerStat();
    }

    // Return the current round's dealer
    currentDealer = (previousDealer + 1) % players.size();
}

// Once the dealer is set as currentDealer, the player to the left (one number higher in the players vector)
// must ante 1 for the "small blind", and the player after that (one numbers higher)
// must ante 2 for the "big blind"
// For example: if the currentDealer index is 3, the "small blind" player index would be at 4 and would pay 1,
// and the "big blind" player index would be 5 and would pay 2
void Round::CollectAnte() {
    // This is done with a for loop because the "big blind" is one larger than the "small blind"
    // and the player index for the "big blind" is also one larger than the "small blind"
    for (int i = 0; i < 2; i++) {
        // The mod of players.size() ensures that the index wraps around to the start of the vector when necessary
        players.at((currentDealer + i + 1) % players.size()).Ante(i + 1);
    }
}

// This function receives player bets from input values by the human players
// and prints to the console appropriate text to help the player know necessary information for the game
void Round::PlaceBet(int roundNumber) {
    std::string choice = "";
    bool bettingRoundOver = false;
    int firstRoundOfBetting = 1;
    int playerIndex; 
    
    // To know whose bet it is, set the playerIndex
    if (communityHand.size()) {
        // If the community hand already has cards, then the person to the left of the dealer (index 1 higher) bets first
        playerIndex = (currentDealer + 1) % players.size();
    }
    else {
        // If this is the first round of betting for the hand, then the "big blind" has already bet,
        // and it is the turn of the player to the left of the big blind (index 1 higher)
        playerIndex = (currentDealer + 3) % players.size();
    }

    // While the bool value bettingRoundOver is false
    while (!bettingRoundOver) {
        // Keep the playerIndex values within possible indices of the players vector
        playerIndex = playerIndex % players.size();

        // If the player has folded, or if the player has gone all in, they are out of the betting. So just pass them over
        if (!players.at(playerIndex).GetFoldedStat() && !players.at(playerIndex).GetAllInStat()) {
            // If the highestBet is 2, the currentPlayer's highestBet is that 2 (the "big blind"), everyone has had a chance to bet, and it is the first round of betting
            if (highestBet == 2 && players.at(playerIndex).GetTotalBet() == highestBet && firstRoundOfBetting >= players.size() && roundNumber != -1) {
                // Use the standard PrintHandText with all the necessary information (explained below)
                PrintHandText(playerIndex, roundNumber);
                std::cout << players.at(playerIndex).GetName();
                std::cout << " still has the highest bet from the big blind. ";
                std::cout << players.at(playerIndex).GetName();
                std::cout << " can either (r)aise or (c)heck.\n";
                // Receive the player's input for the choice they decide
                getline(std::cin, choice);

                // Keep asking until they give one of the two viable options
                while (choice != "r" && choice != "c") {
                    std::cout << "That is not a valid choice, please enter again.\n";
                    std::cout << "Choose to (r)aise or (c)heck\n";

                    getline(std::cin, choice);
                }

                // If the player chooses to (c)heck, then the round ends
                if (choice == "c") {
                    bettingRoundOver = true;
                    break;
                }
                // Otherwise, the player raises using the player BetChips function
                else if (choice == "r") {
                    int raisedAmount = players.at(playerIndex).BetChips(highestBet);
                    // If the player's highestBet is larger than the previous set highestBet, mark it as the new highestBet
                    if (highestBet < players.at(playerIndex).GetTotalBet()) {
                        highestBet = players.at(playerIndex).GetTotalBet();
                    }
                    playerIndex++;
                    continue;
                }
            }
            // If the bet is larger than the "big blind" and everyone has had a chance to bet, the round ends
            else if ((players.at(playerIndex).GetTotalBet() == highestBet && firstRoundOfBetting > players.size())) {
                bettingRoundOver = true;
                break;
            }

            // Use the standard PrintHandText with all the necessary information (explained below)
            PrintHandText(playerIndex, roundNumber);
            std::cout << players.at(playerIndex).GetName();
            
            // For rounds of betting after the first (say after there are 3 community cards),
            // if the player has the matched highestBet, the option for them would be "check" instead of "call"
            if (players.at(playerIndex).GetTotalBet() == highestBet) {
                std::cout << " must decide to (r)aise, (c)heck or (f)old.\n";

                getline(std::cin, choice);

                while (choice != "r" && choice != "c" && choice != "f") {
                    std::cout << "That is not a valid choice, please enter again." << std::endl;
                    std::cout << "Choose to (r)aise, (c)heck or (f)old.\n";

                    getline(std::cin, choice);
                }
            }
            // Otherwise the currentPlayer has the option to "call" what the current highestBet is
            else {
                std::cout << " must decide to (r)aise, (c)all or (f)old.\n";

                getline(std::cin, choice);

                while (choice != "r" && choice != "c" && choice != "f") {
                    std::cout << "That is not a valid choice, please enter again." << std::endl;
                    std::cout << "Choose to (r)aise, (c)all or (f)old.\n";

                    getline(std::cin, choice);
                }
            }

            // If the player chooses to raise, then the BetChips function is called
            if (choice == "r") {
                int raisedAmount = players.at(playerIndex).BetChips(highestBet);
                // If it turns out the player cannot raise and instead chooses to fold,
                // the BetChips function will return 0
                if (raisedAmount == 0) {
                    players.at(playerIndex).SetFolded(true);
                }
                // Otherwise, if the player's highestBet is the new highestBet, 
                // reassign the highestBet to the new value
                else if (highestBet < players.at(playerIndex).GetTotalBet()) {
                    highestBet = players.at(playerIndex).GetTotalBet();
                }
            }
            // If the player selects "call", use the player's CallBet function
            else if (choice == "c") {
                players.at(playerIndex).CallBet(highestBet);
            }
            // If the player selects "fold", mark the player's folded stat to true
            else if (choice == "f") {
                players.at(playerIndex).SetFolded(true);
            }
        }
        // Check if either all the players but 1 have folded, or everyone is either all-in or has folded
        // If either is true, end the betting round as there is no one left to up the bet
        else if (CountFolded() == players.size() - 1 || CountAllInAndFolded() == players.size()) {
            bettingRoundOver = true;
        }

        // After interacting with each player (or not, if they folded or are all-in)
        // increment the playerIndex and increment the firstRoundOfBetting count
        playerIndex++;
        firstRoundOfBetting += 1;
    }
}

// The PrintHandText function prints to the console all necessary information
// for the player when it is their turn to bet
void Round::PrintHandText(int playerIndex, int roundNumber) {
    Player currentPlayer = players.at(playerIndex);
    std::string takeTurn = "";

    // Clears screen for each player so previous information is not seen
    std::cout << "\x1B[2J\x1B[H";
    // Explain whose turn it is next so the computer can be passed to the next person before cards are shown
    std::cout << "It is now " << players.at(playerIndex).GetName() << "'s turn. Press 'enter' to continue...\n";
    getline(std::cin, takeTurn);

    //Clears screen again so only pertinent information is present
    std::cout << "\x1B[2J\x1B[H";

    // If the communityHand vector is not empty, then print out the cards in the community hand
    if (communityHand.size() > 0) {
        PrintCommunityHand();
    }
    // Otherwise mark that it is the start of a new hand, so players know the previous hand has ended
    else if (roundNumber > 1) {
        std::cout << "Start of round " << roundNumber << std::endl;
    }
    
    // If there are players who have gone all-in, let the currentPlayer know
    if (CountAllIn() > 0) {
        PrintAllIns();
    }

    // If there are players who have folded, let the currentPlayer know
    if (CountFolded() > 0) {
        PrintFolded();
    }
    
    //Lists out all pertinent information for the current player
    std::cout << currentPlayer.GetName() << " currently has the ";
    currentPlayer.PrintHand();
    std::cout << "\nThe current highest a player has bet is " << highestBet << ", and ";
    std::cout << currentPlayer.GetName();
    std::cout << " has currently bet " << currentPlayer.GetTotalBet() << std::endl;
    std::cout << currentPlayer.GetName() << " has " << currentPlayer.GetChips() << " chips remaining.\n";
}

// A function for printing to the console the cards in the communityHand
void Round::PrintCommunityHand() {
    std::cout << "The community hand includes the ";

    // For each card in the hand, get the string version of the card ("Ace of spades" instead of {1, 3})
    // and print it to the console
    for (size_t i = 0; i < communityHand.size(); i++) {
        // If it is the last card in the hand, don't add any punctuation after the card string
        if (i == communityHand.size() - 1) {
            std::cout << communityHand.at(i).GetCardString();
        }
        // Otherwise if it is the second to last, print out a final "," and an "and" following the card string
        else if (i == communityHand.size() - 2) {
            std::cout << communityHand.at(i).GetCardString() << ", and ";
        }
        // Otherwise, just print out a ", " following the card string
        else {
            std::cout << communityHand.at(i).GetCardString() << ", ";
        }
    }

    std::cout << std::endl;
}

// Cycle through all the players and return how many have folded
int Round::CountFolded() {
    int numFolded = 0;

    for (int i = 0; i < players.size(); i++) {
        if (players.at(i).GetFoldedStat()) {
            numFolded++;
        }
    }

    return numFolded;
}

// Cycle through all the players and return how many are all-in
int Round::CountAllIn() {
    int numAllIn = 0;

    for (int i = 0; i < players.size(); i++) {
        if (players.at(i).GetAllInStat())
        {
            numAllIn++;
        }
    }

    return numAllIn;
}

// Cycle through the players and see how many have either gone all-in or folded
int Round::CountAllInAndFolded() {
    int numFoldedOrAllIn = 0;

    for (int i = 0; i < players.size(); i++) {
        if (players.at(i).GetFoldedStat() || players.at(i).GetAllInStat()) {
            numFoldedOrAllIn++;
        }
    }

    return numFoldedOrAllIn;
}

// Function to print to the console all the players who have gone all-in
void Round::PrintAllIns() {
    int numAllIns = 0;

    for(int i = 0; i < players.size(); i++) {
        if (players.at(i).GetAllInStat()) {  
            // If this is the first player found going all-in, do not add punctuation
            if (numAllIns == 0) {
                std::cout << players.at(i).GetName();
                numAllIns++;
            }
            // Otherwise, place a ", " in front of the next name found
            else {
                std::cout << ", " << players.at(i).GetName();
                numAllIns++;
            }
        }
    }

    // If there is only one player who went all-in, use the correct grammar
    if (numAllIns == 1) {
        std::cout << " is all-in\n";
    }
    // Otherwise change to acknowledge that several people have gone all-in
    else {
        std::cout << " are all-in\n";
    }
}

// A function similar to PrintAllIns, except with players who have folded
void Round::PrintFolded() {
    int numFolded = 0;

    for (int i = 0; i < players.size(); i++) {
        if (players.at(i).GetFoldedStat()) {
            if (numFolded == 0) {
                std::cout << players.at(i).GetName();
                numFolded++;
            }
            else {
                std::cout << ", " << players.at(i).GetName();
                numFolded++;
            }
        }
    }

    if (numFolded == 1) {
        std::cout << " has folded\n";
    }
    else {
        std::cout << " have folded\n";
    }
}

// For each player who has not folded, use the EvaluateCards function to create a
// handStats vector determining the value of the hand
void Round::ScoreHands() {
    for (size_t i = 0; i < players.size(); i++) {
        if (!players.at(i).GetFoldedStat()) {
            players.at(i).EvaluateCards(communityHand);
        }
    }
}

// The function that, after determining the winner, divides up the pot accordingly to winners, 
// and resets (or deletes) players as necessary
std::vector<Player> Round::DivvyPots(int roundNumber, bool winnerByFolding) {
    // If the winner has not already been determined because everyone else folded
    if (!winnerByFolding) {    
        // Create a vector of pairs that contain the winner's index in the players vector,
        // and whether that player has tied someone else for the value of their handStats
        std::vector<std::pair<int, int>> winnerIndexAndTies;

        // For each player that has not folded, order the players' handStats vectors from highest to lowest
        // using the recursive OrderPlayerHands stat, with the number of times the recursive function has been called set to 0
        for (size_t i = 0; i < players.size(); i++) {
            if (!players.at(i).GetFoldedStat()) {
                int recursiveCount = 0;
                OrderPlayerHands(winnerIndexAndTies, i, recursiveCount);
            }
        }

        // Once ordered, go through the players in the order of winnerIndexAndTies
        for (int m = 0; m < winnerIndexAndTies.size(); m++) {
            // If the player at the current index has some totalBet remaining 
            // (has not all been removed from previous loops of this function)
            if (players.at(winnerIndexAndTies.at(m).first).GetTotalBet() > 0) {
                // If the player did not tie with anyone else, then subtract that player's totalBet
                // from all remaining players with totalBet values equal to or less than the player's totalBet
                if (winnerIndexAndTies.at(m).second == 0) { 
                    AddBetsToWinner(m, winnerIndexAndTies);
                }
                // If there are ties between winners
                else {
                    // Create a vector of pairs to track all the players who tied and what their totalBet was
                    std::vector<std::pair<int, int>> tiesAndTotalBet;
                    int j = m;

                    // While the tie number (unique to each tied group) is the same as the first player being checked
                    while (winnerIndexAndTies.at(j).second == winnerIndexAndTies.at(m).second) {
                        // Add the pair {playerIndex, totalBet} to the tiesAndTotalBet vector
                        std::pair<int, int> tempHolder = {winnerIndexAndTies.at(j).first, players.at(winnerIndexAndTies.at(j).first).GetTotalBet()}; 
                        tiesAndTotalBet.push_back(tempHolder);
                        j++;
                    }

                    // Then order this group of tied players from smallest totalBet to greatest
                    tiesAndTotalBet = OrderTiedPlayers(tiesAndTotalBet);

                    // While there are still players in the tiesAndTotalBet vector
                    while (tiesAndTotalBet.size()) {
                        // If the player still has some totalBet remaining
                        if (players.at(tiesAndTotalBet.at(0).first).GetTotalBet() != 0) {
                            // Cycle through all the players with totalBet remaining, 
                            // and add either the winner's totalBet, or the currentPlayer's totalBet, whichever is smaller, to the totalInPot
                            int totalInPot = 0;
                            for (int l = 0; l < players.size(); l++) {
                                if (players.at(l).GetTotalBet() != 0) {    
                                    if (players.at(l).GetTotalBet() <= players.at(tiesAndTotalBet.at(0).first).GetTotalBet()) {
                                        totalInPot += players.at(l).GetTotalBet();
                                    }
                                    else {
                                        totalInPot += players.at(tiesAndTotalBet.at(0).first).GetTotalBet();
                                    }
                                }
                            }
                            // Calculate what fraction of that pot goes to the current winner given how many are tied
                            int currentWonAmount = totalInPot / tiesAndTotalBet.size();
                            // If the chips do not divide evenly among the tied winners, give the currentPlayer 1 extra chip
                            if (totalInPot % tiesAndTotalBet.size() != 0) {
                                AddBetsToWinner(tiesAndTotalBet.at(0).first, winnerIndexAndTies, currentWonAmount + 1);
                            }
                            // Otherwise just give the divided amount to the player, and subtract it from all the other players
                            else {
                                AddBetsToWinner(tiesAndTotalBet.at(0).first, winnerIndexAndTies, currentWonAmount);
                            }
                        }
                        // After this player has received their portion of the pot, delete them from the tiesAndTotalBet vector
                        tiesAndTotalBet.erase(tiesAndTotalBet.begin());
                        // Increment the m value tracking what player has been paid in the winnerIndexAndTies vector
                        m++;
                    }
                    m--;
                }
            }
        }

        // Then export the results to a txt file for later review
        ExportStatsToFile(winnerIndexAndTies, roundNumber);

        //Clears screen for each player so previous information is not seen
        std::cout << "\x1B[2J\x1B[H";

        // After exporting the information to the txt file, print to the console who won
        // And include the type of winning hand the player won with (flush, straight, full house, etc.)
        for (int i = 0; i < winnerIndexAndTies.size(); i++) {
            if (i == 0 && winnerIndexAndTies.at(i).second == 0) {
                std::cout << players.at(winnerIndexAndTies.at(i).first).GetName() << " won with " << players.at(winnerIndexAndTies.at(i).first).GetBestHand();
            }
            else if (winnerIndexAndTies.at(i).second != 0 && winnerIndexAndTies.at(i).second == winnerIndexAndTies.at(0).second) {
                std::cout << players.at(winnerIndexAndTies.at(i).first).GetName() << " tied with " << players.at(winnerIndexAndTies.at(i).first).GetBestHand();
            }
            else {
                std::cout << players.at(winnerIndexAndTies.at(i).first).GetName() << " had " << players.at(winnerIndexAndTies.at(i).first).GetBestHand();
            }

            std::cout << std::endl;
        }

        // Then add a pause for everyone to see and review the results before starting another hand
        std::string pause = "";
        std::cout << "Press 'enter' to continue...\n";
        getline(std::cin, pause);
    }   
    // If, however, the player won because everyone else folded
    else {
        // Just find that winning player
        // subtract the winner's totalBet from everyone else
        // and export to the txt file that the winner won because everyone else folded
        int winnerIndex = FindNotFolded();
        AddBetsToWinnerFolded(winnerIndex);
        ExportWinnerFolded(roundNumber);
    }

    // Then check if any players are out of the game because they have no chips left
    for (int i = 0; i < players.size(); i++) {
        if (players.at(i).GetChips() == 0) {
            // If they are, erase them from the players vector
            players.erase(players.begin() + i);
            i--;
        }
        // Otherwise make sure their folded stat gets reset to false and their cards and cardStats are cleared
        else {
            if (players.at(i).GetFoldedStat()) {
                players.at(i).SetFolded(false);
            }
            players.at(i).EmptyHand();
            players.at(i).EmptyHandStats();
        }
    }

    // Then return the revised players vector
    return players;
}

// Take in a vector of pairs containing a player's totalBet as the second element,
// and order the pairs in increasing order based on that totalBet value, then return the ordered vector
std::vector<std::pair<int, int>> Round::OrderTiedPlayers(std::vector<std::pair<int, int>> tiesAndTotalBet) {
    // Create new vector to place the elements in order
    std::vector<std::pair<int, int>> newOrderedTies = {tiesAndTotalBet.at(0)};

    // Cycle through the elements in the original vector, tiesAndTotalBet
    for (int i = 1; i < tiesAndTotalBet.size(); i++) {
        // Then compare its elements to those in the new vector, newOrderedTies
        for (int j = 0; j < newOrderedTies.size(); j++) {
            // If the totalBet is smaller than the current element
            if (tiesAndTotalBet.at(i).second < newOrderedTies.at(j).second) {
                // Then insert the tiesAndTotalBet element into the newOrderedTies vector and break the loop
                newOrderedTies.insert(newOrderedTies.begin() + j, tiesAndTotalBet.at(i));
                break;
            }
            // If the loop has reached the end of newOrderedTies with no larger totalBets found
            else if (j == newOrderedTies.size() - 1) {
                // Just insert the new element at the end of newOrderedTies
                newOrderedTies.push_back(tiesAndTotalBet.at(i));
            }
        }
    }

    // Return the newly ordered vector
    return newOrderedTies;
}

// A function that takes in the index of the winnerIndexAndTies vector of the winning player
// and includes a winnerShare if this player tied and was dividing up their winnings
// It then subtracts the specified amount (or the winner's totalBet) from every player's totalBet greater than 0
// and adds that amount to the winning player
void Round::AddBetsToWinner(int winnerIndex, std::vector<std::pair<int, int>> &winnerIndexAndTies, int winnerShare) {
    // If a divided up share is not specified
    if (!winnerShare) {
        // winnerShare becomes the totalBet of the winner
        winnerShare = players.at(winnerIndexAndTies.at(winnerIndex).first).GetTotalBet();
    }

    // It then loops through every player
    for (size_t i = 0; i < players.size(); i++) {
        // If the player has a totalBet greater than zero remaining
        if (players.at(i).GetTotalBet() > 0) {
            // Add the winnerShare or the losing player's totalBet, whichever is smaller, to the winner's chips
            players.at(winnerIndexAndTies.at(winnerIndex).first).TakeWinnings(players.at(i).GetTotalBet(), winnerShare);
            // Then subtract that same value from the loser's totalBet, tracking what is remaining to lose, if anything
            players.at(i).SubtractTotalBet(winnerShare);
        }
    }
}

// If a player won by everyone else folding, and no winnerIndexAndTies was created
// then this function takes in the index of the player vector who won and adds the winnings
// to that player's chips and subtracts from every other player's totalBet
void Round::AddBetsToWinnerFolded(int winnerIndex) {
    int winningAmount = players.at(winnerIndex).GetTotalBet();
    
    // Loop through every player
    for (size_t i = 0; i < players.size(); i++) {
        // If the player had made a totalBet, then subtract that amount from the loser's totalBet 
        // and add it to the winner's chips
        if (players.at(i).GetTotalBet() > 0) {
            players.at(winnerIndex).TakeWinnings(players.at(i).GetTotalBet(), winningAmount);
            players.at(i).SubtractTotalBet(winningAmount);
        }
    }
}

// This function takes in the vector of pairs, winnerIndexAndTies, the index i tracking the current player index in the players vector,
// and the count of how many times this function has been called recursively
// It then orders the currentPlayer (at index i) in the winnerIndexAndTies vector, in decreasing order,
// based on the size of the values in the handStats vector
void Round::OrderPlayerHands(std::vector<std::pair<int, int>> &winnerIndexAndTies, int i, int recursiveCount) {
    // If the recursiveCount gets to then end of the winnerIndexAndTies vector without 
    // successfully finding a smaller handStats vector
    if (recursiveCount == winnerIndexAndTies.size()) {
        // Then it gets placed at the end of the winnerIndexAndTies vector
        winnerIndexAndTies.push_back(std::make_pair(i, 0));
        return;
    }
    
    int bestHandIndex = winnerIndexAndTies.at(0 + recursiveCount).first;

    // If the currentPlayer's handStats vector is larger than the handStats of the player at
    // the current index of winnerIndexAndTies
    if (players.at(i).GetHandStats() > players.at(bestHandIndex).GetHandStats()) {
        // Insert the currentPlayer's index as the pair's first value, and 0 as the second value,
        // because no tie was found
        winnerIndexAndTies.insert(winnerIndexAndTies.begin() + recursiveCount, std::make_pair(i, 0));
    }
    // If the currentPlayer's handStats tied the current winnerIndexAndTies handStats
    else if (players.at(i).GetHandStats() == players.at(bestHandIndex).GetHandStats()) {
        // Then check if the winnerIndexAndTies player already has tied someone else
        int nextBestTieCount = winnerIndexAndTies.at(recursiveCount).second; 
        
        // If that player had already tied someone else
        if (nextBestTieCount > 0) {
            // Then insert the new player index into winnerIndexAndTies, and use the tied value provided from the winnerIndexAndTies player
            winnerIndexAndTies.insert(winnerIndexAndTies.begin() + recursiveCount, std::make_pair(i, nextBestTieCount));
        }
        // Otherwise, insert the new player index, but use the recursiveCount + 1 as the second value for both tied pairs in winnerIndexAndTies
        else {
            winnerIndexAndTies.insert(winnerIndexAndTies.begin() + recursiveCount, std::make_pair(i, 1 + recursiveCount));
            winnerIndexAndTies.at(1 + recursiveCount).second = 1 + recursiveCount;
        }

        // Then go to the index just past the newly tied players
        int j = 2 + recursiveCount;
        // Track how many players tied with those two and keep incrementing the index until it gets to a player not tied with the inserted player
        while (j < winnerIndexAndTies.size() && winnerIndexAndTies.at(j).second != winnerIndexAndTies.at(recursiveCount).second) {
            j++;
        }

        // Then increment the second value in the pair (the tied value) for every tied group after the inserted tied group
        // This will ensure that no two different tied groups will have the same tied value
        for (j; j < winnerIndexAndTies.size(); j++) {
            if (winnerIndexAndTies.at(j).second >= 1) {
                winnerIndexAndTies.at(j).second++;
            }
        }
    }
    // If the handStats of the currentPlayer is smaller, then call OrderPlayerHands and increment the recursiveCount
    else {
        OrderPlayerHands(winnerIndexAndTies, i, recursiveCount + 1);
    }

    return;
}

// This function exports to a txt file the name of the winning player and the round number they won
int Round::ExportWinnerFolded(int roundNumber) {
    std::ofstream dataFile;

    // Open the file for appending and check itt make sure it opened successfully
    dataFile.open("game_stats.txt", std::ios_base::app);

    if (!dataFile.is_open()) {
        std::cout << "Could not open file: 'game_stats.txt'\n";

        return 1;
    }

    Player winner = players.at(FindNotFolded());

    // Then write to the opened file the information of who won, the round number, and the status of winning by folding
    dataFile << winner.GetName() << " won round " << roundNumber << " because everyone else folded.";

    dataFile << std::endl;

    // Then close the file
    dataFile.close();

    return 0;
}

// This function exports to a txt file the winner, the handStats information as a string,
// and the hand itself, to review to make sure the analysis was accurate
int Round::ExportStatsToFile(std::vector<std::pair<int, int>> &winnerIndexAndTies, int roundNumber) {
    std::ofstream dataFile;

    // Open the file for appending and check itt make sure it opened successfully
    dataFile.open("game_stats.txt", std::ios_base::app);

    if (!dataFile.is_open()) {
        std::cout << "Could not open file: 'game_stats.txt'\n";

        return 1;
    }

    Player winner = players.at(winnerIndexAndTies.at(0).first);
    
    // Then export the player's name, the round they won, the best winning hand according to the hand checks,
    // and then loops through each card in the hand and exports the string of each card
    dataFile << winner.GetName() << " won round " << roundNumber << " with ";
    dataFile << winner.GetBestHand() << ", using the cards: ";
    std::vector<Card> winnerHand = winner.GetHand();
    for (int i = 0; i < winnerHand.size(); i++) {
        dataFile << winnerHand.at(i).GetCardString();
        // Include punctuation as necessary
        if (i == winnerHand.size() - 2) {
            dataFile << ", and ";
        }
        else if (i != winnerHand.size() - 1) {
            dataFile << ", ";
        }
    }
    dataFile << std::endl;

    // Then close the file
    dataFile.close();

    return 0;
}