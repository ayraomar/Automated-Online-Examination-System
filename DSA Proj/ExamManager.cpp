#include "ExamManager.h"
#include <fstream>
#include <ctime>   
#include <cstdlib> 

ExamManager::ExamManager() {

    srand(static_cast<unsigned int>(time(0)));

    root = new DifficultyNode("Medium");  
    easyNode = new DifficultyNode("Easy"); 
    hardNode = new DifficultyNode("Hard"); 

    root->left = easyNode;
    root->right = hardNode;

    populateList(easyNode, "Foundational_Set_1.txt");
    populateList(root, "Intermediate_Set_2.txt");
    populateList(hardNode, "Advanced_Set_3.txt");
}

void ExamManager::addQuestion(QuestionNode*& head, string qText, string opts, char key) {
    QuestionNode* newNode = new QuestionNode();
    newNode->questionText = qText;
    newNode->options = opts;
    newNode->correctAnswer = key; // Store the key here [cite: 58]

    if (!head) {
        head = newNode;
    } else {
        QuestionNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
        newNode->prev = temp;
    }
}

void ExamManager::populateList(DifficultyNode* diffNode, string filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) return;

    struct TempQuestion { string text; string opts; char key; };
    TempQuestion pool[25]; 
    int count = 0;
    string line;

    while (getline(file, line) && count < 25) {
        if (line.empty()) continue;

        // Locating all pipes to separate question, 4 options, and 1 key [cite: 123]
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);

        if (p1 != string::npos && p5 != string::npos) {
            pool[count].text = line.substr(0, p1);
            
            // Constructing clean options for printing [cite: 161, 162]
            string a = line.substr(p1 + 1, p2 - p1 - 1);
            string b = line.substr(p2 + 1, p3 - p2 - 1);
            string c = line.substr(p3 + 1, p4 - p3 - 1);
            string d = line.substr(p4 + 1, p5 - p4 - 1);
            pool[count].opts = "A) " + a + " B) " + b + " C) " + c + " D) " + d;
            
            pool[count].key = line[p5 + 1]; // Hidden correct answer key [cite: 56, 58]
            count++;
        }
    }
    file.close();

    // Randomizing order [cite: 94, 136]
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        TempQuestion temp = pool[i];
        pool[i] = pool[j];
        pool[j] = temp;
    }

    for (int i = 0; i < 15 && i < count; i++) {
        addQuestion(diffNode->head, pool[i].text, pool[i].opts, pool[i].key);
    }
}

void ExamManager::printList(DifficultyNode* diffNode) {
    if (!diffNode || !diffNode->head) return;

    cout << "\n--- " << diffNode->levelName << " LEVEL Questions ---" << endl;
    QuestionNode* temp = diffNode->head;
    int qNum = 1;
    while (temp) {
        cout << qNum << ". " << temp->questionText << endl;
        cout << "   Options: " << temp->options << endl;
        temp = temp->next;
        qNum++;
    }
}