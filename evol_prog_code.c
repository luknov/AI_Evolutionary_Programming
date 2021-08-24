#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning (disable:4996)

void dvojkove(unsigned x, char jedinec[64][8], int i) {		//prevod ËÌsla x na bin·rne
	int c, k;
	for (c = 7; c >= 0; c--)
	{
		k = x >> c;

		if (k & 1) {
			jedinec[i][c] = '1';
		}
		else {
			jedinec[i][c] = '0';
		}
	}
}

int fitness(int pocet_najdenych_pokladov, int pocet_krokov) {	//v˝poËet fitness funkcie
	return 100*(pocet_najdenych_pokladov + 1) - pocet_krokov;
}

int virtualny_stroj(char jedinec[64][8], char mapa[7][7], int start_x, int start_y) {	//virtu·lny stroj - vracia fitness jedinca
	int i, j, adresa_int, pocet_1 = 0, pocet_instrukcii = 0, f = 0;
	unsigned int hodnota_int;
	char adresa_bit[64][7], smery[64];

	for (i = 0; i < 64; i++)
		for (j = 2; j < 8; j++)
			adresa_bit[i][j-2] = jedinec[i][j];				//adresa = posledn˝ch 6 bitov

	for (i = 0; i < 64; i++) {
			//inkrementuj
			if (jedinec[i][0] == '0' && jedinec[i][1] == '0') {		//prvÈ 2 bity = 00 -> inkrement·cia
				adresa_int = fromBinary(adresa_bit[i]);				//prevod adresy z bin·rneho na ËÌslo
				hodnota_int = fromBinary(jedinec[adresa_int]);		//prevod hodnoty bunky
				hodnota_int++;	
				if (hodnota_int == 255)								//cyklick· inötrukcia
					hodnota_int = 0;
				dvojkove(hodnota_int, jedinec, adresa_int);			//prevod z ËÌsla na bin·rne
				pocet_instrukcii++;
			}
			//dekrementuj
			else if (jedinec[i][0] == '0' && jedinec[i][1] == '1') {
				adresa_int = fromBinary(adresa_bit[i]);
				hodnota_int = fromBinary(jedinec[adresa_int]);
				hodnota_int--;
				if (hodnota_int == -1)
					hodnota_int = 255;
				dvojkove(hodnota_int, jedinec, adresa_int);
				pocet_instrukcii++;
			}
			//skok	
			else if (jedinec[i][0] == '1' && jedinec[i][1] == '0') {
				adresa_int = fromBinary(adresa_bit[i]);
				if (i < adresa_int) {								//skok ak nem·m skoËiù dozadu - nedoch·dza k zacykleniu
					i = adresa_int;
					pocet_instrukcii++;
				}
			}
			if (pocet_instrukcii >= 500)
				break;
	}

	for (i = 0; i < 64; i++) {
		pocet_1 = 0;
		for (j = 0; j < 8; j++) {
			if (jedinec[i][j] == '1')							//urËenie postupnosti krokov jedinca
				pocet_1++;
			if (j == 7) {
				if (pocet_1 <= 2)
					smery[i] = 'H';
				else if (pocet_1 > 2 && pocet_1 <= 4)
					smery[i] = 'D';
				else if (pocet_1 > 4 && pocet_1 <= 6)
					smery[i] = 'P';
				else if (pocet_1 > 6 && pocet_1 <= 8)
					smery[i] = 'L';
			}
		}
	}
	j = 0;
	printf("Smery: ");
	for (i = 0; i < 64; i++) {
		if (jedinec[i][0] == '1' && jedinec[i][1] == '1') {		//inötrukcia 11 - v˝pis postupnosti
			while (j <= i) {
				printf("%c ", smery[j]);
				j++;
			}
			pocet_instrukcii++;
		}
		if (pocet_instrukcii >= 500)
			break;
	}

	printf("\n");
	f = posun(smery, mapa, start_x, start_y, jedinec);	//funkcia na posun v mape pomocou postupnosti krokov jedinca
	if (f == -1)		//ak som naöiel rieöenie
		return 0;

	return f;
}

void vypis_riesenie(char *smery, int pocet_krokov, char jedinec[64][8]) {	//funkcia na v˝pis rieöenie pri n·jdenÌ vöetk˝ch pokladov
	int i, j;

	printf("NAJDENE VSETKY POKLADY\n");
	printf("POSTUPNOST: ");
	for (i = 0; i < pocet_krokov; i++) {
		printf("%c ", smery[i]);
	}

	printf("\nJEDINEC: ");
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 8; j++)
			printf("%c", jedinec[i][j]);	
	}
}

int posun(char *smery, char mapa[7][7], int start_x, int start_y, char jedinec[64][8]) {
	int i, j, pocet_najdenych_pokladov = 0, pocet_krokov = 0;
	char jedinec_mapa[7][7];

	for (i = 0; i < 7; i++) {
		for (j = 0; j < 7; j++)
			jedinec_mapa[i][j] = mapa[i][j];		//aby nedoölo k zmeneniu origin·lnej mapy
	}

	for (i = 0; i < 64; i++) {
		if (smery[i] == 'D' && start_x + 1 < 7) {		//ak sa mÙûem posun˙ù dole
			if (jedinec_mapa[start_x + 1][start_y] == 'P')	//skontrolujem, Ëi som nenaöiel poklad
				pocet_najdenych_pokladov++;
			jedinec_mapa[start_x][start_y] = 'm';			//polÌËko kde sa nach·dza zmenÌm na 'm'
			start_x++;
			jedinec_mapa[start_x][start_y] = 'S';			//polÌËko po posunutÌ zmenÌm na 'S' -> sl˙ûi ako pan·Ëik
			pocet_krokov++;
		}
		else if (smery[i] == 'H' && start_x - 1 > 0) {	//ak sa mÙûem posun˙ù hore
			if (jedinec_mapa[start_x - 1][start_y] == 'P')
				pocet_najdenych_pokladov++;
			jedinec_mapa[start_x][start_y] = 'm';
			start_x--;
			jedinec_mapa[start_x][start_y] = 'S';
			pocet_krokov++;
		}
		else if (smery[i] == 'P' && start_y + 1 < 7) {	//ak sa mÙûem posun˙ù vpravo
			if (jedinec_mapa[start_x][start_y + 1] == 'P')
				pocet_najdenych_pokladov++;
			jedinec_mapa[start_x][start_y] = 'm';
			start_y++;
			jedinec_mapa[start_x][start_y] = 'S';
			pocet_krokov++;
		}
		else if (smery[i] == 'L' && start_y - 1 > 0) {	//ak sa mÙûem posun˙ù væavo
			if (jedinec_mapa[start_x][start_y - 1] == 'P')
				pocet_najdenych_pokladov++;
			jedinec_mapa[start_x][start_y] = 'm';
			start_y--;
			jedinec_mapa[start_x][start_y] = 'S';
			pocet_krokov++;
		}
		else if (pocet_najdenych_pokladov == 5) {		//ak som naöiel vöetky poklady
			vypis_riesenie(smery, pocet_krokov, jedinec);
			return -1;
		}
		else {			//ak som vyöiel mimo mapy
			pocet_krokov++;	
			break;
		}
	}

	printf("Najdene poklady: %d", pocet_najdenych_pokladov);
	return fitness(pocet_najdenych_pokladov, pocet_krokov);
}

int fromBinary(char *s) {				//prevod z bin·rneho na ËÌslo
	return (int)strtol(s, NULL, 2);
}

int *ruleta(int sucet_f, int **copy_jedinci, int velkost_populacie) {		//typ selekcie - ruleta
	int *rodic, i, j;
	int r;
	time_t t;

	srand((unsigned)time(&t));

	rodic = (int*)malloc(65 * sizeof(int));

	i = 0;
	r = rand() % sucet_f;						//random ËÌslo medzi 0 a s˙Ëtom funkciÌ fitness v popul·cii
	while (r > 0 && i < velkost_populacie){
		r = r - copy_jedinci[i][64];			//pokiaæ nie je r z·pornÈ alebo 0 -> ak ·no, aktu·lny jedinec = rodiË
		i++;
	}
	
	for (j = 0; j < 65; j++) {
		if (r == 0)
			rodic[j] = copy_jedinci[i][j];
		else
			rodic[j] = copy_jedinci[i - 1][j];
	}
		
	return rodic;
}

int **top10_jedincov(int **sort_jedinci, int velkost_populacie) {		//najlepöÌch 10% jedincov
	int **top_jedinci;
	int i, j;

	top_jedinci = (int**)malloc(20 * sizeof(int));
	for (i = 0; i < velkost_populacie*0.1; i++)
		top_jedinci[i] = (int*)malloc(64 * sizeof(int));
	
	for (i = 0; i < velkost_populacie*0.1; i++)
		for (j = 0; j < 64; j++)
			top_jedinci[i][j] = sort_jedinci[velkost_populacie-1-i][j];

	return top_jedinci;
}

int **sort(int **jedinci, int velkost_populacie) {		//funkcia na usporiadanie jedincov podæa ich fitness
	int i, j;											//vyuûÌva sa pri urËenÌ 10% najlepöÌch
	int	*tmp;
	int **sort_jedinci;

	sort_jedinci = (int**)malloc(velkost_populacie * sizeof(int));
	for (i = 0; i < velkost_populacie; i++)
		sort_jedinci[i] = (int*)malloc(65 * sizeof(int));

	for (i = 0; i < velkost_populacie; i++)
		for (j = 0; j < 65; j++)
			sort_jedinci[i][j] = jedinci[i][j];

	for (i = 0; i < velkost_populacie; i++)
		for (j = 0; j < velkost_populacie - 1 - i; j++)
			if (sort_jedinci[j][64] > sort_jedinci[j + 1][64])
			{
				tmp = *(sort_jedinci + j);
				*(sort_jedinci + j) = *(sort_jedinci + j + 1);
				*(sort_jedinci + j + 1) = tmp;
			}

	return sort_jedinci;
}

int **krizenie(int **rodicia, int velkost_populacie) {		//funkcia krÌûenia
	int i, j;
	int *tmp = (int*)malloc(32 * sizeof(int));
	int **nova_populacia;
	double p;
	time_t t;

	srand((unsigned)time(&t));

	nova_populacia = (int**)malloc(velkost_populacie * sizeof(int));
	for (i = 0; i < velkost_populacie; i++)
		nova_populacia[i] = (int*)malloc(64 * sizeof(int));

	i = 0;
	while (i < 0.9*velkost_populacie) {
		p = (double)rand() / (double)RAND_MAX;			//random medzi 0 a 1
		for (j = 2; j < 8; j++) {
			if (p <= 0.7) {
				tmp[j] = rodicia[i][j];					//zamenenie
				rodicia[i][j] = rodicia[i + 1][j];
				rodicia[i + 1][j] = tmp[j];
			}
		}
		i += 2;					//posuniem sa na ÔalöÌ p·r
	}

	for (i = 0; i < 0.9 * velkost_populacie; i++)
		for (j = 0; j < 64; j++)
			nova_populacia[i][j] = rodicia[i][j];

	return nova_populacia;
}

int **mutacia(int **nova_populacia, int velkost_populacie) {		//funkcia mut·cie
	int i = 0, rand_j, r;
	double p;
	time_t t;

	srand((unsigned)time(&t));
	while (i < 0.9*velkost_populacie) {
		p = (double)rand() / (double)RAND_MAX;		//random medzi 0 a 1		
		rand_j = rand() % 64;						//random pre urËenie n·hodnej bunky, ktor˙ zmutujem
		r = rand() % 256;							//random ËÌslo, ktor˝m nahradÌm hodnotu v bunke 
		if (p <= 0.1) {
			nova_populacia[i][rand_j] = r;
		}
		i++;
	}

	return nova_populacia;
}


int main() {
	FILE* file;
	time_t t;
	char mapa[7][7];
	unsigned int hodnota_bunky;
	char jedinec[64][8], naj_jedinec[64][8];
	int i, j, x, y, velkost_populacie, sucet_fitness = 0, pomoc, pocet_gen, gen, pom_gen;
	int start_x = 0, start_y = 0;
	int **sort_fitness;
	int **jedinci, **top_jedinci, **rodicia, *pom, **copy, **nova_populacia;

	file = fopen("file.txt", "r");

	fscanf(file, "%d %d", &x, &y);				//naËÌtanie zo s˙boru
	for (i = 0; i < x; i++)
		for (j = 0; j < y; j++) {
			fscanf(file, " %c", &mapa[i][j]);
			if (mapa[i][j] == 'S') {
				start_x = i;
				start_y = j;
			}
		}

	printf("Zadaj velkost populacie: ");
	scanf("%d", &velkost_populacie);

	printf("Zadaj max pocet generacii: ");
	scanf("%d", &pocet_gen);

	sort_fitness = (int**)malloc(velkost_populacie * sizeof(int));
	for (i = 0; i < velkost_populacie; i++)
		sort_fitness[i] = (int*)malloc(65 * sizeof(int));
	
	jedinci = (int**)malloc(velkost_populacie * sizeof(int));
	for (i = 0; i < velkost_populacie; i++)
		jedinci[i] = (int*)malloc(65 * sizeof(int));
	
	top_jedinci = (int**)malloc(20 * sizeof(int));
	for (i = 0; i < 20; i++)
		top_jedinci[i] = (int*)malloc(64 * sizeof(int));
	
	rodicia = (int**)malloc(velkost_populacie * sizeof(int));
	for (i = 0; i < 0.9*velkost_populacie; i++)
		rodicia[i] = (int*)malloc(65 * sizeof(int));
	
	copy = (int**)malloc(velkost_populacie * sizeof(int*));
	for (i = 0; i < velkost_populacie; i++)
		copy[i] = (int*)malloc(65 * sizeof(int));

	nova_populacia = (int**)malloc(velkost_populacie * sizeof(int*));
	for (i = 0; i < velkost_populacie; i++)
		nova_populacia[i] = (int*)malloc(64 * sizeof(int));

	pom = (int*)malloc(65 * sizeof(int));

	srand((unsigned)time(&t));
	
	for (i = 0; i < velkost_populacie; i++) {			//inicializ·cia 1. gener·cie
		for (j = 0; j < 64; j++) {
			hodnota_bunky = rand() % 256;				//n·hodn· hodnota bunky
			dvojkove(hodnota_bunky, jedinec, j);		//hodnotu premeÚ na bity a vloû do jedinca
			jedinci[i][j] = hodnota_bunky;				//hodnotu vloû do pola vöetk˝ch jedincov popul·cie
		}
		jedinci[i][64] = virtualny_stroj(jedinec, mapa, start_x, start_y);	//na konci = fitness jedinca
		sucet_fitness += jedinci[i][64];
		printf("\n");
		printf("\n");
	}


	for (i = 0; i < velkost_populacie; i++)
		for (j = 0; j < 65; j++)
			copy[i][j] = jedinci[i][j];

	i = 0;
	while (i < 0.9 * velkost_populacie){		//urËenie rodiËov
		pom = ruleta(sucet_fitness, copy, velkost_populacie);
		rodicia[i] = pom;
		i++;
	}

	sort_fitness = sort(jedinci, velkost_populacie);		//usporiadanie jedincov podæa fitness

	top_jedinci = top10_jedincov(sort_fitness, velkost_populacie);	//najlepöÌch 10% jedincov, ktorÌ id˙ do novej popul·cie

	nova_populacia = krizenie(rodicia, velkost_populacie);	//krÌûenie rodiËov
	nova_populacia = mutacia(nova_populacia, velkost_populacie);	//mutovanie 
	
	pomoc = 0;
	for (i = velkost_populacie - 1; i >= velkost_populacie - velkost_populacie*0.1 - 1; i--) {
		for (j = 0; j < 64; j++)
			if (pomoc < velkost_populacie * 0.1)
				nova_populacia[i][j] = top_jedinci[pomoc][j];	//vloûenie najlepöÌch jedincov na koniec popul·cie
		pomoc++;
	}

	////////////////////////////////////////////
	pom_gen = pocet_gen;				
	gen = 1;
	while (pocet_gen > 1) {				//generovanie ÔalöÌch gener·ciÌ 
		for (i = 0; i < velkost_populacie; i++) {
			for (j = 0; j < 64; j++) {
				dvojkove(nova_populacia[i][j], jedinec, j);
				jedinci[i][j] = nova_populacia[i][j];
			}
			jedinci[i][64] = virtualny_stroj(jedinec, mapa, start_x, start_y);
			sucet_fitness += jedinci[i][64];
			printf("\n");
			printf("\n");
		}

		sort_fitness = sort(jedinci, velkost_populacie);
		top_jedinci = top10_jedincov(sort_fitness, velkost_populacie);

		for (i = 0; i < velkost_populacie; i++)
			for (j = 0; j < 65; j++)
				copy[i][j] = jedinci[i][j];

		i = 0;
		while (i < 0.9 * velkost_populacie) {
			pom = ruleta(sucet_fitness, copy, velkost_populacie);
			rodicia[i] = pom;
			i++;
		}

		nova_populacia = krizenie(rodicia, velkost_populacie);
		nova_populacia = mutacia(nova_populacia, velkost_populacie);

		pomoc = 0;
		for (i = velkost_populacie - 1; i >= velkost_populacie - velkost_populacie * 0.1 - 1; i--) {
			for (j = 0; j < 64; j++)
				if (pomoc < 10)
					nova_populacia[i][j] = top_jedinci[pomoc][j];
			pomoc++;
		}
		
		pocet_gen--;
		gen++;
	}

	for (i = 0; i < velkost_populacie; i++)
		for (j = 0; j < 64; j++) {
			dvojkove(sort_fitness[i][j], naj_jedinec, j);
		}

	if (gen == pom_gen) {						//ak sa nen·jdu vöetky poklady, vypÌö najlepöie rieöenie
		int pocet_najdenych_pokladov = 0, pocet_krokov = 0, smery[64], pocet_1;
		printf("NAJLEPSIE RIESENIE \n");
		printf("FITNESS: %d\n", sort_fitness[velkost_populacie - 1][64]);
		printf("JEDINEC: ");
		for (i = 0; i < 64; i++)
			for (j = 0; j < 8; j++) {
				printf("%c", naj_jedinec[i][j]);
			}

		for (i = 0; i < 64; i++) {
			pocet_1 = 0;
			for (j = 0; j < 8; j++) {
				if (jedinec[i][j] == '1')
					pocet_1++;
				if (j == 7) {
					if (pocet_1 <= 2)
						smery[i] = 'H';
					else if (pocet_1 > 2 && pocet_1 <= 4)
						smery[i] = 'D';
					else if (pocet_1 > 4 && pocet_1 <= 6)
						smery[i] = 'P';
					else if (pocet_1 > 6 && pocet_1 <= 8)
						smery[i] = 'L';
				}
			}
		}

		for (i = 0; i < 64; i++) {
			if (smery[i] == 'D' && start_x + 1 < 7) {
				if (mapa[start_x + 1][start_y] == 'P')
					pocet_najdenych_pokladov++;
				mapa[start_x][start_y] = 'm';
				start_x++;
				mapa[start_x][start_y] = 'S';
				pocet_krokov++;
			}
			else if (smery[i] == 'H' && start_x - 1 > 0) {
				if (mapa[start_x - 1][start_y] == 'P')
					pocet_najdenych_pokladov++;
				mapa[start_x][start_y] = 'm';
				start_x--;
				mapa[start_x][start_y] = 'S';
				pocet_krokov++;
			}
			else if (smery[i] == 'P' && start_y + 1 < 7) {
				if (mapa[start_x][start_y + 1] == 'P')
					pocet_najdenych_pokladov++;
				mapa[start_x][start_y] = 'm';
				start_y++;
				mapa[start_x][start_y] = 'S';
				pocet_krokov++;
			}
			else if (smery[i] == 'L' && start_y - 1 > 0) {
				if (mapa[start_x][start_y - 1] == 'P')
					pocet_najdenych_pokladov++;
				mapa[start_x][start_y] = 'm';
				start_y--;
				mapa[start_x][start_y] = 'S';
				pocet_krokov++;
			}
			else {
				pocet_krokov++;
				break;
			}
		}
		printf("\nPostupnost krokov: ");
		for (i = 0; i < pocet_krokov; i++)
			printf("%c ", smery[i]);
		printf("\nPocet najdenych pokladov: %d\n", pocet_najdenych_pokladov);
	}

	fclose(file);

	return 0;
}