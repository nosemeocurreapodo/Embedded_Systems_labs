import numpy as np
import random
import matplotlib.pyplot as plt
from matplotlib import colors

class WumpusWorldEnvironment:
    def __init__(self, grid_size=32, num_pits=32):
        # Grid size
        self.grid_size = grid_size
        # Number of pits
        self.num_pits = num_pits
        # Define actions
        self.actions = ['UP', 'DOWN', 'LEFT', 'RIGHT']
        # Initialize environment
        self.reset_environment()
        # Game over flag
        self.done = False

    def reset_environment(self):
        # All possible positions except the starting position
        positions = [(x, y) for x in range(self.grid_size) for y in range(self.grid_size) if (x, y) != (0, 0)]
        random.shuffle(positions)

        # Place pits
        self.pits = positions[:self.num_pits]
        positions = positions[self.num_pits:]

        # Place Wumpus
        self.wumpus = positions[0]
        positions = positions[1:]

        # Place Gold
        self.gold = positions[0]

        # Cells with breeze (adjacent to pits)
        self.breeze_cells = set()
        for pit in self.pits:
            for neighbor in self.get_neighbors(pit):
                self.breeze_cells.add(neighbor)

        # Cells with stench (adjacent to Wumpus)
        self.stench_cells = set()
        for neighbor in self.get_neighbors(self.wumpus):
            self.stench_cells.add(neighbor)

        # Define agent's starting position
        self.agent_start_position = (0, 0)
        self.agent_position = self.agent_start_position

    def reset(self):
        self.agent_position = self.agent_start_position
        self.done = False
        return self.get_state()

    def get_neighbors(self, position):
        x, y = position
        neighbors = []
        for action in self.actions:
            nx, ny = x, y
            if action == 'UP':
                nx -= 1
            elif action == 'DOWN':
                nx += 1
            elif action == 'LEFT':
                ny -= 1
            elif action == 'RIGHT':
                ny += 1
            if 0 <= nx < self.grid_size and 0 <= ny < self.grid_size:
                neighbors.append((nx, ny))
        return neighbors

    def get_state(self):
        x, y = self.agent_position
        # Percepts
        breeze = 1 if self.agent_position in self.breeze_cells else 0
        stench = 1 if self.agent_position in self.stench_cells else 0
        # State includes position and percepts
        return (x, y, breeze, stench)

    def step(self, action):
        if self.done:
            raise Exception("Game is over")

        x, y, _, _ = self.get_state()

        # Move agent
        if action == 'UP':
            x -= 1
        elif action == 'DOWN':
            x += 1
        elif action == 'LEFT':
            y -= 1
        elif action == 'RIGHT':
            y += 1

        # Check for boundaries
        if x < 0 or x >= self.grid_size or y < 0 or y >= self.grid_size:
            # Hit a wall, stay in same position
            reward = -5  # Penalty for invalid move
            self.done = False
            return self.get_state(), reward, self.done

        self.agent_position = (x, y)

        # Check for pit
        if self.agent_position in self.pits:
            reward = -100
            self.done = True
        # Check for Wumpus
        elif self.agent_position == self.wumpus:
            reward = -100
            self.done = True
        # Check for gold
        elif self.agent_position == self.gold:
            reward = 100
            self.done = True
        else:
            reward = -1  # Small penalty for each move
            self.done = False

        return self.get_state(), reward, self.done

    def render(self):
        plt.clf()  # Clear the current figure
        grid_display = np.zeros((self.grid_size, self.grid_size))

        # Set pits
        for pit in self.pits:
            grid_display[pit] = 1  # Pit

        # Set Wumpus
        grid_display[self.wumpus] = 2  # Wumpus

        # Set Gold
        grid_display[self.gold] = 3  # Gold

        # Set Agent
        x, y = self.agent_position
        if self.agent_position == self.gold:
            grid_display[x, y] = 5  # Agent on Gold
        else:
            grid_display[x, y] = 4  # Agent

        # Create a color map
        cmap = colors.ListedColormap(['white', 'black', 'red', 'yellow', 'blue', 'green'])
        bounds = [0, 1, 2, 3, 4, 5, 6]
        norm = colors.BoundaryNorm(bounds, cmap.N)

        # Plot the grid
        plt.imshow(grid_display, cmap=cmap, norm=norm)

        # Draw grid lines
        plt.grid(which='major', axis='both', linestyle='-', color='k', linewidth=2)
        plt.xticks(np.arange(-0.5, self.grid_size, 1), [])
        plt.yticks(np.arange(-0.5, self.grid_size, 1), [])

        plt.draw()
        plt.pause(0.5)  # Pause to update the figure

class QLearningAgent:
    def __init__(self, environment, alpha=0.5, gamma=0.9, epsilon=1.0, epsilon_decay=0.995, min_epsilon=0.01):
        self.env = environment
        self.alpha = alpha  # Learning rate
        self.gamma = gamma  # Discount factor
        self.epsilon = epsilon  # Exploration rate
        self.epsilon_decay = epsilon_decay
        self.min_epsilon = min_epsilon

        # Initialize Q-table with zeros
        self.q_table = {}

    def get_q_values(self, state):
        # Initialize Q-values for new states
        if state not in self.q_table:
            self.q_table[state] = {action: 0.0 for action in self.env.actions}
        return self.q_table[state]

    def choose_action(self, state):
        # Epsilon-greedy action selection
        if random.uniform(0, 1) < self.epsilon:
            # Explore: select a random action
            action = random.choice(self.env.actions)
        else:
            # Exploit: select the action with max Q-value
            q_values = self.get_q_values(state)
            max_q = max(q_values.values())
            max_actions = [action for action, q in q_values.items() if q == max_q]
            action = random.choice(max_actions)
        return action

    def learn(self, state, action, reward, next_state, done):
        q_values = self.get_q_values(state)
        q_predict = q_values[action]
        if done:
            q_target = reward  # No future state
        else:
            next_q_values = self.get_q_values(next_state)
            q_target = reward + self.gamma * max(next_q_values.values())
        # Update Q-value
        q_values[action] += self.alpha * (q_target - q_predict)

    def update_epsilon(self):
        # Decay epsilon
        if self.epsilon > self.min_epsilon:
            self.epsilon *= self.epsilon_decay

if __name__ == "__main__":
    env = WumpusWorldEnvironment()
    agent = QLearningAgent(env, alpha=0.5, gamma=0.9, epsilon=1.0, epsilon_decay=0.995, min_epsilon=0.01)

    num_episodes = 1000

    for episode in range(num_episodes):
        state = env.reset()
        done = False

        while not done:
            action = agent.choose_action(state)
            next_state, reward, done = env.step(action)
            agent.learn(state, action, reward, next_state, done)
            state = next_state

        agent.update_epsilon()  # Decay epsilon after each episode

        if (episode + 1) % 100 == 0:
            print(f"Episode {episode + 1} completed - Epsilon: {agent.epsilon:.4f}")

    # Set epsilon to 0 to exploit learned policy
    agent.epsilon = 0.0
    state = env.reset()
    done = False
    total_reward = 0
    steps = 0

    print("\nTesting trained agent...")
    plt.ion()  # Turn on interactive plotting
    env.render()  # Initial render

    while not done and steps < 50:
        action = agent.choose_action(state)
        next_state, reward, done = env.step(action)
        print(f"State: {state}, Action: {action}, Reward: {reward}, Next State: {next_state}")
        total_reward += reward
        state = next_state
        steps += 1
        env.render()  # Render after each step

    plt.ioff()  # Turn off interactive plotting
    print(f"Total Reward: {total_reward}")
    plt.show()  # Keep the final plot open
