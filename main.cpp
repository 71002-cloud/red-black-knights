#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <array>

class Board {
public:
    int size;
    int number_of_squares;
    std::vector<int> board;

    static constexpr std::array<std::pair<int, int>, 8> moves = {{
            {2,1},
            {2,-1},
            {-2,1},
            {-2,-1},
            {1,2},
            {1,-2},
            {-1,2},
            {-1,-2}
        }};

    explicit Board(int board_size = 0) : size(board_size) {
        initialize_board();
    }

    void initialize_board() {
        board.clear();
        if (size <= 0) {
            return;
        }

        number_of_squares = size * size;
        board.reserve(number_of_squares);
        for (int square = 0; square < number_of_squares; ++square) {
            board.push_back(-1); // -1 indicates an empty square
        }
    }

    int m(int k) const {
        return (2 * k + 1) * (2 * k + 1) - 1;
    }

    std::pair<int, int> number_to_cords(int n) const {
        if (n == 1) {
            return {0, 1};
        }

        const int k = static_cast<int>((std::sqrt(n) + 1) / 2);
        const int d = m(k) - n;
        int x;
        int y;

        if (0 <= d && d <= 2 * k || d < 0) {
            x = -k;
            y = k - d;
        } else if (2 * k < d && d <= 4 * k) {
            x = -k + (d - 2 * k);
            y = -k;
        } else if (4 * k < d && d <= 6 * k) {
            x = k;
            y = -k + (d - 4 * k);
        } else if (6 * k < d && d <= 8 * k) {
            x = k - (d - 6 * k);
            y = k;
        } else {
            x = 0;
            y = 0;
        }
        return {x, y};
    }

    int cords_to_number(std::pair<int, int> cords) const {
        const int x = cords.first;
        const int y = cords.second;
        int ax = std::abs(x);
        int ay = std::abs(y);
        int k;
        int n;

        if (x < 0 && ax >= ay) {
            k = ax;
            n = -k + m(k) + y;
        } else if (y < 0 && ay >= ax) {
            k = ay;
            n = -k + m(k) - 2 * k - x;
        } else if (ax >= ay) {
            k = ax;
            n = -5 * k + m(k) - y;
        } else {
            k = ay;
            n = m(k) - 7 * k + x;
        }
        return n;
    }

    bool can_place_knight(int square, int color) {
        if (board[square] != -1) {
            return false;
        }

        auto position = number_to_cords(square);

        for (const auto& move : moves) {
            int x = position.first + move.first;
            int y = position.second + move.second;

            int number = cords_to_number({x, y});

            int knight = -1;

            if (number >= 0 && number < number_of_squares) {
                knight = board[number];
            }

            if (knight != -1 && knight != color) {
                return false;
            }
        }

        return true;
    }

    bool add_knight(int square, int color) {
        if (!can_place_knight(square, color)) {
            return false;
        }

        board[square] = color;
        return true;
    }

    void print_knights() {
        const std::vector<std::string> colors = {
            "red", "black", "green", "blue",
            "pink", "yellow", "purple", "orange"};
        std::ofstream output("knights.txt");
        for (int i = 0; i < number_of_squares; ++i) {
            if (board[i] != -1) {
                output << "Square " << std::setw(4) << i << " | " << colors[board[i]] << "\n";
            }
        }
        // Write the output to a file - Is 68 secounds faster than printing when we are using 3001 as size
        //std::cout << output.str();
    }
};

class Game {
public:
    Board board;
    int number_of_knights;
    int turn = 0;
    std::array<std::pair<int, int>, 8> knights_next_moves;

    Game(int number, int board_size)
        : board(board_size), number_of_knights(number) {
        initialize_number_of_knights();
        initialize_next_moves();
    }

    void initialize_next_moves() {
        for (int i = 0; i < number_of_knights; ++i) {
            knights_next_moves[i].first = i; // Assign the color to the first element of the pair
            knights_next_moves[i].second = 0;
        }
    }

    void initialize_number_of_knights() {
        number_of_knights = number_of_knights < 0 ? 0 : (number_of_knights >= 8 ? 8 : number_of_knights);
    }

    void next_turn() {
        ++turn;
        if (turn >= number_of_knights) {
            turn = 0;
        }
    }

    bool try_place_knight() {
        if (number_of_knights <= 0) {
            return false;
        }
        const int color = turn;
        int& square = knights_next_moves[turn].second;

        while (square < board.number_of_squares) {
            if (board.add_knight(square, color)) {
                ++square;
                next_turn();
                return true;
            }
            ++square;
        }
        return false;
    }
};
// Most in this function is for timing
int main() {
    auto start = std::chrono::high_resolution_clock::now();
    // Create the game
    Game game(2, 3001); // Takes (number_of_knights, board_size) board size a grid with max number = size^2 - 1 recomend keeping it to odd numbers for better results
    // Run the game
    while (game.try_place_knight()) {
        // Keep trying to place knights until no more can be placed
    }
    auto end_of_game = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> game_time = end_of_game - start;
    game.board.print_knights(); // Output - It is changed to make a file for faster output

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> print_time = end - end_of_game;
    std::chrono::duration<double> total_time = end - start;

    std::cout << "Time spend on game: " << game_time.count() << " seconds" <<"\n";
    std::cout << "Time spend on printing: " << print_time.count() << " seconds" <<"\n";
    std::cout << "Time spend: " << total_time.count() << " seconds" <<"\n";
    return 0;
}
