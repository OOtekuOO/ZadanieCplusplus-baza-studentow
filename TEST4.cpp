#include <iostream>
#include <vector>
#include "sqlite3.h"
#include <algorithm>
#include <sstream>

class BazaDanych {
public:
    BazaDanych(const std::string& nazwaBazy) {
        if (otworzBazeDanych(nazwaBazy) != SQLITE_OK) {
            throw std::runtime_error("Nie można otworzyć bazy danych.");
        }
    }

    ~BazaDanych() {
        zamknijBazeDanych();
    }

    void dodajUcznia(const std::string& imie, const std::string& nazwisko, double sredniaRokZeszly, const std::string& klasa) {
        std::string sql = "INSERT INTO uczniowie (imie, nazwisko, srednia_rok_zeszly, klasa) VALUES ('" +
            imie + "', '" + nazwisko + "', " +
            na_string(sredniaRokZeszly) + ", '" + klasa + "');";
        wykonajZapytanieSQL(sql);
    }

    void usunUcznia(const std::string& nazwisko) {
        std::string sql = "DELETE FROM uczniowie WHERE nazwisko = '" + nazwisko + "';";
        wykonajZapytanieSQL(sql);
    }

    void edytujUcznia(const std::string& nazwisko, double nowaSrednia, const std::string& nowaKlasa) {
        std::string sql = "UPDATE uczniowie SET srednia_rok_zeszly = " + na_string(nowaSrednia) +
            ", klasa = '" + nowaKlasa + "' WHERE nazwisko = '" + nazwisko + "';";
        wykonajZapytanieSQL(sql);
    }

    void wyszukajUczniowPoNazwisku(const std::string& nazwisko) {
        std::string sql = "SELECT * FROM uczniowie WHERE nazwisko = '" + nazwisko + "';";
        wyswietlUczniow(sql);
    }

private:
    sqlite3* db;

    int otworzBazeDanych(const std::string& nazwaBazy) {
        return sqlite3_open(nazwaBazy.c_str(), &db);
    }

    void zamknijBazeDanych() {
        sqlite3_close(db);
    }

    int wykonajZapytanieSQL(const std::string& sql) {
        char* komunikatBledu = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &komunikatBledu);
        if (rc != SQLITE_OK) {
            std::cerr << "Błąd wykonania zapytania SQL: " << komunikatBledu << std::endl;
            sqlite3_free(komunikatBledu);
        }
        return rc;
    }

    std::string na_string(double wartosc) {
        std::ostringstream oss;
        oss << wartosc;
        return oss.str();
    }

    void wyswietlUczniow(const std::string& sql) {
        int rc = wykonajZapytanieSQL(sql);
        if (rc == SQLITE_OK) {
            sqlite3_stmt* stmt;
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    int id = sqlite3_column_int(stmt, 0);
                    const unsigned char* imie = sqlite3_column_text(stmt, 1);
                    const unsigned char* nazwisko = sqlite3_column_text(stmt, 2);
                    double sredniaRokZeszly = sqlite3_column_double(stmt, 3);
                    const unsigned char* klasa = sqlite3_column_text(stmt, 4);

                    std::cout << "ID: " << id << ", Imie: " << imie << ", Nazwisko: " << nazwisko
                        << ", Srednia rok zeszly: " << sredniaRokZeszly << ", Klasa: " << klasa << std::endl;
                }
                sqlite3_finalize(stmt);
            }
            else {
                std::cerr << "Błąd przygotowywania zapytania: " << sqlite3_errmsg(db) << std::endl;
            }
        }
        else {
            std::cerr << "Błąd wykonania zapytania SQL: " << sqlite3_errmsg(db) << std::endl;
        }
    }
};

void wyswietlMenu() {
    std::cout << "=========================" << std::endl;
    std::cout << "1. Dodaj ucznia" << std::endl;
    std::cout << "2. Usun ucznia" << std::endl;
    std::cout << "3. Edytuj ucznia" << std::endl;
    std::cout << "4. Wyszukaj ucznia" << std::endl;
    std::cout << "5. Zakoncz program" << std::endl;
    std::cout << "Wybierz opcje: ";
}

int main() {
    BazaDanych baza("baza_uczniow.db");

    int wybor;
    while (true) {
        wyswietlMenu();
        std::cin >> wybor;

        switch (wybor) {
        case 1: {
            std::string imie, nazwisko, klasa;
            double sredniaRokZeszly;
            std::cout << "Podaj imie ucznia: ";
            std::cin >> imie;
            std::cout << "Podaj nazwisko ucznia: ";
            std::cin >> nazwisko;
            std::cout << "Podaj srednia rok zeszly ucznia: ";
            std::cin >> sredniaRokZeszly;
            std::cout << "Podaj klase ucznia: ";
            std::cin >> klasa;
            baza.dodajUcznia(imie, nazwisko, sredniaRokZeszly, klasa);
            break;
        }
        case 2: {
            std::string nazwisko;
            std::cout << "Podaj nazwisko ucznia do usuniecia: ";
            std::cin >> nazwisko;
            baza.usunUcznia(nazwisko);
            break;
        }
        case 3: {
            std::string nazwisko, nowaKlasa;
            double nowaSrednia;
            std::cout << "Podaj nazwisko ucznia do edycji: ";
            std::cin >> nazwisko;
            std::cout << "Podaj nowa srednia rok zeszly: ";
            std::cin >> nowaSrednia;
            std::cout << "Podaj nowa klase ucznia: ";
            std::cin >> nowaKlasa;
            baza.edytujUcznia(nazwisko, nowaSrednia, nowaKlasa);
            break;
        }
        case 4: {
            std::string nazwisko;
            std::cout << "Podaj nazwisko ucznia do wyszukania: ";
            std::cin >> nazwisko;
            baza.wyszukajUczniowPoNazwisku(nazwisko);
            break;
        }
        case 5:
            return 0;
        default:
            std::cout << "Nieprawidlowy wybor. Sprobuj ponownie." << std::endl;
        }
    }

    return 0;
}
