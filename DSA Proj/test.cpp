#include "ExamManager.h"
#include "ExamSystem.cpp"
#include "ReviewCLL.h"
#include "submission.h"
#include "PracticeManager.h"
#include <iostream>

using namespace std;

// ─────────────────────────────────────────────────────────────
//  Display a question during the exam
// ─────────────────────────────────────────────────────────────
void displayQuestion(QuestionNode* q, bool isSkipped = false) {
    if (!q) return;
    cout << "Q: " << q->questionText << "\n";
    cout << "   " << q->options << "\n";
    if (isSkipped)
        cout << "   [Previously Skipped]\n";
}

// ─────────────────────────────────────────────────────────────
//  Review Mode -- user answers skipped questions
// ─────────────────────────────────────────────────────────────
void runReviewMode(SkippedList& skippedList, AttemptedList& attemptedList) {
    if (skippedList.isEmpty()) {
        cout << "\n[No skipped questions to review]\n";
        return;
    }

    cout << "\n==============================================";
    cout << "\n        REVIEW MODE -- SKIPPED QUESTIONS";
    cout << "\n==============================================";
    cout << "\nYou have " << skippedList.count << " skipped question(s).\n";

    while (!skippedList.isEmpty()) {
        QuestionNode* q = skippedList.getNext();
        if (!q) break;

        cout << "\n----------------------------------------------";
        cout << "\nSkipped Remaining: " << skippedList.count;
        cout << "\n----------------------------------------------\n";
        displayQuestion(q, true);

        char input;
        cout << "\nAnswer (A/B/C/D) or 'S' to Submit now: ";
        cin  >> input;
        cin.ignore(1000, '\n');
        input = toupper(input);

        if (input == 'S') {
            cout << "\n[Remaining skipped questions will be marked wrong]\n";
            break;
        }
        if (input >= 'A' && input <= 'D') {
            bool isCorrect = (input == q->correctAnswer);
            if (isCorrect) cout << "\n>>> [CORRECT!]\n";
            else           cout << "\n>>> [INCORRECT!] Correct answer: "
                                << q->correctAnswer << "\n";
            skippedList.remove(q->questionText);
            attemptedList.record(q, input);
        } else {
            cout << "[Invalid -- enter A, B, C, or D]\n";
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  Start Quiz -- full adaptive exam session
// ─────────────────────────────────────────────────────────────
void startQuiz(const string& studentName, const string& studentID) {
    ExamManager     manager;
    ReviewCLL       reviewSystem;
    AttemptedList   attemptedList;
    SkippedList     skippedList;
    SubmissionQueue submissionQueue;

    manager.easyNode->parent = manager.root;
    manager.hardNode->parent = manager.root;

    DifficultyNode* currentTier = manager.root;
    QuestionNode*   currentQ    = currentTier->head;

    int questionsAttempted         = 0;
    int questionsAnsweredSinceSkip = 0;
    const int MAX_QUESTIONS        = 15;

    cout << "\n--- Starting Adaptive Exam ---\n";
    cout << "Controls: A/B/C/D = Answer | K = Skip | N = Next | P = Prev | X = Exit\n";
    cout << "(You must answer 2 questions before you can skip)\n";

    // ── Main Exam Loop ────────────────────────────────────────
    while (currentQ != NULL && questionsAttempted < MAX_QUESTIONS) {

        cout << "\n==============================================";
        cout << "\nCURRENT LEVEL    : " << currentTier->levelName;
        cout << "\nQUESTIONS DONE   : " << questionsAttempted << " / " << MAX_QUESTIONS;
        cout << "\nSKIPPED SO FAR   : " << skippedList.count;
        cout << "\nANSWERS TILL SKIP: " << questionsAnsweredSinceSkip << " / 2";
        cout << "\n==============================================\n";

        displayQuestion(currentQ);

        char input;
        cout << "\nYour input: ";
        cin  >> input;
        cin.ignore(1000, '\n');
        input = toupper(input);

        if (input == 'X') break;

        // Answer
        if (input == 'A' || input == 'B' || input == 'C' || input == 'D') {
            bool isCorrect = (input == currentQ->correctAnswer);
            if (isCorrect) cout << "\n>>> [CORRECT!]\n";
            else           cout << "\n>>> [INCORRECT!] Correct answer: "
                                << currentQ->correctAnswer << "\n";

            questionsAttempted++;
            questionsAnsweredSinceSkip++;
            attemptedList.record(currentQ, input);

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
        // Skip
        else if (input == 'K') {
            if (questionsAnsweredSinceSkip < 2) {
                cout << "\n[Answer " << (2 - questionsAnsweredSinceSkip)
                     << " more question(s) before you can skip]\n";
            } else {
                skippedList.add(currentQ);
                questionsAnsweredSinceSkip = 0;
                questionsAttempted++;

                QuestionNode*   nodeToSkip = currentQ;
                DifficultyNode* sourceTier = currentTier;

                if      (currentQ->next) currentQ = currentQ->next;
                else if (currentQ->prev) currentQ = currentQ->prev;
                else                     currentQ = NULL;

                unlinkFromExam(sourceTier->head, nodeToSkip);

                if (currentQ == nodeToSkip)
                    currentQ = currentTier->head;

                cout << "\n[Skipped -- answer in Review Mode after the exam]\n";
            }
        }
        // Navigation
        else if (input == 'N') {
            if (currentQ->next) currentQ = currentQ->next;
            else cout << "[End of current tier]\n";
        }
        else if (input == 'P') {
            if (currentQ->prev) currentQ = currentQ->prev;
            else cout << "[Start of current tier]\n";
        }
        else {
            cout << "[Invalid -- use A/B/C/D, K, N, P, or X]\n";
        }
    }

    // ── Post-Exam ─────────────────────────────────────────────
    cout << "\n\n--- Exam Phase Complete ---";
    cout << "\nAnswered : " << attemptedList.total;
    cout << "\nSkipped  : " << skippedList.count << "\n";

    if (!skippedList.isEmpty()) {
        cout << "\nYou have " << skippedList.count << " skipped question(s).\n";
        cout << "[R] Review & Answer Skipped  [S] Submit Now: ";
        char choice;
        cin  >> choice;
        cin.ignore(1000, '\n');
        if (toupper(choice) == 'R')
            runReviewMode(skippedList, attemptedList);
        else
            cout << "\n[Skipped questions will be marked wrong]\n";
    }

    // ── Enqueue and Grade ─────────────────────────────────────
    ExamSubmission submission;
    submission.studentName    = studentName;
    submission.studentID      = studentID;
    submission.responses      = &attemptedList;
    submission.totalQuestions = MAX_QUESTIONS;

    submissionQueue.enqueue(submission);
    gradeSubmission(submissionQueue, skippedList.count);
}

// ─────────────────────────────────────────────────────────────
//  Practice Questions Menu
// ─────────────────────────────────────────────────────────────
void practiceMenu(PracticeManager& practice) {
    int choice;
    while (true) {
        cout << "\n==============================================";
        cout << "\n         PRACTICE QUESTIONS";
        cout << "\n==============================================";
        cout << "\n  1. Browse All Questions";
        cout << "\n  2. Search by Keyword";
        cout << "\n  3. Back to Main Menu";
        cout << "\n==============================================";
        cout << "\nChoice: ";
        cin  >> choice;
        cin.ignore(1000, '\n');

        if (choice == 1) {
            practice.browseAll();
        }
        else if (choice == 2) {
            string keyword;
            cout << "\nEnter keyword to search: ";
            getline(cin, keyword);
            if (keyword.empty()) {
                cout << "[No keyword entered]\n";
            } else {
                practice.searchByKeyword(keyword);
            }
        }
        else if (choice == 3) {
            break;
        }
        else {
            cout << "[Invalid choice -- enter 1, 2, or 3]\n";
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  Leaderboard placeholder (to be implemented next)
// ─────────────────────────────────────────────────────────────
void viewLeaderboard() {
    cout << "\n==============================================";
    cout << "\n            LEADERBOARD";
    cout << "\n==============================================";
    cout << "\n  [Coming soon -- no results saved yet]\n";
    cout << "==============================================\n";
}

// ─────────────────────────────────────────────────────────────
//  MAIN -- Main Menu
// ─────────────────────────────────────────────────────────────
int main() {
    // Pre-load practice questions once at startup
    PracticeManager practice;

    cout << "\n==============================================";
    cout << "\n       ADAPTIVE EXAM SYSTEM";
    cout << "\n        NUCES-FAST  |  CS2001";
    cout << "\n==============================================\n";

    // Student login -- collected once, reused if they start quiz
    string studentName, studentID;
    cout << "Enter your name: ";
    getline(cin, studentName);
    cout << "Enter your ID  : ";
    getline(cin, studentID);
    cout << "\nWelcome, " << studentName << "!\n";

    // ── Main Menu Loop ────────────────────────────────────────
    int choice;
    while (true) {
        cout << "\n==============================================";
        cout << "\n              MAIN MENU";
        cout << "\n==============================================";
        cout << "\n  1. Start Quiz";
        cout << "\n  2. View Leaderboard";
        cout << "\n  3. Practice Questions";
        cout << "\n  4. Exit";
        cout << "\n==============================================";
        cout << "\nChoice: ";
        cin  >> choice;
        cin.ignore(1000, '\n');

        if (choice == 1) {
            startQuiz(studentName, studentID);
        }
        else if (choice == 2) {
            viewLeaderboard();
        }
        else if (choice == 3) {
            practiceMenu(practice);
        }
        else if (choice == 4) {
            cout << "\nGoodbye, " << studentName << "!\n";
            break;
        }
        else {
            cout << "[Invalid choice -- enter 1, 2, 3, or 4]\n";
        }
    }

    return 0;
}