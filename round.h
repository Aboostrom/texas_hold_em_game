#ifndef ROUND_H
#define ROUND_H

#include "player.h"
#include "deck.h"

// Class constructed to handle all the events within a game's individual round
class Round {
    public:
        /* Initializer function, requiring a parameter of a vector of players that serves as the round's players.
           When a Round class is created, it populates a new Deck class consisting of a series of 52 Cards.
           The Round also initializes the highestBet to be 2 for the big blind, and a currentDealer variable starting at 0
        */
        Round(std::vector<Player> &players);

        /* This method uses the member players vector to loop through each player
           and deal the specified amount of cards. It is called to deal player cards
           at the beginning of each game round. the method increase the player's hand vector,
           but does not change the Round class
        */
        void DealCards();
        /* This method takes in a parameter of the number of cards to be dealt. It
           places the dealt cards within the member communityHand vector, increasing that vector's size. It is called
           when the community hand is supposed to receive more cards as part of the game's structure
        */
        void DealCommunityCards(int numCards);
        /* This method searches through the member player vector to find the current player
           with the boolean value set to true for that player's member variable "dealer".
           The method then returns the index of that player in the player vector. It is called when
           the dealer index needs to be known
        */
        int FindDealer();
        /* This method searches through the member player vector to find the only player 
           whose member variable "folded" is not set to true. Once found, the method returns 
           the index of that player.
           This function is ONLY called when every player but 1 has folded, and that remaining player's index is needed to be known.
        */
        int FindNotFolded();
        /* This method calls the FindDealer method to get the index of the current dealer.
           Upon finding the current dealer, this method changes the player's member variable "dealer" to false
           and goes to the next player in the player vector (or loops to the first player if the end of the vector has been reached),
           changing that player's member variable "dealer" to true. After determining the index in the member players vector
           of the new dealer, this method assigns that index to the member variable "currentDealer". 
           This method is called at the beginning of each new round to assign a new dealer for each hand dealt in the game.
        */
        void AssignDealer();
        /* This method is called immediately after AssignDealer and uses the populated member variable "currentDealer" to determine the
           player "to the left" (one higher in the player vector), and assigns that player as owing the small blind. The method then calls 
           the player member function Ante to subtract the appropriate amount from that player's member variable chips and adds that to the 
           player's member variable totalBet. The method then does this with the player to the left of the small blind, who is assigned as 
           owing the big blind and the same functions are called but just for the different player and the higher ante amount.
        */
        void CollectAnte();
        /* This method takes in a roundNumber as a parameter if the community hand has not been dealt any cards yet. Otherwise, the parameter 
           is assigned as -1 to be ignored by the method. The method then interacts with every user in the player vector, 
           asking if the user wishes to raise, call, or fold. Depending on the user's response, either the player's member functions
           BetChips or CallBet are called, or the player's member variable "folded" is set to true, respectively. If chips are bet, the player
           has those chips subtracted from their member variable chips and their totalBet variable is increased by the same amount. This process of
           looping through players in the players vector continues until an entire loop goes through without anyone increasing the bet, or until everyone 
           but 1 person folds, or until everyone has either folded or gone all-in.
           This method is called after every increasing of cards, regardless of what vector receives the cards. This method is only skipped if there is
           no available money left to bet because everyone has gone all-in.
        */
        void PlaceBet(int roundNumber=-1);
        /* This method loops through all players in the member players vector who do not have the player variable "folded" set to true.
           For each player, the method calls the player member function EvaluateCards to alter the player's handStats vector,
           with the first int value in the vector representing the type of best winning hand (straight, flush, pair, etc.), and the remaining
           5 int values being the 5 highest cards (for example, with the full house, the second value 
           in the handStats vector would be the three-of-a-kind card number, and the third value in the handStats vector would be the pair card
           number. All other values in the handStats vector would be 0 as the other 3 card numbers are repeats of what already is listed). ScoreHands 
           is called after all betting is complete and only if there is more than one player who has not folded.
        */
        void ScoreHands();
        /* This method takes in the game's roundNumber and boolean value that is assigned true if a player won a round because everyone
           else folded. Upon being called, DivvyPots takes one of 2 paths, either organizing the players from best hand to worst hand using the player's
           handStats vector populated by the ScoreHands method, or begins adding the pot's winnings to the player who remained after everyone else
           folded. If ScoreHands is called, then the hands are ordered by the OrderPlayerHands method, properly handling ties by doing a second ordering,
           from least bet to most bet with the OrderTiedPlayers method. After the ordering is complete, AddBetsToWinners loops through everyone in order,
           so that everyone who should receive money from the pot, does, resetting the player variable totalBet in the process. DivvyPots then clears every
           player's hands and resets all player folded variables so that no player is marked as being folded. Finally DivvyPots deletes any players
           that at the end of the round have no chips. DivvyPots returns the updated player vector to be used for future rounds as necessary. DivvyPots is
           called at the end of each round as a way of summing up the events of each round.
        */
        std::vector<Player> DivvyPots(int roundNumber, bool winnerByFolding = false);
        /* This method loops through the member players vector and tracks the number of players that either
           have 0 chips left (because they are all-in), or have their member folded variable marked as true. If either of these
           statements is true, the method counts that player. The method returns the resultant counted total. CountAllInAndFolded is called
           before betting within each round to ensure there is a need to have betting. If all players have either gone all-in or folded,
           no more betting is required and should be skipped. Nothing is changed in calling this method.
        */
        int CountAllInAndFolded();
        /* This method loops through the member players vector and counts the number of players that have thier folded
           variable marked as true. CountFolded returns the total count, not having changed any values in the process.
           CountFolded is called after a round of betting to check if all players except 1 have folded. If this is the case,
           the round ends without dealing the rest of the cards or completing the round's betting. CountFolded is also called
           during the PrintHandText function to aid in listing all players who have folded.
        */
        int CountFolded();
        /* This method loops through the member players vector and counts the number of players that have 0 chips left
           because they are all-in. CountAllIn returns the total count, not having changed any values in the process.
           CountAllIn is called during the PrintHandText function to aid in listing all players who are all in, and during the betting, 
           and ALWAYS before the player's totalBet has been reassigned. That way there is never confusion between a player who 
           is all-in and a player who has lost all their chips.
        */
        int CountAllIn();

        /* This method prints to the console all pertinent information a player (at the parameter's playerIndex value) might need when making a bet during a round.
           This information includes a list of all players who have folded, using the PrintFolded function, all players who are all-in,
           using the PrintAllIns function, what the highestBet currently is, using the Round's member variable, how many chips the current player
           has, using the Player member function GetTotalBet, the parameter roundNumber (if it is greater than 1), and the player's cards as well as the community cards, using
           the Card's GetCardString function. PrintHandText is called before every player makes a bet. No values are changed during this process.
        */
        void PrintHandText(int playerIndex, int roundNumber);
        /* This method specializes in printing to the console the cards in the member communityHand vector and is called only within the PrintHandText function.
           It does not change any values in the process and only calls the card's GetCardString function.
        */
        void PrintCommunityHand();
        /* This method is called only during the PrintHandText function and only if there are AllIns (determined by the
           CountAllIn function). No values are changed when calling this function and player names are just printed
           to the console if that player is all-in.
        */
        void PrintAllIns();
        /* This method is called only during the PrintHandText function and only if there are folded players (determined by the
           CountFolded function). No values are changed when calling this function and player names are just printed
           to the console if that player has folded.
        */
        void PrintFolded(); 
        /* This recursive method is called when ordering the hands of all players who have not folded. After the player at index 0
           is added to winnerIndexAndTies (a parameter of this method, consisting of a vector of pairs), each successive player (looping through all the players) 
           compares their handStats vector to the first player in the winnerIndexAndTies vector. If the winnerIndexAndTies player's
           handStats vector is better (higher int values), then OrderPlayerHands is called recursively (with the recursiveCount being tracked), until it finds a player that 
           has either a lower or equally high handStats vector. If OrderPlayerHands reaches the end of the winnerIndexAndTies vector 
           finding no hand lower, then the currentPlayer's index and tied value pair is inserted at the end of the winnerIndexAndTies vector.
           This process continues until all players who have not folded are present in the winnerIndexAndTies vector, which is updated
           by reference for future use in the DivvyPots method, from which this method is called.
        */
        void OrderPlayerHands(std::vector<std::pair<int, int>> &winnerIndexAndTies, int i, int recursiveCount);
        /* This method is called only if there are tied players in the winnerIndexAndTies vector, meaning the second
           value in the winnerIndexAndTies vector pair is greater than 0. If that is the case, AND these players had a 
           good enough hand to share some of the pot, then this method takes the parameter tiesAndTotalBet vector and re-orders
           it by ranking the totalBet (the second value in the pair) from smallest to to largest. Once the newOrderedTies vector
           has all the elements of tiesAndTotalBet, OrderTiedPlayers returns the new vector to replace the previous vector that was 
           unordered. OrderTiedPlayers is only called in DivvyPots and only for the specific purpose of sorting a group
           of tied players. 
        */
        std::vector<std::pair<int, int>> OrderTiedPlayers(std::vector<std::pair<int, int>> tiesAndTotalBet);
        /* This method takes in the current winner's index in the winnerIndexAndTies vector, the winnerIndexAndTies
           vector itself, and, if the winner tied with others, the winnerShare int value. The method then loops through all the players
           in the players vector. If a player has a totalBet greater than 0, the winner adds to their member chips either the loser's totalBet 
           or the winner's totalBet, whichever is smaller, and then subtracts that same amount from the loser's totalBet. 
           This method is called within the DivvyPots method and as many times as necessary, until all players have a totalBet of 0. 
        */
        void AddBetsToWinner(int winnerIndex, std::vector<std::pair<int, int>> &winnerIndexAndTies, int winnerShare=0);
        /* Similar to AddBetsToWinner, this function specializes in the situation where all players have folded except one.
           In this case, the method loops through all the players who have a totalBet greater than zero. All totalBets from losers
           get added to the winner of the round (dictated by the index parameter associated with the players vector) as only that 
           player will get any chips. And then the losing player's totalBet returns to 0. Again, this function is only called during 
           DivvyPots after gameplay has finished for a game round.
        */
        void AddBetsToWinnerFolded(int winnerIndex);
        /* This method exports the winner's name, roundNumber, and the hands of all players who did not fold (as listed by the 
           winnerIndexAndTies vector parameter) to a txt file for tracking the results of each round of the game. This method
           is called at the end of each round unless ExportWinnerFolded is called instead, meaning hands are not shown at the end of the round.
           No variables are changed in exporting this information to the txt file.
        */
        int ExportStatsToFile(std::vector<std::pair<int, int>> &winnerIndexAndTies, int roundNumber);
        /* This method exports the winner's name and that they won because everyone else folded to a txt file for tracking 
           the results of each round of the game. This method is called at the end of each round unless ExportStatsToFile 
           is called instead, meaning hands hand information is available and multiple players did not fold. No variables 
           are changed in exporting this information to the txt file.
        */
        int ExportWinnerFolded(int roundNumber);

    private: 
        int highestBet;
        std::vector<Player> players;
        Deck tableDeck;
        int currentDealer;
        std::vector<Card> communityHand;

};

#endif