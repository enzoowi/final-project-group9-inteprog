#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cctype>
#include <memory>
#include <utility>

using namespace std;

// Forward declarations
class CinemaBookingSystem;

// Helper function to clear input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Helper function for numeric input validation
int getValidChoice(int min, int max) {
    int choice;
    bool valid = false;
    while (!valid) {
        cout << "Enter your choice (" << min << "-" << max << "): ";
        if (cin >> choice && choice >= min && choice <= max) {
            clearInputBuffer();
            valid = true;
        } else {
            cin.clear();
            clearInputBuffer();
            cout << "Invalid input. Please try again." << endl;
        }
    }
    return choice;
}

// Helper function for yes/no confirmations
bool getConfirmation(const string& prompt) {
    char confirm;
    bool valid = false;
    while (!valid) {
        cout << prompt << " (Y/N): ";
        cin >> confirm;
        clearInputBuffer();
        confirm = toupper(confirm);
        if (confirm == 'Y') return true;
        if (confirm == 'N') return false;
        cout << "Invalid input. Please enter Y or N." << endl;
    }
    return false;
}

// Helper function to validate date format (YYYY-MM-DD)
bool isValidDate(const string& date) {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    
    int year, month, day;
    try {
        year = stoi(date.substr(0, 4));
        month = stoi(date.substr(5, 2));
        day = stoi(date.substr(8, 2));
    } catch (...) {
        return false;
    }
    
    if (year < 2023 || month < 1 || month > 12 || day < 1 || day > 31) return false;
    return true;
}

// Helper function to validate time format (HH:MM)
bool isValidTime(const string& time) {
    if (time.length() != 5 || time[2] != ':') return false;
    
    int hour, minute;
    try {
        hour = stoi(time.substr(0, 2));
        minute = stoi(time.substr(3, 2));
    } catch (...) {
        return false;
    }
    
    return hour >= 0 && hour < 24 && minute >= 0 && minute < 60;
}


class Schedule {
private:
    string date;
    string time;

public:
    Schedule(string d, string t) : date(d), time(t) {}

    string getDate() const { return date; }
    string getTime() const { return time; }
    string getFullSchedule() const { return date + " " + time; }

    void display() const {
        cout << date << " at " << time;
    }
};

class User {
protected:
    string username;
    string password;
    int userID;
    static int nextUserID;

public:
    User(string uname, string pwd) : username(uname), password(pwd), userID(nextUserID++) {}
    virtual ~User() = default;

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    int getUserID() const { return userID; }

    virtual void displayMenu() = 0;
    virtual string getUserType() const = 0;
};
int User::nextUserID = 1;

class Customer : public User {
private:
    string name;

public:
    Customer(string uname, string pwd, string n) : User(uname, pwd), name(n) {}

    string getName() const { return name; }
    string getUserType() const override { return "CUSTOMER"; }

    void displayMenu() override;
    void viewMovies();
    void bookTicket();
    void viewBookings();
    void editBooking();
    void cancelBooking();
};

class Admin : public User {
public:
    Admin(string uname, string pwd) : User(uname, pwd) {}
    string getUserType() const override { return "ADMIN"; }

    void displayMenu() override;
    void addMovie();
    void editMovie();
    void deleteMovie();
    void viewAllBookings();
    void manageSeats();
    void manageSchedules();
    void generateReports();
};

class Movie {
private:
    int movieID;
    string title;
    string genre;
    vector<Schedule> schedules;
    static int nextMovieID;

public:
    Movie(string t, string g) : title(t), genre(g), movieID(nextMovieID++) {}

    int getMovieID() const { return movieID; }
    string getTitle() const { return title; }
    string getGenre() const { return genre; }
    const vector<Schedule>& getSchedules() const { return schedules; }

    void addSchedule(const Schedule& schedule) { schedules.push_back(schedule); }
    void removeSchedule(int index) {
        if (index >= 0 && index < schedules.size()) {
            schedules.erase(schedules.begin() + index);
        }
    }

    void displayDetails() const {
        cout << "\nMovie ID: " << movieID << endl
             << "Title: " << title << endl
             << "Genre: " << genre << endl
             << "Schedules:" << endl;
        for (size_t i = 0; i < schedules.size(); i++) {
            cout << "  " << i+1 << ". ";
            schedules[i].display();
            cout << endl;
        }
    }
};
int Movie::nextMovieID = 1;

class Booking {
private:
    int bookingID;
    string customerUsername;
    int movieID;
    Schedule schedule;
    string seat;
    static int nextBookingID;

public:
    Booking(string username, int mID, const Schedule& sched, string st) :
        customerUsername(username), movieID(mID), schedule(sched), seat(st), bookingID(nextBookingID++) {}

    int getBookingID() const { return bookingID; }
    string getCustomerUsername() const { return customerUsername; }
    int getMovieID() const { return movieID; }
    Schedule getSchedule() const { return schedule; }
    string getSeat() const { return seat; }

    void displayDetails(const vector<Movie>& movies) const {
        string movieTitle = "Unknown";
        for (const auto& movie : movies) {
            if (movie.getMovieID() == movieID) {
                movieTitle = movie.getTitle();
                break;
            }
        }
        
        cout << "\nBooking ID: " << bookingID << endl
             << "Customer: " << customerUsername << endl
             << "Movie: " << movieTitle << endl
             << "Date: " << schedule.getDate() << endl
             << "Time: " << schedule.getTime() << endl
             << "Seat: " << seat << endl;
    }
};
int Booking::nextBookingID = 1;

class CinemaBookingSystem {
private:
    static CinemaBookingSystem* instance;
    vector<unique_ptr<User>> users;
    vector<Movie> movies;
    vector<Booking> bookings;
    map<pair<int, string>, map<string, bool>> movieSeats; // <movieID, date> -> seats

    CinemaBookingSystem() { loadData(); }

    void initializeSeatsForMovie(int movieID, const string& date) {
        map<string, bool> seats;
        for (char row = 'A'; row <= 'H'; row++) {
            for (int num = 1; num <= 10; num++) {
                seats[string(1, row) + to_string(num)] = true;
            }
        }
        movieSeats[{movieID, date}] = seats;
    }

    void loadData() {
        // Load users
        ifstream userFile("users.txt");
        if (userFile.is_open()) {
            string line;
            while (getline(userFile, line)) {
                vector<string> tokens;
                string token;
                istringstream tokenStream(line);
                while (getline(tokenStream, token, ',')) {
                    tokens.push_back(token);
                }

                try {
                    if (tokens.size() >= 3) {
                        if (tokens[0] == "CUSTOMER" && tokens.size() >= 4) {
                            users.push_back(make_unique<Customer>(tokens[1], tokens[2], tokens[3]));
                        } else if (tokens[0] == "ADMIN") {
                            users.push_back(make_unique<Admin>(tokens[1], tokens[2]));
                        }
                    }
                } catch (...) {
                    cerr << "Error loading user: " << line << endl;
                }
            }
            userFile.close();
        }
        
        // Load movies
        ifstream movieFile("movies.txt");
        if (movieFile.is_open()) {
            string line;
            while (getline(movieFile, line)) {
                vector<string> tokens;
                string token;
                istringstream tokenStream(line);
                while (getline(tokenStream, token, ',')) {
                    tokens.push_back(token);
                }

                if (tokens.size() >= 3) {
                    Movie movie(tokens[1], tokens[2]);
                    for (size_t i = 3; i < tokens.size(); i += 2) {
                        if (i + 1 < tokens.size()) {
                            movie.addSchedule(Schedule(tokens[i], tokens[i+1]));
                        }
                    }
                    movies.push_back(movie);
                }
            }
            movieFile.close();
        }

        // Load bookings
        ifstream bookingFile("bookings.txt");
        if (bookingFile.is_open()) {
            string line;
            while (getline(bookingFile, line)) {
                vector<string> tokens;
                string token;
                istringstream tokenStream(line);
                while (getline(tokenStream, token, ',')) {
                    tokens.push_back(token);
                }

                if (tokens.size() >= 6) {
                    bookings.push_back(Booking(
                        tokens[1], 
                        stoi(tokens[2]), 
                        Schedule(tokens[3], tokens[4]), 
                        tokens[5]
                    ));
                }
            }
            bookingFile.close();
        }

        // Load seats
        ifstream seatFile("seats.txt");
        if (seatFile.is_open()) {
            string line;
            while (getline(seatFile, line)) {
                vector<string> tokens;
                string token;
                istringstream tokenStream(line);
                while (getline(tokenStream, token, ',')) {
                    tokens.push_back(token);
                }

                if (tokens.size() >= 4) {
                    int movieID = stoi(tokens[0]);
                    string date = tokens[1];
                    string seat = tokens[2];
                    bool available = (tokens[3] == "1");
                    movieSeats[{movieID, date}][seat] = available;
                }
            }
            seatFile.close();
        } else {
            // Initialize seats for existing movies
            for (const auto& movie : movies) {
                for (const auto& schedule : movie.getSchedules()) {
                    initializeSeatsForMovie(movie.getMovieID(), schedule.getDate());
                }
            }
        }
    }

public:
    static CinemaBookingSystem* getInstance() {
        if (!instance) instance = new CinemaBookingSystem();
        return instance;
    }

    static void cleanup() {
        delete instance;
        instance = nullptr;
    }

    ~CinemaBookingSystem() { saveData(); }

    void saveData() {
        // Save users
        ofstream userFile("users.txt");
        if (userFile.is_open()) {
            for (const auto& user : users) {
                if (user->getUserType() == "CUSTOMER") {
                    Customer* cust = dynamic_cast<Customer*>(user.get());
                    if (cust) {
                        userFile << "CUSTOMER," << cust->getUsername() << "," 
                                << cust->getPassword() << "," << cust->getName() << endl;
                    }
                } else if (user->getUserType() == "ADMIN") {
                    userFile << "ADMIN," << user->getUsername() << "," << user->getPassword() << endl;
                }
            }
            userFile.close();
        }

        // Save movies
        ofstream movieFile("movies.txt");
        if (movieFile.is_open()) {
            for (const auto& movie : movies) {
                movieFile << movie.getMovieID() << "," << movie.getTitle() << "," << movie.getGenre();
                for (const auto& sched : movie.getSchedules()) {
                    movieFile << "," << sched.getDate() << "," << sched.getTime();
                }
                movieFile << endl;
            }
            movieFile.close();
        }

        // Save bookings
        ofstream bookingFile("bookings.txt");
        if (bookingFile.is_open()) {
            for (const auto& booking : bookings) {
                bookingFile << booking.getBookingID() << "," << booking.getCustomerUsername() << ","
                           << booking.getMovieID() << "," << booking.getSchedule().getDate() << ","
                           << booking.getSchedule().getTime() << "," << booking.getSeat() << endl;
            }
            bookingFile.close();
        }

        // Save seats
        ofstream seatFile("seats.txt");
        if (seatFile.is_open()) {
            for (const auto& movieSeatPair : movieSeats) {
                int movieID = movieSeatPair.first.first;
                string date = movieSeatPair.first.second;
                for (const auto& seat : movieSeatPair.second) {
                    seatFile << movieID << "," << date << "," << seat.first << "," << seat.second << endl;
                }
            }
            seatFile.close();
        }
    }

    vector<unique_ptr<User>>& getUsers() { return users; }
    vector<Movie>& getMovies() { return movies; }
    vector<Booking>& getBookings() { return bookings; }

    // Seat management interface
    void initializeSeatsForNewMovie(int movieID, const string& date) {
        initializeSeatsForMovie(movieID, date);
    }

    void removeSeatsForMovie(int movieID, const string& date) {
        movieSeats.erase({movieID, date});
    }

    bool hasBookingsForSchedule(int movieID, const string& date) const {
        return any_of(bookings.begin(), bookings.end(), 
            [&](const Booking& b) { 
                return b.getMovieID() == movieID && b.getSchedule().getDate() == date; 
            });
    }

    bool isSeatAvailable(int movieID, const string& date, const string& seat) const {
        auto it = movieSeats.find({movieID, date});
        return it != movieSeats.end() && it->second.count(seat) && it->second.at(seat);
    }

    void bookSeat(int movieID, const string& date, const string& seat) {
        auto& seats = movieSeats[{movieID, date}];
        if (seats.count(seat)) seats[seat] = false;
    }

    void freeSeat(int movieID, const string& date, const string& seat) {
        auto& seats = movieSeats[{movieID, date}];
        if (seats.count(seat)) seats[seat] = true;
    }

    void displaySeatLayout(int movieID, const string& date) const {
        auto it = movieSeats.find({movieID, date});
        if (it == movieSeats.end()) {
            cout << "No seat data available for this date." << endl;
            return;
        }

        cout << "\n\t\t\t   ============ SCREEN ============" << endl;
        cout << "\t\t\t   -------------------------------" << endl << endl;
        
        // Display column numbers
        cout << "      ";
        for (int num = 1; num <= 10; num++) {
            cout << setw(3) << num;
        }
        cout << endl;

        // Display seat rows
        for (char row = 'A'; row <= 'H'; row++) {
            cout << "   " << row << "  ";
            for (int num = 1; num <= 10; num++) {
                string seat = string(1, row) + to_string(num);
                cout << setw(3) << (it->second.at(seat) ? "O" : "X");
            }
            cout << endl;
        }

        // Display key and additional information
        cout << "\n\tO = Available\tX = Booked\t[ ] = Your Selection" << endl;
        cout << "\t-----------------------------------------------" << endl;
        cout << "\t\t   <<< FRONT OF THEATER >>>" << endl << endl;
    }

    User* login() {
        string username, password;
        bool loggedIn = false;
        User* user = nullptr;
        
        while (!loggedIn) {
            cout << "\n=== Login ===" << endl;
            cout << "Username (or '0' to cancel): ";
            cin >> username;
            
            if (username == "0") {
                loggedIn = true; // Exit loop
                continue;
            }
            
            cout << "Password: ";
            cin >> password;
            clearInputBuffer();

            for (const auto& u : users) {
                if (u->getUsername() == username && u->getPassword() == password) {
                    cout << "Login successful!" << endl;
                    user = u.get();
                    loggedIn = true;
                    break;
                }
            }
            
            if (!loggedIn) {
                cout << "Invalid username or password. Please try again." << endl;
            }
        }
        return user;
    }

    void registerUser() {
        string username, password, name;
        bool registered = false;
        
        while (!registered) {
            cout << "\n=== User Registration ===" << endl;
            cout << "Username: ";
            cin >> username;
            
            bool usernameTaken = false;
            for (const auto& user : users) {
                if (user->getUsername() == username) {
                    usernameTaken = true;
                    break;
                }
            }
            
            if (usernameTaken) {
                cout << "Username already exists. Please choose another." << endl;
                continue;
            }

            cout << "Password: ";
            cin >> password;
            clearInputBuffer();
            cout << "Full Name: ";
            getline(cin, name);
