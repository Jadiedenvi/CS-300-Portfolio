#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

class BinarySearchTree {
private:
    Node* root;

    void addNode(Node* node, Course course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            }
            else {
                addNode(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            }
            else {
                addNode(node->right, course);
            }
        }
    }

    void inOrder(Node* node) {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            inOrder(node->right);
        }
    }

    void destroyTree(Node* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        destroyTree(root);
    }

    void Insert(Course course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            addNode(root, course);
        }
    }

    Course Search(string courseNumber) {
        Node* current = root;

        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return current->course;
            }
            else if (courseNumber < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        Course course;
        return course;
    }

    void PrintInOrder() {
        inOrder(root);
    }

    bool IsEmpty() {
        return root == nullptr;
    }
};

string trim(string text) {
    text.erase(0, text.find_first_not_of(" \t\r\n"));
    text.erase(text.find_last_not_of(" \t\r\n") + 1);
    return text;
}

string toUpperCase(string text) {
    transform(text.begin(), text.end(), text.begin(), ::toupper);
    return text;
}

vector<string> splitLine(string line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

void loadCourses(string fileName, BinarySearchTree& courses) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Could not open file." << endl;
        return;
    }

    vector<Course> loadedCourses;
    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        vector<string> tokens = splitLine(line);

        if (tokens.size() < 2) {
            cout << "Error: Invalid course format in line: " << line << endl;
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens[0]);
        course.courseTitle = tokens[1];

        for (unsigned int i = 2; i < tokens.size(); i++) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(toUpperCase(tokens[i]));
            }
        }

        loadedCourses.push_back(course);
    }

    file.close();

    for (Course course : loadedCourses) {
        courses.Insert(course);
    }

    for (Course course : loadedCourses) {
        for (string prerequisite : course.prerequisites) {
            Course foundPrereq = courses.Search(prerequisite);

            if (foundPrereq.courseNumber.empty()) {
                cout << "Warning: Prerequisite " << prerequisite
                    << " for " << course.courseNumber << " was not found." << endl;
            }
        }
    }

    cout << "Courses loaded successfully." << endl;
}

void printCourseInformation(BinarySearchTree& courses, string courseNumber) {
    courseNumber = toUpperCase(courseNumber);

    Course course = courses.Search(courseNumber);

    if (course.courseNumber.empty()) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";

        for (unsigned int i = 0; i < course.prerequisites.size(); i++) {
            Course prereq = courses.Search(course.prerequisites[i]);

            if (!prereq.courseNumber.empty()) {
                cout << prereq.courseNumber << ", " << prereq.courseTitle;
            }
            else {
                cout << course.prerequisites[i];
            }

            if (i < course.prerequisites.size() - 1) {
                cout << "; ";
            }
        }

        cout << endl;
    }
}

void displayMenu() {
    cout << endl;
    cout << "Welcome to the course planner." << endl;
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main() {
    BinarySearchTree courses;
    int choice = 0;
    string fileName;
    string courseNumber;
    bool dataLoaded = false;

    while (choice != 9) {
        displayMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "Enter the course file name: ";
            getline(cin, fileName);

            loadCourses(fileName, courses);
            dataLoaded = true;
            break;

        case 2:
            if (!dataLoaded || courses.IsEmpty()) {
                cout << "Please load the course data first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                courses.PrintInOrder();
            }
            break;

        case 3:
            if (!dataLoaded || courses.IsEmpty()) {
                cout << "Please load the course data first." << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                getline(cin, courseNumber);
                printCourseInformation(courses, courseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}