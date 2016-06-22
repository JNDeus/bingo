#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>


#define VALIDO 0
#define INVALIDO 1
// Atributos da tela
#define LARGURA_TELA 1024
#define ALTURA_TELA 700
// Atributos dos menus
#define LARGURA_ITEM_MENU 200
#define ALTURA_ITEM_MENU 75
#define ESPACO_ENTRE_ITENS 5
#define TOTAL_DE_ITENS 3
// Atributos dos numeros
#define TOT_NUMS 75
#define LARGURA_NUM 50
#define ALTURA_NUM 30
// Atributos cartelas
#define TAM 5
#define NUM_CARTELAS 5
#define NUM_CHAR_JOGADOR 10



const int FRAMES_POR_SEGUNDO = 30;
double tempoInicial = 0;

double obterTempoTimer()
{
    return al_get_time() - tempoInicial;
}
int inicializar_allegro(){
    if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar a Allegro.\n");
        return 0;
    }
    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on image.\n");
        return 0;
    }

    //Inicialização do add-on para uso de fontes
    al_init_font_addon();
    // Inicialização do add-on para uso de fontes True Type
    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return 0;
    }

    return 1;

}
int inicializar_janela(ALLEGRO_DISPLAY *janela){
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela)
    {
        fprintf(stderr, "Falha ao criar janela.\n");
        return 0;
    }

    // Configura o título da janela
    al_set_window_title(janela, "Jogo da memória");
    return janela;
}

int inicializar_mouse(ALLEGRO_DISPLAY *janela){
    if (!al_install_mouse())
    {
        fprintf(stderr, "Falha ao inicializar o mouse.\n");
        return 0;
    }
    // Atribui o cursor padrão do sistema para ser usado
    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
        return 0;
    }
    return 1;
}

int inicializar_teclado(){
    if (!al_install_keyboard())
    {
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return 0;
    }
    return 1;
}

int inicializar_fila_eventos(ALLEGRO_EVENT_QUEUE *fila_eventos, ALLEGRO_DISPLAY *janela){
    fila_eventos = al_create_event_queue();
    if (!fila_eventos)
    {
        fprintf(stderr, "Falha ao inicializar o fila de eventos.\n");
        return 0;
    }

    // Dizemos que vamos tratar os eventos vindos do mouse
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    return fila_eventos;
}

int carrega_fonte(ALLEGRO_FONT *fonte, int tam){
    // Carregando o arquivo de fonte
    fonte = al_load_font("FORTE.TTF", tam, 0);
    if (!fonte)
    {
        fprintf(stderr, "Falha ao carregar fonte.\n");
        return 0;
    }
    return fonte;
}

void carregaNumeros(ALLEGRO_BITMAP *numeros[TOT_NUMS]){
    int i;
    char iStr[2];
    for(i = 1; i <= TOT_NUMS;i++){
        int tmp = i;
        char caminhoImg[25] = "imagens\\numeros\\";
        sprintf(iStr, "%d", tmp);
        strcat(caminhoImg, iStr);
        strcat(caminhoImg, ".jpg");
        numeros[i-1] = al_load_bitmap(caminhoImg);
    }
}
void carregaBotoes(ALLEGRO_BITMAP *menus[TOTAL_DE_ITENS]){
    menus[0] = al_load_bitmap("imagens\\botoes\\Jogar.jpg");
    menus[1] = al_load_bitmap("imagens\\botoes\\creditos.jpg");
    menus[2] = al_load_bitmap("imagens\\botoes\\sair.jpg");
}

void escreveNaStr(ALLEGRO_EVENT evento, char str[])
{
    if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
    {
        if (strlen(str) < NUM_CHAR_JOGADOR)
        {
            char temp[] = {evento.keyboard.unichar, '\0'};
            if (evento.keyboard.unichar == ' ')
            {
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= '0' &&
                     evento.keyboard.unichar <= '9')
            {
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= 'A' &&
                     evento.keyboard.unichar <= 'Z')
            {
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= 'a' &&
                     evento.keyboard.unichar <= 'z')
            {
                strcat(str, temp);
            }
        }

        if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0)
        {
            str[strlen(str) - 1] = '\0';
        }
    }
}

typedef struct {
    int cart[TAM][TAM], id;
    char jogador[NUM_CHAR_JOGADOR];
} Cartela;

void preenCARTELA(int cart[][TAM]){ //PREENCHE A CARTELA E EVITA QUE O NUMERO SE REPITA
    int i, j, k, status, temp;
        for(i = 0; i < TAM; i++){
            for(j = 0; j < TAM; j++){

                if(i == 0){
                    do{
                        temp = 1 + (rand() % 15);
                        status = VALIDO;
                        for(k = 0; k < TAM; k++){
                            if(temp == cart[i][k]){
                                status = INVALIDO;
                            }
                        }
                    }while(status == INVALIDO);
                    cart[i][j] = temp;

                }
                else if(i == 1){
                        do{
                        temp = 16 + (rand() % (30 - 16));
                        status = VALIDO;
                        for(k = 0; k < TAM; k++){
                            if(temp == cart[i][k]){
                                status = INVALIDO;
                            }
                        }
                    }while(status == INVALIDO);
                    cart[i][j] = temp;

                }
                else if(i == 2){
                        do{
                        temp = 31 + (rand() % (45 - 31));
                        status = VALIDO;
                        for(k = 0; k < TAM; k++){
                            if(temp == cart[i][k]){
                                status = INVALIDO;
                            }
                        }
                    }while(status == INVALIDO);
                    cart[i][j] = temp;

                }
                else if(i == 3){
                        do{
                        temp = 46 + (rand() % (60 - 46));
                        status = VALIDO;
                        for(k = 0; k < TAM; k++){
                            if(temp == cart[i][k]){
                                status = INVALIDO;
                            }
                        }
                    }while(status == INVALIDO);
                    cart[i][j] = temp;

                }
                else{
                    do{
                        temp = 61 + (rand() % (75 - 61));
                        status = VALIDO;
                        for(k = 0; k < TAM; k++){
                            if(temp == cart[i][k]){
                                status = INVALIDO;
                            }
                        }
                    }while(status == INVALIDO);
                    cart[i][j] = temp;

                }
            }
        }
    organizarLINHAS(cart, TAM);
}

void organizarLINHAS(int cart[TAM][TAM], int n){ //ORGANIZA AS CARTELAS
    int i, j,k, cond, temp;

    for(i = 0; i < TAM; i++ ){

        cond = 1;
        for (j = n-1; (j >= 1) && (cond == 1); j--) {
        cond = 0;
            for (k = 0; k < j ;k++) {
                if (cart[i][k+1] < cart[i][k]) {
                    temp = cart[i][k];
                    cart[i][k] = cart[i][k+1];
                    cart[i][k+1] = temp;
                    cond = 1;
                }
            }
        }
    }
}

int main()
{
     srand(time(NULL));
    Cartela cartelas[NUM_CARTELAS];
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL, *fila_contador = NULL;
    ALLEGRO_FONT *fonte = NULL, *fonte24 = NULL;
    ALLEGRO_BITMAP *back_menu, *back_cartelas, *cobrir, *numeros[TOT_NUMS], *menus[TOTAL_DE_ITENS], *voltar, *creditos;
    ALLEGRO_TIMER *contador = 0;
    int tela_atual = 0, sair = 0;

    if(!inicializar_allegro()){
        return -1;
    }
    if(!inicializar_teclado()){
        return -1;
    }
    janela = inicializar_janela(janela);
    if(!janela){
        return -1;
    }
    if(!inicializar_mouse(janela)){
        al_destroy_display(janela);
        return -1;
    }

    fonte = carrega_fonte(fonte, 30);
    if(!fonte){
        al_destroy_display(janela);
        return -1;
    }
    fonte24 = carrega_fonte(fonte24, 24);
    if(!fonte24){
        al_destroy_display(janela);
        return -1;
    }
    fila_eventos = inicializar_fila_eventos(fila_eventos, janela);
    if(!fila_eventos){
        al_destroy_display(janela);
        return -1;
    }

    contador = al_create_timer(1.0);
    if (!contador)
    {
        fprintf(stderr, "Falha ao criar timer.\n");
        al_destroy_font(fonte);
        al_destroy_event_queue(fila_eventos);
        al_destroy_display(janela);
        return false;
    }

    fila_contador = al_create_event_queue();
    if (!fila_contador)
    {
        fprintf(stderr, "Falha ao criar fila do contador.\n");
        al_destroy_timer(contador);
        al_destroy_font(fonte);
        al_destroy_event_queue(fila_eventos);
        al_destroy_display(janela);
        return false;
    }
    al_register_event_source(fila_contador, al_get_timer_event_source(contador));

    //AS IMAGENS SÃO CARREGADAS NO INICIO DO PROGRAMA BEM AQUI
    back_cartelas = al_load_bitmap("imagens\\cartelas.jpg");
    back_menu = al_load_bitmap("imagens\\Menu.jpg");
    cobrir = al_load_bitmap("imagens\\cobrir.jpg");
    creditos = al_load_bitmap("imagens\\Telacreditos.jpg");
    voltar = al_load_bitmap("imagens\\botoes\\voltar.jpg");
    carregaNumeros(numeros);
    carregaBotoes(menus);

            int a = 0;
    while (!sair){// Verificamos se há eventos na fila
        if(tela_atual == 0){
            while (!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento;
                al_wait_for_event(fila_eventos, &evento);

                if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                    sair = 1;
                }

                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    int y = ALTURA_TELA/2+20, x = LARGURA_TELA/2-LARGURA_ITEM_MENU/2, i;
                    for(i = 0; i < TOTAL_DE_ITENS; i++){
                        if(evento.mouse.x >= x
                           && evento.mouse.x <= x + LARGURA_ITEM_MENU
                           && evento.mouse.y >= y
                           && evento.mouse.y <= y + ALTURA_ITEM_MENU){
                            tela_atual = i + 1;
                            break;
                        }
                        y += ALTURA_ITEM_MENU+ESPACO_ENTRE_ITENS;
                    }
                }
            }
            al_draw_bitmap(back_menu, 0, 0, 0);

            int y = ALTURA_TELA/2+20, i;
            for(i = 0; i < TOTAL_DE_ITENS; i++){
                if(a < 3){
                    a++;
                }
                al_draw_bitmap(menus[i], LARGURA_TELA/2-LARGURA_ITEM_MENU/2, y, 0);
                y += ALTURA_ITEM_MENU+ESPACO_ENTRE_ITENS;
            }
            al_flip_display();
        }else if(tela_atual == 1){
            int num = 0, i, numSorteados[TOT_NUMS];
            zeraVet(numSorteados, TOT_NUMS);
            for(i=0; i<NUM_CARTELAS; i++){
                preenCARTELA(cartelas[i].cart);
                cartelas[i].id = i+1;
                strcpy(cartelas[i].jogador, "");

                int concluido = 0;
                do{
                    while (!al_is_event_queue_empty(fila_eventos)){
                        ALLEGRO_EVENT evento;
                        al_wait_for_event(fila_eventos, &evento);
                        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                            sair = 1;
                            concluido = 1;
                        }
                        if(!concluido){
                            escreveNaStr(evento, cartelas[i].jogador);
                            if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER){
                                concluido = 1;
                            }
                        }
                    }

                    al_draw_bitmap(back_menu, 0, -68, 0);
                    if(!sair){
                        perguntaNome(cartelas[i].jogador, fonte24, i);
                    }

                    al_flip_display();
                }while(!concluido);
                if(sair){
                    break;
                }
            }
            if(sair){
                break;
            }

            al_start_timer(contador);
            int returnJogar, cont = 0;
            returnJogar = jogar(fila_eventos, numSorteados, &cartelas, back_cartelas, numeros, cobrir, fonte, contador, fila_contador, &num, &cont);
            if(returnJogar == 2){
                sair = 1;
            }
        }else if(tela_atual == 2){
            while (!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento;
                al_wait_for_event(fila_eventos, &evento);

                if ( evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                    sair = 1;
                }else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    int y = 620, x =  LARGURA_TELA/2-LARGURA_ITEM_MENU/2, i;
                    if(evento.mouse.x >= x
                       && evento.mouse.x <= x + LARGURA_ITEM_MENU
                       && evento.mouse.y >= y
                       && evento.mouse.y <= y + ALTURA_ITEM_MENU){
                        tela_atual = 0;
                        break;
                    }
                }

            }
            al_draw_bitmap(creditos, 0, 0, 0);
            al_draw_bitmap(voltar, LARGURA_TELA/2-LARGURA_ITEM_MENU/2, 620, 0);
            al_flip_display();
        } if(tela_atual == 3){
            sair = 1;
        }
    }

    return 0;
}

int jogar(
        ALLEGRO_EVENT_QUEUE *fila_eventos,
        int *numSorteados[TOT_NUMS],
        Cartela *cartelas[NUM_CARTELAS],
        ALLEGRO_BITMAP *back_cartelas,
        ALLEGRO_BITMAP *numeros[TOT_NUMS],
        ALLEGRO_BITMAP *cobrir,
        ALLEGRO_FONT *fonte,
        ALLEGRO_TIMER *contador,
        ALLEGRO_EVENT_QUEUE *fila_contador,
        int *num,
        int *cont
    ){
    iniciarTimer();
    int sair = 0, frame = 0, venc = -1;
    while (!al_is_event_queue_empty(fila_eventos)){
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            sair = 1;
            break;
        }
    }
    if(contaVetor(numSorteados, TOT_NUMS) >= 25){
        venc = verificaVencedor(cartelas);
    }
    if(venc == -1){
        if (!al_is_event_queue_empty(fila_contador))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_contador, &evento);

            if (evento.type == ALLEGRO_EVENT_TIMER)
            {
                *num = sorteioNUMERO(numSorteados);
                verificaCartelas(cartelas, *num);
            }
        }
    }
    al_draw_bitmap(back_cartelas, 0, 0, 0);
    desenhaCartelas(cartelas, numeros, cobrir, fonte);
    if(venc > -1){
        mostraVencedor(cartelas, venc, fonte);
    }else{
        al_draw_textf(fonte, al_map_rgb(255, 255, 255), 10, 315, 0, "Numero Sorteado: %d", *num);
    }
    if (obterTempoTimer() < 1.0 / FRAMES_POR_SEGUNDO)
    {
        al_rest((1.0 / FRAMES_POR_SEGUNDO) - obterTempoTimer());
        *cont += 1;
    }
    al_flip_display();
    if(sair){
        return 2;
    }else{
        return jogar(fila_eventos, numSorteados, cartelas, back_cartelas, numeros, cobrir, fonte, contador, fila_contador, num, cont);
    }
}
void mostraVencedor(Cartela cartelas[NUM_CARTELAS], int venc, ALLEGRO_FONT *fonte){
    al_draw_textf(fonte, al_map_rgb(255, 255, 255), 10, 315, 0, "O jogador %s venceu!!", cartelas[venc].jogador);
}
void iniciarTimer()
{
    tempoInicial = al_get_time();
}
int verificaVencedor(Cartela cartelas[NUM_CARTELAS]){
    int i = 0;
    for(i = 0; i < NUM_CARTELAS;i++){
        int j,k, ok = 1;
        for(j = 0; j< TAM;j++){
            for(k = 0; k< TAM;k++){
                if(cartelas[i].cart[j][k] <= TOT_NUMS){
                    ok = 0;
                }
            }
        }
        if(ok){
            return i;
        }
    }
    return -1;
}
void desenhaCartelas(Cartela cartelas[NUM_CARTELAS], ALLEGRO_BITMAP *numeros[TOT_NUMS], ALLEGRO_BITMAP *cobrir, ALLEGRO_FONT *fonte){
    int i,x=17,y=58, xNome = 158, yNome = 22, xId = 16, yId = 276;
    for(i=0;i<NUM_CARTELAS;i++){
        al_draw_textf(fonte, al_map_rgb(255, 255, 255), xNome, yNome, 0, cartelas[i].jogador);
        al_draw_textf(fonte, al_map_rgb(255, 255, 255), xId, yId, 0, "ID: 00%d", cartelas[i].id);
        int j,k;
        for(j=0;j<TAM;j++){
            for(k=0;k<TAM;k++){
                if(cartelas[i].cart[j][k] <= TOT_NUMS){
                    al_draw_bitmap(numeros[cartelas[i].cart[j][k]-1], x, y, 0);
                }else{
                    al_draw_bitmap(cobrir, x, y, 0);
                }
                x += LARGURA_NUM+15;
            }
            if(i>2){
                x = 174 + 364 * (i-3);
            }else{
                x = 17 + 339 * i;
            }
            y += ALTURA_NUM+14;
        }
        if(i == 2){
            y = 417;
            yNome = 641;
            xNome = 318;
            xId = 174;
            yId = 385;
            x = 174;
        }else if(i>2){
            y = 417;
            x += 364;
            xNome = 318 + 363 * (i-2);
            xId = 174 + 363 * (i-2);
        }else{
            y = 58;
            x += 339;
            xNome += 340;
            xId += 340;
        }
    }
}

int contaVetor(int vet[], int tam){
    int i, cont = 0;
    for(i = 0; i < tam; i++){
        if(vet[i] > 0){
            cont++;
        }
    }
    return cont;
}

void verifica(int num, int cart[][TAM]){//VERIFICA SE TEM O NUMERO NAS CARTELAS
    int i, j;

    for(i = 0; i < TAM; i++){
        for(j = 0; j < TAM; j++){
            if(num == cart[i][j]){
                cart[i][j] = 9999;
            }

        }
    }
}


void verificaCartelas(Cartela cartelas[NUM_CARTELAS], int numSort){
    int i;
    for(i=0;i<NUM_CARTELAS;i++){
        verifica(numSort, cartelas[i].cart);
    }
}

void addNextIndex(int *vet[], int tam, int value){
    int i;
    for(i=0;i<tam;i++){
        if(vet[i] == 0){
            vet[i] = value;
            break;
        }
    }
}

int sorteioNUMERO(int *numSorteados[TOT_NUMS]){ // FAZ O SORTEIO DO NÚMERO VERIFICANDO SE ELE JÁ NÃO SORTEADO
    int num;
    num = 1 + (rand() % TOT_NUMS);
    if(buscaLinear(numSorteados, TOT_NUMS, num) == -1){
        addNextIndex(numSorteados, TOT_NUMS, num);
        return num;
    }
    return sorteioNUMERO(numSorteados);
}

int buscaLinear(int vetor[], int tamanho, int elementoProcurado) {
    int i;
    for (i = 0; i < tamanho; i++) {
         if (vetor[i] == elementoProcurado) {
             return i;
         }
    }

    return -1;
}


int zeraVet(int vet[], int tam){
    int i;
    for(i = 0; i < tam; i++){
        vet[i] = 0;
    }
}
void perguntaNome(char str[], ALLEGRO_FONT *fonte, int i)
{
    al_draw_textf/**<  */(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2,
                    (ALTURA_TELA - al_get_font_ascent(fonte)) / 2,
                    ALLEGRO_ALIGN_CENTRE, "Digite o nome do jogador %d:", i+1);
    if (strlen(str) > 0)
    {
        al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2,
                     (ALTURA_TELA - al_get_font_ascent(fonte)) / 2 +30,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
}
