#ifdef _WIN32
#include <conio.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


/* PROTOTYPES */
void clearScreen(void);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Purpose:         This program takes in words from an ASCII file, sorts, and prints them.
 *                  The user is then prompted for a word to search this list. If found, the
 *                  sorted location of this word is reported.
 *
 * Program name:    BCFinalExam.c
 *
 * Invocation:      ./a.out on Unix/Linux/MacOS
 *                    a.exe on Windows
 *
 * Variables:       const char FILENAME[]           - CONSTANT; filename of file with the words
 *                  const short int MAX_LIST_SIZE   - CONSTANT; max number of words from file
 *                  const short int MAX_WORD_SIZE   - CONSTANT; max number of chars per word
 *                  const short int NOT_FOUND       - CONSTANT; indicates that a word is not listed
 *                  const short int READERR         - CONSTANT; error code for file open error
 *                  const short int NOERR           - CONSTANT; code for no error
 *
 *                  short char wordList[][]     - 2D array for holding a list of c-strings
 *                  short char cString[]        - Holds c-string values
 *                  short int  fileReadStatus   - return for fscanf(). Used to determine EOF
 *                  short int  alphaStatus      - (+) 1st string = later; (-) 1st string = earlier;
 *                                                 a zero value indicates strings are equal
 *                  short int rtnCode           - The current error status of main()
 *                  short int wrdCnt            - The number of words found in the file
 *                  short int index             - Used to index 'wordList' during operations
 *
 *                  short int range             - The width of the search range
 *                  short int front             - Where the seearch range begins
 *                  short int end               - Where the seearch range ends
 *                  short int mid               - The midpoint of the search range
 *
 *                  bool isFullySrched          - Indicates the whole word list was processed.
 *                  bool isSwapped              - Indicates if a swap was used in sorting
 *                  bool debug                  - Used to enable print statements for debug
 *                  bool isEnd                  - Indicates the end of file/string was reached
 *
 *                  FILE *sourceFile            - Pointer to the file holding the unsorted words
 *
 * Functions:       clearScreen()               - Cross-platform clear screen from professor
 *                  strcmp()                    - Determines the alphabetic order of c-strings
 *                  strcpy()                    - Used to copy c-strings
 *
 * Written by:      Brandon Crenshaw  7/07/2025
 *
 * Modifications:   None
 *
 * Special notes:   Program assumes that all words in the ascii file are lower-case.
 *                  The word list limited to 50 words; words limited to 20 chars.
 *                  If multiple instances of a word; ANY of their positions can be reported.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

int main()
{
    /*CONSTANTS*/
    const char MAIN_WRDS_FILE[] = "FinalExamTextToParse.txt";
	const char RSVD_WRDS_FILE[] = "FinalReservedWords.txt";
    const short int MAX_LIST_SIZE = 50;
    const short int MAX_WORD_SIZE = 20;
    const short int NOT_FOUND = -1;
    const short int READERR   = 10;
    const short int NOERR     =  0;

    /*VARAIABLES*/
    char mainWordList[MAX_LIST_SIZE + 1][MAX_WORD_SIZE + 1];
	char wordsToRemove[MAX_LIST_SIZE + 1][MAX_WORD_SIZE + 1];
    char cString[MAX_WORD_SIZE + 1];

    short int position = NOT_FOUND;
    short int rtnCode = NOERR;
    short int fileReadStatus;
    short int alphaStatus;
    short int matchStatus;
    short int wrdCnt = 0;
    short int front = 0;
    short int index;
    short int range;
    short int end;
    short int mid;

    bool isFullySrched = false;
    bool isSwapped   = true;
    bool isEnd = false;
    bool debug = false;

    FILE *mainWordsFile;
	FILE *rsvdWordsFile;


    /* LOAD FILE TO MEMORY * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    clearScreen();
    mainWordsFile = fopen(MAIN_WRDS_FILE, "r");
	rsvdWordsFile = fopen(RSVD_WRDS_FILE, "r");

    if(mainWordsFile == NULL)
    {
        printf("\tERROR: '%s' failed to open.\n",MAIN_WRDS_FILE);
        rtnCode = READERR;
        goto EOP;
    }                                                       // End of Read File Error

    if(rsvdWordsFile == NULL)
    {
        printf("\tERROR: '%s' failed to open.\n",RSVD_WRDS_FILE);
        rtnCode = READERR;
        goto EOP;
    }                                                       // End of Read File Error

	
    index = 0;
    while(isEnd == false && index < MAX_LIST_SIZE + 1)
    {
        fileReadStatus = fscanf(rsvdWordsFile,"%s",wordsToRemove[index++]);
        isEnd = (fileReadStatus == EOF);                    // To avoid negative logic
    }                                                       // End of file-to-array
    wrdCnt = --index;

    fclose(mainWordsFile);
	fclose(rsvdWordsFile);


    /* ROCK SORTING  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(isSwapped)
    {
        isSwapped = false;
        index--;

        for(short int i = 0; i < index; i++)
        {
            alphaStatus = strcmp(wordsToRemove[i], wordsToRemove[i+1]);
            if(alphaStatus > 0)
            {
                strcpy(cString, wordsToRemove[i]);          // cString as a placeholder
                strcpy(wordsToRemove[i], wordsToRemove[i+1]);
                strcpy(wordsToRemove[i+1], cString);
                isSwapped = true;
            }                                               // End of position swap
        }                                                   // End of a single iteration
    }                                                       // End of sorting


    /* PRINT SORTED LIST * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    printf("\nThe sorted list of %hd words..\n",wrdCnt);
    for(short int i = 0; i < wrdCnt; i++)
    {
        printf("   %s\n",wordsToRemove[i]);
    }                                                       // End of print array


    /* PROMPTING * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    printf("Enter the word you would like to search for:  ");
    scanf("%s", cString);                                   // cString as user input
    while(getchar() != '\n');


    /* INPUT VALIDATION  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    index = 0;
    isEnd = false;
    while (isEnd == false)
    {
        isEnd = (cString[index] == '\n');                   // To avoid negative logic
        cString[index] = tolower(cString[index++]);
    }                                                       // End of case change loop

    if(debug)
    {
        printf("\n\tDEBUG: validated search word is \"%s\"\n", cString);
    }                                                       // End of DEBUG statement


    /* BINARY SEARCH * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    end = wrdCnt - 1;
    range = end - front;
    mid = front + (range / 2);

    while(isFullySrched == false && position == NOT_FOUND)
    {
        if(debug)
        {
            printf("\tDEBUG: IN LOOP: str:%hd, mid:%hd, end:%hd\n", front, mid, end);
        }                                                   // End of DEBUG statement

        matchStatus = strcmp(cString, wordsToRemove[front]);
        if(matchStatus == 0)
        {
            position = front;
        }                                                   // End of 1st word check

        matchStatus = strcmp(cString, wordsToRemove[end]);
        if(matchStatus == 0)
        {
            position = end;
        }                                                   // End of last word check

        matchStatus = strcmp(cString, wordsToRemove[mid]);
        if(matchStatus == 0)
        {
            position = mid;
        }                                                   // If word is 'found'
            else if(matchStatus < 0)
            {
                end = mid - 1;
            }                                               // If word is 'earlier'
                else
                {
                    front = mid + 1;
                }                                           // If word is 'later'

        range = end - front;
        mid = front + (range / 2);
        isFullySrched = range < 1;
    }                                                       // End of Binary Search


    /* REPORTING * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    if(position == NOT_FOUND)
    {
        printf("\"%s\" was not found...\n",cString);
    }                                                       // Reporting: word NOT found.
        else
        {
            printf("\"%s\" is word #%hd of %hd.\n",cString, position + 1, wrdCnt);
        }                                                   // Reporting: word WAS found.


EOP:
    return (rtnCode);
}                                                           // end of main



/********************************************************************************************/
/*										    FUNCTIONS
/********************************************************************************************/

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Purpose:  Clear the screen in either Windows or Unix
 * Function Name:  clearScreen
 * Invocation:  clearScreen()
 * Variables: None
 * Functions called:  None
 * Written by Doc G.
 * Date: 2/19/23
 * Modifications:   Added flower box 2/17/25
 *                  Added "smart" include 5/28/25
 *                  stdlib is need for the WIN32 clear screen too
 *                      so preprocessor IF cmd changed to accomodate.
 *                      -- Brandon Crenshaw 06/27/25
 *
 * Special Notes:  This code will include the correct library
 *                 for either Windows or Unix.  See #ifdef
 *                 above the flower box.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
void clearScreen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}