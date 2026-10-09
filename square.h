#ifndef SQUARE_H
#define SQUARE_H

#include <string>

struct Square {
    int rank;
    int file;

    Square(const int& r=-1, const int& f=-1) : rank(r), file(f) {}

    Square(const std::string& tile) : rank(tile[1] - '1'), file(tile[0] - 'a') {}

    bool operator==(const Square& s) const {
        return (s.rank == rank && s.file == file);
    }

    bool operator!=(const Square& s) const {
        return !(*this == s);
    }

    friend std::ostream& operator<<(std::ostream& os, const Square& s) {
        os << "[" << s.rank << "][" << s.file << "]";
        return os;
    }
    
    const std::string translate() const {
        std::string f = "abcdefgh";
        
        return (f[file] + std::to_string(rank + 1));
    }
};

#endif