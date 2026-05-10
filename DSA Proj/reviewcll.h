#ifndef REVIEWCLL_H
#define REVIEWCLL_H

#include "ExamManager.h"
#include <iostream>
using namespace std;

// ─────────────────────────────────────────────────────────────
//  AttemptedNode  —  one entry in the AttemptedList CLL
//  Stores a pointer to the question and the answer given
// ─────────────────────────────────────────────────────────────
struct AttemptedNode {
    QuestionNode*  qPtr;
    char           answer;      // 'A' | 'B' | 'C' | 'D'
    AttemptedNode* next;        // circular: tail->next == head

    AttemptedNode(QuestionNode* q, char ans)
        : qPtr(q), answer(ans), next(nullptr) {}
};

// ─────────────────────────────────────────────────────────────
//  AttemptedList  —  CLL that records every answered question
//  for final grading. One entry per question, added only when
//  the user submits an answer (not on skip).
//  Uses questionText as the unique identifier (no index field).
// ─────────────────────────────────────────────────────────────
class AttemptedList {
public:
    AttemptedNode* head;
    AttemptedNode* tail;
    int            total;

    AttemptedList() : head(nullptr), tail(nullptr), total(0) {}

    // Find an existing entry by questionText
    AttemptedNode* find(const string& qText) const;

    // Record a newly answered question (ignores duplicates)
    void record(QuestionNode* q, char ans);

    // Check if a question has been answered
    bool isAttempted(const string& qText) const;

    // Get stored answer ('\0' if not answered)
    char getAnswer(const string& qText) const;

    // Print all answered questions in CLL order
    void printAll() const;

    ~AttemptedList();
};

// ─────────────────────────────────────────────────────────────
//  SkippedNode  —  one entry in the SkippedList CLL
// ─────────────────────────────────────────────────────────────
struct SkippedNode {
    QuestionNode* qPtr;
    SkippedNode*  next;
    SkippedNode(QuestionNode* q) : qPtr(q), next(nullptr) {}
};

// ─────────────────────────────────────────────────────────────
//  SkippedList  —  CLL of questions skipped during the exam.
//  Surfaced in Review Mode after 15 questions are attempted.
//  When answered in Review Mode: remove from here, add to
//  AttemptedList.
//  Uses questionText as the unique identifier (no index field).
// ─────────────────────────────────────────────────────────────
class SkippedList {
public:
    SkippedNode*  head;
    SkippedNode*  tail;
    SkippedNode*  cursor;   // current position for cyclic traversal
    int           count;

    SkippedList() : head(nullptr), tail(nullptr), cursor(nullptr), count(0) {}

    void          add(QuestionNode* q);
    bool          contains(const string& qText) const;
    QuestionNode* getNext();
    void          remove(const string& qText);
    bool          isEmpty() const;

    ~SkippedList();
};

// ─────────────────────────────────────────────────────────────
//  ReviewCLL  —  your original CLL, physical store for nodes
//  that have left the active exam DLL (answered questions).
// ─────────────────────────────────────────────────────────────
class ReviewCLL {
public:
    QuestionNode* head;
    QuestionNode* tail;
    int           count;

    ReviewCLL();
    void addToReview(QuestionNode* q);
    void nextReviewQuestion(QuestionNode*& currentReviewPtr);
    void displayReviewStatus();
};

// ─────────────────────────────────────────────────────────────
//  Standalone helper — safely unlinks a node from the exam DLL
// ─────────────────────────────────────────────────────────────
void unlinkFromExam(QuestionNode*& tierHead, QuestionNode* nodeToDelete);

#endif