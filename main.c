#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VSTUPNI_SOUBOR "startovni_listina_kombinace_zeny.txt"
#define VYSTUPNI_SOUBOR "vysledkova_listina_kombinace_zeny.txt"
#define DEL " :,\n"
#define POCET_ZNAKU 100
#define POCET_RADKU 150

typedef struct {
    int sjezd_m;
    int sjezd_s;
    int sjezd_setiny;
    int slalom_s;
    int slalom_setiny;
} CAS;

typedef struct {
    int startovni_cislo;
    char jmeno[20];
    char prijmeni[20];
    char stat[5];
    CAS cas;
} ZAVODNIK;

int nacti_soubor(ZAVODNIK *zavodnik) {
    int index_zavodnika = 0;
    FILE *soubor = fopen(VSTUPNI_SOUBOR, "r");
    if (soubor == NULL) {
        printf("Soubor %s se nepodarilo otevrit.\n", VSTUPNI_SOUBOR);
    } else {
        char radek[POCET_ZNAKU];
        char *cas_radku;
        while (fgets(radek, sizeof(radek), soubor) != NULL) {
            int sloupec = 0;
            cas_radku = strtok(radek, DEL);
            while (cas_radku != NULL) {
                switch (sloupec) {
                    case 0:
                        zavodnik[index_zavodnika].startovni_cislo = atoi(cas_radku);
                        break;
                    case 1:
                        strcpy(zavodnik[index_zavodnika].jmeno, cas_radku);
                        break;
                    case 2:
                        strcpy(zavodnik[index_zavodnika].prijmeni, cas_radku);
                        break;
                    case 3:
                        strcpy(zavodnik[index_zavodnika].stat, cas_radku);
                        break;
                    case 4:
                        zavodnik[index_zavodnika].cas.sjezd_m = atoi(cas_radku);
                        break;
                    case 5:
                        zavodnik[index_zavodnika].cas.sjezd_s = atoi(cas_radku);
                        break;
                    case 6:
                        zavodnik[index_zavodnika].cas.sjezd_setiny = atoi(cas_radku);
                        break;
                    case 7:
                        zavodnik[index_zavodnika].cas.slalom_s = atoi(cas_radku);
                        break;
                    case 8:
                        zavodnik[index_zavodnika].cas.slalom_setiny = atoi(cas_radku);
                        break;
                }
                cas_radku = strtok(NULL, DEL);
                sloupec++;
            }
            index_zavodnika++;
        }
        if (fclose(soubor) == EOF) {
            printf("Soubor %s se nepodarilo zavrit.\n", VSTUPNI_SOUBOR);
        }
    }
    return index_zavodnika;
}

int prevod_casu_sjezdu_na_setiny(CAS cas) {
    return cas.sjezd_m * 60 * 100 + cas.sjezd_s * 100 + cas.sjezd_setiny;
}

int prevod_casu_slalomu_na_setiny(CAS cas) {
    return cas.slalom_s * 100 + cas.slalom_setiny;
}

int soucet_setin(CAS cas) {
    return prevod_casu_sjezdu_na_setiny(cas) + prevod_casu_slalomu_na_setiny(cas);
}

CAS prevod_sekund_sjezdu_na_cas(int celkove_setiny) {
    CAS vysledny_cas;
    int sekundy = celkove_setiny / 100;
    vysledny_cas.sjezd_m = (sekundy % 3600) / 60;
    vysledny_cas.sjezd_s = sekundy % 60;
    vysledny_cas.sjezd_setiny = celkove_setiny % 100;
    return vysledny_cas;
}

void serad_zavodniky(ZAVODNIK *zavodnik, int pocet) {
    for (int i = 0; i < pocet - 1; ++i) {
        for (int j = 0; j < pocet - i - 1; ++j) {
            int cas1 = soucet_setin(zavodnik[j].cas);
            int cas2 = soucet_setin(zavodnik[j + 1].cas);
            if (cas1 > cas2) {
                ZAVODNIK temp = zavodnik[j];
                zavodnik[j] = zavodnik[j + 1];
                zavodnik[j + 1] = temp;
            }
        }

    }
}

void vypis(ZAVODNIK *zavodnik, int pocet) {
    printf("Alpske lyzovani startovni listina KOMBINACE ZENY\n");
    printf("----------------------------------------------------------------------------\n");
    printf("startovni cislo      jmeno         prijmeni        stat        sjezd          slalom\n");
    printf("----------------------------------------------------------------------------\n");
    for (int i = 0; i < pocet; ++i) {
        printf("%15d %10s %16s %11s       %d:%02d,%02d        %02d,%02d \n", zavodnik[i].startovni_cislo,
               zavodnik[i].jmeno, zavodnik[i].prijmeni, zavodnik[i].stat, zavodnik[i].cas.sjezd_m,
               zavodnik[i].cas.sjezd_s, zavodnik[i].cas.sjezd_setiny, zavodnik[i].cas.slalom_s,
               zavodnik[i].cas.slalom_setiny);
    }
}

void zapis_do_souboru(ZAVODNIK *zavodnik, int pocet){
    FILE *soubor = fopen(VYSTUPNI_SOUBOR, "w");
    if(soubor == NULL){
        printf("Soubor %s se nepodarilo otevrit\n", VYSTUPNI_SOUBOR);
        return;
    }
    fprintf(soubor, "ALPSKE LYZOVANI - KOMBINACE ZENY\n");
    fprintf(soubor, "-------------------------------------------------------------------------\n");
    fprintf(soubor, "poradi     jmeno    prijmeni     stat     sjezd      slalom    celkem      ztrata  \n");
    fprintf(soubor, "-------------------------------------------------------------------------\n");

    int vytezne_setiny = soucet_setin(zavodnik[0].cas);

    for (int i = 0; i < pocet; ++i) {
        int aktualni_setiny = soucet_setin(zavodnik[i].cas);
        int ztrata = aktualni_setiny - vytezne_setiny;
        CAS celkovy_cas = prevod_sekund_sjezdu_na_cas(aktualni_setiny);
        CAS ztrata_cas = prevod_sekund_sjezdu_na_cas(ztrata);

        fprintf(soubor, " %6d %10s %10s %7s  %d:%02d,%02d        %02d,%02d   %d:%02d,%02d ", i + 1,
                zavodnik[i].jmeno, zavodnik[i].prijmeni, zavodnik[i].stat, zavodnik[i].cas.sjezd_m,
                zavodnik[i].cas.sjezd_s, zavodnik[i].cas.sjezd_setiny, zavodnik[i].cas.slalom_s,
                zavodnik[i].cas.slalom_setiny, celkovy_cas.sjezd_m, celkovy_cas.sjezd_s, celkovy_cas.sjezd_setiny);

        if(i == 0){
            fprintf(soubor, "\n");
        }else {
            fprintf(soubor, "%0d:%02d,%02d \n", ztrata_cas.sjezd_m, ztrata_cas.sjezd_s, ztrata_cas.sjezd_setiny);
        }

    }
    if(fclose(soubor) == EOF){
        printf("Soubor %s se nepodarilo zavrit", VYSTUPNI_SOUBOR);
    }
}

int main() {
    ZAVODNIK zavodnik[POCET_RADKU];
    int pocet = nacti_soubor(zavodnik);
    vypis(zavodnik, pocet);
    serad_zavodniky(zavodnik, pocet);
    zapis_do_souboru(zavodnik, pocet);

    return 0;
}
