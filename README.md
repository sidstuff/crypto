# Cryptography Tutorial

Cryptography refers to techniques for the encryption/decryption and signing/verification of messages.

Symmetric cryptosystems use the same key, which should be kept private, for encryption and decryption, and is thus aka private key cryptography.

Asymmetric cryptography uses a key pair consisting of a public and a private key, and is thus aka public key cryptography.

From [Wikipedia](https://en.wikipedia.org/wiki/Public-key_cryptography):

> In a public-key encryption system, anyone with a public key can encrypt a message, yielding a ciphertext, but only those who know the corresponding private key can decrypt the ciphertext to obtain the original message.
>
> In a digital signature system, a sender can use a private key together with a message to create a signature. Anyone with the corresponding public key can verify whether the signature matches the message, but a forger who does not know the private key cannot find any message/signature pair that will pass verification with the public key.

## Software

In this tutorial, we'll be using [OpenSSL](https://www.openssl.org), as well as a simple implementation [`crypto.c`](https://github.com/sidstuff/crypto/blob/master/crypto.c) of the [RSA public key algorithm](https://en.wikipedia.org/wiki/RSA_(cryptosystem)). It has the [GNU Multiple Precision Arithmetic Library](https://gmplib.org) as a dependency and can be compiled by running
```
gcc crypto.c -o crypto -lgmp
```

## Usage

Symmetric cryptography is easy to use – here's how to do it using OpenSSL, relying on the AES-256 (Advanced Encryption Standard) algorithm:

Encryption:
```
openssl enc -in myfile -out encrypted.dat -e -aes256 -k symmetrickey
```

Decryption:
```
openssl enc -in encrypted.dat -out myfile -d -aes256 -k symmetrickey
```

`crypto` can output random data sourced from `/dev/urandom`, for example, `./crypto gen random 256` prints a random 256 bits as a base10 number. But 256 random bits means that the number generated may also be very small – in cryptography, an $n$-bit number means one in the range $[2^{n-1},2^n)$. `./crypto gen key 256` will output such a number, formatted in base62 for easy sending, and `./crypto gen prime 256` will output a (base10-formatted) prime in that range. The output of `./crypto gen key` can be used as a key for symmetric cryptography.
```
sVQCtdv7Ka4FMFcEKxBIWMyCVtaUY0P8cIi9C2IOImH
```

> [!NOTE]  
> Determining with 100% certainty whether a number is prime is computationally infeasible for very large primes. Instead, we run the Miller-Rabin test, a probilistic primality test. Even if special composite numbers are chosen to trick the MR test, it will declare them prime at worst $\frac{1}{4}$ of the time. Thus we can run $k$ iterations of the test with independently chosen bases, and reduce this probability to $4^{-k}$. In our program, $k=40$. There are diminishing returns in trying many bases, because if $n$ is a pseudoprime to some base, then it seems more likely to be a pseudoprime to another base, and also because after a point, a bit flip seems more likely than the test returning an inaccurate result.

For asymmetric cryptography, we'll be using the RSA (Rivest–Shamir–Adleman) cryptosystem. In it, the private and public keys, at minimum, consist of the number pairs `N d` and `N e`, respectively. The key size refers to the size of `N`. `./crypto gen pair 2048` will output a 2048-bit (the NIST recommended minimum till 2030) private and public key (separated by a newline), in that order.
```
dYVK2diBVkss902lbkuwP74Q0z4XZSByXl8jftg3fi0csv9dz2oiAvIKkf3bdMdPb1zcrzVMJArkPSFScmyiTSCOSwCykpJk7tyYBoJkeAiLvxDsRokXrLZmaziCb3in8U3tnF1LNWW1oLffIKLbfsiWAWi4nh5LTYoHgaS9xdGHnnaajRYN47uPkDQ12b4GCD0oJabZuipjNZ1oQa6yAXbVz21EvbqChvt2Px2uggDVlyNBs3STBBEqjNbS0NgvqkhZicOf9cDwqnd7UcVALtmzyOB6rY6Bp2CnpJqoBKWahNjfVEYWx7NBT40CCOToh09RfsZnsgdr2x5CbEHsP47X 3yvTRAMdpDt04xOphJLENahQGOZw8KxwkzvSTmdPKVaePbeaxC3VYfPKRiuzDMtgs7Vvdytz52G7pe2hXxSRJIdzesbRmN8MUIrc56GVdcvDhrW8jkIyaEZUv8FAH8orupbTvbZ6DEHZ4WbTZFrQZepyD81SjuAoNrLZT4YHg8RxWVCaTr15OQddkHf47CvYKaAuc3ni9MgQMeNf1gxeIHl6O0JWL6jm1tjdwL9zGwRy2UA14NlLH8nOqzRNOYKakEMM7n3qIw8IGDfZIfhE0kdNQia7WPh8OFjW2yCqj4trg5VSeQQGTcz19jj5HlNXkHVMF7AgSdW6Es5XaaU4AWSD
dYVK2diBVkss902lbkuwP74Q0z4XZSByXl8jftg3fi0csv9dz2oiAvIKkf3bdMdPb1zcrzVMJArkPSFScmyiTSCOSwCykpJk7tyYBoJkeAiLvxDsRokXrLZmaziCb3in8U3tnF1LNWW1oLffIKLbfsiWAWi4nh5LTYoHgaS9xdGHnnaajRYN47uPkDQ12b4GCD0oJabZuipjNZ1oQa6yAXbVz21EvbqChvt2Px2uggDVlyNBs3STBBEqjNbS0NgvqkhZicOf9cDwqnd7UcVALtmzyOB6rY6Bp2CnpJqoBKWahNjfVEYWx7NBT40CCOToh09RfsZnsgdr2x5CbEHsP47X H33
```

Encryption and decryption consist of raising the message `m` and the ciphertext `c`, to the power `e` and `d` mod `N`, respectively. It relies on the fact that given only `e` and `N`, it is extremely difficult to find `d`. As the one generating the keys, we first find two large primes and multiply them together to generate `N`, and having those primes, it is easy to find `d` for a given `e`. To make encryption fast, `e` is a small value, conventionally 65537 – so if we have a private key we know was generated using that convention (`./crypto gen private 2048 > private.key`), we can produce the corresponding public key from it (`cat private.key | ./crypto pub > public.key`).

It is necessary that $0<m<N$, so the size of the message is limited by the size of the key. The usual practice is to symmetrically encrypt a longer message/file, and use asymmetric encryption on the symmetric key (which should be smaller than the asymmetric key).
```
$ cat public.key | ./crypto enc sVQCtdv7Ka4FMFcEKxBIWMyCVtaUY0P8cIi9C2IOImH
O0xqoJltic1Nt5U4f2ChI1rMhBCP41mny2LG7N0P7SfYRPRRMpHteabw5QuB14Yt9i6sBehiMen3Qd6tcmKzbuPSyoAMZoSfSjXwcArttnMSBcjROLOpsXC4bgyO1bCMausDrAD9X6Z905QB0oo6JXaMW3kkVPK5ZSORq2Yz5CRzRZh12UGpmtcwMkV5qXPmvjsF5p0Xz98PbW4P4UPBzYRJ6rXWTITfzN5yZN1N3SF54WHR3Aj4EvHa4d5UDmXozJ6FebtZvk126UGzq3rJwdM8NqvCBHrN0Q1xfvOJop6cn7Nc3t0PEE8CdecSNoxE7iQvMGJfkmXBxWalaoMLygj6
$ cat private.key | ./crypto dec O0xqoJltic1Nt5U4f2ChI1rMhBCP41mny2LG7N0P7SfYRPRRMpHteabw5QuB14Yt9i6sBehiMen3Qd6tcmKzbuPSyoAMZoSfSjXwcArttnMSBcjROLOpsXC4bgyO1bCMausDrAD9X6Z905QB0oo6JXaMW3kkVPK5ZSORq2Yz5CRzRZh12UGpmtcwMkV5qXPmvjsF5p0Xz98PbW4P4UPBzYRJ6rXWTITfzN5yZN1N3SF54WHR3Aj4EvHa4d5UDmXozJ6FebtZvk126UGzq3rJwdM8NqvCBHrN0Q1xfvOJop6cn7Nc3t0PEE8CdecSNoxE7iQvMGJfkmXBxWalaoMLygj6
sVQCtdv7Ka4FMFcEKxBIWMyCVtaUY0P8cIi9C2IOImH
```

To sign messages, first we compute its hash using SHA-256 (Secure Hash Algorithm)
```
$ echo -e "TIME: 1727777777\n\nI know what you did last summer :)" > msg.txt
$ openssl dgst -sha256 msg.txt | awk '{print $NF}'
78329016ff71d84925c7a109bc9af71828dec2828ca30e5a865af12aa6da38b6
```
Then we sign it using our private key, i.e., raise it to the power `d` mod `N`.
```
$ cat private.key | ./crypto sign 78329016ff71d84925c7a109bc9af71828dec2828ca30e5a865af12aa6da38b6
6IMmkNsmXRSd0GTFExIMFTB4NZL1D8qBzBKxfjCZ9nRV060lgQnRMYISyZxTeGs0sRbEBSwDeAQoLVEjEUPy3J9Wa2jmhJ5czLqS8VmZiZy7NZg1pIMknsjSIQy8VFzvV4teC33kOabnSUphKsJ6TPgOazeUl4rti6Jq3LpQWQ2NETyiAu9jMZ8JklWjirKVI4Y1UJyuyfoVRnG4bQD62vraZ42ARt1kn1ouaLtFiWCfPqKWxKudPEpNgPXkH12ENz8kKctUMEwxOzvZAOznV28jjKFn2is1ZdsaN3cZtdVfZT7xyVvH6N2eLSIImnseNiBN37CQG6yka0Bqpz6mW3w4
```
The reader can verify the signature using our public key, i.e., raise it to the power `e` mod `N`, and see that it matches the hash of the message.
```
$ cat public.key | ./crypto verify 6IMmkNsmXRSd0GTFExIMFTB4NZL1D8qBzBKxfjCZ9nRV060lgQnRMYISyZxTeGs0sRbEBSwDeAQoLVEjEUPy3J9Wa2jmhJ5czLqS8VmZiZy7NZg1pIMknsjSIQy8VFzvV4teC33kOabnSUphKsJ6TPgOazeUl4rti6Jq3LpQWQ2NETyiAu9jMZ8JklWjirKVI4Y1UJyuyfoVRnG4bQD62vraZ42ARt1kn1ouaLtFiWCfPqKWxKudPEpNgPXkH12ENz8kKctUMEwxOzvZAOznV28jjKFn2is1ZdsaN3cZtdVfZT7xyVvH6N2eLSIImnseNiBN37CQG6yka0Bqpz6mW3w4
78329016ff71d84925c7a109bc9af71828dec2828ca30e5a865af12aa6da38b6
```
Only the one with the private key could have signed the message, thus its authenticity is confirmed.

We include a timestamp so a MitM (man in the middle) can't intercept your message and resend it at a different time pretending to be you again. Perhaps one might want to send more than one message per second, then some othet kind of unique ID can be used for the messages. Now, how do we combine encryption and signing?

If we sign the plaintext (sign-then-encrypt), the recipient can re-encrypt the message for another person and send it to them, who'll now think you're threatening *them* over last summer. To prevent this, we should include the intended recipient's identity in the message if using sign-then-encrypt.

If we sign the ciphertext (encrypt-then-sign), a MitM can intercept the message and replace the signature with their own. Now the recipient thinks the intermediary is the one who knows what they did last summer. To prevent this, we should include our (the sender's) identity in the message if using encrypt-then-sign.

But even then, if the ciphertext is what's being signed, an intermediary can compute the ciphertext hash and verify the signature with the public keys of everyone on the network to find out who sent the message. This makes encrypt-then-sign incompatible with additional anonymization technologies, so sign-then-encrypt is the standard method. Note that if we're using such anonymization methods then the sender's name won't be transmitted in cleartext, and will also have to be included in the message for the recipient to know who is contacting them.

Now here are the OpenSSL equivalents of the various commands we've learnt so far:
```
openssl rand -hex 256
openssl genrsa -out privkey.pem 2048
openssl rsa -in privkey.pem -pubout -out pubkey.pem
openssl rsautl -encrypt -pubin -inkey pubkey.pem -in message.txt -out encrypted.txt
openssl rsautl -decrypt -inkey privkey.pem -in encrypted.txt -out message.txt
openssl rsautl -sign -inkey privkey.pem -in message.txt -out signed.txt
openssl rsautl -verify -pubin -inkey pubkey.pem -in signed.txt -out message.txt
```

## Digital certificates

For pseudonymous communications, the above is sufficient. Your identity is simply "the one whose public key is `xyz`". You can sign and send messages like `you can call me sidstuff`, `i like cats <3`, etc., and those will be associated with this online identity.

But what if you want to associate your public key with your real life identity or your website, so people can trust they're actually communicating with the person/website they think they are? The simplest solution is a centralized trust model, where you have trusted third parties called certificate authorities, whose public keys come built into your OS and browser. They are trusted to verify your identity or ownership of a given email or domain name, and grant you a certificate signed by them saying that your public key does indeed belong to XYZ.

For instance, let's say youcangetintouchwithsid@gmail.com is my work email (it's not, consider it compromised). Go to https://www.actalis.com/request-s-mime-certificate, create an account, and generate `certificate_s_mime.p12`.

From [Wikipedia](https://en.wikipedia.org/wiki/PKCS_12),
> A PKCS #12 file (`.p12` or `.pfx`) defines an archive file format for storing many cryptography objects as a single file. It is commonly used to bundle a private key with its X.509 certificate or to bundle all the members of a chain of trust.

To obtain our private key from `certificate_s_mime.p12`, run
```
$ openssl pkcs12 -in certificate_s_mime.p12 -nocerts -nodes -out priv.key
Enter Import Password: z6KVABZTK2gd*bC&02
$ cat priv.key
Bag Attributes
    localKeyID: EF 8C B5 D8 AC 16 2E E3 C8 BD 8C 60 E5 E8 B9 94 DA 90 1A B1 
    friendlyName: youcangetintouchwithsid@gmail.com
Key Attributes: <No Attributes>
-----BEGIN PRIVATE KEY-----
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCrM5z/aBuIEaHs
y45yd+aslEs6VsT3iLM4YFKYgx747ixZZmIAswozQENwnhZU71TzgRr81Jl4kRcE
DuH6YSQqn2ZiUAy59Jnd7hHvtyzKiwCJvF6+RRrp5ONWkeojcnzQ6jbgn+1Z3Cn7
TpMDmsKogtFpFB0Li5K+vfQerVbJX/YovCIYtbz3tuUDnSp4qPzpfGMlc6O/Ci+k
4ZIjxKhgzHcdoLir2/EHUrLBDQJrjHf7uWQEpV9rbO58H8rC4mwx1nyACJENRgOR
xTQyRrx34ZL3vZGPtZKNyYPAEUw0UdgARB/zeYPw6jCN0nJrMq0DNUPu9O19Z4SS
hnUPZhc5AgMBAAECggEACQfa8zoJob7UXMpMBEvA1j+oTDaBg1EpqwKhuLvJ6XWe
z0RwTPskwZlVNV6RbP+G6qVXyBhq1ApLPYKPdwaiwsDPxMXWNgTqDOPe/FccPL7i
U2GlWcIJbmcNeuaNhDsozFhedrDY81GCwWfep2UFeaAU7ccK1S4Smo8ojmm0V0Dr
fsJCXypsunwIgf2ND+rZj/drqDkWmq4Bb2BaPbmviVwNwFjtVJjBov5BNP4m+Rnz
RTapFiv3ML5Xv61rxa8E/oD3W6T3Z5TXmi1SLLI77uCE70NyOfHVpnOBXS6JaJo7
iq4NYuqtsOY86i1AUfUXFh8LlUB6PRFzJA7N4SW39QKBgQDapJKAFVpUTZEqhw6d
EVE13vtrMvp+DoQNppCnr1X4jEfuazkS3BjAi7l46+bJLa2kYh2w0ifreQscpRri
Nn1i3LsSRk88xGNUJoFV+G+ssYPobfKHGHR9hMdoz5PKXscuZYbidVynstaCaTth
EIcJMwpEdYm7psBhSPlz98P8LwKBgQDIc/aZ1fpVauyPddBOpXrE1ULdwkin0TXG
9FWu/BosYiL/mIKgcOUDoB1SLXevHV3yirqOxpdrJ/TtBAfWUt8GZ9hoVDX0yk+t
WBdWHat42OtquJDyhJRF9nvJlCdqDY1RVxXNYRRMnJLEssJHGdgReC8EyLmBjYMP
CIlhXUXBFwKBgF0QBYddD1iot2xJli/GqjFq2srzJOm41f5VcmbdW5Nyaj+SBfYA
Fs0F7AkLJ+1jcvBwDsEvgmxY1iD4Kaw1k0McPsf1g06TLNApn41vNGFmIYYyHjPg
ddzjQllbWUWe99rIvhPv7mM23NUfY3XaKhABWJJxERH9VQ9Tl1IEGLhjAoGAZXkO
pOGFUChIiHcDdEyg0XOD3e60CQAHUSfvGB3WwhEL1ofF2KJNzIAJV8hchfE/1cAU
oKI3lK8tda5MqVpaPEvlaSr4BWtE2mN1rMpoO0zUH7o+5gXnaWNVVr4BFda2ay7C
gq1aMU4xnhE/9+5FStKkOq/B+KtCZSHVKcyluxECgYEAyExP8bR3li5XuaN9aGfd
/TcufkCFWLJfuxZLP7vk/3P71oR9Ew3tvhU+tb3eW72AtsYwjpW4pwQCM7B/PnRy
pnSqgT/x4j8vvVMdq+IUcKghYza85gBVuNdh7jK2u3WYrZcsvjyf5NKBqjQyeHoq
xWOS3cvSPQyDcyY6xVTLKZA=
-----END PRIVATE KEY-----
```
To get our certificate, run
```
$ openssl pkcs12 -in certificate_s_mime.p12 -clcerts -nokeys -out mycert.crt
Enter Import Password: z6KVABZTK2gd*bC&02
```
The password was sent via email. Now let's take a look inside our certificate.
```
$ cat mycert.crt
Bag Attributes
    localKeyID: EF 8C B5 D8 AC 16 2E E3 C8 BD 8C 60 E5 E8 B9 94 DA 90 1A B1 
    friendlyName: youcangetintouchwithsid@gmail.com
subject=CN = youcangetintouchwithsid@gmail.com
issuer=C = IT, ST = Bergamo, L = Ponte San Pietro, O = Actalis S.p.A., CN = Actalis Client Authentication CA G3
-----BEGIN CERTIFICATE-----
MIIGBzCCA++gAwIBAgIQFQkvcybhP8kHtmX838Hi+DANBgkqhkiG9w0BAQsFADCB
gTELMAkGA1UEBhMCSVQxEDAOBgNVBAgMB0JlcmdhbW8xGTAXBgNVBAcMEFBvbnRl
IFNhbiBQaWV0cm8xFzAVBgNVBAoMDkFjdGFsaXMgUy5wLkEuMSwwKgYDVQQDDCNB
Y3RhbGlzIENsaWVudCBBdXRoZW50aWNhdGlvbiBDQSBHMzAeFw0yNDEwMTAxODUw
MjBaFw0yNTEwMTAxODUwMjBaMCwxKjAoBgNVBAMMIXlvdWNhbmdldGludG91Y2h3
aXRoc2lkQGdtYWlsLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB
AKsznP9oG4gRoezLjnJ35qyUSzpWxPeIszhgUpiDHvjuLFlmYgCzCjNAQ3CeFlTv
VPOBGvzUmXiRFwQO4fphJCqfZmJQDLn0md3uEe+3LMqLAIm8Xr5FGunk41aR6iNy
fNDqNuCf7VncKftOkwOawqiC0WkUHQuLkr699B6tVslf9ii8Ihi1vPe25QOdKnio
/Ol8YyVzo78KL6ThkiPEqGDMdx2guKvb8QdSssENAmuMd/u5ZASlX2ts7nwfysLi
bDHWfIAIkQ1GA5HFNDJGvHfhkve9kY+1ko3Jg8ARTDRR2ABEH/N5g/DqMI3Scmsy
rQM1Q+707X1nhJKGdQ9mFzkCAwEAAaOCAc0wggHJMAwGA1UdEwEB/wQCMAAwHwYD
VR0jBBgwFoAUvpepqoS/gL8QU30JMvnhLjIbz3cwfgYIKwYBBQUHAQEEcjBwMDsG
CCsGAQUFBzAChi9odHRwOi8vY2FjZXJ0LmFjdGFsaXMuaXQvY2VydHMvYWN0YWxp
cy1hdXRjbGlnMzAxBggrBgEFBQcwAYYlaHR0cDovL29jc3AwOS5hY3RhbGlzLml0
L1ZBL0FVVEhDTC1HMzAsBgNVHREEJTAjgSF5b3VjYW5nZXRpbnRvdWNod2l0aHNp
ZEBnbWFpbC5jb20wUgYDVR0gBEswSTA8BgYrgR8QAQEwMjAwBggrBgEFBQcCARYk
aHR0cHM6Ly93d3cuYWN0YWxpcy5pdC9hcmVhLWRvd25sb2FkMAkGB2eBDAEFAQEw
HQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMEMEgGA1UdHwRBMD8wPaA7oDmG
N2h0dHA6Ly9jcmwwOS5hY3RhbGlzLml0L1JlcG9zaXRvcnkvQVVUSENMLUczL2dl
dExhc3RDUkwwHQYDVR0OBBYEFO+MtdisFi7jyL2MYOXouZTakBqxMA4GA1UdDwEB
/wQEAwIFoDANBgkqhkiG9w0BAQsFAAOCAgEAWk4iQYgFm3QMQcZgT9F5O9udBEsh
fi9QNIyaPZg/cjWT0UedyPfnqxfq3f284ttlKO8D3HaAWw3+Vk047Q/sOGonyMOr
hoN8shZ7m3CK7tMH2ifxSlmE7o4Nsz+n2NrWvtySQVhwiNW5EWxAk0UvGVaDkcgx
lzV6mi9Q1xCbHKlBW8xyrrBO+5nAW8P/3/HH9SMGAKoXQO79xCrF9nj4iEeNDgmq
pazS8Fx2I6UnnnG8PrcWUvdSiU7QL6pINsKDqXqNeFM/1mgnPqH+Z9GICjaNt/j8
jw5mI0w6eUc1eCSpo3QPAnbTRUDJdUxWOOBx6V5WKI3jR3sVd08hSXk6ytrPuXZ5
m92jopQqJUrM/4aXXnDkAzCsRpF1WdpCC25vr7Cv9UD92yrRETRFrjWggG+RC4Ya
YwZvk2dKTekZbmh02ewMhy6B4s+Y9/yhY+UFMObxEER7/2fOnbrvpinG2fgX/Pmg
B3ZRhQzxIvNQKleHHEpXnXMS19P7EUTTQss8dWFDia2bDRlQHMfC7YJcCM4Ut4f2
4sCR2rD1/WL+0kBQSXPNQX1IUsWe2zqV4y1fzFHpADfkutbVoAZhp/wUoNQjIl8b
oL+wOfsvPRbrx9BEVIgewP+HiAQQmHA0ODET1abxF8BcmzvOudbg88Pig/l3wZHH
Y/7icI64btlgLCA=
-----END CERTIFICATE-----
```
The stuff before `-----BEGIN CERTIFICATE-----` can be removed, running the following will do it:
```
openssl x509 -in mycert.crt -out my_cert.crt
```
We share our certificate with the people we want to communicate with. To read the information contained in the certificate, they can run
```
$ openssl x509 -in my_cert.crt -text -noout
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number:
            15:09:2f:73:26:e1:3f:c9:07:b6:65:fc:df:c1:e2:f8
        Signature Algorithm: sha256WithRSAEncryption
        Issuer: C = IT, ST = Bergamo, L = Ponte San Pietro, O = Actalis S.p.A., CN = Actalis Client Authentication CA G3
        Validity
            Not Before: Oct 10 18:50:20 2024 GMT
            Not After : Oct 10 18:50:20 2025 GMT
        Subject: CN = youcangetintouchwithsid@gmail.com
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                Public-Key: (2048 bit)
                Modulus:
                    00:ab:33:9c:ff:68:1b:88:11:a1:ec:cb:8e:72:77:
                    e6:ac:94:4b:3a:56:c4:f7:88:b3:38:60:52:98:83:
                    1e:f8:ee:2c:59:66:62:00:b3:0a:33:40:43:70:9e:
                    16:54:ef:54:f3:81:1a:fc:d4:99:78:91:17:04:0e:
                    e1:fa:61:24:2a:9f:66:62:50:0c:b9:f4:99:dd:ee:
                    11:ef:b7:2c:ca:8b:00:89:bc:5e:be:45:1a:e9:e4:
                    e3:56:91:ea:23:72:7c:d0:ea:36:e0:9f:ed:59:dc:
                    29:fb:4e:93:03:9a:c2:a8:82:d1:69:14:1d:0b:8b:
                    92:be:bd:f4:1e:ad:56:c9:5f:f6:28:bc:22:18:b5:
                    bc:f7:b6:e5:03:9d:2a:78:a8:fc:e9:7c:63:25:73:
                    a3:bf:0a:2f:a4:e1:92:23:c4:a8:60:cc:77:1d:a0:
                    b8:ab:db:f1:07:52:b2:c1:0d:02:6b:8c:77:fb:b9:
                    64:04:a5:5f:6b:6c:ee:7c:1f:ca:c2:e2:6c:31:d6:
                    7c:80:08:91:0d:46:03:91:c5:34:32:46:bc:77:e1:
                    92:f7:bd:91:8f:b5:92:8d:c9:83:c0:11:4c:34:51:
                    d8:00:44:1f:f3:79:83:f0:ea:30:8d:d2:72:6b:32:
                    ad:03:35:43:ee:f4:ed:7d:67:84:92:86:75:0f:66:
                    17:39
                Exponent: 65537 (0x10001)
        X509v3 extensions:
            X509v3 Basic Constraints: critical
                CA:FALSE
            X509v3 Authority Key Identifier: 
                BE:97:A9:AA:84:BF:80:BF:10:53:7D:09:32:F9:E1:2E:32:1B:CF:77
            Authority Information Access: 
                CA Issuers - URI:http://cacert.actalis.it/certs/actalis-autclig3
                OCSP - URI:http://ocsp09.actalis.it/VA/AUTHCL-G3
            X509v3 Subject Alternative Name: 
                email:youcangetintouchwithsid@gmail.com
            X509v3 Certificate Policies: 
                Policy: 1.3.159.16.1.1
                  CPS: https://www.actalis.it/area-download
                Policy: 2.23.140.1.5.1.1
            X509v3 Extended Key Usage: 
                TLS Web Client Authentication, E-mail Protection
            X509v3 CRL Distribution Points: 
                Full Name:
                  URI:http://crl09.actalis.it/Repository/AUTHCL-G3/getLastCRL
            X509v3 Subject Key Identifier: 
                EF:8C:B5:D8:AC:16:2E:E3:C8:BD:8C:60:E5:E8:B9:94:DA:90:1A:B1
            X509v3 Key Usage: critical
                Digital Signature, Key Encipherment
    Signature Algorithm: sha256WithRSAEncryption
    Signature Value:
        5a:4e:22:41:88:05:9b:74:0c:41:c6:60:4f:d1:79:3b:db:9d:
        04:4b:21:7e:2f:50:34:8c:9a:3d:98:3f:72:35:93:d1:47:9d:
        c8:f7:e7:ab:17:ea:dd:fd:bc:e2:db:65:28:ef:03:dc:76:80:
        5b:0d:fe:56:4d:38:ed:0f:ec:38:6a:27:c8:c3:ab:86:83:7c:
        b2:16:7b:9b:70:8a:ee:d3:07:da:27:f1:4a:59:84:ee:8e:0d:
        b3:3f:a7:d8:da:d6:be:dc:92:41:58:70:88:d5:b9:11:6c:40:
        93:45:2f:19:56:83:91:c8:31:97:35:7a:9a:2f:50:d7:10:9b:
        1c:a9:41:5b:cc:72:ae:b0:4e:fb:99:c0:5b:c3:ff:df:f1:c7:
        f5:23:06:00:aa:17:40:ee:fd:c4:2a:c5:f6:78:f8:88:47:8d:
        0e:09:aa:a5:ac:d2:f0:5c:76:23:a5:27:9e:71:bc:3e:b7:16:
        52:f7:52:89:4e:d0:2f:aa:48:36:c2:83:a9:7a:8d:78:53:3f:
        d6:68:27:3e:a1:fe:67:d1:88:0a:36:8d:b7:f8:fc:8f:0e:66:
        23:4c:3a:79:47:35:78:24:a9:a3:74:0f:02:76:d3:45:40:c9:
        75:4c:56:38:e0:71:e9:5e:56:28:8d:e3:47:7b:15:77:4f:21:
        49:79:3a:ca:da:cf:b9:76:79:9b:dd:a3:a2:94:2a:25:4a:cc:
        ff:86:97:5e:70:e4:03:30:ac:46:91:75:59:da:42:0b:6e:6f:
        af:b0:af:f5:40:fd:db:2a:d1:11:34:45:ae:35:a0:80:6f:91:
        0b:86:1a:63:06:6f:93:67:4a:4d:e9:19:6e:68:74:d9:ec:0c:
        87:2e:81:e2:cf:98:f7:fc:a1:63:e5:05:30:e6:f1:10:44:7b:
        ff:67:ce:9d:ba:ef:a6:29:c6:d9:f8:17:fc:f9:a0:07:76:51:
        85:0c:f1:22:f3:50:2a:57:87:1c:4a:57:9d:73:12:d7:d3:fb:
        11:44:d3:42:cb:3c:75:61:43:89:ad:9b:0d:19:50:1c:c7:c2:
        ed:82:5c:08:ce:14:b7:87:f6:e2:c0:91:da:b0:f5:fd:62:fe:
        d2:40:50:49:73:cd:41:7d:48:52:c5:9e:db:3a:95:e3:2d:5f:
        cc:51:e9:00:37:e4:ba:d6:d5:a0:06:61:a7:fc:14:a0:d4:23:
        22:5f:1b:a0:bf:b0:39:fb:2f:3d:16:eb:c7:d0:44:54:88:1e:
        c0:ff:87:88:04:10:98:70:34:38:31:13:d5:a6:f1:17:c0:5c:
        9b:3b:ce:b9:d6:e0:f3:c3:e2:83:f9:77:c1:91:c7:63:fe:e2:
        70:8e:b8:6e:d9:60:2c:20
```
This also tells them where to get the CA certificate that they can use to verify our certificate was indeed signed by the CA. Visit https://cacert.actalis.it/certs/actalis-autclig3 and `cacertificate_actalis_autclientg3.cer` (DER format, binary encoding) will be downloaded. Alternatively, visit https://www.actalis.it/documenti-it/actalisclientauthenticationcag3.aspx and `Certificato della SubCA G3.cer` (PEM format, base64 encoded and delimited by `-----BEGIN/END CERTIFICATE-----`) will be downloaded.

The formats can be interconverted pretty easily:
```
openssl x509 -inform DER -outform PEM -in cacertificate_actalis_autclientg3.cer -out autclientg3.cer
```
Now the person you're communicating with can use the PEM encoded CA certifcate (which contains the CA public key) to verify that your certificate was indeed signed by the CA.
```
$ openssl verify -CAfile 'Certificato della SubCA G3.cer' my_cert.crt
my_cert.crt: OK
```
They can get your public key from the certificate by running
```
$ openssl x509 -in my_cert.crt -pubkey -noout
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqzOc/2gbiBGh7MuOcnfm
rJRLOlbE94izOGBSmIMe+O4sWWZiALMKM0BDcJ4WVO9U84Ea/NSZeJEXBA7h+mEk
Kp9mYlAMufSZ3e4R77csyosAibxevkUa6eTjVpHqI3J80Oo24J/tWdwp+06TA5rC
qILRaRQdC4uSvr30Hq1WyV/2KLwiGLW897blA50qeKj86XxjJXOjvwovpOGSI8So
YMx3HaC4q9vxB1KywQ0Ca4x3+7lkBKVfa2zufB/KwuJsMdZ8gAiRDUYDkcU0Mka8
d+GS972Rj7WSjcmDwBFMNFHYAEQf83mD8OowjdJyazKtAzVD7vTtfWeEkoZ1D2YX
OQIDAQAB
-----END PUBLIC KEY-----
```
