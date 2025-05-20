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
if (getConfirmation("Confirm registration?")) {
                users.push_back(make_unique<Customer>(username, password, name));
                saveData();
                cout << "Registration successful! You can now login." << endl;
                registered = true;
            } else {
                cout << "Registration cancelled." << endl;
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

    void updateBooking(int index, const Schedule& newSchedule, const string& newSeat) {
        if (index >= 0 && index < bookings.size()) {
            Booking& booking = bookings[index];
            freeSeat(booking.getMovieID(), booking.getSchedule().getDate(), booking.getSeat());
            booking = Booking(
                booking.getCustomerUsername(),
                booking.getMovieID(),
                newSchedule,
                newSeat
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
            cin >> seat;
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
            cin >> date;
            if (!isValidDate(date)) {
                cout << "Invalid date format. Please use YYYY-MM-DD." << endl;
                continue;
            }

            cout << "Enter time (HH:MM): ";
            cin >> time;
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
void Customer::viewMovies() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    const vector<Movie>& movies = system->getMovies();
    
    cout << "\n=== Available Movies ===" << endl;
    if (movies.empty()) {
        cout << "No movies available at this time." << endl;
        return;
    }
    
    for (const auto& movie : movies) {
        movie.displayDetails();
    }
}

void Customer::bookTicket() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    vector<Movie>& movies = system->getMovies();
    
    viewMovies();
    
    if (movies.empty()) {
        cout << "No movies available for booking." << endl;
        return;
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
    cout << "\t----------------------------" << endl;
    
    cout << "\n=== Booking Summary ===" << endl;
    cout << "Movie: " << selectedMovie.getTitle() << endl;
    cout << "Date: " << selectedSchedule.getDate() << endl;
    cout << "Time: " << selectedSchedule.getTime() << endl;
    cout << "Seat: " << seat << endl;
    
    if (getConfirmation("Confirm booking?")) {
        system->addBooking(Booking(getUsername(), selectedMovie.getMovieID(), selectedSchedule, seat));
        cout << "\n\t*********************************" << endl;
        cout << "\t*                               *" << endl;
        cout << "\t*      BOOKING CONFIRMED!       *" << endl;
        cout << "\t*                               *" << endl;
        cout << "\t*********************************" << endl;
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
    
    cout << "\n=== Updated Booking Summary ===" << endl;
    cout << "Movie: " << selectedMovie->getTitle() << endl;
    cout << "Date: " << newSchedule.getDate() << endl;
    cout << "Time: " << newSchedule.getTime() << endl;
    cout << "Seat: " << newSeat << endl;
    
    if (getConfirmation("Confirm changes?")) {
        system->updateBooking(actualIndex, newSchedule, newSeat);
        cout << "Booking updated successfully!" << endl;
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
        cout << "\n=== Customer Menu ===" << endl;
        cout << "1. View Movies" << endl;
        cout << "2. Book Ticket" << endl;
        cout << "3. View My Bookings" << endl;
        cout << "4. Edit Booking" << endl;
        cout << "5. Cancel Booking" << endl;
        cout << "6. Logout" << endl;
        
        int choice = getValidChoice(1, 6);

        switch (choice) {
            case 1:
                viewMovies();
                break;
            case 2:
                bookTicket();
                break;
            case 3:
                viewBookings();
                break;
            case 4:
                editBooking();
                break;
            case 5:
                cancelBooking();
                break;
            case 6:
                cout << "Logging out..." << endl;
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
    clearInputBuffer();
    
    cout << "\n=== Add New Movie ===" << endl;
    cout << "Enter movie title: ";
    getline(cin, title);
    cout << "Enter genre: ";
    getline(cin, genre);
    
    Movie newMovie(title, genre);
    
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
    clearInputBuffer();
    cout << "Current title: " << movieToEdit.getTitle() << endl;
    cout << "Enter new title (leave blank to keep current): ";
    getline(cin, newTitle);
    
    cout << "Current genre: " << movieToEdit.getGenre() << endl;
    cout << "Enter new genre (leave blank to keep current): ";
    getline(cin, newGenre);
    
    if (!newTitle.empty() || !newGenre.empty()) {
        movieToEdit = Movie(
            newTitle.empty() ? movieToEdit.getTitle() : newTitle,
            newGenre.empty() ? movieToEdit.getGenre() : newGenre
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
                        cout << "Schedule removed." << endl;
                    }
                } else {
                    cout << "No schedules to remove." << endl;
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
        
        if (system->hasBookingsForSchedule(movieID, "")) {
            cout << "Cannot delete movie because there are existing bookings." << endl;
        } else {
            // Remove all seats for this movie
            for (const auto& schedule : movies[movieChoice - 1].getSchedules()) {
                system->removeSeatsForMovie(movieID, schedule.getDate());
            }
            movies.erase(movies.begin() + movieChoice - 1);
            system->saveData();
            cout << "Movie deleted successfully." << endl;
        }
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
    
    for (const auto& booking : bookings) {
        booking.displayDetails(movies);
    }
    
    cout << "\nTotal bookings: " << bookings.size() << endl;
}