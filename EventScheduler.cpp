#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <vector>
#include <cctype>
#include <string>
using namespace std;

string EventsFile = "Events.txt";
string ArchiveFile = "Archive.txt";

enum enChoises 
{
    ShowEvents = 1,
    AddEvent = 2,
    DeleteEvent = 3,
    UpdateEvent = 4,
    FindEvent = 5,
    Archive = 6,
    Exit = 7
};

struct stDate
{
    int Day, Month, Year;
    string date_to_string()
    {
        return (to_string(Day) + "/" + to_string(Month) + "/" + to_string(Year));
    }
};

struct stTime
{
    int Hour, Minute;
    string time_to_string()
    {
        string period = (Hour >= 12) ? "PM" : "AM";
        int h = (Hour > 12) ? Hour - 12 : Hour;
        if (h == 0) h = 12;
        return to_string(h) + ":" + (Minute < 10 ? "0" : "") + to_string(Minute) + " " + period;
    }
};

struct stEvent
{
    string ID;
    string Name = "No Name Is Added";
    string Place = "No Place Is Added";

    stDate StartDate;
    stDate EndDate;
    stTime StartTime;

    int ReminderMinutes;
    bool Done = false;
};

enChoises ReadChoise()
{
    int Choise;
    cout << "Choose what do you want to do ? [1 to 7]? ";
    cin >> Choise;
    return (enChoises)Choise;
}

bool IsLeapYear(int Year)
{
    return (Year % 400 == 0) || (Year % 4 == 0 && Year % 100 != 0);
}

int DaysInMonth(int Month, int Year)
{
    if (Month == 2)
        return IsLeapYear(Year) ? 29 : 28;

    if (Month == 4 || Month == 6 || Month == 9 || Month == 11)
        return 30;

    return 31;
}

bool IsValidDate(stDate d)
{
    if (d.Year <= 0) return false;
    if (d.Month < 1 || d.Month > 12) return false;
    if (d.Day < 1 || d.Day > DaysInMonth(d.Month, d.Year)) return false;
    return true;
}

bool IsValidTime(stTime t)
{
    if (t.Hour < 0 || t.Hour > 23) return false;
    if (t.Minute < 0 || t.Minute > 59) return false;
    return true;
}

stDate ReadValidDate(string Message)
{
    stDate d;

    while (true)
    {
        cout << Message << "(D M Y):";
        cin >> d.Day >> d.Month >> d.Year;

        if (IsValidDate(d))
            return d;

        cout << "\nInvalid date. Please enter a real calendar date.\n";
    }
}

stTime ReadValidTime(string Message)
{
    stTime t;

    while (true)
    {
        cout << Message << " (H M)  : ";
        cin >> t.Hour >> t.Minute;

        if (IsValidTime(t))
            return t;

        cout << "\nInvalid time. Use 24-hour format.\n";
    }
}

bool IsStartBeforeOrEqual(stDate s, stDate e)
{
    if (s.Year != e.Year) return s.Year < e.Year;
    if (s.Month != e.Month) return s.Month < e.Month;
    return s.Day <= e.Day;
}

bool IsIntersecting(stEvent NewEvent, vector<stEvent>& vEvents)
{
    for (stEvent& E : vEvents)
    {
        if (!IsStartBeforeOrEqual(NewEvent.EndDate, E.StartDate) &&
            !IsStartBeforeOrEqual(E.EndDate, NewEvent.StartDate))
        {
            return true;
        }
    }
    return false;
}

void ReadEndAndStartDate(stEvent &E)
{
    do
    {
        E.StartDate = ReadValidDate("Start Date ");
        E.EndDate   = ReadValidDate("End Date   ");

        if (!IsStartBeforeOrEqual(E.StartDate, E.EndDate))
            cout << "\nStart date must be before or equal to end date.\n";

    } while (!IsStartBeforeOrEqual(E.StartDate, E.EndDate));

}

stEvent ReadNewEvent(vector<stEvent>& vEvents, string ID = "")
{
    stEvent E;

    if (ID != "")
    {
        cout << "ID                : " << ID;
    }
    else
    {
        E.ID = to_string(vEvents.size() + 1);
        cout << "\nEvent ID          : " << E.ID << endl;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Name              : ";
    getline(cin, E.Name);

    ReadEndAndStartDate(E);

    E.StartTime = ReadValidTime("Start Time");

    cout << "Reminder (minutes): ";
    cin >> E.ReminderMinutes;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Place             : ";
    getline(cin, E.Place);

    if (IsIntersecting(E, vEvents))
    {
        cout << "\n\nEvent intersects with another event. Operation refused.\n";
        return ReadNewEvent(vEvents);
    }

    return E;
}

string ReadID()
{
    string ID;
    cout << "\nPlease enter account number to perform operation on? ";
    cin >> ID;
    return ID;
}

vector<string> SplitString(string stLine, string delim)
{
    vector<string> vString;
    string sWard;
    short pos = 0;
    while((pos = stLine.find(delim)) != string::npos)
    {
        sWard = stLine.substr(0, pos);
        if(sWard != "")
        {
            vString.push_back(sWard);
        }
        stLine.erase(0, pos + delim.length());
    }
    if(stLine != "")
    {
        vString.push_back(stLine);
    }

    return vString;
}

stEvent CovertLineToRecord(string Line)
{
    stEvent Event;
    vector<string> v = SplitString(Line, "#//#");

    Event.ID = v[0];
    Event.Name = v[1];
    Event.Place = v[2];

    Event.StartDate.Day = stoi(v[3]);
    Event.StartDate.Month = stoi(v[4]);
    Event.StartDate.Year = stoi(v[5]);

    Event.EndDate.Day = stoi(v[6]);
    Event.EndDate.Month = stoi(v[7]);
    Event.EndDate.Year = stoi(v[8]);

    Event.StartTime.Hour = stoi(v[9]);
    Event.StartTime.Minute = stoi(v[10]);

    Event.ReminderMinutes = stoi(v[11]);
    Event.Done = stoi(v[12]);

    return Event;
}

string ConvertRecordToLine(stEvent E)
{
    string s = "#//#";

    return E.ID + s +
           E.Name + s +
           E.Place + s +
           to_string(E.StartDate.Day) + s +
           to_string(E.StartDate.Month) + s +
           to_string(E.StartDate.Year) + s +
           to_string(E.EndDate.Day) + s +
           to_string(E.EndDate.Month) + s +
           to_string(E.EndDate.Year) + s +
           to_string(E.StartTime.Hour) + s +
           to_string(E.StartTime.Minute) + s +
           to_string(E.ReminderMinutes) + s +
           to_string(E.Done);
}

vector<stEvent> GetEventsFromFile(string FileName)
{
    vector<stEvent> vEvents;
    fstream MyFile;

    MyFile.open(FileName, ios::in);
    
    if(MyFile.is_open())
    {
        string Line;
        while(getline(MyFile, Line))
        {
            vEvents.push_back(CovertLineToRecord(Line));
        }
        MyFile.close();
    }
    return vEvents;
}

void PrintRecord(stEvent Event)
{
    cout << "\nEvent Details:\n";
    cout << "ID           : " << Event.ID << endl;
    cout << "Name         : " << Event.Name << endl;
    cout << "Place        : " << Event.Place << endl;
    cout << "Start Date   : " << Event.StartDate.date_to_string() << endl;
    cout << "End Date     : " << Event.EndDate.date_to_string() << endl;
    cout << "Start Time   : " << Event.StartTime.time_to_string() << endl;
    cout << "Reminder     : " << Event.ReminderMinutes << " minutes\n";
    cout << "Done         : " << (Event.Done ? "Yes" : "No") << endl;
}

void SaveEventToArchive(stEvent &Event, string FileName)
{
    string line;
    fstream MyFile;
    MyFile.open(FileName, ios::app);

    if(MyFile.is_open())
    {
        line = ConvertRecordToLine(Event);
        MyFile << line;
        MyFile.close();
    }
}
void SaveEventsDataToFile(vector<stEvent> &vEvents, string FileName)
{
    string line;
    fstream MyFile;
    MyFile.open(FileName, ios::out);

    if(MyFile.is_open())
    {
        for (stEvent &Event : vEvents)
        {
            if (Event.Done == true)
            {
                SaveEventToArchive(Event, ArchiveFile);
            }
            else
            {
                line = ConvertRecordToLine(Event);
                MyFile << line << endl;
            }
        }
        MyFile.close();
    }
}

bool FindEventByID(string ID, stEvent &Event, vector<stEvent> &vEvents)
{
    for(stEvent &E : vEvents)
    {
        if(E.ID == ID)
        {
            Event = E;
            return true;
        }
    }
    return false;
}

void UpdataEventInFile(vector<stEvent> &vEvents)
{
    system("cls");
    cout << "=================================================" << endl;
    cout << "\t\tUpdate Event Screen" << endl;
    cout << "=================================================" << endl;

    vEvents = GetEventsFromFile(EventsFile);

    string ID = ReadID();
    stEvent Event;
    char Answer = 'N';

    if (FindEventByID(ID, Event, vEvents))
    {
        PrintRecord(Event);

        cout << "\n\nAre you sure you want update this Event? y/n ? ";         
        cin >> Answer;
        if(tolower(Answer) == 'y')
        {
            for (stEvent &E : vEvents)
            {
                if(E.ID == ID)
                {
                    E = ReadNewEvent(vEvents, ID);
                    break;
                }
            }
            SaveEventsDataToFile(vEvents, EventsFile);
        }
    }
    else 
    {
    cout << "\nEvent with account number (" << ID << ") is not found !" << endl;
    }
    cout << "\n\n\n";
    cout << "Press Enter To Go Back ...";
    system("pause > 0");
}

void MarkDone(string ID, vector<stEvent> &vEvents)
{
    for (stEvent &E : vEvents)
    {
        if(E.ID == ID)
        {
            E.Done = true;
        }
    }
}

void FindEventByID(vector<stEvent> &vEvents)
{
    system("cls");
    cout << "=================================================" << endl;
    cout << "\t\tSearching Event Screen" << endl;
    cout << "=================================================" << endl;    
    string ID = ReadID();

    vEvents = GetEventsFromFile(EventsFile);
    bool NotExisted = true;

    for(stEvent &E : vEvents)
    {
        if(E.ID == ID)
        {
            PrintRecord(E);
            NotExisted = false;
        }
    }
    if(NotExisted)
    {
        cout << "\nEvent with account number (" << ID << ") is not found !" << endl;
    }

    cout << "\n\n\n";
    cout << "Press Enter To Go Back ...";
    system("pause > 0");
}

void DeleteEventFromFile(vector<stEvent> &vEvents)
{
    system("cls");
    cout << "=================================================" << endl;
    cout << "\t\tDelete Event Screen" << endl;
    cout << "=================================================" << endl;    string ID = ReadID();

    vEvents = GetEventsFromFile(EventsFile);

    stEvent Event;
    char Answer = 'N';

    if (FindEventByID(ID, Event, vEvents))
    {
        PrintRecord(Event);

        cout << "\n\nAre you sure you want delete this Event? y/n ? ";         
        cin >> Answer;
        if(tolower(Answer) == 'y')
        {
            MarkDone(ID, vEvents);
            SaveEventsDataToFile(vEvents, EventsFile);
            vEvents = GetEventsFromFile(EventsFile);
        }
    }
    else 
    {
        cout << "\nEvent with account number (" << ID << ") is not found !" << endl;
    }
    cout << "\n\n\n";
    cout << "Press Enter To Go Back ...";
    system("pause > 0");
}

char AddMore()
{
    char answer;
    cout << "\nDo you want to add more ? [Yes]: Y , [No]: N ? ";
    cin >> answer;
    return answer;
}

void CheckID(string &ID, vector<stEvent> &vEvents)
{
    for(int i = 0 ; i < vEvents.size() ; i++)
    {
        if(vEvents[i].ID == ID)
        {
            cout << "\nEvent with [" << ID << "] already exists. Enter another Account Number? ";
            getline(cin >> ws , ID);
            i = 0;
        }
    }
}

void AddNewEvent(vector<stEvent> &vEvents)
{
    system("cls");
    cout << "=================================================" << endl;
    cout << "\t\tAdding New Event Screen" << endl;
    cout << "=================================================" << endl;

    vEvents = GetEventsFromFile(EventsFile);
    
    stEvent NewEvent;
    char More = 'N';
    do
    {
        NewEvent = ReadNewEvent(vEvents);
        vEvents.push_back(NewEvent);
        SaveEventsDataToFile(vEvents, EventsFile);
        More = AddMore();

    } while(toupper(More) == 'Y');

    cout << "\n\n\n";
    cout << "Press Enter To Go Back ...";
    system("pause > 0");
}

void PrintHeader(int EventsNumber)
{
    cout << "\t\t\t\t\tEvent List " << EventsNumber << " Event(s)." << endl;

    cout << setw(110) << left
         << "--------------------------------------------------------------------------------------------------------------"
         << endl;
    cout << setw(12) << left << "| Event ID"
         << setw(25) << left << "| Event Name"
         << setw(15) << left << "| Start Date"
         << setw(15) << left << "| End Date"
         << setw(12) << left << "| Start Time"
         << setw(15) << left << "| Place"
         << setw(10) << left << "| Done"
         << endl;
    cout << setw(110) << left
         << "--------------------------------------------------------------------------------------------------------------"
         << endl;
}

void PrintRecords(vector<stEvent> &vEvents)
{
    for (int i = 0; i < vEvents.size(); i++)
    {
        cout << setw(12) << left << "| " + vEvents[i].ID
             << setw(25) << left << "| " + vEvents[i].Name
             << setw(15) << left << "| " + vEvents[i].StartDate.date_to_string()
             << setw(15) << left << "| " + vEvents[i].EndDate.date_to_string()
             << setw(12) << left << "| " + vEvents[i].StartTime.time_to_string()
             << setw(15) << left << "| " + vEvents[i].Place
             << setw(10) << left << string("| ") + (vEvents[i].Done ? "Yes" : "No")
             << endl;
    }

    cout << setw(110) << left
         << "--------------------------------------------------------------------------------------------------------------"
         << endl;
}

void ShowEventList(vector<stEvent> &vClinets)
{
    system("cls");
    vector<stEvent> vEvents = GetEventsFromFile(EventsFile);
    PrintHeader(vEvents.size());
    PrintRecords(vEvents);
    cout << "\n\n\n";
    cout << "Press Enter To Go Back ...";
    system("pause > 0");
}

void PrintChoises()
{
    cout << "=================================================" << endl;
    cout << "\t\tMain Menue Screen" << endl;
    cout << "=================================================" << endl;
    cout << "\t[1] Show Event List.\n";
    cout << "\t[2] Add New Event.\n";
    cout << "\t[3] Delete Event.\n";
    cout << "\t[4] Update Event Info.\n";
    cout << "\t[5] Find Event.\n";
    cout << "\t[6] Archive.\n";
    cout << "\t[7] Exit.\n";
    cout << "=================================================" << endl;
}

void ShowDeletedEvents(string FileName)
{
    system("cls");
    vector<stEvent> vEvents = GetEventsFromFile(FileName);
    cout << "\n\t\t\t\t\t\tArchive\n";
    PrintHeader(vEvents.size());
    PrintRecords(vEvents);
    cout << "\n\n\n";
    cout << "Press Enter To Go Back ...";
    system("pause > 0");
}

void MainMenueScreen()
{
    vector<stEvent> vClinets;

    while (true)
    {
        system("cls");
        PrintChoises();
        enChoises Choise = ReadChoise();

        switch (Choise)
        {
            case enChoises::ShowEvents : ShowEventList(vClinets); break;
            case enChoises::AddEvent   : AddNewEvent(vClinets); break;
            case enChoises::FindEvent  : FindEventByID(vClinets); break;
            case enChoises::DeleteEvent: DeleteEventFromFile(vClinets); break;
            case enChoises::UpdateEvent: UpdataEventInFile(vClinets); break;
            case enChoises::Archive    : ShowDeletedEvents(ArchiveFile); break;
            case enChoises::Exit       : return;
        }
    }
}

int main()
{
    MainMenueScreen();
    return 0;
}