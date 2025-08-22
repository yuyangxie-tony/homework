#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

// 课程结构体
struct Course {
    string name;
    int credit;
    int score;
    double gpa;
};

// 根据成绩计算单门课程的绩点
double calculateCourseGPA(int score) {
    if (score >= 90 && score <= 100) {
        return 4.0;
    } else if (score >= 85 && score <= 89) {
        return 3.7;
    } else if (score >= 82 && score <= 84) {
        return 3.3;
    } else if (score >= 78 && score <= 81) {
        return 3.0;
    } else if (score >= 75 && score <= 77) {
        return 2.7;
    } else if (score >= 72 && score <= 74) {
        return 2.3;
    } else if (score >= 68 && score <= 71) {
        return 2.0;
    } else if (score >= 64 && score <= 67) {
        return 1.5;
    } else if (score >= 60 && score <= 63) {
        return 1.0;
    } else if (score < 60 && score >= 0) {
        return 0.0;
    } else {
        // 无效成绩
        return -1.0;
    }
}

// 输入课程信息
Course inputCourse() {
    Course course;
    cout << "请输入课程名称: ";
    getline(cin, course.name);

    do {
        cout << "请输入" << course.name << "的学分(正整数): ";
        cin >> course.credit;
        if (course.credit <= 0) {
            cout << "学分必须是正整数，请重新输入！" << endl;
        }
        cin.ignore(); // 清除输入缓冲区
    } while (course.credit <= 0);

    do {
        cout << "请输入" << course.name << "的成绩(0-100): ";
        cin >> course.score;
        if (course.score < 0 || course.score > 100) {
            cout << "成绩必须在0-100之间，请重新输入！" << endl;
        }
        cin.ignore(); // 清除输入缓冲区
    } while (course.score < 0 || course.score > 100);

    course.gpa = calculateCourseGPA(course.score);
    return course;
}

// 显示课程信息
void displayCourse(const Course& course) {
    cout << course.name << "的绩点为: " << fixed << setprecision(1) << course.gpa << endl;
}

int main() {
    // 设置控制台输出为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8); // 设置输入编码为UTF-8

    cout << "===== 大学生绩点计算器 =====" << endl;
    cout << "请按照要求输入课程信息，输入完成后将为您计算平均绩点" << endl;
    cout << "---------------------------" << endl;

    vector<Course> courses;
    char continueInput;

    do {
        Course course = inputCourse();
        if (course.gpa >= 0) {
            courses.push_back(course);
            displayCourse(course);
        } else {
            cout << "成绩无效，该课程将不纳入计算！" << endl;
        }

        cout << "是否继续输入下一门课程？(y/n): ";
        cin >> continueInput;
        cin.ignore(); // 清除输入缓冲区

    } while (continueInput == 'y' || continueInput == 'Y');

    // 计算并显示平均绩点
    if (courses.empty()) {
        cout << "\n您没有输入任何有效课程信息！" << endl;
    } else {
        double totalCredit = 0.0;
        double totalGPA = 0.0;

        for (const auto& course : courses) {
            totalCredit += course.credit;
            totalGPA += course.credit * course.gpa;
        }

        double avgGPA = totalGPA / totalCredit;
        cout << "\n===== 绩点计算结果 =====" << endl;
        cout << "共输入 " << courses.size() << " 门课程，总学分: " << totalCredit << endl;
        cout << "您的平均绩点为: " << fixed << setprecision(2) << avgGPA << endl;
    }

    cout << "\n感谢使用绩点计算器，再见！" << endl;
    return 0;
}
