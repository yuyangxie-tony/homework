#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h>

using namespace std;

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
    } else if (score >= 68 && score <= 71) { // 修正了这里
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

int main() {
    // 设置控制台输出为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8); // 设置输入编码为UTF-8

    cout << "===== 大学生绩点计算器 =====" << endl;
    cout << "请按照要求输入课程信息，输入完成后将为您计算平均绩点" << endl;
    cout << "---------------------------" << endl;
    
    int courseCount = 0;        // 课程数量
    double totalCredit = 0.0;   // 总学分
    double totalGPA = 0.0;      // 总绩点（学分*绩点之和）
    
    char continueInput;
    do {
        courseCount++;
        string courseName;
        int credit;
        int score;
        
        // 输入课程名称
        cout << "\n请输入第 " << courseCount << " 门课程的名称: ";
        cin.ignore(); // 清除输入缓冲区
        getline(cin, courseName);
        
        // 输入学分（确保输入有效）
        do {
            cout << "请输入" << courseName << "的学分(正整数): ";
            cin >> credit;
            if (credit <= 0) {
                cout << "学分必须是正整数，请重新输入！" << endl;
            }
        } while (credit <= 0);
        
        // 输入成绩（确保输入有效）
        int inputScore;
        do {
            cout << "请输入" << courseName << "的成绩(0-100): ";
            cin >> inputScore;
            if (inputScore < 0 || inputScore > 100) {
                cout << "成绩必须在0-100之间，请重新输入！" << endl;
            }
        } while (inputScore < 0 || inputScore > 100);
        score = inputScore;
        
        // 计算单门课程绩点
        double courseGPA = calculateCourseGPA(score);
        if (courseGPA < 0) {
            cout << "成绩无效，该课程将不纳入计算！" << endl;
            courseCount--;
        } else {
            // 显示单门课程信息
            cout << courseName << "的绩点为: " << fixed << setprecision(1) << courseGPA << endl;
            
            // 累加总学分和总绩点
            totalCredit += credit;
            totalGPA += credit * courseGPA;
        }
        
        // 询问是否继续输入
        cout << "是否继续输入下一门课程？(y/n): ";
        cin >> continueInput;
        
    } while (continueInput == 'y' || continueInput == 'Y');
    
    // 计算并显示平均绩点
    if (courseCount == 0) {
        cout << "\n您没有输入任何有效课程信息！" << endl;
    } else {
        double avgGPA = totalGPA / totalCredit;
        cout << "\n===== 绩点计算结果 =====" << endl;
        cout << "共输入 " << courseCount << " 门课程，总学分: " << totalCredit << endl;
        cout << "您的平均绩点为: " << fixed << setprecision(2) << avgGPA << endl;
    }
    
    cout << "\n感谢使用绩点计算器，再见！" << endl;
    return 0;
}
