echo off
rem generate a private key for a curve
openssl ecparam -name prime256v1 -genkey -noout -out private-key.pem

rem generate corresponding public key
openssl ec -in private-key.pem -pubout -out public-key.pem
