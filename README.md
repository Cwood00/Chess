# Chess App

Simple chess app with text based user interface, with the board presented in a 8x8 grid.
Pieces are represented by the corresponding letters (p)awn, (r)ook, k(n)ight
(b)ishop, (k)ing, (q)ueen. Uppercase letters represent white pieces,
lowercase letters represent black pieces, and _ represents an empty square.

App works with both multiplayer, and single player vs computer.  
Computer player uses min-max algorithm taht searches the game tree to a depth of 4.

## How to play

First select number of human players, either 1 or 2
If playing vs computers select if human player plays as white or black
Enter moves in the form of abcd, where ab represents the column and the row
of the piece being moved, and cd represents the column and the row of the
destination square, respectively. a and c should be letters a-h, well b and
d should be numbers between 1-8. e.g. e2e4. To castle move the king two
squares in the direction of the rook. To promote a pawn add the letter of
the piece being promoted to at the end of the move. e.g. a7a8Q  

Other commands  
(r)esign to resign  
(d)raw to offer a draw  
(h)elp to display help message  
