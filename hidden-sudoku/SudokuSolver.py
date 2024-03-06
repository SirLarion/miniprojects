""""

31 May 2020

Sudoku Solver

https://github.com/LiorSinai/SudokuSolver-Python

"""

from copy import deepcopy, copy
from typing import List, Tuple, Set
import random
import time
import shutil
import os

from Sudoku import Sudoku, SIZE
from scrape import scrape_puzzle


def grid_equal(A, B):
    """ Check if 2 grids are equal or not"""
    n = len(A)
    if n != len(B):
        return False
    for i in range(n):
        for j in range(n):
            if A[i][j] != B[i][j]:
                return False
    return True

def get_nonempty(A):
    n = len(A)
    m = len(A[0])
    nonempty = []
    for nm in range(n*m):
        i = nm // n
        j = nm % m
        if A[i][j] != 0:
            nonempty.append(nm)
    return nonempty

def flatten(grid):
    arr = []
    for row in grid:
        arr.extend(row)
    return arr

def unflatten(arr: List[int], n=9):
    grid = []
    for i in range(0, len(arr), n):
        grid.append(arr[i:i+n])
    return grid

def arr2str(arr):
    string = ''
    for digit in arr:
        string += str(digit)
    return string

def str2arr(string):
    arr = []
    end = string.find('-')
    end = len(string) if end == -1 else end
    for c in string[0:end]:
        if c == '.':
            arr.append(0)
        else:
            arr.append(int(c))
    return arr  # [int(c) for c in string]

def grid2str(grid: List[List[int]]) -> str:
    return arr2str(flatten(grid))

def str2grid(string: str) -> List[List[int]]:
    return unflatten(str2arr(string))

def print_grid(grid: List[List[int]]) -> None:
        repr = ''
        for row in grid:
            repr += str(row) + '\n'
        print(repr[:-1])

def solveSudokuBrute(grid):
    """
    Only uses backtracking. Very slow, especially on hard puzzles
    """
    def solve(game, depth=0, ij=0):
        nonlocal calls, depth_max
        calls += 1
        depth_max = depth if depth > depth_max else depth_max
        solved = False
        while not solved:
            if ij == 81:
                solved = game.check_done()
                return game.grid, solved
            i = ij // 9
            j = ij % 9
            if game.grid[i][j] == 0:
                # backtracking check point:
                options = game.find_options(i, j)
                if len(options) == 0:
                    return game.grid, False  # this call is going nowhere
                for y in options:
                    game_next = deepcopy(game)
                    game_next.grid[i][j] = y
                    game_next.place_and_erase(i, j, y)
                    grid_final, solved = solve(
                        game_next, ij=ij+1, depth=depth+1)
                    if solved:
                        break
                return grid_final, solved
            ij += 1

        return game.grid, solved

    calls, depth_max = 0, 0
    game = Sudoku(grid)
    grid, solved = solve(game, depth=0, ij=0)

    info = {'calls': calls, 'max depth': depth_max, 'solutions': 1}
    return grid, solved, info


def solveSudoku(grid, t0, num_boxes=SIZE, verbose=True, all_solutions=False, is_X_Sudoku=False):
    """
    idea based on https://dev.to/aspittel/how-i-finally-wrote-a-sudoku-solver-177g
    Try each step until failure, and repeat:
    1) write numbers with only have 1 option
    2) write candidates with only 1 option/ 2 pairs
    3) with multiple options, take a guess and branch (backtrack)
    """
    def solve(game, t0, depth=0, progress_factor=1):
        nonlocal calls, depth_max, progress, progress_update, update_increment
        calls += 1
        depth_max = max(depth, depth_max)
        solved = False
        dt = time.time() - t0
        while not solved and dt < 1:
            solved = True  # assume solved
            edited = False  # if no edits, either done or stuck
            for i in range(game.n):
                for j in range(game.n):
                    if game.grid[i][j] == 0:
                        solved = False
                        options = game.candidates[i][j]
                        if len(options) == 0:
                            progress += progress_factor
                            return False  # this call is going nowhere
                        elif len(options) == 1:  # Step 1
                            game.place_and_erase(
                                i, j, list(options)[0])  # Step 2
                            # game.flush_candidates() # full grid cleaning
                            edited = True
            if not edited:  # changed nothing in this round -> either done or stuck
                if solved:
                    progress += progress_factor
                    solution_set.append(grid2str(game.grid.copy()))
                    return True
                else:
                    # Find the box with the least number of options and take a guess
                    # The place_and_erase() call changes this dynamically
                    min_guesses = (game.n + 1, -1)
                    for i in range(game.n):
                        for j in range(game.n):
                            options = game.candidates[i][j]
                            if len(options) < min_guesses[0] and len(options) > 1:
                                min_guesses = (len(options), (i, j))
                    i, j = min_guesses[1]
                    options = game.candidates[i][j]
                    # backtracking check point:
                    progress_factor *= (1/len(options))
                    for y in options:
                        game_next = deepcopy(game)
                        game_next.place_and_erase(i, j, y)
                        # game_next.flush_candidates() # full grid cleaning
                        solved = solve(
                            game_next, t0, depth=depth+1, progress_factor=progress_factor)
                        if solved and not all_solutions:
                            break  # return 1 solution
                        if verbose and progress > progress_update:
                            print("%.1f" % (progress*100), end='...')
                            progress_update = (
                                (progress//update_increment) + 1) * update_increment
                    return solved
        return solved

    calls, depth_max = 0, 0
    progress, update_increment, progress_update = 0.0, 0.01, 0.01
    solution_set = []

    game = Sudoku(grid, is_X_Sudoku=is_X_Sudoku)
    game.flush_candidates()  # check for obvious candidates

    possible, message = game.check_possible()
    if not possible:
        print('Error on board. %s' % message)
        info = {
            'calls': calls,
            'max depth': depth_max,
            'nsolutions': len(solution_set),
        }
        return solution_set, False, info

    if verbose:
        print("solving: ", end='')
    solve(game, t0, depth=0)
    if verbose:
        print("100.0")
    solved = (len(solution_set) >= 1)

    info = {
        'calls': calls,
        'max depth': depth_max,
        'nsolutions': len(solution_set),
        }
    return solution_set, solved, info

def replace(s, c, i):
    return s[:i] + c + s[i+1:]

def diff(list_1, list_2):
    return list(set(list_1) - set(list_2)) + list(set(list_2) - set(list_1))

def eq(list_1, list_2):
    return len(diff(list_1, list_2)) == 0
  
def read_puzzles(file):
  f = open(file, 'r')
  puzzles = f.read().strip().split('\n')
  
  f.close()
  return puzzles


PUZZLE_LENGTH = 81
MIN_CLUES = 23
puzzle_base = '025000160700405008100060009800070004900000005060000020008000500000209000000030000'
puzzle_empty = '000000000000000000000000000000000000000000000000000000000000000000000000000000000'
hint_indices = [1, 2, 6, 7, 9, 12, 14, 17, 18, 22, 26, 27, 31, 35, 36, 44, 46, 52, 56, 60, 66, 68, 76]

def find_unique(puzzle, mutable=True):

    found_unique = False
    attempts = []

    def mutate():
      possible = False
      mutables = [hint_indices[random.randint(0, 21)] for _ in range(0, 6)]

      while not possible: 
        for m in mutables:
          c = str(random.randint(1, 9))
          puzzle = replace(puzzle, c, m)
        
        game = Sudoku(str2grid(puzzle))
        game.flush_candidates()
        possible, _ = game.check_possible()
        if possible:
          try:
            attempts.index(puzzle)
            print('Already attempted')
            possible = False 
          except:
            attempts.append(puzzle)  


    while not found_unique and mutable:

      grid = str2grid(puzzle)
      ## make multiple solutions
      nonempty = get_nonempty(grid)
      for ij in random.sample(nonempty, k=0): #k=number of values to set to zero
          i = ij // 9
          j = ij % 9
          grid[i][j] = 0

      solution = solution if 'solution' in locals() else None

      verbose       = True
      all_solutions = True

      t0 = time.time()

      print_grid(grid)
      ## solve
      solution_set, done, info = solveSudoku(
          grid, 
          t0,
          verbose=verbose, 
          all_solutions=all_solutions
          )
      print('N:', info['nsolutions']) 

      found_unique = done and info['nsolutions'] == 1

      if not found_unique:
        mutate()
    

    print(str2grid(puzzle))
    print()
    if found_unique:
      print('Has a unique solution')
    else:
      print('Does not have a unique solution')
    print()
    return found_unique

def validate_single(p):
  print('Validating', p)
  unique = find_unique(p)
  if unique and 81 - p.count('0') >= MIN_CLUES:
    compatible = True
    i = 0
    while compatible and i < len(hint_indices):
      index = hint_indices[i]
      compatible = puzzle_base[index] != p[index]
      i += 1

    return compatible 
  
  return False

def validate_set(puzzles):
  print('Validating set')
  external_indices = diff(range(0, 81), hint_indices)
  index_digits = dict()
  for i in external_indices:
    index_digits[i] = list()

  for p in puzzles:
    for i in external_indices: 
      c = int(p[i])
      if c != 0:
        index_digits[i].append(c)
  
  valid = True
  i = 0
  while valid and i < len(external_indices):
    index = external_indices[i]
    arr = index_digits.get(index)
    difference = diff(range(1, 10), arr)
    valid = len(difference) == 0
    if valid:
      print(f'{index} is valid')
    else:
      print(f'{index} missing {len(difference)} digits')

    i += 1
  
  return valid

def try_remove(puzzles, index):
  tmp = puzzles.copy()
  p = tmp[index]
  tmp.remove(p)
  if validate_set(tmp):
    return tmp

  return puzzles
  
    

def get_valid_puzzles():
  folder = 'test/'
  files = ['puzzles_validated.txt']
  existing = []
  validated = []
  
  for f in files:
    existing += read_puzzles(folder + f)

  for p in existing:
    if validate_single(p):
      print('Valid')
      validated.append(p)

  print('Valid existing:', len(validated))
  print()
  print('Validating scraped')

  while not validate_set(validated):
    p = scrape_puzzle()
    if validate_single(p):
      print('Valid')
      validated.append(p)
  
  print('Valid overall:', len(validated))
  print()

  for i in range(len(validated)-1, 0, -1):
    if len(validated) > 69:
      validated = try_remove(validated, i)

  p_file = open('puzzles_validated.txt', 'w')
  p_file.write('\n'.join(validated))
  p_file.close()


def create_puzzle_svgs():
  print('Creating SVGs')
  folder = 'svg/'

  shutil.rmtree(f'{os.getcwd()}/{folder}')
  os.mkdir(f'{os.getcwd()}/{folder}')

  template_file = open('grid_empty.svg', 'r')
  template_svg = template_file.read()
  template_file.close()

  puzzles = read_puzzles('puzzles_validated.txt')
  puzzles.append(puzzle_empty)

  for index, p in enumerate(puzzles):

    svg = template_svg
    fi = svg.find('%')
    svg = replace(svg, f'{index+1}', svg.find('@'))
    file_num = f'{int(index / 10)}{index % 10}'

    i  = 0

    while fi >= 0 and i < PUZZLE_LENGTH:
      c = p[i]
      if c == '0':
        svg = replace(svg, '', fi)
      else:
        svg = replace(svg, c, fi)

      fi = svg.find('%')
      i += 1
    
    svg_file = open(folder + f'puzzle{file_num}.svg', 'w')
    svg_file.write(svg)
    svg_file.close()
  
  print('Created')
  print()


find_unique(puzzle_base)
get_valid_puzzles()
create_puzzle_svgs()

