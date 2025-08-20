# Heaps of Bagels (Pwn)

## Recon
We're given a program that implements a bagel store. It first asks our name to start the execution, then gives us all these options:
```
1. Order a bagel
2. View order status
3. Edit your bagel order
4. Cancel an order
5. Show your profile
6. Update your profile
7. Show the quote of the day
8. Exit
```
We may use option 1 to create an order, and receive its order ID (a number from 0-9). Note that it is not possible to create more than 10 orders. We can then view its status, edit it, or cancel it by referencing the order using its ID. There are some additional options to do stuff with our profile (view and edit). Lastly, there's an option that simply prints the quote of the day, and one that exits.

## Cast to UserProfile
The first vulnerability lies in option 5 (show profile). When the program starts, it initializes a `user` variable with the type `char*`, essentially a pointer to a character array. This variable would have the size of a pointer (8 bytes for a 64-bit program). However, when we show the profile, it casts this variable to a special struct defined in the start:
```C
typedef struct {
    char* username_ptr;
    int user_age;
    int user_weight;
    int user_height;
    int user_bmi;
} UserProfile;
```
The size of this struct is 8 + 4 + 4 + 4 + 4 = `24 bytes`, clearly more than the original 8-byte pointer. Furthermore, all of these values are printed out, as shown in the source code:
```C
printf("Your username: %s\n", user_profile->username_ptr);
printf("Your age: %d\n", user_profile->user_age);
printf("Your weight: %d kg\n", user_profile->user_weight);
printf("Your height: %d cm\n", user_profile->user_height);
printf("Your BMI: %d whatevs\n", user_profile->user_bmi);
```
This means that the age, weight, height and BMI fields read an additional 24 - 8 = `16 bytes` from the stack. We may debug the binary in GDB or something to find that right after the `user` pointer, we have a pointer to the supposed "quote of the day". Since this quote is a string literal defined as a `const char*`, it isn't a local variable; rather, it's a global variable with a fixed offset in the binary. If we can leak its address (which we clearly can because of the overread), we can just subtract this address with its fixed offset in the binary to get the `PIE base`. We have successfully defeated PIE.

## Use-After-Free
When we order a bagel, the `order_bagel()` function is run. The program first asks us for the flavor of the bagel. This flavor is stored in a 64-byte buffer, which is then allocated as a 64-byte chunk on the heap:
```C
char* heap_buf = (char *)malloc(sizeof(buf));
```

 A pointer to this chunk is itself allocated as another chunk on the heap. This new chunk contains a pointer to the flavor chunk, and it has the size of a normal pointer (8 bytes for a 64-bit program):
 ```C
 char** heap_buf_ptr = (char **)malloc(sizeof(char *));
 *heap_buf_ptr = heap_buf;
 ```

Finally, after doing all this, the `order_bagel` function returns a pointer to the 8-byte chunk (which itself is a pointer to the actual flavor chunk). This double-layered process looks secure on the surface, but it all goes tumbling down once we find the use-after-free (UAF) vulnerability.

Take a look at a portion of the source code for the `cancel order` functionality:
```C
free(*mem_list[index]);
free(mem_list[index]);
```
Both the `heap_buf` and the `heap_buf_ptr` chunks are freed, but the pointer at `mem_list[index]` still points to the `heap_buf_ptr` chunk which no longer exists. This dangling pointer is the cause of the UAF.

We can think about the program this way: whenever we perform a given action on the order at index `i`, `mem_list[i]` is dereferenced to get another pointer. This new pointer is dereferenced again to get the bagel's flavor. But, when this order is canceled, the pointer chunk that `mem_list[i]` points to is freed. It then enters the `tcache_bins` in memory. Being in the tcache_bins means that if we ever try to allocate a chunk with the same size as the one that was freed, then the exact same memory region will be used to store the chunk. This is different from the default behavior of assigning a random space in the heap memory.

This is a classic UAF scenario. After having freed the `heap_buf_ptr` chunk which is 8 bytes long, it is stored in the tcache_bins. If we can allocate an arbitrary chunk of exactly 8 bytes filled with data that we control, we could potentially write any arbitrary pointer into the `heap_buf_ptr`. Then, whenever this pointer gets dereferenced (in view order, edit order, etc.) we are essentially dereferencing our own custom pointer.

And, we hit a jackpot: option 6 (update profile). This option asks us for the length of our name, then allocates a chunk of size `length + 1` containing our username. Because we control the length *and* the data in the username field, we can successfully allocate an 8-byte chunk containing a custom pointer instead of the username. This chunk will directly coincide with what used to be the `heap_buf_ptr`, and is still referenced by `mem_list[index]`. 

Having access to the memory space pointed to by our custom pointer gives us millions of opportunities: option 2 (view order status) **reads** arbitrary data from the memory space and prints it out, while option 3 (edit order) **writes** arbitrary data into the memory space.

We now have arbitrary read-and-write anywhere in the program.

## GOT Overwrite
With arbitrary R&W, along with a leaked PIE base, we are on top of the world. Though there are probably many ways to do this from this point forth, the easiest way is to overwrite the GOT address of some function. Now, the task is to find a function that, if overwritten, can allow us to pop a shell.

The most cliche way to do this is to find a function that can be overwritten with the address of `system`. As a prerequisite, this ideal function should have an argument passed to it that is a pointer to some region in memory that we control. That way, we can write `/bin/sh` to that memory space, and make the program call `whatever_function(pointer_to_memory_space)`.

Scanning the source code gives us one such function that may be used, which is also the function that has caused so many more vulnerabilities in the program: `free`. We saw that on canceling an order, the program runs:

```C
free(*mem_list[index]);
free(mem_list[index]);
```

`*mem_list[index]` is just a pointer to the `flavor` chunk for a given order. And, obviously, we control the flavor for any given order. If we overwrite the GOT address of `free` with the address of `system`, then the call becomes `system(*mem_list[index])`. As mentioned before, this call can be rewritten as `system(pointer_to_flavor_chunk)` because `*mem_list[index]` is a pointer to the `flavor` chunk. If we specify the flavor as `/bin/sh`, the call becomes `system(pointer_to_bin_sh)`. Do you see where we're going? This function call gives us a shell on the remote machine.

## Exploit

1. Run the program and provide any random username and length to begin with (really doesn't matter).

2. Create a new order and specify any random flavor (again, doesn't matter for now). The order ID for this order should be 0, because it's the first order.

3. Now, choose option 4 to cancel this order, using the order ID `0`. Doing this frees the `heap_buf` and `heap_buf_ptr` chunks for that particular order, storing those chunks in the `tcache_bins`, beginning the UAF exploit.

4. Before proceeding with allocating our custom pointer, we need to calculate the PIE base (the pointer will be relative to this base address). So, use option 5 (show profile) to get the 16-byte overread and leak the address of `const char *quote_of_the_day`. Then, we use this, along with its fixed offset in the binary, to leak the PIE base.

5. Relative to the PIE base, we calculate the GOT address of `free`. Choose option 6 to update your profile. Now, we want to allocate exactly 8 bytes on the heap. And, we know that if we specify a length `n`, then `n + 1` bytes are allocated. So the length we should specify is `7`. In place of the username, we put the little-endian GOT address of `free`.

6. Next up, we need to leak the data present in the newly allocated pointer to `free`'s GOT, which will be the runtime address of `free`. For that, we may use option 2 (view order status) which will simply print out the desired address because of the pointer dereference.

7. Before anything else, we're gonna need the libc binary. There are many ways to get it, including making use of the leaked GOT addresses, but a much easier way is to pull out the libc binary directly from the challenge docker image. Because the challenge uses the `ubuntu:22.04` image, we could just get the binary from there. Now that we have the libc, we calculate the libc base by subtracting `free`'s fixed offset from its leaked runtime address, which gives us the libc base.

8. Relative to this base, we can now calculate pointers to other useful functions like `system`. Because we also have arbitrary write, we can simply write arbitrary addresses into `free`'s GOT address, for example, replacing it with the address of `system`, and we're going to do exactly that. It's worth noting that in the GOT table, the address of `free` is directly followed by the address of `puts`. Accidentally overwriting `puts` may cause undefined segmentation faults to occur, which is bound to happen because at least one trailing newline character will overflow into the GOT of `puts`. To fix this, we can just overwrite the GOT of `puts` with its own runtime address, essentially causing no change to occur in that function. The newline overflow occurs in the function present after `puts`, which is `__stack_chk_fail`: a function which can never be run in our program (we aren't overwriting the canary). Overwriting this function's address hardly matters.

9. Finally, we create a new order with the flavor set as `/bin/sh`. When we cancel this order, the program runs `free(<pointer_to_flavor>)`. We have already overwritten free's GOT address with system, so the call becomes `system(<pointer_to_flavor>)`. And, since the flavor was specified as `/bin/sh`, the call becomes `system(<pointer_to_bin_sh>)`. Voila! We've popped a shell.

## Flag

```
└─$ python3 exploit.py REMOTE
[+] Opening connection to localhost on port 3000: Done
[*] Switching to interactive mode
$ $ id
uid=1000(ctf) gid=1000(ctf) groups=1000(ctf)
$ $ cat /flag.txt
encryptid{0h_mY_sw33t_buTT3rY_b4g3ls}
```
