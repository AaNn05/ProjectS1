#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
    char *surname;
    char *name;
};

struct subject {
    char *name;
    float grade;
    int coefficient;
};

struct TreeNode {
    float finalGrade;
    struct student studentInfo;
    struct subject *subjects;
    struct TreeNode *left;
    struct TreeNode *right;
};

int number_of_students() {
    int stud;
    printf("Enter the number of students: ");
    scanf("%d", &stud);
    return stud;
}

void free_student(struct student *student) {
    free(student->surname);
    free(student->name);
}

void insert_surname(struct student *student) {
    student->surname = malloc(sizeof(char) * 100);
    printf("Enter the surname of the student: ");
    scanf("%s", student->surname);
}

void insert_name(struct student *student) {
    student->name = malloc(sizeof(char) * 100);
    printf("Enter the name of the student: ");
    scanf("%s", student->name);
}

int quantity() {
    int n;
    printf("Enter the quantity of subjects: ");
    scanf("%d", &n);
    return n;
}

void free_subjects(struct subject *subjects, int n) {
    for (int i = 0; i < n; i++) {
        free(subjects[i].name);
    }
    free(subjects);
}

void insert_subject(struct subject *subjects, int n) {
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

void insert_grade(struct subject *subjects, int m) {
    for (int j = 0; j < m; j++) {
        printf("Enter the grade for %s: ", subjects[j].name);
        scanf("%f", &subjects[j].grade);
    }
}

void insert_coefficient(struct subject *subjects, int m) {
    for (int j = 0; j < m; j++) {
        printf("Enter the coefficient for %s: ", subjects[j].name);
        scanf("%d", &subjects[j].coefficient);
    }
}

int sum_of_cooficent(struct subject *subjects, int m) {
    int sum_coef = 0;
    for (int j = 0; j < m; j++) {
        sum_coef = sum_coef + subjects[j].coefficient;
    }
    return sum_coef;
}

void grades(struct subject *subjects, int m, float *mark) {
    for (int i = 0; i < m; i++) {
        mark[i] = subjects[i].coefficient * subjects[i].grade;
        printf("The mark for '%s' is: %.2f \n", subjects[i].name, mark[i]);
    }
}

float final_grade(struct subject *subjects, int m) {
    float totalGrade = 0;
    for (int i = 0; i < m; i++) {
        totalGrade += subjects[i].coefficient * subjects[i].grade;
    }
    return totalGrade;
}

float student_GCD(struct TreeNode *studentNode) {
    float totalGrade = studentNode->finalGrade;
    int totalCoof = sum_of_cooficent(studentNode->subjects, studentNode->finalGrade);

    if (totalCoof != 0) {
        return totalGrade / totalCoof;
    } else {
        printf("You can't divide by 0");
        return 0;  
    }
}

struct TreeNode *createNode(float finalGrade, struct student s, struct subject *subjects) {
    struct TreeNode *newNode = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    newNode->finalGrade = finalGrade;
    newNode->studentInfo = s;
    newNode->subjects = subjects;
    newNode->left = newNode->right = NULL;
    return newNode;
}

struct TreeNode *insertNode(struct TreeNode *root, float finalGrade, struct student s, struct subject *subjects) {
    if (root == NULL) {
        return createNode(finalGrade, s, subjects);
    }

    if (finalGrade < root->finalGrade) {
        root->left = insertNode(root->left, finalGrade, s, subjects);
    } else {
        root->right = insertNode(root->right, finalGrade, s, subjects);
    }

    return root;
}

void printTreeToFile(struct TreeNode *root, FILE *file) {
    if (root != NULL) {
        printTreeToFile(root->left, file);
        fprintf(file, "Student: %s %s, Final Grade: %.2f, GCD: %.2f\n",
                root->studentInfo.surname, root->studentInfo.name, root->finalGrade,
                student_GCD(root));
        printTreeToFile(root->right, file);
    }
}


void free_tree(struct TreeNode *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free_student(&(root->studentInfo));
        free_subjects(root->subjects, sum_of_cooficent(root->subjects, root->finalGrade));
        free(root);
    }
}

int main() {
    int st = number_of_students();

    struct TreeNode *root = NULL;

    for (int student_index = 0; student_index < st; ++student_index) {
        printf("\n=== Student %d ===\n", student_index + 1);

        struct student s;
        insert_surname(&s);
        insert_name(&s);
        int q = quantity();
        struct subject *subjects = malloc(q * sizeof(struct subject));
        insert_subject(subjects, q);
        insert_grade(subjects, q);
        insert_coefficient(subjects, q);

        float *mark = malloc(q * sizeof(float));
        grades(subjects, q, mark);

        float f = final_grade(subjects, q);

        root = insertNode(root, f, s, subjects);

        
        free_subjects(subjects, q);
        free(mark);
    }

    
    FILE *outputFile = fopen("output.txt", "w");
    if (outputFile != NULL) {
        
        printTreeToFile(root, outputFile);

        
        fclose(outputFile);
    } else {
        printf("Error opening file for writing.\n");
    }

    
    free_tree(root);

    return 0;
}
