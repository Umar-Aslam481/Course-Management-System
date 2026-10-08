#include <iostream>
#include <vector>
#include <memory>
#include <limits>
#include <variant>
#include <algorithm>
#include <string>
using namespace std;


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
struct CoreInfo {
    int lectureHours;
};

struct ElectiveInfo {
    int labHours;
};

// Structure to represent a course with all its attributes
struct Course {
    string name;          // Name of the course (e.g., "Data Structures")
    string code;          // Course code (e.g., "CS201")
    int creditHours;      // Number of credit hours (e.g., 3)
    string instructor;    // Name of the instructor
    Semester semester;    // Semester when the course is offered
    bool isCoreCourse;    // Flag: true for core, false for elective
    std::variant<CoreInfo, ElectiveInfo> info;  // Additional info (lecture/lab hours)
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
    virtual void clearInputBuffer() = 0;
    virtual ~Manager() = default;
};

// Concrete class implementing the course management functionality
class CourseManager : public Manager {
private:
    std::vector<Course> courses;  // Vector to store all courses



    // Helper function to get semester term from user input
    SemesterTerm getTermFromUser() {
        int choice;
        while (true) {
            cout << "Select Term:\n"
                 << "1. Fall\n2. Spring\n3. Summer\n4. Winter\nChoice: ";
            if (cin >> choice && choice >= 1 && choice <= 4) {
                return static_cast<SemesterTerm>(choice - 1);
            }
            cout << "Invalid input. Please enter a number between 1 and 4.\n";
            clearInputBuffer();
        }
    }

    // Helper function to get semester year from user input
    SemesterYear getYearFromUser() {
        int choice;
        while (true) {
            cout << "Select Year:\n"
                 << "1. 2025\n2. 2026\n3. 2027\n4. 2028\nChoice: ";
            if (cin >> choice && choice >= 1 && choice <= 4) {
                return static_cast<SemesterYear>(choice - 1);
            }
            cout << "Invalid input. Please enter a number between 1 and 4.\n";
            clearInputBuffer();
        }
    }

public:
    // Helper function to clear input buffer on invalid input
    void clearInputBuffer() override {
        if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    // Implementation of adding a new course
    void addCourse() override {


        Course newCourse;
        cin.ignore();  // Clear any leftover newline in input buffer

        // Get course details from user
        cout << "\nEnter Course Name: ";
        getline(cin, newCourse.name);

        cout << "Enter Course Code: ";
        getline(cin, newCourse.code);

        while (true) {
            cout << "Enter Credit Hours: ";
            if (cin >> newCourse.creditHours) break;
            cout << "Invalid input. Please enter a number.\n";
            clearInputBuffer();
        }

        cout << "Enter Instructor Name: ";
        cin.ignore();  // Clear buffer before getting line
        getline(cin, newCourse.instructor);

        // Get semester information
        newCourse.semester.term = getTermFromUser();
        newCourse.semester.year = getYearFromUser();

        // Determine if course is core or elective
        while (true) {
            cout << "Is this a Core Course? (1 for Yes, 0 for No): ";
            int temp;
            if (cin >> temp && (temp == 0 || temp == 1)) {
                newCourse.isCoreCourse = temp;
                break;
            }
            cout << "Invalid input. Please enter 1 or 0.\n";
            clearInputBuffer();
        }

        // Get additional info based on course type
        if (newCourse.isCoreCourse) {
            cout << "Enter Lecture Hours: ";
            CoreInfo cInfo;
            while (!(cin >> cInfo.lectureHours)) {
                cout << "Invalid input. Please enter a number.\nEnter Lecture Hours: ";
                clearInputBuffer();
            }
            newCourse.info = cInfo;
        } else {
            cout << "Enter Lab Hours: ";
            ElectiveInfo eInfo;
            while (!(cin >> eInfo.labHours)) {
                cout << "Invalid input. Please enter a number.\nEnter Lab Hours: ";
                clearInputBuffer();
            }
            newCourse.info = eInfo;
        }

        // Add the new course to the array and increment count
        courses.push_back(newCourse);
        cout << "Course added successfully!\n";
    }

    // Implementation of displaying all courses
    void displayCourses() override {
        if (courses.empty()) {
            cout << "No courses to display!\n";
            return;
        }

        // Loop through all courses and print their details
        int i = 0;
        for (const auto& course : courses) {
            cout << "\nCourse " << (++i) << ":\n"
                 << "Name: " << course.name << "\n"
                 << "Code: " << course.code << "\n"
                 << "Credits: " << course.creditHours << "\n"
                 << "Instructor: " << course.instructor << "\n"
                 << "Term: " << static_cast<int>(course.semester.term) + 1 << "\n"
                 << "Year: " << (2023 + static_cast<int>(course.semester.year)) << "\n"
                 << "Type: " << (course.isCoreCourse ? "Core" : "Elective") << "\n";
            
            // Print additional info based on course type
            if (std::holds_alternative<CoreInfo>(course.info)) {
                cout << "Lecture Hours: " << std::get<CoreInfo>(course.info).lectureHours << "\n";
            } else if (std::holds_alternative<ElectiveInfo>(course.info)) {
                cout << "Lab Hours: " << std::get<ElectiveInfo>(course.info).labHours << "\n";
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
        auto it = std::find_if(courses.begin(), courses.end(), [&code](const Course& c) { return c.code == code; });
        if (it != courses.end()) {
            cout << "Updating course:\n";
            Course& course = *it;  // Reference to the found course

            // Update name
            cout << "New Course Name (" << course.name << "): ";
            cin.ignore();
            getline(cin, course.name);

            // Update credit hours
            while (true) {
                cout << "New Credit Hours (" << course.creditHours << "): ";
                if (cin >> course.creditHours) break;
                cout << "Invalid input. Please enter a number.\n";
                clearInputBuffer();
            }

            // Optionally update semester
            int choice;
            while (true) {
                cout << "Update semester? (1=Yes, 0=No): ";
                if (cin >> choice && (choice == 0 || choice == 1)) break;
                cout << "Invalid input. Please enter 1 or 0.\n";
                clearInputBuffer();
            }
            if (choice) {
                course.semester.term = getTermFromUser();
                course.semester.year = getYearFromUser();
            }

            cout << "Course updated successfully!\n";
        } else {
            cout << "Course not found!\n";
        }
    }

    // Implementation of removing a course
    void removeCourse() override {
        string code;
        cout << "Enter course code to remove: ";
        cin >> code;

        // Search for the course by code and remove it
        auto it = std::find_if(courses.begin(), courses.end(), [&code](const Course& c) { return c.code == code; });
        if (it != courses.end()) {
            courses.erase(it);
            cout << "Course removed successfully!\n";
        } else {
            cout << "Course not found!\n";
        }
    }

    // Implementation of searching for a course by code
    void searchByCode() override {
        string code;
        cout << "Enter course code to search: ";
        cin >> code;

        // Search for the course
        auto it = std::find_if(courses.begin(), courses.end(), [&code](const Course& c) { return c.code == code; });
        if (it != courses.end()) {
            // Display basic course info if found
            cout << "Course found:\n"
                 << "Name: " << it->name << "\n"
                 << "Code: " << it->code << "\n"
                 << "Instructor: " << it->instructor << "\n";
        } else {
            cout << "Course not found!\n";
        }
    }

    // Implementation of listing courses by semester
    void listBySemester() override {
        // Get semester criteria from user
        SemesterTerm term = getTermFromUser();
        SemesterYear year = getYearFromUser();

        cout << "Courses for selected semester:\n";
        bool found = false;
        
        // Search for matching courses
        for (const auto& course : courses) {
            if (course.semester.term == term &&
                course.semester.year == year) {
                cout << "- " << course.code << ": " << course.name << "\n";
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
    std::unique_ptr<Manager> manager = std::make_unique<CourseManager>();
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

        if (!(cin >> choice)) {
            if (cin.eof()) break; // Exit gracefully on EOF
            cout << "Invalid choice! Please enter a number between 1 and 7.\n";
            manager->clearInputBuffer();
            choice = 0; // reset choice to avoid matching any valid case
            continue;
        }

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