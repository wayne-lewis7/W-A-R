#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct card_s{
	int rank;    //number
	char * type; //type of card
	struct card_s * nextptr;
}card_t;

//function prototypes
void rules(); //display rules
int playRound(); //simulate round
card_t * openCardDeck(); //open the card deck and place into a linkedlist setup
card_t * insertBackSetup(card_t *node, char *name, int cardrank); //take card from orginial deck and place in back of linked list for setup of game
int empty(card_t * node); //check to see if linked list is empty
void cleanUp(card_t * head); //free memory to prevent memory leaks
int deckSize(card_t * head); //count number of nodes in the linked list
card_t * search(card_t * node, int spot); //search list for a specific spot in the card deck indexing is similar to array setup
card_t * copyCard(card_t * node); //make a deep copy of card
card_t * removeCard(card_t * node, int spot); //remove card from linkedlist
card_t * insertBackDeck(card_t *head, card_t *node); //place card at end of pile
int compareCard(card_t * cardp1, card_t * cardp2); //compare cards
card_t * moveCardBack(card_t *head); //place card at top of deck to the bottom of the deck

int main()
{
	int seed;
	printf("Enter Seed: ");
	scanf("%d", &seed);
	
	srand(seed); //seed set
	rules();
	
	int player; //1 or 2
	int result;
	
	printf("Would you like to be player 1 or 2?\n");
	printf("Enter 1 or 2: ");
	scanf("%d", &player); //choose player
	
	for(int game = 1; game <= 5; ++game) //simulate games
	{
		printf("Alright lets play game %d.\n", game);
		printf("Lets split the deck.\n");

		result = playRound(); //play game
		
		if((result == 1 && player == 1) || (result == 2 && player == 2)) //determine who won
			printf("You won game %d.\n", game);
		else
			printf("I won game %d.\n", game);
	}
	
	return 0;
}

void rules()
{
	printf("Welcome to the card game war!\n");
	printf("Here are the rules.\n");
	printf("You have a pile of cards and I have a pile of cards.\n");
	printf("We will each pull the top card off of our respective deck and compare them.\n");
	printf("The card with the highest number will win the round and take both cards.\n");
	printf("However if there is a tie, then we have to we have to place one card faced down and the next one faced up and compare the results.\n");
	printf("Winner of the tie, will grab all the cards out. However if it's a tie again, then we repeat the same action.\n");
	printf("Ready? Here we go!\n");
}

card_t * openCardDeck()
{
	FILE *fptr = fopen("cardDeck.txt", "r");
	
	char *name = (char *) malloc(sizeof(char) * 20);
	
	if (name == NULL)
	{
		printf("Error in malloc...\n");
		exit(1);
	}
	
	card_t * head = NULL;
	
	int cardrank = 13;
	int tracker = 1;
	while(fscanf(fptr, "%s", name) == 1)
	{			
		strcat(name, " ");
		
		if(tracker % 5 == 1)
		{
			strcat(name, "of Clubs");
			head = insertBackSetup(head, name, cardrank);
		}
		else if(tracker % 5 == 2)
		{
			strcat(name, "of Diamonds");
			head = insertBackSetup(head, name, cardrank);
		}
		else if(tracker % 5 == 3)
		{
			strcat(name, "of Hearts");
			head = insertBackSetup(head, name, cardrank);
		}
		else if(tracker % 5 == 4)
		{
			strcat(name, "of Spades");
			head = insertBackSetup(head, name, cardrank);
			tracker = 0;
			--cardrank;
		}
		
		++tracker;
		
	}
	
	free(name);
	fclose(fptr);
	
	return head;
}

card_t * insertBackSetup(card_t *node, char *name, int cardrank)
{
	
    if(empty(node)) //check to see if list is empty
    {
		node = (card_t *) malloc(sizeof(card_t));
		
		if(empty(node)) //check to see if malloc worked
		{
			printf("Did not allocate head successfully...");
			printf("Program Terminating...\n");
			exit(1);
		}
		else //otherwise populate data of new head node
		{
			node->type = (char *) malloc(sizeof(char) * 20);
			
			if(node->type == NULL)
			{
				printf("Error with malloc...\n");
				exit(1);
			}
			
			strcpy(node->type, name);
			node->rank = cardrank;
			node->nextptr = NULL; //must make new tail nextptr NULL!!!
			
			return node;  //terminate
		}
      
    }
	
	//here we know that the list has at least one node
	
	card_t *head = node; //keep pointer to head since we will need to return this address
	
	while(node->nextptr != NULL) //traverse to tail
		node = node->nextptr;
	
	node->nextptr = (card_t *) malloc(sizeof(card_t)); 
	
	if(node->nextptr == NULL) //see if new tail was allocated successfully
	{
		printf("Did not allocate node successfully...");
		return head; //terminate if tail didn't get created
	}
	
	//populate new node
	node->nextptr->type = (char *) malloc(sizeof(char) * 20);
	
	if(node->nextptr->type == NULL)
	{
		printf("Error with malloc...\n");
		exit(1);
	}
	 
	strcpy(node->nextptr->type, name);
	node->nextptr->rank = cardrank;
	node->nextptr->nextptr = NULL; //very important
			
	return head; //return head node since we need our starting point of the linked list
}

int empty(card_t * node)
{
	return (node == NULL); //return condition result
}

int playRound()
{
    card_t *setupDeck = openCardDeck();      // Assigning struct pointer deck to openCardDeck's returned pointer
	printf("There are %d cards in the deck.\n", deckSize(setupDeck));

	card_t *player1Deck = NULL;
	card_t *player2Deck = NULL;

	while(deckSize(setupDeck) != 0)
	{
		int randNum2 = rand() % deckSize(setupDeck);
		player2Deck = insertBackDeck(player2Deck, search(setupDeck, randNum2));
		
		setupDeck = removeCard(setupDeck, randNum2);

		int randNum1 = rand() % deckSize(setupDeck);
		player1Deck = insertBackDeck(player1Deck, search(setupDeck, randNum1));

		setupDeck = removeCard(setupDeck, randNum1);	
	}

	int roundResult = 0;
	card_t *warDeck = NULL;		// Deck to be used for cards that are up for grabs in war rounds

	while(deckSize(player1Deck) != 52 && deckSize(player2Deck) != 52)
	{
		card_t * p1card = NULL;
		card_t * p2card = NULL;
		
		roundResult = compareCard(player1Deck, player2Deck);

		if(roundResult == 1)
		{
			printf("Player 1 won that round.\n");
			player1Deck = insertBackDeck(player1Deck, search(player2Deck, 0));
			player2Deck = removeCard(player2Deck, 0);
			player1Deck = moveCardBack(player1Deck);
		}

		else if(roundResult == 2)
		{	
			printf("Player 2 won that round.\n");
			player2Deck = insertBackDeck(player2Deck, search(player1Deck, 0));
			player1Deck = removeCard(player1Deck, 0);
			player2Deck = moveCardBack(player2Deck);
		}
		
		// W-A-R
		else
		{
			while(roundResult == 0)
			{
				printf("Ugh oh...We have a tie! W-A-R!\n");

				if(deckSize(player1Deck) == 0)		// Player 1 has no more cards to draw
					roundResult = 2;

				else if(deckSize(player2Deck) == 0)		// Player 2 has no more cards to draw
					roundResult = 1;
					
				else if(deckSize(player1Deck) == 1)		// If Player 1 has one card left, they lose the war by default
				{
					warDeck = insertBackDeck(warDeck, search(player1Deck, 0));
					player1Deck = removeCard(player1Deck, 0);
					roundResult = 2;
				}

				else if(deckSize(player2Deck) == 1)		// If Player 2 has one card left, then they lose the war by default
				{
					warDeck = insertBackDeck(warDeck, search(player2Deck, 0));
					player2Deck = removeCard(player2Deck, 0);
					roundResult = 1;
				}

				else
				{
					if(deckSize(player1Deck) == 2 || deckSize(player2Deck) == 2)
					{
						
						p1card = copyCard(search(player1Deck, 1));
						p2card = copyCard(search(player2Deck, 1));

						warDeck = insertBackDeck(warDeck, search(player1Deck, 0));
						player1Deck = removeCard(player1Deck, 0);

						warDeck = insertBackDeck(warDeck, search(player1Deck, 0));
						player1Deck = removeCard(player1Deck, 0);

						warDeck = insertBackDeck(warDeck, search(player2Deck, 0));
						player2Deck = removeCard(player2Deck, 0);

						warDeck = insertBackDeck(warDeck, search(player2Deck, 0));
						player2Deck = removeCard(player2Deck, 0);

						roundResult = compareCard(p1card, p2card);
						cleanUp(p1card);
						cleanUp(p2card);
					}

					else
					{
						p1card = copyCard(search(player1Deck, 2));
						p2card = copyCard(search(player2Deck, 2));

						warDeck = insertBackDeck(warDeck, search(player1Deck, 0));
						player1Deck = removeCard(player1Deck, 0);

						warDeck = insertBackDeck(warDeck, search(player1Deck, 0));
						player1Deck = removeCard(player1Deck, 0);

						warDeck = insertBackDeck(warDeck, search(player2Deck, 0));
						player2Deck = removeCard(player2Deck, 0);

						warDeck = insertBackDeck(warDeck, search(player2Deck, 0));
						player2Deck = removeCard(player2Deck, 0);

						warDeck = insertBackDeck(warDeck, search(player1Deck, 0));
						player1Deck = removeCard(player1Deck, 0);

						warDeck = insertBackDeck(warDeck, search(player2Deck, 0));
						player2Deck = removeCard(player2Deck, 0);

						roundResult = compareCard(p1card, p2card);
						cleanUp(p1card);
						cleanUp(p2card);
					}
				}
			}
			
			// War round while loop ended; checking result of war round
			if(roundResult == 1)
			{
				printf("Player 1 won that W-A-R!\n");
				while(deckSize(warDeck) != 0)
				{
					player1Deck = insertBackDeck(player1Deck, search(warDeck, 0));
					warDeck = removeCard(warDeck, 0);
					player1Deck = moveCardBack(player1Deck);
				}
			}

			else
			{
				printf("Player 2 won that W-A-R!\n");
				while(deckSize(warDeck) != 0)
				{
					player2Deck = insertBackDeck(player2Deck, search(warDeck, 0));
					warDeck = removeCard(warDeck, 0);
					player2Deck = moveCardBack(player2Deck);
				}
			}
		}

		printf("Player 1 has %d cards.\n", deckSize(player1Deck));
		printf("Player 2 has %d cards.\n", deckSize(player2Deck));

	}
	
	if(deckSize(player1Deck) == 52)		// Player 1 wins the game
	{
		cleanUp(player1Deck);
		return 1;
	}

	else		// Player 2 wins the game
	{
		cleanUp(player2Deck);
		return 2;
	}
}

void cleanUp(card_t *head)
{
	if(head == NULL)
		return;
	
	else
	{
		card_t * temp = head;
		head = head->nextptr;
		free(temp->type);
		free(temp);
		cleanUp(head);		// Recursive call
	}
	return;
}

int deckSize(card_t *head)
{
	int numCards = 0;
	
	while(head != NULL)
	{
		numCards++;
		head = head->nextptr;
	}

	return numCards;
}

card_t * search(card_t *node, int spot)
{
	for(int i = 0; i < spot; i++)
		node = node->nextptr;

	if(empty(node))
		return NULL;

	return node;
}

card_t * copyCard(card_t *node)
{
	card_t *cardCopy = (card_t *) malloc(sizeof(card_t));

	cardCopy->type = (char *) malloc(sizeof(char) * 20);
	int cardRank = node->rank;		// Grabbing rank value of card to be copied
	cardCopy->rank = cardRank;

	strcpy(cardCopy->type, node->type);
	cardCopy->nextptr = NULL;

	return cardCopy;
}

card_t * removeCard(card_t *node, int spot)
{
	if(empty(node))
		return NULL;
	
	card_t *head = node;		// Keeping reference to head of linked list

	card_t *temp = search(node, spot);

	if(temp == NULL)
		return head;

	if(head == temp)		// If card is the head of the linked list
	{
		head = head->nextptr;
		free(temp->type);
		free(temp);
		return head;
	}

	while(node->nextptr != temp)		// Traverse linked list until node preceding temp is reached
		node = node->nextptr;

	node->nextptr = temp->nextptr;		// Node's next pointer is assigned to temp's next pointer, so temp can now be freed without breaking the list
	free(temp->type);
	free(temp);

	return head;
}

card_t * insertBackDeck(card_t *head, card_t *node)
{
	if(empty(head))
	{
		head = copyCard(node);
		return head;
	}

	card_t *temp = head;

	while(temp->nextptr != NULL)		// Traversing linked list until tail pointer is reached
		temp = temp->nextptr;

	temp->nextptr = copyCard(node);

	return head;
}

int compareCard(card_t * cardp1, card_t * cardp2)
{
	printf("Player 1 pulled out %s. 	 Player 2 pulled out %s.\n", cardp1->type, cardp2->type);

	if(cardp1->rank > cardp2->rank)
	{
		return 1;
	}
	
	else if(cardp2->rank > cardp1->rank)
	{
		return 2;
	}

	else
	{
		return 0;
	}
}

card_t * moveCardBack(card_t *head)
{
	if(head->nextptr == NULL)
		return head;
		
	card_t * node = head;
	head = head->nextptr;		// Head's next pointer is now new head of linked list

	card_t * temp = node;

	while(temp->nextptr != NULL)
		temp = temp->nextptr;
	
	temp->nextptr = copyCard(node);

	free(node->type);
	free(node);

	return head;
}