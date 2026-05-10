#include "PracticeManager.h"
#include <algorithm>
using namespace std;

// =============================================================
//  PracticeManager
// =============================================================

PracticeManager::PracticeManager()
    : head(nullptr), tail(nullptr), totalQuestions(0) {
    loadFromFile("Practice_Questions.txt");
}

PracticeManager::~PracticeManager() {
    PracticeNode* cur = head;
    while (cur) {
        PracticeNode* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
}

void PracticeManager::append(const string& qText,
                              const string& opts,
                              char key) {
    PracticeNode* node = new PracticeNode();
    node->questionText  = qText;
    node->options       = opts;
    node->correctAnswer = key;

    if (!head) {
        head = tail = node;
    } else {
        tail->next  = node;
        node->prev  = tail;
        tail        = node;
    }
    totalQuestions++;
}

void PracticeManager::loadFromFile(const string& filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "  [Error: could not open " << filename << "]\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        // Format: question|optA|optB|optC|optD|key
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);

        if (p1 == string::npos || p5 == string::npos) continue;

        string qText = line.substr(0, p1);
        string a     = line.substr(p1 + 1, p2 - p1 - 1);
        string b     = line.substr(p2 + 1, p3 - p2 - 1);
        string c     = line.substr(p3 + 1, p4 - p3 - 1);
        string d     = line.substr(p4 + 1, p5 - p4 - 1);
        char   key   = line[p5 + 1];

        string opts = "A) " + a + "  B) " + b +
                      "  C) " + c + "  D) " + d;

        append(qText, opts, key);
    }
    file.close();
    cout << "  [Practice bank loaded: "
         << totalQuestions << " questions]\n";
}

// ─────────────────────────────────────────────────────────────
//  Case-insensitive substring check
// ─────────────────────────────────────────────────────────────
bool PracticeManager::containsIgnoreCase(const string& haystack,
                                          const string& needle) const {
    string h = haystack;
    string n = needle;
    transform(h.begin(), h.end(), h.begin(), ::tolower);
    transform(n.begin(), n.end(), n.begin(), ::tolower);
    return h.find(n) != string::npos;
}

// ─────────────────────────────────────────────────────────────
//  Linear search by keyword
//  Checks every question's text for the keyword (case-insensitive)
// ─────────────────────────────────────────────────────────────
int PracticeManager::searchByKeyword(const string& keyword) const {
    if (!head) {
        cout << "  [Practice bank is empty]\n";
        return 0;
    }

    cout << "\n==============================================";
    cout << "\n  Search results for: \"" << keyword << "\"";
    cout << "\n==============================================\n";

    int           found = 0;
    PracticeNode* cur   = head;
    int           num   = 1;

    while (cur) {
        if (containsIgnoreCase(cur->questionText, keyword)) {
            found++;
            cout << "\n  [" << found << "] " << cur->questionText << "\n";
            cout << "      " << cur->options << "\n";
            cout << "      Correct Answer: " << cur->correctAnswer << "\n";
        }
        cur = cur->next;
        num++;
    }

    cout << "\n----------------------------------------------\n";
    if (found == 0)
        cout << "  0 results found for \"" << keyword << "\"\n";
    else
        cout << "  " << found << " result(s) found for \""
             << keyword << "\"\n";
    cout << "==============================================\n";

    return found;
}

// ─────────────────────────────────────────────────────────────
//  Browse all practice questions
// ─────────────────────────────────────────────────────────────
void PracticeManager::browseAll() const {
    if (!head) {
        cout << "  [No practice questions available]\n";
        return;
    }

    cout << "\n==============================================";
    cout << "\n         ALL PRACTICE QUESTIONS";
    cout << "\n==============================================\n";

    PracticeNode* cur = head;
    int i = 1;
    while (cur) {
        cout << "\n  " << i++ << ". " << cur->questionText << "\n";
        cout << "     " << cur->options << "\n";
        cout << "     Correct Answer: " << cur->correctAnswer << "\n";
        cur = cur->next;
    }
    cout << "\n==============================================\n";
}