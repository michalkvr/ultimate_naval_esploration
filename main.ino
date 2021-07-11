/**
 * @file main.ino
 * @brief Ultimate Naval Esploration source code
 * @author by Michal Kovar
 * @date 2 Jan 2019 
 */
 
#include <Esplora.h>
#include <TFT.h>

//=====================================================< CONSTANTS >======================================================

#define HERO_MOVE 5       /**< "Rychlost" pohybu vaší lodě*/
#define HERO_LBORDER 50   /**< Oblast pohybu vaší lodě*/
#define HERO_RBORDER 100  /**< Oblast pohybu vaší lodě*/

#define BG_R 0    /**< Barva pozadí RED hodnota*/
#define BG_G 255  /**< Barva pozadí GREEN hodnota*/
#define BG_B 255  /**< Barva pozadí BLUE hodnota*/
 
#define MOVE_TRIGGER 30 /**< Mrtvá zóna joysticku */

#define GAMESPEED 10    /**< Rychlost hry */

#define SHIP_H 20 /**< Výška vaší lodě */
#define SHIP_W 10 /**< Šířka vaší lodě */

#define SHOT_S 7  /**< Velikost střely */

#define OBST_S 30 /**< Velikost překážky */

#define SD_CS    8   // Chip select line for SD card in Esplora

int score = 0;
int highscore = 0;
int oldscore = 0;
int def_speed = 100;
int charged = 1;

//=====================================================< OBJECT >=======================================================

struct Obj {
  int width;
  int height;
  int x;
  int y;
  int pos;
};

//=====================================================< NEW OBJECT >======================================================
/**                                             
 * @brief Vytovření nového objektu                                              
 * 
 *    Funkce newObj vytvoří nový objekt o šířce a výšce získané přes parametry
 *    
 * @param width Požadovaná šířka nového objektu
 * @param height Požadovaná výška nového objektu
 * 
 * @retval Vytvořený objekt Obj
 */
Obj newObj (int width, int height) {
  Obj a;
  a.width = width;
  a.height = height;
  return a;
} 

//===================================================< SETTING A SCENE >====================================================
/**                                             
 * @brief Funkce pro prvotní nastavení umístění a vykreslení vaší lodi                                            
 * 
 *    Funkce setHero umístí(tj. vykreslí) vaší loď na určené souřadnice
 *    
 * @param Pointer na objekt Obj, tj. vaše loď
 */
 
void setHero (Obj *a) {
  a->x = 75;
  a->y = 90;
  EsploraTFT.fill(205, 133, 63);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);
}

/**                                             
 * @brief Funkce pro prvotní nastavení umístění a vykreslení překážky                                              
 * 
 *    Funkce setObst umístí(tj. vykreslí) překážku na určené souřadnice
 *    
 * @param a Pointer na objekt Obj, tj. překážka
 */
 
void setObst (Obj *a) {
  a->pos = (rand()*millis())%2; 
  if (a->pos) 
    a->x = 50;
  else
    a->x = 80;
  a->y =0;
  EsploraTFT.fill(0,146,0);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);  
}

/**                                             
 * @brief Funkce pro prvotní nastavení umístění a vykreslení nepřátelské lodi                                              
 * 
 *    Funkce setEnemy umístí(tj. vykreslí) nepřátelskou loď na určené souřadnice
 *    
 * @param a Pointer na objekt Obj, tj. nepřátelská loď 
 */
 
void setEnemy (Obj *a) {
  a->pos = (rand()*millis())%2; 
  if (a->pos)
    a->x = 20;
  else
    a->x = 130;
  a->y = 128;
  EsploraTFT.fill(200,0,0);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);  
}

/**                                             
 * @brief Funkce pro umístění střely                                            
 * 
 *    Funkce nastaví souřadnice střely na souřadnice vaší lodi
 *    
 * @param s Pointer na objekt Obj, tj. střela
 * @param s Pointer na objekt Obj, tj. vaše loď
 * @param pos Určuje jestli se střela bude pohybovat doleva nebo doprava
 */

void setShot (Obj *s, Obj *h, int pos) {
  s->x = h->x;
  s->y = h->y + SHIP_H/2;
  s->pos = pos;
}

//==================================================< DELETE OBJECT >===================================================
/**                                             
 * @brief Funkce pro smazání objektu z displeje                                              
 *    
 * @param a Pointer na objekt Obj. který je vzápětí smazán
 */
 
void delObj (Obj *a) 
{
  EsploraTFT.fill(0,255,255);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);  
}

//=================================================< MOVING FUNCTIONS >==================================================
/**                                             
 * @brief Funkce pro pohyb vaší loďě                                              
 * 
 *    Funkce moveHero zkontroluje zda-li jsme dosáhli levé či pravé hranice oblasti určené pro vaši loď,
 *    pokud ne, posune loď o konstatnu HERO_MOVE, pokud ano, funkce nemá účinek.
 *    
 * @param a Pointer na objekt Obj, tj. vaše loď 
 * @param x Hodnota x načtená z joysticku Esplory
 */
 
void moveHero (Obj *a, int x) { 
  EsploraTFT.fill(0,255,255);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);
  if (x < (-1)*MOVE_TRIGGER)
    if (a->x < HERO_RBORDER)
      a->x += HERO_MOVE;
  if (x > MOVE_TRIGGER)
    if (a->x > HERO_LBORDER)
      a->x -= HERO_MOVE;
  EsploraTFT.fill(205,133,63);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);
}

/**                                             
 * @brief Funkce pro pohyb překážky                                         
 * 
 *    Funkce moveObst při každém zavolání posune překážku na displeji o 3px dolů,
 *    pokud překážka dosáhla okraje displeje, zavolá se funkce setObst a vytvoří se překážka nová.
 *    
 * @param a Pointer na objekt Obj, tj. překážka
 */
void moveObst (Obj *a) {
  EsploraTFT.fill(0,255,255);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);
  (a->y) += 5;
  EsploraTFT.fill(0,146,0);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);
  if (a->y > 128) {
    setObst(a);  
    charged = 1;
    if (def_speed > 0)
      def_speed -= GAMESPEED;
  }
}

/**                                             
 * @brief Funkce pro pohyb nepřátelské lodi                                         
 * 
 *    Funkce moveEnemy při každém zavolání posune nepřátelskou loď na displeji o 2px nahorů,
 *    složí pouze k nástupu nepřátelské lodi na scénu.
 *    
 * @param a Pointer na objekt Obj, tj. nepřátelská loď
 */
 
void moveEnemy (Obj *a) {
  EsploraTFT.fill(0,255,255);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);
  (a->y) -= 2;
  EsploraTFT.fill(200,0,0);
  EsploraTFT.rect(a->x, a->y, a->width, a->height);
}

/**                                             
 * @brief Funkce pro pohyb střely                                        
 * 
 *    Funkce moveShot při každém zavolání posune střelu, dokud nenarazí na nepřátelský objekt 
 *    nebo okraj displeje
 *    
 * @param a Pointer na objekt Obj, tj. střela
 * @retval Vrátí 1 pokud se střela dostala na okraj displeje a byla smazána, jinak vrátí 0
 */

int moveShot (Obj *s) {
  EsploraTFT.fill(0,255,255);
  EsploraTFT.rect(s->x, s->y, s->width, s->height);
  if (s->x > 10 && s->x < 150) {
    if (s->pos)
      s->x += 5;    
    else 
      s->x -= 5;
    EsploraTFT.fill(0,0,0);
    EsploraTFT.rect(s->x, s->y, s->width, s->height);
    return 0;
  }
  else {
    delObj(s);
    return 1;
  }
}

//=================================================< CHECK FUNCTIONS >==================================================
/**                                             
 * @brief Funkce pro kontrolu, jestli se vaše loď srazila s překážkou                                         
 * 
 *    Funkce zkontroluje, jestli střela dosáhla souřadnic nepřátelské lodě
 *    
 * @param s Pointer na objekt Obj, tj. střelu
 * @param e Pointer na objekt Obj, tj. nepřátelskou loď
 * 
 * @retval Vrátí číslo 1 pokud došlo ke kolizi, číslo 0 pokud ke kolizi nedošlo
 */
 
int isHit (Obj *s, Obj *e) {
  int bingo = 0;
  if (s->pos && !e->pos) 
    if (s->x >= e->x) 
      bingo = 1;
  if (!s->pos && e->pos)
    if (s->x <= e->x)
      bingo = 1;
  if (bingo) {
    delObj(e);
    score++;
    return 1;
  }
  else 
    return 0; 
}

/**                                             
 * @brief Funkce pro kontrolu, jestli se vaše loď srazila s překážkou                                         
 * 
 *    Funkce zkontroluje, jestli je překážka podle osy y na úrovni vaší lodi,
 *    pokud ano, tak zkontroluje jestli vzniká průnik souřadnice y vaší lodi a překážky,
 *    pokud ano, nastala kolize.
 *    
 * @param h Pointer na objekt Obj, tj. vaši loď
 * @param o Pointer na objekt Obj, tj. překážku
 * 
 * @retval Vrátí číslo 1 pokud došlo ke kolizi, číslo 0 pokud ke kolizi nedošlo
 */
                                                
int isCrash (Obj *h, Obj *o) {
  if (o->y > 60 && o->y < 110) {
    if (o->pos && h->x < 80)
      return 1;
    if (!(o->pos) && h->x > 70)
      return 1;   
  }
  else
    return 0;        
}

//====================================================< RUN FUNCTIONS >=======================================================
/**                                             
 * @brief Funkce pro samotné spuštění hry
 * 
 *    Zobrazí menu, resetuje skóre, rychlost, nastaví všechny objekty a spustí hru
 */

void runPlay() {
  while (1) {
    runMenu();
    score = 0;
    def_speed = 100;
    int button;
    int x;
    int play = 1;
    int shoot = 0;
    charged = 1;
    Obj shot = newObj(SHOT_S, SHOT_S);
    Obj hero = newObj(SHIP_W, SHIP_H);
    setHero(&hero);  
    Obj obst = newObj(OBST_S, OBST_S);
    setObst(&obst);
    Obj enemy = newObj(SHIP_W, SHIP_H);
    setEnemy(&enemy);
    while (1) {
      x = Esplora.readJoystickX();
      showScore();
      moveHero(&hero, x);
      moveObst(&obst);
      if (enemy.y > 90) 
        moveEnemy(&enemy);
      delay(def_speed);
      if (isCrash(&hero, &obst)) {
        runDefeat();
        break;
      }
      if (charged) {
        button = Esplora.readButton(SWITCH_LEFT);
        if (button == LOW) {
          setShot(&shot, &hero, 0);
          charged = 0;
          shoot = 1;
        }
        button = Esplora.readButton(SWITCH_RIGHT);
        if (button == LOW) {
          setShot(&shot, &hero, 1);
          charged = 0;
          shoot = 1;
        }
      }
      if (shoot) {
        if (moveShot(&shot))
          shoot = 0;
        if (isHit(&shot,&enemy)) {
          shoot = 0;
          setEnemy(&enemy);
          if (enemy.y > 90) 
            moveEnemy(&enemy);
        }
      }
    }
  } 
}

/**                                             
 * @brief Funkce vykreslí hlavní menu hry
 */

void runMenu() {
  int button = HIGH;
  EsploraTFT.background(BG_R,BG_G,BG_B);
  EsploraTFT.stroke(0,0,0);
  EsploraTFT.text("ULTIMATE", 55, 20);
  EsploraTFT.text("NAVAL", 65, 40);
  EsploraTFT.text("ESPLORATION",48,60);
  EsploraTFT.stroke(0,0,255);
  EsploraTFT.text("PRESS SWITCH UP", 38, 100);
  while(button == HIGH)
    button = Esplora.readButton(SWITCH_UP);
  EsploraTFT.background(BG_R,BG_G,BG_B);
  EsploraTFT.stroke(0,255,255);
}

/**                                             
 * @brief Funkce oznámí prohru a zobrazí získané skóre
 */

void runDefeat() {
  int button = HIGH;
  char printout[4];
  char high_printout[4];
  String str_score = String(score);
  String str_highscore = String(highscore);
  str_score.toCharArray(printout, 4);
  str_highscore.toCharArray(high_printout, 4);
  EsploraTFT.background(BG_R,BG_G,BG_B);
  EsploraTFT.stroke(0,0,0);
  EsploraTFT.text("DEFEAT", 63,40);
  EsploraTFT.text("SCORE: ", 60,60);
  EsploraTFT.text(printout, 100, 60);
  EsploraTFT.text("HIGHSCORE: ", 50,80);
  EsploraTFT.text(high_printout, 115, 80);
  if (score > highscore)
    highscore = score;
  while(button == HIGH) {
    button = Esplora.readButton(SWITCH_UP);
  }
  EsploraTFT.background(BG_R,BG_G,BG_B);
  EsploraTFT.stroke(BG_R,BG_G,BG_B);
}

//=======================================================< OTHER FUNCTIONS >==========================================================
/**                                             
 * @brief Funkce pro zobrazení skóre na displeji
 */

void showScore() {
  if(oldscore != score) {
    char printout[4];
    char oldprintout[4];
    String str_score = String(score);
    String str_oldscore = String(oldscore);
    str_score.toCharArray(printout, 4);
    str_oldscore.toCharArray(oldprintout, 4);
    EsploraTFT.stroke(BG_R,BG_G,BG_B);
    EsploraTFT.text("SCORE: ", 10, 10);
    EsploraTFT.text(oldprintout, 140, 10);
    EsploraTFT.stroke(0,0,0);
    EsploraTFT.text("SCORE: ", 10, 10);
    EsploraTFT.text(printout, 140, 10);
    EsploraTFT.stroke(0,255,255);
    oldscore = score;
  }
}

//========================================================< SETUP & LOOP >=============================================================
void setup() 
{
  EsploraTFT.begin();
  EsploraTFT.background(BG_R,BG_G,BG_B);
}
void loop() 
{ 
  runPlay();
}