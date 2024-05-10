# Sisop-1-2024-MH-IT06
# Laporan Resmi Praktikum Sistem Operasi Modul 1

Kelompok IT-06:

-Daffa Rajendra Priyatama 5027231009

-Nicholas Arya Krisnugroho Rerangin 5027231058

-Johanes Edward Nathanael 5027231067

# Soal 1

## Isi Soal
Pada zaman dahulu pada galaksi yang jauh-jauh sekali, hiduplah seorang Stelle. Stelle adalah seseorang yang sangat tertarik dengan Tempat Sampah dan Parkiran Luar Angkasa. Stelle memulai untuk mencari Tempat Sampah dan Parkiran yang terbaik di angkasa. Dia memerlukan program untuk bisa secara otomatis mengetahui Tempat Sampah dan Parkiran dengan rating terbaik di angkasa. Programnya berbentuk microservice sebagai berikut:

a. Dalam auth.c pastikan file yang masuk ke folder new-entry adalah file csv dan berakhiran         trashcan dan parkinglot. Jika bukan, program akan secara langsung akan delete file tersebut.     Contoh dari nama file yang akan diautentikasi:

    - belobog_trashcan.csv
    - osaka_parkinglot.csv
    
b. Format data (Kolom)  yang berada dalam file csv adalah seperti berikut:

![belobog1](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/d1e867a7-3636-400f-b17c-784a10d7d315)

atau

![osaka1](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/18ed1f5e-0ed8-4556-82d7-263e3d1a9d37)

c. File csv yang lolos tahap autentikasi akan dikirim ke shared memory. 

d. Dalam rate.c, proses akan mengambil data csv dari shared memory dan akan memberikan output       Tempat Sampah dan Parkiran dengan Rating Terbaik dari data tersebut.

![output1](https://github.com/DaffaEA/Sisop-3-2024-MH-IT06/assets/132379720/efc83c2a-6d10-4d18-811b-28c30a5c9c03)

e. Pada db.c, proses bisa memindahkan file dari new-data ke folder microservices/database, WAJIB MENGGUNAKAN SHARED MEMORY.

f. Log semua file yang masuk ke folder microservices/database ke dalam file db.log dengan contoh format sebagai berikut:
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

