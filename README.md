Deterministic Finite Automaton Minimize

This application initially takes as input a .txt file containing the description of a Deterministic Finite Automaton (DFA) in the following format:

alphabet: a, b, c, d # List of symbols in the alphabet accepted by the automaton
states: q0, q1, q2 # List of states in the automaton
initial: q0 # Indicates the initial state
finals: q1, q2 # Specifies the final states of the automaton.
transitions
q0, q1, a # Represents a transition from q0 to q1 with symbol "a"
q1, q2, b # Represents a transition from q1 to q2 with symbol "b"
...

Next, the application should use the Myhill-Nerode algorithm (or Table Filling Method) to minimize the given DFA. The algorithm should display the step-by-step execution on the screen and, in the end, use a library or software to display the diagram of the resulting minimized DFA.

Important: It is necessary to verify if the DFA provided as input is valid before using the conversion algorithm.
