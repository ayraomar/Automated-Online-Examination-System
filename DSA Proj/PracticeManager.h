#ifndef PRACTICEMANAGER_H
#define PRACTICEMANAGER_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// ─────────────────────────────────────────────────────────────
//  PracticeNode  --  a single practice question stored in a DLL
//  Separate from QuestionNode so practice data never mixes
//  with exam data.
// ─────────────────────────────────────────────────────────────
struct PracticeNode {
    string       questionText;
    string       options;       // formatted: "A) .. B) .. C) .. D) .."
    char         correctAnswer;
    PracticeNode* next;
    PracticeNode* prev;

    PracticeNode()
        : correctAnswer(' '), next(nullptr), prev(nullptr) {}
};

// ─────────────────────────────────────────────────────────────
//  PracticeManager  --  loads Practice_Questions.txt into a DLL
//  and provides keyword search (linear search) over it
// ─────────────────────────────────────────────────────────────
class PracticeManager {
public:
    PracticeNode* head;
    PracticeNode* tail;
    int           totalQuestions;

    PracticeManager();
    ~PracticeManager();

    // Load questions from file into the DLL
    void loadFromFile(const string& filename);

    // Linear search -- finds all questions containing keyword
    // prints results to console, returns count of matches
    int searchByKeyword(const string& keyword) const;

    // Browse all practice questions one by one
    void browseAll() const;

private:
    void append(const string& qText, const string& opts, char key);

    // Case-insensitive check if haystack contains needle
    bool containsIgnoreCase(const string& haystack,
                            const string& needle) const;
};

#endif