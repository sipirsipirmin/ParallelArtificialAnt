#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<unistd.h>
#include<omp.h>

#define NODE_SAYISI 5
#define HEDEF_DUGUM 4
#define BASLANGIC_DURUMU 0
#define FEROMON_ESIGI 1
#define FEROMON_SALGI_MIKTARI 0.2
#define KARINCA_SAYISI 100
#define MAX_YOL_UZUNLUGU 30
#define TRUE 1
#define FALSE 0
#define NO_WAY 999
#define NO_CHOOSEN_WAY 99


_Bool komsuluk_matrisi[NODE_SAYISI][NODE_SAYISI];
int agirlik_matrisi[NODE_SAYISI][NODE_SAYISI];
float feromon_matrisi[NODE_SAYISI][NODE_SAYISI];
int yol[KARINCA_SAYISI][MAX_YOL_UZUNLUGU];

void create_graph(){
    for(int i=0;i<NODE_SAYISI;i++)
        for(int j=0; j<NODE_SAYISI; j++)
            feromon_matrisi[i][j] = 0;

    komsuluk_matrisi[0][1] = TRUE;
    agirlik_matrisi[0][1] = 5;

    komsuluk_matrisi[0][2] = TRUE;
    agirlik_matrisi[0][2] = 6;

    komsuluk_matrisi[1][3] = TRUE;
    agirlik_matrisi[1][3] = 5;

    komsuluk_matrisi[2][1] = TRUE;
    agirlik_matrisi[2][1] = 2;

    komsuluk_matrisi[3][4] = TRUE;
    agirlik_matrisi[3][4] = 10;

    komsuluk_matrisi[2][3] = TRUE;
    agirlik_matrisi[2][3] = 1;

    komsuluk_matrisi[1][2] = TRUE;
    agirlik_matrisi[1][2] = 2;

}


void print_feromon_matrisi(){
    for (int i = 0; i < NODE_SAYISI; i++)
        for(int j = 0; j<NODE_SAYISI; j++)
            printf("%d'den %d'ye: %f\n", i,j,feromon_matrisi[i][j]);
}


void print_ant_way(int karinca_no){
    printf("karinca no: %d, yol: ", karinca_no);
    int temp_uzunluk = 0;
    for(int i = 0; i<MAX_YOL_UZUNLUGU; i++){
        if(yol[karinca_no][i+1] != NO_WAY){
            printf(" %d ->", yol[karinca_no][i]);
            temp_uzunluk += agirlik_matrisi[yol[karinca_no][i]][yol[karinca_no][i+1]];
        }else{
            printf("%d, toplam uzunluk: %d\n", yol[karinca_no][i], temp_uzunluk);
            break;
        }
    }
}


int find_possible_ways_and_get_counts(int durum, int *gidilebilecek_olasi_yollar){
    int temp_sayac = 0;
    for(int i = 0; i < NODE_SAYISI; i++){
        if(komsuluk_matrisi[durum][i]){
            gidilebilecek_olasi_yollar[temp_sayac] = i;
            temp_sayac++;
        }
    }

    return temp_sayac;
}


int smell_ways_and_get_feromon(int durum, int *gidilebilecek_olasi_yollar, int temp_sayac){
    int secilen_yol = NO_CHOOSEN_WAY;

    for(int i = 0; i<temp_sayac; i++){
        if(feromon_matrisi[durum][gidilebilecek_olasi_yollar[i]] > FEROMON_ESIGI){
            secilen_yol = gidilebilecek_olasi_yollar[i];
            // printf("ferr seçildi, %d->%d\n", durum,secilen_yol);
        }
    }
    return secilen_yol;
}


int get_random_way(int *gidilebilecek_olasi_yollar, int temp_sayac){
    sleep(0.2); // for more randomizing
    return gidilebilecek_olasi_yollar[abs(rand()%temp_sayac)];
}


int main(int argc, char const *argv[]) {
    create_graph();
    int durum;
    srand(time(NULL));
    int temp_sayac;
    int gidilebilecek_olasi_yollar[NODE_SAYISI];
    int iterasyon_no;
    int secilen_yol;

    #pragma omp parallel for num_threads(4)  private(durum, iterasyon_no, secilen_yol, temp_sayac, gidilebilecek_olasi_yollar)
    for(int karinca_no = 0; karinca_no < KARINCA_SAYISI; karinca_no++){
        durum = BASLANGIC_DURUMU;
        yol[karinca_no][0] = durum;
        iterasyon_no = 1; // Başlangıç durumuna gelmiş olması da bir iterasyon
        secilen_yol = NO_CHOOSEN_WAY;

        for(int i = 1; i<MAX_YOL_UZUNLUGU; i++)
            yol[karinca_no][i] = NO_WAY;

        while(durum != HEDEF_DUGUM && iterasyon_no < MAX_YOL_UZUNLUGU){
            temp_sayac = find_possible_ways_and_get_counts(durum, gidilebilecek_olasi_yollar);
            secilen_yol = smell_ways_and_get_feromon(durum, gidilebilecek_olasi_yollar, temp_sayac);

            if(secilen_yol == NO_CHOOSEN_WAY)
                secilen_yol = get_random_way(gidilebilecek_olasi_yollar, temp_sayac);

            #pragma omp critical
            feromon_matrisi[durum][secilen_yol] += FEROMON_SALGI_MIKTARI/agirlik_matrisi[durum][secilen_yol];

            durum = secilen_yol;
            yol[karinca_no][iterasyon_no] = secilen_yol;
            iterasyon_no++;
        }

    }
    for(int i=0; i<KARINCA_SAYISI;i++)
        print_ant_way(i);
    return 0;
}
