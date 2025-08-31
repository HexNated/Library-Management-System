#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Book {
private:
    string title;
    string author;
    string isbn;
    bool isAvailable;

public:
    Book() {}

    Book(string t, string a, string i, bool available = true)
        : title(t), author(a), isbn(i), isAvailable(available) {}

    string getTitle() { return title; }
    string getAuthor() { return author; }
    string getISBN() { return isbn; }
    bool available() { return isAvailable; }

    void setAvailability(bool status) { isAvailable = status; }

    string serialize() {
        return title + "," + author + "," + isbn + "," + (isAvailable ? "1" : "0");
    }

    static Book parse(string line) {
        stringstream ss(line);
        string t, a, i, av;
        getline(ss, t, ',');
        getline(ss, a, ',');
        getline(ss, i, ',');
        getline(ss, av);
        return Book(t, a, i, av == "1");
    }
};

class LibraryUser {
private:
    string userID;
    string name;
    vector<string> borrowedBooks;

public:
    LibraryUser() {}

    LibraryUser(string id, string n) : userID(id), name(n) {}

    string getID() { return userID; }
    string getName() { return name; }

    void borrow(string isbn) {
        borrowedBooks.push_back(isbn);
    }

    void returnBook(string isbn) {
        for (int i = 0; i < borrowedBooks.size(); i++) {
            if (borrowedBooks[i] == isbn) {
                borrowedBooks.erase(borrowedBooks.begin() + i);
                break;
            }
        }
    }

    bool hasBorrowed(string isbn) {
        for (string b : borrowedBooks) {
            if (b == isbn) return true;
        }
        return false;
    }

    void showBorrowedBooks() {
        if (borrowedBooks.empty()) {
            cout << "No books borrowed.\n";
            return;
        }

        cout << "Borrowed books: ";
        for (string b : borrowedBooks) {
            cout << b << " ";
        }
        cout << endl;
    }

    string serialize() {
        string data = userID + "," + name;
        for (string b : borrowedBooks) {
            data += "," + b;
        }
        return data;
    }

    static LibraryUser parse(string line) {
        stringstream ss(line);
        string id, name, isbn;
        getline(ss, id, ',');
        getline(ss, name, ',');

        LibraryUser user(id, name);
        while (getline(ss, isbn, ',')) {
            user.borrow(isbn);
        }
        return user;
    }
};

class Library {
private:
    vector<Book> books;
    vector<LibraryUser> users;

    void loadBooks() {
        ifstream file("books.txt");
        if (!file.is_open()) {
            ofstream create("books.txt"); 
            create.close();
            return;
        }

        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                books.push_back(Book::parse(line));
            }
        }
        file.close();
    }

    void loadUsers() {
        ifstream file("users.txt");
        if (!file.is_open()) {
            ofstream create("users.txt"); 
            create.close();
            return;
        }

        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                users.push_back(LibraryUser::parse(line));
            }
        }
        file.close();
    }

    void saveBooks() {
        ofstream file("books.txt");
        for (Book b : books) {
            file << b.serialize() << endl;
        }
        file.close();
    }

    void saveUsers() {
        ofstream file("users.txt");
        for (LibraryUser u : users) {
            file << u.serialize() << endl;
        }
        file.close();
    }

public:
    Library() {
        loadBooks();
        loadUsers();
    }

    ~Library() {
        saveBooks();
        saveUsers();
    }

    void addBook() {
        string title, author, isbn;
        cin.ignore();
        cout << "Enter title: ";
        getline(cin, title);
        cout << "Enter author: ";
        getline(cin, author);
        cout << "Enter ISBN: ";
        getline(cin, isbn);

        books.push_back(Book(title, author, isbn));
        cout << "Book added.\n";
    }

    void removeBook() {
        string isbn;
        cout << "Enter ISBN to remove: ";
        cin >> isbn;

        for (int i = 0; i < books.size(); i++) {
            if (books[i].getISBN() == isbn) {
                books.erase(books.begin() + i);
                cout << "? Book removed.\n";
                return;
            }
        }
        cout << "Book not found.\n";
    }

    void registerUser() {
        string id, name;
        cout << "Enter User ID: ";
        cin >> id;
        cin.ignore();
        cout << "Enter Name: ";
        getline(cin, name);

        users.push_back(LibraryUser(id, name));
        cout << "User registered.\n";
    }

    void removeUser() {
        string id;
        cout << "Enter User ID to remove: ";
        cin >> id;

        for (int i = 0; i < users.size(); i++) {
            if (users[i].getID() == id) {
                users.erase(users.begin() + i);
                cout << "User removed.\n";
                return;
            }
        }
        cout << "User not found.\n";
    }

    void borrowBook() {
        string id, isbn;
        cout << "Enter User ID: ";
        cin >> id;
        cout << "Enter ISBN to borrow: ";
        cin >> isbn;

        int userIndex = -1, bookIndex = -1;
        for (int i = 0; i < users.size(); i++) {
            if (users[i].getID() == id) {
                userIndex = i;
                break;
            }
        }

        for (int i = 0; i < books.size(); i++) {
            if (books[i].getISBN() == isbn) {
                bookIndex = i;
                break;
            }
        }

        if (userIndex != -1 && bookIndex != -1) {
            if (books[bookIndex].available()) {
                books[bookIndex].setAvailability(false);
                users[userIndex].borrow(isbn);
                cout << "Book borrowed.\n";
            } else {
                cout << "Book is already borrowed.\n";
            }
        } else {
            cout << "Invalid User ID or ISBN.\n";
        }
    }

    void returnBook() {
        string id, isbn;
        cout << "Enter User ID: ";
        cin >> id;
        cout << "Enter ISBN to return: ";
        cin >> isbn;

        int userIndex = -1, bookIndex = -1;
        for (int i = 0; i < users.size(); i++) {
            if (users[i].getID() == id) {
                userIndex = i;
                break;
            }
        }

        for (int i = 0; i < books.size(); i++) {
            if (books[i].getISBN() == isbn) {
                bookIndex = i;
                break;
            }
        }

        if (userIndex != -1 && bookIndex != -1) {
            if (users[userIndex].hasBorrowed(isbn)) {
                books[bookIndex].setAvailability(true);
                users[userIndex].returnBook(isbn);
                cout << "Book returned.\n";
            } else {
                cout << "User didn't borrow this book.\n";
            }
        } else {
            cout << "Invalid User ID or ISBN.\n";
        }
    }

    void showAllBooks() {
        cout << "\n All Books:\n";
        for (Book b : books) {
            cout << "Title: " << b.getTitle()
                 << " | Author: " << b.getAuthor()
                 << " | ISBN: " << b.getISBN()
                 << " | Available: " << (b.available() ? "Yes" : "No") << endl;
        }
    }

    void showAllUsers() {
        cout << "\n All Users:\n";
        for (LibraryUser u : users) {
            cout << "ID: " << u.getID() << " | Name: " << u.getName() << endl;
            u.showBorrowedBooks();
        }
    }
};

int main() {
    Library lib;
    int choice;

    do {
        cout << "\n====== LIBRARY MENU ======\n";
        cout << "1. Add Book\n";
        cout << "2. Remove Book\n";
        cout << "3. Register User\n";
        cout << "4. Remove User\n";
        cout << "5. Borrow Book\n";
        cout << "6. Return Book\n";
        cout << "7. Show All Books\n";
        cout << "8. Show All Users\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: lib.addBook(); break;
            case 2: lib.removeBook(); break;
            case 3: lib.registerUser(); break;
            case 4: lib.removeUser(); break;
            case 5: lib.borrowBook(); break;
            case 6: lib.returnBook(); break;
            case 7: lib.showAllBooks(); break;
            case 8: lib.showAllUsers(); break;
            case 9: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 9);

    return 0;
}
