#include <iostream>
#include <vector>
#include <memory>
#include <limits>
#include <variant>
#include <algorithm>
#include <string>
#include <sstream>
#include <gtkmm.h>
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
    virtual string addCourse(const Course& course) = 0;
    virtual string displayCourses() = 0;
    virtual string updateCourse(const string& code, const Course& new_data, bool update_semester) = 0;
    virtual string removeCourse(const string& code) = 0;
    virtual string searchByCode(const string& code) = 0;
    virtual string listBySemester(SemesterTerm term, SemesterYear year) = 0;
    virtual ~Manager() = default;
};

// Concrete class implementing the course management functionality
class CourseManager : public Manager {
private:
    std::vector<Course> courses;

public:
    string addCourse(const Course& course) override {
        courses.push_back(course);
        return "Course added successfully!";
    }

    string displayCourses() override {
        if (courses.empty()) {
            return "No courses to display!";
        }

        std::stringstream ss;
        int i = 0;
        for (const auto& course : courses) {
            ss << "Course " << (++i) << ":\n"
               << "Name: " << course.name << "\n"
               << "Code: " << course.code << "\n"
               << "Credits: " << course.creditHours << "\n"
               << "Instructor: " << course.instructor << "\n"
               << "Term: " << static_cast<int>(course.semester.term) + 1 << "\n"
               << "Year: " << (2025 + static_cast<int>(course.semester.year)) << "\n"
               << "Type: " << (course.isCoreCourse ? "Core" : "Elective") << "\n";
            
            if (std::holds_alternative<CoreInfo>(course.info)) {
                ss << "Lecture Hours: " << std::get<CoreInfo>(course.info).lectureHours << "\n";
            } else if (std::holds_alternative<ElectiveInfo>(course.info)) {
                ss << "Lab Hours: " << std::get<ElectiveInfo>(course.info).labHours << "\n";
            }
            ss << "-----------------------------\n";
        }
        return ss.str();
    }

    string updateCourse(const string& code, const Course& new_data, bool update_semester) override {
        auto it = std::find_if(courses.begin(), courses.end(), [&code](const Course& c) { return c.code == code; });
        if (it != courses.end()) {
            it->name = new_data.name;
            it->creditHours = new_data.creditHours;
            if (update_semester) {
                it->semester = new_data.semester;
            }
            return "Course updated successfully!";
        }
        return "Course not found!";
    }

    string removeCourse(const string& code) override {
        auto it = std::find_if(courses.begin(), courses.end(), [&code](const Course& c) { return c.code == code; });
        if (it != courses.end()) {
            courses.erase(it);
            return "Course removed successfully!";
        }
        return "Course not found!";
    }

    string searchByCode(const string& code) override {
        auto it = std::find_if(courses.begin(), courses.end(), [&code](const Course& c) { return c.code == code; });
        if (it != courses.end()) {
            std::stringstream ss;
            ss << "Course found:\n"
               << "Name: " << it->name << "\n"
               << "Code: " << it->code << "\n"
               << "Instructor: " << it->instructor << "\n";
            return ss.str();
        }
        return "Course not found!";
    }

    string listBySemester(SemesterTerm term, SemesterYear year) override {
        std::stringstream ss;
        ss << "Courses for selected semester:\n";
        bool found = false;
        
        for (const auto& course : courses) {
            if (course.semester.term == term && course.semester.year == year) {
                ss << "- " << course.code << ": " << course.name << "\n";
                found = true;
            }
        }
        
        if (!found) {
            return "No courses found for this semester.";
        }
        return ss.str();
    }
};

// Main function - entry point of the program


class MainWindow : public Gtk::Window {
protected:
    std::unique_ptr<Manager> manager;

    // Layout
    Gtk::Box m_VBox;
    Gtk::Grid m_Grid;
    Gtk::ScrolledWindow m_ScrolledWindow;

    // Widgets
    Gtk::Entry m_EntryName;
    Gtk::Entry m_EntryCode;
    Gtk::Entry m_EntryCredits;
    Gtk::Entry m_EntryInstructor;
    Gtk::ComboBoxText m_ComboTerm;
    Gtk::ComboBoxText m_ComboYear;
    Gtk::ComboBoxText m_ComboType;
    Gtk::Entry m_EntryHours;

    // Buttons
    Gtk::Button m_BtnAdd;
    Gtk::Button m_BtnUpdate;
    Gtk::Button m_BtnRemove;
    Gtk::Button m_BtnSearch;
    Gtk::Button m_BtnListSem;
    Gtk::Button m_BtnDisplayAll;

    // Output area
    Gtk::TextView m_TextView;
    Glib::RefPtr<Gtk::TextBuffer> m_TextBuffer;

    void print_output(const std::string& text) {
        m_TextBuffer->set_text(text);
    }

    Course parse_course_from_inputs() {
        Course c;
        c.name = m_EntryName.get_text();
        c.code = m_EntryCode.get_text();

        try {
            c.creditHours = std::stoi(m_EntryCredits.get_text());
        } catch(...) { c.creditHours = 0; }

        c.instructor = m_EntryInstructor.get_text();

        int term_idx = m_ComboTerm.get_active_row_number();
        c.semester.term = (term_idx >= 0) ? static_cast<SemesterTerm>(term_idx) : SemesterTerm::FALL;

        int year_idx = m_ComboYear.get_active_row_number();
        c.semester.year = (year_idx >= 0) ? static_cast<SemesterYear>(year_idx) : SemesterYear::Y2025;

        int type_idx = m_ComboType.get_active_row_number();
        c.isCoreCourse = (type_idx == 0); // Core is first option

        int hours = 0;
        try {
            hours = std::stoi(m_EntryHours.get_text());
        } catch(...) { hours = 0; }

        if (c.isCoreCourse) {
            CoreInfo ci; ci.lectureHours = hours;
            c.info = ci;
        } else {
            ElectiveInfo ei; ei.labHours = hours;
            c.info = ei;
        }

        return c;
    }

    // Callbacks
    void on_button_add_clicked() {
        Course c = parse_course_from_inputs();
        if (c.code.empty()) {
            print_output("Error: Course Code cannot be empty!");
            return;
        }
        string res = manager->addCourse(c);
        print_output(res);
    }

    void on_button_update_clicked() {
        Course c = parse_course_from_inputs();
        string code = m_EntryCode.get_text();
        if (code.empty()) {
            print_output("Error: Please provide a Course Code to update!");
            return;
        }
        string res = manager->updateCourse(code, c, true);
        print_output(res);
    }

    void on_button_remove_clicked() {
        string code = m_EntryCode.get_text();
        if (code.empty()) {
            print_output("Error: Please provide a Course Code to remove!");
            return;
        }
        string res = manager->removeCourse(code);
        print_output(res);
    }

    void on_button_search_clicked() {
        string code = m_EntryCode.get_text();
        if (code.empty()) {
            print_output("Error: Please provide a Course Code to search!");
            return;
        }
        string res = manager->searchByCode(code);
        print_output(res);
    }

    void on_button_list_sem_clicked() {
        int term_idx = m_ComboTerm.get_active_row_number();
        SemesterTerm term = (term_idx >= 0) ? static_cast<SemesterTerm>(term_idx) : SemesterTerm::FALL;

        int year_idx = m_ComboYear.get_active_row_number();
        SemesterYear year = (year_idx >= 0) ? static_cast<SemesterYear>(year_idx) : SemesterYear::Y2025;

        string res = manager->listBySemester(term, year);
        print_output(res);
    }

    void on_button_display_all_clicked() {
        string res = manager->displayCourses();
        print_output(res);
    }

public:
    MainWindow()
        : m_VBox(Gtk::ORIENTATION_VERTICAL, 10),
          m_BtnAdd("Add Course"), m_BtnUpdate("Update Course"),
          m_BtnRemove("Remove Course"), m_BtnSearch("Search by Code"),
          m_BtnListSem("List by Semester"), m_BtnDisplayAll("Display All")
    {
        set_title("Course Management System");
        set_default_size(700, 600);
        set_border_width(10);

        manager = std::make_unique<CourseManager>();

        add(m_VBox);

        // Setup Grid
        m_Grid.set_row_spacing(5);
        m_Grid.set_column_spacing(10);

        // Row 0
        m_Grid.attach(*Gtk::make_managed<Gtk::Label>("Name:"), 0, 0, 1, 1);
        m_Grid.attach(m_EntryName, 1, 0, 1, 1);
        m_Grid.attach(*Gtk::make_managed<Gtk::Label>("Code:"), 2, 0, 1, 1);
        m_Grid.attach(m_EntryCode, 3, 0, 1, 1);

        // Row 1
        m_Grid.attach(*Gtk::make_managed<Gtk::Label>("Credits:"), 0, 1, 1, 1);
        m_Grid.attach(m_EntryCredits, 1, 1, 1, 1);
        m_Grid.attach(*Gtk::make_managed<Gtk::Label>("Instructor:"), 2, 1, 1, 1);
        m_Grid.attach(m_EntryInstructor, 3, 1, 1, 1);

        // Row 2
        m_Grid.attach(*Gtk::make_managed<Gtk::Label>("Term:"), 0, 2, 1, 1);
        m_ComboTerm.append("Fall"); m_ComboTerm.append("Spring"); m_ComboTerm.append("Summer"); m_ComboTerm.append("Winter");
        m_ComboTerm.set_active(0);
        m_Grid.attach(m_ComboTerm, 1, 2, 1, 1);

        m_Grid.attach(*Gtk::make_managed<Gtk::Label>("Year:"), 2, 2, 1, 1);
        m_ComboYear.append("2025"); m_ComboYear.append("2026"); m_ComboYear.append("2027"); m_ComboYear.append("2028");
        m_ComboYear.set_active(0);
        m_Grid.attach(m_ComboYear, 3, 2, 1, 1);

        // Row 3
        m_Grid.attach(*Gtk::make_managed<Gtk::Label>("Type:"), 0, 3, 1, 1);
        m_ComboType.append("Core"); m_ComboType.append("Elective");
        m_ComboType.set_active(0);
        m_Grid.attach(m_ComboType, 1, 3, 1, 1);

        m_Grid.attach(*Gtk::make_managed<Gtk::Label>("Hours (Lec/Lab):"), 2, 3, 1, 1);
        m_Grid.attach(m_EntryHours, 3, 3, 1, 1);

        m_VBox.pack_start(m_Grid, Gtk::PACK_SHRINK);

        // Setup Buttons Box
        Gtk::Box* btnBox1 = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 5);
        btnBox1->pack_start(m_BtnAdd); btnBox1->pack_start(m_BtnUpdate); btnBox1->pack_start(m_BtnRemove);
        m_VBox.pack_start(*btnBox1, Gtk::PACK_SHRINK);

        Gtk::Box* btnBox2 = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 5);
        btnBox2->pack_start(m_BtnSearch); btnBox2->pack_start(m_BtnListSem); btnBox2->pack_start(m_BtnDisplayAll);
        m_VBox.pack_start(*btnBox2, Gtk::PACK_SHRINK);

        // Setup Text View for output
        m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
        m_TextBuffer = Gtk::TextBuffer::create();
        m_TextView.set_buffer(m_TextBuffer);
        m_TextView.set_editable(false);
        m_ScrolledWindow.add(m_TextView);
        m_VBox.pack_start(m_ScrolledWindow, Gtk::PACK_EXPAND_WIDGET);

        // Connect Signals
        m_BtnAdd.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_add_clicked));
        m_BtnUpdate.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_update_clicked));
        m_BtnRemove.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_remove_clicked));
        m_BtnSearch.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_search_clicked));
        m_BtnListSem.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_list_sem_clicked));
        m_BtnDisplayAll.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_display_all_clicked));

        show_all_children();
        print_output("Welcome to the GUI Course Management System!");
    }
};

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example.CourseManager");
    MainWindow window;
    return app->run(window);
}