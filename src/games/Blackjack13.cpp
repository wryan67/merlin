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

    int Blackjack13::announceCards(char *who, vector<int> &cards) {
        char msg[256];
        char tmpstr[32];

        sprintf(msg, "%s has %d card%s, ", who, (int)cards.size(), (cards.size()==1)?"":"s");
        for (int card : cards) {
            sprintf(tmpstr, "%d ", card);
            strcat(msg, tmpstr);
        }
        eSpeak(msg);
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

        printVector("deck", deckCards);

        dealFromDeck(playerCards, false);
        dealFromDeck(dealerCards, false);

        printVector("deck", deckCards);
        printVector("player", playerCards);
        printVector("dealer", dealerCards);

        if (screenReader) {
            announceCards("player", playerCards);
            announceCards("dealer", dealerCards);
        }
        render();
        isActive = true;
    }

    int Blackjack13::dealFromDeck(vector<int> &hand, bool audiable) {
        int card = deckCards.back();
        hand.push_back(card);
        deckCards.pop_back();

        render();
        if (audiable) {
            playTone(noteHz[card], .333, &wavForamt);
        }

        return card;
    }


    enum Articles {
        A, AN, THE
    };
    char* articles[3] = {
        "a",
        "an",
        "the"
    };

    char* Blackjack13::getArticle(int card) {
        switch (card) {
        case 1: return articles[A];
        case 2: return articles[A];
        case 3: return articles[A];
        case 4: return articles[A];
        case 5: return articles[A];
        case 6: return articles[A];
        case 7: return articles[A];
        case 8: return articles[AN];
        case 9: return articles[A];
        case 10: return articles[A];
        }

    }

    void Blackjack13::keypadButtonReleased(int button, long long elapsed) {
        if (debug) fprintf(stderr, "Magic Square -- key pressed:  %d\n", button);
        render();
    }

    void Blackjack13::hitMe() {
        if (debounce(HIT_ME, 200)) return;
        printf("hit me\n");
        if (!isActive) {
            return;
        }

        int card=dealFromDeck(playerCards, true);
        int score = computeScore(playerCards);

        if (screenReader) {
            char msg[256];
            sprintf(msg, "player receives %s %d", getArticle(card), card);
            eSpeak(msg);
        }

        if (score > 13) {
            isActive = false;
            playWav("playerbusted.wav", false);
        }
    }

    void Blackjack13::computerTurn() {
        if (debounce(COMP_TURN, 100)) return;
        printf("computer turn\n");
        if (!isActive) {
            return;
        }

        int playerScore = computeScore(playerCards);
        int dealerScore = computeScore(dealerCards);

        while (dealerScore < 13 && dealerScore < playerScore) {
            int card = dealFromDeck(dealerCards, true);
            dealerScore += card;
            
            if (screenReader) {
                char msg[256];
                sprintf(msg, "dealer receives %s %d", getArticle(card), card);
                eSpeak(msg);
            }

        }

        if (dealerScore > 13) {
            isActive = false;
            playWav("achievement-00.wav", true);
            delay(1000);
            playWav("playerwins.wav", true);
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