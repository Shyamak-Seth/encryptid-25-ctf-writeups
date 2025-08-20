# Hex Converter (Pwn)
## Recon
We are given a service that converts any given text into hex, and prints the result out. We can do four things in the program:

1. Give it some text to get the hex output.
2. A beta option where we give it text, and along with that, we also specify the number of characters to convert. (The first `n` characters get converted and printed out).
3. Leave a feedback.
4. Exit

## Vulnerabilities

1. In the second option, the program does not validate the value of `n` that we provide. So, if we specify a value that is greater than the length of the text, it's still gonna convert exactly `n` characters and directly print it out. Since we gave it less than `n` characters, the remaining bytes that get encoded are arbitrary values from the stack. We thus get out-of-bounds read access on the stack.

2. The third option contains a simple buffer overflow vulnerability, where it reads more bytes into the buffer than the size of the buffer itself. That way, we can overwrite return addresses and other arbitrary memory on the stack. However, canary protection is enabled, so it would be difficult to exploit this unless we can somehow get our hands on the canary.

## Exploitation
Using the two vulnerabilities we found, we can create an exploit to return to the `win()` function, which gives us the flag. You can find the exploit script in `exploit.py`. Here are the steps that are followed in the script:
1. We use the OOB read vulnerability to leak the canary value, along with a function address that may be stored on the stack. The function address will help us in bypassing PIE. On debugging the program, we find that at a particular offset, the address of `<main+0x7d>` is stored. Based on this, we calculate the runtime address of the program, and then that address will help us in calculating the runtime address of the `win()` function.
2. Once we've found all this, we create a buffer overflow payload in such a way that we overwrite the canary with the exact value of the canary itself, use a `RET` gadget to fix stack alignment issues, and finally specify the calculated address of the `win` function to return to it. This way, the function will be run and the flag is printed.