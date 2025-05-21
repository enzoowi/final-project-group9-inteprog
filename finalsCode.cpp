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

// Color codes for Windows console
const string RESET = "\033[0m";
const string GREEN = "\033[32m";
const string RED = "\033[31m";
const string YELLOW = "\033[33m";
const string CYAN = "\033[36m";

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
        cout << CYAN << "\n  Enter your choice (" << min << "-" << max << "): " << RESET;
        if (cin >> choice && choice >= min && choice <= max) {
            clearInputBuffer();
            valid = true;
        } else {
            cin.clear();
            clearInputBuffer();
            cout << RED << "\n  Invalid input. Please try again." << RESET << endl;
        }
    }
    return choice;
}

// Helper function for yes/no confirmations
bool getConfirmation(const string& prompt) {
    char confirm;
    bool valid = false;
    while (!valid) {
        cout << YELLOW << "\n  " << prompt << " (Y/N): " << RESET;
        cin >> confirm;
        clearInputBuffer();
        confirm = toupper(confirm);
        if (confirm == 'Y') return true;
        if (confirm == 'N') return false;
        cout << RED << "\n  Invalid input. Please enter Y or N." << RESET << endl;
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

// Add this helper function after the other helper functions
string getValidPaymentMode() {
    cout << "\nSelect Payment Mode:" << endl;
    cout << "1. Cash" << endl;
    cout << "2. Credit/Debit Card" << endl;
    cout << "3. GCash" << endl;
    
    int choice = getValidChoice(1, 3);
    switch (choice) {
        case 1: return "Cash";
        case 2: return "Credit/Debit Card";
        case 3: return "GCash";
        default: return "Cash";
    }
}

// Class definitions
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
    double price;
    vector<Schedule> schedules;
    static int nextMovieID;

public:
    Movie(string t, string g, double p) : title(t), genre(g), price(p), movieID(nextMovieID++) {}

    int getMovieID() const { return movieID; }
    string getTitle() const { return title; }
    string getGenre() const { return genre; }
    double getPrice() const { return price; }
    const vector<Schedule>& getSchedules() const { return schedules; }

    void setPrice(double p) { price = p; }

    void addSchedule(const Schedule& schedule) { schedules.push_back(schedule); }
    void removeSchedule(int index) {
        if (index >= 0 && index < schedules.size()) {
            schedules.erase(schedules.begin() + index);
        }
    }

    void displayDetails() const {
        cout << "\n\t╔═══════════════════════════════════╗" << endl;
        cout << CYAN << "\t║          Movie Details             ║" << RESET << endl;
        cout << "\t╠═══════════════════════════════════╣" << endl;
        cout << "\t║  Movie ID: " << YELLOW << setw(20) << left << movieID << RESET << "║" << endl;
        cout << "\t║  Title: " << YELLOW << setw(22) << left << title << RESET << "║" << endl;
        cout << "\t║  Genre: " << YELLOW << setw(22) << left << genre << RESET << "║" << endl;
        cout << "\t║  Price: $" << GREEN << setw(20) << left << fixed << setprecision(2) << price << RESET << "║" << endl;
        cout << "\t╠═══════════════════════════════════╣" << endl;
        cout << CYAN << "\t║          Schedules                 ║" << RESET << endl;
        cout << "\t╠═══════════════════════════════════╣" << endl;
        for (size_t i = 0; i < schedules.size(); i++) {
            string schedStr = schedules[i].getDate() + " at " + schedules[i].getTime();
            cout << "\t║  " << YELLOW << setw(2) << left << i+1 << ". " << setw(23) << left << schedStr << RESET << "║" << endl;
        }
        cout << "\t╚═══════════════════════════════════╝" << endl;
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
    double price;
    string paymentMode;
    static int nextBookingID;

public:
    Booking(string username, int mID, const Schedule& sched, string st, double p, string pm) :
        customerUsername(username), movieID(mID), schedule(sched), seat(st), price(p), 
        paymentMode(pm), bookingID(nextBookingID++) {}

    int getBookingID() const { return bookingID; }
    string getCustomerUsername() const { return customerUsername; }
    int getMovieID() const { return movieID; }
    Schedule getSchedule() const { return schedule; }
    string getSeat() const { return seat; }
    double getPrice() const { return price; }
    string getPaymentMode() const { return paymentMode; }

    void displayDetails(const vector<Movie>& movies) const {
        string movieTitle = "Unknown";
        for (const auto& movie : movies) {
            if (movie.getMovieID() == movieID) {
                movieTitle = movie.getTitle();
                break;
            }
        }
        
        cout << "\n\t╔═══════════════════════════════════╗" << endl;
        cout << CYAN << "\t║         Booking Details            ║" << RESET << endl;
        cout << "\t╠═══════════════════════════════════╣" << endl;
        cout << "\t║  Booking ID: " << YELLOW << setw(18) << left << bookingID << RESET << "║" << endl;
        cout << "\t║  Customer: " << YELLOW << setw(20) << left << customerUsername << RESET << "║" << endl;
        cout << "\t║  Movie: " << YELLOW << setw(23) << left << movieTitle << RESET << "║" << endl;
        cout << "\t║  Date: " << YELLOW << setw(24) << left << schedule.getDate() << RESET << "║" << endl;
        cout << "\t║  Time: " << YELLOW << setw(24) << left << schedule.getTime() << RESET << "║" << endl;
        cout << "\t║  Seat: " << YELLOW << setw(24) << left << seat << RESET << "║" << endl;
        cout << "\t║  Price: $" << GREEN << setw(22) << left << fixed << setprecision(2) << price << RESET << "║" << endl;
        cout << "\t║  Payment Mode: " << YELLOW << setw(16) << left << paymentMode << RESET << "║" << endl;
        cout << "\t╚═══════════════════════════════════╝" << endl;
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

                if (tokens.size() >= 4) {
                    try {
                        double price = stod(tokens[3]);
                        Movie movie(tokens[1], tokens[2], price);
                        for (size_t i = 4; i < tokens.size(); i += 2) {
                            if (i + 1 < tokens.size()) {
                                movie.addSchedule(Schedule(tokens[i], tokens[i+1]));
                            }
                        }
                        movies.push_back(movie);
                    } catch (...) {
                        cerr << "Error loading movie: " << line << endl;
                    }
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

                if (tokens.size() >= 7) {
                    try {
                        bookings.push_back(Booking(
                            tokens[1], 
                            stoi(tokens[2]), 
                            Schedule(tokens[3], tokens[4]), 
                            tokens[5],
                            stod(tokens[6]),
                            tokens[7]
                        ));
                    } catch (...) {
                        cerr << "Error loading booking: " << line << endl;
                    }
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
                    try {
                        int movieID = stoi(tokens[0]);
                        string date = tokens[1];
                        string seat = tokens[2];
                        bool available = (tokens[3] == "1");
                        movieSeats[{movieID, date}][seat] = available;
                    } catch (...) {
                        cerr << "Error loading seat: " << line << endl;
                    }
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
                movieFile << movie.getMovieID() << "," << movie.getTitle() << "," 
                         << movie.getGenre() << "," << fixed << setprecision(2) << movie.getPrice();
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
                           << booking.getSchedule().getTime() << "," << booking.getSeat() << ","
                           << fixed << setprecision(2) << booking.getPrice() << "," << booking.getPaymentMode() << endl;
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
            cout << "\n\t╔═══════════════════════════════════╗" << endl;
            cout << YELLOW << "\t║   No seat data for this date      ║" << RESET << endl;
            cout << "\t╚═══════════════════════════════════╝" << endl;
            return;
        }

        cout << "\n\t╔═══════════════════════════════════════════════╗" << endl;
        cout << CYAN << "\t║                    SCREEN                      ║" << RESET << endl;
        cout << "\t╚═══════════════════════════════════════════════╝" << endl;
        
        // Display column numbers
        cout << "\n\t        ";
        for (int num = 1; num <= 10; num++) {
            cout << YELLOW << setw(3) << num << RESET;
        }
        cout << endl;

        // Create horizontal line using individual characters
        cout << "\t     ╔";
        for (int i = 0; i < 31; i++) cout << "═";
        cout << "╗" << endl;
        
        // Display seat rows
        for (char row = 'A'; row <= 'H'; row++) {
            cout << "\t  " << YELLOW << row << RESET << "  ║";
            for (int num = 1; num <= 10; num++) {
                string seat = string(1, row) + to_string(num);
                if (it->second.at(seat)) {
                    cout << " " << GREEN << "O" << RESET << " ";
                } else {
                    cout << " " << RED << "X" << RESET << " ";
                }
            }
            cout << "║" << endl;
        }
        
        // Create bottom horizontal line using individual characters
        cout << "\t     ╚";
        for (int i = 0; i < 31; i++) cout << "═";
        cout << "╝" << endl;

        // Display key and additional information
        cout << "\n\t╔═══════════════════════════════════╗" << endl;
        cout << "\t║    " << GREEN << "O" << RESET << " = Available    " << RED << "X" << RESET << " = Booked     ║" << endl;
        cout << "\t║    [ ] = Your Selection            ║" << endl;
        cout << "\t╚═══════════════════════════════════╝" << endl;
    }

    User* login() {
        string username, password;
        bool loggedIn = false;
        User* user = nullptr;
        
        while (!loggedIn) {
            cout << "\n\t╔═══════════════════════════════════╗" << endl;
            cout << "\t║             Login                 ║" << endl;
            cout << "\t╚═══════════════════════════════════╝\n" << endl;
            
            cout << "  Username (or '0' to cancel): ";
            getline(cin, username);
            
            // Check for spaces in username
            if (username.find(' ') != string::npos) {
                cout << RED << "\n  Error: Username cannot contain spaces. Please try again." << RESET << endl;
                continue;
            }
            
            if (username == "0") {
                break;
            }
            
            bool validPassword = false;
            while (!validPassword && !loggedIn) {
                cout << "  Password: ";
                getline(cin, password);
                
                // Check for spaces in password
                if (password.find(' ') != string::npos) {
                    cout << RED << "\n  Error: Password cannot contain spaces. Please try again." << RESET << endl;
                    continue;
                }

                for (const auto& u : users) {
                    if (u->getUsername() == username && u->getPassword() == password) {
                        cout << GREEN << "\n  Login successful!" << RESET << endl;
                        user = u.get();
                        loggedIn = true;
                        break;
                    }
                }
                
                if (!loggedIn) {
                    cout << RED << "\n  Invalid username or password. Please try again." << RESET << endl;
                    break;
                } else {
                    validPassword = true;
                }
            }
        }
        return user;
    }

    void registerUser() {
        string username, password, name;
        bool registered = false;
        
        while (!registered) {
            cout << "\n\t╔═══════════════════════════════════╗" << endl;
            cout << "\t║        User Registration          ║" << endl;
            cout << "\t╚═══════════════════════════════════╝\n" << endl;
            
            // Get username with space validation
            bool validUsername = false;
            while (!validUsername && !registered) {
                cout << "  Username (no spaces allowed): ";
                getline(cin, username);
                if (username.find(' ') != string::npos) {
                    cout << RED << "\n  Error: Username cannot contain spaces. Please try again." << RESET << endl;
                } else if (username.empty()) {
                    cout << RED << "\n  Error: Username cannot be empty. Please try again." << RESET << endl;
                } else {
                    validUsername = true;
                }
            }
            
            if (!validUsername) continue;

            bool usernameTaken = false;
            for (const auto& user : users) {
                if (user->getUsername() == username) {
                    usernameTaken = true;
                    break;
                }
            }
            
            if (usernameTaken) {
                cout << RED << "\n  Error: Username already exists. Please choose another." << RESET << endl;
                continue;
            }

            // Get password with space validation
            bool validPassword = false;
            while (!validPassword && !registered) {
                cout << "  Password (no spaces allowed): ";
                getline(cin, password);
                if (password.find(' ') != string::npos) {
                    cout << RED << "\n  Error: Password cannot contain spaces. Please try again." << RESET << endl;
                } else if (password.empty()) {
                    cout << RED << "\n  Error: Password cannot be empty. Please try again." << RESET << endl;
                } else {
                    validPassword = true;
                }
            }
            
            if (!validPassword) continue;

            cout << "  Full Name: ";
            getline(cin, name);

            if (getConfirmation("Confirm registration?")) {
                users.push_back(make_unique<Customer>(username, password, name));
                saveData();
                cout << GREEN << "\n  Registration successful! You can now login." << RESET << endl;
                registered = true;
            } else {
                cout << YELLOW << "\n  Registration cancelled." << RESET << endl;
                registered = true;
            }
        }
    }

    void addBooking(const Booking& booking) {
        bookings.push_back(booking);
        bookSeat(booking.getMovieID(), booking.getSchedule().getDate(), booking.getSeat());
        saveData();
    }

    void removeBooking(int index) {
        if (index >= 0 && index < bookings.size()) {
            Booking& booking = bookings[index];
            freeSeat(booking.getMovieID(), booking.getSchedule().getDate(), booking.getSeat());
            bookings.erase(bookings.begin() + index);
            saveData();
        }
    }

    void updateBooking(int index, const Schedule& newSchedule, const string& newSeat, double newPrice, const string& newPaymentMode) {
        if (index >= 0 && index < bookings.size()) {
            Booking& booking = bookings[index];
            freeSeat(booking.getMovieID(), booking.getSchedule().getDate(), booking.getSeat());
            booking = Booking(
                booking.getCustomerUsername(),
                booking.getMovieID(),
                newSchedule,
                newSeat,
                newPrice,
                newPaymentMode
            );
            bookSeat(booking.getMovieID(), newSchedule.getDate(), newSeat);
            saveData();
        }
    }

    string getValidSeat(int movieID, const string& date) {
        string seat;
        bool validSeat = false;
        
        while (!validSeat) {
            cout << "Enter seat (e.g., A1) or '0' to cancel: ";
            getline(cin, seat);
            transform(seat.begin(), seat.end(), seat.begin(), ::toupper);
            
            if (seat == "0") {
                validSeat = true;
                seat = "";
            } else if (isSeatAvailable(movieID, date, seat)) {
                validSeat = true;
            } else {
                cout << "Invalid or already booked seat. Please try again." << endl;
            }
        }
        return seat;
    }

    Schedule getValidSchedule() {
        string date, time;
        bool valid = false;
        
        while (!valid) {
            cout << "Enter date (YYYY-MM-DD): ";
            getline(cin, date);
            if (!isValidDate(date)) {
                cout << "Invalid date format. Please use YYYY-MM-DD." << endl;
                continue;
            }

            cout << "Enter time (HH:MM): ";
            getline(cin, time);
            if (!isValidTime(time)) {
                cout << "Invalid time format. Please use HH:MM." << endl;
                continue;
            }

            valid = true;
        }
        return Schedule(date, time);
    }
};

// Initialize static member
CinemaBookingSystem* CinemaBookingSystem::instance = nullptr;

// Customer method implementations
void Customer::bookTicket() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Movie>& movies = system->getMovies();
    
    if (movies.empty()) {
        cout << "No movies available for booking." << endl;
        return;
    }
    
    cout << "\n=== Available Movies ===" << endl;
    for (size_t i = 0; i < movies.size(); i++) {
        cout << i+1 << ".";
        movies[i].displayDetails();
    }
    
    cout << "Enter movie number to book (0 to cancel): ";
    int movieChoice = getValidChoice(0, movies.size());
    
    if (movieChoice == 0) {
        cout << "Booking cancelled." << endl;
        return;
    }
    
    Movie& selectedMovie = movies[movieChoice - 1];
    const vector<Schedule>& schedules = selectedMovie.getSchedules();
    
    if (schedules.empty()) {
        cout << "No schedules available for this movie." << endl;
        return;
    }
    
    cout << "\nAvailable schedules for " << selectedMovie.getTitle() << ":" << endl;
    for (size_t i = 0; i < schedules.size(); i++) {
        cout << i+1 << ". ";
        schedules[i].display();
        cout << endl;
    }
    
    cout << "Enter schedule number (0 to cancel): ";
    int scheduleChoice = getValidChoice(0, schedules.size());
    
    if (scheduleChoice == 0) {
        cout << "Booking cancelled." << endl;
        return;
    }
    
    Schedule selectedSchedule = schedules[scheduleChoice - 1];
    
    // Display theater layout
    cout << "\n\t\t=== THEATER LAYOUT ===" << endl;
    system->displaySeatLayout(selectedMovie.getMovieID(), selectedSchedule.getDate());
    
    string seat = system->getValidSeat(selectedMovie.getMovieID(), selectedSchedule.getDate());
    if (seat.empty()) {
        cout << "Booking cancelled." << endl;
        return;
    }
    
    // Show selection confirmation
    cout << "\n\tYou have selected: " << seat << endl;
    cout << "\tPrice: $" << fixed << setprecision(2) << selectedMovie.getPrice() << endl;
    cout << "\t----------------------------" << endl;
    
    cout << "\n=== Booking Summary ===" << endl;
    cout << "Movie: " << selectedMovie.getTitle() << endl;
    cout << "Date: " << selectedSchedule.getDate() << endl;
    cout << "Time: " << selectedSchedule.getTime() << endl;
    cout << "Seat: " << seat << endl;
    cout << "Price: $" << fixed << setprecision(2) << selectedMovie.getPrice() << endl;
    
    if (getConfirmation("Confirm booking details?")) {
        string paymentMode = getValidPaymentMode();
        
        cout << "\nPayment Summary:" << endl;
        cout << "Amount to Pay: $" << fixed << setprecision(2) << selectedMovie.getPrice() << endl;
        cout << "Payment Mode: " << paymentMode << endl;
        
        if (getConfirmation("Confirm payment?")) {
            system->addBooking(Booking(getUsername(), selectedMovie.getMovieID(), selectedSchedule, seat, selectedMovie.getPrice(), paymentMode));
            cout << "\n\t*********************************" << endl;
            cout << "\t*                               *" << endl;
            cout << "\t*      BOOKING CONFIRMED!       *" << endl;
            cout << "\t*                               *" << endl;
            cout << "\t*********************************" << endl;
            cout << "\nPayment of $" << fixed << setprecision(2) << selectedMovie.getPrice() 
                 << " via " << paymentMode << " has been processed." << endl;
        } else {
            cout << "Payment cancelled. Booking not confirmed." << endl;
        }
    } else {
        cout << "Booking cancelled." << endl;
    }
}

void Customer::viewBookings() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    const vector<Booking>& bookings = system->getBookings();
    const vector<Movie>& movies = system->getMovies();
    
    cout << "\n=== My Bookings ===" << endl;
    
    bool hasBookings = false;
    for (const auto& booking : bookings) {
        if (booking.getCustomerUsername() == getUsername()) {
            booking.displayDetails(movies);
            hasBookings = true;
        }
    }
    
    if (!hasBookings) {
        cout << "You have no bookings." << endl;
    }
}

void Customer::editBooking() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Booking>& bookings = system->getBookings();
    vector<Movie>& movies = system->getMovies();
    
    cout << "\n=== My Bookings ===" << endl;
    vector<int> userBookingIndices;
    int count = 1;
    
    for (size_t i = 0; i < bookings.size(); i++) {
        if (bookings[i].getCustomerUsername() == getUsername()) {
            cout << count << ".";
            bookings[i].displayDetails(movies);
            userBookingIndices.push_back(i);
            count++;
        }
    }
    
    if (userBookingIndices.empty()) {
        cout << "You have no bookings to edit." << endl;
        return;
    }
    
    cout << "Enter booking number to edit (0 to cancel): ";
    int bookingChoice = getValidChoice(0, userBookingIndices.size());
    
    if (bookingChoice == 0) {
        cout << "Edit cancelled." << endl;
        return;
    }
    
    int actualIndex = userBookingIndices[bookingChoice - 1];
    Booking& bookingToEdit = bookings[actualIndex];
    
    Movie* selectedMovie = nullptr;
    for (auto& movie : movies) {
        if (movie.getMovieID() == bookingToEdit.getMovieID()) {
            selectedMovie = &movie;
            break;
        }
    }
    
    if (!selectedMovie) {
        cout << "Error: Movie not found." << endl;
        return;
    }
    
    const vector<Schedule>& schedules = selectedMovie->getSchedules();
    cout << "\nAvailable schedules for " << selectedMovie->getTitle() << ":" << endl;
    for (size_t i = 0; i < schedules.size(); i++) {
        cout << i+1 << ". ";
        schedules[i].display();
        cout << endl;
    }
    
    cout << "Enter new schedule number (0 to keep current): ";
    int scheduleChoice = getValidChoice(0, schedules.size());
    
    Schedule newSchedule = bookingToEdit.getSchedule();
    if (scheduleChoice > 0) {
        newSchedule = schedules[scheduleChoice - 1];
    }
    
    system->displaySeatLayout(selectedMovie->getMovieID(), newSchedule.getDate());
    
    cout << "Enter new seat (current: " << bookingToEdit.getSeat() << ", enter 0 to keep current): ";
    string newSeat = system->getValidSeat(selectedMovie->getMovieID(), newSchedule.getDate());
    if (newSeat.empty()) {
        newSeat = bookingToEdit.getSeat();
    }
    
    double newPrice = selectedMovie->getPrice();
    
    cout << "\n=== Updated Booking Summary ===" << endl;
    cout << "Movie: " << selectedMovie->getTitle() << endl;
    cout << "Date: " << newSchedule.getDate() << endl;
    cout << "Time: " << newSchedule.getTime() << endl;
    cout << "Seat: " << newSeat << endl;
    cout << "Price: $" << fixed << setprecision(2) << newPrice << endl;
    cout << "Current Payment Mode: " << bookingToEdit.getPaymentMode() << endl;
    
    string newPaymentMode = bookingToEdit.getPaymentMode();
    if (getConfirmation("Would you like to change the payment mode?")) {
        newPaymentMode = getValidPaymentMode();
    }
    
    cout << "\nFinal Payment Summary:" << endl;
    cout << "Amount to Pay: $" << fixed << setprecision(2) << newPrice << endl;
    cout << "Payment Mode: " << newPaymentMode << endl;
    
    if (getConfirmation("Confirm changes?")) {
        system->updateBooking(actualIndex, newSchedule, newSeat, newPrice, newPaymentMode);
        cout << "Booking updated successfully!" << endl;
        if (newPaymentMode != bookingToEdit.getPaymentMode()) {
            cout << "Payment mode has been updated to: " << newPaymentMode << endl;
        }
    } else {
        cout << "Edit cancelled." << endl;
    }
}

void Customer::cancelBooking() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Booking>& bookings = system->getBookings();
    vector<Movie>& movies = system->getMovies();
    
    cout << "\n=== My Bookings ===" << endl;
    vector<int> userBookingIndices;
    int count = 1;
    
    for (size_t i = 0; i < bookings.size(); i++) {
        if (bookings[i].getCustomerUsername() == getUsername()) {
            cout << count << ".";
            bookings[i].displayDetails(movies);
            userBookingIndices.push_back(i);
            count++;
        }
    }
    
    if (userBookingIndices.empty()) {
        cout << "You have no bookings to cancel." << endl;
        return;
    }
    
    cout << "Enter booking number to cancel (0 to cancel): ";
    int bookingChoice = getValidChoice(0, userBookingIndices.size());
    
    if (bookingChoice == 0) {
        cout << "Cancellation aborted." << endl;
        return;
    }
    
    int actualIndex = userBookingIndices[bookingChoice - 1];
    
    if (getConfirmation("Are you sure you want to cancel this booking?")) {
        system->removeBooking(actualIndex);
        cout << "Booking cancelled successfully." << endl;
    } else {
        cout << "Cancellation aborted." << endl;
    }
}

void Customer::displayMenu() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    bool logout = false;
    
    while (!logout) {
        cout << "\n\n\t╔═══════════════════════════════════╗" << endl;
        cout << "\t║          Customer Menu            ║" << endl;
        cout << "\t╠═══════════════════════════════════╣" << endl;
        cout << "\t║  1. Book Ticket                   ║" << endl;
        cout << "\t║  2. View My Bookings              ║" << endl;
        cout << "\t║  3. Edit Booking                  ║" << endl;
        cout << "\t║  4. Cancel Booking                ║" << endl;
        cout << "\t║  5. Logout                        ║" << endl;
        cout << "\t╚═══════════════════════════════════╝" << endl;
        
        int choice = getValidChoice(1, 5);

        switch (choice) {
            case 1:
                cout << "\n";
                bookTicket();
                break;
            case 2:
                cout << "\n";
                viewBookings();
                break;
            case 3:
                cout << "\n";
                editBooking();
                break;
            case 4:
                cout << "\n";
                cancelBooking();
                break;
            case 5:
                cout << "\n\t╔═══════════════════════════════════╗" << endl;
                cout << YELLOW << "\t║          Logging out...           ║" << RESET << endl;
                cout << "\t╚═══════════════════════════════════╝" << endl;
                cout << "\n";
                logout = true;
                break;
        }
    }
}

// Admin method implementations
void Admin::addMovie() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Movie>& movies = system->getMovies();
    
    string title, genre;
    
    cout << "\n=== Add New Movie ===" << endl;
    cout << "Enter movie title: ";
    getline(cin, title);
    cout << "Enter genre: ";
    getline(cin, genre);
    
    double price;
    cout << "Enter ticket price: $";
    while (!(cin >> price) || price <= 0) {
        cout << "Invalid price. Please enter a positive number: $";
        cin.clear();
        clearInputBuffer();
    }
    clearInputBuffer();
    
    Movie newMovie(title, genre, price);
    
    bool addMoreSchedules = true;
    while (addMoreSchedules) {
        cout << "\nAdding new schedule:" << endl;
        Schedule schedule = system->getValidSchedule();
        newMovie.addSchedule(schedule);
        
        system->initializeSeatsForNewMovie(newMovie.getMovieID(), schedule.getDate());
        
        addMoreSchedules = getConfirmation("Add another schedule?");
    }
    
    movies.push_back(newMovie);
    system->saveData();
    cout << "Movie added successfully!" << endl;
}

void Admin::editMovie() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Movie>& movies = system->getMovies();
    
    if (movies.empty()) {
        cout << "No movies available to edit." << endl;
        return;
    }
    
    cout << "\n=== Available Movies ===" << endl;
    for (size_t i = 0; i < movies.size(); i++) {
        cout << i+1 << ".";
        movies[i].displayDetails();
    }
    
    cout << "Enter movie number to edit (0 to cancel): ";
    int movieChoice = getValidChoice(0, movies.size());
    
    if (movieChoice == 0) {
        cout << "Edit cancelled." << endl;
        return;
    }
    
    Movie& movieToEdit = movies[movieChoice - 1];
    
    string newTitle, newGenre;
    double newPrice;
    cout << "Current title: " << movieToEdit.getTitle() << endl;
    cout << "Enter new title (leave blank to keep current): ";
    getline(cin, newTitle);
    
    cout << "Current genre: " << movieToEdit.getGenre() << endl;
    cout << "Enter new genre (leave blank to keep current): ";
    getline(cin, newGenre);
    
    cout << "Current price: $" << fixed << setprecision(2) << movieToEdit.getPrice() << endl;
    cout << "Enter new price (0 to keep current): $";
    cin >> newPrice;
    clearInputBuffer();
    
    if (!newTitle.empty() || !newGenre.empty() || newPrice > 0) {
        movieToEdit = Movie(
            newTitle.empty() ? movieToEdit.getTitle() : newTitle,
            newGenre.empty() ? movieToEdit.getGenre() : newGenre,
            newPrice <= 0 ? movieToEdit.getPrice() : newPrice
        );
    }
    
    bool editingSchedules = true;
    while (editingSchedules) {
        cout << "\nCurrent schedules:" << endl;
        const vector<Schedule>& schedules = movieToEdit.getSchedules();
        for (size_t i = 0; i < schedules.size(); i++) {
            cout << i+1 << ". ";
            schedules[i].display();
            cout << endl;
        }
        
        cout << "\n1. Add schedule" << endl;
        cout << "2. Remove schedule" << endl;
        cout << "3. Done editing" << endl;
        cout << "Enter choice: ";
        int scheduleChoice = getValidChoice(1, 3);
        
        switch (scheduleChoice) {
            case 1: {
                cout << "\nAdding new schedule:" << endl;
                Schedule newSchedule = system->getValidSchedule();
                movieToEdit.addSchedule(newSchedule);
                system->initializeSeatsForNewMovie(movieToEdit.getMovieID(), newSchedule.getDate());
                cout << "Schedule added." << endl;
                break;
            }
            case 2:
                if (!schedules.empty()) {
                    cout << "Enter schedule number to remove: ";
                    int removeIndex = getValidChoice(1, schedules.size());
                    
                    if (system->hasBookingsForSchedule(movieToEdit.getMovieID(), schedules[removeIndex - 1].getDate())) {
                        cout << "Cannot remove schedule because there are existing bookings." << endl;
                    } else {
                        system->removeSeatsForMovie(movieToEdit.getMovieID(), schedules[removeIndex - 1].getDate());
                        movieToEdit.removeSchedule(removeIndex - 1);
                    }
                }
                break;
            case 3:
                editingSchedules = false;
                break;
        }
    }
    
    system->saveData();
    cout << "Movie updated successfully!" << endl;
}

void Admin::deleteMovie() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Movie>& movies = system->getMovies();
    vector<Booking>& bookings = system->getBookings();
    
    if (movies.empty()) {
        cout << "No movies available to delete." << endl;
        return;
    }
    
    cout << "\n=== Available Movies ===" << endl;
    for (size_t i = 0; i < movies.size(); i++) {
        cout << i+1 << ".";
        movies[i].displayDetails();
    }
    
    cout << "Enter movie number to delete (0 to cancel): ";
    int movieChoice = getValidChoice(0, movies.size());
    
    if (movieChoice == 0) {
        cout << "Deletion cancelled." << endl;
        return;
    }
    
    if (getConfirmation("Are you sure you want to delete this movie?")) {
        int movieID = movies[movieChoice - 1].getMovieID();
        
        // Count how many bookings will be affected
        int bookingsToRemove = 0;
        for (const auto& booking : bookings) {
            if (booking.getMovieID() == movieID) {
                bookingsToRemove++;
            }
        }
        
        if (bookingsToRemove > 0) {
            cout << "\nWarning: This movie has " << bookingsToRemove << " active booking(s)." << endl;
            if (!getConfirmation("Deleting this movie will also remove all associated bookings. Continue?")) {
                cout << "Deletion cancelled." << endl;
                return;
            }
            
            // Remove all bookings for this movie
            auto it = remove_if(bookings.begin(), bookings.end(),
                [movieID](const Booking& booking) {
                    return booking.getMovieID() == movieID;
                });
            bookings.erase(it, bookings.end());
            cout << bookingsToRemove << " booking(s) have been removed." << endl;
        }
        
        // Remove all seats for this movie
        for (const auto& schedule : movies[movieChoice - 1].getSchedules()) {
            system->removeSeatsForMovie(movieID, schedule.getDate());
        }
        
        // Remove the movie
        movies.erase(movies.begin() + movieChoice - 1);
        system->saveData();
        cout << "Movie deleted successfully." << endl;
    } else {
        cout << "Deletion cancelled." << endl;
    }
}

void Admin::viewAllBookings() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    const vector<Booking>& bookings = system->getBookings();
    const vector<Movie>& movies = system->getMovies();
    
    cout << "\n=== All Bookings ===" << endl;
    
    if (bookings.empty()) {
        cout << "No bookings found." << endl;
        return;
    }
    
    double totalRevenue = 0.0;
    for (const auto& booking : bookings) {
        booking.displayDetails(movies);
        totalRevenue += booking.getPrice();
    }
    
    cout << "\nTotal bookings: " << bookings.size() << endl;
    cout << "Total revenue: $" << fixed << setprecision(2) << totalRevenue << endl;
}

void Admin::manageSeats() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Movie>& movies = system->getMovies();
    
    if (movies.empty()) {
        cout << "No movies available to manage seats." << endl;
        return;
    }
    
    cout << "\n=== Available Movies ===" << endl;
    for (size_t i = 0; i < movies.size(); i++) {
        cout << i+1 << ".";
        movies[i].displayDetails();
    }
    
    cout << "Enter movie number to manage seats (0 to cancel): ";
    int movieChoice = getValidChoice(0, movies.size());
    
    if (movieChoice == 0) {
        cout << "Operation cancelled." << endl;
        return;
    }
    
    Movie& selectedMovie = movies[movieChoice - 1];
    const vector<Schedule>& schedules = selectedMovie.getSchedules();
    
    if (schedules.empty()) {
        cout << "No schedules available for this movie." << endl;
        return;
    }
    
    cout << "\nAvailable dates for " << selectedMovie.getTitle() << ":" << endl;
    for (size_t i = 0; i < schedules.size(); i++) {
        cout << i+1 << ". " << schedules[i].getDate() << endl;
    }
    
    cout << "Enter date number to manage seats (0 to cancel): ";
    int dateChoice = getValidChoice(0, schedules.size());
    
    if (dateChoice == 0) {
        cout << "Operation cancelled." << endl;
        return;
    }
    
    string selectedDate = schedules[dateChoice - 1].getDate();
    
    system->displaySeatLayout(selectedMovie.getMovieID(), selectedDate);
    
    cout << "\n1. Add new seat" << endl;
    cout << "2. Remove seat" << endl;
    cout << "3. Back to menu" << endl;
    cout << "Enter choice: ";
    int choice = getValidChoice(1, 3);
    
    switch (choice) {
        case 1: {
            string newSeat;
            cout << "Enter new seat ID (e.g., I1): ";
            getline(cin, newSeat);
            transform(newSeat.begin(), newSeat.end(), newSeat.begin(), ::toupper);
            
            if (system->isSeatAvailable(selectedMovie.getMovieID(), selectedDate, newSeat)) {
                system->bookSeat(selectedMovie.getMovieID(), selectedDate, newSeat);
                system->freeSeat(selectedMovie.getMovieID(), selectedDate, newSeat);
                system->saveData();
                cout << "Seat added successfully." << endl;
            } else {
                cout << "Seat already exists." << endl;
            }
            break;
        }
        case 2: {
            string seatToRemove;
            cout << "Enter seat ID to remove: ";
            getline(cin, seatToRemove);
            transform(seatToRemove.begin(), seatToRemove.end(), seatToRemove.begin(), ::toupper);
            
            if (!system->isSeatAvailable(selectedMovie.getMovieID(), selectedDate, seatToRemove)) {
                cout << "Seat doesn't exist." << endl;
            } else {
                bool isBooked = false;
                for (const auto& booking : system->getBookings()) {
                    if (booking.getMovieID() == selectedMovie.getMovieID() && 
                        booking.getSchedule().getDate() == selectedDate && 
                        booking.getSeat() == seatToRemove) {
                        isBooked = true;
                        break;
                    }
                }
                
                if (isBooked) {
                    cout << "Cannot remove seat because it has active bookings." << endl;
                } else {
                    system->freeSeat(selectedMovie.getMovieID(), selectedDate, seatToRemove);
                    system->saveData();
                    cout << "Seat removed successfully." << endl;
                }
            }
            break;
        }
    }
}

void Admin::manageSchedules() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Movie>& movies = system->getMovies();
    
    if (movies.empty()) {
        cout << "No movies available to manage schedules." << endl;
        return;
    }
    
    cout << "\n=== Available Movies ===" << endl;
    for (size_t i = 0; i < movies.size(); i++) {
        cout << i+1 << ".";
        movies[i].displayDetails();
    }
    
    cout << "Enter movie number to manage schedules (0 to cancel): ";
    int movieChoice = getValidChoice(0, movies.size());
    
    if (movieChoice == 0) {
        cout << "Operation cancelled." << endl;
        return;
    }
    
    Movie& selectedMovie = movies[movieChoice - 1];
    
    cout << "\nCurrent schedules for " << selectedMovie.getTitle() << ":" << endl;
    const vector<Schedule>& schedules = selectedMovie.getSchedules();
    for (size_t i = 0; i < schedules.size(); i++) {
        cout << i+1 << ". ";
        schedules[i].display();
        cout << endl;
    }
    
    cout << "\n1. Add schedule" << endl;
    cout << "2. Remove schedule" << endl;
    cout << "3. Back to menu" << endl;
    cout << "Enter choice: ";
    int choice = getValidChoice(1, 3);
    
    switch (choice) {
        case 1: {
            cout << "\nAdding new schedule:" << endl;
            Schedule newSchedule = system->getValidSchedule();
            selectedMovie.addSchedule(newSchedule);
            system->initializeSeatsForNewMovie(selectedMovie.getMovieID(), newSchedule.getDate());
            system->saveData();
            cout << "Schedule added successfully." << endl;
            break;
        }
        case 2:
            if (!schedules.empty()) {
                cout << "Enter schedule number to remove: ";
                int removeIndex = getValidChoice(1, schedules.size());
                
                if (system->hasBookingsForSchedule(selectedMovie.getMovieID(), schedules[removeIndex - 1].getDate())) {
                    cout << "Cannot remove schedule because there are existing bookings." << endl;
                } else {
                    system->removeSeatsForMovie(selectedMovie.getMovieID(), schedules[removeIndex - 1].getDate());
                    selectedMovie.removeSchedule(removeIndex - 1);
                    system->saveData();
                    cout << "Schedule removed successfully." << endl;
                }
            } else {
                cout << "No schedules to remove." << endl;
            }
            break;
    }
}

void Admin::generateReports() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    const vector<Booking>& bookings = system->getBookings();
    const vector<Movie>& movies = system->getMovies();
    
    if (bookings.empty()) {
        cout << "\n\t╔═══════════════════════════════════╗" << endl;
        cout << YELLOW << "\t║      No bookings to generate       ║" << RESET << endl;
        cout << YELLOW << "\t║           reports.                 ║" << RESET << endl;
        cout << "\t╚═══════════════════════════════════╝" << endl;
        return;
    }
    
    map<int, pair<int, double>> movieStats;
    double totalRevenue = 0.0;
    
    for (const auto& booking : bookings) {
        movieStats[booking.getMovieID()].first++;
        movieStats[booking.getMovieID()].second += booking.getPrice();
        totalRevenue += booking.getPrice();
    }
    
    cout << "\n\t╔═══════════════════════════════════════════════════╗" << endl;
    cout << CYAN << "\t║                   Sales Report                     ║" << RESET << endl;
    cout << "\t╠═══════════════════════╦═══════════╦═══════════════╣" << endl;
    cout << "\t║      Movie Title      ║  Tickets  ║    Revenue    ║" << endl;
    cout << "\t╠═══════════════════════╬═══════════╬═══════════════╣" << endl;
    
    for (const auto& movie : movies) {
        auto it = movieStats.find(movie.getMovieID());
        if (it != movieStats.end()) {
            cout << "\t║ " << YELLOW << left << setw(19) << movie.getTitle().substr(0, 19) << RESET
                 << "║ " << CYAN << right << setw(9) << it->second.first << RESET
                 << " ║ $" << GREEN << right << setw(10) << fixed << setprecision(2) << it->second.second << RESET << " ║" << endl;
        }
    }
    
    cout << "\t╠═══════════════════════╬═══════════╬═══════════════╣" << endl;
    cout << "\t║ " << CYAN << "TOTAL" << RESET << "                 ║ " 
         << CYAN << right << setw(9) << bookings.size() << RESET
         << " ║ $" << GREEN << right << setw(10) << fixed << setprecision(2) << totalRevenue << RESET << " ║" << endl;
    cout << "\t╚═══════════════════════╩═══════════╩═══════════════╝" << endl;
}

void Admin::displayMenu() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    bool logout = false;
    
    while (!logout) {
        cout << "\n\n\t╔═══════════════════════════════════╗" << endl;
        cout << "\t║           Admin Menu               ║" << endl;
        cout << "\t╠═══════════════════════════════════╣" << endl;
        cout << "\t║  1. Add Movie                      ║" << endl;
        cout << "\t║  2. Edit Movie                     ║" << endl;
        cout << "\t║  3. Delete Movie                   ║" << endl;
        cout << "\t║  4. View All Bookings              ║" << endl;
        cout << "\t║  5. Manage Seats                   ║" << endl;
        cout << "\t║  6. Manage Schedules               ║" << endl;
        cout << "\t║  7. Generate Reports               ║" << endl;
        cout << "\t║  8. Logout                         ║" << endl;
        cout << "\t╚═══════════════════════════════════╝" << endl;
        
        int choice = getValidChoice(1, 8);

        switch (choice) {
            case 1:
                addMovie();
                break;
            case 2:
                editMovie();
                break;
            case 3:
                deleteMovie();
                break;
            case 4:
                viewAllBookings();
                break;
            case 5:
                manageSeats();
                break;
            case 6:
                manageSchedules();
                break;
            case 7:
                generateReports();
                break;
            case 8:
                cout << "\n\t╔═══════════════════════════════════╗" << endl;
                cout << YELLOW << "\t║          Logging out...           ║" << RESET << endl;
                cout << "\t╚═══════════════════════════════════╝" << endl;
                cout << "\n";
                logout = true;
                break;
        }
    }
}

int main() {
    // Initialize system
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    
    // Add default admin if none exists
    bool hasAdmin = false;
    for (const auto& user : system->getUsers()) {
        if (user->getUserType() == "ADMIN") {
            hasAdmin = true;
            break;
        }
    }
    
    if (!hasAdmin) {
        system->getUsers().push_back(make_unique<Admin>("admin", "admin123"));
        system->saveData();
    }

    // Main menu loop
    bool exitProgram = false;
    while (!exitProgram) {
        cout << "\n\t╔═══════════════════════════════════╗" << endl;
        cout << "\t║      Cinema Booking System         ║" << endl;
        cout << "\t╠═══════════════════════════════════╣" << endl;
        cout << "\t║  1. Login                         ║" << endl;
        cout << "\t║  2. Register                      ║" << endl;
        cout << "\t║  3. Exit                          ║" << endl;
        cout << "\t╚═══════════════════════════════════╝" << endl;
        
        switch (getValidChoice(1, 3)) {
            case 1: {
                if (User* user = system->login()) {
                    user->displayMenu();
                }
                break;
            }
            case 2:
                system->registerUser();
                break;
            case 3:
                exitProgram = getConfirmation("Are you sure you want to exit?");
                break;
        }
    }

    CinemaBookingSystem::cleanup();
    return 0;
}