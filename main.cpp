#include <iostream>
#include <fstream>

#include "player.h"
#include "deck.h"
#include "round.h"

// Function to take in an empty players vector, and populate it with a user-specified
// number of players, each given a unique name. 
void SetupTable(std::vector<Player> &players);
// A function to return if there is still no winner based on the size of the players vector
bool noWinner(std::vector<Player> &players);
// A function that takes in the current round of the game (based on number of hands played)
// and the players vector, and runs through an entire round of gameplay, returning the altered players vector accordingly
std::vector<Player> PlayRound(std::vector<Player> &players, int roundNumber);
// A function that takes in the players vector and exports the results of the finished game to a txt file
int ExportWinnerInfo(std::vector<Player> &players);

int main() {
    std::vector<Player> players;
    int roundNumber = 0;
    
    // Set up the table with a user specified number of players and has the user give each
    // player a unique name
    SetupTable(players);

    // While there is more than 1 player in the players vector
    while (noWinner(players)) {
        // Increment the start of a new round and then run the PlayRound function
        roundNumber++;
        players = PlayRound(players, roundNumber);
    }

    // Upon completion of the game, export the results to a txt file
    ExportWinnerInfo(players);
    // Then print that the game ended to the console
    std::cout << "Game Over!\n";

    return 0;
}

// Function to take in an empty players vector, and populate it with a user-specified
// number of players, each given a unique name.
void SetupTable(std::vector<Player> &players) {
    std::string numPlayers;
    int chips;

    // Ask for the number of players, capping the number at 10 for deck-size reasons
    std::cout << "Enter the number of players (max 10): ";
    getline(std::cin, numPlayers);
    
    // While the answer is not within appropriate parameters (a number between 2 and 10)
    while (!isdigit(numPlayers[0]) || (stoi(numPlayers) > 10 || stoi(numPlayers) < 2)) {
        // Keep asking until an acceptable answer is provided
        std::cout << "Enter a number of players between 2 and 10: ";
        getline(std::cin, numPlayers);
    }

    int intNumPlayers = stoi(numPlayers);

    // Once the number of players is decided, ask for names of each player
    for (int i = 0; i < intNumPlayers; i++) {
        std::string name;

        std::cout << "Enter the name of Player " << i + 1 << ": ";
        std::cin >> name;

        // Then create a new player with that name and insert it into the players vector
        Player newPlayer = Player(name);
        players.push_back(newPlayer);
    }

    // The last player inserted will be marked as the dealer so that when the dealer changes
    // at the beginning of the first round, the new dealer will become the player at the 0 index
    players.at(players.size() - 1).FlipDealerStat();
}

// Return a boolean value for whether the size of the players vector is greater than 1
bool noWinner(std::vector<Player> &players) {
    return players.size() > 1;
}

// A function that takes in the current round of the game (based on number of hands played)
// and the players vector, and runs through an entire round of gameplay, returning the altered players vector accordingly
std::vector<Player> PlayRound(std::vector<Player> &players, int roundNumber) {
    Round currentRound = Round(players);
    std::vector<int> cardAmounts = {3, 1, 1};
    bool winnerByFolding = false;

    // If it is the first round played, due to user input, clear the cin
    if (roundNumber == 1) {
        std::cin.ignore();
    }
    
    // Then call the AssignDealer function, changing the dealer to the "next" player
    // which is 1 higher in the players vector, going back to 0 once the end of the vector is reached
    currentRound.AssignDealer();
    // Then using the dealer's placement, collect the small and big blinds from the players to the "left"
    // or 1 and 2 higher in the players vector, respectively
    currentRound.CollectAnte();
    // Then deal cards to every player in the players vector
    currentRound.DealCards();
    // Then run through a betting phase, requiring user input and altering the totalBet and chip total of all players 
    // who have not folded
    currentRound.PlaceBet(roundNumber);
    // Then deal cards to the community hand and follow it with betting until all the community cards are dealt
    // or everyone but one player has folded
    for (size_t i = 0; i < cardAmounts.size(); i++) {
        // If everyone but 1 player has folded, mark winnerByFolding as true
        // and break out of the loop as no more cards are needed to be dealt
        if (currentRound.CountFolded() == players.size() - 1) {
            winnerByFolding = true;
            break;
        }

        currentRound.DealCommunityCards(cardAmounts.at(i));

        // If everyone has NOT either gone all in or folded, then run another betting round,
        // Otherwise skip this step and deal the rest of the cards
        if (currentRound.CountAllInAndFolded() < players.size() - 1) {
            currentRound.PlaceBet();
        }
        // If everyone but 1 player has folded, mark winnerByFolding as true
        // and break out of the loop as no more cards are needed to be dealt
        if (currentRound.CountFolded() == players.size() - 1) {
            winnerByFolding = true;
            break;
        }
    }
    // If more than one player has not folded
    if (!winnerByFolding) {
        // Then calculate handStats of each player's hand who is still currently not folded
        currentRound.ScoreHands();
    }
    
    // Then use those calculated stats, or the winnerByFolding boolean, to divide up the winnings of the hand appropriately
    // DivvyPots also resets the players' hands and removes players who have run out of chips, returning the resultant players vector
    return currentRound.DivvyPots(roundNumber, winnerByFolding);
}

// A function that takes in the players vector and exports the results of the finished game to a txt file
int ExportWinnerInfo(std::vector<Player> &players) {
    std::ofstream dataFile;

    // Open the file for appending and check itt make sure it opened successfully
    dataFile.open("game_stats.txt", std::ios_base::app);

    if (!dataFile.is_open()) {
        std::cout << "Could not open file: 'game_stats.txt'\n";

        return 1;
    }

    Player winner = players.at(0);

    // Then export the name of the winner and that they won the game
    // and export the amount the winning player managed to win over the course of the game
    dataFile << winner.GetName() << " won won the game!\n";
    dataFile << winner.GetName() << " managed to win a total of " << winner.GetChips() << " chips. Congratulations!";
    dataFile << std::endl;

    // Finally, close the file when exporting is complete
    dataFile.close();

    return 0;
}