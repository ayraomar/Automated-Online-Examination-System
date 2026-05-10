#include "submission.h"
#include <iostream>
using namespace std;

// =============================================================
//  SubmissionQueue
// =============================================================

void SubmissionQueue::enqueue(ExamSubmission s) {
    QueueNode* node = new QueueNode(s);
    if (!rear) {
        front = rear = node;
    } else {
        rear->next = node;
        rear       = node;
    }
    size++;
    cout << "\n  [Submission queued for: "
         << s.studentName << " | " << s.studentID << "]\n";
}

ExamSubmission SubmissionQueue::dequeue() {
    if (!front) {
        cout << "  [Queue is empty — nothing to dequeue]\n";
        return ExamSubmission();
    }
    QueueNode*     tmp  = front;
    ExamSubmission data = tmp->data;
    front               = front->next;
    if (!front) rear    = nullptr;
    delete tmp;
    size--;
    return data;
}

bool SubmissionQueue::isEmpty() const { return front == nullptr; }

SubmissionQueue::~SubmissionQueue() {
    while (front) {
        QueueNode* tmp = front;
        front          = front->next;
        delete tmp;
    }
}

// =============================================================
//  gradeSubmission
//  Dequeues the submission, walks the AttemptedList CLL,
//  compares each answer against correctAnswer on the node,
//  and prints the full result report.
// =============================================================

void gradeSubmission(SubmissionQueue& queue, int skippedCount) {
    if (queue.isEmpty()) {
        cout << "\n[No submission found in queue]\n";
        return;
    }

    ExamSubmission submission = queue.dequeue();
    AttemptedList* responses  = submission.responses;

    if (!responses || !responses->head) {
        cout << "\n[No responses found — cannot grade]\n";
        return;
    }

    // ── Walk the AttemptedList CLL and grade ─────────────────
    int correct   = 0;
    int answered  = responses->total;
    int attempted = answered + skippedCount;  // should equal 15

    AttemptedNode* cur = responses->head;
    do {
        if (cur->answer == cur->qPtr->correctAnswer)
            correct++;
        cur = cur->next;
    } while (cur != responses->head);

    // Skipped questions count as wrong — already excluded from
    // correct count since they were never in AttemptedList
    int total   = submission.totalQuestions;
    int wrong   = total - correct;
    double pct  = (static_cast<double>(correct) / total) * 100.0;

    // ── Result Report ─────────────────────────────────────────
    cout << "\n\n==============================================";
    cout << "\n                EXAM RESULTS";
    cout << "\n==============================================";
    cout << "\n  Student  : " << submission.studentName;
    cout << "\n  ID       : " << submission.studentID;
    cout << "\n----------------------------------------------";
    cout << "\n  Correct  : " << correct  << " / " << total;
    cout << "\n  Wrong    : " << wrong    << " / " << total;
    cout << "\n  Skipped  : " << skippedCount << " (marked wrong)";
    cout << "\n  Score    : " << pct << "%";
    cout << "\n----------------------------------------------";

    // ── Performance badge ─────────────────────────────────────
    cout << "\n  Grade    : ";
    if      (pct >= 90) cout << "A  -- Excellent!";
    else if (pct >= 80) cout << "B  -- Good job!";
    else if (pct >= 70) cout << "C  -- Average";
    else if (pct >= 60) cout << "D  -- Needs improvement";
    else                cout << "F  -- Better luck next time";

    cout << "\n==============================================\n";

    // ── Per-question breakdown ────────────────────────────────
    cout << "\n  -- Answer Breakdown --\n";
    AttemptedNode* bcur = responses->head;
    int i = 1;
    do {
        char userAns    = bcur->answer;
        char correctAns = bcur->qPtr->correctAnswer;
        string result   = (userAns == correctAns) ? "[CORRECT]" : "[WRONG]  ";
        cout << "  " << i++ << ". " << result
             << "  Your answer: " << userAns
             << "  Correct: "     << correctAns
             << "\n     " << bcur->qPtr->questionText << "\n";
        bcur = bcur->next;
    } while (bcur != responses->head);

    cout << "\n==============================================\n";
}