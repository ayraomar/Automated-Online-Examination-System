#include "ReviewCLL.h"
#include <iostream>
using namespace std;

// =============================================================
//  AttemptedList
// =============================================================

AttemptedNode* AttemptedList::find(const string& qText) const {
    if (!head) return nullptr;
    AttemptedNode* cur = head;
    do {
        if (cur->qPtr->questionText == qText) return cur;
        cur = cur->next;
    } while (cur != head);
    return nullptr;
}

void AttemptedList::record(QuestionNode* q, char ans) {
    // No duplicates - each question recorded exactly once
    if (find(q->questionText)) {
        cout << "  [Already recorded - duplicate ignored]\n";
        return;
    }

    AttemptedNode* node = new AttemptedNode(q, ans);
    if (!head) {
        head       = node;
        tail       = node;
        node->next = head;
    } else {
        tail->next = node;
        node->next = head;
        tail       = node;
    }
    total++;
    cout << "  [Recorded: " << ans << "  |  " << total << " answered so far]\n";
}

bool AttemptedList::isAttempted(const string& qText) const {
    return find(qText) != nullptr;
}

char AttemptedList::getAnswer(const string& qText) const {
    AttemptedNode* node = find(qText);
    return node ? node->answer : '\0';
}

void AttemptedList::printAll() const {
    if (!head) { cout << "  (no questions answered)\n"; return; }
    AttemptedNode* cur = head;
    int i = 1;
    do {
        cout << "  " << i++ << ". "
             << cur->qPtr->questionText
             << "  ->  " << cur->answer << "\n";
        cur = cur->next;
    } while (cur != head);
}

AttemptedList::~AttemptedList() {
    if (!head) return;
    tail->next = nullptr;       // break circle before deleting
    AttemptedNode* cur = head;
    while (cur) {
        AttemptedNode* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
}

// =============================================================
//  SkippedList
// =============================================================

void SkippedList::add(QuestionNode* q) {
    SkippedNode* node = new SkippedNode(q);
    if (!head) {
        head       = tail = node;
        node->next = head;
        cursor     = head;
    } else {
        tail->next = node;
        node->next = head;
        tail       = node;
    }
    count++;
    cout << "  [Skipped - " << count << " skipped question(s) total]\n";
}

bool SkippedList::contains(const string& qText) const {
    if (!head) return false;
    SkippedNode* cur = head;
    do {
        if (cur->qPtr->questionText == qText) return true;
        cur = cur->next;
    } while (cur != head);
    return false;
}

QuestionNode* SkippedList::getNext() {
    if (!cursor) return nullptr;
    QuestionNode* q = cursor->qPtr;
    cursor = cursor->next;
    return q;
}

void SkippedList::remove(const string& qText) {
    if (!head) return;

    if (count == 1) {
        if (head->qPtr->questionText == qText) {
            delete head;
            head = tail = cursor = nullptr;
            count = 0;
        }
        return;
    }

    SkippedNode* prev = tail;
    SkippedNode* cur  = head;
    do {
        if (cur->qPtr->questionText == qText) {
            prev->next = cur->next;
            if (cur == head)   head   = cur->next;
            if (cur == tail)   tail   = prev;
            if (cursor == cur) cursor = cur->next;
            delete cur;
            count--;
            return;
        }
        prev = cur;
        cur  = cur->next;
    } while (cur != head);
}

bool SkippedList::isEmpty() const { return head == nullptr; }

SkippedList::~SkippedList() {
    if (!head) return;
    tail->next = nullptr;
    SkippedNode* cur = head;
    while (cur) {
        SkippedNode* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
}

// =============================================================
//  ReviewCLL  (your original - behaviour unchanged)
// =============================================================

ReviewCLL::ReviewCLL() : head(NULL), tail(NULL), count(0) {}

void ReviewCLL::addToReview(QuestionNode* q) {
    if (!q) return;

    // Isolate from previous DLL connections before inserting
    q->next = NULL;
    q->prev = NULL;

    if (head == NULL) {
        head    = q;
        tail    = q;
        q->next = head;
    } else {
        tail->next = q;
        q->next    = head;
        tail       = q;
    }
    count++;
}

void ReviewCLL::nextReviewQuestion(QuestionNode*& currentReviewPtr) {
    if (currentReviewPtr != NULL) {
        currentReviewPtr = currentReviewPtr->next;
    }
}

void ReviewCLL::displayReviewStatus() {
    if (!head) { cout << "  [ReviewCLL is empty]\n"; return; }
    cout << "  [ReviewCLL contains " << count << " node(s)]\n";
    QuestionNode* cur = head;
    int i = 1;
    do {
        cout << "  " << i++ << ". " << cur->questionText << "\n";
        cur = cur->next;
    } while (cur != head);
}

// =============================================================
//  unlinkFromExam
// =============================================================

void unlinkFromExam(QuestionNode*& tierHead, QuestionNode* nodeToDelete) {
    if (!tierHead || !nodeToDelete) return;

    if (tierHead == nodeToDelete)
        tierHead = nodeToDelete->next;

    if (nodeToDelete->next != NULL)
        nodeToDelete->next->prev = nodeToDelete->prev;

    if (nodeToDelete->prev != NULL)
        nodeToDelete->prev->next = nodeToDelete->next;

    // Fully isolate - no stale DLL pointers
    nodeToDelete->next = NULL;
    nodeToDelete->prev = NULL;
}