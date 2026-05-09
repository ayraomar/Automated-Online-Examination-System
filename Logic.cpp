#include "ExamSystem.h"

BSTNode* setupDifficultyBST() {
    BSTNode* medium = new BSTNode{"Medium", nullptr, nullptr, nullptr};
    BSTNode* easy = new BSTNode{"Easy", nullptr, nullptr, nullptr};
    BSTNode* hard = new BSTNode{"Hard", nullptr, nullptr, nullptr};

    medium->left = easy;
    medium->right = hard;

    return medium; 
}

BSTNode* getNextTier(BSTNode* currentTier, bool wasCorrect) {
    if (wasCorrect && currentTier->right != nullptr) {
        return currentTier->right; 
    } 
    if (!wasCorrect && currentTier->left != nullptr) {
        return currentTier->left;  
    }
    return currentTier; 
}

void deleteController(BSTNode* root) {
    if (root == nullptr) return;
    deleteController(root->left);
    deleteController(root->right);
    delete root;
}
