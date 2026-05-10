#ifndef EXAMMANAGER_H
#define EXAMMANAGER_H

#include <iostream>
#include <string>

using namespace std;

struct QuestionNode {
    string questionText;
    string options;
    QuestionNode* next;
    QuestionNode* prev;

    QuestionNode() : next(NULL), prev(NULL) {}
};

struct DifficultyNode {
    string levelName;
    QuestionNode* head; 
    DifficultyNode* left;
    DifficultyNode* right;

    DifficultyNode(string name) : levelName(name), head(NULL), left(NULL), right(NULL) {}
};

class ExamManager {
public:
    DifficultyNode* root; //Medium Node
    DifficultyNode* easyNode; //Left child is Easy node
    DifficultyNode* hardNode;//Right child is Hard node

    ExamManager();
    void addQuestion(QuestionNode*& head, string qText, string opts);
    void populateList(DifficultyNode* diffNode, string filename);
    void printList(DifficultyNode* diffNode);
};

#endif
