#include <iostream>
using namespace std;

// Maximum number of courses that can be stored in the system
const int MAX_COURSES = 100;

// Enumeration for semester terms (FALL, SPRING, SUMMER, WINTER)
enum class SemesterTerm { FALL, SPRING, SUMMER, WINTER };

// Enumeration for semester years (2025 to 2028)
enum class SemesterYear { Y2025, Y2026, Y2027, Y2028 };

// Structure to hold semester information (term and year)
struct Semester {
    SemesterTerm term;  // The term (e.g., FALL, SPRING)
    SemesterYear year;  // The year (e.g., 2023, 2024)
};

// Union to hold course-specific information (different for core vs elective courses)
union CourseInfo {
    struct {
        int lectureHours;  // Core courses have lecture hours
    } core;
    struct {
        int labHours;  // Elective courses have lab hours
    } elective;
};

// Structure to represent a course with all its attributes
struct Course {
    string name;          // Name of the course (e.g., "Data Structures")
    string code;          // Course code (e.g., "CS201")
    int creditHours;      // Number of credit hours (e.g., 3)
    string instructor;    // Name of the instructor
    Semester semester;    // Semester when the course is offered
    bool isCoreCourse;    // Flag: true for core, false for elective
    CourseInfo info;      // Additional info (lecture/lab hours)
};

// Abstract base class defining the interface for course management
class Manager {
public:
    // Pure virtual functions (must be implemented by derived classes)
    virtual void addCourse() = 0;           // Add a new course
    virtual void displayCourses() = 0;      // Display all courses
    virtual void updateCourse() = 0;        // Update a course
    virtual void removeCourse() = 0;        // Remove a course
    virtual void searchByCode() = 0;        // Search by course code
    virtual void listBySemester() = 0;      // List courses by semester
};

// Concrete class implementing the course management functionality
class CourseManager : public Manager {
private:
    Course courses[MAX_COURSES];  // Array to store all courses
    int courseCount = 0;          // Counter for number of courses added

    // Helper function to get semester term from user input
    SemesterTerm getTermFromUser() {
        int choice;
        cout << "Select Term:\n"
             << "1. Fall\n2. Spring\n3. Summer\n4. Winter\nChoice: ";
        cin >> choice;
        return static_cast<SemesterTerm>(choice - 1);  // Convert input to enum
    }

    // Helper function to get semester year from user input
    SemesterYear getYearFromUser() {
        int choice;
        cout << "Select Year:\n"
             << "1. 2025\n2. 2026\n3. 2027\n4. 2028\nChoice: ";
        cin >> choice;
        return static_cast<SemesterYear>(choice - 1);  // Convert input to enum
    }

public:
    // Implementation of adding a new course
    void addCourse() override {
        if (courseCount >= MAX_COURSES) {
            cout << "Maximum number of courses reached!\n";
            return;
        }

        Course newCourse;
        cin.ignore();  // Clear any leftover newline in input buffer

        // Get course details from user
        cout << "\nEnter Course Name: ";
        getline(cin, newCourse.name);

        cout << "Enter Course Code: ";
        getline(cin, newCourse.code);

        cout << "Enter Credit Hours: ";
        cin >> newCourse.creditHours;

        cout << "Enter Instructor Name: ";
        cin.ignore();  // Clear buffer before getting line
        getline(cin, newCourse.instructor);

        // Get semester information
        newCourse.semester.term = getTermFromUser();
        newCourse.semester.year = getYearFromUser();

        // Determine if course is core or elective
        cout << "Is this a Core Course? (1 for Yes, 0 for No): ";
        cin >> newCourse.isCoreCourse;

        // Get additional info based on course type
        if (newCourse.isCoreCourse) {
            cout << "Enter Lecture Hours: ";
            cin >> newCourse.info.core.lectureHours;
        } else {
            cout << "Enter Lab Hours: ";
            cin >> newCourse.info.elective.labHours;
        }

        // Add the new course to the array and increment count
        courses[courseCount++] = newCourse;
        cout << "Course added successfully!\n";
    }

    // Implementation of displaying all courses
    void displayCourses() override {
        if (courseCount == 0) {
            cout << "No courses to display!\n";
            return;
        }

        // Loop through all courses and print their details
        for (int i = 0; i < courseCount; i++) {
            cout << "\nCourse " << (i + 1) << ":\n"
                 << "Name: " << courses[i].name << "\n"
                 << "Code: " << courses[i].code << "\n"
                 << "Credits: " << courses[i].creditHours << "\n"
                 << "Instructor: " << courses[i].instructor << "\n"
                 << "Term: " << static_cast<int>(courses[i].semester.term) + 1 << "\n"
                 << "Year: " << (2023 + static_cast<int>(courses[i].semester.year)) << "\n"
                 << "Type: " << (courses[i].isCoreCourse ? "Core" : "Elective") << "\n";
            
            // Print additional info based on course type
            if (courses[i].isCoreCourse) {
                cout << "Lecture Hours: " << courses[i].info.core.lectureHours << "\n";
            } else {
                cout << "Lab Hours: " << courses[i].info.elective.labHours << "\n";
            }
            cout << "-----------------------------\n";
        }
    }

    // Implementation of updating a course
    void updateCourse() override {
        string code;
        cout << "Enter course code to update: ";
        cin >> code;

        // Search for the course by code
        for (int i = 0; i < courseCount; i++) {
            if (courses[i].code == code) {
                cout << "Updating course:\n";
                Course& course = courses[i];  // Reference to the found course
                
                // Update name
                cout << "New Course Name (" << course.name << "): ";
                cin.ignore();
                getline(cin, course.name);

                // Update credit hours
                cout << "New Credit Hours (" << course.creditHours << "): ";
                cin >> course.creditHours;

                // Optionally update semester
                cout << "Update semester? (1=Yes, 0=No): ";
                int choice;
                cin >> choice;
                if (choice) {
                    course.semester.term = getTermFromUser();
                    course.semester.year = getYearFromUser();
                }

                cout << "Course updated successfully!\n";
                return;
            }
        }
        cout << "Course not found!\n";
    }

    // Implementation of removing a course
    void removeCourse() override {
        string code;
        cout << "Enter course code to remove: ";
        cin >> code;

        // Search for the course by code
        for (int i = 0; i < courseCount; i++) {
            if (courses[i].code == code) {
                // Shift all subsequent courses left to fill the gap
                for (int j = i; j < courseCount - 1; j++) {
                    courses[j] = courses[j + 1];
                }
                courseCount--;  // Decrease total course count
                cout << "Course removed successfully!\n";
                return;
            }
        }
        cout << "Course not found!\n";
    }

    // Implementation of searching for a course by code
    void searchByCode() override {
        string code;
        cout << "Enter course code to search: ";
        cin >> code;

        // Search for the course
        for (int i = 0; i < courseCount; i++) {
            if (courses[i].code == code) {
                // Display basic course info if found
                cout << "Course found:\n"
                     << "Name: " << courses[i].name << "\n"
                     << "Code: " << courses[i].code << "\n"
                     << "Instructor: " << courses[i].instructor << "\n";
                return;
            }
        }
        cout << "Course not found!\n";
    }

    // Implementation of listing courses by semester
    void listBySemester() override {
        // Get semester criteria from user
        SemesterTerm term = getTermFromUser();
        SemesterYear year = getYearFromUser();

        cout << "Courses for selected semester:\n";
        bool found = false;
        
        // Search for matching courses
        for (int i = 0; i < courseCount; i++) {
            if (courses[i].semester.term == term && 
                courses[i].semester.year == year) {
                cout << "- " << courses[i].code << ": " << courses[i].name << "\n";
                found = true;
            }
        }
        
        if (!found) {
            cout << "No courses found for this semester.\n";
        }
    }
};

// Main function - entry point of the program
int main() {
    // Create a CourseManager instance (polymorphically as a Manager)
    Manager* manager = new CourseManager();
    int choice;

    // Main menu loop
    do {
        cout << "\nCourse Management System\n"
             << "1. Add Course\n"
             << "2. Display Courses\n"
             << "3. Update Course\n"
             << "4. Remove Course\n"
             << "5. Search by Code\n"
             << "6. List by Semester\n"
             << "7. Exit\n"
             << "Enter your choice: ";
        cin >> choice;

        // Execute the selected operation
        switch (choice) {
            case 1: 
                manager->addCourse(); 
                break;
            case 2: 
                manager->displayCourses(); 
                break;
            case 3: 
                manager->updateCourse(); 
                break;
            case 4: 
                manager->removeCourse(); 
                break;
            case 5: 
                manager->searchByCode(); 
                break;
            case 6: 
                manager->listBySemester(); 
                break;
            case 7: 
                cout << "Exiting the Course Management System. Goodbye!\n"; 
                break;
            default: 
                cout << "Invalid choice! Please enter a number between 1 and 7.\n";
        }
    } while (choice != 7);  // Continue until user chooses to exit

    return 0;
}