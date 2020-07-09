# YAHYA ALHINAI  #5181597 - Assignment 1: textRain

# Dscriptions of design decisions
- In the setup(), the program will take up to 10,000 alphabetical letters and set their X-position, Y-position, time to appear on the screen, and their initial speed.
- The source of textRain is an external file ("TextSource.txt"). The program only reads the alphabetical letters.
- However, the letters of each word are pseudo-random with a margin of 100 pixels between each letter vertically and horizontally. Making the letter of each word being placed randomly within a 100x100 pixels area. This will increase the likelihood of grouping a bunch of letters together to form a word.
- Every letter has randomly assigned initial speed and all letters have acceleration to mimic a free fall.
- Update the position of the textRain as long as it is above the bottom line limit
