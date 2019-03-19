/* --- 100 characters -----------------------------------------------------------------------------
Created by: Laurie 2019/03/19

Sudoku puzzle solver.

Compile: gcc solver.c -o solver -lm -m64 -O3
Usage: ./solver
*/

#include <stdbool.h>
#include <stdio.h>


bool contains(unsigned char val, unsigned char list[9]) {
    for (unsigned char j = 0; j < 9; j += 1) {
        if (list[j] == val) {
            return true;
        }
    }
    return false;
}


void get_row(unsigned char row_j, unsigned char data[81], unsigned char res[9]) {
    for (unsigned char k = 0; k < 9; k += 1) {
        res[k] = data[row_j * 9 + k];
    }
}


void get_column(unsigned char column_k, unsigned char data[81], unsigned char res[9]) {
    for (unsigned char j = 0; j < 9; j += 1) {
        res[j] = data[j * 9 + column_k];
    }
}


void get_box(
        unsigned char box_j,
        unsigned char box_k,
        unsigned char data[81],
        unsigned char res[9]) {
    for (unsigned char j = 0; j < 3; j += 1) {
        for (unsigned char k = 0; k < 3; k += 1) {
            res[j * 3 + k] = data[(box_j * 3 + j) * 9 + box_k * 3 + k];
        }
    }
}


void get_box_for(
        unsigned char row_j,
        unsigned char column_k,
        unsigned char data[81],
        unsigned char res[9]) {
    get_box(row_j / 3, column_k / 3, data, res);
}


void copy_data(unsigned char data[81], unsigned char res[81]) {
    for (unsigned char j = 0; j < 81; j += 1) {
        res[j] = data[j];
    }
}


void print_data(unsigned char data[81]) {
    unsigned char val;
    char data_str[256];
    unsigned char data_str_j = 0;

    for (unsigned char j = 0; j < 9; j += 1) {
        for (unsigned char k = 0; k < 9; k += 1) {
            val = data[j * 9 + k];
            if (val > 0) {
                data_str_j += sprintf(data_str + data_str_j, "%d", val);
            } else {
                data_str_j += sprintf(data_str + data_str_j, " ");
            }
            if ((k == 2) || (k == 5)) {
                data_str_j += sprintf(data_str + data_str_j, "|");
            }
        }
        data_str_j += sprintf(data_str + data_str_j, "\n");
        if ((j == 2) || (j == 5)) {
            data_str_j += sprintf(data_str + data_str_j, "-----------\n");
        }
    }
    puts(data_str);
}


bool solve(unsigned char data[81]) {
    unsigned char new_data[81];
    unsigned char row[9];
    unsigned char column[9];
    unsigned char box[9];

    for (unsigned char j = 0; j < 9; j += 1) {
        for (unsigned char k = 0; k < 9; k += 1) {
            if (data[j * 9 + k] != 0) {
                continue;
            }
            // fprintf(stderr, "Processing cell: %d, %d\n", j, k);
            // print_data(data);
            get_row(j, data, row);
            get_column(k, data, column);
            get_box_for(j, k, data, box);
            for (unsigned char l = 1; l < 10; l += 1) {
                if (contains(l, row) || contains(l, column) || contains(l, box)) {
                    continue;
                }
                copy_data(data, new_data);
                new_data[j * 9 + k] = l;
                if (solve(new_data)) {
                    copy_data(new_data, data);
                    // print_data(data);
                    fflush(stdout);
                    return true;
                }
            }
            // fprintf(stderr, "No available values for cell: %d, %d\n", j, k);
            return false;
        }
    }
    return true;
}


bool parse_puzzle_str(char* puzzle_str, unsigned char res[81]) {
    unsigned char j = 0;
    char val_char;
    unsigned char val;
    char tmp_str[2] = "\0";

    for (unsigned char k = 0; k < 132; k += 1) {
        val_char = puzzle_str[k];
        if (val_char == '|') {
            // fprintf(stderr, "Encountered '|'\n");
            continue;
        }
        if (val_char == '\n') {
            // fprintf(stderr, "Encountered newline\n");
            if (k % 12 != 11) {
                fprintf(stderr, "Found unexpected newline at characher %d in puzzle file\n", k);
                return false;
            }
            continue;
        }
        if (val_char == '-') {
            // fprintf(stderr, "Encountered '-'\n");
            if (!(((k > 35) && (k < 48)) || ((k > 83) && (k < 96)))) {
                fprintf(stderr, "Found unexpected '-' at characher %d in puzzle file\n", k);
                return false;
            }
            continue;
        }
        if (val_char == '.') {
            // fprintf(stderr, "Encountered '.'\n");
            val = 0;
        } else {
            val = val_char - '0';
            if ((val > 9) || (val < 1)) {
                tmp_str[0] = val_char;
                fprintf(
                    stderr,
                    "Found unexpected value '%s' at characher %d in puzzle file\n",
                    tmp_str,
                    k);
                return false;
            }
        }
        if (j > 80) {
            fprintf(stderr, "Too many values in puzzle file\n");
            return false;
        }
        res[j] = val;
        j += 1;
    }
    if (j < 81) {
        fprintf(stderr, "Not enough values (found %d) in puzzle file\n", j);
        return false;
    }
    return true;
}


bool load_puzzle_file(char* puzzle_fname, unsigned char res[81]) {
    FILE* f;
    char puzzle_str[133];

    f = fopen(puzzle_fname, "r");
    fread(puzzle_str, sizeof(char), 133, f);
    if (fclose(f) != 0) {
        fprintf(stderr, "Failed to close puzzle file\n");
        return false;
    }

    // puts(puzzle_str);
    return parse_puzzle_str(puzzle_str, res);
}


int main(int argc, char** argv) {
    unsigned char data[81];
    // unsigned char data[81] = {
    //     5, 0, 8, 0, 6, 0, 0, 7, 1,
    //     2, 0, 0, 0, 0, 0, 6, 9, 0,
    //     0, 0, 0, 2, 7, 4, 0, 0, 0,
    //     6, 1, 3, 0, 0, 5, 8, 0, 0,
    //     0, 2, 0, 7, 0, 1, 0, 6, 0,
    //     0, 0, 5, 6, 0, 0, 1, 4, 9,
    //     0, 0, 0, 1, 3, 7, 0, 8, 0,
    //     0, 6, 9, 0, 0, 2, 0, 0, 5,
    //     8, 7, 0, 0, 5, 0, 4, 0, 2,
    // };

    if (argc != 2) {
        fprintf(stderr, "Usage: %s puzzle\n", argv[0]);
        return 2;
    }

    if (!load_puzzle_file(argv[1], data)) {
        fprintf(stderr, "Failed to load puzzle file\n");
        return 1;
    }

    printf("Initial puzzle:\n");
    print_data(data);

    if (!solve(data)) {
        fprintf(stderr, "Unsolvable puzzle\n");
        return 1;
    }

    printf("Solved puzzle:\n");
    print_data(data);

    return 0;
}
