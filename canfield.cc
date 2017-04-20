/*********************************************************************
  Author:        Dana Vrajitoru, IUSB, CS
  Class:         C243 Data Structures
  File name:     canfield.cc
  Last updated:  September 2014
  Description:   Implementation of the card game of Canfield using 
                 stacks and queues.
**********************************************************************/

#include "Queue.h"
#include "Stack.h"
#include "canfield.h"
#include "general.h"
#include <iostream>
#include <iomanip>
using namespace std;
#include <ctime>
#include <cstdlib>

// It's easier to keep the game data global.
Queue deck;
Stack reserve;
Stack foundation[NUM_SUITS], tableau[NUM_SUITS];
int waste;
bool quit=false;
char from, to;
int fromId, toId, scoreCount;

// The main loop of the game.
void canfield()
{
    char answer;
    srand(time(NULL));
    newGame();
    printGame();
    while (!quit) 
    {
        printMenu();
        cin >> answer;
        performAction(answer);
    }
} // canfield()

// Swap two integers
void swap(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
} // swap

// Starting a new game. It must reset all the data structures,
// generate a new set of 52 random cards and place them in the
// appropriate places in the game.
void newGame()
{
    int i, card;
    int cardsLeft = NUM_CARDS;
    int newDeck[NUM_CARDS];

    // Empty all the data structures
    emptyAll();

    // Create a new deck and shuffle it
    newShuffledPack(newDeck);

    // Now place the cards in the data structures
    for (i=0; i<NUM_RANKS; i++)      // fill in the reserve with 13 cards
    {
        card = newDeck[--cardsLeft]; // use a card and decrease the total count
        reserve.push(card);
    }
    for (i=0; i<NUM_SUITS; i++)      // draw a card for each tableau
    {
        card = newDeck[--cardsLeft]; // use a card and decrease the
                                     // total count
        tableau[i].push(card);
    }
    for (i=0; i<NUM_SUITS; i++)      // Initialize the foundations with 0
        foundation[i].push(0);
    waste = newDeck[--cardsLeft];    // the card to play
    while (cardsLeft > 0)            // the remaining cards go into the deck
    {
        card = newDeck[--cardsLeft];
        deck.enqueue(card);
    }
} // newGame()

// Create a new pack of cards and shuffles them
void newShuffledPack(int *newDeck)
{
    int i, j;
    for (i=0; i<NUM_CARDS; i++)
        newDeck[i] = 1+i % NUM_RANKS; // add the cards in order
    for (i=0; i<NUM_CARDS-1; i++)     // then shuffle them
    {
        j = i+rand() % (NUM_CARDS-i); // random other card
        swap(newDeck[i], newDeck[j]); // swap them
    }
} // newShuffledPack()

// Empty all the data structures
void emptyAll()
{
    int i;
    deck.makeEmpty();
    reserve.makeEmpty();
    for (i=0; i<NUM_SUITS; i++) 
    {
        foundation[i].makeEmpty();
        tableau[i].makeEmpty();
    }
} // emptyAll()

// Prints the choices of actions for the player.
void printMenu()
{
    cout << "[q] quit [N] new game [m] move [d] deal a card [r] repeat last move" << endl;
    cout << "Enter your choice: ";
} // printMenu()

// Performs an action based on the user's choice.
void performAction(char answer)
{
    switch (answer) 
    {
    case 'q':
    case 'Q':
        quit = true;
    break;
    //case 'n': this causes problems because it's next to m
    case 'N':
        newGame();
        break;
    case 'm':
    case 'M':
        move();
        break;
    case 'd':
    case 'D':
        deal();
        break;
    case 'r':
    case 'R':
	repeat();
    }
    if (!quit)
        printGame();
} // performAction()

// Displays the entire content of the game.
void printGame()
{
    clearScreen();
    cout << "Deck:    ";
    if (waste != -1)
        cout << waste;
    cout << endl << endl
         << "Reserve: ";
    if (!reserve.isEmpty())
        cout << reserve.top();
    cout << endl << endl;
    for (int i=0; i<NUM_SUITS; i++) 
    {
        cout << "F" << i << "      ";
        if (foundation[i].top() > 0)
            cout << setw(2) << foundation[i].top() << "   ";
        else
            cout << "     ";
        cout <<"T" << i << ' ';
        tableau[i].print();
        cout << endl << endl;
    }
    cout << endl;
	cout << "Score: " << scoreCount << endl << endl;
} //  printGame()

// Deals one card from the deck.
void deal()
{
    if (!deck.isEmpty()) 
    {
        deck.enqueue(waste);
        waste = deck.dequeue();
    }
} // deal()

// Performs one move.
void move()
{
    int card;
    cout << "From:" << endl
         << "[t] top of the tableau [T] whole tableau [d] deck [r] reserve : ";
    cin >> from;
    if (from == 't' || from == 'T') 
    {
        cout << "number : ";
        getNumber(fromId, NUM_SUITS);
    }
    cout << "To: [t] tableau [f] foundation : ";
    cin >> to;
    cout << "number : ";
    getNumber(toId, NUM_SUITS);
    card = getCard();
    cout << "card to move: " << card << endl;
    if (card != -1 && checkMove(card)) 
    {
        removeCard();
        addCard(card);
        moveFromReserve(card);
	winGame();                              //check if game has been won
    }
    else 
    {
        cout << "Movement not permitted. Strike any key to continue."; 
        cin.get();
        while (cin.get() != '\n') ; // Clear the line
    }
	
} // move()

// Checks if the proposed movement is possible.
bool checkMove(int card)
{
    if (to == 't' || to == 'T') 
        if (tableau[toId].isEmpty()) // one can always move anything to an
                                     // empty tableau
            return true;
        else
            return (tableau[toId].top() == card+1 || 
                    tableau[toId].top() == 1 && card == NUM_RANKS) ;
	// One can place a card on the top of the tableau if it's one
	// card smaller than the top of that tableau. One can also
	// place a 13 over a 1.
    else if (to == 'f' || to == 'F') 
    {
	// Following if statement returns true if foundation top card
	// is equivelant to the top element of tableau subtracted by
	// tableau size.
        if (from == 'T')
            return (foundation[toId].top() == 
                    (card - tableau[fromId].stored()));
        return (foundation[toId].top() == card-1);
    }
    return false; // Anything else is not permitted.
} // checkMove()

// Removes the card to be moved.
void removeCard()
{
    int error=0;
    switch (from) 
    {
    case 'd':
    case 'D':
        if (!deck.isEmpty())
            waste=deck.dequeue();
        else
            waste=-1;
        break;
    case 'r':
    case 'R':
        if (!reserve.isEmpty())
            reserve.pop();
        else 
            error = 1;
        break;
    case 't':
        if (!tableau[fromId].isEmpty()) 
            tableau[fromId].pop();
        else 
            error = 2;
        break;
    case 'T':
        if (!tableau[fromId].isEmpty())
            ;
        else 
            error = 3;
        break;
    }
    if (error) 
    {
        cout << "Error in the code: " << error << endl;
        exit(1);
    }
} // removeCard()

// Adds the card or the entire tableau to its new destination.
void addCard(int card)
{
    if (from == 'T' && (to == 't' || to == 'T')) 
    {
        // add the stack onto which we move at the end of the one we move.
        tableau[fromId].concatenate(tableau[toId]);
        // move that stack back into the destination stack.
        tableau[toId].concatenate(tableau[fromId]);
    }
    else if (from == 'T' && (to == 'f' || to == 'F')) 
    {
        int size = tableau[fromId].stored();  // store the size of the tableau 
                                              // into a variable since its size
	                                      // is modified.

        // Place elements from tableau into foundation
        // in reverse order
        for (int i = 0; i < size; ++i)
        {
            foundation[toId].push(tableau[fromId].pop());
            score();                                       // increase score
        }
    }
    else 
        if (to == 't' || to == 'T') 
            tableau[toId].push(card);
        else 
        {
            foundation[toId].push(card);
            score();
        }
} // addCard()

// Determines what card is to be moved.
int getCard()
{
    switch (from) 
    {
    case 'd':
    case 'D':
        return waste;
    case 'r':
    case 'R':
        if (!reserve.isEmpty())
            return reserve.top();
        else
            return -1;
    case 't':
        if (!tableau[fromId].isEmpty())
            return tableau[fromId].top();
        else
            return -1;
    case 'T':
        if (!tableau[fromId].isEmpty())
            return tableau[fromId].bottom();
        else
            return -1;
    default:
        return -1;
    }  
} // getCard()

// Clears the terminal screen.
void clearScreen()
{
    for (int i=0; i<24; i++)
        cout << endl;
} // clearScreen()


// Repeats the previous move.
void repeat()
{
	int card;

    card = getCard();

    // Statement does not modify to, from, toId, or fromId
    // Simply uses the values from the previous move
    if (card != -1 && checkMove(card)) //check if the move is valid
    {
        removeCard();
        addCard(card);
        moveFromReserve(card);
        winGame();
    }
    else 
    {
        cout << "Movement not permitted. Strike any key to continue."; 
        cin.get();
        while (cin.get() != '\n') ; // Clear the line
    }
}//repeat()

//Simple function that increments the score when called
void score()
{
    ++scoreCount;
}// score()

// Function checks if the game has been won, and if it has,
// will display a small congratulations, and give user the
// chance to quit or start a new game.
void winGame()
{
    char answer;
    if (scoreCount == NUM_CARDS)
    {
        printGame();
        cout << endl;
        cout << "Congratulations! You beat the game!" << endl << endl;
        cout <<"[q] quit [N] new game" << endl;
        cout << "Enter your choice: ";
        cin >> answer;
	while (answer == 'm' || answer =='M' || answer == 'd' || 
               answer == 'D' || answer == 'r' || answer == 'R')         // Game is won, cannot make moves
	{
	    cout << "Invalid input, options are [q] quit [N] new game" << endl;
	    cin >> answer;
	}
        performAction(answer);
        scoreCount = 0;                     // Reset the score
    } 
}// winGame()


// Function checks if tableau is empty, and if it is
// moves card from reserve to empty tableau.
void moveFromReserve(int card)
{
    if (tableau[fromId].isEmpty() && (!reserve.isEmpty()))   // Checks that both tableau 
                                                             // is empty and reserve is not empty
    {
        char tempTo = to, tempFrom = from;               // Store current values of to and from in variables
	int tempToId = toId, tempFromId = fromId;
	to = from;
	from = 'r';                                      // Store r in from to pull from reserve
	toId = fromId;
	card = getCard();                                // Get the card from reserve
	removeCard();                                    // Pop card off the stack
	addCard(card);                                   // Add card to tableau
       	to = tempTo;                                     // Return to and from to its original values
	from = tempFrom;
	toId = tempToId;
	fromId = tempFromId;
    }
}// moveFromReserve(int card)
