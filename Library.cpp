#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Book {
private:
    string title, author, isbn;
    bool isAvailable;

public:
    Book(string t = "", string a = "", string i = "", bool av = true)
        : title(t), author(a), isbn(i), isAvailable(av) {}

    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getISBN() const { return isbn; }
    bool available() const { return isAvailable; }

    void setAvailability(bool av) { isAvailable = av; }

    void showBook() const {
        cout << "Title: " << title
             << " | Author: " << author
             << " | ISBN: " << isbn
             << " | " << (isAvailable ? "Available" : "Borrowed") << endl;
    }

    // Save to string (for file)
    string toString() const {
        return title + ";" + author + ";" + isbn + ";" + (isAvailable ? "1" : "0");
    }

    void fromString(string data) {
        size_t p1 = data.find(';');
        size_t p2 = data.find(';', p1 + 1);
        size_t p3 = data.find(';', p2 + 1);

        title = data.substr(0, p1);
        author = data.substr(p1 + 1, p2 - p1 - 1);
        isbn = data.substr(p2 + 1, p3 - p2 - 1);
        isAvailable = (data.substr(p3 + 1) == "1");
    }
};

class User {
private:
    string id, name;
    vector<string> borrowed;

public:
    User(string uid = "", string uname = "") : id(uid), name(uname) {}

    string getID() const { return id; }
    string getName() const { return name; }

    void borrowBook(string isbn) { borrowed.push_back(isbn); }
    void returnBook(string isbn) {
        auto it = find(borrowed.begin(), borrowed.end(), isbn);
        if (it != borrowed.end()) borrowed.erase(it);
    }

    void showUser() const {
        cout << "ID: " << id << " | Name: " << name << "\n   Borrowed: ";
        if (borrowed.empty()) cout << "None";
        else for (auto &b : borrowed) cout << b << " ";
        cout << endl;
    }

    string toString() const {
        string data = id + ";" + name + ";";
        for (size_t i = 0; i < borrowed.size(); i++) {
            data += borrowed[i];
            if (i != borrowed.size() - 1) data += ",";
        }
        return data;
    }

    void fromString(string data) {
        size_t p1 = data.find(';');
        size_t p2 = data.find(';', p1 + 1);

        id = data.substr(0, p1);
        name = data.substr(p1 + 1, p2 - p1 - 1);

        borrowed.clear();
        string books = data.substr(p2 + 1);
        size_t start = 0, end;
        while ((end = books.find(',', start)) != string::npos) {
            borrowed.push_back(books.substr(start, end - start));
            start = end + 1;
        }
        if (!books.empty()) borrowed.push_back(books.substr(start));
    }
};

class Library {
private:
    vector<Book> books;
    vector<User> users;

public:
    string bookFile = "books.txt";
    string userFile = "users.txt";

    Library() { load(); }
    ~Library() { save(); }

    void addBook() {
        string t, a, i;
        cout << "Enter Title: "; getline(cin, t);
        cout << "Enter Author: "; getline(cin, a);
        cout << "Enter ISBN: "; getline(cin, i);
        books.push_back(Book(t, a, i));
        cout << "Book added!\n";
    }

    void showBooks() {
        cout << "\n--- Books ---\n";
        for (auto &b : books) b.showBook();
    }

    void addUser() {
        string id, n;
        cout << "Enter ID: "; getline(cin, id);
        cout << "Enter Name: "; getline(cin, n);
        users.push_back(User(id, n));
        cout << "User added!\n";
    }

    void showUsers() {
        cout << "\n--- Users ---\n";
        for (auto &u : users) u.showUser();
    }

    void borrowBook() {
        string id, isbn;
        cout << "User ID: "; getline(cin, id);
        cout << "Book ISBN: "; getline(cin, isbn);

        auto u = find_if(users.begin(), users.end(), [&](User &usr) { return usr.getID() == id; });
        auto b = find_if(books.begin(), books.end(), [&](Book &bk) { return bk.getISBN() == isbn; });

        if (u != users.end() && b != books.end() && b->available()) {
            u->borrowBook(isbn);
            b->setAvailability(false);
            cout << "Borrowed successfully!\n";
        } else cout << "Borrow failed!\n";
    }

    void returnBook() {
        string id, isbn;
        cout << "User ID: "; getline(cin, id);
        cout << "Book ISBN: "; getline(cin, isbn);

        auto u = find_if(users.begin(), users.end(), [&](User &usr) { return usr.getID() == id; });
        auto b = find_if(books.begin(), books.end(), [&](Book &bk) { return bk.getISBN() == isbn; });

        if (u != users.end() && b != books.end() && !b->available()) {
            u->returnBook(isbn);
            b->setAvailability(true);
            cout << "Returned successfully!\n";
        } else cout << "Return failed!\n";
    }

    void load() {
        ifstream bf(bookFile), uf(userFile);
        string line;

        while (getline(bf, line)) {
            Book b; b.fromString(line);
            books.push_back(b);
        }
        while (getline(uf, line)) {
            User u; u.fromString(line);
            users.push_back(u);
        }
    }

    void save() {
        ofstream bf(bookFile), uf(userFile);
        for (auto &b : books) bf << b.toString() << endl;
        for (auto &u : users) uf << u.toString() << endl;
    }
};

int main() {
    Library lib;
    int choice;

    do {
        cout << "\n===== Library Menu =====\n";
        cout << "1. Add Book\n2. Show Books\n3. Add User\n4. Show Users\n5. Borrow Book\n6. Return Book\n0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: lib.addBook(); break;
            case 2: lib.showBooks(); break;
            case 3: lib.addUser(); break;
            case 4: lib.showUsers(); break;
            case 5: lib.borrowBook(); break;
            case 6: lib.returnBook(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    return 0;
}