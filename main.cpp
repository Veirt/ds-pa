#include <cmath>
#include <iostream>
#include <limits>
#include <unordered_map>

using namespace std;

struct User {
  string username;
  string password;
  bool admin;
  unordered_map<int, int> filmRatings;
};

struct Film {
  int id; // unique identifier
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
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return menu;
  }
}

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

    if (rating < 0 || rating > 10) {
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      printMessage("Rating harus diantara 0 sampai 10");
      continue;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return rating;
  }
}

Film findByPosition(FilmNode *headFilm, int position) {
  FilmNode *temp = headFilm;
  for (int i = 0; i < position - 1; i++) {
    temp = temp->next;
  }

  return temp->film;
}

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
  user.filmRatings = {};

  return user;
}

Film createFilm(string title = "", string director = "", string genre = "",
                int year = 0, string synopsis = "") {
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

  if (synopsis == "") {
    cout << "Masukkan sinopsis film: ";
    getline(cin, synopsis);
  }

  film.id = filmCount + 1;
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

void deleteFilmRatings(UserNode **headUser, int filmId) {
  UserNode *temp = *headUser;
  while (temp != NULL) {
    if (temp->user.filmRatings.find(filmId) != temp->user.filmRatings.end()) {
      temp->user.filmRatings.erase(filmId);
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
  deleteFilmRatings(&headUser, temp->film.id);
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
  deleteFilmRatings(&headUser, temp->next->film.id);
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
  deleteFilmRatings(&headUser, nextNode->film.id);
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

float calculateAvgRating(FilmNode *headFilm, UserNode *headUser, int filmId) {
  FilmNode *temp = headFilm;
  int totalRating = 0;
  int totalUser = 0;
  while (temp != NULL) {
    if (temp->film.id == filmId) {
      UserNode *tempUser = headUser;
      while (tempUser != NULL) {
        if (tempUser->user.filmRatings.find(filmId) !=
            tempUser->user.filmRatings.end()) {

          // kalo ratingnya 0, berarti belum pernah di rate/udah di hapus
          if (tempUser->user.filmRatings[filmId] == 0) {
            tempUser = tempUser->next;
            continue;
          }

          totalRating += tempUser->user.filmRatings[filmId];
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

int getMyRating(User *user, int filmId) {
  if (user->filmRatings.find(filmId) == user->filmRatings.end()) {
    return 0;
  }

  return user->filmRatings[filmId];
}

void readFilm(FilmNode *headFilm) {
  FilmNode *temp = headFilm;
  while (temp != NULL) {
    cout << "====================" << endl;
    cout << "ID: " << temp->film.id << endl;
    cout << "Judul: " << temp->film.title << endl;
    cout << "Sutradara: " << temp->film.director << endl;
    cout << "Genre: " << temp->film.genre << endl;
    cout << "Tahun: " << temp->film.year << endl;
    cout << "Sinopsis: " << temp->film.synopsis << endl;

    float rating = calculateAvgRating(headFilm, headUser, temp->film.id);
    if (isnan(rating) || rating == 0) {
      cout << "Rating: -" << endl;
    } else {
      cout << "Rating: " << rating << endl;
    }

    // kalo bukan admin, tampilkan ratingnya sendiri
    if (!currentUser->admin) {
      int myRating = getMyRating(currentUser, temp->film.id);
      if (myRating == 0) {
        cout << "Rating saya: -" << endl;
      } else {
        cout << "Rating saya: " << myRating << endl;
      }
    }

    temp = temp->next;
  }
}

// TODO: sorting
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
  while (s <= (n - m)) { // perulangan dimana s akan selalu melanjut hingga s <=
                         // panjang teks-panjang pattern
    int j = m - 1;       // variabel j digunakan untuk mengatur posisi untuk
                         // mencocokkan pattern dengan karakter di teks
    // Perulangan ini akan terus berulang selama ada karakter yang masih bisa di
    // cocokkan
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

// fungsi yang menggunakan metode boyer moore yang ada yang dapat mencari judul
// film
// menggunakan keyword dan fungsi ini akan mengembalikan daftar film sesuai
// dengan kata kunci yang dimasukkan(pattern)
FilmNode *searchFilmByTitle(FilmNode *headFilm, const string &keyword) {
  FilmNode *resultHead = nullptr;
  FilmNode *resultTail = nullptr;

  // temp digunakan sebagai iterator dimana fungsi akan memerikasa apakah judul
  // film cocok dengan keyword/patter yang di inputkan dengan menggunkana
  // booyerMooreSearch
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

// TODO: Stack

void rateFilm(User *user, FilmNode *headFilm, int filmId, int rating) {
  FilmNode *temp = headFilm;
  while (temp != NULL) {
    if (temp->film.id == filmId) {
      user->filmRatings[filmId] = rating;
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
    cout << "=====================" << endl;

    int choice = inputMenu();
    if (choice == -1) {
      continue;
    }

    if (choice == 0) {
      break;
    }

    if (choice == 1) {
      readFilm(headFilm);

      int position;
      cout << "Masukkan posisi film: ";
      cin >> position;
      cin.ignore();
      addFilmSpecific(&headFilm, createFilm(), filmCount, position);
      printMessage("Berhasil menambahkan film.");
    } else if (choice == 2) {
      clearScreen();
      readFilm(headFilm);
      printMessage("");
    } else if (choice == 3) {
      readFilm(headFilm);

      while (true) {
        int position = inputPosition(filmCount);
        if (position == -1) {
          continue;
        } else if (position > filmCount) {
          printMessage("Posisi tidak valid");
          continue;
        }

        deleteFilmSpecific(&headFilm, filmCount, position);
        break;
      }

      printMessage("Berhasil menghapus film.");
    } else if (choice == 4) {
      // todo
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

    } else if (choice == 5) {
      // todo
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

    if (choice == 1) {
      clearScreen();
      readFilm(headFilm);
      printMessage("");
    } else if (choice == 2) {
      clearScreen();
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

        int rating = inputRating(findByPosition(headFilm, position));

        rateFilm(currentUser, headFilm, position, rating);

        break;
      }
      printMessage("Berhasil memberikan rating.");

    } else if (choice == 3) {
      while (true) {
        readFilm(headFilm);
        int position = inputPosition(filmCount);
        if (position == -1) {
          continue;
        } else if (position > filmCount) {
          printMessage("Posisi tidak valid");
          continue;
        }

        rateFilm(currentUser, headFilm, position, 0);
        break;
      }
      printMessage("Berhasil menghapus rating.");
    } else if (choice == 4) {
      // todo

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
    }
  }
}

User *login(UserNode *headUser, string username, string password) {
  UserNode *temp = headUser;
  while (temp != NULL) {
    if (temp->user.username == username && temp->user.password == password) {
      return &temp->user;
    }
    temp = temp->next;
  }

  return NULL;
}

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
    adminMenu();
  } else {
    userMenu();
  }
}

int main() {
  registerUser(&headUser, createUser("admin", "admin", true));
  User *user = registerUser(&headUser, createUser("user", "user", false));
  User *user2 = registerUser(&headUser, createUser("user2", "user2", false));

  addFilmAtLast(
      &headFilm,
      createFilm("Film 1", "Director 1", "Genre 1", 2021, "Synopsis 1"),
      filmCount);
  addFilmAtLast(
      &headFilm,
      createFilm("Film 2", "Director 2", "Genre 2", 2021, "Synopsis 2"),
      filmCount);
  addFilmAtLast(
      &headFilm,
      createFilm("Film 3", "Director 3", "Genre 3", 2021, "Synopsis 3"),
      filmCount);

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
      registerUser(&headUser, createUser());
      break;
    }
  }

  return 0;
}
