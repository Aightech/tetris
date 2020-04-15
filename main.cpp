#include <iostream>
#include <random>
#include <unistd.h>
#include <ctime>
struct Piece {
    static const uint8_t nb_pieces;
    static const uint8_t pieces[];
    static const uint8_t sizes[];
    uint8_t wh[2];
    uint16_t xyo[3];
    const uint8_t *shape;

    void
    reset(int i, uint16_t xi) {
        i %= nb_pieces;
        shape = pieces;
        for(int j = 0; j < i; j++)
            shape += (sizes[j * 2] * sizes[j * 2 + 1]);
        for(int j = 0; j < 2; j++) {
            xyo[j + 1] = 0;
            wh[j] = sizes[i * 2 + j];
        }
        xyo[0] = xi - wh[0] / 2;
    }

    uint8_t
    index(uint8_t i) {
        i = (xyo[2] < 2) ? i : wh[0] * wh[1] - i - 1;
        if(xyo[2] % 2)
            return shape[((i + 1) % 2) * wh[1] + i / wh[0]];
        else
            return shape[i];
    };
};

struct Field {
    static constexpr uint8_t fill = '#';
    static constexpr uint8_t empty = ' ';
    static constexpr uint16_t nb_row = 20;
    static constexpr uint16_t nb_col = 10;
    uint8_t arr[nb_col * nb_row] = {};
    struct Piece curr_piece;
    int32_t score = 0;
    bool full = false;

    Field() {
      
      std::srand(std::time(nullptr));
        for(int i = 0; i < nb_row * nb_col; i++)
            arr[i] = 0;
        curr_piece.reset(std::rand(), nb_col/2);
    }

    bool
    move(int16_t dx, int16_t dy) {

        if(curr_piece.xyo[0] + dx >= 0 && curr_piece.xyo[0] + dx < nb_col && curr_piece.xyo[1] + dy >= 0 && curr_piece.xyo[1] + dy < nb_row)
            if(free_space(dx, dy)) {
                curr_piece.xyo[0] = (int8_t)(curr_piece.xyo[0]) + dx;
                curr_piece.xyo[1] = (int8_t)curr_piece.xyo[1] + dy;
                return true;
            }
        return false;
    };

    bool
    turn(int o) {
        if(o) {
            curr_piece.xyo[2] = (curr_piece.xyo[2] + o) % 4;
            uint8_t tmp = curr_piece.wh[1];
            curr_piece.wh[1] = curr_piece.wh[0];
            curr_piece.wh[0] = tmp;

            if(!free_space(0, 0)) {
                curr_piece.xyo[2] = (curr_piece.xyo[2] - o) % 4;
                uint8_t tmp = curr_piece.wh[1];
                curr_piece.wh[1] = curr_piece.wh[0];
                curr_piece.wh[0] = tmp;
            }
        }
        return true;
    }

    int
    insert() {
        for(int i = 0; i < curr_piece.wh[0] * curr_piece.wh[1]; i++)
            arr[curr_piece.xyo[0] + i % curr_piece.wh[0] + (curr_piece.xyo[1] + i / curr_piece.wh[0]) * nb_col] += curr_piece.index(i);

        int k = nb_row - 1;
        bool clear;
        int max = nb_row;
        for(int i = nb_row - 1; i >= 0; i--) {
            clear = true;
            for(int j = 0; j < nb_col; j++) {
                if(arr[i * nb_col + j] == 0)
                    clear = false;
                else
                    max = nb_row - k;
            }
            if(!clear) {
                if(k != i)
                    for(int j = 0; j < nb_col; j++)
                        arr[k * nb_col + j] = arr[i * nb_col + j];
                k--;
            }
            curr_piece.reset(std::rand(), nb_col/2);
        }

        score += k + 1;

        if(max == nb_row) {
            full = true;
            return -1;
        } else
            return k + 1;
    };

    bool
    free_space(int16_t dx, int16_t dy) {
        if(curr_piece.wh[0] + curr_piece.xyo[0] + dx > nb_col || curr_piece.wh[1] + curr_piece.xyo[1] + dy > nb_row)
            return false;
        for(int i = 0; i < curr_piece.wh[0] * curr_piece.wh[1]; i++)
            if(curr_piece.index(i) + arr[curr_piece.xyo[0] + i % curr_piece.wh[0] + dx + (curr_piece.xyo[1] + dy + i / curr_piece.wh[0]) * nb_col] == 2)
                return false;
        return true;
    };

    void
    draw() {
        std::cout << "\t" << score << std::endl;
        ;
        for(int i = 0; i < nb_col; i++)
            std::cout << "#";
        std::cout << std::endl;

        int k = 0;
        for(int i = 0; i < nb_row; i++) {
            for(int j = 0; j < nb_col; j++) {
                if(j >= curr_piece.xyo[0] && j < curr_piece.xyo[0] + curr_piece.wh[0] && i >= curr_piece.xyo[1] && i < curr_piece.xyo[1] + curr_piece.wh[1])
                    std::cout << ((curr_piece.index(k++) + arr[i * nb_col + j]) ? fill : empty);
                else
                    std::cout << ((arr[i * nb_col + j]) ? fill : empty);
            }
            std::cout << std::endl;
        }

        for(int i = 0; i < nb_col; i++)
            std::cout << "#";
        std::cout << std::endl;

        //if(curr_piece)
    };
};

using Field = struct Field;
using Piece = struct Piece;

const uint8_t Piece::pieces[] = {1, 1, 1, 1,
                                 1, 1,
                                 1, 1,
                                 1, 1, 1,
                                 0, 1, 0,
                                 1, 1, 1,
                                 1, 0, 0,
                                 1, 1, 1,
                                 0, 0, 1,
                                 1, 1, 0,
                                 0, 1, 1,
                                 0, 1, 1,
                                 1, 1, 0};

const uint8_t Piece::sizes[] = {4, 1,
                                2, 2,
                                3, 2,
                                3, 2,
                                3, 2,
                                3, 2,
                                3, 2};

const uint8_t Piece::nb_pieces = 7;

int
main() {
    Field f;

    int chance = 0;
    int k = 0;
    while(!f.full) {
        int16_t dx = 1 - 2 * (std::rand() % 2);
        f.move(dx, 0);
        f.turn(std::rand() % 2);
        if(!f.move(0, 1))
            if(++chance > 3)
	      {
                f.insert();
                chance = 0;
            }
        f.draw();
        std::cout << std::endl;
        usleep(10000);
    }

    return 0;
}
