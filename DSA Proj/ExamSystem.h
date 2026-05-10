#ifndef EXAMSYSTEM_H
#define EXAMSYSTEM_H

#include "ExamManager.h"
#include <iostream>
#include <string>
using namespace std;

// The structure for individual questions in the linked lists
// struct QuestionNode {
//     int id;
//     string questionText;
//     // Linked List Pointers (For the Tiers and Review List)
//     QuestionNode* prev;
//     QuestionNode* next;
//     char correctAnswer;
//     char userSelection;
// };

// The structure for the BST Controller
// struct BSTNode {
//     string difficultyLevel; // "Easy", "Medium", "Hard"
//     QuestionNode* head;     // Pointer to the start of the linked list for this tier
//     BSTNode* left;          // Points to "Easy" tier
//     BSTNode* right;         // Points to "Hard" tier
//     BSTNode* parent;
// };

DifficultyNode* getNextTier(DifficultyNode* currentTier, bool wasCorrect);

#endif