#include "ExamSystem.h"

QuestionNode* createNode(int id, string text) {
    QuestionNode* newNode = new QuestionNode();
    newNode->id = id;
    newNode->questionText = text;
    newNode->left = newNode->right = newNode->parent = nullptr;
    newNode->next = newNode->prev = nullptr;
    return newNode;
}

QuestionNode* insert(QuestionNode* root, QuestionNode* parent, int id, string text) {
    if (root == nullptr) {
        QuestionNode* newNode = createNode(id, text);
        newNode->parent = parent;
        return newNode;
    }
    if (id < root->id) root->left = insert(root->left, root, id, text);
    else root->right = insert(root->right, root, id, text);
    return root;
}

QuestionNode* getNextQuestion(QuestionNode* current, bool wasCorrect) {
    if (wasCorrect && current->right != nullptr) return current->right;
    if (!wasCorrect && current->left != nullptr) return current->left;
    return nullptr; 
}

void deleteTree(QuestionNode* root) {
    if (root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}