# COMP4900-Quantum-Secure-OTA

## Authors
- Ben Whitten 101234504
- Caitlin Wardle 101219043
- Jacob Bonner 101234823

## Functions
### hash
Prints the SHA256 hash to the input file to the terminal.
```console
./hash test.txt
```

### signing
Signs a SHA256 hash using a private key and writes it to a .bin file.\
RSA
```console
./sign_rsa rsakeypair.pem $(./hash test.txt) signature.bin
```
MLDSA
```console
./sign_mldsa mldsa_prvkey.bin $(./hash test.txt) signature.bin
```

### validate_rsa
Validates a signature using a public RSA key and SHA256 hash.\
RSA
```console
./validate_rsa rsapubkey.pem $(./hash test.txt) signature.bin
```
MLDSA
```console
./validate_mldsa mldsa_pubkey.bin $(./hash test.txt) signature.bin
```
