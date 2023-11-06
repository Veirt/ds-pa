#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>

using namespace std;

struct User {
  string username;
  string password;
  bool admin;
  unordered_map<string, int> filmRatings;
};

struct Film {
  string title;
  string director;
  string genre;
  int year;
  string synopsis;
};

struct FilmNode {
  Film film;
  FilmNode *next;
};

struct UserNode {
  User user;
  UserNode *next;
};

struct HistoryStack {
  string title;
  HistoryStack *next;
};

// History
HistoryStack *topHistory = NULL;

// Operasinya akan ada addFirst, addLast, addSpecific, Update, deleteFirst,
// deleteLast, deleteSpecific
FilmNode *headFilm = NULL;
int filmCount = 0;

// Operasinya akan ada login (traversal dan cek), register (addLast)
UserNode *headUser = NULL;
int userCount = 0;

User *currentUser = NULL;

// UTILITY //
void clearScreen() { cout << "\033[2J\033[1;1H"; }

void printMessage(string message) {
  cout << "\n";
  cout << message << endl;
  cout << "Tekan enter untuk melanjutkan...";
  cin.ignore();
  clearScreen();
}

/*
  Function untuk mengambil input menu dari user, serta melakukan validasi
*/
int inputMenu() {
  int menu;
  cout << "Masukkan pilihan: ";
  if (!(cin >> menu) || menu < 0) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    printMessage("Input tidak valid");
    return -1;
  } else {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return menu;
  }
}

/*
  Function untuk mengambil input posisi dari user, serta melakukan validasi
  Jika input tidak valid, maka akan mengembalikan -1. -1 Akan dihandle saat
  pemanggilan.
*/
int inputPosition(int filmCount) {
  int position;

  cout << "Masukkan posisi film: ";
  if (!(cin >> position)) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    printMessage("Input tidak valid");
    return -1;
  } else if (position <= 0 || position > filmCount + 1) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    printMessage("Posisi tidak valid");
    return -1;
  } else {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return position;
  }
}

void pushHistory(string title) {
  HistoryStack *newNode = new HistoryStack;
  newNode->title = title;
  newNode->next = NULL;

  if (topHistory == NULL) {
    topHistory = newNode;
  } else {
    HistoryStack *temp = topHistory;
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = newNode;
  }
}

void readHistory() {
  HistoryStack *temp = topHistory;
  while (temp != NULL) {
    cout << temp->title << endl;
    temp = temp->next;
  }
}

/*
  Function untuk mengambil input rating dari user, dan untuk melakukan validasi
*/
int inputRating(Film film) {
  int rating;

  while (true) {
    cout << "Film: " << film.title << endl;
    cout << "Masukkan rating: ";
    if (!(cin >> rating)) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      printMessage("Input tidak valid");
      continue;
    }

    if (rating < 1 || rating > 10) {
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      printMessage("Rating harus diantara 1 sampai 10");
      continue;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return rating;
  }
}

string inputGenre() {
  string genre = "";

  while (true) {
    clearScreen();
    if (genre != "") {
      cout << "Genre film yang telah dipilih: " << genre << endl;
    }
    cout << "Pilihan Genre: " << endl;
    cout << "0. Selesai" << endl;
    cout << "1. Action" << endl;
    cout << "2. Adventure" << endl;
    cout << "3. Comedy" << endl;
    cout << "4. Drama" << endl;
    cout << "5. Fantasy" << endl;
    cout << "6. Horror" << endl;
    cout << "7. Mystery" << endl;
    cout << "8. Romance" << endl;

    int choice = inputMenu();
    if (choice == -1) {
      continue;
    }

    if (choice == 0) {
      if (genre == "") {
        printMessage("Genre tidak boleh kosong");
        continue;
      }
      break;
    }

    string newGenre;
    switch (choice) {
    case 1:
      newGenre = "Action";
      break;
    case 2:
      newGenre = "Adventure";
      break;
    case 3:
      newGenre = "Comedy";
      break;
    case 4:
      newGenre = "Drama";
      break;
    case 5:
      newGenre = "Fantasy";
      break;
    case 6:
      newGenre = "Horror";
      break;
    case 7:
      newGenre = "Mystery";
      break;
    case 8:
      newGenre = "Romance";
      break;
    default:
      printMessage("Pilihan tidak valid");
      continue;
    }

    // cek apakah genre sudah dipilih
    if (genre.find(newGenre) != string::npos) {
      printMessage("Genre sudah dipilih");
      continue;
    }

    if (genre != "") {
      genre += ", ";
    }

    genre += newGenre;
  }

  return genre;
}

Film *findByPosition(FilmNode *headFilm, int position) {
  FilmNode *temp = headFilm;
  for (int i = 0; i < position - 1; i++) {

    temp = temp->next;
    if (temp == NULL) {
      return NULL;
    }
  }

  return &temp->film;
}

Film *findByTitle(FilmNode *headFilm, string title) {
  FilmNode *temp = headFilm;
  while (temp != NULL) {
    if (temp->film.title == title) {
      return &temp->film;
    }
    temp = temp->next;
  }

  return NULL;
}

/*
  Function untuk mencari user berdasarkan username.
  Digunakan saat register, dan ketika load data dari file.
*/
User *findByUsername(UserNode *headUser, string username) {
  UserNode *temp = headUser;
  while (temp != NULL) {
    if (temp->user.username == username) {
      return &temp->user;
    }
    temp = temp->next;
  }

  return NULL;
}

/*
  Function untuk menambahkan user baru ke linked list.
  registerUser ini sama seperti addLast ke linked list user.
*/
User *registerUser(UserNode **headUser, User user) {
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

  return &newNode->user;
}

void readUser(UserNode *headUser) {
  UserNode *temp = headUser;
  while (temp != NULL) {
    cout << temp->user.username << endl;
    temp = temp->next;
  }
}

User createUser(string username = "", string password = "", bool admin = false,
                unordered_map<string, int> filmRatings = {}) {
  User user = *new User;

  if (username == "") {
    cout << "Masukkan username: ";
    getline(cin, username);
  }
  if (password == "") {
    cout << "Masukkan password: ";
    getline(cin, password);
  }
  user.username = username;
  user.password = password;
  user.admin = admin;
  user.filmRatings = filmRatings;

  return user;
}

bool validateFilmTitle(FilmNode *headFilm, string title) {
  FilmNode *temp = headFilm;
  while (temp != NULL) {
    if (temp->film.title == title) {
      return false;
    }
    temp = temp->next;
  }

  return true;
}

Film createFilm(string title = "", string director = "", string genre = "",
                int year = 0, string synopsis = "") {
  Film film;

  if (title == "") {
    while (true) {
      cout << "Masukkan judul film: ";
      getline(cin, title);

      if (!validateFilmTitle(headFilm, title)) {
        printMessage("Judul film sudah digunakan");
        continue;
      }

      if (title == "") {
        printMessage("Judul film tidak boleh kosong");
        continue;
      }

      break;
    }
  }

  if (director == "") {
    while (true) {
      cout << "Masukkan sutradara film: ";
      getline(cin, director);

      if (director == "") {
        printMessage("Sutradara film tidak boleh kosong");
        continue;
      }

      break;
    }
  }

  if (genre == "") {
    while (true) {
      genre = inputGenre();

      if (genre == "") {
        printMessage("Genre film tidak boleh kosong");
        continue;
      }

      break;
    }
  }

  if (year == 0) {
    while (true) {
      cout << "Masukkan tahun film: ";
      if (!(cin >> year)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        printMessage("Input tidak valid");
        continue;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (year < 0) {
        printMessage("Tahun film tidak boleh negatif");
        continue;
      } else if (year > 2023) {
        printMessage("Tahun film tidak boleh lebih dari 2023");
        continue;
      }

      break;
    }
  }

  if (synopsis == "") {
    while (true) {
      cout << "Masukkan sinopsis film: ";
      getline(cin, synopsis);

      if (synopsis == "") {
        printMessage("Sinopsis film tidak boleh kosong");
        continue;
      }

      break;
    }
  }

  film.title = title;
  film.director = director;
  film.genre = genre;
  film.year = year;
  film.synopsis = synopsis;

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
  // misal ada 5 data, kalo mau nambahin di posisi 6, berarti nambahin di
  // posisi terakhir
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

void deleteFilmRatings(UserNode **headUser, string filmTitle) {
  UserNode *temp = *headUser;
  while (temp != NULL) {
    if (temp->user.filmRatings.find(filmTitle) !=
        temp->user.filmRatings.end()) {
      temp->user.filmRatings.erase(filmTitle);
    }
    temp = temp->next;
  }
}

void deleteFilmAtFirst(FilmNode **headFilm, int &count) {
  if (*headFilm == NULL) {
    return;
  }

  FilmNode *temp = *headFilm;
  *headFilm = temp->next;
  deleteFilmRatings(&headUser, temp->film.title);
  delete temp;
  count--;
}

void deleteFilmAtLast(FilmNode **headFilm, int &count) {
  if (*headFilm == NULL) {
    return;
  }

  FilmNode *temp = *headFilm;
  while (temp->next->next != NULL) {
    temp = temp->next;
  }
  deleteFilmRatings(&headUser, temp->next->film.title);
  delete temp->next;
  temp->next = NULL;
  count--;
}

void deleteFilmSpecific(FilmNode **headFilm, int &count, int position) {
  // ketika head kosong : data belum ada
  if (*headFilm == NULL) {
    return;
  }

  // ketika posisi yang dihapus adalah posisi pertama: sama saja dengan
  // deleteFirst
  if (position == 1) {
    deleteFilmAtFirst(headFilm, count);
    return;
  }

  // ketika posisi yang dihapus adalah posisi terakhir: sama saja dengan
  // deleteLast
  if (position == count) {
    deleteFilmAtLast(headFilm, count);
    return;
  }

  // ketika posisi yang dihapus diluar batas
  // misal ada 5 data, kalo mau hapus di posisi 6, berarti diluar batas
  // langsung aja berhenti. dihandle pas pemanggilan
  if (position > count) {
    return;
  }

  FilmNode *temp = *headFilm;
  for (int i = 0; i < position - 2; i++) {
    temp = temp->next;
  }
  FilmNode *nextNode = temp->next;
  deleteFilmRatings(&headUser, nextNode->film.title);
  temp->next = temp->next->next;
  delete nextNode;
  count--;
}

void updateFilm(FilmNode **headFilm, Film film, int position) {
  if (*headFilm == NULL) {
    return;
  }

  // posisi kurang dari/sama dengan 0 atau lebih dari jumlah data
  if (position <= 0 || position > filmCount) {
    return;
  }

  FilmNode *temp = *headFilm;
  for (int i = 0; i < position - 1; i++) {
    temp = temp->next;
  }
  temp->film = film;
}

/*
  Function untuk menyimpan data user ke file dengan format tsv
*/
void saveToUserTsv(UserNode *headNode) {
  ofstream file;
  file.open("user.tsv");

  UserNode *temp = headNode;
  while (temp != NULL) {
    file << temp->user.username << "\t" << temp->user.password << "\t"
         << temp->user.admin << endl;
    temp = temp->next;
  }

  file.close();
}

void saveUserRatingFile(UserNode *headNode) {
  ofstream file;
  file.open("rating.tsv");

  UserNode *temp = headNode;
  while (temp != NULL) {
    for (auto const &x : temp->user.filmRatings) {
      Film *film = findByTitle(headFilm, x.first);
      if (film == NULL) {
        continue;
      }
      file << temp->user.username << "\t" << film->title << "\t" << x.second
           << endl;
    }

    temp = temp->next;
  }

  file.close();
}

/*
  Function untuk menyimpan data film ke file dengan format tsv
*/
void saveFilmFile(FilmNode &headNode) {
  ofstream file;
  file.open("film.tsv");

  FilmNode *temp = &headNode;
  while (temp != NULL) {
    file << temp->film.title << "\t" << temp->film.director << "\t"
         << temp->film.genre << "\t" << temp->film.year << "\t"
         << temp->film.synopsis << endl;
    temp = temp->next;
  }

  file.close();
}

/*
  Function untuk mengambil data user dari file dengan format tsv
*/
void loadUserFile(UserNode **headNode) {
  ifstream file;
  file.open("user.tsv");

  string line;
  while (getline(file, line)) {
    stringstream ss(line);
    string username, password, admin;
    getline(ss, username, '\t');
    getline(ss, password, '\t');
    getline(ss, admin, '\t');

    User user = createUser(username, password, admin == "1");

    registerUser(headNode, user);
  }

  file.close();
}

void loadRatingFile(UserNode **headNode) {
  ifstream file;
  file.open("rating.tsv");

  string line;
  while (getline(file, line)) {
    stringstream ss(line);
    string username, filmTitle, rating;
    getline(ss, username, '\t');
    getline(ss, filmTitle, '\t');
    getline(ss, rating, '\t');

    User *user = findByUsername(*headNode, username);
    if (user == NULL) {
      continue;
    }

    Film *film = findByTitle(headFilm, filmTitle);
    int ratingInt = stoi(rating);
    user->filmRatings[film->title] = ratingInt;
  }

  file.close();
}

void loadFilmFile(FilmNode **headNode) {
  ifstream file;
  file.open("film.tsv");

  string title, director, genre, year, synopsis;
  string line;
  while (getline(file, line)) {
    stringstream ss(line);
    getline(ss, title, '\t');
    getline(ss, director, '\t');
    getline(ss, genre, '\t');
    getline(ss, year, '\t');
    getline(ss, synopsis, '\t');

    addFilmAtLast(headNode,
                  createFilm(title, director, genre, stoi(year), synopsis),
                  filmCount);
  }

  file.close();
}

float calculateAvgRating(FilmNode *headFilm, UserNode *headUser,
                         string filmTitle) {
  FilmNode *temp = headFilm;
  int totalRating = 0;
  int totalUser = 0;
  while (temp != NULL) {
    if (temp->film.title == filmTitle) {
      UserNode *tempUser = headUser;
      while (tempUser != NULL) {
        if (tempUser->user.filmRatings.find(filmTitle) !=
            tempUser->user.filmRatings.end()) {

          // kalo ratingnya 0, berarti belum pernah di rate/udah di hapus
          if (tempUser->user.filmRatings[filmTitle] == 0) {
            tempUser = tempUser->next;
            continue;
          }

          totalRating += tempUser->user.filmRatings[filmTitle];
          totalUser++;
        }
        tempUser = tempUser->next;
      }
      break;
    }
    temp = temp->next;
  }

  return (float)totalRating / totalUser;
}

int getMyRating(User *user, string filmTitle) {
  if (user->filmRatings.find(filmTitle) == user->filmRatings.end()) {
    return 0;
  }

  return user->filmRatings[filmTitle];
}

void readFilm(FilmNode *headFilm) {
  FilmNode *temp = headFilm;
  int num = 1;
  while (temp != NULL) {
    cout << "====================" << endl;
    cout << "Nomor: " << num << endl;
    cout << "Judul: " << temp->film.title << endl;
    cout << "Sutradara: " << temp->film.director << endl;
    cout << "Genre: " << temp->film.genre << endl;
    cout << "Tahun: " << temp->film.year << endl;
    cout << "Sinopsis: " << temp->film.synopsis << endl;

    float rating = calculateAvgRating(headFilm, headUser, temp->film.title);
    if (isnan(rating) || rating == 0) {
      cout << "Rating: -" << endl;
    } else {
      cout << "Rating: " << rating << endl;
    }

    // kalo bukan admin, tampilkan ratingnya sendiri
    if (!currentUser->admin) {
      int myRating = getMyRating(currentUser, temp->film.title);
      if (myRating == 0) {
        cout << "Rating saya: -" << endl;
      } else {
        cout << "Rating saya: " << myRating << endl;
      }
    }

    temp = temp->next;
    num++;
  }
}

void swapNodes(FilmNode **headFilm, FilmNode *a, FilmNode *b) {
  if (a == b)
    return;

  FilmNode *prevA = NULL, *prevB = NULL, *curr = *headFilm;
  while (curr) {
    if (curr->next == a)
      prevA = curr;
    if (curr->next == b)
      prevB = curr;
    curr = curr->next;
  }

  if (a == *headFilm)
    *headFilm = b;
  else if (b == *headFilm)
    *headFilm = a;

  if (prevA)
    prevA->next = b;
  if (prevB)
    prevB->next = a;

  FilmNode *temp = a->next;
  a->next = b->next;
  b->next = temp;
}

FilmNode *findNode(FilmNode *headFilm, int idx) {
  FilmNode *temp = headFilm;

  for (int i = 0; i < idx; i++) {
    temp = temp->next;
  }

  return temp;
}

enum SortType { TitleAsc, TitleDesc, AvgRatingAsc, AvgRatingDesc };

bool sortCondition(SortType sortType, FilmNode **headFilm, int j, int gap) {
  switch (sortType) {
  case TitleAsc:
    return findNode(*headFilm, j - gap)->film.title >
           findNode(*headFilm, j)->film.title;
  case TitleDesc:
    return findNode(*headFilm, j - gap)->film.title <
           findNode(*headFilm, j)->film.title;
  case AvgRatingAsc:
    return calculateAvgRating(*headFilm, headUser,
                              findNode(*headFilm, j - gap)->film.title) >
           calculateAvgRating(*headFilm, headUser,
                              findNode(*headFilm, j)->film.title);
  case AvgRatingDesc:
    return calculateAvgRating(*headFilm, headUser,
                              findNode(*headFilm, j - gap)->film.title) <
           calculateAvgRating(*headFilm, headUser,
                              findNode(*headFilm, j)->film.title);
  }

  // harusnya ga akan kesini
  return false;
}

void shellSort(FilmNode **headFilm, int filmCount, SortType sortType) {
  for (int gap = filmCount / 2; gap > 0; gap /= 2) {
    for (int i = gap; i < filmCount; i++) {
      FilmNode *temp = findNode(*headFilm, i);
      int j;
      for (j = i; j >= gap && sortCondition(sortType, headFilm, j, gap);
           j -= gap) {
        swapNodes(headFilm, findNode(*headFilm, j),
                  findNode(*headFilm, j - gap));
      }
    }
  }
}

FilmNode *copyLinkedList(FilmNode *headFilm) {
  FilmNode *newHead = nullptr;
  FilmNode *tail = nullptr;
  FilmNode *current = headFilm;

  while (current != nullptr) {
    FilmNode *newNode = new FilmNode;
    newNode->film = current->film;
    newNode->next = nullptr;

    if (newHead == nullptr) {
      newHead = newNode;
      tail = newNode;
    } else {
      tail->next = newNode;
      tail = newNode;
    }

    current = current->next;
  }

  return newHead;
}

void deleteLinkedList(FilmNode *headFilm) {
  while (headFilm != nullptr) {
    FilmNode *temp = headFilm;
    headFilm = headFilm->next;
    delete temp;
  }
}

// Searching Menggunakan Metode Boyer-Moore

// mendefinisikan metode pencarian menggunakan metode boyer moore
// metode ini mengambil dua argumen, teks dan pattern
// fungsi ini akan mengembalikan pola yang ditemukan jika ditemukan
int boyerMooreSearch(const string &text, const string &pattern) {

  int m = pattern.length(); // variabel m digunakan untuk menyimpan panjang
                            // karakter dari pettern
  int n = text.length();    // variabel n digunakan untuk menyimpan panjang teks
                            // yang akan dicari
  int badChar[256];         // array badChar digunakan untuk menyimpan karakter
                    // index/informasi karakter yaitu menggunakan 256 untuk
                    // mendukung karakter ASCII
  for (int i = 0; i < 256; i++) {
    badChar[i] = -1; // karakter yang tidak ada didalam pola
  }
  for (int i = 0; i < m; i++) { // mengisi array badChar dengan lokasi
    // kemunculan karakter dalam pattern
    badChar[tolower(pattern[i])] =
        i; // untuk menconvert patternya ke lower case
  }
  int s = 0;             // mengatur posisi awal pencarian
  while (s <= (n - m)) { // perulangan dimana s akan selalu melanjut hingga s
                         // <= panjang teks-panjang pattern
    int j = m - 1;       // variabel j digunakan untuk mengatur posisi untuk
                         // mencocokkan pattern dengan karakter di teks
    // Perulangan ini akan terus berulang selama ada karakter yang masih bisa
    // di cocokkan
    while (j >= 0 &&
           tolower(pattern[j]) ==
               tolower(text[s + j])) { // untuk merubah text ke lower case
      j--; // untuk menggeser karakter unutuk dicocokkan dengan pattern
    }
    if (j < 0) {
      return s; // Pattern cocok dengan teks yang ada dan Pattern Ditemukan,
                // fungsi mengembalikan posisi awal pattern ditemukan
    } else {
      // menggeser posisi pencarian dengan memanfaatkan informasi Badchar
      // untuk melompati sejumlah karakter yang tidak memiliki kecocokkan
      // p=dnegan pattern sehingga "mempercepat pencarian"
      s += max(1, j - badChar[tolower(
                          text[s + j])]); // untuk merubah text ke lower case
    }
  }
  return -1; // Fungsi mengembalikan -1 bila Pattern Tidak Ditemukan
}

// fungsi yang menggunakan metode boyer moore yang ada yang dapat mencari
// judul film menggunakan keyword dan fungsi ini akan mengembalikan daftar
// film sesuai dengan kata kunci yang dimasukkan(pattern)
FilmNode *searchFilmByTitle(FilmNode *headFilm, const string &keyword) {
  FilmNode *resultHead = nullptr;
  FilmNode *resultTail = nullptr;

  // temp digunakan sebagai iterator dimana fungsi akan memerikasa apakah
  // judul film cocok dengan keyword/patter yang di inputkan dengan
  // menggunkana booyerMooreSearch
  FilmNode *temp = headFilm;
  while (temp != nullptr) {
    if (boyerMooreSearch(temp->film.title, keyword) != -1) {
      FilmNode *newFilmNode = new FilmNode;
      newFilmNode->film = temp->film;
      newFilmNode->next = nullptr;

      if (resultHead == nullptr) {
        resultHead = newFilmNode;
        resultTail = resultHead;
      } else {
        resultTail->next = newFilmNode;
        resultTail = newFilmNode;
      }
    }

    temp = temp->next;
  }

  return resultHead;
}

bool checkIfEmpty(FilmNode *headFilm) {
  if (headFilm == NULL) {
    printMessage("Belum ada data film.");
    return true;
  }

  return false;
}

// TODO: Stack

void rateFilm(User *user, FilmNode *headFilm, string filmTitle, int rating) {
  FilmNode *temp = headFilm;
  while (temp != NULL) {
    if (temp->film.title == filmTitle) {
      user->filmRatings[filmTitle] = rating;
      return;
    }
    temp = temp->next;
  }
}

/*
  Menu Admin (CRUD). Pada menu ini admin dapat melakukan operasi CRUD
*/
void adminMenu() {
  while (true) {
    clearScreen();
    cout << "=====================" << endl;
    cout << "|   Pilihan Menu    |" << endl;
    cout << "|[0] Keluar         |" << endl;
    cout << "|[1] Tambah Film    |" << endl;
    cout << "|[2] Tampilkan Film |" << endl;
    cout << "|[3] Hapus Film     |" << endl;
    cout << "|[4] Cari Film      |" << endl;
    cout << "|[5] Urutkan Film   |" << endl;
    cout << "|[6] Lihat History  |" << endl;
    cout << "=====================" << endl;

    int choice = inputMenu();
    if (choice == -1) {
      continue;
    }

    if (choice == 0) {
      break;
    }

    // #### TAMBAH FILM ####
    if (choice == 1) {
      clearScreen();

      while (true) {
        readFilm(headFilm);
        cout << endl;
        int position = inputPosition(filmCount);
        if (position == -1) {
          continue;
        } else if (position > filmCount + 1) {
          printMessage("Posisi tidak valid");
          continue;
        }

        Film film = createFilm();
        addFilmSpecific(&headFilm, film, filmCount, position);
        pushHistory("Menambahkan film " + film.title);
        break;
      }

      saveFilmFile(*headFilm);
      printMessage("Berhasil menambahkan film.");
    }
    // #### TAMPILKAN FILM ####
    else if (choice == 2) {
      clearScreen();
      readFilm(headFilm);
      pushHistory("Admin melihat daftar film.");
      printMessage("");
    }
    // #### HAPUS FILM ####
    else if (choice == 3) {
      if (checkIfEmpty(headFilm)) {
        continue;
      }

      readFilm(headFilm);

      while (true) {
        int position = inputPosition(filmCount);
        if (position == -1) {
          continue;
        } else if (position > filmCount) {
          printMessage("Posisi tidak valid");
          continue;
        }

        pushHistory("Admin menghapus film: " +
                    findByPosition(headFilm, position)->title);
        deleteFilmSpecific(&headFilm, filmCount, position);
        break;
      }

      saveFilmFile(*headFilm);
      saveUserRatingFile(headUser);
      printMessage("Berhasil menghapus film.");
    }
    // #### CARI FILM ####
    else if (choice == 4) {
      if (checkIfEmpty(headFilm)) {
        continue;
      }

      string keyword;
      cout << "Masukkan Judul Film Yang Ingin Di Cari : ";
      getline(cin, keyword);

      FilmNode *searchResult = searchFilmByTitle(headFilm, keyword);
      pushHistory("Admin mencari film dengan keyword: " + keyword);

      if (searchResult == nullptr) {
        printMessage("Film Tidak Ditemukan");
      } else {
        cout << "Hasil Pencarian" << endl;
        readFilm(searchResult);
        printMessage("");

        while (searchResult != nullptr) {
          FilmNode *temp = searchResult;
          searchResult = searchResult->next;
          delete temp;
        }
      }
    }
    // #### URUTKAN FILM ####
    else if (choice == 5) {
      if (checkIfEmpty(headFilm)) {
        continue;
      }

      clearScreen();
      cout << "Pilihan Urutan Film:" << endl;
      cout << "[1] Urutkan berdasarkan judul (A-Z)" << endl;
      cout << "[2] Urutkan berdasarkan judul (Z-A)" << endl;
      cout << "[3] Urutkan berdasarkan rata-rata rating (Asc)" << endl;
      cout << "[4] Urutkan berdasarkan rata-rata rating (Desc)" << endl;

      int sortChoice = inputMenu();

      if (sortChoice == 1) {
        shellSort(&headFilm, filmCount, SortType::TitleAsc);
        pushHistory("Admin mengurutkan film berdasarkan judul (A-Z).");
        printMessage("Film telah diurutkan berdasarkan judul (A-Z).");
      } else if (sortChoice == 2) {
        shellSort(&headFilm, filmCount, SortType::TitleDesc);
        pushHistory("Admin mengurutkan film berdasarkan judul (Z-A).");
        printMessage("Film telah diurutkan berdasarkan judul (Z-A).");
      } else if (sortChoice == 3) {
        shellSort(&headFilm, filmCount, SortType::AvgRatingAsc);
        pushHistory(
            "Admin mengurutkan film berdasarkan rata-rata rating (Asc).");
        printMessage(
            "Film telah diurutkan berdasarkan rata-rata rating (Asc).");
      } else if (sortChoice == 4) {
        shellSort(&headFilm, filmCount, SortType::AvgRatingDesc);
        pushHistory(
            "Admin mengurutkan film berdasarkan rata-rata rating (Desc).");
        printMessage(
            "Film telah diurutkan berdasarkan rata-rata rating (Desc).");
      } else {
        printMessage("Pilihan urutan tidak valid.");
      }

    }
    // #### LIHAT HISTORY ####
    else if (choice == 6) {
      clearScreen();
      readHistory();
      printMessage("");
    } else {
      printMessage("Pilihan menu tidak ada");
    }
  }
}

void userMenu() {
  while (true) {
    clearScreen();
    cout << "Selamat datang " << currentUser->username << endl;
    cout << endl;

    cout << "======================" << endl;
    cout << "|    Pilihan Menu    |" << endl;
    cout << "|[0] Keluar          |" << endl;
    cout << "|[1] Lihat Film      |" << endl;
    cout << "|[2] Rate Film       |" << endl;
    cout << "|[3] Hapus Rate Film |" << endl;
    cout << "|[4] Cari Film       |" << endl;
    cout << "======================" << endl;

    int choice = inputMenu();
    if (choice == -1) {
      continue;
    }

    if (choice == 0) {
      break;
    }

    // #### LIHAT FILM ####
    if (choice == 1) {
      if (checkIfEmpty(headFilm)) {
        continue;
      }

      clearScreen();
      cout << "Pilihan Penglihatan Film:" << endl;
      cout << "[1] Lihat tanpa diurutkan" << endl;
      cout << "[2] Lihat dengan diurutkan berdasarkan judul (A-Z)" << endl;
      cout << "[3] Lihat dengan diurutkan berdasarkan judul (Z-A)" << endl;
      cout << "[4] Urutkan berdasarkan rata-rata rating (Asc)" << endl;
      cout << "[5] Urutkan berdasarkan rata-rata rating (Desc)" << endl;

      int readChoice = inputMenu();
      FilmNode *sortedCopy = copyLinkedList(headFilm);

      if (readChoice == 1) {
        clearScreen();
        readFilm(headFilm);
        printMessage("");
      } else if (readChoice == 2) {
        shellSort(&sortedCopy, filmCount, SortType::TitleAsc);
        clearScreen();
        readFilm(sortedCopy);
        printMessage("");
      } else if (readChoice == 3) {
        shellSort(&sortedCopy, filmCount, SortType::TitleDesc);
        clearScreen();
        readFilm(sortedCopy);
        printMessage("");
      } else if (readChoice == 4) {
        shellSort(&sortedCopy, filmCount, SortType::AvgRatingAsc);
        clearScreen();
        readFilm(sortedCopy);
        printMessage("");
      } else if (readChoice == 5) {
        shellSort(&sortedCopy, filmCount, SortType::AvgRatingDesc);
        clearScreen();
        readFilm(sortedCopy);
        printMessage("");
      } else {
        printMessage("Pilihan Lihat tidak valid.");
      }

      deleteLinkedList(sortedCopy);
    }

    // #### RATE FILM ####
    else if (choice == 2) {
      clearScreen();
      if (checkIfEmpty(headFilm)) {
        continue;
      }

      while (true) {
        readFilm(headFilm);

        cout << endl;
        int position = inputPosition(filmCount);
        if (position == -1) {
          continue;
        } else if (position > filmCount) {
          printMessage("Posisi tidak valid");
          continue;
        }

        Film *film = findByPosition(headFilm, position);
        int rating = inputRating(*film);

        rateFilm(currentUser, headFilm, film->title, rating);

        break;
      }

      saveUserRatingFile(headUser);
      printMessage("Berhasil memberikan rating.");
    }
    // #### HAPUS RATING FILM ####
    else if (choice == 3) {
      if (checkIfEmpty(headFilm)) {
        continue;
      }

      while (true) {
        readFilm(headFilm);
        int position = inputPosition(filmCount);
        if (position == -1) {
          continue;
        } else if (position > filmCount) {
          printMessage("Posisi tidak valid");
          continue;
        }

        Film *film = findByPosition(headFilm, position);
        if (getMyRating(currentUser, film->title) == 0) {
          printMessage("Anda belum memberikan rating pada film ini.");
          continue;
        }

        rateFilm(currentUser, headFilm, film->title, 0);
        break;
      }

      saveUserRatingFile(headUser);
      printMessage("Berhasil menghapus rating.");
    }
    // #### CARI FILM ####
    else if (choice == 4) {

      if (checkIfEmpty(headFilm)) {
        continue;
      }

      string keyword;
      cout << "Masukkan Judul Film Yang Ingin Di Cari : ";
      getline(cin, keyword);

      FilmNode *searchResult = searchFilmByTitle(headFilm, keyword);

      if (searchResult == nullptr) {
        printMessage("Film Tidak Ditemukan");
      } else {
        cout << "Hasil Pencarian" << endl;
        readFilm(searchResult);
        printMessage("");

        while (searchResult != nullptr) {
          FilmNode *temp = searchResult;
          searchResult = searchResult->next;
          delete temp;
        }
      }
    } else {
      printMessage("Pilihan menu tidak ada");
    }
  }
}

User *login(UserNode *headUser, string username, string password) {
  UserNode *temp = headUser;
  // traversal dengan mencocokkan username dan password
  while (temp != NULL) {
    if (temp->user.username == username && temp->user.password == password) {
      return &temp->user;
    }
    temp = temp->next;
  }

  return NULL;
}

/*
  Menu Login. Pada menu ini user akan login.
  Tergantung apa yang akan di return dari function login, maka akan diarahkan
  ke menu admin atau user.
*/
void loginMenu() {
  string username, password;
  cout << "Masukkan username: ";
  getline(cin, username);
  cout << "Masukkan password: ";
  getline(cin, password);

  User *user = login(headUser, username, password);
  if (user == NULL) {
    printMessage("Username atau password salah!");
    return;
  }

  currentUser = user;
  if (user->admin) {
    pushHistory("Admin Login");
    adminMenu();
  } else {
    userMenu();
  }
}

/*
  Menu Register. Pada menu ini user dapat melakukan registrasi
*/
void registerMenu() {
  while (true) {
    User newUser = createUser();
    // validasi username, kalau sudah ada, tampilkan pesan error
    if (findByUsername(headUser, newUser.username) != NULL) {
      printMessage("Username sudah digunakan");
      continue;
    }

    registerUser(&headUser, newUser);
    break;
  }

  saveToUserTsv(headUser);
  printMessage("Berhasil mendaftar.");
}

int main() {
  loadFilmFile(&headFilm);
  loadUserFile(&headUser);
  loadRatingFile(&headUser);

  while (true) {
    // reset kembali menjadi NULL
    currentUser = NULL;

    cout << "===== Selamat Datang =====" << endl;
    cout << "[0] Keluar" << endl;
    cout << "[1] Login" << endl;
    cout << "[2] Register" << endl;
    cout << "==========================" << endl;
    int choice = inputMenu();
    if (choice == -1) {
      continue;
    }

    switch (choice) {
    case 0:
      exit(0);
    case 1:
      loginMenu();
      break;
    case 2:
      registerMenu();
      break;
    default:
      printMessage("Pilihan menu tidak ada");
      break;
    }
  }

  return 0;
}
