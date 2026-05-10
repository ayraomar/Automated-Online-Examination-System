#ifndef SUBMISSIONQUEUE_H
#define SUBMISSIONQUEUE_H

#include "ReviewCLL.h"
#include <string>
using namespace std;

// ─────────────────────────────────────────────────────────────
//  ExamSubmission  —  one student's complete exam attempt
//  Created at the start of the session with name + ID,
//  responses pointer filled at the end before enqueuing.
// ─────────────────────────────────────────────────────────────
struct ExamSubmission {
    string        studentName;
    string        studentID;
    AttemptedList* responses;   // points to the session's AttemptedList
    int           totalQuestions;

    ExamSubmission()
        : studentName(""), studentID(""),
          responses(nullptr), totalQuestions(15) {}
};

// ─────────────────────────────────────────────────────────────
//  QueueNode  —  internal node for the SubmissionQueue
// ─────────────────────────────────────────────────────────────
struct QueueNode {
    ExamSubmission data;
    QueueNode*     next;

    QueueNode(ExamSubmission s) : data(s), next(nullptr) {}
};

// ─────────────────────────────────────────────────────────────
//  SubmissionQueue  —  FIFO queue of exam submissions
//  Enqueue on Submit, Dequeue to grade.
//  In a single-session run there will always be exactly one
//  item in the queue, but the structure supports multiple.
// ─────────────────────────────────────────────────────────────
class SubmissionQueue {
public:
    QueueNode* front;
    QueueNode* rear;
    int        size;

    SubmissionQueue() : front(nullptr), rear(nullptr), size(0) {}

    // Add a submission to the back of the queue
    void enqueue(ExamSubmission s);

    // Remove and return the submission at the front
    ExamSubmission dequeue();

    bool isEmpty() const;

    ~SubmissionQueue();
};

// ─────────────────────────────────────────────────────────────
//  GradingEngine  —  dequeues and grades one submission,
//  prints the result report with name + ID header
// ─────────────────────────────────────────────────────────────
void gradeSubmission(SubmissionQueue& queue, int skippedCount);

#endif