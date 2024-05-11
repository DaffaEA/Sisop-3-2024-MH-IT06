# Sisop-1-2024-MH-IT06
# Laporan Resmi Praktikum Sistem Operasi Modul 1

Kelompok IT-06:

-Daffa Rajendra Priyatama 5027231009

-Nicholas Arya Krisnugroho Rerangin 5027231058

-Johanes Edward Nathanael 5027231067

# Soal 1

## Isi Soal
Pada zaman dahulu pada galaksi yang jauh-jauh sekali, hiduplah seorang Stelle. Stelle adalah seseorang yang sangat tertarik dengan Tempat Sampah dan Parkiran Luar Angkasa. Stelle memulai untuk mencari Tempat Sampah dan Parkiran yang terbaik di angkasa. Dia memerlukan program untuk bisa secara otomatis mengetahui Tempat Sampah dan Parkiran dengan rating terbaik di angkasa. Programnya berbentuk microservice sebagai berikut:

- Dalam auth.c pastikan file yang masuk ke folder new-entry adalah file csv dan berakhiran         trashcan dan parkinglot. Jika bukan, program akan secara langsung akan delete file tersebut.     Contoh dari nama file yang akan diautentikasi:

    - belobog_trashcan.csv
    - osaka_parkinglot.csv
    
- Format data (Kolom)  yang berada dalam file csv adalah seperti berikut:

![belobog1](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/d1e867a7-3636-400f-b17c-784a10d7d315)

atau

![osaka1](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/18ed1f5e-0ed8-4556-82d7-263e3d1a9d37)

- File csv yang lolos tahap autentikasi akan dikirim ke shared memory. 

-  Dalam rate.c, proses akan mengambil data csv dari shared memory dan akan memberikan output       Tempat Sampah dan Parkiran dengan Rating Terbaik dari data tersebut.

![output1](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/efc83c2a-6d10-4d18-811b-28c30a5c9c03)

- Pada db.c, proses bisa memindahkan file dari new-data ke folder microservices/database, WAJIB MENGGUNAKAN SHARED MEMORY.

- log semua file yang masuk ke folder microservices/database ke dalam file db.log dengan contoh format sebagai berikut:
[DD/MM/YY hh:mm:ss] [type] [filename]
ex : `[07/04/2024 08:34:50] [Trash Can] [belobog_trashcan.csv]

Contoh direktori awal:
```
├── auth.c
├── microservices
│   ├── database
│   │   └── db.log
│   ├── db.c
│   └── rate.c
└── new-data
    ├── belobog_trashcan.csv
    ├── ikn.csv
    └── osaka_parkinglot.csv
```
Contoh direktori akhir setelah dijalankan auth.c dan db.c:
```
├── auth.c
├── microservices
│   ├── database
│   │   ├── belobog_trashcan.csv
│   │   ├── db.log
│   │   └── osaka_parkinglot.csv
│   ├── db.c
│   └── rate.c
└── new-data
```

## Penyelesaian
### Auth.c

```c
void movetoshared(const char* filename, key_t key){
    FILE *file = fopen(filename, "r");
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    int shmid = shmget(key, file_size, IPC_CREAT | 0666);

    char *shm_ptr = (char *)shmat(shmid, NULL, 0);

    size_t bytes_read = fread(shm_ptr, 1, file_size, file);

    shmdt(shm_ptr);
    fclose(file);
}
```
- Membuka file dengan fopen
- Mendapatkan ukuran dari file dengan fseek dan ftell
- Mengembalikan pointer di file dengan rewind
- Membuat shared memory dengan shmget dan shmptr untuk alokasi shared memory
- Membaca file dan dipindah ke shared memory dengan fread
- menutup shared memory dan menutup file

```c

void movenametoshared(const char* filename, key_t key){
    int shmid = shmget(key, strlen(filename) + 1, IPC_CREAT | 0666);
    char *shm_ptr = (char *)shmat(shmid, NULL, 0);

    strcpy(shm_ptr, filename);

    shmdt(shm_ptr);
}
```
- Membuat shared memory dengan shmget dan shmptr untuk alokasi shared memory
- Menyalin filename ke dalam shared memory
- menutup shared memory

```c
int main() {
    char dir_path[100];
    getcwd(dir_path, sizeof(dir_path));
    strcat(dir_path, "/new-data");

    DIR *dir = opendir(dir_path);
```
- Mengambil path directory executable dengan getcwd
- Menggunakan strcat untuk menambah "/newdata" pada directory path
- Membuka directory path

```c
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name + strlen(entry->d_name) - 14, "parkinglot.csv") == 0) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
             movenametoshared(filepath, 1233);
            movetoshared(filepath, 1234);
        } else if (strcmp(entry->d_name + strlen(entry->d_name) - 12, "trashcan.csv") == 0) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
            movenametoshared(filepath, 1235);
            movetoshared(filepath, 1236);
        } else {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
            remove(filepath);
        }
    }
    closedir(dir);

    return 0;
}
```
- Membuat program berjalan berulang sampai semua file di directory dieksekusi
- Apabila akhir dari nama file adalah "parkinglot.csv" atau "trashcan.csv"
  - menggunakan snprintf untuk membuat filepath lengkap dengan menambah nama file ke directory path
  - Memindah filepath ke shared memory dengan fungsi movenametoshared
  - Memindah isi dari file ke shared memory dengan fungsi movetoshared
- Apabila tidak
  -  menggunakan snprintf untuk membuat filepath lengkap dengan menambah nama file ke directory path
  -  menghapus file dengan fungsi remove
-Menutup file

 ### Rate.c
```c
const char *get_filename(const char *path) {
    const char *filename = strrchr(path, '/');
    if (filename == NULL)
        return path;
    else
        return filename + 1;
}
```
- Mencari kemunculan terakhir char "/" dengan stchr dan disimpan dalam filename
- Apabila tidak menemukan return seperti asalnya
- Apabila menemukan mengambil filename + 1 untuk menghapus "/" dalam filename

```c
void readFromSharedMemory(key_t key) {
    int shmid = shmget(key, 0, 0666);
    char *shm_ptr = (char *)shmat(shmid, NULL, 0);
    
    const char *filename = NULL;
    if (key == 1233) {
        filename = get_filename(shm_ptr);
        printf("Type: Parking Lot\nFilename: %s\n", filename);
    } else if (key == 1235) {
        filename = get_filename(shm_ptr);
        printf("Type: Trash Can\nFilename: %s\n", filename);
    }
    
    printf("------------------------------------\n");

    shmdt(shm_ptr);
}
```
 - Mengambil identifikasi shared memory (shmid) menggunakan fungsi shmget() dengan kunci (key) yang diberikan.
 - Menempelkan (attach) shared memory ke ruang alamat proses menggunakan shmat() dan mendapatkan pointer ke shared memory tersebut.
 - Memeriksa nilai kunci yang diberikan. Jika kunci adalah 1233, itu menganggap data di shared memory sebagai "Parking Lot". Jika kunci adalah 1235, itu menganggap data sebagai data "Trash Can". Ini dilakukan dengan memanggil fungsi get_filename() untuk mendapatkan nama file dari data tersebut.
 - Mencetak informasi yang diperoleh, yaitu jenis ("Type") dan nama file ("Filename"), sesuai dengan jenis data yang dikenali.
 - Melepaskan (detach) shared memory dari ruang alamat proses menggunakan shmdt().

```c
void find_max_rating(key_t key) {
    float max_rating = -1;
    char max_name[100] = "";
    
    int shmid = shmget(key, 0, 0666);
    char *shm_ptr = (char *)shmat(shmid, NULL, 0);
    char *data = strdup(shm_ptr);

    char *line = strtok(data, "\n");
```
- Membuat variabel max_rating yang diinisialisasi dengan nilai -1 dan max_name yang diinisialisasi dengan string kosong. Ini akan digunakan untuk menyimpan nilai tertinggi dan nama yang terkait.
- Mengambil identifikasi shared memory (shmid) menggunakan fungsi shmget() dengan kunci (key) yang diberikan.
- Menempelkan (attach) shared memory ke ruang alamat proses menggunakan shmat() dan mendapatkan pointer ke shared memory tersebut.
- Membuat salinan dari data yang ada di shared memory ke dalam variabel data menggunakan fungsi strdup(). Ini dilakukan untuk mempermudah pemrosesan data tanpa mengubah data yang ada di shared memory.
- Menggunakan fungsi strtok() untuk membagi data menjadi baris-baris terpisah. Setiap baris kemudian diperiksa untuk mencari nilai rating.

```c
   while (line != NULL) {
        char name[100];
        float rating;
        sscanf(line, "%[^,], %f", name, &rating);

        if (rating > max_rating) {
            max_rating = rating;
            strcpy(max_name, name);
        }

        line = strtok(NULL, "\n");
    }

    free(data);
    shmdt(shm_ptr);

    printf("Name: %s\nRating: %.1f\n", max_name, max_rating);
}
```
- Melakukan looping melalui setiap baris dari suatu data yang diwakili oleh variabel line. Loop ini berjalan hingga line tidak lagi menunjuk ke baris data (yaitu line menjadi NULL).
- sscanf digunakan untuk memformat data dari line. Data tersebut adalah nama (name) dan nilai rating (rating). Fungsi sscanf membaca data dari line sesuai dengan pola yang diberikan dalam argumen kedua, kemudian menyimpan hasilnya di variabel name dan rating.
- membandingkan nilai rating (rating) dengan nilai maksimum yang sudah ada (max_rating). Jika nilai rating saat ini lebih besar dari nilai maksimum yang ada, maka nilai maksimum (max_rating) diperbarui dengan nilai rating saat ini, dan nama yang sesuai (name) disalin ke dalam variabel max_name.
- memperbarui pointer line untuk menunjuk ke baris data berikutnya. Ini dilakukan dengan menggunakan fungsi strtok dengan argumen NULL untuk melanjutkan pencarian dari tempat terakhir kali dipanggil, dan dengan menggunakan pemisah baris baru (\n).
- program membebaskan memori yang dialokasikan untuk variabel data menggunakan fungsi free. Juga, program melepaskan memori bersama (shared memory) menggunakan shmdt dengan melepaskan pointer ke memori bersama (shm_ptr).
- mencetak nama (max_name) dan nilai rating maksimum (max_rating) ke layar.

```c
int main() {
    readFromSharedMemory(1233);
    find_max_rating(1234);
    printf("\n\n");
    readFromSharedMemory(1235);
    find_max_rating(1236);
    
    return 0;
}
```
- Memanggil fungsi diatas dengan key shared memory yang sesuai.

### db.c
```c
const char *get_filename(const char **path) {
    const char *filename = strrchr(*path, '/');
    if (filename == NULL)
        return *path;
    else
        return filename + 1;
}
```
- Fungsi sama seperti yang ada di rate.c

```c
char *read_shared_memory(key_t key) {
    int shmid = shmget(key, 0, 0666);
    char *shm_ptr = (char *)shmat(shmid, NULL, 0);
    char *string = strdup(shm_ptr);
    shmdt(shm_ptr);
    return string;
}
```
- Mencari ID dari shared memory dengan kunci (key) yang diberikan dengan shmget.
- Menghubungkan program ke shared memory dengan ID yang ditemukan sebelumnya, mengembalikan pointer ke area shared memory dengan shmat.
- Membuat salinan dari string yang ada di shared memory menggunakan fungsi strdup, sehingga hasilnya adalah alamat memori yang baru dialokasikan untuk string tersebut strdup.
- Memutuskan hubungan program dengan shared memory dengan shmdt.
- Mengembalikan pointer ke string yang telah disalin dari shared memory.

```c
void updatePath(char **path) {
    char *token;
    const char search[] = "new-data";
    const char replacement[] = "microservices/database";

    token = strstr(*path, search);

    if (token != NULL) {
        size_t length_before = token - *path;
        size_t length_after = strlen(token + strlen(search));
        char *new_path = (char *)malloc(length_before + strlen(replacement) + length_after + 1);
        strncpy(new_path, *path, length_before);
        new_path[length_before] = '\0';
        strcat(new_path, replacement);
        strcat(new_path, token + strlen(search));
        free(*path);
        *path = new_path;
    }
}
```
- Fungsi ini menggunakan strstr untuk mencari kemunculan pertama dari substring yang dicari (search) dalam string path.
- Jika substring yang dicari ditemukan (token != NULL), langkah-langkah perbaruan dilakukan.
- Panjang bagian sebelum dan sesudah substring yang dicari dihitung untuk mengalokasikan memori yang tepat untuk string hasil.
- Memori baru dialokasikan dengan ukuran yang sesuai untuk menyimpan string hasil.
- Bagian string sebelum substring yang dicari disalin ke string hasil.
- Substring pengganti ditambahkan ke string hasil.
- Bagian string setelah substring yang dicari disalin ke string hasil.
- Memori yang dialokasikan sebelumnya untuk string path dilepaskan.
- Pointer path diperbarui dengan alamat memori baru yang berisi string hasil.

```c
int main() {
    char *string_1 = read_shared_memory(1233);
    char *oldstring_1 = read_shared_memory(1233);
    updatePath(&string_1);
    if (rename(oldstring_1, string_1) == 0) {
        const char *basename = get_filename((const char **)&oldstring_1);
        FILE *log_file = fopen("database/db.log", "a");
        if (log_file != NULL) {
            time_t raw_time;
            struct tm *time_info;
            time(&raw_time);
            time_info = localtime(&raw_time);
            fprintf(log_file, "[%02d/%02d/%04d %02d:%02d:%02d] [Parking Lot] [%s]\n",
                    time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900,
                    time_info->tm_hour, time_info->tm_min, time_info->tm_sec, basename);
            fclose(log_file);
        }
    }
    free(oldstring_1);

    char *string_2 = read_shared_memory(1235);
    char *oldstring_2 = read_shared_memory(1235);
    updatePath(&string_2);
    if (rename(oldstring_2, string_2) == 0) {
        const char *basename = get_filename((const char **)&oldstring_2);
        FILE *log_file = fopen("database/db.log", "a");
        if (log_file != NULL) {
            time_t raw_time;
            struct tm *time_info;
            time(&raw_time);
            time_info = localtime(&raw_time);
            fprintf(log_file, "[%02d/%02d/%04d %02d:%02d:%02d] [Trash Can] [%s]\n",
                    time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900,
                    time_info->tm_hour, time_info->tm_min, time_info->tm_sec, basename);
            fclose(log_file);
        }
    }
    free(oldstring_2);

    return 0;
}
```
- Fungsi untuk membaca string dari shared memory dengan ID 1233 dan 1235, masing-masing disimpan dalam pointer string_1, string_2, oldstring_1 dan oldstring_2 dengan fungsi read_shared_memory.
- Memperbarui path yang disimpan dalam string_1 dan string_2 dengan menggunakan fungsi updatePath.
- Mencoba untuk mengubah nama file dari oldstring_1 menjadi string_1, dan dari oldstring_2 menjadi string_2. Jika berhasil (return value 0), ini menunjukkan bahwa file telah berhasil direname dengan fungsi rename.
- Jika rename berhasil, program membuka file log db.log dalam mode append, kemudian menuliskan pesan log yang mencatat waktu (tanggal dan jam), jenis aktivitas (Parking Lot atau Trash Can), dan nama file (basename) yang terlibat dalam operasi.
- memori yang dialokasikan untuk oldstring_1 dan oldstring_2 dibebaskan menggunakan free.
  
## Output

### Auth.c
#### - Before
![Screenshot 2024-05-11 115209](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/f2644e24-a7a4-46d5-b165-38054e436353)
#### - After
![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/c71678e0-d262-4294-8c5f-360643fa7138)

### Rate.c
![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/c2bed9f3-191e-4f39-af61-a35dabe44bee)

### db.c
![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/45092369-b6c1-41c8-b824-db8aaf5a0038)
![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/7c337e44-c1a2-4ec3-bef6-f64444587697)

# Soal 2
Max Verstappen 🏎️ seorang pembalap F1 dan programer memiliki seorang adik bernama Min Verstappen (masih SD) sedang menghadapi tahap paling kelam dalam kehidupan yaitu perkalian matematika, Min meminta bantuan Max untuk membuat kalkulator perkalian sederhana (satu sampai sembilan). Sembari Max nguli dia menyuruh Min untuk belajar perkalian dari web (referensi) agar tidak bergantung pada kalkulator.
(Wajib menerapkan konsep pipes dan fork seperti yang dijelaskan di modul Sisop. Gunakan 2 pipes dengan diagram seperti di modul 3).
Sesuai request dari adiknya Max ingin nama programnya dudududu.c. Sebelum program parent process dan child process, ada input dari user berupa 2 string. Contoh input: tiga tujuh. 
Pada parent process, program akan mengubah input menjadi angka dan melakukan perkalian dari angka yang telah diubah. Contoh: tiga tujuh menjadi 21. 
Pada child process, program akan mengubah hasil angka yang telah diperoleh dari parent process menjadi kalimat. Contoh: `21` menjadi “dua puluh satu”.
Max ingin membuat program kalkulator dapat melakukan penjumlahan, pengurangan, dan pembagian, maka pada program buatlah argumen untuk menjalankan program : 
```
   i. perkalian	: ./kalkulator -kali
   ii. penjumlahan	: ./kalkulator -tambah
   iii. pengurangan	: ./kalkulator -kurang
   iv. pembagian	: ./kalkulator -bagi
```

Beberapa hari kemudian karena Max terpaksa keluar dari Australian Grand Prix 2024 membuat Max tidak bersemangat untuk melanjutkan programnya sehingga kalkulator yang dibuatnya cuma menampilkan hasil positif jika bernilai negatif maka program akan print “ERROR” serta cuma menampilkan bilangan bulat jika ada bilangan desimal maka dibulatkan ke bawah.

Setelah diberi semangat, Max pun melanjutkan programnya dia ingin (pada child process) kalimat akan di print dengan contoh format : 
i. perkalian	: “hasil perkalian tiga dan tujuh adalah dua puluh satu.”
ii. penjumlahan	: “hasil penjumlahan tiga dan tujuh adalah sepuluh.”
iii. pengurangan	: “hasil pengurangan tujuh dan tiga adalah empat.”
iv. pembagian	: “hasil pembagian tujuh dan tiga adalah dua.”

Max ingin hasil dari setiap perhitungan dicatat dalam sebuah log yang diberi nama histori.log. Pada parent process, lakukan pembuatan file log berdasarkan data yang dikirim dari child process. 
```
Format: [date] [type] [message]
Type: KALI, TAMBAH, KURANG, BAGI
Ex:
1. [10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.
2. [10/03/24 00:30:00] [TAMBAH] sembilan tambah sepuluh sama dengan sembilan belas.
3. [10/03/24 00:30:12] [KURANG] ERROR pada pengurangan.
```


### Pengerjaan
### dudududu.c (revisi)
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <math.h>

// Function to convert words to numbers
int wordsToNumbers(char *word) {
    const char *words[] = {"", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    for (int i = 1; i <= 9; i++) {
        if (strcmp(word, words[i]) == 0) {
            return i;
        }
    }
    return -1; // Return -1 for unknown words
}

// Function to convert numbers to words
char *intToWords(int num) {
    const char *satuan[] = {"", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    const char *belasan[] = {"", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
    const char *puluh[] = {"", "sepuluh", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};

    char *result = (char *)malloc(100 * sizeof(char));
    strcpy(result, "");

    if (num >= 100) {
        strcat(result, satuan[num / 100]);
        strcat(result, " ratus ");
        num %= 100;
    }

    if (num >= 20) {
        strcat(result, puluh[num / 10]);
        strcat(result, " ");
        num %= 10;
    }

    if (num >= 11 && num <= 19) {
        strcat(result, belasan[num - 10]);
    } else if (num >= 1 && num <= 9) {
        strcat(result, satuan[num]);
    }

    return result;
}

// Function to write to log file
void writeToLog(const char *operation, int num1, int num2, int result) {
    FILE *logFile = fopen("histori.log", "a");
    if (logFile != NULL) {
        time_t rawtime;
        struct tm *info;
        char buffer[80];

        time(&rawtime);
        info = localtime(&rawtime);
        strftime(buffer, 80, "%d/%m/%y %H:%M:%S", info);

        char upperOperation[10];
        int i;
        for (i = 0; operation[i]; i++) {
            upperOperation[i] = toupper(operation[i]);
        }
        upperOperation[i] = '\0';

        if (result < 0) {
            fprintf(logFile, "[%s] [%s] ERROR pada %s.\n", buffer, upperOperation, upperOperation);
        } else {
            fprintf(logFile, "[%s] [%s] %s %s %s sama dengan %s.\n", buffer, upperOperation, intToWords(num1), operation, intToWords(num2), intToWords(result));
        }
        fclose(logFile);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <operation>\n", argv[0]);
        return 1;
    }

    char *operation = argv[1];
    char operand1[20], operand2[20];

    printf("Masukkan angka pertama: ");
    scanf("%s", operand1);
    printf("Masukkan angka kedua: ");
    scanf("%s", operand2);

    int num1 = wordsToNumbers(operand1);
    int num2 = wordsToNumbers(operand2);

    if (num1 == -1 || num2 == -1) {
        printf("Kata tidak valid.\n");
        return 1;
    }

    int result = 0;

    if (strcmp(operation, "-kali") == 0) {
        result = num1 * num2;
    } else if (strcmp(operation, "-tambah") == 0) {
        result = num1 + num2;
    } else if (strcmp(operation, "-kurang") == 0) {
        result = num1 - num2;
        if (result < 0) {
            printf("ERROR: Hasil negatif.\n");
            return 1;
        }
    } else if (strcmp(operation, "-bagi") == 0) {
        if (num2 == 0) {
            printf("ERROR: Pembagian oleh nol.\n");
            return 1;
        }
        result = num1 / num2;
    } else {
        printf("Operasi tidak valid.\n");
        return 1;
    }

    printf("Hasil %s %s dan %s adalah %s.\n", operation + 1, operand1, operand2, intToWords(result));

    writeToLog(operation + 1, num1, num2, result);

    return 0;
}
```
```c
const char *words[] = {"", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    for (int i = 1; i <= 9; i++) {
        if (strcmp(word, words[i]) == 0) {
            return i;
        }
    }
    return -1; // Return -1 for unknown words
}

// Function to convert numbers to words
char *intToWords(int num) {
    const char *satuan[] = {"", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    const char *belasan[] = {"", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
    const char *puluh[] = {"", "sepuluh", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};

```
- Array words digunakan untuk menyimpan kata-kata dalam bahasa Indonesia yang mewakili angka-angka dari 0 hingga 9.

- Melalui loop for, setiap kata dalam array words diperiksa satu per satu untuk mencocokkan kata yang diberikan sebagai argumen fungsi wordsToNumbers().

- Jika kata cocok, indeks kata tersebut dikembalikan sebagai hasil konversi. Jika tidak, fungsi mengembalikan -1 untuk menunjukkan bahwa kata tersebut tidak dikenali sebagai angka.

- Fungsi intToWords() mengonversi angka menjadi kata-kata dalam bahasa Indonesia. Ini dilakukan dengan memeriksa setiap digit angka dan menyusun kata-kata yang sesuai dari tiga array: satuan, belasan, dan puluh.

- Proses konversi dimulai dengan memeriksa apakah angka tersebut lebih besar dari atau sama dengan 100 dan kemudian dilanjutkan dengan memeriksa puluhan dan satuan.
- 
- Hasil konversi kata-kata dikembalikan sebagai hasil fungsi.

- Fungsi writeToLog() digunakan untuk menulis operasi dan hasilnya ke file log.

- Pada main(), pengguna diminta untuk memasukkan operasi (tambah, kurang, kali, atau bagi) dan dua angka dalam kata-kata.

- Angka-angka tersebut dikonversi menjadi nilai numerik menggunakan fungsi wordsToNumbers() dan kemudian operasi perhitungan dilakukan.

- Hasil operasi ditampilkan dalam kata-kata dan juga ditulis ke file log menggunakan fungsi writeToLog().
  
```c
// Function to write to log file
void writeToLog(const char *operation, int num1, int num2, int result) {
    FILE *logFile = fopen("histori.log", "a");
    if (logFile != NULL) {
        time_t rawtime;
        struct tm *info;
        char buffer[80];

        time(&rawtime);
        info = localtime(&rawtime);
        strftime(buffer, 80, "%d/%m/%y %H:%M:%S", info);

        char upperOperation[10];
        int i;
        for (i = 0; operation[i]; i++) {
            upperOperation[i] = toupper(operation[i]);
        }
        upperOperation[i] = '\0';

        if (result < 0) {
            fprintf(logFile, "[%s] [%s] ERROR pada %s.\n", buffer, upperOperation, upperOperation);
        } else {
            fprintf(logFile, "[%s] [%s] %s %s %s sama dengan %s.\n", buffer, upperOperation, intToWords(num1), operation, intToWords(num2), intToWords(result));
        }
        fclose(logFile);
    }
}
```

-Fungsi membuka file log "histori.log" dalam mode append.

-Memeriksa keberhasilan pembukaan file log.

-Mendapatkan waktu lokal saat ini dan mengonversinya menjadi format yang sesuai.

-Mengonversi operasi matematika menjadi huruf kapital.

-Jika hasil operasi matematika negatif, menulis pesan kesalahan ke dalam file log.

-Jika hasil operasi matematika tidak negatif, menulis operasi dan hasilnya (dalam kata-kata bahasa Indonesia) ke dalam file log.

-Menutup file log setelah selesai menulis.

```c
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <operation>\n", argv[0]);
        return 1;
    }

    char *operation = argv[1];
    char operand1[20], operand2[20];

    printf("Masukkan angka pertama: ");
    scanf("%s", operand1);
    printf("Masukkan angka kedua: ");
    scanf("%s", operand2);

    int num1 = wordsToNumbers(operand1);
    int num2 = wordsToNumbers(operand2);

    if (num1 == -1 || num2 == -1) {
        printf("Kata tidak valid.\n");
        return 1;
    }

    int result = 0;

    if (strcmp(operation, "-kali") == 0) {
        result = num1 * num2;
    } else if (strcmp(operation, "-tambah") == 0) {
        result = num1 + num2;
    } else if (strcmp(operation, "-kurang") == 0) {
        result = num1 - num2;
        if (result < 0) {
            printf("ERROR: Hasil negatif.\n");
            return 1;
        }
    } else if (strcmp(operation, "-bagi") == 0) {
        if (num2 == 0) {
            printf("ERROR: Pembagian oleh nol.\n");
            return 1;
        }
        result = num1 / num2;
    } else {
        printf("Operasi tidak valid.\n");
        return 1;
    }

    printf("Hasil %s %s dan %s adalah %s.\n", operation + 1, operand1, operand2, intToWords(result));

    writeToLog(operation + 1, num1, num2, result);

    return 0;
}
```

-Fungsi main memiliki dua argumen: argc yang merupakan jumlah argumen baris perintah, dan argv yang merupakan array dari string-string argumen baris perintah.

-Pertama-tama, fungsi memeriksa apakah jumlah argumen yang diberikan tepat 2. Jika tidak, maka pesan penggunaan program dicetak, dan program mengembalikan nilai 1, menandakan bahwa ada kesalahan dalam penggunaan.

-Argumen operasi (tambah, kurang, kali, atau bagi) disimpan dalam variabel operation.

-Variabel operand1 dan operand2 dideklarasikan untuk menyimpan kata-kata yang akan dikonversi menjadi angka.

-Pengguna diminta untuk memasukkan dua angka dalam bentuk kata-kata menggunakan scanf.

-Angka-angka tersebut dikonversi menjadi nilai numerik menggunakan fungsi wordsToNumbers.

-Jika salah satu angka tidak valid (hasil konversi adalah -1), program mencetak pesan kesalahan bahwa kata tidak valid dan mengembalikan nilai 1.

-Jika kedua angka valid, dilakukan operasi matematika sesuai dengan argumen operasi yang diberikan.

-Jika operasi adalah pengurangan (-kurang), dilakukan pemeriksaan apakah hasilnya negatif. Jika ya, program mencetak pesan kesalahan dan mengembalikan nilai 1.

-Jika operasi adalah pembagian (-bagi), dilakukan pemeriksaan apakah pembaginya adalah nol. Jika ya, program mencetak pesan kesalahan dan mengembalikan nilai 1.

-Hasil operasi dan argumen operasi yang dimodifikasi (tanpa tanda minus) dicetak ke layar.

-Hasil operasi, argumen operasi, dan angka-angka yang dioperasikan dicatat dalam file log menggunakan fungsi writeToLog.

-Program mengembalikan nilai 0, menunjukkan bahwa tidak ada kesalahan yang terjadi dalam eksekusi program.

### Dokumentasi Setelah Revisi 
![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/142997842/39f38c7d-a4bc-4491-9771-34cce1d632cf)

![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/142997842/e3b05026-bf78-4095-ad0d-7f40e43a530e)

# Soal 3

Shall Leglerg🥶 dan Carloss Signs 😎 adalah seorang pembalap F1 untuk tim Ferrari 🥵. Mobil F1 memiliki banyak pengaturan, seperti penghematan ERS, Fuel, Tire Wear dan lainnya. Pada minggu ini ada race di sirkuit Silverstone. Malangnya, seluruh tim Ferrari diracun oleh Super Max Max pada hari sabtu sehingga seluruh kru tim Ferrari tidak bisa membantu Shall Leglerg🥶 dan Carloss Signs 😎 dalam race. Namun, kru Ferrari telah menyiapkan program yang bisa membantu mereka dalam menyelesaikan race secara optimal. Program yang dibuat bisa mengatur pengaturan - pengaturan dalam mobil F1 yang digunakan dalam balapan. Programnya ber ketentuan sebagai berikut:
Pada program actions.c, program akan berisi function function yang bisa di call oleh paddock.c
Action berisikan sebagai berikut:
- Gap [Jarak dengan driver di depan (float)]: Jika Jarak antara Driver dengan Musuh di depan adalah < 3.5s maka return Gogogo, jika jarak > 3.5s dan 10s return Push, dan jika jarak > 10s maka return Stay out of trouble.
- Fuel [Sisa Bensin% (string/int/float)]: Jika bensin lebih dari 80% maka return Push Push Push, jika bensin di antara 50% dan 80% maka return You can go, dan jika bensin kurang dari 50% return Conserve Fuel.
- Tire [Sisa Ban (int)]: Jika pemakaian ban lebih dari 80 maka return Go Push Go Push, jika pemakaian ban diantara 50 dan 80 return Good Tire Wear, jika pemakaian di antara 30 dan 50 return Conserve Your Tire, dan jika pemakaian ban kurang dari 30 maka return Box Box Box.
Tire Change [Tipe ban saat ini (string)]: Jika tipe ban adalah Soft return Mediums Ready, dan jika tipe ban Medium return Box for Softs.

```
Contoh:
[Driver] : [Fuel] [55%]
[Paddock]: [You can go]

```
Pada paddock.c program berjalan secara daemon di background, bisa terhubung dengan driver.c melalui socket RPC.
Program paddock.c dapat call function yang berada di dalam actions.c.
Program paddock.c tidak keluar/terminate saat terjadi error dan akan log semua percakapan antara paddock.c dan driver.c di dalam file race.log

Format log:
[Source] [DD/MM/YY hh:mm:ss]: [Command] [Additional-info]
ex :
[Driver] [07/04/2024 08:34:50]: [Fuel] [55%]
[Paddock] [07/04/2024 08:34:51]: [Fuel] [You can go]

Program driver.c bisa terhubung dengan paddock.c dan bisa mengirimkan pesan dan menerima pesan serta menampilan pesan tersebut dari paddock.c sesuai dengan perintah atau function call yang diberikan.
Jika bisa digunakan antar device/os (non local) akan diberi nilai tambahan.
untuk mengaktifkan RPC call dari driver.c, bisa digunakan in-program CLI atau Argv (bebas) yang penting bisa send command seperti poin B dan menampilkan balasan dari paddock.c

```		
ex:
Argv: 
./driver -c Fuel -i 55% 
in-program CLI:
Command: Fuel
Info: 55%
```

```
Contoh direktori 😶‍🌫️:
.		.
├── client
│   └── driver.c
└── server
    ├── actions.c
    ├── paddock.c
    └── race.log
```

## Penyelesaian

### Action.c

```c
char* gap(float jarak) {
    if (jarak < 3.5)
        return "Gogogo";
    else if (jarak >= 3.5 && jarak <= 10)
        return "Push";
    else
        return "Stay out of trouble";
}
```
char* gap(float jarak): Fungsi ini mengambil jarak sebagai parameter dalam kilometer dan memberikan saran kepada pengemudi berdasarkan jarak tersebut. Jika jarak kurang dari 3.5 km, saran yang diberikan adalah "Gogogo". Jika jarak antara 3.5 km dan 10 km, saran adalah "Push". Jika jarak lebih dari 10 km, saran adalah "Stay out of trouble".

```c
char* fuel(int talit) {
    if (talit > 80)
        return "Push Push Push";
    else if (talit >= 50 && talit <= 80)
        return "You can go";
    else
        return "Conserve Fuel";
}
```
char* fuel(int talit): Fungsi ini mengambil tingkat bahan bakar sebagai parameter (dalam persentase) dan memberikan saran kepada pengemudi berdasarkan tingkat bahan bakar tersebut. Jika tingkat bahan bakar lebih dari 80%, saran yang diberikan adalah "Push Push Push". Jika tingkat bahan bakar antara 50% dan 80%, saran adalah "You can go". Jika tingkat bahan bakar kurang dari atau sama dengan 50%, saran adalah "Conserve Fuel".

```c
char* tire(int kanisir) {
    if (kanisir > 80)
        return "Go Push Go Push";
    else if (kanisir >= 50 && kanisir <= 80)
        return "Good Tire Wear";
    else if (kanisir >= 30 && kanisir < 50)
        return "Conserve Your Tire";
    else
        return "Box Box Box";
}
```

char* tire(int kanisir): Fungsi ini mengambil tingkat keausan ban sebagai parameter (dalam persentase) dan memberikan saran kepada pengemudi berdasarkan tingkat keausan tersebut. Jika keausan ban lebih dari 80%, saran yang diberikan adalah "Go Push Go Push". Jika keausan ban antara 50% dan 80%, saran adalah "Good Tire Wear". Jika keausan ban antara 30% dan kurang dari 50%, saran adalah "Conserve Your Tire". Jika keausan ban kurang dari atau sama dengan 30%, saran adalah "Box Box Box".

```c
char* tire_change(char* current_tire) {
    if (strcmp(current_tire, "Soft") == 0)
        return "Mediums Ready";
    else if (strcmp(current_tire, "Medium") == 0)
        return "Box for Softs";
    else
        return "Invalid tire type";
}
```

char* tire_change(char* current_tire): Fungsi ini mengambil jenis ban saat ini sebagai parameter dan memberikan saran kepada pengemudi tentang jenis ban yang harus dipasang berikutnya. Jika jenis ban saat ini adalah "Soft", saran yang diberikan adalah "Mediums Ready". Jika jenis ban saat ini adalah "Medium", saran adalah "Box for Softs". Jika jenis ban saat ini tidak dikenali, saran adalah "Invalid tire type".

```c
char* MauMuApaSu(){
    return "GOBLOG";

}
```
char* MauMuApaSu(): Ini adalah fungsi yang memberikan jawaban yang agak kasar "GOBLOG". Fungsi ini mungkin dimaksudkan sebagai easter egg atau lelucon dalam kode.

### Driver.c

#### Inklusi Header

```c

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
```

Di sini, beberapa header standar dan sistem digunakan:

- stdio.h: Untuk fungsi input-output standar.
- sys/socket.h: Untuk operasi socket.
- stdlib.h: Untuk fungsi-fungsi umum seperti alokasi memori.
- netinet/in.h: Untuk struktur data dan konstanta yang berkaitan dengan jaringan, khususnya untuk IPv4.
- string.h: Untuk fungsi manipulasi string seperti memset dan strlen.
- unistd.h: Untuk fungsi-fungsi sistem POSIX seperti close.
- arpa/inet.h: Untuk fungsi-fungsi konversi alamat IP.

#### Definisi Port

```c

#define PORT 8080
```

Ini adalah konstanta yang mendefinisikan port yang akan digunakan dalam koneksi.

#### Fungsi Utama (main())

```c

int main(int argc, char const *argv[])
```

Ini adalah fungsi utama dari program. Ini menerima argumen baris perintah, meskipun dalam kode yang diberikan, argumen tersebut tidak digunakan.

#### Membuat Socket

```c

int sock = 0;
sock = socket(AF_INET, SOCK_STREAM, 0);
```
Pertama-tama, program membuat sebuah soket menggunakan fungsi socket(). Parameter pertama AF_INET menunjukkan bahwa ini adalah domain alamat IPv4. Parameter kedua SOCK_STREAM menunjukkan bahwa ini adalah soket TCP. Parameter ketiga yang diberikan sebagai 0 menunjukkan bahwa protokol default untuk TCP akan digunakan.

#### Menetapkan Alamat Server

```c

struct sockaddr_in serv_addr;
memset(&serv_addr, '0', sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_port = htons(PORT);
```

Struktur sockaddr_in digunakan untuk menetapkan alamat server. Fungsi memset() digunakan untuk menginisialisasi struktur tersebut dengan nol. Kemudian, kita menetapkan domain alamat (sin_family) sebagai AF_INET (IPv4) dan port (sin_port) sebagai PORT yang telah didefinisikan sebelumnya.

#### Membuat Koneksi

```c

connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))
```

Fungsi connect() digunakan untuk membuat koneksi ke server yang ditentukan oleh alamat dan port yang telah ditetapkan sebelumnya.

Bagian ini adalah bagian utama dari program yang memungkinkan pengguna untuk berinteraksi dengan server secara langsung melalui klien yang dibuat. Ini menggunakan loop tak terbatas (while(1)) untuk terus menerima input dari pengguna dan mengirimkannya ke server.


```c

while (1) {
```

Ini adalah loop tak terbatas yang akan terus berjalan sampai kondisi berhenti dipenuhi.

```c
printf("Enter message: ");
```

Ini mencetak pesan untuk meminta pengguna memasukkan pesan yang ingin dikirim ke server.

```c
memset(bapak, 0, sizeof(bapak));
```

memset() digunakan untuk mengatur ulang array bapak menjadi nol. Ini memastikan bahwa tidak ada data yang tersisa dari iterasi sebelumnya.

```c
fgets(bapak, sizeof(bapak), stdin);
```

fgets() digunakan untuk membaca input dari pengguna dari stdin (standar input) dan menyimpannya dalam array bapak.

```c
bapak[strcspn(bapak, "\n")] = '\0';
```
Ini menghapus karakter newline (\n) dari akhir string yang dibaca oleh fgets(). Hal ini dilakukan karena fgets() akan menambahkan karakter newline ke dalam string yang dibaca.

```c
if (strcmp(bapak, "exit") == 0) {
    printf("Exiting...\n");
    break;
}
```

Program memeriksa apakah pesan yang dimasukkan oleh pengguna adalah "exit". Jika iya, program mencetak pesan "Exiting..." dan keluar dari loop dengan pernyataan break, sehingga program selesai.

```c
send(sock, bapak, strlen(bapak), 0);
```

Pesan yang dimasukkan oleh pengguna dikirim ke server menggunakan send().

```c
memset(bapak, 0, sizeof(bapak));
```

Setelah mengirim pesan ke server, array bapak diatur ulang menjadi nol untuk menyiapkan penerimaan pesan dari server.

```c
valread = recv(sock, bapak, sizeof(bapak), 0);
```
Program menerima respons dari server menggunakan recv() dan menyimpannya dalam array bapak.

```c
printf("[Papa]: [%s]\n", bapak);
```
Respons dari server dicetak ke layar dengan label [Papa].

```c
close(sock);
  return 0;
}
```
Setelah keluar dari loop, koneksi socket ditutup dengan close(sock) dan program keluar dengan return 0.

### Paddock.c

Inklusi Header
```c
#include "action.c"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
```
Kode ini mengimpor beberapa header yang diperlukan untuk operasi socket dan fungsi-fungsi standar.

Definisi Makro
```c
#define PORT 8080
```
Makro ini mendefinisikan port yang akan digunakan oleh server.

Deklarasi Fungsi-fungsi
```c
void log_message(char* source, char* command, char* additional_info);
char* rpc_call(char* command, char* info);
```

Dua fungsi ini dideklarasikan di bagian atas kode. log_message digunakan untuk mencatat pesan dalam file log, sementara rpc_call digunakan untuk memproses perintah yang diterima dari klien.

Implementasi Fungsi log_message
```c
void log_message(char* source, char* command, char* additional_info) {
    FILE *fp;
    fp = fopen("race.log", "a");
    if (fp == NULL) {
        printf("Error opening file race.log.\n");
        return;
    }
    fprintf(fp, "[%s] [%s]: [%s]\n", source, command, additional_info);
    fclose(fp);
}
```

Fungsi ini digunakan untuk mencatat pesan dalam file log dengan format yang ditentukan.

Implementasi Fungsi rpc_call

```c
char* rpc_call(char* command, char* info) {
    char* response = "";
    // Memproses perintah sesuai dengan command yang diterima
    // ...
    log_message("Paddock", command, response);
    return response;
}
```

Fungsi ini memproses perintah yang diterima dari klien berdasarkan perintah yang diberikan. Setelah memproses perintah, ia mencatat pesan dalam log dan mengembalikan respons yang akan dikirimkan kembali ke klien.

Fungsi main()
```c
int main(int argc, char const *argv[]) {
    // ...
}
```

Ini adalah fungsi utama dari server.

Membuat Socket
```c

int server_fd, new_socket, valread, n;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char bapak[1024] = {0};
char *hello = "Hello from server";
  
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
}
```

Di sini, server membuat socket menggunakan socket(). Kemudian, mengonfigurasi opsi socket dengan setsockopt() untuk mengizinkan penggunaan ulang alamat dan port.
Mengikat dan Mendengarkan Port

```c
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons( PORT );

if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
}

if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
}
```

Setelah membuat socket, server mengikatnya ke alamat dan port yang ditentukan dengan bind(), kemudian mendengarkan koneksi masuk dengan listen().
Menerima dan Memproses Permintaan Klien

```c
if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
    perror("accept");
    exit(EXIT_FAILURE);
}

while(1){
    char *balik;
    n = read(new_socket, bapak, 1024);
    char ayah[100], ibu[100], anak[100], kakek[100];
    sscanf(bapak, "%s %s %s", ayah, ibu, anak);
    if(strcmp(ibu, "Change") == 0){
        sprintf(kakek, "%s %s", ayah, ibu);
        balik = rpc_call(kakek, anak);
    }
    else balik = rpc_call(ayah, ibu);
    send(new_socket, balik, strlen(balik), 0);
}
```

Server menerima koneksi dari klien menggunakan accept(), kemudian masuk ke dalam loop tak terbatas untuk menerima dan memproses permintaan dari klien. Setiap permintaan yang diterima dibaca menggunakan read(), diproses dengan rpc_call(), dan responsnya dikirim kembali ke klien dengan send().

## Contoh Output

![Screenshot from 2024-05-11 14-20-36](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/144967723/bd6a181b-0792-4630-b13b-aecdba527d4d)

## Contoh Output File Log
![Screenshot from 2024-05-11 14-21-32](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/144967723/ed72e657-add2-4bfb-9cb0-d858ef0d37b3)


# Soal 4

## Isi Soal
Lewis Hamilton 🏎 seorang wibu akut dan sering melewatkan beberapa episode yang karena sibuk menjadi asisten. Maka dari itu dia membuat list anime yang sedang ongoing (biar tidak lupa) dan yang completed (anime lama tapi pengen ditonton aja). Tapi setelah Lewis pikir-pikir malah kepikiran untuk membuat list anime. Jadi dia membuat file (harap diunduh) dan ingin menggunakan socket yang baru saja dipelajarinya untuk melakukan CRUD pada list animenya. 
- Client dan server terhubung melalui socket. 
- Client.c di dalam folder client dan server.c di dalam folder server
- Client berfungsi sebagai pengirim pesan dan dapat menerima pesan dari server.
- Server berfungsi sebagai penerima pesan dari client dan hanya menampilkan pesan perintah client saja.  
- Server digunakan untuk membaca myanimelist.csv. Dimana terjadi pengiriman data antara client ke server dan server ke client.
    - Menampilkan seluruh judul
    - Menampilkan judul berdasarkan genre
    - Menampilkan judul berdasarkan hari
    - Menampilkan status berdasarkan berdasarkan judul
    - Menambahkan anime ke dalam file myanimelist.csv
    - Melakukan edit anime berdasarkan judul
    - Melakukan delete berdasarkan judul
    - Selain command yang diberikan akan menampilkan tulisan “Invalid Command”
- Karena Lewis juga ingin track anime yang ditambah, diubah, dan dihapus. Maka dia membuat server dapat mencatat anime yang dihapus dalam sebuah log yang diberi nama change.log.
    - Format: [date] [type] [massage]
    - Type: ADD, EDIT, DEL
    - Ex:
        - [29/03/24] [ADD] Kanokari ditambahkan.
        - [29/03/24] [EDIT] Kamis,Comedy,Kanokari,completed diubah menjadi Jumat,Action,Naruto,completed.
        - [29/03/24] [DEL] Naruto berhasil dihapus.

- Koneksi antara client dan server tidak akan terputus jika ada kesalahan input dari client, cuma terputus jika user mengirim pesan “exit”. Program exit dilakukan pada sisi client.
Hasil akhir:
```c
soal_4/
    ├── change.log
    ├── client/
    │   └── client.c
    ├── myanimelist.csv
    └── server/
        └── server.c
```
Cara kerja
![carakerja1](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/5bc71491-6a17-41b2-97ab-33888db65664)
![carakerja2](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/05ce4b54-2713-40df-a4d1-f980eab3cf12)

## Penyelesaian 
### Client.c
```c

int main(int argc, char const *argv[]) {
while(1){
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    
        char input[100];
        printf("you : ");
        fgets(input, sizeof(input), stdin);

        // Send the input to the server
        send(sock, input, strlen(input), 0);
        // Receive and print the response from the server
        valread = read(sock, buffer, 1024);
        printf("server : \n%s\n", buffer);
        
        if(strcmp(input, "exit") == 0){
        return 0;
        }
   
    }

    return 0;
}
```
- Menghubungkan client pada server melalui socket
- Print "you :"  lalu meminta input dari pengguna
- Mengirim input pada pengguna
- Menerima input dari server dalam bentuk buffer
- print "server :" dan print buffer
- bila input adalah "exit" return 0

### server.c
```c
char* tampil() {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    size_t resultSize = 1024;
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0';

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 3) {
                snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, token);
                count++;
                break;
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}
```
- Membuka file "../myanimelist.csv" dalam mode baca ("r") menggunakan fungsi fopen.
- Memesan memori sebesar 1024 byte untuk menyimpan hasil bacaan file.
- Iterasi dilakukan melalui setiap baris dalam file menggunakan loop while dan fungsi fgets. Setiap baris dibaca dan disimpan dalam buffer line.
- Buffer line kemudian dipecah-pecah menjadi token menggunakan fungsi strtok. Pemisahan dilakukan dengan delimiter koma (,).
- Dalam setiap iterasi, fungsi ini mencari token yang berada di kolom ketiga, dengan menggunakan variabel column untuk melacak posisi saat ini dalam baris.
- Ketika token di kolom ketiga ditemukan, fungsi menggunakan snprintf untuk menambahkan nilai token tersebut ke dalam string hasil. Nomor urut juga ditambahkan sebelum token tersebut.
- Jumlah baris yang telah diproses (dan juga di mana token kolom ketiga berhasil ditemukan) dilacak menggunakan variabel count.
- Setelah semua baris diproses, file ditutup dengan fclose.
- String hasil yang berisi daftar nilai kolom ketiga dari setiap baris dalam file "myanimelist.csv" dikembalikan. String ini berisi nomor urut di depan setiap nilai, dipisahkan oleh newline (\n).

```c
char* genre(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    size_t resultSize = 1024;
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0';

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 2) {
                if (strstr(token, searchString) != NULL) {
                    token = strtok(NULL, ",");
                    snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, token);
                    count++;
                    break;
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}
```
- Fungsi ini membuka file "../myanimelist.csv" untuk dibaca.
- Fungsi mengalokasikan memori sebesar 1024 byte untuk menyimpan hasil pencarian.
- Iterasi dilakukan melalui setiap baris file menggunakan fgets.
- Setiap baris dipisahkan menjadi token menggunakan strtok dengan delimiter koma (,).
- Fungsi mencari substring yang sesuai dengan searchString dalam kolom kedua (genre).
- Jika substring ditemukan, fungsi menambahkan nilai dari kolom ketiga (judul) ke dalam string hasil dengan nomor urut di depannya.
- File ditutup setelah selesai dibaca.
- String hasil dikembalikan.

```c
char* hari(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    size_t resultSize = 1024;
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0';

    fseek(file, 0, SEEK_SET);

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 1) {
                if (strstr(token, searchString) != NULL) {
                    for(int i = 0; i < 2; i++){
                        token = strtok(NULL, ",");
                    }
                    snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, token);
                    count++;
                    break;
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}
```
- Sama seperti fungsi genre akan tetapi pencarian substring yang sama dilakukan di kolom 1.

```c
char* status(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    size_t resultSize = 1024;
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0';

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 3) {
                if (strstr(token, searchString) != NULL) {
                    token = strtok(NULL, ",");
                    snprintf(result + strlen(result), resultSize - strlen(result), "%s\n", token);
                    break;
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}
```
- Sama seperti fungsi hari() dan genre() akan tetapi pencarian substring dilakukan di kolom ke 3 dan string yang dikembalikan adalah yang di kolom 4.

```c
int findrow(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return -1;
    }

    int row = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        row++;
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 3) {
                if (strstr(token, searchString) != NULL) {
                    fclose(file);
                    return row;
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return 0;
}
```
- Fungsi membuka file "../myanimelist.csv" untuk dibaca dengan fopen
- Setiap baris dari file dibaca menggunakan fgets.
- Variabel row ditingkatkan untuk mencatat nomor baris saat ini.
- Setiap baris diproses untuk memecahnya menjadi token menggunakan strtok dengan delimiter koma (,).
- Pencarian dilakukan dalam kolom ketiga (judul anime).
- Jika ditemukan substring yang cocok dengan searchString, nomor baris saat ini dikembalikan, dan file ditutup.

```c

void deleteRow(const char *filename, int rowToDelete, const char* roww) {
    FILE *fp, *tempFile;
    char buffer[1024];
    int row = 0;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    tempFile = fopen("temp.csv", "w");
    if (tempFile == NULL) {
        printf("Error creating temporary file!\n");
        fclose(fp);
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        row++;

        if (row == rowToDelete)
            continue;

        fputs(buffer, tempFile);
    }

    fclose(fp);
    fclose(tempFile);

    remove(filename);

    rename("temp.csv", filename);

    FILE *logFile = fopen("../change.log", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    fprintf(logFile, "[%02d/%02d/%02d] [DEL] %s berhasil dihapus.\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, roww);
    fclose(logFile);
}
```
- fopen(filename, "r"): Membuka file CSV untuk dibaca dan fopen("temp.csv", "w"): Membuat file sementara untuk menulis hasil operasi penghapusan.
- Looping melalui setiap baris dalam file asli, Jika nomor baris saat ini tidak sama dengan rowToDelete, maka baris tersebut disalin ke file sementara. Jika sama, baris tersebut diabaikan.
- Menutup semua file
- Menghapus file asli dengan "remove" dan mengganti file asli dengan file temporary dengan "rename"
- Mencatat log dalam "../change.log"

```c
void addRow(const char *filename, const char *rowData) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "\n%s", rowData);
    fclose(file);

    FILE *logFile = fopen("../change.log", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    fprintf(logFile, "[%02d/%02d/%02d] [ADD] %s ditambahkan.\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, rowData);
    fclose(logFile);
}
```
-  Menerima nama file CSV yang akan dimodifikasi. Data yang akan ditambahkan sebagai baris baru ke file.
-  Membuka file CSV untuk ditambahkan baris baru. Mode "a" digunakan untuk menambahkan data pada akhir file.
-  Menuliskan data baru sebagai baris baru ke dalam file CSV. \n digunakan untuk memastikan baris baru dimulai sebelum data ditambahkan dengan fprintf.
-  Menutup file setelah selesai ditambahkan data.
-  Melakukan pencatatan log pada "../change.log"

```c
void editRow(const char *filename, int rowNumber, const char *rowData) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char rows[MAX_COLS][MAX_ROW_LEN];
    int currentRow = 0;

    while (fgets(rows[currentRow], MAX_ROW_LEN, file) != NULL && currentRow < MAX_COLS) {
        currentRow++;
    }
    fclose(file);

    if (rowNumber < 1 || rowNumber > currentRow) {
        printf("Invalid row number\n");
        return;
    }

    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < currentRow; i++) {
        if (i == rowNumber - 1) {
            fprintf(file, "%s\n", rowData);
        } else {
            fprintf(file, "%s", rows[i]);
        }
    }
    fclose(file);

    FILE *logFile = fopen("../change.log", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *originalRow = strdup(rows[rowNumber - 1]);
    originalRow[strcspn(originalRow, "\n")] = '\0';
    fprintf(logFile, "[%02d/%02d/%02d] [EDIT] %s diubah menjadi %s.\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, originalRow, rowData);
    free(originalRow);
    fclose(logFile);
}
```
- Menerima nama file CSV yang akan dimodifikasi(filename), nomor baris yang akan diedit(rownumber) dan data yang akan menjadi isi baris yang diubah(row data).
- Membuka file CSV untuk dibaca dengan fopen(filename, "r")
- Membaca baris dari file dan menyimpannya dalam array dengan fgets
- melacak jumlah baris yang telah dibaca dengan current row
- Menutup file
- Membuka file CSV untuk ditulis ulang dengan fopen(filename, "w") untuk penulisan
- Mengulang setiap baris yang ada dalam array rows.
- Jika baris saat ini adalah baris yang akan diedit, menulis rowData sebagai gantinya.
- Jika bukan baris yang akan diedit, menulis baris yang ada sebelumnya.
- Menutup file setelah selesai ditulis.
- Melakukan pencatatan log pada "../change.log"

```c
int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
```
- Menyambungkan server ke socket

```c
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = read(new_socket, buffer, 1024);

        printf("Received: %s\n", buffer);

        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';

        char *firstWord = strtok(buffer, " ");
        char *restOfString = NULL;

        if (firstWord != NULL) {
            restOfString = strtok(NULL, "");
        }

        if (firstWord != NULL) {
            if (strcmp("status", firstWord) == 0) {
                char* statres = status(restOfString);
                if (statres != NULL) {
                    send(new_socket, statres, strlen(statres), 0);
                    free(statres);
                } else {
                    char errmsg[] = "Error retrieving status.";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("tampilkan", firstWord) == 0) {
                char* tampres = tampil();
                if (tampres != NULL) {
                    send(new_socket, tampres, strlen(tampres), 0);
                    free(tampres);
                } else {
                    char errmsg[] = "Error displaying data.";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("genre", firstWord) == 0) {
                char* genreres = genre(restOfString);
                if (genreres != NULL) {
                    send(new_socket, genreres, strlen(genreres), 0);
                    free(genreres);
                } else {
                    char errmsg[] = "Error retrieving genre.";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("hari", firstWord) == 0) {
                char* harires = hari(restOfString);
                if (harires != NULL) {
                    send(new_socket, harires, strlen(harires), 0);
                    free(harires);
                } else {
                    char errmsg[] = "Error retrieving day.";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("delete", firstWord) == 0) {
                int delrow = findrow(restOfString);
                if (delrow != -1) {
                    deleteRow("../myanimelist.csv", delrow, restOfString);
                    char delmessage[] = "Anime berhasil dihapus";
                    send(new_socket, delmessage, strlen(delmessage), 0);
                } else {
                    char errmsg[] = "Anime tidak ditemukan";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("add", firstWord) == 0) {
                addRow("../myanimelist.csv", restOfString);
                char addmessage[] = "Anime berhasil ditambahkan";
                send(new_socket, addmessage, strlen(addmessage), 0);
            } else if (strcmp("edit", firstWord) == 0) {
                char *restOfStringCopy = strdup(restOfString); 
                char *secondWord = strtok(restOfStringCopy, ","); 
                char *anotherword = strtok(NULL, "");
                int editrow = findrow(secondWord);
                if (editrow != 0) {
                    editRow("../myanimelist.csv", editrow, anotherword);
                    char editmessage[] = "Anime berhasil diubah";
                    send(new_socket, editmessage, strlen(editmessage), 0);
                } else {
                    char notfoundmsg[] = "Anime tidak ditemukan";
                    send(new_socket, notfoundmsg, strlen(notfoundmsg), 0);
                }
                free(restOfStringCopy);
            } else if(strcmp("exit", firstWord) == 0){
            	char exitmsg[] = "Exiting the client";
            	send(new_socket, exitmsg, strlen(exitmsg), 0);
            	return 0;
            }else {
                char invalidmsg[] = "Invalid Command";
                send(new_socket, invalidmsg, strlen(invalidmsg), 0);
            }
        }
        memset(buffer, 0, sizeof(buffer));
        close(new_socket);
    }

    return 0;
}
```
- Menggunakan sistem panggilan accept untuk menerima koneksi dari klien.
- Menggunakan read untuk membaca data yang dikirimkan oleh klien ke dalam buffer yang ditentukan.
- Menggunakan strtok untuk memisahkan kata pertama dari pesan yang diterima dari klien.
- Server memeriksa kata pertama dari pesan untuk menentukan tindakan yang harus dilakukan.
- Memanipulasi data dalam file myanimelist.csv sesuai dengan instruksi yang diberikan.
- Setelah operasi selesai dilakukan, server akan mengirimkan respons ke klien menggunakan sistem panggilan send.
- Setelah menangani pesan dari klien, buffer akan direset untuk digunakan kembali dengan mengosongkannya menggunakan memset.
- Koneksi dengan klien akan ditutup menggunakan sistem panggilan close
- Jika klien mengirimkan perintah "exit", server akan mengirimkan pesan ke klien bahwa klien akan keluar, kemudian server juga akan keluar dari loop utama dan menutup koneksi dengan klien.

## Output
![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/8bc73264-84a3-4b19-a9d4-767f233da7b8)
![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/f52b0377-d650-4910-90e7-af4167ec4bf0)
![image](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/2b9ff8f6-8a6f-443c-8597-3e66854fe70c)


