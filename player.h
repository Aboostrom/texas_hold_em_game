#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

// Class created for human players, managing all the setters, getters, 
// and helper functions relating to each player
class Player {
    public:
        // Basic initializers, getters, setters, and resetters(emptying hand, etc.)
        Player(std::string name = "none");
        void PrintHandStats(); 
        int GetChips();
        std::string GetName();
        std::vector<Card> GetHand();
        std::vector<int> GetHandStats(); 
        int GetTotalBet();
        bool GetFoldedStat();
        bool GetAllInStat();
        std::string GetBestHand();
        std::string StringifyCardNumber(int card); 
        void SetFolded(bool value);
        void TakeWinnings(int playerChips, int winnerShare);
        void SubtractTotalBet(int lostAmount);
        bool isDealer();

        // Interaction functions used during main game play
        void TakeCard(Card dealtCard);
        void SortHand(Card dealtCard);
        int BetChips(int highestBet);
        void CallBet(int highestBet);
        void WinOrLoseHand(int chips);
        void EmptyHand();
        void EmptyHandStats();
        void Ante(int amount);
        void FlipDealerStat();

        // Functions to assess the hand given, checking for all possible win scenarios
        void CombineHands(std::vector<Card> &communityHand);
        int LookForSets(int cardAmount, int previousFind = 14);
        int EvaluateCards(std::vector<Card> &communityHand);
        int CheckForStraightFlush();
        int CheckForFourOfAKind();
        int CheckForFullHouse();
        int CheckForFlush();
        int CheckForStraight();
        int CheckForThreeOfAKind();
        int CheckForTwoPair();
        int CheckForPair();
        int CheckForHighCard();
        void GrabCardsInOrder(int numCards);

        // A print function to see the hand at any given time
        // Mostly for testing purposes
        void PrintHand();

    private:
        int chips;
        std::string name;
        std::vector<Card> hand;
        bool dealer;
        int totalBet;
        std::vector<int> handStats = {0, 0, 0, 0, 0, 0};
        bool folded;
};

#endif