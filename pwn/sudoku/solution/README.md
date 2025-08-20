# Sudoku
## Recon
We are given an implementation of the game, `Sudoku`, written in C. Our job is to find vulnerabilities in the program. Note that there is no direct way to get a flag in this challenge. So, we must at least get a shell on the remote machine, because we know that the flag is located at /flag.txt.

Here's what we know about the program:

1. An incomplete Sudoku is provided. In each turn, we specify the row and column of the desired cell, along with the number we want to fill in that cell.

2. After every turn, the program runs a system command that logs our attempt in a file at /tmp/logs.txt. It logs our user ID, which is just a random number chosen at the start of the game.

3. The game continues until we have correctly filled the entire grid. Even if we do manage to solve the Sudoku, the program just prints "You win", and exits. Nothing useful.

## Vulnerabilities
1. The main vulnerability that leads us to everything is the serious lack of validation in the program. The row and column we specify must be less than 9, but it doesn't check for negative numbers. Similarly, it doesn't validate the number that we insert into the cell. It can be anything from a negative number to an extremely large positive number.

2. It is very obvious that all of this leads to an out-of-bounds write vulnerability. By putting in negative numbers in the row and column fields, we can potentially overwrite other local variables within the context of the function.

## Exploitation
1. A juicy target for all these vulnerabilities is the command string that is being executed. If we can somehow overwrite the command string, then we can run any arbitrary command on the system, including `sh`, which directly gets us a shell.

2. The command itself can't be overwritten, because it is redefined at the end of every turn. Even if we do manage to overwrite it, it would only execute after it has been re-written by the program. So, an even juicier variable to overwrite is the user ID. This is because the ID is being directly injected into the command. By changing the user ID into a command injection payload, we can execute our own commands on the machine.

3. On debugging the program, we find that the user ID variable is located exactly 32 bytes before the sudoku grid variable. Since negative array indexing in an integer array jumps 4 bytes at a time (because one integer equals four bytes), we somehow need to write to `grid[-8]`. Then, we can inject a payload like `';sh;` or something similar that gives us a shell.

4. We can accomplish this in many different combinations of the row and column inputs, but the simplest one would be to just make the row `0` and the column `-8`. We then set the number to the integer representation of the command injection payload in little-endian format. Note that because this is an integer, we can only overwrite 4 bytes at a time. Since our payload consists of 5 bytes, we will overwrite the first four bytes at the index `-8`, and then overwrite the last byte at the index `-7`.

5. Finally, once the command gets executed, we get a shell on the system, and simply print the flag by running `cat /flag.txt`.