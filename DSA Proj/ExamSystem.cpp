#include "ExamSystem.h"

// BSTNode* setupDifficultyBST() {
//     BSTNode* medium = new BSTNode{"Medium", nullptr, nullptr, nullptr, nullptr};
//     BSTNode* easy = new BSTNode{"Easy", nullptr, nullptr, nullptr, medium};
//     BSTNode* hard = new BSTNode{"Hard", nullptr, nullptr, nullptr, medium};

//     medium->left = easy;
//     medium->right = hard;

//     return medium; 
// }

int correctStreak = 0;
int incorrectStreak = 0;

DifficultyNode* getNextTier(DifficultyNode* currentTier, bool wasCorrect) {
    if (wasCorrect) {
        correctStreak++;
        incorrectStreak = 0;
    } else {
        incorrectStreak++;
        correctStreak = 0;
    }

    // Logic to move UP (Easy -> Medium -> Hard)
    if (correctStreak == 2) {
        correctStreak = 0;
        // If Medium, move to Hard (Right Child)
        if (currentTier->levelName == "Medium" && currentTier->right != nullptr) {
            return (DifficultyNode*)currentTier->right;
        }
        // If Easy, move back to Medium (Parent)
        if (currentTier->levelName == "Easy" && currentTier->parent != nullptr) {
            return (DifficultyNode*)currentTier->parent; 
        }
    }

    // Logic to move DOWN (Hard -> Medium -> Easy)
    if (incorrectStreak == 2) {
        incorrectStreak = 0;
        // If Medium, move to Easy (Left Child)
        if (currentTier->levelName == "Medium" && currentTier->left != nullptr) {
            return (DifficultyNode*)currentTier->left;
        }
        // If Hard, move back to Medium (Parent)
        if (currentTier->levelName == "Hard" && currentTier->parent != nullptr) {
            return (DifficultyNode*)currentTier->parent;
        }
    }

    return currentTier; // Stay in the same tier if no streak is hit
}

// void deleteController(DifficultyNode* root) {
//     if (root == nullptr) return;
//     deleteController(root->left);
//     deleteController(root->right);
//     delete root;
// }