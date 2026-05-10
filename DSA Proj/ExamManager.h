#ifndef EXAMMANAGER_H
#define EXAMMANAGER_H

#include <iostream>
#include <string>

using namespace std;

struct QuestionNode {
    string questionText;
    string options;
    char correctAnswer;  // Added for Ayra's grading logic
    char userSelection;  // Added for Aleeza's review logic
    QuestionNode* next;
    QuestionNode* prev;

    QuestionNode() : next(NULL), prev(NULL), correctAnswer(' '), userSelection(' ') {}
};

struct DifficultyNode {
    string levelName;
    QuestionNode* head; 
    DifficultyNode* left;
    DifficultyNode* right;
    DifficultyNode* parent; // Added for Ayra's getNextTier logic

    DifficultyNode(string name) : levelName(name), head(NULL), left(NULL), right(NULL), parent(NULL) {}
};

class ExamManager {
public:
    DifficultyNode* root;
    DifficultyNode* easyNode;
    DifficultyNode* hardNode;

    ExamManager();
    void addQuestion(QuestionNode*& head, string qText, string opts , char key);
    void populateList(DifficultyNode* diffNode, string filename);
    void printList(DifficultyNode* diffNode);
};

#endif