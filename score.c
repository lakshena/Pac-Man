#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "score.h"
/*
 * Load scores from the scores.txt file.
 *
 * Each line in the file has the format:
 * name score
 *
 * Example:
 * Rithika 5000
 * Lakshena 4000
 * Player 2500
 */
ScoreNode *loadScores(void)
{
    return loadScoresFromFile(SCORE_FILE);
}

ScoreNode *loadScoresFromFile(const char *path)
{
    FILE *file;
    ScoreNode *head = NULL;
    char name[MAX_NAME_LENGTH];
    int score;
    if (path == NULL)
    {
        fprintf(stderr, "Error: Score file path is null.\n");
        return NULL;
    }
    file = fopen(path, "r");
    /*
     * If the file does not exist, simply return
     * an empty linked list.
     */
    if (file == NULL)
    {
        return NULL;
    }
    /*
     * Read each player's name and score.
     */
    while (fscanf(file, "%29s %d", name, &score) == 2)
    {
        head = insertScore(head, name, score);
    }
    fclose(file);
    return head;
}
/*
 * Insert a score into the linked list
 * while maintaining descending order.
 */
ScoreNode *insertScore(ScoreNode *head, const char *name, int score)
{
    ScoreNode *newNode;
    ScoreNode *current;
    /*
     * Create a new node.
     */
    newNode = (ScoreNode *)malloc(sizeof(ScoreNode));
    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        return head;
    }
    /*
     * Store player's name and score.
     */
    strncpy(newNode->name, name, MAX_NAME_LENGTH - 1);
    newNode->name[MAX_NAME_LENGTH - 1] = '\0';
    newNode->score = score;
    newNode->next = NULL;
    /*
     * Case 1:
     * The list is empty.
     *
     * OR
     *
     * The new score is greater than
     * the current highest score.
     */
    if (head == NULL || score > head->score)
    {
        newNode->next = head;
        return newNode;
    }
    /*
     * Find the correct position.
     *
     * We stop when the next node has a score
     * smaller than the new score.
     */
    current = head;
    while (current->next != NULL &&
           current->next->score >= score)
    {
        current = current->next;
    }
    /*
     * Insert the new node.
     */
    newNode->next = current->next;
    current->next = newNode;
    return head;
}
/*
 * Save the linked list into scores.txt.
 *
 * Only the top MAX_SCORES scores are saved.
 */
int saveScores(ScoreNode *head)
{
    return saveScoresToFile(head, SCORE_FILE);
}

int saveScoresToFile(ScoreNode *head, const char *path)
{
    FILE *file;
    ScoreNode *current;
    int count = 0;
    int failed = 0;

    if (path == NULL)
    {
        fprintf(stderr, "Error: Score file path is null.\n");
        return -1;
    }
    file = fopen(path, "w");
    if (file == NULL)
    {
        perror("Error: Could not save scores");
        return -1;
    }
    current = head;
    while (current != NULL && count < MAX_SCORES)
    {
        if (fprintf(file, "%s %d\n", current->name, current->score) < 0)
        {
            failed = 1;
            break;
        }
        current = current->next;
        count++;
    }
    if (fclose(file) != 0)
    {
        failed = 1;
    }
    if (failed)
    {
        perror("Error: Could not finish writing scores");
        return -1;
    }
    return 0;
}
/*
 * Display the high-score table.
 */
void displayScores(ScoreNode *head)
{
    ScoreNode *current;
    int rank = 1;
    printf("\n");
    printf("=====================================\n");
    printf("           HIGH SCORES\n");
    printf("=====================================\n");
    if (head == NULL)
    {
        printf("       No scores available.\n");
        printf("=====================================\n");
        return;
    }
    current = head;
    while (current != NULL && rank <= MAX_SCORES)
    {
        printf("%2d. %-20s %6d\n",
               rank,
               current->name,
               current->score);
        current = current->next;
        rank++;
    }
    printf("=====================================\n");
}


/*
 * Return the number of nodes in the list.
 */
int getScoreCount(ScoreNode *head)
{
    int count = 0;
    ScoreNode *current = head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}


/*
 * Free all dynamically allocated nodes.
 */
void freeScores(ScoreNode *head)
{
    ScoreNode *current;
    ScoreNode *next;
    current = head;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}
