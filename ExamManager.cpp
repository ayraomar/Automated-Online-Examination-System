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

void ExamManager::addQuestion(QuestionNode*& head, string qText, string opts) {
    QuestionNode* newNode = new QuestionNode();
    newNode->questionText = qText;
    newNode->options = opts;

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
    if (!file.is_open()) {
        cout << "[Debug] Failed to open: " << filename << endl;
        return;
    }

    struct TempQuestion { string text; string opts; };
    TempQuestion pool[25]; // Larger buffer for safety
    int count = 0;
    string line;

    while (getline(file, line) && count < 25) {
        if (line.empty()) continue;

        size_t firstPipe = line.find('|');
        if (firstPipe != string::npos) {
            pool[count].text = line.substr(0, firstPipe);
            pool[count].opts = line.substr(firstPipe + 1);
            count++;
        }
    }
    file.close();

    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        TempQuestion temp = pool[i];
        pool[i] = pool[j];
        pool[j] = temp;
    }

    for (int i = 0; i < 15 && i < count; i++) {
        addQuestion(diffNode->head, pool[i].text, pool[i].opts);
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