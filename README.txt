Willis A. Hershey
10/14/2020

This command-line program randomly generates passwords.

The program requires the following set of arguments:

-l (Length of password:		the program will expect the following argument to be a valid integer)
-c (Case:			the program will expect the following argument to be one of {lower,upper,mixed})

You may also optionally provide any of the following flags:

-s (Special characters:		the program will interpret this as a request to include special characters in the password)
-n (Numbers:			the program will interpret this as a request to include numeric characters 0-9 in the password)
-w (Write to:			the program will expect the following argument to be a filename openable for writing, and the password will be written there)
-stdio_rand (pseudo random:	the program will interpret this as a command to use the pseudo-random number generator instead of an entropy based one where available)

Because these passwords are (at least pseudo-)randomly generated, there is no guarantee that passwords generated with, for example, "-c mixed" will contain both capital and lowercase letters; there is always a chance that all of the values generated are in one or the other set. This is also true of number and special characters if you request them.

If this program does not produce a password that meets some requirement, and your input is propperly set, then you should just run the program again with the same arguments until it produces something that meets the requirement.
