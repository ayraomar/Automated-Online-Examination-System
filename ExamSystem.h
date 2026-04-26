#ifndef EXAMSYSTEM_H
#define EXAMSYSTEM_H
#include <iostream>
using namespace std;

struct QuestionNode {
	int id;
	string questionText;
	QuestionNode* prev;
	QuestionNode* next;
	QuestionNode* left;
	QuestionNode* right;
	char correctAnswer;
	char userSelection;
	QuestionNode* parent;
};

QuestionNode* createNode(string data) {
	return new QuestionNode(data);
}

QuestionNode* findMin(QuestionNode* root) {
	return root;
}

#endif