#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>

using namespace std;

// Helper functions
string toString(int value) {
    ostringstream oss;
    oss << value;
    return oss.str();
}

int toInt(const string& value) {
    istringstream iss(value);
    int result;
    iss >> result;
    return result;
}

// Book structure
struct Book {
    int id;
    string name;
    string author;
    int quantity;

    Book(int id, string name, string author, int quantity)
        : id(id), name(name), author(author), quantity(quantity) {}

    string toString() const {
        return "ID: " + ::toString(id) + ", Name: " + name + ", Author: " + author + ", Quantity: " + ::toString(quantity);
    }
};

// Linked list node
struct Node {
    Book book;
    Node* next;

    Node(Book b) : book(b), next(NULL) {}
};

// Book Linked List
class BookLinkedList {
private:
    Node* head;

public:
    BookLinkedList() : head(NULL) {}

    void addBook(const Book& book) {
        // If a book with the same ID exists, don't add it again
        if (searchById(book.id)) {
            cout << "Book with ID " << book.id << " already exists. Update or add a new book with a different ID.\n";
            return;
        }

        // Create a new node for the book
        Node* newNode = new Node(book);

        // If the list is empty or the book ID is smaller than the head's ID, insert at the front
        if (!head || head->book.id > book.id) {
            newNode->next = head;
            head = newNode;
        } else {
            // Traverse the list to find the appropriate insertion point
            Node* temp = head;
            while (temp->next && temp->next->book.id < book.id) {
                temp = temp->next;
            }
            // Insert the new book at the correct position
            newNode->next = temp->next;
            temp->next = newNode;
        }
        cout << "Book added successfully!\n";
    }

    void displayBooks() const {
        if (!head) {
            cout << "No books in the database.\n";
            return;
        }
        Node* temp = head;
        cout << "ID\tName\t\tAuthor\t\tQuantity\n";
        while (temp) {
            cout << temp->book.id << "\t" << temp->book.name << "\t\t"
                 << temp->book.author << "\t\t" << temp->book.quantity << "\n";
            temp = temp->next;
        }
    }

    Node* searchById(int id) const {
        Node* temp = head;
        while (temp) {
            if (temp->book.id == id) {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }

    ~BookLinkedList() {
        Node* current = head;
        while (current) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }
};

// Student behavior tracking
class StudentPortal {
private:
    map<string, vector<int> > userBehavior; // Maps username to book IDs
    set<int> reservedBooks;               // Reserved book IDs
    map<int, int> borrowedBooks;          // Maps book ID to quantity borrowed

public:
    void borrowBook(const string& username, BookLinkedList& books, int bookId) {
        Node* bookNode = books.searchById(bookId);
        if (bookNode && bookNode->book.quantity > 0) {
            bookNode->book.quantity--;
            borrowedBooks[bookId]++;
            userBehavior[username].push_back(bookId);
            cout << "Book borrowed successfully: " << bookNode->book.name << "\n";
        } else {
            cout << "Book unavailable for borrowing.\n";
        }
    }

    void reserveBook(const string& username, BookLinkedList& books, int bookId) {
        Node* bookNode = books.searchById(bookId);
        if (bookNode) {
            reservedBooks.insert(bookId);
            userBehavior[username].push_back(bookId);
            cout << "Book reserved successfully: " << bookNode->book.name << "\n";
        } else {
            cout << "Book ID not found.\n";
        }
    }

    void returnBook(BookLinkedList& books, int bookId) {
        if (borrowedBooks[bookId] > 0) {
            borrowedBooks[bookId]--;
            Node* bookNode = books.searchById(bookId);
            if (bookNode) {
                bookNode->book.quantity++;
                cout << "Book returned successfully: " << bookNode->book.name << "\n";
            }
        } else {
            cout << "No borrowed record for this book ID.\n";
        }
    }

    void suggestBooks(const string& username, BookLinkedList& books) {
        cout << "Suggested books for " << username << ":\n";

        // Retrieve the user's behavior and avoid duplicates using a set
        vector<int>& userBookIds = userBehavior[username];
        set<int> uniqueBookIds;
        for (vector<int>::iterator it = userBookIds.begin(); it != userBookIds.end(); ++it) {
            uniqueBookIds.insert(*it);
        }

        // Iterate through the unique book IDs to find and display book details
        for (set<int>::iterator it = uniqueBookIds.begin(); it != uniqueBookIds.end(); ++it) {
            Node* bookNode = books.searchById(*it);
            if (bookNode) {
                cout << "- " << bookNode->book.name << " by " << bookNode->book.author << "\n";
            }
        }
    }
};

// Admin Menu
void adminMenu(BookLinkedList& books) {
    int choice;
    do {
        cout << "\n=== Admin Menu ===\n";
        cout << "1. Add Book\n";
        cout << "2. View Books\n";
        cout << "3. Exit Admin Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int id, quantity;
                string name, author;
                cout << "Enter Book ID: ";
                cin >> id;
                cin.ignore(); // To clear the newline character left by cin >> id

                cout << "Enter Book Name: ";
                getline(cin, name);  // Correct way to read a string with spaces

                cout << "Enter Author Name: ";
                getline(cin, author);  // Correct way to read a string with spaces

                cout << "Enter Quantity: ";
                cin >> quantity;

                // Add the book to the list
                books.addBook(Book(id, name, author, quantity));
                break;
            }
            case 2:
                books.displayBooks();
                break;
            case 3:
                cout << "Exiting Admin Menu.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 3);
}

// Student Menu
void studentMenu(BookLinkedList& books, StudentPortal& portal) {
    int choice;
    do {
        cout << "\n=== Student Menu ===\n";
        cout << "1. Borrow Book\n";
        cout << "2. Reserve Book\n";
        cout << "3. Return Book\n";
        cout << "4. Get Suggestions\n";
        cout << "5. Exit Student Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string username;
                int bookId;
                cout << "Enter your username: ";
                cin >> username;
                cout << "Enter Book ID to borrow: ";
                cin >> bookId;
                portal.borrowBook(username, books, bookId);
                break;
            }
            case 2: {
                string username;
                int bookId;
                cout << "Enter your username: ";
                cin >> username;
                cout << "Enter Book ID to reserve: ";
                cin >> bookId;
                portal.reserveBook(username, books, bookId);
                break;
            }
            case 3: {
                int bookId;
                cout << "Enter Book ID to return: ";
                cin >> bookId;
                portal.returnBook(books, bookId);
                break;
            }
            case 4: {
                string username;
                cout << "Enter your username: ";
                cin >> username;
                portal.suggestBooks(username, books);
                break;
            }
            case 5:
                cout << "Exiting Student Menu.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5);
}

// Main Program
int main() {
    BookLinkedList books;
    StudentPortal portal;
    int userType;

    do {
        cout << "\n=== Library Management System ===\n";
        cout << "1. Admin\n";
        cout << "2. Student\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> userType;

        switch (userType) {
            case 1:
                adminMenu(books);
                break;
            case 2:
                studentMenu(books, portal);
                break;
            case 3:
                cout << "Exiting Program.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (userType != 3);

    return 0;
}

