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
 
