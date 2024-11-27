import numpy as np
import matplotlib.pyplot as plt
from collections import deque
import random

def generate_random_map(width, height, hole_prob=0.2):
    """Generates a random frozen lake map."""
    grid = np.full((height, width), 'F', dtype=str)
    # Randomly select start and goal positions
    all_positions = [(r, c) for r in range(height) for c in range(width)]
    start_cell = random.choice(all_positions)
    all_positions.remove(start_cell)
    goal_cell = random.choice(all_positions)
    # Place 'S' and 'G' on the grid
    grid[start_cell] = 'S'
    grid[goal_cell] = 'G'
    # Now fill the rest with 'F' or 'H' (excluding 'S' and 'G')
    for r in range(height):
        for c in range(width):
            if (r, c) != start_cell and (r, c) != goal_cell:
                grid[r, c] = 'H' if random.random() < hole_prob else 'F'
    return grid

def to_s(row, col, ncol):
    """Converts row, col to state number."""
    return row * ncol + col

def to_rc(s, ncol):
    """Converts state number to row, col."""
    return s // ncol, s % ncol

def get_neighbors(s, desc):
    """Gets neighboring states and the actions leading to them."""
    neighbors = []
    nrow, ncol = desc.shape
    r, c = to_rc(s, ncol)
    if desc[r, c] in 'GH':
        return neighbors  # No neighbors if it's a goal or hole
    directions = [(-1, 0, 3), (1, 0, 1), (0, -1, 0), (0, 1, 2)]  # (dr, dc, action)
    for dr, dc, action in directions:
        nr, nc = r + dr, c + dc
        if 0 <= nr < nrow and 0 <= nc < ncol:
            if desc[nr, nc] != 'H':  # Avoid holes
                ns = to_s(nr, nc, ncol)
                neighbors.append((ns, action))
    return neighbors

def bfs(start_state, goal_states, desc):
    """Performs BFS to find the shortest path from start to goal."""
    frontier = deque()
    frontier.append((start_state, []))
    explored = set()
    ncol = desc.shape[1]
    while frontier:
        state, path = frontier.popleft()
        if state in explored:
            continue
        explored.add(state)
        if state in goal_states:
            return path
        for ns, action in get_neighbors(state, desc):
            if ns not in explored:
                frontier.append((ns, path + [action]))
    return None

def plot_solution(desc, path, start_state):
    """Plots the grid and overlays the solution path."""
    nrow, ncol = desc.shape
    fig, ax = plt.subplots()
    ax.set_aspect('equal')
    # Draw the grid
    for r in range(nrow):
        for c in range(ncol):
            cell = desc[r, c]
            color = None
            if cell == 'S':
                color = 'green'
            elif cell == 'G':
                color = 'red'
            elif cell == 'F':
                color = 'skyblue'
            elif cell == 'H':
                color = 'black'
            rect = plt.Rectangle([c, nrow - 1 - r], 1, 1, facecolor=color, edgecolor='black')
            ax.add_patch(rect)
    # Overlay the path
    positions = [to_rc(start_state, ncol)]
    state = start_state
    for action in path:
        r, c = to_rc(state, ncol)
        if action == 0:  # LEFT
            nc = c - 1
            nr = r
        elif action == 1:  # DOWN
            nr = r + 1
            nc = c
        elif action == 2:  # RIGHT
            nc = c + 1
            nr = r
        elif action == 3:  # UP
            nr = r - 1
            nc = c
        state = to_s(nr, nc, ncol)
        positions.append((nr, nc))
    # Plot the path
    x = [c + 0.5 for r, c in positions]
    y = [nrow - 1 - r + 0.5 for r, c in positions]
    ax.plot(x, y, 'yellow', linewidth=2, markersize=12)
    ax.plot(x, y, 'o', color='blue', markersize=8)
    ax.set_xlim(0, ncol)
    ax.set_ylim(0, nrow)
    ax.set_xticks(np.arange(ncol + 1))
    ax.set_yticks(np.arange(nrow + 1))
    ax.invert_yaxis()
    plt.grid(True)
    plt.show()

def main():
    width = 50
    height = 50
    hole_prob = 0.2
    max_attempts = 10

    for attempt in range(max_attempts):
        desc = generate_random_map(width, height, hole_prob)
        nrow, ncol = desc.shape
        start_state = None
        goal_states = []
        for r in range(nrow):
            for c in range(ncol):
                cell = desc[r, c]
                s = to_s(r, c, ncol)
                if cell == 'S':
                    start_state = s
                elif cell == 'G':
                    goal_states.append(s)
        if start_state is None or not goal_states:
            print("Start or Goal state not defined.")
            continue  # Try again
        path = bfs(start_state, goal_states, desc)
        if path is not None:
            print(f"A solvable map was found after {attempt+1} attempts.")
            print("Found a path:", path)
            print("Actions: Left(0), Down(1), Right(2), Up(3)")
            print("Number of steps:", len(path))
            # Plot the grid and the solution path
            plot_solution(desc, path, start_state)
            break
        else:
            print(f"No path found in attempt {attempt+1}, generating a new map.")
    else:
        print("Failed to generate a solvable map after maximum attempts.")

if __name__ == "__main__":
    main()
