#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PATIENTS 1000
#define MAX_DOCTORS 500
#define MAX_BOOKINGS 1000
#define ADMIN_PASSWORD "123"
// ------------------------------
// Patient record information
// ------------------------------
typedef struct
{
    int id;
    int age;
    char name[50];
    char patientAddress[50];
    char disease[50];
    char date[12];
    char password[20];
    int isAvailable;
    char suggestion[200];
} Patient;

// ------------------------------
// Doctor record information
// ------------------------------
typedef struct
{
    int id;
    char name[50];
    char address[50];
    char specialize[50];
    char date[12];
    char password[20];
    int isBooked;
    int bookingCount;
    char suggestion[200];

} Doctor;

// ------------------------------
// Appointment booking details
// ------------------------------
typedef struct
{
    int patientId;
    int doctorId;
    int bookingId;
    char appointmentDate[12];
    int isBooked;  // 0 for not booked, 1 for booked
    int isAccepted;  // 0 for not booked, 1 for booked
} Booking;

// Global arrays and counters
Patient patients[MAX_PATIENTS];
Doctor doctors[MAX_DOCTORS];
Booking bookings[MAX_BOOKINGS];
int patientCount = 0;
int doctorCount = 0;
int bookingCount = 0;
int nextBookingId = 1;

void loadPatients();
void savePatients();
void loadDoctors();
void saveDoctors();
void loadBookings();
void saveBookings();
void acceptBooking(int doctorId, int patientId);
void cancelBooking(int doctorId, int patientId);
void clearDoctorData();
void clearPatientData();
void admitPatient();
void listofPatients();
void dischargePatient();
void addDoctor();
void listofDoctors();
void doctorPortal();
void patientPortal();
void adminPortal();
int login(const char *type, char *password);
void createPatientProfile();
void createDoctorProfile();
void bookAppointment(int patientId);
void viewDoctorAppointments(int doctorId);
void viewPatientBookings(int patientId);
void cancelPatientBooking(int patientId);
void clearAllAppointmentsForPatient(int patientId);
static void readInputLine(char *buffer, size_t size);
static void clearInputBuffer(void);
static int isValidDateFormat(const char *date);

static void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

static int isValidDateFormat(const char *date)
{
    int day, month, year;

    if (date == NULL || strlen(date) != 10)
    {
        return 0;
    }

    if (sscanf(date, "%d/%d/%d", &day, &month, &year) != 3)
    {
        return 0;
    }

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900)
    {
        return 0;
    }

    return 1;
}

static void readInputLine(char *buffer, size_t size)
{
    while (1)
    {
        if (fgets(buffer, (int)size, stdin) == NULL)
        {
            buffer[0] = '\0';
            return;
        }

        if (buffer[0] == '\n')
        {
            continue;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        return;
    }
}

// ------------------------------
// Default sample data for first-time use
// ------------------------------
static void seedSampleData(void)
{
    if (patientCount == 0)
    {
        Patient samplePatients[] = {
            {1, 25, "Rahim", "Dhaka", "Fever", "19/09/2026", "p123", 1, ""},
            {2, 32, "Nusrat", "Chittagong", "Diabetes", "20/09/2026", "p456", 1, ""},
            {3, 40, "Karim", "Sylhet", "Hypertension", "21/09/2026", "p789", 1, ""}
        };

        int samplePatientCount = (int)(sizeof(samplePatients) / sizeof(samplePatients[0]));
        for (int i = 0; i < samplePatientCount; i++)
        {
            patients[i] = samplePatients[i];
        }
        patientCount = samplePatientCount;
        savePatients();
    }

    if (doctorCount == 0)
    {
        Doctor sampleDoctors[] = {
            {1, "Dr. Hasan", "Dhaka", "Cardiology", "19/09/2026", "d123", 0, 0, ""},
            {2, "Dr. Farah", "Chittagong", "Neurology", "20/09/2026", "d456", 0, 0, ""},
            {3, "Dr. Ahmed", "Sylhet", "Orthopedic", "21/09/2026", "d789", 0, 0, ""}
        };

        int sampleDoctorCount = (int)(sizeof(sampleDoctors) / sizeof(sampleDoctors[0]));
        for (int i = 0; i < sampleDoctorCount; i++)
        {
            doctors[i] = sampleDoctors[i];
        }
        doctorCount = sampleDoctorCount;
        saveDoctors();
    }
}

// ------------------------------
// File handling for bookings
// ------------------------------
void loadBookings()
{
    FILE *fp = fopen("bookings.dat", "rb");
    if (fp)
    {
        while (fread(&bookings[bookingCount], sizeof(Booking), 1, fp) == 1)
        {
            bookingCount++;
        }
        fclose(fp);
    }
    else
    {
        printf("No booking data file found.\n");
    }

    nextBookingId = 1;
    for (int i = 0; i < bookingCount; i++)
    {
        if (bookings[i].bookingId >= nextBookingId)
        {
            nextBookingId = bookings[i].bookingId + 1;
        }
    }
}

void saveBookings()
{
    FILE *fp = fopen("bookings.dat", "wb");
    if (fp)
    {
        fwrite(bookings, sizeof(Booking), bookingCount, fp);
        fclose(fp);
    }
    else
    {
        printf("Error saving booking data.\n");
    }
}

// Accept an appointment request from a patient
void acceptBooking(int doctorId, int patientId)
{
    if (doctorId <= 0 || doctorId > doctorCount)
    {
        printf("Invalid Doctor ID.\n");
        return;
    }

    for (int i = 0; i < bookingCount; i++)
    {
        if (bookings[i].doctorId == doctorId && bookings[i].patientId == patientId && bookings[i].isBooked == 1)
        {
            bookings[i].isAccepted = 1;
            saveBookings();
            printf("Booking accepted for Patient ID: %d\n", patientId);
            return;
        }
    }
    printf("No booking found for Patient ID: %d.\n", patientId);
}

// Cancel an appointment request from a doctor side
void cancelBooking(int doctorId, int patientId)
{
    if (doctorId <= 0 || doctorId > doctorCount)
    {
        printf("Invalid Doctor ID.\n");
        return;
    }

    for (int i = 0; i < bookingCount; i++)
    {
        if (bookings[i].doctorId == doctorId && bookings[i].patientId == patientId && bookings[i].isBooked == 1)
        {
            bookings[i].isBooked = 0;
            bookings[i].isAccepted = 0;
            saveBookings();
            printf("Booking canceled for Patient ID: %d\n", patientId);
            return;
        }
    }
    printf("No booking found for Patient ID: %d.\n", patientId);
}

// ------------------------------
// File handling for patients
// ------------------------------
void loadPatients()
{
    FILE *fp = fopen("patients.dat", "rb");
    if (fp)
    {
        patientCount = fread(patients, sizeof(Patient), MAX_PATIENTS, fp);
        fclose(fp);
    }
    else
    {
        printf("No patient data file found.\n");
    }
}

void savePatients()
{
    FILE *fp = fopen("patients.dat", "wb");
    if (fp)
    {
        fwrite(patients, sizeof(Patient), patientCount, fp);
        fclose(fp);
    }
    else
    {
        printf("Error saving patient data.\n");
    }
}
// View doctor suggestion for a patient
void viewSuggestions()
{
    int patientId;

    printf("<== View Suggestions ==>\n");
    printf("Enter Patient ID: ");
    scanf("%d", &patientId);

    if (patientId <= 0 || patientId > patientCount)
    {
        printf("Invalid Patient ID.\n");
        return;
    }

    if (strlen(patients[patientId - 1].suggestion) == 0)
    {
        printf("No suggestion available for this patient.\n");
    }
    else
    {
        printf("Suggestion for Patient %d: %s\n", patientId, patients[patientId - 1].suggestion);
    }
}
// ------------------------------
// File handling for doctors
// ------------------------------
void loadDoctors()
{
    FILE *fp = fopen("doctors.dat", "rb");
    if (fp)
    {
        doctorCount = fread(doctors, sizeof(Doctor), MAX_DOCTORS, fp);
        fclose(fp);
    }
    else
    {
        printf("No doctor data file found.\n");
    }
}

void saveDoctors()
{
    FILE *fp = fopen("doctors.dat", "wb");
    if (fp)
    {
        fwrite(doctors, sizeof(Doctor), doctorCount, fp);
        fclose(fp);
    }
    else
    {
        printf("Error saving doctor data.\n");
    }
}
// Add advice for a specific patient by a doctor
void giveSuggestion()
{
    int doctorId, patientId;

    printf("<== Give Suggestion ==>\n");
    printf("Enter Doctor ID: ");
    scanf("%d", &doctorId);
    clearInputBuffer();

    if (doctorId <= 0 || doctorId > doctorCount)
    {
        printf("Invalid Doctor ID.\n");
        return;
    }

    printf("Enter Patient ID: ");
    scanf("%d", &patientId);
    clearInputBuffer();

    if (patientId <= 0 || patientId > patientCount)
    {
        printf("Invalid Patient ID.\n");
        return;
    }

    printf("Enter your suggestion for the patient: ");
    readInputLine(doctors[doctorId - 1].suggestion, sizeof(doctors[doctorId - 1].suggestion));

    if (doctors[doctorId - 1].suggestion[0] == '\0')
    {
        printf("Suggestion cannot be empty.\n");
        return;
    }

    strcpy(patients[patientId - 1].suggestion, doctors[doctorId - 1].suggestion);

    printf("Suggestion given to patient %d by doctor %d.\n", patientId, doctorId);
}
// Clear all doctor-related records and remove matching bookings
void clearDoctorData()
{
    FILE *fp = fopen("doctors.dat", "wb");
    if (fp)
    {
        fclose(fp);
    }

    int oldDoctorCount = doctorCount;
    int writeIndex = 0;
    doctorCount = 0;

    for (int i = 0; i < bookingCount; i++)
    {
        if (bookings[i].doctorId > 0 && bookings[i].doctorId <= oldDoctorCount)
        {
            continue;
        }

        if (writeIndex != i)
        {
            bookings[writeIndex] = bookings[i];
        }
        writeIndex++;
    }

    bookingCount = writeIndex;
    saveBookings();

    printf("Doctor data cleared successfully.\n");
}

// Clear all appointment statuses from the system
void clearAllAppointments()
{
    for (int i = 0; i < bookingCount; i++)
    {
        bookings[i].isBooked = 0;
        bookings[i].isAccepted = 0;
    }
    saveBookings();

    printf("All appointments cleared.\n");
}

// Clear all patient-related records and remove matching bookings
void clearPatientData()
{
    FILE *fp = fopen("patients.dat", "wb");
    if (fp)
    {
        fclose(fp);
    }

    int oldPatientCount = patientCount;
    int writeIndex = 0;
    patientCount = 0;

    for (int i = 0; i < bookingCount; i++)
    {
        if (bookings[i].patientId > 0 && bookings[i].patientId <= oldPatientCount)
        {
            continue;
        }

        if (writeIndex != i)
        {
            bookings[writeIndex] = bookings[i];
        }
        writeIndex++;
    }

    bookingCount = writeIndex;
    saveBookings();

    printf("Patient data cleared successfully.\n");
}

// Add a new patient to the hospital system
void admitPatient()
{
    char currentDate[12];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(currentDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    printf("<== Admit Patient ==>\n\n");

    if (patientCount < MAX_PATIENTS)
    {
        patients[patientCount].id = patientCount + 1;
        strcpy(patients[patientCount].date, currentDate);

        printf("Enter Patient Name: ");
        readInputLine(patients[patientCount].name, sizeof(patients[patientCount].name));

        printf("Enter Patient Age: ");
        scanf("%d", &patients[patientCount].age);

        printf("Enter Patient Address: ");
        readInputLine(patients[patientCount].patientAddress, sizeof(patients[patientCount].patientAddress));

        printf("Enter Disease: ");
        readInputLine(patients[patientCount].disease, sizeof(patients[patientCount].disease));

        printf("Set Password for Patient: ");
        readInputLine(patients[patientCount].password, sizeof(patients[patientCount].password));

        patientCount++;
        savePatients();
        printf("\nPatient admitted successfully on %s.\n", currentDate);
    }
    else
    {
        printf("Patient list is full!\n");
    }
}

// Show all admitted patients in a table
void listofPatients()
{
    printf("\n===============================================\n");
    printf("                  Patient List                  \n");
    printf("===============================================\n");
    printf("%-5s | %-20s | %-10s | %-20s | %-20s | %-10s\n",
           "ID", "Name", "Age", "Address", "Disease", "Date");
    printf("-------------------------------------------------------------------------------"
           "----------------------\n");

    if (patientCount == 0)
    {
        printf("No patients admitted.\n");
    }
    else
    {
        for (int i = 0; i < patientCount; i++)
        {
            printf("%-5d | %-20s | %-10d | %-20s | %-20s | %-10s\n",
                   patients[i].id,
                   patients[i].name,
                   patients[i].age,
                   patients[i].patientAddress,
                   patients[i].disease,
                   patients[i].date);
        }
    }
    printf("===============================================\n");
}


// Remove a patient from the active patient list
void dischargePatient()
{
    printf("<== Discharge Patient ==>\n\n");
    int id, found = 0;

    printf("Enter Patient ID to discharge: ");
    scanf("%d", &id);

    for (int i = 0; i < patientCount; i++)
    {
        if (patients[i].id == id)
        {
            for (int j = i; j < patientCount - 1; j++)
            {
                patients[j] = patients[j + 1];
            }
            patientCount--;
            savePatients();
            found = 1;
            printf("\nPatient discharged successfully.\n");
            break;
        }
    }

    if (!found)
    {
        printf("\nPatient with ID %d not found!\n", id);
    }
}

// Add a new doctor to the system
void addDoctor()
{
    char currentDate[12];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(currentDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    printf("<== Add Doctor ==>\n\n");

    if (doctorCount < MAX_DOCTORS)
    {
        doctors[doctorCount].id = doctorCount + 1;
        strcpy(doctors[doctorCount].date, currentDate);

        printf("Enter Doctor Name: ");
        readInputLine(doctors[doctorCount].name, sizeof(doctors[doctorCount].name));

        printf("Enter Doctor Address: ");
        readInputLine(doctors[doctorCount].address, sizeof(doctors[doctorCount].address));

        printf("Enter Specialization: ");
        readInputLine(doctors[doctorCount].specialize, sizeof(doctors[doctorCount].specialize));

        printf("Set Password for Doctor: ");
        readInputLine(doctors[doctorCount].password, sizeof(doctors[doctorCount].password));

        doctorCount++;
        saveDoctors();
        printf("\nDoctor added successfully on %s.\n", currentDate);
    }
    else
    {
        printf("Doctor list is full!\n");
    }
}


// Show all doctors in a table
void listofDoctors()
{
    printf("\n=============================================\n");
    printf("                 Doctor List                 \n");
    printf("=============================================\n");
    printf("%-5s | %-20s | %-20s | %-20s | %-10s\n",
           "ID", "Name", "Address", "Specialization", "Date");
    printf("----------------------------------------------------------------------"
           "--------------------\n");

    if (doctorCount == 0)
    {
        printf("No doctors added.\n");
    }
    else
    {
        for (int i = 0; i < doctorCount; i++)
        {
            printf("%-5d | %-20s | %-20s | %-20s | %-10s\n",
                   doctors[i].id,
                   doctors[i].name,
                   doctors[i].address,
                   doctors[i].specialize,
                   doctors[i].date);
        }
    }
    printf("=============================================\n");
}



// Check whether the entered password matches the stored password
int login(const char *type, char *password)
{
    char enteredPassword[20];
    printf("Enter %s Password: ", type);
    readInputLine(enteredPassword, sizeof(enteredPassword));

    if (strcmp(enteredPassword, password) == 0)
    {
        return 1;
    }
    return 0;
}

// Create a patient profile from the public patient portal
void createPatientProfile()
{
    char currentDate[12];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(currentDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    printf("<== Create Patient Profile ==>\n\n");

    if (patientCount < MAX_PATIENTS)
    {
        int newId = patientCount + 1;
        patients[patientCount].id = newId;
        printf("Your ID No. is: %d\n", newId);

        strcpy(patients[patientCount].date, currentDate);

        printf("Enter Patient Name: ");
        readInputLine(patients[patientCount].name, sizeof(patients[patientCount].name));

        printf("Enter Patient Age: ");
        scanf("%d", &patients[patientCount].age);

        printf("Enter Patient Address: ");
        readInputLine(patients[patientCount].patientAddress, sizeof(patients[patientCount].patientAddress));

        printf("Enter Disease: ");
        readInputLine(patients[patientCount].disease, sizeof(patients[patientCount].disease));

        printf("Set Password for Patient: ");
        readInputLine(patients[patientCount].password, sizeof(patients[patientCount].password));

        patientCount++;
        savePatients();
        printf("\nPatient profile created successfully on %s.\n", currentDate);
    }
    else
    {
        printf("Patient list is full! Cannot create new profiles.\n");
    }
}


// Create a doctor profile from the doctor portal
void createDoctorProfile()
{
    char currentDate[12];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(currentDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    printf("<== Create Doctor Profile ==>\n\n");

    if (doctorCount < MAX_DOCTORS)
    {
        int newId = doctorCount + 1;
        doctors[doctorCount].id = newId;
        printf("Your ID No. is: %d\n", newId);

        strcpy(doctors[doctorCount].date, currentDate);

        printf("Enter Doctor Name: ");
        readInputLine(doctors[doctorCount].name, sizeof(doctors[doctorCount].name));

        printf("Enter Doctor Address: ");
        readInputLine(doctors[doctorCount].address, sizeof(doctors[doctorCount].address));

        printf("Enter Specialization: ");
        readInputLine(doctors[doctorCount].specialize, sizeof(doctors[doctorCount].specialize));

        printf("Set Password for Doctor: ");
        readInputLine(doctors[doctorCount].password, sizeof(doctors[doctorCount].password));

        doctorCount++;
        saveDoctors();
        printf("\nDoctor profile created successfully on %s.\n", currentDate);
    }
    else
    {
        printf("Doctor list is full! Cannot create new profiles.\n");
    }
}


// ------------------------------
// Doctor portal menu and actions
// ------------------------------
void doctorPortal()
{
    int doctorId;
    char password[20];
    int choice;

    while (1)
    {
        printf("1. Create New Doctor Profile\n");
        printf("2. Visit Existing Doctor Profile\n");
        printf("3. View Appointment\n");
        printf("4. Accept Booking from Patient\n");
        printf("5. Cancel Booking from Patient\n");
        printf("6. Give Suggestion to Patient\n");
        printf("7. Clear All Appointments\n");
        printf("0. Exit Doctor Portal\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 0)
        {
            printf("Exiting Doctor Portal...\n");
            return; // Exit the function, returning to the main menu
        }

        switch (choice)
        {
        case 1:
            createDoctorProfile();
            break;

        case 2:
            printf("Enter Doctor ID: ");
            scanf("%d", &doctorId);

            if (doctorId <= 0 || doctorId > doctorCount)
            {
                printf("Invalid Doctor ID. Please try again.\n");
                break;
            }

            strncpy(password, doctors[doctorId - 1].password, sizeof(password) - 1);
            password[sizeof(password) - 1] = '\0'; // Null-terminate the string

            if (login("Doctor", password))
            {
                printf("Login successful.\n");
                printf("Doctor: %s\n", doctors[doctorId - 1].name);
                // Additional doctor functionalities can be added here
            }
            else
            {
                printf("Invalid password.\n");
            }
            break;

        case 3:
            printf("Enter Doctor ID: ");
            scanf("%d", &doctorId);

            if (doctorId <= 0 || doctorId > doctorCount)
            {
                printf("Invalid Doctor ID. Please try again.\n");
                break;
            }


            viewDoctorAppointments(doctorId);
            break;

        case 4:
            printf("Enter Doctor ID: ");
            scanf("%d", &doctorId);

            if (doctorId <= 0 || doctorId > doctorCount)
            {
                printf("Invalid Doctor ID. Please try again.\n");
                break;
            }

            int patientId;
            printf("Enter Patient ID to accept booking: ");
            scanf("%d", &patientId);

            acceptBooking(doctorId, patientId);
            break;

        case 5:
            printf("Enter Doctor ID: ");
            scanf("%d", &doctorId);

            if (doctorId <= 0 || doctorId > doctorCount)
            {
                printf("Invalid Doctor ID. Please try again.\n");
                break;
            }

            printf("Enter Patient ID to cancel booking: ");
            scanf("%d", &patientId);

            cancelBooking(doctorId, patientId);
            break;

        case 6:
            giveSuggestion();
            break;
        case 7:
            clearAllAppointments();
            break;

        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}


// ------------------------------
// Patient portal menu and actions
// ------------------------------
void patientPortal()
{
    int patientId;
    char password[20];
    int choice;

    while (1) // Keep the user in the portal until they choose to exit
    {
        printf("\n<== Patient Portal ==>\n");
        printf("1. Create New Patient Profile\n");
        printf("2. Visit Existing Patient Profile\n");
        printf("3. View Doctor List\n");
        printf("4. Book Appointment\n");
        printf("5. View Suggestions from Doctor\n");
        printf("6. View Your Bookings\n");
        printf("7. Cancel Booking\n");
        printf("8. Clear All Bookings\n");
        printf("0. Exit Patient Portal\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 0)
        {
            printf("Exiting Patient Portal...\n");
            return; // Exit the function, returning to the main menu
        }

        switch (choice)
        {
        case 1:
            createPatientProfile(); // Function to create a patient profile
            break;

        case 2:
            printf("Enter Patient ID: ");
            scanf("%d", &patientId);

            if (patientId <= 0 || patientId > patientCount)
            {
                printf("Invalid Patient ID. Please try again.\n");
                break;
            }

            // Get the password from the patient
            strncpy(password, patients[patientId - 1].password, sizeof(password) - 1);
            password[sizeof(password) - 1] = '\0'; // Null-terminate the string

            if (login("Patient", password)) // Verify password
            {
                printf("Login successful.\n");
                printf("Patient: %s\n", patients[patientId - 1].name);
                // Additional patient functionalities can be added here
            }
            else
            {
                printf("Invalid password. Please try again.\n");
            }
            break;

        case 3:
            listofDoctors(); // Function to list all doctors
            break;

        case 4:
            printf("Enter Patient ID: ");
            scanf("%d", &patientId);

            if (patientId <= 0 || patientId > patientCount)
            {
                printf("Invalid Patient ID. Please try again.\n");
                break;
            }

            bookAppointment(patientId); // Book an appointment
            break;

        case 5:
            viewSuggestions(); // Function to view suggestions from doctors
            break;

        case 6:
            printf("Enter Patient ID to view your bookings: ");
            scanf("%d", &patientId);

            if (patientId <= 0 || patientId > patientCount)
            {
                printf("Invalid Patient ID. Please try again.\n");
                break;
            }

            viewPatientBookings(patientId); // View all bookings for this patient
            break;

        case 7:
            printf("Enter Patient ID to cancel your booking: ");
            scanf("%d", &patientId);

            if (patientId <= 0 || patientId > patientCount)
            {
                printf("Invalid Patient ID. Please try again.\n");
                break;
            }

            cancelPatientBooking(patientId);
            break;

        case 8:
            printf("Enter Patient ID to clear all bookings: ");
            scanf("%d", &patientId);

            if (patientId <= 0 || patientId > patientCount)
            {
                printf("Invalid Patient ID. Please try again.\n");
                break;
            }

            printf("Are you sure you want to clear all your bookings? (y/n): ");
            char confirmation;
            scanf(" %c", &confirmation);

            if (confirmation == 'y' || confirmation == 'Y')
            {
                for (int i = 0; i < bookingCount; i++)
                {
                    if (bookings[i].patientId == patientId)
                    {
                        bookings[i].isBooked = 0;
                        bookings[i].isAccepted = 0;
                    }
                }
                saveBookings();
                printf("All your bookings have been cleared.\n");
            }
            else
            {
                printf("Booking cancellation cancelled.\n");
            }
            break;

        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}




// Display all booked appointments for a specific doctor
void viewDoctorAppointments(int doctorId)
{
    printf("<== Doctor Appointments ==>\n");
    printf("%-5s %-12s %-20s %-10s\n", "ID", "Patient ID", "Appointment Date", "Status");

    int appointmentCount = 0;

    for (int i = 0; i < bookingCount; i++)
    {
        // Display only valid (booked) appointments
        if (bookings[i].doctorId == doctorId && bookings[i].isBooked)
        {
            appointmentCount++;
            printf("%-5d %-12d %-20s %-10s\n",
                   bookings[i].doctorId,
                   bookings[i].patientId,
                   bookings[i].appointmentDate,
                   "Booked");
        }
    }

    if (appointmentCount == 0)
    {
        printf("No appointments found for Doctor ID: %d\n", doctorId);
        printf("==============================================\n\n");
    }
}



// Show all appointment bookings for one patient
void viewPatientBookings(int patientId)
{
    int found = 0;

    printf("\n==============================================\n");
    printf("               Your Bookings                  \n");
    printf("==============================================\n");
    printf("Booking ID | Doctor ID | Appointment Date | Status\n");
    printf("-----------|-----------|------------------|----------\n");

    for (int i = 0; i < bookingCount; i++)
    {
        if (bookings[i].patientId == patientId)
        {
            printf("%-10d | %-9d | %-16s | %-9s\n",
                   bookings[i].bookingId,
                   bookings[i].doctorId,
                   bookings[i].appointmentDate,
                   bookings[i].isBooked
                   ? (bookings[i].isAccepted ? "Accepted" : "Pending")
                   : "Cancelled");
            found = 1;
        }
    }

    if (!found)
    {
        printf("\nNo bookings found for this patient.\n");
    }

    printf("==============================================\n");
}

void clearAllAppointmentsForPatient(int patientId) {
    int writeIndex = 0;

    // Iterate through all bookings
    for (int i = 0; i < bookingCount; i++) {
        // If the current booking doesn't belong to the patient, copy it to the new position
        if (bookings[i].patientId != patientId) {
            bookings[writeIndex] = bookings[i];
            writeIndex++;
        }
    }

    // Update the booking count to reflect the new number of bookings
    bookingCount = writeIndex;

    // Save the updated bookings to the file
    saveBookings();

    printf("All your appointments have been cleared.\n");
}
// Cancel a specific booking selected by a patient
void cancelPatientBooking(int patientId)
{
    int bookingId;
    printf("Enter the Booking ID to cancel: ");
    scanf("%d", &bookingId);
    clearInputBuffer();

    for (int i = 0; i < bookingCount; i++)
    {
        if (bookings[i].bookingId == bookingId && bookings[i].patientId == patientId)
        {
            if (bookings[i].isBooked && !bookings[i].isAccepted)
            {
                bookings[i].isBooked = 0;
                bookings[i].isAccepted = 0;
                printf("Booking ID %d has been successfully canceled.\n", bookingId);
                saveBookings();
                return;
            }

            printf("Booking is either already accepted or cancelled. Cannot cancel it.\n");
            return;
        }
    }

    printf("Invalid Booking ID or this booking does not belong to you.\n");
}

// Reserve an appointment for a patient with a doctor
void bookAppointment(int patientId)
{
    int doctorId;
    char appointmentDate[12];

    printf("Enter Doctor ID to book appointment: ");
    scanf("%d", &doctorId);
    clearInputBuffer();

    if (doctorId <= 0 || doctorId > doctorCount)
    {
        printf("Invalid Doctor ID.\n");
        return;
    }

    for (int i = 0; i < bookingCount; i++)
    {
        if (bookings[i].patientId == patientId && bookings[i].doctorId == doctorId && bookings[i].isBooked == 1)
        {
            printf("You already have an active booking with this doctor.\n");
            return;
        }
    }

    printf("Enter Appointment Date (DD/MM/YYYY): ");
    readInputLine(appointmentDate, sizeof(appointmentDate));

    if (!isValidDateFormat(appointmentDate))
    {
        printf("Invalid date format. Use DD/MM/YYYY.\n");
        return;
    }

    bookings[bookingCount].bookingId = nextBookingId++;
    bookings[bookingCount].patientId = patientId;
    bookings[bookingCount].doctorId = doctorId;
    strcpy(bookings[bookingCount].appointmentDate, appointmentDate);
    bookings[bookingCount].isBooked = 1;
    bookings[bookingCount].isAccepted = 0;
    bookingCount++;
    saveBookings();

    printf("Appointment booked successfully.\n");
}

// ------------------------------
// Admin portal menu and actions
// ------------------------------
void adminPortal()
{
    char enteredPassword[20];
    int attempts = 3;

    while (attempts > 0)
    {
        printf("<== Admin Portal ==>\n");
        printf("Enter Admin Password: ");
        readInputLine(enteredPassword, sizeof(enteredPassword));

        if (strcmp(enteredPassword, ADMIN_PASSWORD) == 0)
        {
            printf("Login successful.\n");
            break;
        }

        attempts--;
        if (attempts > 0)
        {
            printf("Incorrect password. You have %d attempt(s) left.\n", attempts);
        }
        else
        {
            printf("Too many incorrect attempts. Access denied.\n");
        }
    }

    if (attempts > 0)
    {

        printf("Accessing Admin Portal...\n");

    }
    else
    {
        return;
    }


    int choice;
    while (1)
    {
        printf("\n<== Admin Portal ==>\n");
        printf("1. Admit Patient\n");
        printf("2. List of All Patients\n");
        printf("3. Discharge Patient\n");
        printf("4. List of All Doctors\n");
        printf("5. Add Doctor\n");
        printf("6. Clear All Patient Data\n");
        printf("7. Clear All Doctor Data\n");
        printf("8. Log Out\n");

        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();  // Clear newline after scanf

        switch (choice)
        {
        case 1:
            admitPatient();
            break;
        case 2:
            listofPatients();

            break;
        case 3:
            dischargePatient();
            break;
        case 4:
            listofDoctors();
            break;
        case 5:
            addDoctor();
            break;
        case 6:
            clearPatientData();
            break;
        case 7:
            clearDoctorData();
            break;
        case 8:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice. Try again.\n");
        }
    }
}

// ------------------------------
// Entry point of the application
// ------------------------------
int main()
{
    loadPatients();
    loadDoctors();
    loadBookings();
    seedSampleData();
    int choice;
    while (1)

    {
        system("cls");
        printf("<== WELCOME TO JBF MEDICAL ==>\n");
        printf("1. Admin Portal\n");
        printf("2. Doctor Portal\n");
        printf("3. Patient Portal\n");
        printf("0. Exit\n\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 0:
            printf("Exiting...\n");
            exit(0);
        case 1:
            adminPortal();
            break;
        case 2:
            doctorPortal();
            break;
        case 3:
            patientPortal();
            break;
        default:
            printf("Invalid choice! Please try again.\n");
        }

        printf("\n\nPress any key to continue...");
        getchar();
        getchar();
    }

    return 0;
}


