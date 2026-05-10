#include "ExamManager.h"
#include "ExamSystem.cpp"
#include "ReviewCLL.h"
#include <iostream>

using namespace std;

// -------------------------------------------------------------
//  Display current question with its options
// -------------------------------------------------------------
void displayQuestion(QuestionNode* q, bool isSkipped = false) {
    if (!q) return;
    cout << "Q: " << q->questionText << endl;
    cout << "   " << q->options << endl;
    if (isSkipped)
        cout << "   [Previously Skipped]\n";
}

// -------------------------------------------------------------
//  Review Mode - cycles through SkippedList, user answers each
//  Answered questions move: SkippedList -> AttemptedList
// -------------------------------------------------------------
void runReviewMode(SkippedList& skippedList, AttemptedList& attemptedList) {
    if (skippedList.isEmpty()) {
        cout << "\n[No skipped questions to review]\n";
        return;
    }

    cout << "\n==============================================";
    cout << "\n        REVIEW MODE - SKIPPED QUESTIONS";
    cout << "\n==============================================";
    cout << "\nYou have " << skippedList.count << " skipped question(s) to answer.\n";

    while (!skippedList.isEmpty()) {
        QuestionNode* q = skippedList.getNext();
        if (!q) break;

        cout << "\n----------------------------------------------";
        cout << "\nSkipped Questions Remaining: " << skippedList.count;
        cout << "\n----------------------------------------------\n";
        displayQuestion(q, true);

        char input;
        cout << "\nAnswer (A/B/C/D) or 'S' to Submit now: ";
        cin  >> input;
        cin.ignore(1000, '\n');
        input = toupper(input);

        if (input == 'S') {
            cout << "\n[Exiting Review Mode - remaining skipped questions marked wrong]\n";
            break;
        }

        if (input >= 'A' && input <= 'D') {
            bool isCorrect = (input == q->correctAnswer);
            if (isCorrect) cout << "\n>>> [CORRECT!]\n";
            else           cout << "\n>>> [INCORRECT!] The right answer was: "
                                << q->correctAnswer << "\n";

            // Move: SkippedList -> AttemptedList
            skippedList.remove(q->questionText);
            attemptedList.record(q, input);
        } else {
            cout << "[Invalid input - enter A, B, C, or D]\n";
        }
    }
}

// -------------------------------------------------------------
//  End-of-exam summary
// -------------------------------------------------------------
void showSummary(const AttemptedList& attemptedList, int skippedRemaining) {
    cout << "\n==============================================";
    cout << "\n                EXAM SUMMARY";
    cout << "\n==============================================\n";
    cout << "\n  -- Answered Questions --\n";
    attemptedList.printAll();
    if (skippedRemaining > 0)
        cout << "\n  -- " << skippedRemaining
             << " skipped question(s) left unanswered -> marked wrong --\n";
    cout << "\n  Total Answered: " << attemptedList.total << "\n";
}

// -------------------------------------------------------------
//  MAIN
// -------------------------------------------------------------
int main() {
    ExamManager   manager;
    ReviewCLL     reviewSystem;   // physical store for nodes leaving the DLL
    AttemptedList attemptedList;  // silent record of all answered questions
    SkippedList   skippedList;    // active skip queue for Review Mode

    // Wire parent pointers for BST traversal
    manager.easyNode->parent = manager.root;
    manager.hardNode->parent = manager.root;

    DifficultyNode* currentTier = manager.root;   // start at Medium
    QuestionNode*   currentQ    = currentTier->head;

    int questionsAttempted         = 0;   // answered + skipped (max 15)
    int questionsAnsweredSinceSkip = 0;   // gate: must answer 2 before skipping
    const int MAX_QUESTIONS        = 15;

    cout << "--- Starting Adaptive Exam Test Session ---\n";
    cout << "Controls: A/B/C/D = Answer | K = Skip | N = Next | P = Prev | X = Exit\n";
    cout << "(You must answer 2 questions before you can skip)\n";

    // -- MAIN EXAM LOOP ----------------------------------------
    while (currentQ != NULL && questionsAttempted < MAX_QUESTIONS) {

        cout << "\n==============================================";
        cout << "\nCURRENT LEVEL    : " << currentTier->levelName;
        cout << "\nQUESTIONS DONE   : " << questionsAttempted << " / " << MAX_QUESTIONS;
        cout << "\nSKIPPED SO FAR   : " << skippedList.count;
        cout << "\nANSWERS TILL SKIP: " << questionsAnsweredSinceSkip << " / 2";
        cout << "\n==============================================\n";

        displayQuestion(currentQ);

        char input;
        cout << "\nYour input (A/B/C/D, K=Skip, N=Next, P=Prev, X=Exit): ";
        cin  >> input;
        cin.ignore(1000, '\n');
        input = toupper(input);

        if (input == 'X') break;

        // -- ANSWER -------------------------------------------
        if (input == 'A' || input == 'B' || input == 'C' || input == 'D') {

            bool isCorrect = (input == currentQ->correctAnswer);
            if (isCorrect) cout << "\n>>> [CORRECT!]\n";
            else           cout << "\n>>> [INCORRECT!] The right answer was: "
                                << currentQ->correctAnswer << "\n";

            questionsAttempted++;
            questionsAnsweredSinceSkip++;

            // Record in AttemptedList
            attemptedList.record(currentQ, input);

            // Adaptive tier logic
            DifficultyNode* nextTier   = getNextTier(currentTier, isCorrect);
            QuestionNode*   nodeToMove = currentQ;
            DifficultyNode* sourceTier = currentTier;

            if (nextTier != currentTier) {
                cout << "\n*** STREAK TRIGGERED: MOVING TO "
                     << nextTier->levelName << " ***\n";
                currentTier = nextTier;
                currentQ    = currentTier->head;
            } else {
                if      (currentQ->next) currentQ = currentQ->next;
                else if (currentQ->prev) currentQ = currentQ->prev;
                else                     currentQ = NULL;
            }

            unlinkFromExam(sourceTier->head, nodeToMove);
            reviewSystem.addToReview(nodeToMove);

            if (currentQ == nodeToMove)
                currentQ = currentTier->head;
        }

        // -- SKIP ---------------------------------------------
        else if (input == 'K') {
            if (questionsAnsweredSinceSkip < 2) {
                cout << "\n[You must answer "
                     << (2 - questionsAnsweredSinceSkip)
                     << " more question(s) before you can skip]\n";
            } else {
                skippedList.add(currentQ);
                questionsAnsweredSinceSkip = 0;  // reset gate
                questionsAttempted++;             // counts toward 15

                QuestionNode*   nodeToSkip = currentQ;
                DifficultyNode* sourceTier = currentTier;

                // Advance pointer before unlinking
                if      (currentQ->next) currentQ = currentQ->next;
                else if (currentQ->prev) currentQ = currentQ->prev;
                else                     currentQ = NULL;

                unlinkFromExam(sourceTier->head, nodeToSkip);
                // Skipped node lives only in SkippedList - not in reviewSystem

                if (currentQ == nodeToSkip)
                    currentQ = currentTier->head;

                cout << "\n[Question skipped - answer it in Review Mode after the exam]\n";
            }
        }

        // -- NAVIGATION ---------------------------------------
        else if (input == 'N') {
            if (currentQ->next) currentQ = currentQ->next;
            else cout << "[End of current tier list]\n";
        }
        else if (input == 'P') {
            if (currentQ->prev) currentQ = currentQ->prev;
            else cout << "[At the start of current tier list]\n";
        }
        else {
            cout << "[Invalid input - use A/B/C/D, K, N, P, or X]\n";
        }
    }

    // -- POST-EXAM ---------------------------------------------
    cout << "\n\n--- Exam Phase Complete ---";
    cout << "\nAnswered : " << attemptedList.total;
    cout << "\nSkipped  : " << skippedList.count << "\n";

    if (!skippedList.isEmpty()) {
        cout << "\nYou have " << skippedList.count << " skipped question(s).\n";
        cout << "[R] Review & Answer Skipped  [S] Submit Now: ";
        char choice;
        cin  >> choice;
        cin.ignore(1000, '\n');

        if (toupper(choice) == 'R') {
            runReviewMode(skippedList, attemptedList);
        } else {
            cout << "\n[Skipped questions will be marked wrong]\n";
        }
    }

    showSummary(attemptedList, skippedList.count);

    cout << "\nAdaptive Exam Session Complete.\n";
    return 0;
}