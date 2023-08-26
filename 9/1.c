#include <string.h>
#include "unistd.h"
#include <openssl/evp.h>

int main(int argc, char** argv) {
  char data[8];
  char salt[8];
  read(0, data, 8);
  read(0, salt, 8);
  // Создание контекста
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  EVP_CIPHER_CTX_init(ctx);
  unsigned char key[EVP_CIPHER_key_length(EVP_aes_256_cbc())];
  unsigned char iv[EVP_CIPHER_iv_length(EVP_aes_256_cbc())];

  // Начальная стадия: инициализация
  EVP_BytesToKey(
      EVP_aes_256_cbc(),    // алгоритм шифрования
      EVP_sha256(),         // алгоритм хеширования пароля
      salt,                 // соль
      argv[1], strlen(argv[1]), // пароль
      1,                    // количество итераций хеширования
      key,          // результат: ключ нужной длины
      iv            // результат: начальный вектор нужной длины
  );

  // Генерация ключа и начального вектора из
  // пароля произвольной длины и 8-байтной соли
  EVP_DecryptInit(
      ctx,                  // контекст для хранения состояния
      EVP_aes_256_cbc(),    // алгоритм шифрования
      key,                  // ключ нужного размера
      iv                    // начальное значение нужного размера
  );

  char inp[2048];
  char decrypt[4096] = {};
  int decrypted_len;
  int also_reads = 2048;

  while (also_reads == 2048) {
    also_reads = read(0, inp, 2048);
    EVP_DecryptUpdate(
        ctx,
        decrypt,
        &decrypted_len,
        inp,
        also_reads
        );
    write(1, decrypt, decrypted_len);
  }

  EVP_DecryptFinal(
      ctx,
      decrypt,
      &decrypted_len
      );
  write(1, decrypt, decrypted_len);
  EVP_CIPHER_CTX_cleanup(ctx);
}
