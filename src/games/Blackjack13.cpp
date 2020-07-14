#include "Blackjack13.h"
#include <algorithm> 

using namespace std;

namespace Games {
    Blackjack13::Blackjack13() {
        gameName = "Blackjack 13";
        gameWav = "blackjack13.wav";
    }

    void Blackjack13::render() {
        if (debug) fprintf(stderr, "render: ");

        clearBoard();
        for (int card: playerCards) {
            setPixelColor(card, playerColor);
        }

        for (int card : dealerCards) {
            setPixelColor(card, computerColor);
        }

        GameEngine::render();
    }


    void Blackjack13::restartGame() {
        printf("%s\n", gameName);

        playerCards.clear();
        dealerCards.clear();
        deckCards.clear();

        for (int i = 1; i <= 10; ++i) {
            deckCards.push_back(i);
        }

        random_shuffle(deckCards.begin(), deckCards.end());

        printHand("deck", deckCards);

        dealFromDeck(playerCards, false);
        dealFromDeck(dealerCards, false);

        printHand("deck", deckCards);
        printHand("player", playerCards);
        printHand("dealer", dealerCards);

        render();
        isActive = true;
    }

    int Blackjack13::dealFromDeck(vector<int> &hand, bool audiable) {
        int card = deckCards.back();
        hand.push_back(card);
        deckCards.pop_back();

        render();
        if (audiable) {
            playTone(globalSoundCardHandle, noteHz[card], .333, &wavHeader);
        }


        return card;
    }

    void Blackjack13::printHand(const char* message, vector<int> cards) {
        bool first = true;
        printf("%s: ",message);
        for (int i : cards) {
            if (!first) {
                printf(",");
            }
            else {
                first = false;
            }
            printf("%d", i);
        }   printf("\n");

    }

    void Blackjack13::keypadButtonReleased(int button) {
        if (debug) fprintf(stderr, "Magic Square -- key pressed:  %d\n", button);

    }

    void Blackjack13::hitMe() {
        printf("hit me\n");
        if (!isActive) {
            return;
        }

        int card=dealFromDeck(playerCards, true);
        
        int score = computeScore(playerCards);

        if (score > 13) {
            isActive = false;
            playWav("playerbusted.wav", false);
        }
    }

    void Blackjack13::computerTurn() {
        printf("computer turn\n");
        if (!isActive) {
            return;
        }

        int playerScore = computeScore(playerCards);
        int dealerScore = computeScore(dealerCards);

        while (dealerScore < 13 && dealerScore < playerScore) {
            dealerScore += dealFromDeck(dealerCards, true);
        }

        if (dealerScore > 13) {
            isActive = false;
            playWav("achievement-00.wav", true);
            return;
        }

        if (dealerScore == playerScore) {
            isActive = false;
            playWav("tiegame.wav", true);
            return;
        }

        isActive = false;
        playWav("youlose.wav", true);
        delay(1000);
        playWav("dealerwins.wav", true);
    }

    int Blackjack13::computeScore(vector<int> &cards) {
        int score = 0;
        for (int card : cards) {
            score += card;
        }

        return score;
    }

}