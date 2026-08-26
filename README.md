# Information Security

BMSTU IU7, semester 7.

Classical and modern ciphers implemented from scratch in C++ — no crypto
libraries — each with a console interface and a written report.

## Structure

| Path                        | Topic                                          |
|-----------------------------|------------------------------------------------|
| `lab-01-enigma`             | Enigma rotor machine                           |
| `lab-02-enigma`             | Enigma, extended version                       |
| `lab-03-rsa`                | RSA key generation, encryption, decryption     |
| `lab-04-des`                | DES block cipher                               |
| `lab-05-des`                | DES, extended version                          |
| `lab-06-08-aes`             | AES with its own S-box and round-key schedule  |
| `lab-08-09-rsa-signature`   | RSA digital signature: signing and verifying   |
| `final-vigenere`            | Vigenere cryptanalysis                         |
| `docs`                      | Assignments and reports                        |

## Build

Every lab is standalone and builds with `make`:

```sh
cd lab-06-08-aes && make
./aes
```

## Stack

C++, POSIX, Makefile
