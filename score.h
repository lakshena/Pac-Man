#ifndef SCORE_H
#define SCORE_H

#define MAX_NAME_LENGTH 30
#define MAX_SCORES 10
#define SCORE_FILE "scores.txt"

/* Node for the sorted linked list */
typedef struct ScoreNode {
    char name[MAX_NAME_LENGTH];
    int score;
    struct ScoreNode *next;
} ScoreNode;

/* Load scores from scores.txt */
ScoreNode *loadScores(void);

/* Insert a new score in descending order */
ScoreNode *insertScore(ScoreNode *head, const char *name, int score);

/* Save scores to scores.txt */
void saveScores(ScoreNode *head);

/* Display the high-score table */
void displayScores(ScoreNode *head);

/* Free all nodes in the linked list */
void freeScores(ScoreNode *head);

/* Get the number of scores in the list */
int getScoreCount(ScoreNode *head);

#endif
