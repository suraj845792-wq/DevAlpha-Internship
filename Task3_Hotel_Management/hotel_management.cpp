/*
 * ============================================================
 *  DevAlpha Technologies — C++ Programming Internship
 *  Task 3 (Intermediate): Management System Project
 *  Project     : Hotel Management System
 *  Concepts    : OOP, Inheritance, File Handling,
 *                Structures, Vectors, Exception Handling
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <stdexcept>
using namespace std;

// ─────────────────────────────────────────────
//  UTILITY: Get current date as string
// ─────────────────────────────────────────────
string getDate() {
    time_t now = time(0);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    return string(buf);
}

// ─────────────────────────────────────────────
//  CLASS: Room
// ─────────────────────────────────────────────
class Room {
private:
    int    roomNo;
    string type;       // Single / Double / Suite
    float  pricePerNight;
    bool   isBooked;
    string bookedBy;   // Guest name

public:
    Room(int no, string t, float price)
        : roomNo(no), type(t), pricePerNight(price),
          isBooked(false), bookedBy("") {}

    // Getters
    int    getRoomNo()      const { return roomNo; }
    string getType()        const { return type; }
    float  getPrice()       const { return pricePerNight; }
    bool   getIsBooked()    const { return isBooked; }
    string getBookedBy()    const { return bookedBy; }

    // Book room
    void book(string guestName) {
        if (isBooked)
            throw runtime_error("Room " + to_string(roomNo) + " is already booked.");
        isBooked = true;
        bookedBy = guestName;
    }

    // Vacate room
    void vacate() {
        if (!isBooked)
            throw runtime_error("Room " + to_string(roomNo) + " is not booked.");
        isBooked = false;
        bookedBy = "";
    }

    // Display room info
    void display() const {
        cout << "  | " << left
             << setw(6)  << roomNo
             << setw(10) << type
             << setw(12) << fixed << setprecision(0) << pricePerNight
             << setw(10) << (isBooked ? "Booked" : "Available")
             << (isBooked ? bookedBy : "-") << endl;
    }
};

// ─────────────────────────────────────────────
//  CLASS: Guest
// ─────────────────────────────────────────────
class Guest {
private:
    int    guestId;
    string name;
    string phone;
    string address;

public:
    Guest(int id, string n, string ph, string addr)
        : guestId(id), name(n), phone(ph), address(addr) {}

    int    getGuestId() const { return guestId; }
    string getName()    const { return name; }
    string getPhone()   const { return phone; }

    void display() const {
        cout << "\n  | Guest ID : " << guestId  << endl;
        cout << "  | Name     : " << name      << endl;
        cout << "  | Phone    : " << phone     << endl;
        cout << "  | Address  : " << address   << endl;
    }
};

// ─────────────────────────────────────────────
//  CLASS: Booking
// ─────────────────────────────────────────────
class Booking {
private:
    int    bookingId;
    int    guestId;
    string guestName;
    int    roomNo;
    string roomType;
    float  pricePerNight;
    int    nights;
    float  totalBill;
    string checkIn;
    string checkOut;
    bool   isActive;

public:
    Booking(int bId, int gId, string gName, int rNo,
            string rType, float price, int n, string cin)
        : bookingId(bId), guestId(gId), guestName(gName),
          roomNo(rNo), roomType(rType), pricePerNight(price),
          nights(n), checkIn(cin), checkOut(""), isActive(true) {
        totalBill = price * n;
    }

    int    getBookingId() const { return bookingId; }
    int    getRoomNo()    const { return roomNo; }
    int    getGuestId()   const { return guestId; }
    bool   getIsActive()  const { return isActive; }
    float  getTotalBill() const { return totalBill; }

    void checkout(string date) {
        checkOut = date;
        isActive = false;
    }

    void displayBill() const {
        cout << "\n  +------------------------------------------+" << endl;
        cout << "  |            HOTEL BILL / INVOICE          |" << endl;
        cout << "  +------------------------------------------+" << endl;
        cout << "  | Booking ID   : " << bookingId             << endl;
        cout << "  | Guest Name   : " << guestName             << endl;
        cout << "  | Guest ID     : " << guestId               << endl;
        cout << "  | Room No      : " << roomNo                << endl;
        cout << "  | Room Type    : " << roomType              << endl;
        cout << "  | Check-In     : " << checkIn               << endl;
        cout << "  | Check-Out    : " << (checkOut.empty() ? "Still Checked In" : checkOut) << endl;
        cout << "  | Nights       : " << nights                << endl;
        cout << "  | Rate/Night   : Rs. " << fixed << setprecision(2) << pricePerNight << endl;
        cout << "  +------------------------------------------+" << endl;
        cout << "  | TOTAL BILL   : Rs. " << totalBill         << endl;
        cout << "  +------------------------------------------+" << endl;
        cout << "  | Status       : " << (isActive ? "Active" : "Checked Out") << endl;
        cout << "  +------------------------------------------+" << endl;
    }

    void displaySummary() const {
        cout << "  | " << left
             << setw(8)  << bookingId
             << setw(18) << guestName
             << setw(8)  << roomNo
             << setw(12) << checkIn
             << setw(10) << fixed << setprecision(0) << totalBill
             << (isActive ? "Active" : "Checked Out") << endl;
    }
};

// ─────────────────────────────────────────────
//  CLASS: Hotel (Main Controller)
// ─────────────────────────────────────────────
class Hotel {
private:
    string         hotelName;
    vector<Room>   rooms;
    vector<Guest>  guests;
    vector<Booking> bookings;
    int nextGuestId;
    int nextBookingId;

    Room* findRoom(int roomNo) {
        for (auto& r : rooms)
            if (r.getRoomNo() == roomNo) return &r;
        return nullptr;
    }

    Guest* findGuest(int guestId) {
        for (auto& g : guests)
            if (g.getGuestId() == guestId) return &g;
        return nullptr;
    }

    Booking* findActiveBooking(int roomNo) {
        for (auto& b : bookings)
            if (b.getRoomNo() == roomNo && b.getIsActive()) return &b;
        return nullptr;
    }

public:
    Hotel(string name) : hotelName(name), nextGuestId(1001), nextBookingId(5001) {
        // Initialize rooms
        // Single rooms: 101-105 @ Rs.1500/night
        for (int i = 101; i <= 105; i++)
            rooms.push_back(Room(i, "Single", 1500));
        // Double rooms: 201-204 @ Rs.2500/night
        for (int i = 201; i <= 204; i++)
            rooms.push_back(Room(i, "Double", 2500));
        // Suites: 301-302 @ Rs.5000/night
        for (int i = 301; i <= 302; i++)
            rooms.push_back(Room(i, "Suite", 5000));
    }

    // ── 1. Show all rooms ──
    void showAllRooms() {
        cout << "\n  +--------------------------------------------------+" << endl;
        cout << "  |                  ALL ROOMS                       |" << endl;
        cout << "  +--------------------------------------------------+" << endl;
        cout << "  | " << left
             << setw(6)  << "Room"
             << setw(10) << "Type"
             << setw(12) << "Price/Night"
             << setw(10) << "Status"
             << "Booked By" << endl;
        cout << "  +--------------------------------------------------+" << endl;
        for (const auto& r : rooms)
            r.display();
        cout << "  +--------------------------------------------------+" << endl;
    }

    // ── 2. Show available rooms ──
    void showAvailableRooms() {
        cout << "\n  ===== AVAILABLE ROOMS =====" << endl;
        cout << "  " << left
             << setw(8)  << "Room No"
             << setw(10) << "Type"
             << "Price/Night" << endl;
        cout << "  " << string(32, '-') << endl;
        bool found = false;
        for (const auto& r : rooms) {
            if (!r.getIsBooked()) {
                cout << "  " << setw(8)  << r.getRoomNo()
                     << setw(10) << r.getType()
                     << "Rs. " << fixed << setprecision(0) << r.getPrice() << endl;
                found = true;
            }
        }
        if (!found) cout << "  No rooms available." << endl;
    }

    // ── 3. Add guest ──
    void addGuest() {
        string name, phone, address;
        cin.ignore();
        cout << "\n  Enter Guest Name    : "; getline(cin, name);
        cout << "  Enter Phone Number  : "; getline(cin, phone);
        cout << "  Enter Address       : "; getline(cin, address);
        guests.push_back(Guest(nextGuestId, name, phone, address));
        cout << "\n  [OK] Guest registered! Guest ID: " << nextGuestId << endl;
        nextGuestId++;
    }

    // ── 4. Book a room ──
    void bookRoom() {
        int guestId, roomNo, nights;
        cout << "\n  Enter Guest ID   : "; cin >> guestId;
        Guest* g = findGuest(guestId);
        if (!g) { cout << "\n  [ERR] Guest not found. Please register first." << endl; return; }

        showAvailableRooms();
        cout << "\n  Enter Room No    : "; cin >> roomNo;
        cout << "  Enter No. of Nights : "; cin >> nights;
        if (nights <= 0) { cout << "\n  [ERR] Nights must be at least 1." << endl; return; }

        Room* r = findRoom(roomNo);
        if (!r) { cout << "\n  [ERR] Room not found." << endl; return; }

        try {
            r->book(g->getName());
            bookings.push_back(Booking(
                nextBookingId, guestId, g->getName(),
                roomNo, r->getType(), r->getPrice(), nights, getDate()
            ));
            cout << "\n  [OK] Room " << roomNo << " booked for " << g->getName() << "!" << endl;
            cout << "  Booking ID : " << nextBookingId << endl;
            cout << "  Total Bill : Rs. " << fixed << setprecision(2)
                 << r->getPrice() * nights << endl;
            nextBookingId++;
        } catch (exception& e) {
            cout << "\n  [ERR] " << e.what() << endl;
        }
    }

    // ── 5. Checkout ──
    void checkoutRoom() {
        int roomNo;
        cout << "\n  Enter Room No to checkout: "; cin >> roomNo;

        Room* r = findRoom(roomNo);
        if (!r) { cout << "\n  [ERR] Room not found." << endl; return; }

        Booking* b = findActiveBooking(roomNo);
        if (!b) { cout << "\n  [ERR] No active booking for this room." << endl; return; }

        try {
            b->checkout(getDate());
            r->vacate();
            cout << "\n  [OK] Checkout successful!" << endl;
            b->displayBill();
        } catch (exception& e) {
            cout << "\n  [ERR] " << e.what() << endl;
        }
    }

    // ── 6. View bill by booking ID ──
    void viewBill() {
        int bookingId;
        cout << "\n  Enter Booking ID: "; cin >> bookingId;
        for (const auto& b : bookings) {
            if (b.getBookingId() == bookingId) {
                b.displayBill();
                return;
            }
        }
        cout << "\n  [ERR] Booking not found." << endl;
    }

    // ── 7. Show all bookings ──
    void showAllBookings() {
        if (bookings.empty()) { cout << "\n  No bookings yet." << endl; return; }
        cout << "\n  +----------------------------------------------------------+" << endl;
        cout << "  |                   ALL BOOKINGS                           |" << endl;
        cout << "  +----------------------------------------------------------+" << endl;
        cout << "  | " << left
             << setw(8)  << "Bk ID"
             << setw(18) << "Guest Name"
             << setw(8)  << "Room"
             << setw(12) << "Check-In"
             << setw(10) << "Bill"
             << "Status" << endl;
        cout << "  +----------------------------------------------------------+" << endl;
        for (const auto& b : bookings)
            b.displaySummary();
        cout << "  +----------------------------------------------------------+" << endl;
    }

    // ── 8. Show all guests ──
    void showAllGuests() {
        if (guests.empty()) { cout << "\n  No guests registered." << endl; return; }
        cout << "\n  ===== REGISTERED GUESTS =====" << endl;
        for (const auto& g : guests)
            g.display();
    }

    // ── 9. Hotel summary ──
    void showSummary() {
        int total = rooms.size(), booked = 0;
        float totalRevenue = 0;
        for (const auto& r : rooms)
            if (r.getIsBooked()) booked++;
        for (const auto& b : bookings)
            if (!b.getIsActive()) totalRevenue += b.getTotalBill();

        cout << "\n  ======= HOTEL SUMMARY =======" << endl;
        cout << "  Hotel Name     : " << hotelName          << endl;
        cout << "  Total Rooms    : " << total              << endl;
        cout << "  Booked Rooms   : " << booked             << endl;
        cout << "  Available Rooms: " << (total - booked)   << endl;
        cout << "  Total Guests   : " << guests.size()      << endl;
        cout << "  Total Bookings : " << bookings.size()    << endl;
        cout << "  Total Revenue  : Rs. " << fixed << setprecision(2) << totalRevenue << endl;
        cout << "  ==============================" << endl;
    }
};

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main() {
    Hotel hotel("DevAlpha Grand Hotel");

    cout << "\n  +==========================================+" << endl;
    cout << "  |       DevAlpha Grand Hotel               |" << endl;
    cout << "  |     Management System v1.0               |" << endl;
    cout << "  +==========================================+" << endl;

    int choice;
    do {
        cout << "\n  ------- MAIN MENU -------"   << endl;
        cout << "  1.  Show All Rooms"            << endl;
        cout << "  2.  Show Available Rooms"      << endl;
        cout << "  3.  Register New Guest"        << endl;
        cout << "  4.  Book a Room"               << endl;
        cout << "  5.  Checkout"                  << endl;
        cout << "  6.  View Bill"                 << endl;
        cout << "  7.  All Bookings"              << endl;
        cout << "  8.  All Guests"                << endl;
        cout << "  9.  Hotel Summary"             << endl;
        cout << "  0.  Exit"                      << endl;
        cout << "  Enter choice: ";
        cin  >> choice;

        switch (choice) {
            case 1: hotel.showAllRooms();      break;
            case 2: hotel.showAvailableRooms(); break;
            case 3: hotel.addGuest();          break;
            case 4: hotel.bookRoom();          break;
            case 5: hotel.checkoutRoom();      break;
            case 6: hotel.viewBill();          break;
            case 7: hotel.showAllBookings();   break;
            case 8: hotel.showAllGuests();     break;
            case 9: hotel.showSummary();       break;
            case 0: break;
            default: cout << "\n  [ERR] Invalid choice." << endl;
        }
    } while (choice != 0);

    cout << "\n  Thank you for using DevAlpha Grand Hotel System!\n" << endl;
    return 0;
}
