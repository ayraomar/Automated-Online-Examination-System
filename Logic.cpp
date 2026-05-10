#include "ExamSystem.h"

BSTNode* setupDifficultyBST() {
    BSTNode* medium = new BSTNode{"Medium", nullptr, nullptr, nullptr, nullptr};
    BSTNode* easy = new BSTNode{"Easy", nullptr, nullptr, nullptr, medium};
    BSTNode* hard = new BSTNode{"Hard", nullptr, nullptr, nullptr, medium};

    medium->left = easy;
    medium->right = hard;

    return medium; 
}

int correctStreak = 0;
int incorrectStreak = 0;

BSTNode* getNextTier(BSTNode* currentTier, bool wasCorrect) {
    if (wasCorrect) {
        correctStreak++;
        incorrectStreak = 0;
    } else {
        incorrectStreak++;
        correctStreak = 0;
    }

    if (correctStreak == 2) {
        correctStreak = 0;
        if (currentTier->difficultyLevel == "Medium" && currentTier->right != nullptr) {
            return currentTier->right;
        }
        if (currentTier->difficultyLevel == "Easy" && currentTier->parent != nullptr) {
            return currentTier->parent; 
        }
    }

    if (incorrectStreak == 2) {
        incorrectStreak = 0;
        if (currentTier->difficultyLevel == "Medium" && currentTier->left != nullptr) {
            return currentTier->left;
        }
        if (currentTier->difficultyLevel == "Hard" && currentTier->parent != nullptr) {
            return currentTier->parent;
        }
    }

    return currentTier;
}


void deleteController(BSTNode* root) {
    if (root == nullptr) return;
    deleteController(root->left);
    deleteController(root->right);
    delete root;
}
