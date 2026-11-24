#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#define MAX_STUDENTS 1000
#define MAX_ACCOUNTS 1000
#define STUDENT_FILE "students.dat"
#define ACCOUNT_FILE "accounts.dat"
#define MAX_ATTEMPTS 3
typedef struct {
    int id;
    char name[50];
    char address[100];
    char mobile[16];
    char email[60];
    char course[50];
    float marks;
} Student;
typedef struct {
    int id;
    char name[50];
    char email[60];
    uint64_t passwordHash;
} Account;
Student students[MAX_STUDENTS];
Account accounts[MAX_ACCOUNTS];
int studentCount = 0;
int accountCount = 0;
void readLine(const char *prompt, char *out, size_t size) {
    char buffer[512];
    while (1) {
        printf("%s", prompt);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\nInput error. Exiting.\n");
            exit(EXIT_FAILURE);
        }
        size_t len = strlen(buffer);
        if (len == 0 || buffer[len - 1] != '\n') {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Input too long. Try again.\n");
            continue;
        }
        buffer[--len] = '\0';
        if (len == 0 || strspn(buffer, " \t\r") == len) {
            printf("Input cannot be empty.\n");
            continue;
        }
        if (len >= size) {
            printf("Maximum length is %zu characters.\n", size - 1);
            continue;
        }
        strcpy(out, buffer);
        return;
    }
}
int readInt(const char *prompt) {
    char buffer[100];
    char *end;
    long value;
    while (1) {
        readLine(prompt, buffer, sizeof(buffer));
        errno = 0;
        value = strtol(buffer, &end, 10);
        while (isspace((unsigned char)*end))
            end++;
            if (end != buffer && *end == '\0' &&
            errno != ERANGE &&
            value >= 1 && value <= 2147483647L)
            return (int)value;
            printf("Enter a valid positive integer.\n");
    }
}
float readMarks(void) {
    char buffer[100];
    char *end;
    float marks;
    while (1) {
        readLine("Enter Marks (0-100): ", buffer, sizeof(buffer));
        errno = 0;
        marks = strtof(buffer, &end);
        while (isspace((unsigned char)*end))
            end++;
            if (end != buffer && *end == '\0' &&
            errno != ERANGE && marks >= 0 &&
            marks <= 100)
            return marks;
            printf("Marks must be a valid number from 0 to 100.\n");
    }
}
int validEmail(const char *email) {
    const char *at = strchr(email, '@');
    if (!at || at == email || strchr(at + 1, '@'))
        return 0;
        const char *dot = strchr(at + 1, '.');
    if (!dot || dot == at + 1 || dot[1] == '\0')
        return 0;
        if (strchr(email, ' ') || strchr(email, '\t'))
        return 0;
        return 1;
}
int validMobile(const char *mobile) {
    size_t len = strlen(mobile);
    return len >= 10 && len <= 15 &&
           strspn(mobile, "0123456789") == len;
}
uint64_t hashPassword(const char *password) {
    uint64_t hash = UINT64_C(14695981039346656037);

    for (int round = 0; round < 1000; round++) {
        for (const unsigned char *p =
             (const unsigned char *)password; *p; p++) {
            hash ^= *p;
            hash *= UINT64_C(1099511628211);
            hash ^= hash >> 29;
        }
        hash ^= (uint64_t)round;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}
int findAccount(int id) {
    for (int i = 0; i < accountCount; i++)
        if (accounts[i].id == id)
            return i;
            return -1;
}
int findStudent(int id) {
    for (int i = 0; i < studentCount; i++)
        if (students[i].id == id)
            return i;

    return -1;
}
int saveAccounts(void) {
    FILE *fp = fopen(ACCOUNT_FILE, "wb");
    if (!fp) {
        perror("Cannot save accounts");
        return 0;
    }
    int ok = fwrite(&accountCount, sizeof(accountCount), 1, fp) == 1;
    if (ok && fwrite(accounts, sizeof(Account),
                     (size_t)accountCount, fp) !=
                     (size_t)accountCount)
        ok = 0;
        if (fclose(fp) != 0)
        ok = 0;
        if (!ok)
        printf("Error writing account file.\n");

    return ok;
}
void loadAccounts(void) {
    FILE *fp = fopen(ACCOUNT_FILE, "rb");
    int n;
    Account temp[MAX_ACCOUNTS];
    if (!fp) {
        if (errno != ENOENT)
            perror("Cannot open account file");
        return;
    }
    if (fread(&n, sizeof(n), 1, fp) != 1 ||
        n < 0 || n > MAX_ACCOUNTS ||
        fread(temp, sizeof(Account), (size_t)n, fp) !=
        (size_t)n || fgetc(fp) != EOF) {
        printf("Warning: Invalid account file. No accounts loaded.\n");
        fclose(fp);
        return;
    }
    fclose(fp);
    for (int i = 0; i < n; i++) {
        temp[i].name[49] = '\0';
        temp[i].email[59] = '\0';

        if (temp[i].id <= 0 ||
            temp[i].name[0] == '\0' ||
            !validEmail(temp[i].email)) {
            printf("Invalid account data. File rejected.\n");
            return;
        }
        for (int j = 0; j < i; j++) {
            if (temp[i].id == temp[j].id) {
                printf("Duplicate account ID. File rejected.\n");
                return;
            }
        }
    }
    memcpy(accounts, temp, (size_t)n * sizeof(Account));
    accountCount = n;
}
int saveStudents(void) {
    FILE *fp = fopen(STUDENT_FILE, "wb");
    if (!fp) {
        perror("Cannot save student records");
        return 0;
    }
    int ok = fwrite(&studentCount, sizeof(studentCount), 1, fp) == 1;
    if (ok && fwrite(students, sizeof(Student),
                     (size_t)studentCount, fp) !=
                     (size_t)studentCount)
        ok = 0;
        if (fclose(fp) != 0)
        ok = 0;
        if (!ok)
        printf("Error writing student file.\n");

    return ok;
}
void loadStudents(void) {
    FILE *fp = fopen(STUDENT_FILE, "rb");
    int n;
    Student temp[MAX_STUDENTS];
    if (!fp) {
        if (errno != ENOENT)
            perror("Cannot open student file");
        return;
    }
    if (fread(&n, sizeof(n), 1, fp) != 1 ||
        n < 0 || n > MAX_STUDENTS ||
        fread(temp, sizeof(Student), (size_t)n, fp) !=
        (size_t)n || fgetc(fp) != EOF) {
        printf("Warning: Invalid student file. No records loaded.\n");
        fclose(fp);
        return;
    }
    fclose(fp);
    for (int i = 0; i < n; i++) {
        temp[i].name[49] = '\0';
        temp[i].address[99] = '\0';
        temp[i].mobile[15] = '\0';
        temp[i].email[59] = '\0';
        temp[i].course[49] = '\0';
        if (temp[i].id <= 0 ||
            temp[i].marks < 0 || temp[i].marks > 100 ||
            temp[i].name[0] == '\0' ||
            temp[i].address[0] == '\0' ||
            !validMobile(temp[i].mobile) ||
            !validEmail(temp[i].email) ||
            temp[i].course[0] == '\0') {
            printf("Invalid student record. File rejected.\n");
            return;
        }
        for (int j = 0; j < i; j++) {
            if (temp[i].id == temp[j].id) {
                printf("Duplicate student ID. File rejected.\n");
                return;
            }
        }
    }
    memcpy(students, temp, (size_t)n * sizeof(Student));
    studentCount = n;
}
void createAccount(void) {
    if (accountCount >= MAX_ACCOUNTS) {
        printf("Account limit reached.\n");
        return;
    }
    Account a = {0};
    char password[100], confirm[100];
    printf("\n===== CREATE STUDENT ACCOUNT =====\n");
    a.id = readInt("Create Student ID: ");
    if (findAccount(a.id) != -1) {
        printf("This Student ID is already registered.\n");
        return;
    }
    readLine("Enter Name: ", a.name, sizeof(a.name));
    while (1) {
        readLine("Enter Email: ", a.email, sizeof(a.email));
        if (validEmail(a.email))
            break;
            printf("Invalid email address. Try again.\n");
    }
    while (1) {
        readLine("Create Password (minimum 8 characters): ",
                 password, sizeof(password));
                 if (strlen(password) < 8) {
            printf("Password must contain at least 8 characters.\n");
            continue;
        }
        readLine("Confirm Password: ", confirm, sizeof(confirm));

        if (strcmp(password, confirm) != 0) {
            printf("Passwords do not match. Try again.\n");
            continue;
        }
        break;
    }
    a.passwordHash = hashPassword(password);
    memset(password, 0, sizeof(password));
    memset(confirm, 0, sizeof(confirm));
    accounts[accountCount] = a;
    accountCount++;
    if (!saveAccounts()) {
        accountCount--;
        memset(&accounts[accountCount], 0, sizeof(Account));
        printf("Account was not saved. Please try again.\n");
        return;
    }
    printf("\nAccount created successfully!\n");
    printf("You can now log in using your Student ID and password.\n");
}
int login(void) {
    int id;
    char password[100];
    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        printf("\n===== STUDENT LOGIN =====\n");
        id = readInt("Enter Student ID: ");
        readLine("Enter Password: ", password, sizeof(password));
        int index = findAccount(id);
        uint64_t suppliedHash = hashPassword(password);
        memset(password, 0, sizeof(password));
        if (index != -1 &&
            accounts[index].passwordHash == suppliedHash) {
            printf("\nLogin successful!\n");
            printf("Welcome, %s!\n", accounts[index].name);
            return index;
        }
        printf("Invalid Student ID or password. Attempts left: %d\n",
               MAX_ATTEMPTS - attempt);
    }
    printf("Login failed. Returning to the main menu.\n");
    return -1;
}
void readStudentDetails(Student *s) {
    readLine("Enter Name: ", s->name, sizeof(s->name));
    readLine("Enter Address: ", s->address, sizeof(s->address));
    while (1) {
        readLine("Enter Mobile Number: ", s->mobile, sizeof(s->mobile));
        if (validMobile(s->mobile))
            break;
            printf("Mobile must contain 10-15 digits only.\n");
    }
    while (1) {
        readLine("Enter Email ID: ", s->email, sizeof(s->email));
        if (validEmail(s->email))
            break;
            printf("Invalid email. Try again.\n");
    }
    readLine("Enter Course: ", s->course, sizeof(s->course));
    s->marks = readMarks();
}
void displayStudent(const Student *s) {
    printf("\nStudent ID : %d\n", s->id);
    printf("Name       : %s\n", s->name);
    printf("Address    : %s\n", s->address);
    printf("Mobile     : %s\n", s->mobile);
    printf("Email      : %s\n", s->email);
    printf("Course     : %s\n", s->course);
    printf("Marks      : %.2f/100\n", s->marks);
    printf("Percentage : %.2f%%\n", s->marks);
}
void showResult(const Student *s) {
    printf("\n===== STUDENT RESULT =====\n");
    printf("Name       : %s\n", s->name);
    printf("Student ID : %d\n", s->id);
    printf("Marks      : %.2f/100\n", s->marks);
    printf("Percentage : %.2f%%\n", s->marks);
    if (s->marks >= 90)
        printf("Grade      : A+\n");
    else if (s->marks >= 80)
        printf("Grade      : A\n");
    else if (s->marks >= 70)
        printf("Grade      : B\n");
    else if (s->marks >= 60)
        printf("Grade      : C\n");
    else if (s->marks >= 40)
        printf("Grade      : D\n");
    else
        printf("Grade      : F (Fail)\n");
}
void managementMenu(void) {
    int choice, id;
    while (1) {
        printf("\n====================================\n");
        printf("     STUDENT MANAGEMENT SYSTEM\n");
        printf("====================================\n");
        printf("1. Add Student\n");
        printf("2. View Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Student Result\n");
        printf("7. Class Teacher\n");
        printf("8. Logout\n");
        readLine("Enter choice (1-8): ", (char[10]){0}, 10);
        {
            char buffer[20], *end;
            long value;
            readLine("Select option: ", buffer, sizeof(buffer));
            errno = 0;
            value = strtol(buffer, &end, 10);
            while (isspace((unsigned char)*end))
                end++;
                if (errno == ERANGE || end == buffer || *end != '\0' ||
                value < 1 || value > 8) {
                printf("Invalid choice. Enter 1 to 8.\n");
                continue;
            }
            choice = (int)value;
        }
        switch (choice) {
        case 1: {
            if (studentCount >= MAX_STUDENTS) {
                printf("Student limit reached.\n");
                break;
            }
            Student s = {0};
            s.id = readInt("Enter Student ID: ");
            if (findStudent(s.id) != -1) {
                printf("Student ID already exists!\n");
                break;
            }
            readStudentDetails(&s);

            students[studentCount] = s;
            studentCount++;
            if (saveStudents()) {
                printf("Student added successfully!\n");
            } else {
                studentCount--;
                printf("Save failed; student not added.\n");
            }
            break;
        }
        case 2:
            if (studentCount == 0) {
                printf("No student records found.\n");
                break;
            }
            for (int i = 0; i < studentCount; i++)
                displayStudent(&students[i]);
                printf("Total Students: %d\n", studentCount);
            break;
            case 3:
            id = readInt("Enter Student ID to search: ");
            {
                int index = findStudent(id);

                if (index == -1)
                    printf("Student not found!\n");
                else
                    displayStudent(&students[index]);
            }
            break;
            case 4: {
            id = readInt("Enter Student ID to update: ");
            int index = findStudent(id);
            if (index == -1) {
                printf("Student not found!\n");
                break;
            }
            Student old = students[index];
            Student updated = old;
            printf("Enter updated details:\n");
            readStudentDetails(&updated);
            students[index] = updated;
            if (saveStudents()) {
                printf("Student updated successfully!\n");
            } else {
                students[index] = old;
                printf("Save failed; original record restored in memory.\n");
            }
            break;
        }
        case 5: {
            id = readInt("Enter Student ID to delete: ");
            int index = findStudent(id);
            if (index == -1) {
                printf("Student not found!\n");
                break;
            }
            Student old[MAX_STUDENTS];
            int oldCount = studentCount;
            memcpy(old, students,
                   (size_t)studentCount * sizeof(Student));
                   for (int i = index; i < studentCount - 1; i++)
                students[i] = students[i + 1];
                studentCount--;
            if (saveStudents()) {
                printf("Student deleted successfully!\n");
            } else {
                memcpy(students, old,
                       (size_t)oldCount * sizeof(Student));
                studentCount = oldCount;
                printf("Save failed; original records restored in memory.\n");
            }
            break;
        }
        case 6:
            id = readInt("Enter Student ID for result: ");
            {
                int index = findStudent(id);
                if (index == -1)
                    printf("Student not found!\n");
                else
                    showResult(&students[index]);
            }
            break;
            case 7:
            printf("\n===== CLASS TEACHER =====\n");
            printf("Teacher Name : Your Teacher Name\n");
            printf("Department   : CSE AI ML\n");
            printf("Designation  : Class Teacher\n");
            break;
            case 8:
            printf("Logged out successfully.\n");
            return;
        }
    }
}
int main(void) {
    int choice;
    loadAccounts();
    loadStudents();
        while (1) {
        printf("\n====================================\n");
        printf("        STUDENT PORTAL\n");
        printf("====================================\n");
        printf("1. Create Student Account\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        choice = readInt("Enter choice: ");
        switch (choice) {
        case 1:
            createAccount();
            break;
            case 2: {
            int accountIndex = login();
            if (accountIndex != -1)
                managementMenu();
                break;
        }
        case 3:
            printf("Thank you. Goodbye!\n");
            return 0;
            default:
            printf("Invalid choice.\n");
        }
    }
}