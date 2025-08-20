# Backdoors (Web)

## Recon
We are provided a website that seems to be a simple bunny blog (there are countless pics of bunnies on the cover page). On scrolling down, we see that there are some links to other pages on the site.

These links navigate us to pages that look like the structure `/?page=page1.php`, `/?page=page2.php`, etc. Because the file name is directly passed through the URL, it makes sense that an LFI (local file inclusion) vulnerability could exist here, allowing us to read arbitrary files on the system.

We can try to visit a page like `/?page=/etc/passwd` to read the passwd file, but it only results in the error that "the file name must end with .php". Bummer.

The challenge title and description mention a "backdoor", so there might be some useful stuff in the source code. But if we go to `/?page=index.php`, or rather any PHP file, it would render the PHP code and only give us the output, rather than giving us the actual source. We, though, need the source.

For that, we use a technique called PHP filter chaining, a popular technique used in such LFI scenarios. Essentially, instead of a file like `page1.php`, we provide a php:// url that may look like: `php://filter/convert.base64-encode/resource=page1.php`. Here, php://filter URLs are just some built-in filters that will encode the contents of a given file using whichever filter we specify (in this case, base64), and then print it out. This way, since the server sees no valid PHP code (everything is base64 encoded), it only renders the contents as text. We can then decode the base64 output on our own to get the actual PHP source of the page.

## Exploitation
We use this LFI vulnerability to view the source code of all the PHP pages on the site. For example, to view the source of the homepage, we visit `/?page=php://filter/convert.base64-encode/resource=index.php`. This filter chain will give us the base64 encoded sourced code of index.php. 

The code itself seems fine at first, but at the top, there is a single comment that seems suspicious:

```PHP
// TODO: remove the backdoor at gV82gCVr46xy6lxryz9sLkvk.php
```

which suggests that there exists a page at `/gV82gCVr46xy6lxryz9sLkvk.php`. Let's look at the source code for that too, using `/?page=php://filter/convert.base64-encode/resource=gV82gCVr46xy6lxryz9sLkvk.php`.

Now this is interesting: this page is just a file explorer sort of utility. It needs two GET parameters in the URL: a `key` that it uses for authentication, comparing it with a constant located at `/opt/super_secret_key.txt`; and the second param is `dir`, which is just the directory that we wish to explore. If the key doesn't match the key at the file path, the program issues an error.

We can try to look at the key's value using the same LFI technique, but we want the file to end with `.php` when it really ends with `.txt`. This is the tricky part. If we look at the source code for the `.php` check once again (which we now have due to the LFI):
```PHP
...
if (preg_match('/\.php/', $_GET['page'])) {
...
```

This checks for the presence of `.php` *anywhere* in the entire parameter, and not just at the end. To exploit this, we can use the functioning of a PHP filter chain against itself. Here's a concept: if we wanna put more than one filter, then we can essentially *chain the filters together* like `php://filter/<filter_1>/<filter2>/<filter3>/....../<filter_n>/resource=whatever_file`. In the previous case, we only needed one filter which was `convert.base64-encode`. However, check this: if one of the filters is invalid, no error occurs: instead, it just ignores the filter and moves on. That way, a filter might be the best place to smuggle in our `.php`.

A filter chain that bypasses this check could be: `php://filter/.php/convert.base64-encode/resource=/opt/super_secret_key.txt`. The server sees a filter called ".php" which is obviously not a valid filter, so it just moves on to the next one (base64 encoding). This way, our chain now has `.php` in it, *and* it reads the super_secret_key file.

The key is just a very long hex string that looks like: `3ccd....6cb5` (a major portion has been redacted). Now that we have the key, we can access the backdoor at `gV82gCVr46xy6lxryz9sLkvk.php`. Just go to `/gV82gCVr46xy6lxryz9sLkvk.php?key=<key_goes_here>&dir=/`. Replace the `dir` with whatever directory you wish to see the contents of.

On going to the above mentioned page, we now see:
```
Files in directory: /
    proc
    dev
    etc
    lib
    mnt
    var
    opt
    sbin
    sys
    home
    usr
    media
    tmp
    run
    srv
    root
    boot
    bin
    lib64
    .dockerenv
    38278293748293742374237498247.txt
```

Clearly, `38278293748293742374237498247.txt` is not supposed to be there, so we view its contents the same way we looked at the super_secret_key: `/?page=php://filter/.php/convert.base64-encode/resource=/38278293748293742374237498247.txt`

Decode the output with base64 to get the flag:
```
encryptid{st0p_c0d1nG_1n_PHP_f0r_y0Ur_0wN_s4n1tY}
```