# COMP4900-Quantum-Secure-OTA

## Authors
- Ben Whitten 101234504
- Caitlin Wardle 101219043
- Jacob Bonner 101234823

## Functions
# hash
Prints the SHA256 hash to the input file to the terminal.
```console
./hash test.txt
```

# sign_rsa
Signs a SHA256 hash using a private RSA key and writes it to a .bin file.
```console
./sign_rsa rsakeypair.pem $(./hash test.txt) signature.bin
```

# validate_rsa
Validates a signature using a public RSA key and SHA256 hash.
```console
./validate_rsa rsapubkey.pem $(./hash test.txt) signature.bin
```
