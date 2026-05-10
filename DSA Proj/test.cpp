#include "ExamManager.h"
#include "ExamSystem.cpp"
#include "ReviewCLL.h"
#include "submission.h"
#include "PracticeManager.h"
#include <iostream>
#include <fstream>

using namespace std;

// ─────────────────────────────────────────────────────────────
//  Leaderboard  (insertion-sort linked list, descending score)
// ─────────────────────────────────────────────────────────────
struct LeaderboardEntry {
    string            studentName;
    string            studentID;
    double            score;
    char              grade;
    LeaderboardEntry* next;

    LeaderboardEntry(const string& n, const string& id,
                     double s, char g)
        : studentName(n), studentID(id),
          score(s), grade(g), next(nullptr) {}
};

class Leaderboard {
public:
    LeaderboardEntry* head;
    int               count;

    // ── Constructor: restore saved entries on startup ─────────
    Leaderboard() : head(nullptr), count(0) {
        loadFromFile();
    }

    // ── Destructor ────────────────────────────────────────────
    ~Leaderboard() {
        LeaderboardEntry* cur = head;
        while (cur) {
            LeaderboardEntry* tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }

    // ── Raw insert used internally (no file save) ─────────────
    void rawInsert(const string& name, const string& id,
                   double score, char grade) {
        LeaderboardEntry* node = new LeaderboardEntry(name, id, score, grade);
        if (!head || score > head->score) {
            node->next = head;
            head       = node;
        } else {
            LeaderboardEntry* cur = head;
            while (cur->next && cur->next->score >= score)
                cur = cur->next;
            node->next = cur->next;
            cur->next  = node;
        }
        count++;
    }

    // ── Public insert: sorted + persisted ────────────────────
    void insertSorted(const string& name, const string& id,
                      double score, char grade) {
        rawInsert(name, id, score, grade);
        saveToFile();
    }

    // ── Load from Leaderboard.txt on startup ──────────────────
    void loadFromFile() {
        ifstream file("Leaderboard.txt");
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            size_t p1 = line.find('|');
            size_t p2 = line.find('|', p1 + 1);
            size_t p3 = line.find('|', p2 + 1);

            if (p1 == string::npos || p3 == string::npos) continue;

            string name  = line.substr(0, p1);
            string id    = line.substr(p1 + 1, p2 - p1 - 1);
            double score = stod(line.substr(p2 + 1, p3 - p2 - 1));
            char   grade = line[p3 + 1];

            rawInsert(name, id, score, grade);
        }
        file.close();
    }

    // ── Save entire leaderboard to Leaderboard.txt ────────────
    void saveToFile() const {
        ofstream file("Leaderboard.txt");
        if (!file.is_open()) {
            cout << "  [Warning: could not save leaderboard]\n";
            return;
        }
        LeaderboardEntry* cur = head;
        while (cur) {
            file << cur->studentName << "|"
                 << cur->studentID   << "|"
                 << cur->score       << "|"
                 << cur->grade       << "\n";
            cur = cur->next;
        }
        file.close();
    }
};

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
void startQuiz(Leaderboard& leaderboard) {

    // ── Collect student info fresh for every quiz ─────────────
    string studentName, studentID;
    cout << "\nEnter your name: ";
    getline(cin, studentName);
    cout << "Enter your ID  : ";
    getline(cin, studentID);
    cout << "\nWelcome, " << studentName << "!\n";

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

    // ── Calculate score for leaderboard ──────────────────────
    int correct = 0;
    AttemptedNode* cur = attemptedList.head;
    if (cur) {
        do {
            if (cur->answer == cur->qPtr->correctAnswer)
                correct++;
            cur = cur->next;
        } while (cur != attemptedList.head);
    }
    double pct = (static_cast<double>(correct) / MAX_QUESTIONS) * 100.0;

    char gradeChar;
    if      (pct >= 90) gradeChar = 'A';
    else if (pct >= 80) gradeChar = 'B';
    else if (pct >= 70) gradeChar = 'C';
    else if (pct >= 60) gradeChar = 'D';
    else                gradeChar = 'F';

    leaderboard.insertSorted(studentName, studentID, pct, gradeChar);

    gradeSubmission(submissionQueue, skippedList.count);
}

// ─────────────────────────────────────────────────────────────
//  Helper: left-pad a string to a fixed width
// ─────────────────────────────────────────────────────────────
string padRight(const string& s, int width) {
    if ((int)s.size() >= width) return s.substr(0, width);
    return s + string(width - s.size(), ' ');
}

string padLeft(const string& s, int width) {
    if ((int)s.size() >= width) return s.substr(0, width);
    return string(width - s.size(), ' ') + s;
}

// ─────────────────────────────────────────────────────────────
//  View Leaderboard
// ─────────────────────────────────────────────────────────────
void viewLeaderboard(const Leaderboard& lb) {
    cout << "\n==============================================";
    cout << "\n              LEADERBOARD";
    cout << "\n==============================================";

    if (!lb.head) {
        cout << "\n  [No results yet -- take the exam first]\n";
        cout << "==============================================\n";
        return;
    }

    // Fixed column widths
    const int W_RANK  = 5;
    const int W_NAME  = 20;
    const int W_ID    = 12;
    const int W_SCORE = 10;
    const int W_GRADE = 5;

    cout << "\n  "
         << padRight("Rank",  W_RANK)
         << padRight("Name",  W_NAME)
         << padRight("ID",    W_ID)
         << padLeft ("Score", W_SCORE)
         << padLeft ("Grade", W_GRADE);
    cout << "\n  " << string(W_RANK+W_NAME+W_ID+W_SCORE+W_GRADE, '-') << "\n";

    LeaderboardEntry* cur  = lb.head;
    int               rank = 1;
    while (cur) {
        // Build score string e.g. "66.67%"
        // Manual double->string with 2 decimal places without <iomanip>
        int    whole = (int)cur->score;
        int    frac  = (int)((cur->score - whole) * 100 + 0.5);
        string scoreStr = to_string(whole) + "."
                        + (frac < 10 ? "0" : "")
                        + to_string(frac) + "%";

        cout << "  "
             << padRight(to_string(rank), W_RANK)
             << padRight(cur->studentName,  W_NAME)
             << padRight(cur->studentID,    W_ID)
             << padLeft (scoreStr,          W_SCORE)
             << padLeft (string(1, cur->grade), W_GRADE)
             << "\n";
        rank++;
        cur = cur->next;
    }
    cout << "  " << string(W_RANK+W_NAME+W_ID+W_SCORE+W_GRADE, '-') << "\n";
    cout << "==============================================\n";
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
//  MAIN -- Main Menu
// ─────────────────────────────────────────────────────────────
int main() {
    PracticeManager practice;
    Leaderboard     leaderboard;    // loads from Leaderboard.txt automatically

    cout << "\n==============================================";
    cout << "\n       ADAPTIVE EXAM SYSTEM";
    cout << "\n        NUCES-FAST  |  CS2001";
    cout << "\n==============================================\n";

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
            startQuiz(leaderboard);
        }
        else if (choice == 2) {
            viewLeaderboard(leaderboard);
        }
        else if (choice == 3) {
            practiceMenu(practice);
        }
        else if (choice == 4) {
            cout << "\nGoodbye!\n";
            break;
        }
        else {
            cout << "[Invalid choice -- enter 1, 2, 3, or 4]\n";
        }
    }

    return 0;
}
