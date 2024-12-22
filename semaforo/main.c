#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

//Identificadores
#define LEDR 13
#define LEDY 12
#define LEDG 11
#define LEDP 10
#define BTN 7
#define BZZ_PIN 4
#define BZZ_FREQ 660

int main() {
  stdio_init_all();

  //Inicando e configurando o botão
  gpio_init(BTN);
  gpio_set_dir(BTN, false);
  gpio_pull_up(BTN);

  //Iniciando a porta dos leds
  gpio_init(LEDR);
  gpio_init(LEDY);
  gpio_init(LEDG);
  gpio_init(LEDP);

  // Inicializar o PWM no pino do buzzer
  pwm_init_buzzer(BZZ_PIN);


  //Settando as portas como saida
  gpio_set_dir(LEDR, true);
  gpio_set_dir(LEDY, true);
  gpio_set_dir(LEDG, true);
  gpio_set_dir(LEDP, true);

  while (true) {
    semaforoN();
    
    sleep_ms(10); // Pequeno atraso para evitar alto consumo de CPU
  }

}

//Função para acender os leds
void semaforo_put(bool r, bool y, bool g){
     gpio_put(LEDR, r);
     gpio_put(LEDY, y);
     gpio_put(LEDG, g);
     return 0;
}

//Função para rotina de semaforo 
void semaforoN(){

    //Verifica se o botão foi pressionado durante o sinal Verde
    for (int i=0; i<8000;i++){
      sleep_ms(1);
      semaforo_put(false,false,true);
      if(gpio_get(BTN)==0){
        semaforoP('g');
      }
    }

    //Verifica se o botão foi pressionado durante o sinal Amarelo
    int a = 0;
    for (int j=0; j<2000;j++){
      sleep_ms(1);
      semaforo_put(false,true,false);
      if(gpio_get(BTN)==0){
        a = 1;
      }
    }
    if (a){
      semaforoP('y');
    }

    //Semaforo Vermelho
    semaforo_put(true,false,false);
   
    sleep_ms(10000);
    return 0;
}

//Função quando o botão for pressionado
void semaforoP(char s){
  //Se o botão for acionado quando o sinal estiver Verde
  if (s == 'g'){
    semaforo_put(false,true,false);
    sleep_ms(5000);
    //Sinal Vermelho com Beep de espera
    semaforo_put(true,false,false);
    gpio_put(LEDP, true);
    for(int i = 0;i<15;i++ ){
      beep(BZZ_PIN, 500);
    }
    gpio_put(LEDP,false);
     //retorna para a função principal
    main();
    return 0;
  }
   //Se o botão for acionado quando o sinal estiver Laranja
  if (s == 'y'){
    semaforo_put(true,false,false);
    gpio_put(LEDP,true);
    //Sinal Vermelho com Beep de espera
    for(int i = 0;i<15;i++ ){
      beep(BZZ_PIN, 500);
    }
    gpio_put(LEDP,false);
    //retorna para a função principal
    main();
    return 0;
  }

}

//BUZZER 
// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BZZ_FREQ * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// Definição de uma função para emitir um beep com duração especificada
void beep(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 1024);

    // Temporização
    sleep_ms(duration_ms);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);

    // Pausa entre os beeps
    sleep_ms(500); // Pausa de 100ms
}

