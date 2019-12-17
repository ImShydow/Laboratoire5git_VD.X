/**
 * @file main.c  
 * @author Vincent Duplessis
 * @date  4 novembre 2019 
 * @brief  
  * Un programme permettant de jouer � 2 jeux faits sur un �cran LCD, soit Bonhomme pendu et Charivari.
  * Les m�thodes qui contiennent les jeux ont un pointeur comme param�tres et ce pointeur repr�sente le mot choisi.
  * La m�thode jouePendu() est construite par une boucle qui v�rifie si le caract�re choisi est celui du mot cherch� et
  * d'une autre boucle qui d�termine si le joueur a trouv� tous les caract�res. La m�thode joueCharivari() est construite en m�langeant 
  * le mot trouv� au d�but et en comparant les r�sultats du joueur avec le mot non-m�lang�. Il faut utiliser des m�thodes comme rand() et
  * stricmp() pour pouvoir avoir un ordre de caract�res al�atoires et �tre certain que la r�ponse du joueur est la m�me que le mot.
 *
 * @version 1.0
 * Environnement:
 *     D�veloppement: MPLAB X IDE (version 5.05)
 *     Compilateur: XC8 (version 2.00)
 *     Mat�riel: Carte d�mo du Pickit3. PIC 18F45K20
  */

/****************** Liste des INCLUDES ****************************************/
#include <xc.h>
#include <stdbool.h>  // pour l'utilisation du type bool
#include <conio.h>
#include "Lcd4Lignes.h"
#include <stdlib.h> // pour utiliser RAND, la m�thode qui donne un chiffre al�atoire, avec la syntaxe suivante : (rand() % 10) va donner un chiffre al�atoire entre 0 et 9
#include <stdio.h>
#include "serie.h"
#include <string.h>




/********************** CONSTANTES *******************************************/
#define NB_LIGNE 4  //afficheur LCD 4x20
#define NB_COL 20
#define NB_ALIENS 4 //nombre d'aliens qu'on mettra sur chaque ligne. On pourra ainsi facilement changer la difficult� du jeu
#define AXE_X 7  //canal analogique de l'axe x de la manette
#define PORT_FEU PORTBbits.RB1 //sw de la manette
#define DELAI_TMR0 0x0BDC // Vitesse du jeu (temps de d�placement des aliens):  0x0BDC = 2s, 0x85EE = 1s



/********************** PROTOTYPES *******************************************/
void initialisation(void);
void interrupt high_isr(void);
char getAnalog(char canal);
void affichePerdu(int pts);
bool perdu(void);
void feu(char x, int* pts);
void deplace(char* x);
void afficheAliens(void);
void nouveauxAliens(void);


/****************** VARIABLES GLOBALES ****************************************/
char m_aliens[NB_LIGNE][NB_COL+1]= //Ce tableau � 2 dimensions indique o� sont les aliens sur l'�cran. 
{
    {"      *      *      "},
    {"                    "},
    {"                    "},
    {"                    "}
};
//chaque ligne se termine par '\0'
/*Exemple d'initialisation: 
{
    {"      *      *      "},
    {"                    "},
    {"                    "},
    {"                    "}
};*/
 
bool m_tempsDAfficher = false; //Cette variable est mise � true par l'interruption du Timer0 � toutes les 2 secondes

/*               ***** PROGRAMME PRINCPAL *****                             */
void main(void)
{
    /******* variables locale au main ******/
    char verifie =0;
    bool n =false;
    char *points =0;
    char posX=10;
   /******* fin des variables locale au main ******/
  
   
    n = perdu();    
    initialisation();
    lcd_init();
    init_serie();
    
    lcd_gotoXY(10,1);
   
   
   
    
    
    
    
   
 
    
    while(1) //boucle principale
    {
      
      
     
      if(m_tempsDAfficher == true)
      { 
          nouveauxAliens();
          if(n == true)
          {
              affichePerdu(&points);
              lcd_gotoXY(10,4);
              lcd_effaceAffichage();
              *points = 0;
          }
          afficheAliens();
          lcd_gotoXY(&posX,4);
          m_tempsDAfficher =false;
          
      }
      deplace(&posX);
      if(PORT_FEU == false)
      {
          feu(posX,&points);
          afficheAliens();
          while(PORT_FEU == false);
          
      }
            
      __delay_ms(100);  

    
    }
}
/*
 * @brief D�cale les lignes vers le bas et g�n�re une nouvelle ligne avec NB_ALIENS aliens
 * Note: Cette m�thode n'affiche rien sur l'afficheur LCD. Elle ne travaille qu'avec la matrice m_aliens[][]
 * @param rien
 * @return rien
 */
void nouveauxAliens(void)
{
    bool verifie[NB_COL] = {false};
    char counter=0;
    char random=0;
    int cAlien =0;
    int k =0;
    for(int i=5; i>1;i--)//doit partir du bas pour emp�cger un bug de LCD
    {
        strcpy(m_aliens[i-2],m_aliens[i-3]);
    }
    for(int v=0; v<20; v++)
    {
        m_aliens[0][v] = ' ';
    }
    
    while(k<NB_ALIENS)
    {
        random = rand() %20;
        if(((m_aliens[0][random]) == ' ') && (verifie[random] == false))
        {
            
            m_aliens[0][random] = '*';
            verifie[random] = true;
            k++;
            
        }
        
    }
    
    for(int j=0; j<20; j++)
    {
        if(m_aliens[0][j] == '*')
        {
            counter++;
            
            while(counter > NB_ALIENS)
            {
                random = rand() %20;
                if(m_aliens[0][random]== '*')
                {
                    m_aliens[0][random] = ' '; 
                    counter--;
                }
            }       
                
        }
                   
    }
  
    
    
    for(int k=0; k<20;k++)
    {
        m_aliens[3][k] = ' ';
    }
    if(m_tempsDAfficher == false)
    {
       
        m_tempsDAfficher = true;
    }
    else
    {
        
        m_tempsDAfficher = false;
    }
    
    counter =0;
    
   
    
   
    
    


    
}
 
/*
 * @brief Affiche les 4 lignes de la matrice m_aliens[][] sur l'afficheur LCD.
 * @param rien
 * @return rien
 */
void afficheAliens(void)
{
    for(char i =0; i<4;i++)
    {
        lcd_gotoXY(1,i+1);
        lcd_putMessage(m_aliens[i]);
        
        
       
    }
    
}
 
/**
 * @brief Si la manette est vers la droite ou la gauche, on d�place le curseur d'une position
 * @param La position X sur la ligne (i.e. le pointeur de posX du main).
 * @return rien
 */
void deplace(char* x)
{
    char analog = (getAnalog(AXE_X));
    lcd_montreCurseur();
    lcd_gotoXY(*x,4);
    
    
        if(analog < 50)
        {
            (*x)--;
            if(*x <= 0)
            {
                (*x)=20;
            }
            lcd_gotoXY(*x,4);
            
        }
  
        
        if(analog > 200)
        {
            (*x)++; //priorit� des op�rations : il faut mettre *x en parenth�ses pour pouvoir additionner la valeur, sans () on additionne l'adresse
            if(*x >= 21)
            {
                *x=1;
            }
            lcd_gotoXY(*x,4);
            
        }
 
}
 
/*
 * @brief S'il y a un alien en haut (ligne 0,1 ou 2 en position X), on le tue
 * @param La position X sur la ligne. Le pointeur vers le nombre de points (le nombre d'aliens morts).
 * @return rien
 */
void feu(char x, int* pts)
{
     if(m_aliens[2][x-1] == '*')
    {
        m_aliens[2][x-1] = ' ';
        (*pts)++;
    }
     else if(m_aliens[1][x-1] == '*')
    {
        m_aliens[1][x-1] = ' ';
        (*pts)++;
    }
   
    
   
     else if(m_aliens[0][x-1] == '*')
    {
        m_aliens[0][x-1] = ' ';
        (*pts)++;
    }
   
    
}
 

/* * @brief Lorsqu'on arrive ici, le temps d'affichage d'un �cran est �coul�.
 *        Donc, s'il y a au moins 1 aliens sur la ligne 3, on a perdu
 * @param rien
 * @return vrai si on a perdu, faux sinon
 */
bool perdu(void)
{
    int counter =0;
    for(int i=0; i<20;i++)
    {
        if(m_aliens[3][i] == '*')
        {
            return true;
        }
        if(m_aliens[3][i] == ' ')
        {
            counter++;
        }
    }
    if(counter == 20)
    {
        return false; 
    }
    
}
 
 
/*
 * @brief Affiche le nombre de points obtenus pendant 3 secondes.
 * @param int pts. Les points accumul�s
 * @return rien
 */
void affichePerdu(int pts)
{
    lcd_effaceAffichage();
    lcd_gotoXY(1,1);
    lcd_putMessage("Nombre de points: ");
    lcd_putMessage(pts);
    __delay_ms(3000);
}


/*
 * @brief Lit le port analogique. 
 * @param Le no du port � lire
 * @return La valeur des 8 bits de poids forts du port analogique
 */
char getAnalog(char canal)
{ 
    ADCON0bits.CHS = canal;
    __delay_us(1);  
    ADCON0bits.GO_DONE = 1;  //lance une conversion
    while (ADCON0bits.GO_DONE == 1) //attend fin de la conversion
        ;
    return  ADRESH; //retourne seulement les 8 MSB. On laisse tomber les 2 LSB de ADRESL
}
 
 
 
/*
 * @brief Fait l'initialisation des diff�rents regesitres et variables.
 * @param Aucun
 * @return Aucun
 */
void initialisation(void)
{
    TRISD = 0; //Tout le port D en sortie
 
    ANSELH = 0;  // RB0 � RB4 en mode digital. Sur 18F45K20 AN et PortB sont sur les memes broches
    TRISB = 0xFF; //tout le port B en entree
 
    ANSEL = 0;  // PORTA en mode digital. Sur 18F45K20 AN et PortA sont sur les memes broches
    TRISA = 0; //tout le port A en sortie
 
    //Pour du vrai hasard, on doit rajouter ces lignes. 
    //Ne fonctionne pas en mode simulateur.
    T1CONbits.TMR1ON = 1;
    srand(TMR1);
 
   //Configuration du port analogique
    ANSELbits.ANS7 = 1;  //A7 en mode analogique
 
    ADCON0bits.ADON = 1; //Convertisseur AN � on
	ADCON1 = 0; //Vref+ = VDD et Vref- = VSS
 
    ADCON2bits.ADFM = 0; //Alignement � gauche des 10bits de la conversion (8 MSB dans ADRESH, 2 LSB � gauche dans ADRESL)
    ADCON2bits.ACQT = 0;//7; //20 TAD (on laisse le max de temps au Chold du convertisseur AN pour se charger)
    ADCON2bits.ADCS = 0;//6; //Fosc/64 (Fr�quence pour la conversion la plus longue possible)
 
 
    /**************Timer 0*****************/
    T0CONbits.TMR0ON    = 1;
    T0CONbits.T08BIT    = 0; // mode 16 bits
    T0CONbits.T0CS      = 0;
    T0CONbits.PSA       = 0; // prescaler enabled
    T0CONbits.T0PS      = 0b010; // 1:8 pre-scaler
    TMR0 = DELAI_TMR0;
    INTCONbits.TMR0IE   = 1;  // timer 0 interrupt enable
    INTCONbits.TMR0IF   = 0; // timer 0 interrupt flag
    INTCONbits.PEIE = 1; //permet interruption des p�riph�riques
    INTCONbits.GIE = 1;  //interruptions globales permises
}
 
/*
 * @brief Interruptions du PIC. On trouve d'o� elle vient (ici TMR0), on recharge
 * le registre pour la prochaine et on idique au main que le temps est �coul� via 
 * la variable m_tempsDAfficher
 * @param Aucun
 * @return Aucun
 */
void interrupt high_isr(void)
{
    if (INTCONbits.TMR0IF) // timer 0
    {
        INTCONbits.TMR0IF   = 0;    // baisser l'indication de l'interruption
 
        TMR0 = DELAI_TMR0;  //recharge pour la prochaine interruption
        m_tempsDAfficher = true; //Indique au main que le temps est �coul�
    }
}

   
 
