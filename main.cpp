#include <cmath>
#include <iostream>
#include <string>
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
    // The eight possible moves of a knight in chess.
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
        // Calculate the total number of squares on the board.
        number_of_squares = size * size;
        board.reserve(number_of_squares);
        board.assign(number_of_squares, -1); // -1 indicates that a square is empty.
    }

    // Return the largest number in spiral ring k.
    int m(int k) const {
        return (2 * k + 1) * (2 * k + 1) - 1;
    }

    // Convert a spiral square number into x and y coordinates.
    std::pair<int, int> number_to_cords(int n) const {
        if (n == 1) {
            return {0, 1};
        }
        // Calculate which spiral ring the square belongs to (k).
        const int k = static_cast<int>((std::sqrt(n) + 1) / 2);
        // Calculate how many squares n is from the end of the ring (d).
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

    // Convert x and y coordinates back into a spiral square number.
    int cords_to_number(std::pair<int, int> cords) const {
        const int x = cords.first;
        const int y = cords.second;
        const int ax = std::abs(x);
        const int ay = std::abs(y);
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

    bool can_place_knight(int square, int color) const {
        if (board[square] != -1) {
            return false;
        }

        const auto position = number_to_cords(square);

        for (const auto& move : moves) {
            const int x = position.first + move.first;
            const int y = position.second + move.second;

            const int number = cords_to_number({x, y});

            int knight = -1;

            if (number >= 0 && number < number_of_squares) {
                knight = board[number];
            }

            // A knight may be next to one of the same color, but not one of a different color.
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
        const std::array<std::string, 8> colors = {
            "red", "black", "green", "blue",
            "pink", "yellow", "purple", "orange"};
        std::ofstream output("knights.txt");
        if (!output) {
            std::cerr << "Error opening file for writing.\n";
            return;
        }
        for (int i = 0; i < number_of_squares; ++i) {
            if (board[i] != -1) {
                output << "Square " << std::setw(4) << i << " | " << colors[board[i]] << "\n";
            }
        }
        output.close();
        // Writing to a file is about 68 seconds faster than printing to the terminal when using a board size of 3001.
        //std::cout << output.str();
    }
};

class Game {
public:
    Board board;
    int number_of_knights;
    int turn = 0;
    // Store the next square to check for each knight. There can be at most eight knights.
    std::array<int, 8> knights_next_moves;

    Game(int number, int board_size)
        : board(board_size), number_of_knights(number) {
        initialize_number_of_knights();
        initialize_next_moves();
    }

    void initialize_next_moves() {
        for (int i = 0; i < number_of_knights; ++i) {
            // Start each knight at its own color index. Earlier squares have
            // already been checked, so they do not need to be checked again.
            knights_next_moves[i] = i;
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
        int& square = knights_next_moves[turn];

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
// Most of the code in this function is used for timing the game and file output.
int main() {
    const auto start = std::chrono::high_resolution_clock::now();
    // Arguments: (number_of_knights, board_size). The board size is the length
    // of one side, and odd sizes are recommended to match the spiral structure.
    // 46339 is the largest size before the int calculations can overflow;
    Game game(2, 3001); 
    // Try to place knights until no more valid placements are possible.
    while (game.try_place_knight()) {
    }
    const auto end_of_game = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> game_time = end_of_game - start;
    game.board.print_knights();

    const auto end = std::chrono::high_resolution_clock::now();

    const std::chrono::duration<double> print_time = end - end_of_game;
    const std::chrono::duration<double> total_time = end - start;
    // Report the time spent on the game and on writing the output.
    // These measurements are used for testing and optimization.
    std::cout << "Time spend on game: " << game_time.count() << " seconds" <<"\n";
    std::cout << "Time spend on printing: " << print_time.count() << " seconds" <<"\n";
    std::cout << "Time spend: " << total_time.count() << " seconds" <<"\n";
    return 0;
}
