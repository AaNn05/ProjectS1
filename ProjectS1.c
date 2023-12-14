#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
    char* surname;
    char* name;
};

struct subject {
    char* name;
    float grade;
    int coefficient;
};

struct TreeNode {
    float finalGrade;
    float GPA;
    int totalCoefficient;
    struct student studentInfo;
    struct subject* subjects;
    struct TreeNode* left;
    struct TreeNode* right;
};

int number_of_students() {
    int stud;
    printf("Enter the number of students: ");
    scanf("%d", &stud);
    return stud;
}

void insert_surname(struct student* student) {
    student->surname = malloc(sizeof(char) * 100);
    printf("Enter the surname of the student: ");
    scanf("%s", student->surname);
}

void insert_name(struct student* student) {
    student->name = malloc(sizeof(char) * 100);
    printf("Enter the name of the student: ");
    scanf("%s", student->name);
}

void free_student(struct student* student) {
    free(student->surname);
    free(student->name);
}

int quantity_of_subjects() {
    int n;
    printf("Enter the quantity of subjects: ");
    scanf("%d", &n);
    return n;
}

void free_subjects(struct subject* subjects, int n) {
    for (int i = 0; i < n; i++) {
        free(subjects[i].name);
    }
    free(subjects);
}

void insert_subject(struct subject* subjects, int n) {
    getchar();
    for (int i = 0; i < n; i++) {
        subjects[i].name = malloc(100 * sizeof(char));
        printf("Enter the name of %d subject: ", i + 1);
        fgets(subjects[i].name, 100, stdin);

        size_t length = strlen(subjects[i].name);
        if (subjects[i].name[length - 1] == '\n')
            subjects[i].name[length - 1] = '\0';
    }
}

void insert_grade(struct subject* subjects, int m) {
    for (int j = 0; j < m; j++) {
        printf("Enter the grade for %s: ", subjects[j].name);
        scanf("%f", &subjects[j].grade);
    }
}

void insert_coefficient(struct subject* subjects, int m) {
    for (int j = 0; j < m; j++) {
        printf("Enter the coefficient for %s: ", subjects[j].name);
        scanf("%d", &subjects[j].coefficient);
    }
}

int sum_of_cooficent(struct subject* subjects, int m) {
    int sum_coef = 0;
    for (int j = 0; j < m; j++) {
        sum_coef = sum_coef + subjects[j].coefficient;
    }
    return sum_coef;
}

float final_grade(struct subject* subjects, int m) {
    float totalGrade = 0;
    for (int i = 0; i < m; i++) {
        totalGrade += subjects[i].coefficient * subjects[i].grade;
    }
    return totalGrade;
}

float calculate_GPA(float finalGrade, int totalCoefficient) {
    if (totalCoefficient == 0) {
        return 0.0; // Avoid division by zero
    }
    return finalGrade / totalCoefficient;
}

struct TreeNode* createNode(float finalGrade, struct student s, struct subject* subjects, int totalCoefficient) {
    struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    newNode->finalGrade = finalGrade;
    newNode->totalCoefficient = totalCoefficient;
    newNode->GPA = calculate_GPA(finalGrade, totalCoefficient);
    newNode->studentInfo = s;
    newNode->subjects = subjects;
    newNode->left = 0;
    newNode->right = 0;
    return newNode;
}

struct TreeNode* insertNode(struct TreeNode* root, float finalGrade, struct student s, struct subject* subjects, int totalCoefficient) {
    if (root == 0) {
        return createNode(finalGrade, s, subjects, totalCoefficient);
    }

    if (finalGrade < root->finalGrade) {
        root->left = insertNode(root->left, finalGrade, s, subjects, totalCoefficient);
    } else {
        root->right = insertNode(root->right, finalGrade, s, subjects, totalCoefficient);
    }

    return root;
}

struct TreeNode* mostCommonGPANode;
int mostCommonGPARepetitions;

void findMostCommonGPA(struct TreeNode* root, float* maxGPA) {
    if (root != 0) {
        findMostCommonGPA(root->left, maxGPA);

        // Check if the current GPA is more common than the current most common GPA
        if (root->GPA == mostCommonGPANode->GPA) {
            mostCommonGPARepetitions++;
        } else if (mostCommonGPARepetitions == 0 || mostCommonGPARepetitions < 0) {
            // If no common GPA found yet or if the count is negative, update most common GPA
            mostCommonGPANode = root;
            mostCommonGPARepetitions = 1;
        }

        if (root->GPA > *maxGPA) {
            *maxGPA = root->GPA;
        }

        findMostCommonGPA(root->right, maxGPA);
    }
}

void printTreeToFile(struct TreeNode* root, FILE* file) {
    if (root != 0) {
        printTreeToFile(root->left, file);
        fprintf(file, "Student: %s %s, Final Grade: %.2f, GPA: %.2f\n", root->studentInfo.surname, root->studentInfo.name, root->finalGrade, root->GPA);
        printTreeToFile(root->right, file);
    }
}

void free_tree(struct TreeNode* root) {
    if (root != 0) {
        free_tree(root->left);
        free_tree(root->right);
        free_student(&(root->studentInfo));
        free_subjects(root->subjects, root->totalCoefficient);
    }
}

int main() {
    int st = number_of_students();

    struct TreeNode* root = 0;

    for (int student_index = 0; student_index < st; student_index++) {
        printf("\n=== Student %d ===\n", student_index + 1);

        struct student s;
        insert_surname(&s);
        insert_name(&s);
        int q = quantity_of_subjects();
        struct subject* subjects = malloc(q * sizeof(struct subject));
        insert_subject(subjects, q);
        insert_grade(subjects, q);
        insert_coefficient(subjects, q);

        float f = final_grade(subjects, q);
        root = insertNode(root, f, s, subjects, sum_of_cooficent(subjects, q));
    }

    mostCommonGPANode = root;
    mostCommonGPARepetitions = 0;

    float maxGPA = 0.0; // Track the maximum GPA encountered during traversal
    findMostCommonGPA(root, &maxGPA);

    FILE* studentFile = fopen("studentInfo.txt", "w");
    if (studentFile != 0) {
        printTreeToFile(root, studentFile);

        // Print most common GPA and repetitions, or max GPA if there is no common GPA
        if (mostCommonGPARepetitions > 1) {
            fprintf(studentFile, "Most Common GPA: %.2f, Repetitions: %d\n", mostCommonGPANode->GPA, mostCommonGPARepetitions);
        } else {
            fprintf(studentFile, "No common GPA found. Max GPA: %.2f\n", maxGPA);
        }

        fclose(studentFile);
    } else {
        printf("Error opening file for writing.\n");
    }

    free_tree(root);

    return 0;
}    
