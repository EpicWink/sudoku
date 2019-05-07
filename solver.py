# --- 100 characters -------------------------------------------------------------------------------
# Created by: Laurie 2019/03/19

"""Sudoku puzzle solver."""

import sys
import time
import pathlib
import argparse
import logging as lg

import numpy as np

lg.basicConfig(
    level=lg.INFO,
    datefmt="%H:%M:%S",
    format="%(asctime)s [%(levelname)8s] %(name)s: %(message)s")
_logger = lg.getLogger(__name__)
sys.setrecursionlimit(10000)


class Puzzle:
    _horz_sep = "---+---+---"
    _vert_sep = "|"

    def __init__(self, data):
        self.data = data
        assert data.shape == (9, 9)
        assert data.dtype == np.uint8
        assert np.all(data < 10), list(zip(np.argwhere(data < 10), data[data < 10]))

    @classmethod
    def from_str(cls, data_str):
        data = []
        for j, line in enumerate(data_str.strip().splitlines()):
            if not line:
                continue
            row = []
            for k, char in enumerate(line):
                if char == " ":
                    continue
                val = 0 if char == "." else int(char)
                assert 0 <= val < 10, ((j, k), val)
                row.append(val)
            assert len(row) == 9, (j, len(row))
            data.append(row)
        assert len(data) == 9, len(data)
        return cls(np.array(data, dtype=np.uint8))

    def format_puzzle(self):
        line_fmt = self._vert_sep.join(["{}{}{}"] * 3)
        lines = []
        for j, row in enumerate(self.data):
            chars = ["."] * 9
            for k, val in enumerate(row):
                if val > 0:
                    chars[k] = str(val)
            line = self._vert_sep.join("".join(p) for p in [chars[:3], chars[3:6], chars[6:]])
            if j in (3, 6):
                lines.append(self._horz_sep)
            lines.append(line)
        return "\n".join(lines)

    def get_box_for(self, row_j, column_k):
        box_j = row_j // 3
        box_k = column_k // 3
        return self.data[box_j * 3:(box_j + 1) * 3, box_k * 3:(box_k + 1) * 3]

    @staticmethod
    def _is_valid(values):
        filled = values[values != 0]
        return len(set(filled)) == len(filled)

    def _is_valid_box(self, box_j, box_k):
        box = self.data[box_j * 3:(box_j + 1) * 3, box_k * 3:(box_k + 1) * 3]
        return self._is_valid(np.ravel(box))

    def _is_valid_row(self, row_j):
        row = self.data[row_j]
        return self._is_valid(row)

    def _is_valid_column(self, column_k):
        column = self.data[:, column_k]
        return self._is_valid(column)

    def is_valid(self):
        box = [self._is_valid_box(j, k) for j in range(3) for k in range(3)]
        row = [self._is_valid_row(j) for j in range(9)]
        column = [self._is_valid_column(k) for k in range(9)]
        return all(box) and all(row) and all(column)

    def _solve(self):
        for j in range(9):
            for k in range(9):
                if self.data[j, k] != 0:
                    continue
                for l in range(1, 10):
                    if l in self.data[j] or l in self.data[:, k] or l in self.get_box_for(j, k):
                        continue
                    data = self.data.copy()
                    data[j, k] = l
                    p = type(self)(data)
                    if p._solve():
                        self.data = p.data
                        return True
                return False
        return True  # all values are filled

    def solve(self):
        t = time.time()
        if not self._solve():
            raise ValueError("Unsolvable puzzle")
        _logger.info("Solving completed in %.3f seconds", time.time() - t)


def run_app(puzzle_path):
    data_str = puzzle_path.read_text()
    puzzle = Puzzle.from_str(data_str)
    _logger.info("Initial puzzle:\n%s", puzzle.format_puzzle().replace(".", " ").replace("", " "))
    _logger.info("Initial puzzle is valid: %s", puzzle.is_valid())
    puzzle.solve()
    _logger.debug("Solved puzzle:\n%s", puzzle.format_puzzle().replace(".", " ").replace("", " "))
    _logger.debug("Solved puzzle is valid: %s", puzzle.is_valid())
    print(puzzle.format_puzzle())


def main():
    parser = argparse.ArgumentParser(description="Solve Sudoku puzzles.")
    parser.add_argument("puzzle", type=pathlib.Path, help="puzzle file")
    args = parser.parse_args()

    run_app(args.puzzle)


if __name__ == "__main__":
    main()
