/*
 * Anderson Walsh
 * jobsearch.cpp
 * Job Search Simulator
*/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <cctype>

using namespace std;

//global constants, MAXLEN = max string length
//MAXJOBS is both max job skills and max length of
//Job array ERROR & SUCCESS = return values
const int ERROR = 1;
const int SUCCESS = 0;
const int MAXLEN = 256;
const int MAXJOBS = 10;

//custom datatype to store job details
struct Job
{
    char title[MAXLEN];
    char skill[MAXJOBS][MAXLEN];
    char company[MAXLEN];
    int salary, num_skills;
};

//function prototypes
void sortStructs(Job[], int);
int fileHandling(ifstream &);
int getStructs(ifstream &, Job[]);
void promptUser(int &, char[]);
void searchQuery(Job[], int, int, char *);
void lowerString(char *, char *);
int findString(char *, char *);
void outputResults(Job, bool);

int main()
{
    //variable declarations for input file,
    //Job array, and user input
    ifstream input;
    Job job[MAXJOBS];
    char query[MAXLEN];
    int num_jobs, option;
    if (!fileHandling(input))
    {                                      //if file opening is successful
        num_jobs = getStructs(input, job); //read in structs from file and track number of structs
        sortStructs(job, num_jobs);        //alphabetize array by job.title
        //loop until menu option 3
        do
        {
            promptUser(option, query);                 //get input from user
            searchQuery(job, option, num_jobs, query); //print corresponding output
        } while (option != 3);
    }
}

//precondition: ifstream& input exists
int fileHandling(ifstream &input)
{
    string filename;
    cout << "Provide input file for job data: ";
    getline(cin, filename);
    //convert filename to c string for open()
    input.open(filename.c_str());
    //if file is empty or nonexistent, error. else success
    if (input.fail())
    {
        cout << "File " << filename << " does not exist. Exiting.\n";
        return ERROR;
    }
    else if (input.peek() == EOF)
    {
        cout << "File " << filename << " is empty. Exiting.\n";
        input.close();
        return ERROR;
    }
    else
        return SUCCESS;
} //postcondition: input file is open, SUCCESS, or file is empty/non-existent, ERROR

//precondition: input file is open and contains data, Job array exists
int getStructs(ifstream &input, Job job[])
{
    int i = 0;
    //store string to be converted to int salary
    char temp[MAXLEN];
    //loop until array is full or EOF is reached
    while (input.peek() != EOF && i < MAXJOBS)
    {
        //fill ith element of array with file data
        input.getline(job[i].title, MAXLEN);
        input.getline(temp, MAXLEN);
        job[i].num_skills = atoi(temp);
        for (int k = 0; k < job[i].num_skills; k++)
        {
            input.getline(job[i].skill[k], MAXLEN);
        }
        input.getline(temp, MAXLEN);
        //convert char array temp to int for salary
        job[i].salary = atoi(temp);
        input.getline(job[i].company, MAXLEN);
        //move to next element of array
        i++;
    }
    //close file and return number of jobs read in
    input.close();
    return i;
} //postcondition: Job array is filled with job data from file, input is closed

//precondition: Job array is filled, size of array is tracked
void sortStructs(Job job[MAXJOBS], int size)
{
    int smallest = 0;
    Job temp;
    //selection sort algorithm using strcmp to find lowest alphanumeric title on each iteration
    for (int i = 0; i < size; i++)
    {
        smallest = i;
        for (int j = i + 1; j < size; j++)
        {
            if (strcmp(job[smallest].title, job[j].title) > 0)
                smallest = j;
        }
        temp = job[i];
        job[i] = job[smallest];
        job[smallest] = temp;
    }
} //postcondition: job array is alphabetized by job.title

//precondition: variables for storage of user input exist
void promptUser(int &option, char query[])
{
    cout << "1 Search for a job by skill\n"
         << "2 Search for job by title\n3 Quit\nOption>";
    cin >> option;
    //print corresponding prompt based on user input, store in query
    switch (option)
    {
    case 1:
        cout << "Supply job skill: ";
        cin.ignore();
        cin.getline(query, MAXLEN);
        break;
    case 2:
        cout << "Supply job title: ";
        cin.ignore();
        cin.getline(query, MAXLEN);
        break;
    default:
        break;
    }
} //postcondition: user menu option is chosen and corresponding search query is stored

//precondition: job array, array size, menu option and search query are stored
void searchQuery(Job job[], int option, int num_jobs, char *query)
{
    //c strings to store job title/job skills and search query for comparison
    char substring[MAXLEN] = {'\0'};
    char target[MAXLEN] = {'\0'};
    bool print_table = true;
    switch (option)
    {
    case 1: //if menu option 1, iterate through all job skills
        for (int i = 0; i < num_jobs; i++)
        {
            lowerString(query, substring);
            //evalaute query and job skills converted to lowercase
            //(case insensitive search)
            for (int k = 0; k < job[i].num_skills; k++)
            {
                lowerString(job[i].skill[k], target);
                //if match with user query is found, print job
                if (findString(substring, target))
                {
                    outputResults(job[i], print_table);
                    k = job[i].num_skills;
                    print_table = false;
                }
            }

        } //if no match is found, inform user
        if (print_table)
            cout << "No job listings contain that skill.\n";
        break;
    case 2: //if menu option 2, iterate through all job titles
        for (int i = 0; i < num_jobs; i++)
        {
            //conversion for case insensitive comparison
            lowerString(query, substring);
            lowerString(job[i].title, target);
            if (findString(substring, target))
            {
                //if match with user query is found, print job
                outputResults(job[i], print_table);
                print_table = false;
            }
        } //if no match is found, inform user
        if (print_table)
            cout << "No job listings match that title.\n";
        break;
    case 3: //if menu option 3, goodbye!
        cout << "Thank you for using the Job Search Simulator.\n";
        break;
    default: //if user picks out of bounds num
        cout << "Invalid option, please try again.\n";
        break;
    }
} //postcondition: every instance of user search query is printed in a table

//precondition: two c strings, lower and target exist
void lowerString(char *lower, char *target)
{
    int i;
    for (i = 0; i < strlen(lower); i++)
    {
        target[i] = tolower(lower[i]);
    }
    target[i] = '\0';
} //postcondition: target gets contents of lower, converted to lowercase

//precondition: two c strings, substring and target exist
int findString(char *substring, char *target)
{
    int counter = 0;
    //if substring is longer than string to be searched, return 0
    if (strlen(target) < strlen(substring))
        return counter;
    //evaluate target in chunks of size strlen(substring)
    //if all characters match, substring is a member of target
    for (int i = 0; i <= strlen(target) - strlen(substring); i++)
    {
        for (int k = 0; k < strlen(substring); k++)
        {
            if (substring[k] == target[i + k])
                counter++;
        }
        if (counter == strlen(substring))
            return counter;
        counter = 0;
    }
    return counter;
} //postcondition: if substring is found in target, return size of substring. else return 0

//precondition: Job data and print_table exist
void outputResults(Job job, bool print_table)
{
    //print columns if they have not already been printed
    if (print_table)
    {
        cout << "Job Title" << setw(39) << right << "Salary" << setw(15) << "Company" << endl
             << setfill('-') << setw(79) << '-' << endl
             << setfill(' ');
    }
    cout << setw(42) << left << job.title << '$' << setw(12) << job.salary << job.company;
    //print each job skill
    for (int i = 0; i < job.num_skills; i++)
    {
        cout << "\n\t" << job.skill[i];
    }
    cout << endl;
} //postcondition: Job data is printed out with proper formatting, on first job the columns are printed
