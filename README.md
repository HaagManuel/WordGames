# Word Games

This project is part of the course [Advanced Datastructures](https://www.fib.upc.edu/en/studies/masters/master-innovation-and-research-informatics/curriculum/syllabus/ADS-MIRI) taught at the UPC FIB in Barcelona. 
The task is to implement two string based games.

In the first one we want to design an algorithm to generate all words from a dictionary that can be formed from a multi set of letters.
For example given the letters *abracadabra*, 4 words can be formed: *bad, bar, cab, card*.

The second game is the famous [*Wordle*](https://en.wikipedia.org/wiki/Wordle) game.
We consider arbitrarily word lengths and a larger dictionary than in the original game.

Example: Wordle game #196 solved in 4 guess

![wordle_example](/documents/wordle_196.png)


For more information see the *documents* folder.


### Compiling and Running the Project

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

To run the project use:
```bash
./main
```

To show see all commandline options run:
```bash
./main -h
```


### Some Results
We are using a dictionary with 300K english words consisting only of [a-z]. 
Number of visited nodes and result size for  *Word Challenge* when chossing a random word from the dictionary as input.
![wordle_example](/documents/word_challenge_nodes.jpg)

Average number of guesses to solve *Wordle*.
![wordle_example](/documents/wordle_guesses.jpg)
