
# Mado

An implementation of the game of Susan, invented by Stephen Linhart in 1994.


    "SO WHY CALL A BOARD GAME SUSAN?" (an advertising blurb)

    Well, I'll tell you.  One day, me and my friend Michael were having lunch at
    our favorite cafe.  We were playing my new game and I asked him what he thought
    I should call it.  Just then, the waitress came over and Michael asked her
    name.  She said 'Susan', and he turned to me and said 'Susan'.  So I said
    'Ah... Susan'.  Anyway, the name stuck, and it must be a good one because
    everybody asks "So why call a board game SUSAN?"


Right, trademark has been claimed by Stephen Linhart (see below), so I decided to 
do the exact same thing. Me and my wife were having a beer at my favorite bar. I was 
developing this game and asked the waitress what she was called, she said 'Mado', 
we ordered another round and called it a job well done. The rest is history.

I decided to get rid of the Hexagonal representation and go with a board of circles
ordered along a triangular pattern (hexagons in disguise).


Powered by MCTS-AI (TBD)


## How to play


Below a copy of the original (copyrighted) documet published by Stephen Linhart. 
[The original is available at his web-site](http://www.stephen.com/sue/sue_man.txt), there are some more tid-bits [here](http://www.stephen.com/sue/sueabout.html)


    T H E   G A M E   O F   S U S A N (tm)
    ---------------------------------
       An Introduction To Strategy

    Copyright 1994 by Stephen Linhart
       An Enigmatic(tm) Production



    THE FINE PRINT
    --------------
    The board game SUSAN and this manual are protected under international
    copyright conventions and all rights are reserved.  However, you are encouraged
    to copy and distribute this manual freely, as long as you do not modify it in
    any way and you do not charge money for it. SUSAN, Enigma and Enigmatic are
    trademarks of Enigma Games.  All other trademarks are the property of their
    respective holders.



    ABOUT SUSAN
    -----------
    SUSAN is a quick and subtle board game for two players.  It was invented by
    Stephen Linhart (that's me) in 1991.  In addition to the traditional board
    game, SUSAN is also available as a computer game for Macintosh.  You can get
    the Macintosh version on AOL or GEnie, or from the info-mac ftp site at
    sumex-aim.stanford.edu on the Internet.  The Macintosh version of SUSAN
    features an intelligent computer opponent that uses a simulated neural-network
    to actually learn by playing.

    Send comments and questions about SUSAN to:  Stephen123@AOL.com
    Or visit my Web page at:  http://www.stephen.com/sue/sue.html


    "SO WHY CALL A BOARD GAME SUSAN?" (an advertising blurb)

    Well, I'll tell you.  One day, me and my friend Michael were having lunch at
    our favorite cafe.  We were playing my new game and I asked him what he thought
    I should call it.  Just then, the waitress came over and Michael asked her
    name.  She said 'Susan', and he turned to me and said 'Susan'.  So I said
    'Ah... Susan'.  Anyway, the name stuck, and it must be a good one because
    everybody asks "So why call a board game SUSAN?"



    THIS MANUAL
    -----------
    This manual includes an introduction to the game of SUSAN, plus basic and
    intermediate strategy tips.  I created it in ASCII (plain text) so that it can
    travel freely in the electronic world.  Please pass this manual along to your
    friends!

    To see the diagrams correctly you need to use a fixed-width font such as
    Courier or Monaco, not a proportionally spaced font like Times or Helvetica. 
    Most modem software uses a fixed-width font.

    I've tried to give a good, basic introduction to strategy.  But SUSAN is a new
    game, and I am still new at SUSAN.  I'm sure there is much more to learn, and
    it's possible that some of the tips I give in this manual will become outdated.
     If you discover something new and exciting that I haven't discussed in the
    manual, please let me know.



    THE RULES
    ---------
    SUSAN is played on a hexagonal board with 61 spaces, using two sets of playing
    stones.  Here is a diagram of a SUSAN board.  Each star is a space where a
    stone can be placed.  Most spaces have six neighbors.  The edge spaces have
    four neighbors and the corners have three.

        * * * * *
       * * * * * *
      * * * * * * *
     * * * * * * * *
    * * * * * * * * *
     * * * * * * * *
      * * * * * * *
       * * * * * *
        * * * * *

    To begin the game, players choose colors and decide who goes first.  On each
    turn you can place a stone in any empty space, or slide a stone that you
    already have on the board.  When you slide a stone, you move it to any
    neighboring space that's empty.  If both players slide three turns in a row
    (three slides for each player makes six total), the game is a draw.  You can't
    pass your turn.

    The object of the game is to surround any ONE of your opponent's stones.  You
    surround a stone by filling in the spaces around it - a stone can be surrounded
    by any combination of your stones, your opponent's stones and the edge of the
    board.  But be careful; if one of your stones is surrounded on your own turn
    (even if you surround one of your opponent's stones at the same time), you lose
    the game!

    Remember that your stones can be surrounded by your own stones, as well as your
    opponent's stones and the edge of the board.  Having more stones on the board
    is not always better.



    EXAMPLES
    --------
    In these examples (and the examples in other sections) I show black stones as
    'x', white stones as 'o', and empty spaces as dots.  If I want to point out a
    particular stone I use a large 'X' or 'O'.

    T h r e a t e n e d
        . . . . .
       . . . . . .
      . . . . . . .
     . . o o . . . .
    . x . X o . . . .
     . x x o . . . .
      . . . . . . .
       . . . . . .
        . . . . .

    A black stone is surrounded on five sides.  Black needs to make a defensive
    move.  Otherwise White can win on the next turn.

    D e f e n d e d
        . . . ..
       . . . . . .
      . . . . . . .
     . X o o . . . .
    . x . x o . . . .
     . x x o . . . .
      . . . . . . .
       . . . . . .
        . . . . .

    This move guards the space where White would have placed the winning stone. 
    Remember, if one of your stones is surrounded on your own turn, you lose the
    game.

    S t a n d o f f
        . . . . .
       . . . . . .
      . . . . . . .
     . x . o . . . .
    . x O x o . . . .
     . x x o . . . .
      . . . . . . .
       . . . . . .
        . . . . .

    White slides a stone.  In this situation, neither player can make the winning
    move.

    S a f e
        . . . . .
       . . . . . .
      . . . . . . .
     . . . . . . o o
    . . . . . . o X .
     . . . . . . o o
      . . . . . . .
       . . . . . .
        . . . . .
    Because of its position near the corner, this black stone cannot be surrounded.
     White cannot place in the corner without losing the game.



    BASIC STRATEGY
    --------------
    It takes six turns to surround one of your opponent's stones with six of your
    own.  If your opponent has two stones next to each other, you can surround one
    of them in only five turns, like this:

        . . . . .
       . . . . . .
      . . . . . . .
     . . . . . . . .
    . . . o o . . . .
     . . o x o . . .
      . . o x . . .
       . . . . . .
        . . . . .

    The basic principle of attack in SUSAN is to always attack your opponent where
    they're weakest.  Pick the stone you can surround in the fewest turns (the
    enemy stone with the fewest empty neighbors).

    There are three ways to defend a stone in SUSAN.  The first is to finish an
    attack before your opponent does.  If you're close to winning, you don't need
    to defend stones that still have many empty neighbors.

    The second form of defense is to slide the threatened stone to safety.  This is
    a good defensive technique, but stops you from placing a new stone.  If your
    opponent can keep you on the run for several turns, they gain a strategic
    advantage.  If you're winning, your opponent may try to force you to slide in
    order to reach a draw. 

    The third form of defense is to threaten one of the spaces your opponent needs
    for their attack.  If your opponent cannot place one of the six stones they
    need to win, you're safe.  The example titled 'Defended' in the previous
    section is an example of this kind of defense.

    There are six spaces on the board where a stone will be safe even if you don't
    defend it.  Your opponent can never place all six stones needed to surround a
    stone in one of these spaces:

        . . . . .
       . o . . o .
      . . . . . . .
     . . . . . . . .
    . o . . . . . o .
     . . . . . . . .
      . . . . . . .
       . o . . o .
        . . . . .

    The example called 'Safe' in the previous section shows five white stones
    around a black stone in a safe space.  If you are White and you place the sixth
    stone in the corner, you will immediately lose.  Remember, if one of your
    stones is surrounded on your own turn (even if you surround one of your
    opponent's stones at the same time), you lose the game.  The six safe spaces
    are important because a stone in a safe space can be an advantage, but it's
    almost never a disadvantage.  



    PATTERNS
    --------
    The next stage of strategy in SUSAN is to control the pattern of stones that
    develops on the board.  Significant patterns range from simple arcs and
    triangles to complex whole-board patterns.

    The arc pattern is an extension of the same idea that makes spaces near the
    corners safe.  This white stone is safe because Black can't place six stones
    around it without losing.

        . . . . .
       . . . . . .
      . . . . . . .
     . . . . o . . .
    . . . O . o . . .
     . . . . o . . .
      . . . . . . .
       . . . . . .
        . . . . .

    But the three white stones that form the arc aren't safe.  To attack an arc
    formation, attack the stones that form the arc, not the stone they protect.

    Extend the idea of an arc to protect six stones and you get a complete ring.

        . . . . .
       . . . . . .
      . . . . . . .
     . . . o o . . .
    . . . o . o . . .
     . . . o o . . .
      . . . . . . .
       . . . . . .
        . . . . .

    These stones are completely safe unless you break the ring yourself.  A ring
    like this is often an advantage and rarely a liability, but it takes time to
    form a ring.  Six turns could be a fifth of a game.  You rarely want to take
    the time to play it that safe.

    Another important pattern is a triangle in which three stones form the
    potential for three arcs.

        . . . . .
       . . . . . .
      . . . . . . .
     . . . o . . . .
    . . . . . o . . .
     . . . o . . . .
      . . . . . . .
       . . . . . .
        . . . . .

    In this pattern you have a very good chance of protecting any of the three
    stones by forming an arc with the other two.  The ability to protect your
    stones this easily lets you concentrate on offense and keep the initiative.



    WHOLE BOARD PATTERNS
    --------------------
    The idea of using a pattern of stones that protect and reinforce each other can
    be extended to the whole board.  You won't generally finish such a large
    pattern in a serious game, but these patterns do give you ways to plan your
    strategy and to understand board positions.

    One good pattern is to fill the board with triangles based on the safe spaces
    near the corners.

        . . . . .
       . o . . o .
      . . . o . . .
     . . o . . o . .
    . o . . o . . o .
     . . o . . o . .
      . . . o . . .
       . o . . o .
        . . . . .

    Your opponent isn't likely to let you get this far.  But you can still take
    advantage of the pattern, even if you can't finish it.  You can extend the
    pattern with twelve stones around the edges that are completely safe, and don't
    threaten any of the thirteen other stones in the pattern.

        . o . o .
       o o . . o o
      . . . o . . .
     o . o . . o . o
    . o . . o . . o .
     o . o . . o . o
      . . . o . . .
       o o . . o o
        . o . o .

    From here, you can add twelve more stones to reach an even stronger position.

        . o . o .
       o o o o o o
      . o . o . o .
     o o o . . o o o
    . o . . o . . o .
     o o o . . o o o
      . o . o . o .
       o o o o o o
        . o . o .

    Five more stones and a single slide give you the final form of this pattern.

        . o . o .
       o o o o o o
      . o . o . o .
     o o o o o o o o
    . o . o . o . o .
     o o o o o o o o
      . o . o . o .
       o o o o o o
        . o . o .

    You would never see this entire pattern in a game.  But watch for elements of
    it as you play.  It can also form as two solid circles, one inside the other,
    with half-filled circles between them.  This is the early form of the pattern
    as two solid circles.

        . . . . .
       . o o o o .
      . o . . . o .
     . o . o o . o .
    . o . o . o . o .
     . o . o o . o .
      . o . . . o .
       . o o o o .
        . . . . .



    EDGE PLAY
    ---------
    Play around the edges is unique.  You can think of the edge of the board as a
    circle of stones that don't move, and are neither black nor white.  Each corner
    of the board is an arc of three imaginary stones protecting one of the six safe
    spaces.  A stone in a safe space, plus two more on the edge complete the arc to
    form a ring.  This is a very fast way to form a ring, but it doesn't influence
    much of the board.

    This diagram shows three ways to make a partial ring around the edge of the
    board.

        . o . . .
       o o . . . o
      . . . . . o .
     . . . . . . o o
    . . . . . . . . .
     . . . . . . . .
      . . . . . . .
       . o o . . .
        o . o . .

    The three-stone partial on the upper left and the four-stone partial on the
    upper right are compatible.  You can surround the edge of the board with these
    two patterns and have twelve partial rings.

        . o . o .
       o o o o o o
      . o . . . o .
     o o . . . . o o
    . o . . . . . o .
     o o . . . . o o
      . o . . . o .
       o o o o o o
        . o . o .

    This is 30 very safe and stable stones and it's also part of the full-board
    pattern I described in the previous section.  It's unlikely you'd ever form
    this pattern entirely from your own stones.  But it's easy to develop this
    situation with a mixture of stones from both players - a somewhat less stable
    situation, but still quite stable.

    The other four-stone partial creates an incompatible, but also stable,
    situation.

        o . o . o
       . o o o o .
      o o . . . o o
     . o . . . . o .
    o o . . . . . o o
     . o . . . . o .
      o o . . . o o
       . o o o o .
        o . o . o

    Because these two edge patterns are incompatible, conflicts develop where they
    meet.  Either the players slide edge stones until only one pattern remains, or
    else the game becomes focused on the edge conflict.

    Another interesting edge situation to watch for is a partial arc formed by two
    edge locations and a stone.  In this diagram,the indicated stone is protected
    by an edge arc.

        . . . . .
       . . . . . .
      . . . . . O .
     . . . . . . . o
    . . . . . . . . .
     . . . . . . . .
      . . . . . . .
       . . . . . .
        . . . . .

    This is an important technique, because you can use it to protect a threatened
    stone in a single move.



    TRAPS
    -----
    A stone that's enclosed in a two-space trap is often doomed.  If you get into
    this situation without some additional form of protection, you can run but you
    can't escape.  In this example, it's Black's turn.  White will win in three
    moves.

        . . . . .
       . . . . . .
      . . . . . . .
     . . . . . . . .
    . . . . . . . . .
     . . . x x o . .
      . . o . X o .
       . . x o x .
        . . . . .

    One way to take advantage of this technique is to build an empty trap.  That
    leaves you with spaces on the board where you have some flexibility, but your
    opponent doesn't.  In this example, White has constructed an empty trap, while
    Black has formed three partial rings.

        . x . x .
       x x . . x x
      . . . . . . .
     x . . . . . . .
    . x . . . . . . .
     x . . o o o . .
      . . o . . o .
       . . o o o .
        . . . . .

    You can build traps more quickly around the edges.  Edge traps help keep your
    options open, and can also stabilize places where the two edge patterns meet.

        . x . o .
       x x o x x x
      . x . . . x .
     x x . . . . x o
    . x . . . . . o .
     x o . . . . o .
      . x . . . o o
       o o o o o .
        . . o . o

    In this example, two edge traps give White a clear advantage.



    THE END GAME
    ------------
    As the board fills up, it can get hard to find moves that don't put you in
    jeopardy.  You can get in trouble because of a shortage of good moves, or you
    can use it to your advantage.  Sometimes neither player can find a safe move
    except for slides, and the game is a draw.

    If you can create a situation in which you're opponent has no safe moves but
    you do, you can win.  One way to do this is to count down the number of
    remaining moves, and try to control the board so the last safe move will be
    yours.  If both players play very close to a pattern you are familiar with, you
    may be able to count the number of slides that you and your opponent have made
    and know who will win if the pattern isn't disrupted.

    If the board looks like this on White's turn, White can slide the center stone
    and win in one more move.

        . o . x .
       x x x o x x
      . o . x . x .
     o x o x o o x x
    . o . x O . . o .
     o o x x o o x o
      . o . x . o .
       o o x o x x
        . o . x .

    Black needs to avoid this situation, either by sliding instead of placing on
    the previous turn, or by setting up a safe move earlier in the game.

    To have a safe move when the board fills up, you can try to keep more options
    open than your opponent does.  Or you can try to have a place on the board
    where you'll always be able to slide, and there's nothing your opponent can do
    about it.  If you can do that, you can always settle for at least a draw and
    you have a good chance of winning.

    This is a pattern that would guarantee at least a draw, because you can always
    slide the indicated stone.

        o . O . .
       . o o o o o
      o o . o . o .
     . . . . o o o o
    . . . . . . . . .
     . . . . . . . .
      . . . . . . .
       . . . . . .
        . . . . .

    In a real game, you don't have to be so elaborate because your opponent's
    options are limited by the positions of both black and white stones.

    Keep in mind that it is not a simple matter to count the number of turns
    remaining in a game.  The combination of sliding and placing stones makes the
    situation very complex.



    NOTATION
    --------
    This section on notation and the example game may only be of interest to some
    folks.  If you're not particularly interested in this sort of thing, you might
    want to stop here and read these sections later when you're looking for more
    in-depth information about SUSAN.

    To play SUSAN by mail, or to write down and annotate games, you'll need a
    consistent system of notation.  

    Give each row a letter from 'a' thru 'i', starting at the top.  Then number the
    spaces in each row starting at the left.  This gives you a unique letter-number
    combination for each space on the board.

    a     1 2 3 4 5
    b    1 2 3 4 5 6
    c   1 2 3 4 5 6 7
    d  1 2 3 4 5 6 7 8
    e 1 2 3 4 5 6 7 8 9
    f  1 2 3 4 5 6 7 8
    g   1 2 3 4 5 6 7
    h    1 2 3 4 5 6
    i     1 2 3 4 5

    To show the coordinates and the stones in a game of SUSAN, I use a diagram like
    this.

           1 2 3 4 5
        a . . . . . 6
       b . . . . . . 7
      c . . . . . . . 8
     d . . . . . . . . 9
    e . . . . . . . . .
     f . . . . . . . . 9
      g . . . . . . . 8
       h . . . . . . 7
        i . . . . . 6
           1 2 3 4 5

    The letters label rows from the left.  The upper set of numbers labels
    diagonals down to the left.  The lower set of numbers labels diagonals up to
    the left.  Use the upper set of numbers on the upper half of the board and the
    lower set of numbers on the lower half of the board.  The two sets of numbers
    are the same for row 'e'.



    EXAMPLE GAME
    ------------
    Here's a game I played against the computer on the Macintosh version of SUSAN. 
    The computer played Black (actually dark blue) represented here as 'x'.  I
    played White (light red) represented as 'o'.  You can get more out of this game
    if you play along, instead of just reading it.  Black plays first.

     1 B  d6
     2 W  b5
     3 B  c4
     4 W  b2
     5 B  d3
     6 W  e2
     7 B  f3
     8 W  h2
     9 B  g4
    10 W  h5
    11 B  f6
    12 W  e8
    13 B  f2
    14 W  e5

    The computer tries to take control of the middle board.  I take the safe spaces
    and then contest the center.

           1 2 3 4 5
        a . . . . . 6
       b . o . . o . 7
      c . . . x . . . 8
     d . . x . . x . . 9
    e . o . . o . . o .
     f . x x . . x . . 9
      g . . . x . . . 8
       h . o . . o . 7
        i . . . . . 6
           1 2 3 4 5

    15 B  f5
    16 W  d2
    17 B  f4
    18 W  h3
    19 B  d4
    20 W  g2
    21 B  d5
    22 W  g5

    The computer is developing a pattern to make my stone at e5 a liability.  I
    continue to play conservatively (except for the stone at e5) until turn 22 when
    I start to worry about where the computer is heading and decide to attack f5.

           1 2 3 4 5
        a . . . . . 6
       b . o . . o . 7
      c . . . x . . . 8
     d . o x x x x . . 9
    e . o . . o . . o .
     f . x x x x x . . 9
      g . o . x o . . 8
       h . o o . o . 7
        i . . . . . 6
           1 2 3 4 5

    23 B  f6->f7
    24 W  h4
    25 B  f5->f6
    26 W  e6
    27 B  c4->b3
    28 W  g5->g6
    29 B  f3->e4
    30 W  e6->e7

    The computer responds to my attacks on turns 22, 24 and 26 by withdrawing, but
    basically stays within its desired pattern.  I'm succeeding in forcing it to
    waste turns while I build board position.  On turn 28 I try to get back to my
    own pattern, but the computer takes advantage of my less aggressive move to
    attack e5 and gain initiative.

           1 2 3 4 5
        a . . . . . 6
       b . o x . o . 7
      c . . . . . . . 8
     d . o x x x x . . 9
    e . o . x o . o o .
     f . x . x . x x . 9
      g . o . x . o . 8
       h . o o o o . 7
        i . . . . . 6
           1 2 3 4 5

    31 B  c6
    32 W  c4
    33 B  d3->c2
    34 W  d3
    35 B  d5->c5
    36 W  e7->d7
    37 B  f6->f5
    38 W  e5->d5
    39 B  c5->b4

    The computer doesn't press the attack on turn 31, so I return to the offensive.
     The computer is in trouble, and on turn 39 it gives me an opportunity to draw.

           1 2 3 4 5
        a . . . . . 6
       b . o x x o . 7
      c . x . o . x . 8
     d . o o x o x o . 9
    e . o . x . . . o .
     f . x . x x . x . 9
      g . o . x . o . 8
       h . o o o o . 7
        i . . . . . 6
           1 2 3 4 5

    40 W  f3
    41 B  g4->g5
    42 W  h3->g4
    43 B  f5->e6
    44 W  f5
    45 B  d5->e7
    46 W  d5->d6
    47 B  h3
    48 W  g6->f6 *W*

    By turn 45 it's pretty much all over.  The computer's move 47 is just killing
    time until I win on turn 48 by surrounding e7.

           1 2 3 4 5
        a . . . . . 6
       b . o x x o . 7
      c . x . o . x . 8
     d . o o x . o o . 9
    e . o . x . x X o .
     f . x o x o o x . 9
      g . o . o x . . 8
       h . o x o o . 7
        i . . . . . 6
           1 2 3 4 5

    The game never moved out onto the edges, mostly because the computer was
    concentrating on the center.  This isn't a master level game, but it is a good
    illustration of some of the techniques I've described in this manual.



    THE END
    -------
    Once you've mastered all these techniques and you're looking for a deeper, more
    complex game, try playing on a slightly larger 91-space board - six spaces on
    an edge.  The larger board makes for a more involved and intricate game.  SUSAN
    on a 61-space board is a quick and subtle game, and it's a great way to get
    started.

    Thank you for your interest in SUSAN.  Have Fun!

    - Stephen Linhart
