#include <iostream>
#include <limits>

using namespace std;

struct User {
  string username;
  string password;
  bool admin;
};

struct Film {
  string title;
  string director;
  string genre;
  int year;
  string sinopsis;
};

struct FilmNode {
  Film film;
  FilmNode *next;
};

struct UserNode {
  User user;
  UserNode *next;
};

// Operasinya akan ada addFirst, addLast, addSpecific, Update, deleteFirst,
// deleteLast, deleteSpecific
FilmNode *headFilm = NULL;
int filmCount = 0;

// Operasinya akan ada login (traversal dan cek), register (addLast)
UserNode *headUser = NULL;
int userCount = 0;

// UTILITY //
void clearScreen() { cout << "\033[2J\033[1;1H"; }

void printMessage(string message) {
  cout << "\n";
  cout << "Message: " << message << endl;
  cout << "Tekan enter untuk melanjutkan...";
  cin.ignore();
  clearScreen();
}

/*
  Function untuk mengambil input menu dari user, serta melakukan validasi
  Return: int
*/
int inputMenu() {
  int menu;
  cout << "Masukkan pilihan menu: ";
  if (!(cin >> menu)) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    printMessage("Input tidak valid");
    return -1;
  } else {
    return menu;
  }
}

void registerUser(UserNode **headUser, User user) {
  UserNode *newNode = new UserNode;
  newNode->user = user;
  newNode->next = NULL;

  if (*headUser == NULL) {
    *headUser = newNode;
  } else {
    // traversal sampe ujung
    UserNode *temp = *headUser;
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = newNode;
  }
}

void readUser(UserNode *headUser) {
  UserNode *temp = headUser;
  while (temp != NULL) {
    cout << temp->user.username << endl;
    temp = temp->next;
  }
}

User createUser(string username = "", string password = "",
                bool admin = false) {
  User user;

  if (username == "") {
    cout << "Masukkan username: ";
    cin >> username;
  }
  if (password == "") {
    cout << "Masukkan password: ";
    cin >> password;
  }
  user.username = username;
  user.password = password;
  user.admin = admin;

  return user;
}

Film createFilmNode(string title = "", string director = "", string genre = "",
                    int year = 0) {
  Film film;

  if (title == "") {
    cout << "Masukkan judul film: ";
    getline(cin, title);
  }
  if (director == "") {
    cout << "Masukkan sutradara film: ";
    getline(cin, director);
  }
  if (genre == "") {
    cout << "Masukkan genre film: ";
    getline(cin, genre);
  }
  if (year == 0) {
    cout << "Masukkan tahun film: ";
    cin >> year;
  }

  film.title = title;
  film.director = director;
  film.genre = genre;
  film.year = year;

  return film;
}

void addFilmAtFirst(FilmNode **headFilm, Film film, int &count) {
  FilmNode *newNode = new FilmNode;
  newNode->film = film;
  newNode->next = *headFilm;
  *headFilm = newNode;
  count++;
}

void addFilmAtLast(FilmNode **headFilm, Film film, int &count) {
  FilmNode *newNode = new FilmNode;
  newNode->film = film;
  newNode->next = NULL;

  if (*headFilm == NULL) {
    addFilmAtFirst(headFilm, film, count);
    return;
  }

  // traversal sampe ujung
  FilmNode *temp = *headFilm;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = newNode;
  count++;
}

void addFilmSpecific(FilmNode **headFilm, Film film, int &count, int position) {
  FilmNode *newNode = new FilmNode;
  newNode->film = film;
  newNode->next = NULL;

  // pake yang udah ada kalo nambahin di posisi pertama
  if (*headFilm == NULL || position == 1) {
    addFilmAtFirst(headFilm, film, count);
    return;
  }

  // pake yang udah ada kalo nambahin di posisi terakhir
  // misal ada 5 data, kalo mau nambahin di posisi 6, berarti nambahin di posisi
  // terakhir
  if (position == count + 1) {
    addFilmAtLast(headFilm, film, count);
    return;
  }

  // traversal sampe ujung
  FilmNode *temp = *headFilm;
  // dikurang 2 karena posisi dimulai dari 1, dan kita mau sampe satu sebelum
  // posisi yang dituju
  for (int i = 0; i < position - 2; i++) {
    temp = temp->next;
  }

  newNode->next = temp->next;
  temp->next = newNode;
  count++;
}

// TODO: nanti handle pas pemanggilan, cek dulu countnya
void deleteFilmAtFirst(FilmNode **headFilm, int &count) {
  if (*headFilm == NULL) {
    return;
  }

  FilmNode *temp = *headFilm;
  *headFilm = temp->next;
  delete temp;
  count--;
}

// TODO nanti handle pas pemanggilan, cek dulu countnya
void deleteFilmAtLast(FilmNode **headFilm, int &count) {
  if (*headFilm == NULL) {
    return;
  }

  FilmNode *temp = *headFilm;
  while (temp->next->next != NULL) {
    temp = temp->next;
  }
  delete temp->next;
  temp->next = NULL;
  count--;
}

// TODO nanti handle pas pemanggilan, cek dulu countnya
void deleteFilmSpecific(FilmNode **headFilm, int &count, int position) {
  if (*headFilm == NULL) {
    return;
  }

  FilmNode *temp = *headFilm;
  for (int i = 0; i < position - 2; i++) {
    temp = temp->next;
  }
  FilmNode *nextNode = temp->next;
  temp->next = temp->next->next;
  delete nextNode;
  count--;
}

// TODO nanti handle pas pemanggilan, cek dulu countnya
void updateFilm(FilmNode **headFilm, Film film, int position) {
  if (*headFilm == NULL) {
    return;
  }

  FilmNode *temp = *headFilm;
  for (int i = 0; i < position - 1; i++) {
    temp = temp->next;
  }
  temp->film = film;
}

void readFilm(FilmNode *headFilm) {
  FilmNode *temp = headFilm;
  while (temp != NULL) {
    cout << temp->film.title << endl;
    temp = temp->next;
  }
}

/*
  Menu Admin (CRUD). Pada menu ini admin dapat melakukan operasi CRUD
*/
void adminMenu() {
  while (true) {
    cout << "Pilihan Menu" << endl;
    cout << "[0] Keluar" << endl;
    cout << "[1] Tambah Film" << endl;
    cout << "[2] Tampilkan Film" << endl;
    cout << "[3] Hapus Film" << endl;
    cout << "[4] Cari Film" << endl;
    cout << "[5] Urutkan Film" << endl;

    int choice = inputMenu();
    if (choice == -1) {
      continue;
    }
  }
}

int main() {
  // adminMenu();

  registerUser(&headUser, createUser("admin", "admin", true));
  registerUser(&headUser, createUser("user", "user", false));

  addFilmAtLast(&headFilm,
                createFilmNode("Avengers", "Joss Whedon", "Action", 2012),
                filmCount);
  addFilmAtLast(
      &headFilm,
      createFilmNode("Avengers: Age of Ultron", "Joss Whedon", "Action", 2015),
      filmCount);
  addFilmAtLast(
      &headFilm,
      createFilmNode("Avengers: Infinity War", "Anthony Russo", "Action", 2018),
      filmCount);
  deleteFilmAtFirst(&headFilm, filmCount);
  deleteFilmAtLast(&headFilm, filmCount);

  readFilm(headFilm);
  cout << filmCount << endl;

  return 0;
}
