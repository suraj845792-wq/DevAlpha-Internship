/*
 * ============================================================
 *  DevAlpha Technologies — C++ Programming Internship
 *  Task 2 (Easy): Data Structure Application
 *  Project     : Student Grade Management System
 *  Concepts    : Arrays, Sorting (Bubble + Selection),
 *                Searching (Linear + Binary), Structures
 * ============================================================
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
using namespace std;

// ─────────────────────────────────────────────
//  STRUCTURE: Student
// ─────────────────────────────────────────────
struct Student {
    int    rollNo;
    string name;
    float  marks[5];      // 5 subjects
    float  totalMarks;
    float  percentage;
    string grade;
};

// ─────────────────────────────────────────────
//  CONSTANTS
// ─────────────────────────────────────────────
const int   MAX_STUDENTS = 100;
const float MAX_MARKS    = 100.0;
const string SUBJECTS[5] = {
    "Math", "Physics", "Chemistry", "English", "Computer"
};

// ─────────────────────────────────────────────
//  FUNCTION: Calculate grade from percentage
// ─────────────────────────────────────────────
string calculateGrade(float percentage) {
    if (percentage >= 90) return "A+";
    if (percentage >= 80) return "A";
    if (percentage >= 70) return "B+";
    if (percentage >= 60) return "B";
    if (percentage >= 50) return "C";
    if (percentage >= 40) return "D";
    return "F";
}

// ─────────────────────────────────────────────
//  FUNCTION: Compute total, percentage, grade
// ─────────────────────────────────────────────
void computeResult(Student& s) {
    s.totalMarks = 0;
    for (int i = 0; i < 5; i++)
        s.totalMarks += s.marks[i];
    s.percentage = (s.totalMarks / (5 * MAX_MARKS)) * 100.0;
    s.grade      = calculateGrade(s.percentage);
}

// ─────────────────────────────────────────────
//  FUNCTION: Display single student
// ─────────────────────────────────────────────
void displayStudent(const Student& s) {
    cout << "\n  +---------------------------------------+" << endl;
    cout << "  | Roll No : " << s.rollNo                   << endl;
    cout << "  | Name    : " << s.name                     << endl;
    cout << "  +---------------------------------------+" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "  | " << left << setw(12) << SUBJECTS[i]
             << " : " << fixed << setprecision(1) << s.marks[i] << endl;
    }
    cout << "  +---------------------------------------+" << endl;
    cout << "  | Total      : " << s.totalMarks << " / 500"  << endl;
    cout << "  | Percentage : " << s.percentage << "%"        << endl;
    cout << "  | Grade      : " << s.grade                    << endl;
    cout << "  +---------------------------------------+" << endl;
}

// ─────────────────────────────────────────────
//  FUNCTION: Display all students (table)
// ─────────────────────────────────────────────
void displayAll(Student arr[], int n) {
    if (n == 0) { cout << "\n  No students found." << endl; return; }
    cout << "\n  " << string(65, '-') << endl;
    cout << "  " << left
         << setw(8)  << "Roll"
         << setw(20) << "Name"
         << setw(10) << "Total"
         << setw(12) << "Percentage"
         << "Grade" << endl;
    cout << "  " << string(65, '-') << endl;
    for (int i = 0; i < n; i++) {
        cout << "  " << left
             << setw(8)  << arr[i].rollNo
             << setw(20) << arr[i].name
             << setw(10) << fixed << setprecision(1) << arr[i].totalMarks
             << setw(12) << arr[i].percentage
             << arr[i].grade << endl;
    }
    cout << "  " << string(65, '-') << endl;
}

// ─────────────────────────────────────────────
//  SORTING: Bubble Sort (by percentage DESC)
// ─────────────────────────────────────────────
void bubbleSortDesc(Student arr[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j].percentage < arr[j + 1].percentage)
                swap(arr[j], arr[j + 1]);
    cout << "\n  [OK] Students sorted by percentage (High to Low)." << endl;
}

// ─────────────────────────────────────────────
//  SORTING: Selection Sort (by name A-Z)
// ─────────────────────────────────────────────
void selectionSortByName(Student arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j].name < arr[minIdx].name)
                minIdx = j;
        swap(arr[i], arr[minIdx]);
    }
    cout << "\n  [OK] Students sorted by name (A to Z)." << endl;
}

// ─────────────────────────────────────────────
//  SEARCH: Linear Search by name
// ─────────────────────────────────────────────
int linearSearchByName(Student arr[], int n, string name) {
    for (int i = 0; i < n; i++)
        if (arr[i].name == name) return i;
    return -1;
}

// ─────────────────────────────────────────────
//  SEARCH: Binary Search by roll number
//  (array must be sorted by rollNo first)
// ─────────────────────────────────────────────
int binarySearchByRoll(Student arr[], int n, int rollNo) {
    // Sort by rollNo first (simple bubble)
    Student temp[MAX_STUDENTS];
    for (int i = 0; i < n; i++) temp[i] = arr[i];
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (temp[j].rollNo > temp[j + 1].rollNo)
                swap(temp[j], temp[j + 1]);

    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (temp[mid].rollNo == rollNo) {
            // Find original index
            for (int i = 0; i < n; i++)
                if (arr[i].rollNo == rollNo) return i;
        }
        if (temp[mid].rollNo < rollNo) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

// ─────────────────────────────────────────────
//  FUNCTION: Class statistics
// ─────────────────────────────────────────────
void showStatistics(Student arr[], int n) {
    if (n == 0) { cout << "\n  No data available." << endl; return; }

    float highest = arr[0].percentage, lowest = arr[0].percentage, sum = 0;
    int   topIdx = 0, lowIdx = 0;
    int   gradeCount[7] = {0}; // A+,A,B+,B,C,D,F
    string grades[] = {"A+","A","B+","B","C","D","F"};

    for (int i = 0; i < n; i++) {
        sum += arr[i].percentage;
        if (arr[i].percentage > highest) { highest = arr[i].percentage; topIdx = i; }
        if (arr[i].percentage < lowest)  { lowest  = arr[i].percentage; lowIdx = i; }
        for (int g = 0; g < 7; g++)
            if (arr[i].grade == grades[g]) { gradeCount[g]++; break; }
    }

    cout << "\n  ======= CLASS STATISTICS =======" << endl;
    cout << "  Total Students  : " << n << endl;
    cout << "  Class Average   : " << fixed << setprecision(2) << (sum / n) << "%" << endl;
    cout << "  Highest Score   : " << highest << "% (" << arr[topIdx].name << ")" << endl;
    cout << "  Lowest Score    : " << lowest  << "% (" << arr[lowIdx].name  << ")" << endl;
    cout << "\n  Grade Distribution:" << endl;
    for (int g = 0; g < 7; g++)
        if (gradeCount[g] > 0)
            cout << "    " << grades[g] << " : " << gradeCount[g] << " student(s)" << endl;
    cout << "  =================================" << endl;
}

// ─────────────────────────────────────────────
//  FUNCTION: Add student
// ─────────────────────────────────────────────
void addStudent(Student arr[], int& n) {
    if (n >= MAX_STUDENTS) {
        cout << "\n  [ERR] Maximum student limit reached." << endl;
        return;
    }
    Student s;
    cout << "\n  Enter Roll Number : "; cin >> s.rollNo;
    // Check duplicate roll
    for (int i = 0; i < n; i++) {
        if (arr[i].rollNo == s.rollNo) {
            cout << "\n  [ERR] Roll number already exists." << endl;
            return;
        }
    }
    cin.ignore();
    cout << "  Enter Name        : "; getline(cin, s.name);
    cout << "  Enter marks for 5 subjects (out of 100):" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "    " << SUBJECTS[i] << " : ";
        cin >> s.marks[i];
        if (s.marks[i] < 0 || s.marks[i] > 100) {
            cout << "\n  [ERR] Marks must be between 0-100." << endl;
            return;
        }
    }
    computeResult(s);
    arr[n++] = s;
    cout << "\n  [OK] Student added! Grade: " << s.grade
         << " | Percentage: " << fixed << setprecision(2) << s.percentage << "%" << endl;
}

// ─────────────────────────────────────────────
//  FUNCTION: Delete student by roll number
// ─────────────────────────────────────────────
void deleteStudent(Student arr[], int& n) {
    int roll;
    cout << "\n  Enter Roll Number to delete: "; cin >> roll;
    int idx = binarySearchByRoll(arr, n, roll);
    if (idx == -1) { cout << "\n  [ERR] Student not found." << endl; return; }
    for (int i = idx; i < n - 1; i++)
        arr[i] = arr[i + 1];
    n--;
    cout << "\n  [OK] Student deleted successfully." << endl;
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main() {
    Student students[MAX_STUDENTS];
    int count = 0;

    // Preload 3 sample students
    string sampleNames[] = {"Rahul Sharma", "Priya Singh", "Amit Verma"};
    float  sampleMarks[][5] = {
        {85, 90, 78, 92, 88},
        {72, 68, 75, 80, 70},
        {55, 60, 50, 65, 58}
    };
    for (int i = 0; i < 3; i++) {
        students[count].rollNo = 101 + i;
        students[count].name   = sampleNames[i];
        for (int j = 0; j < 5; j++)
            students[count].marks[j] = sampleMarks[i][j];
        computeResult(students[count]);
        count++;
    }

    cout << "\n  +======================================+" << endl;
    cout << "  |  Student Grade Management System     |" << endl;
    cout << "  |  DevAlpha Technologies Internship    |" << endl;
    cout << "  +======================================+" << endl;
    cout << "  (3 sample students preloaded)" << endl;

    int choice;
    do {
        cout << "\n  ------- MAIN MENU -------" << endl;
        cout << "  1. Add Student"              << endl;
        cout << "  2. Display All Students"     << endl;
        cout << "  3. Search by Name"           << endl;
        cout << "  4. Search by Roll Number"    << endl;
        cout << "  5. Sort by Percentage"       << endl;
        cout << "  6. Sort by Name (A-Z)"       << endl;
        cout << "  7. View Student Detail"      << endl;
        cout << "  8. Class Statistics"         << endl;
        cout << "  9. Delete Student"           << endl;
        cout << "  0. Exit"                     << endl;
        cout << "  Enter choice: ";
        cin  >> choice;

        if (choice == 1) {
            addStudent(students, count);

        } else if (choice == 2) {
            displayAll(students, count);

        } else if (choice == 3) {
            string name; cin.ignore();
            cout << "\n  Enter name to search: "; getline(cin, name);
            int idx = linearSearchByName(students, count, name);
            if (idx == -1) cout << "\n  [ERR] Student not found." << endl;
            else displayStudent(students[idx]);

        } else if (choice == 4) {
            int roll;
            cout << "\n  Enter roll number: "; cin >> roll;
            int idx = binarySearchByRoll(students, count, roll);
            if (idx == -1) cout << "\n  [ERR] Student not found." << endl;
            else displayStudent(students[idx]);

        } else if (choice == 5) {
            bubbleSortDesc(students, count);
            displayAll(students, count);

        } else if (choice == 6) {
            selectionSortByName(students, count);
            displayAll(students, count);

        } else if (choice == 7) {
            int roll;
            cout << "\n  Enter roll number: "; cin >> roll;
            int idx = binarySearchByRoll(students, count, roll);
            if (idx == -1) cout << "\n  [ERR] Student not found." << endl;
            else displayStudent(students[idx]);

        } else if (choice == 8) {
            showStatistics(students, count);

        } else if (choice == 9) {
            deleteStudent(students, count);

        } else if (choice != 0) {
            cout << "\n  [ERR] Invalid choice." << endl;
        }

    } while (choice != 0);

    cout << "\n  Thank you for using Grade Management System!\n" << endl;
    return 0;
}