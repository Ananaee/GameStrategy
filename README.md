GameStrategy 
=======================


Basic Information
-----------------

Project name: `GameStrategy <https://github.com/Ananaee/GameStrategy`

GameStrategy is a project aimed at developing AI agents for strategy games
that utilize Voronoi diagrams and the Minimax algorithm for tactical
decision-making.



Problem to Solve
----------------

Strategy games often require intelligent decision-making by AI agents to
compete effectively against human players or other AI opponents. One common
challenge in such games is to make optimal decisions on resource allocation,
troop movement, and combat strategy based on the game's spatial layout. By
leveraging Voronoi diagrams, which partition the game map into regions, and
employing the Minimax algorithm, which enables optimal decision-making in
adversarial settings. Here, we will work on 2D games to simplified the project.



Prospective Users
------------------

- **Game Developers** : For integrating advanced AI capabilities into strategy
  games, enhancing gameplay experiences, and creating more challenging
  opponents for players.

- **Hobbyists** : For developing custom AI agents and experimenting with
  different strategies in strategy games, providing an opportunity for
  personal enjoyment and learning in game development and AI.



System Architecture
---------------------

- Using ``C++`` to implement the core algorithm including the Minimax decision
  making logic and Voronoi analysis.
- Using ``python`` for the frontend part: allowing users to interact with the
  system, define game states and make decisions.



API Description
----------------

The system provides an API that allows users to program custom AI agents
using both C++ and Python. Users can interact with the system by writing
scripts that define game states, process decisions, and interface with the
game environment.

Example API usage in C++:


```c++
    // Include necessary headers
    #include "GameStrategy.h"

    // Define game state
    GameState gameState = createGameState(); // Function to create initial
                                             // game state

    // Initialize AI agent
    GameStrategy aiAgent;

    // Generate decision
    Decision decision = aiAgent.makeDecision(gameState);

    // Execute decision in game environment
    executeDecision(decision);
```

Example API usage in Python:


```py
    # Import GameStrategy module
    import GameStrategy

    # Define game state
    game_state = create_game_state()  # Function to create initial game state

    # Initialize AI agent
    ai_agent = GameStrategy()

    # Generate decision
    decision = ai_agent.make_decision(game_state)

    # Execute decision in game environment
    execute_decision(decision)
```


Engineering Infrastructure
---------------------------

- Use ``make`` to build system for compiling the C++ codebase and managing
  dependencies.
- Use ``git`` for version control.
- Use ``pytest`` for testing the performance and correctness.



Schedule
---------

- Week 1 (4/8): Research about Voronoi diagram and analysis.
- Week 2 (4/15): Implement input processing and basic Voronoi analysis.
- Week 3 (4/22):  Develop Minimax algorithm with alpha-beta pruning and test.
- Week 4 (4/29): Integrate Voronoi analysis with Minimax algorithm and test.
- Week 5 (5/6): Implement decision-making logic and finalize API.
- Week 6 (5/13): Performance test and bug fix.
- Week 7 (5/20): Optimize the algorithm.
- Week 8 (5/27): Document the code and prepare for presentation



Reference
----------

- https://medium.com/@jaemuzzin/how-to-use-voronoi-cells-for-strategy-game-maps-1deaee9a4b34

- https://mathworld.wolfram.com/VoronoiDiagram.html