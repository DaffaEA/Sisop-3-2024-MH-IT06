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

# Soal 2

# Soal 3

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
