#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
#include <conio.h>
#endif

#define WRD_SIZE_LIMIT 20


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
 * Variables:       const char MAIN_WRDS_FILE[]     - CONSTANT; filename of file with words to be parsed
 *                  const char RSVD_WRDS_FILE[]     - CONSTANT; filename of file with words to skip
 *                  const char END_OF_STR           - CONSTANT; the null character used to terminate a char string
 *
 *                  const short int MAX_LIST_SIZE   - CONSTANT; max number of words from file
 *                  const short int NOT_FOUND       - CONSTANT; indicates that a word is not listed
 *                  const short int DIVISOR         - CONSTANT; used to divide search range in half
 *                  const short int READERR         - CONSTANT; error code for file open error
 *                  const short int NOERR           - CONSTANT; code for no error
 *
 *                  char wordsToRemove[][]      - 2D array for holding a list of strings
 *                  char cString[]              - Holds string values
 *                  char lowerChar              - lower-case version of 'readChar'
 *                  char readChar               - character read from file (as is)
 *
 *                  short int fileReadStatus    - return for fscanf(). Used to determine EOF
 *                  short int alphaStatus       - (+) 1st string = later; (-) 1st string = earlier; a zero value indicates strings are equal
 *                  short int matchStatus       - Used during search to indicate where in the list a string may be
 *                  short int position          - Indicates where in 'wordsToRemove' a word is located.
 *                  short int rtnCode           - The current error status of main()
 *                  short int wrdCnt            - The number of words found in the file
 *                  short int index             - Used to index 'wordsToRemove' during operations
 *                  short int range             - The width of the search range
 *                  short int front             - Where the seearch range begins
 *                  short int end               - Where the seearch range ends
 *                  short int mid               - The midpoint of the search range
 *
 *                  bool isFullySrched          - Indicates the whole word list was processed.
 *                  bool isDelimiter            - Indicates if read character is whitespace
 *                  bool isSkippable            - Indicates if read character is punctuation or number
 *                  bool isSwapped              - Indicates if array sorting process is finished or not
 *                  bool isValid                - Indicates if read character is a letter
 *                  bool isEnd                  - Indicates the end of file/string was reached
 *                  bool debug                  - Used to enable print statements for debug
 *
 *                  FILE *mainWordsFile         - Pointer to the file holding the unsorted words
 *                  FILE *rsvdWordsFile         - Pointer to the file holding the unsorted words
 *
 * Functions:       clearScreen()               - Cross-platform clear screen from professor
 *                  strcmp()                    - Determines the alphabetic order of c-strings
 *                  strcpy()                    - Used to copy c-strings
 *
 * Written by:      Brandon Crenshaw  7/31/2025
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
    const char END_OF_STR = 0x0;

    const short int MAX_LIST_SIZE = 50;
    const short int NOT_FOUND = -1;
    const short int DIVISOR =  2;
    const short int READERR = 10;
    const short int NOERR   =  0;


    /*VARAIABLES*/
    char wordsToRemove[MAX_LIST_SIZE + 1][WRD_SIZE_LIMIT + 1];
    char cString[WRD_SIZE_LIMIT + 1] = {END_OF_STR};
    char lowerChar = END_OF_STR;
    char readChar = END_OF_STR;

    short int position = NOT_FOUND;
    short int fileReadStatus = 0;
    short int alphaStatus = 0;
    short int matchStatus = 0;
    short int rtnCode = NOERR;
    short int wrdCnt = 0;
    short int front = 0;
    short int index = 0;
    short int range = 0;
    short int end = 0;
    short int mid = 0;

    bool isFullySrched = false;
    bool isDelimiter = false;
    bool isSkippable = false;
    bool isSwapped   = true;
    bool isValid = false;
    bool isEnd = false;
    bool debug = false;
        // bool debug = true;

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
    }                                                       // End of Read Main File Error

    if(rsvdWordsFile == NULL)
    {
        printf("\tERROR: '%s' failed to open.\n",RSVD_WRDS_FILE);
        rtnCode = READERR;
        goto EOP;
    }                                                       // End of Read RSVD words File Error

    index = 0;
    while(isEnd == false && index < MAX_LIST_SIZE + 1)
    {
        fileReadStatus = fscanf(rsvdWordsFile,"%s",wordsToRemove[index++]);
        isEnd = (fileReadStatus == EOF);                    // To avoid negative logic
    }                                                       // End of file-to-array

    wrdCnt = --index;
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


    /* PARSE WORDS * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    do
    {

NEXT:
        readChar = fgetc(mainWordsFile);
        lowerChar = tolower(readChar);

        isEnd = (lowerChar == EOF);
        isDelimiter = isspace(lowerChar);
        isValid = isalpha(lowerChar);
        isSkippable = ispunct(lowerChar);

        if(debug)
        {
            printf("[%c]",lowerChar);
            if(isDelimiter)
            {
                printf("\n");
            }                                               // close if in debug: newline for new word
        }                                                   // close debug statement

        if(isValid)
        {
            cString[index++] = lowerChar;
        }                                                   // close isValid condition

        if(isSkippable)
        {
            goto NEXT;
        }                                                   // close isSkippable (punctuation) condition

        if(isDelimiter)
        {
            if(index == 0)
            {
                goto NEXT;
            }                                               // closes repeated delimiters condition

            cString[index] = END_OF_STR;


            /* BINARY SEARCH * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
            front = 0;
            end = wrdCnt - 1;
            range = end - front;
            mid = front + (range / DIVISOR);
            isFullySrched = false;
            position = NOT_FOUND;

            if(debug)
            {
                printf("\tDEBUG: IN LOOP: str:%hd, mid:%hd, end:%hd, position:%hd\n", front, mid, end, position);
            }                                               // End of DEBUG statement

            while(isFullySrched == false && position == NOT_FOUND)
            {

                matchStatus = strcmp(cString, wordsToRemove[front]);
                if(matchStatus == 0)
                {
                    position = front;
                }                                           // End of 1st word check

                matchStatus = strcmp(cString, wordsToRemove[end]);
                if(matchStatus == 0)
                {
                    position = end;
                }                                           // End of last word check

                matchStatus = strcmp(cString, wordsToRemove[mid]);
                if(matchStatus == 0)
                {
                    position = mid;
                }                                           // If word is 'found'
                    else if(matchStatus < 0)
                    {
                        end = mid - 1;
                    }                                       // If word is 'earlier'
                        else
                        {
                            front = mid + 1;
                        }                                   // If word is 'later'

                range = end - front;
                mid = front + (range / DIVISOR);
                isFullySrched = range < 1;

                if(debug)
                {
                    printf("\tDEBUG: IN LOOP: str:%hd, mid:%hd, end:%hd, position:%hd\n", front, mid, end, position);
                }
            }                                               // End of Binary Search

            /* END OF BINARY SEARCH - RETURN TO PARSE CODE * * * * * * * * * * * * * * * * * * */

            if(position == NOT_FOUND)
            {
                printf("%s\n",cString);
            }

            if(debug)
            {
                printf("\n");
            }                                               // close debug statement

            index = 0;
            for(short int i = 0; i < (WRD_SIZE_LIMIT + 1); i++)
            {
                cString[i] = END_OF_STR;
            }                                               // close add end of string char code
        }                                                   // close isDelimitter condition

        if(isEnd && position == NOT_FOUND)
        {
            cString[index] = END_OF_STR;
            printf("%s\n",cString);
        }                                                   // End of isEnd condition
    }                                                       // End of do-while block
    while(isEnd == false);                                  // do-while conditional

    fclose(mainWordsFile);

EOP:
    return (rtnCode);
}                                                           // end of main



/********************************************************************************************/
/*                                          FUNCTIONS                                       */
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