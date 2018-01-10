//
// Created by codeninja on 10/01/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include "helper.h"

// File to store user data
#define DATAFILE "/var/chance.data"

// Custom user struct to store information about users
struct user {
    int uid;
    int credits;
    int highscore;
    char name[200];
    int (*current_game) ();
};

// Function prototypes
// Stored in the data segment
void register_new_player();
int get_player_data();
void update_player_data();
void show_highscore();
void jackpot();
void input_name();
void print_cards(char *, char *, int);
int take_wager(int, int);
void play_the_game();
int pick_a_number();
int dealer_no_match;
int find_the_ace();
void fatal(char *);

// Global variables
// Stored in the bss memory segment
struct user player;  // Player struct

int main(int argc, char *argv[]) {
    int choice, last_game;

    // Seed the randomizer with the current time
    srand(time(0));

    if(get_player_data() == -1)  // Try to reload player data from file.
        register_new_player();   // If there is no data, register a new player.

    while(choice != 7) {  // Menu will run until 7 is selected to quit
        printf("|==[Game of Chance Menu]==|\n");
        printf("[1] | Play the Pick a Number game\n");
        printf("[2] | Play the No Match Dealer game\n");
        printf("[3] | Play the Find the Ace game\n");
        printf("[4] | View the current high score\n");
        printf("[5] | Change your user name\n");
        printf("[6] | Reset your account at 100 credits\n");
        printf("[7] | Quit\n");
        printf("\n[Name] | %s\n", player.name);
        printf("[Credits] | %u\n", player.credits);
        printf(">> ");
        scanf("%d", &choice);

        if((choice < 1) || (choice > 7)) {
            printf("\n[!!] The number %d is an invalid selection.\n\n", choice);
        } else if (choice < 4) {          // Otherwise, choice was a game of some sort
            if(choice != last_game) {   // If the function ptr isn't set
                if(choice == 1)         // Then point it at the selected game
                    player.current_game = pick_a_number;
                else if(choice == 2)
                    player.current_game = dealer_no_match;
                else if(choice == 3)
                    player.current_game = find_the_ace;
                last_game = choice;     // and set last_game
            }
            play_the_game();            // Play the game.
        } else if(choice == 4) {
            show_highscore();
        } else if(choice == 5) {
            printf("\nChange user name\n");
            printf("Enter your new user name >>  ");
            input_name();
            printf("Your name has been changed. \n\n");
        } else if(choice == 6) {
            printf("\nYOur account has been reset with 100 credits.\n\n");
            player.credits = 100;
        }
    }

    update_player_data();
    printf("\nThanks for playing! Bye.\n");
}

/* This function is used to input the player name, since
 * scanf("%s", &whatever) will stop input at the first space. */
void input_name() {
    char *name_ptr, input_char='\n';

    // Flush any leftover newline chars
    while(input_char == '\n')
        scanf("%c", &input_char);

    name_ptr = (char *) &(player.name);  // name_ptr = player name's address
    while(input_char != '\n') {          // Loop until newline.
        *name_ptr = input_char;          // Put the input char into name field.
        scanf("%c", &input_char);        // Get the next char.
        name_ptr++;                      // Increment the name pointer.
    }
    *name_ptr = 0;  // Terminate the string.
}

/* This function reads the player data for the current uid from the file.
 * Returns: -1 if it is unable to find a player data for current uid. */
int get_player_data() {
    int fd, uid, read_bytes;
    struct user entry;

    uid = getuid();
    fd = open(DATAFILE, O_RDONLY);

    if(fd == -1)  // Can't open the file, maybe it doesn't exist
        return -1;

    // Uses the sizeof() macro to find the struct data type size of user
    // Uses this value to read only this many characters from the file
    read_bytes = read(fd, &entry, sizeof(struct user));      // Read the first chunk
    while(entry.uid != uid && read_bytes > 0) {              // Loop until proper uid is found
        read_bytes = read(fd, &entry, sizeof(struct user));  // Keep reading
    }
    close(fd);  // Close the file

    if(read_bytes < sizeof(struct user))  // This means that the EOF was reached.
        return -1;
    else
        player = entry;  // Copy the read entry into the player struct.

    return 1;  // Return a success
}

/* This function is for new user registrations.
 * It will create a new player account and append it to the file. */
void register_new_player() {
    int fd;

    printf("|===={ New Player Registration }====|\n");
    printf("Enter your user name >>  ");
    input_name();

    player.uid = getuid();
    player.highscore = player.credits = 100;

    fd = open(DATAFILE, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);

    if(fd == -1)
        fatal("in register_new_player() while opening file");

    write(fd, &player, sizeof(struct user));
    close(fd);

    printf("\nWelcome to the Game of Chance %s.\n", player.name);
    printf("You have been given %u credits.\n", player.credits);
}

/* This function writes the current player data to the file.
 * It is used primarily for updating the credits after game. */

