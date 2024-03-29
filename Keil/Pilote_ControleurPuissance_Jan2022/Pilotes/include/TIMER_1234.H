//________________________________________________________________________
//
//  COMPATIBLE avec "stm32f10x.h" et "Clock.h" (lors du passage KEIL4.53) 
//________________________________________________________________________
//

//============== ** MODIF TR 15/12/2021 **  ==================
// dans les fonctions d'init PWM, ajout de ARPE = 1
// ce qui permet de synchroniser les actions de chargement CCR donc modif rcy.


//=======================================================================================

//============== MODIF TR 01 Oct 2015==================
// Modif TR : version lib : f.
// 18 jan 2015
// Modi Timer : .c : Affectation des ptr de fct d�s le d�but
// 									dans les fct d'intit IT. Sinon, l'IT se d�clenche tout de suite
//									et le handler saute dans l'inconnue.
//                Une seule fonction affect�e : Active_IT_Debordement_Timer

//============== Fin MODIF TR 01 Oct 2015==============


//============== MODIF TR 16 Sept 2015==================
// Modif TR : version lib : d.
// 18 jan 2015
// Modi Timer : .c : IT compare d�bugge� : l'IT voie 1 ne lan�ait pas la fct d'IT depuis le handler
//============== Fin MODIF TR 16 Sept 2015==============

// Modif TR :

// 03 Sept 2015
// Modification de la fonction fondamentale :
//Timer_1234_Init(TIM_TypeDef *Timer, float Duree_us );
// Elle est maintenant configur�e pour que le Timer tourne en mode Up.
// NOM LIB :  Drivers_STM32F103_107_Jan_2015_c



// 21 D�cembre 2014
// Ajout de la fonction :
// Ajout de la macro qui permet de donner une polarit� n�gative � un channel de PWM
// PWM_Polarite_Neg(Timer,Voie)


#include "stm32f10x.h"
  
/******************************************************************************
* File Name          : TIMER_1234.h
* Author             : T Rocacher
* Version            : revu 21/12/2014
* Date               : 01/03/2011
* Description        : Module qui configure les TIMER 1,2,3 et 4
*  MISE A JOUR		 : Commentaire revus 18/01/2015                     
*******************************************************************************/



#ifndef _TIMER_1234_H__
#define _TIMER_1234_H__

//=======================================================================================
// Rappel sur les ressources 
//
//
//  3 Timers "general Purpose", TIM2, TIM3 et TIM4 + TIM1
//  Chacun d'entre eux dispose de 4 voies de sorties num�rot�es de 1 � 4
//  Mapping des IO:
//  	TIM1_CH1 - PA08       TM2_CH1_ETR - PA0 TM3_CH1 - PA6		TIM4_CH1 - PB6	
//    TIM1_CH2 - PA09       TM2_CH2 - PA1		  TM3_CH2 - PA7		TIM4_CH2 - PB7
// 		TIM1_CH3 - PA10       TM2_CH3 - PA2			TM3_CH3 - PB0		TIM4_CH3 - PB8
// 		TIM1_CH4 - PA11		    TM2_CH4 - PA3			TM3_CH4 - PB1		TIM4_CH4 - PB9
//	  TIM1_CH1N - PB13
//	  TIM1_CH2N - PB14
//	  TIM1_CH3N - PB15
//=======================================================================================



//=======================================================================================
// Fonctions basiques des timers
//=======================================================================================

//______________________________________________________________________________

float Timer_1234_Init(TIM_TypeDef *Timer, float Duree_us );
//______________________________________________________________________________

/*
Timer : de TIM1 � TIM4
Duree_us : c'est l'intervalle de temps exprim� en �s entre deux d�bordements successifs
Renvoie la dur�e v�ritable, au plus proche de la valeur souhait�e
Le prescaler est calcul� le plus petit possible pour une pr�cision la plus 
fine possible). Le Timer est lanc� en mode UP et non down (modif 2015).
NB: Lancer avant la fonction d'initialis�tion du syst�me d'hrologe. 
Elle permet de r�gler toutes les horloges par d�faut.

Exemple :
Init_Clock_System(); // lance l'initialis�tion du syst�me d'horloge de la puce
Timer_1234_Init(TIM2, 10.0 );   // Lance le timer avec une p�riodicit� de 10.0 uS. 
*/
//______________________________________________________________________________




//______________________________________________________________________________

void Active_IT_Debordement_Timer( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void));
//______________________________________________________________________________

/*
La fonction initialise le p�riph�rique et le NVIC de mani�re � g�n�rer une interruption 
� chaque d�bordement du timer pr�cis�.
La fonction pass�e en param�tre, forc�ment de type void Toto(void) est lanc�e sous interruption
Prio fixe la priorit� (0 � 15, 0 �tant le plus prioritaire)

Exemple :
Active_IT_Debordement_Timer( TIM2, 1, IT_Timer2); // La fonction active une interruption 
lors du d�bordement du Timer 2. La fonction appel�e est IT_Timer2. La priorit� associ�e est	1 
*/

//______________________________________________________________________________
//============== MODIF TR 18 Avril 2012==================
void Active_IT_Compare_Timer( TIM_TypeDef *Timer, char Voie, char Prio, void (*IT_function) (void));
/*
M�me principe que la pr�c�dente. Permet de lancer une fonction d'IT sur comparaison
du compteur et du CCR (capture compare). Le CCR est donc � r�gler, gr�ce � la macro
d�j� existante : PWM_Valeur(Timer,Voie) */
//============== FIN MODIF TR 18 Avril 2012==================


//=======================================================================================
// Timers en PWM
//=======================================================================================

//============== ** MODIF TR 15/12/2021 **  ==================
// dans les fonctions d'init PWM, ajout de ARPE = 1
// ce qui permet de synchroniser les actions de chargement CCR donc modif rcy. 
vu16 PWM_Init(TIM_TypeDef *Timer, char Voie, float Frequence_PWM_Khz);
/*
Cette fonction initialise la voie sp�cifi�e du timer sp�cifi� en PWM.
La fr�quence souhait�e est pass�e en param�tre.
La fonction renvoie un entier qui correspond � la r�solution de la PWM 
pour pouvoir ensuite r�gler les rapports cycliques (Resolution = ARR+1)
3 Timer "general Purpose", TIM2, TIM3 et TIM4 + TIM1
Chacun d'entre eux dispose de 4 voies de sorties num�rot�es de 1 � 4
Mapping des IO, voir d�but du .h.

!!C'est au user de configurer la sortie correctement, Altenate ppull !

Exemple :
Reso = PWM_Init(TIM2, 2, 20.0);
La fr�quence de PWM vaut 20kHz. La PWM est disponible sur la voie 2 du Timer 2
Renvoie la valeur 3600, ce qui veut dire que PWM_Valeur(TIM2,2) = 3600 donne 100% de rapport cyclique
*/


//============== MODIF TR 21 D�cembre 2014==================
#define PWM_Polarite_Neg(Timer,Voie) Timer->CCER = (Timer->CCER)|(0x01<<((Voie-1)*4+1))
/*
Par d�faut, apr�s l'init de PWM, la polarit� est positive. Si besoin est de l'inverser, 
il suffit d'�crire cette ligne, par exemple :
PWM_Polarite_Neg(TIM2,3) aura pour effet d'inverser la polarit� de la PWM voie 3 du TIM2
*/
//============== FIN MODIF TR 21 D�cembre 2014==================



void PWM_Complementaire_Timer1(char Voie);
/* petite fonction qui configure le timer suppos� d�j� en PWM
de mani�re � sortir �galement la PWM compl�mentaire sur l'un des 
3 channels ou c'est possible : 1, 2 et 3.
Ce sont donc les channel CH1N, CH2N et CH3N qui s'activent en opposition
de phase avec leur homoloque CH1, CH2, CH3.
NB : les dead time ne sont pas g�r�s, donc � priori � 0.
*/

#define PWM_Valeur(Timer,Voie) Timer->CCR##Voie
/*
Permet de fixer  la dur�e � l'�tat haut de la PWM, dont indirectement son rapport 
cyclique. La grandeur doit �tre comprise entre 0 et ARR. Ex:
Reso = PWM_Init (TIM3,2,25.0);
PWM_Valeur(TIM3,2) = Reso /4; // arrondi � gerer
*/

#define CNT(Timer) Timer->CNT
/*
Permet un acc�s direct au compteur du Timer sp�cifi� sans avoir � conna�tre 
les registres du STM32
*/
#define ARR(Timer) Timer->ARR
/*
Permet un acc�s direct � l'autoreload du Timer (modulo du timer) sp�cifi� sans 
avoir � conna�tre les registres du STM32
*/
#define Clear_Flag_Overflow(Timer) 	Timer->SR = ((Timer->SR)&~0x01)

//============== MODIF TR 18 Avril 2012==================
// Gestion de la direction de comptage
#define CptUp(Timer) Timer->CR1=Timer->CR1&~(1<<4)
#define CptDown(Timer) Timer->CR1=Timer->CR1|(1<<4)
//============== FIN MODIF TR 18 Avril 2012==================


//=======================================================================================
// Timers en mode incr�mental
//=======================================================================================

//  3 Timer "general Purpose", TIM2, TIM3 et TIM4
//  Chacun d'entre eux dispose de 4 voies de sorties num�rot�es de 1 � 4
//  Mapping des IO:
//  	TM2_CH1_ETR - PA0 	TM3_CH1 - PA6		TIM4_CH1 - PB6	
//    	TM2_CH2 - PA1		TM3_CH2 - PA7		TIM4_CH2 - PB7
//
//   Dans ce mode, le timerx utilise les 2 entr�es 1 et 2
//
char Timer_Inc_Init(TIM_TypeDef *Timer, char Resolution);

#define Reso_Demie_Per_Ch1 2
#define Reso_Demie_Per_Ch2 1
#define Reso_Quart_Per 3

/*
Fonction de configuration du timer en question en mode codeur incr�mental
Compte les fronts montant et descendant. Selon la configuration d�sir�e, 
la r�solution est de � p�riode du channel 1, ou  2, ou encore d' � de p�riode 
en comptant sur les deux voies.

!!C'est au user de configurer des canaux  en entr�e !! Le cnt est lanc�.
*/
#define Reset_Timer(Timer) Timer->CNT=0
// remet � 0 le compteur 

#define Bloque_Timer(Timer) Timer->CR1=(Timer->CR1)&~(1<<0)
// bloque le timer (CEN=0)


#define Run_Timer(Timer) Timer->CR1=(Timer->CR1)|(1<<0)
// Lance timer (CEN=1)

//=======================================================================================
// Timers en mode capture
//=======================================================================================


vu16 Capture_Init(TIM_TypeDef *Timer, char Voie, float Largeur_Pulse_Max_us, vu16 Resolution, char Prio);
/*
Le Timer consid�r� est configur� en capture. La fonction permet de mesurer une largeur d'impulsion.
Elle est associ�e � la fonction vu16  Lire_Duree_Pulse(TIM_TypeDef *Timer,int Voie).
La grandeur renvoy�e cette derni�re fonction est proportionnelle � la dur�e de l'impulsion.

Ainsi :
Largeur_Pulse_Max_us est la dur�e la plus grande mesurable en �s
Resolution  est la valeur maximale souhait�e pour la grandeur de lecture
On a donc la correspondance :

Dur�e impulsion :  -------->  Grandeur renvoy�e par la fonction de lecture
0 �s ---------------------->  0
Largeur_Pulse_Max_us ------>  Resolution

La fonction Capture_Init n'est pas forc�ment capable de faire la correspondance exacte. C'est pourquoi elle
renvoie la Resolution exacte, afin que l'utilisateur puisse s'adapter. D'autre part, si la configuration voulue 
va au del� des limites physiques, la valeur de r�solution renvoy�e est la mini/maxi possible.

Exemple :
Val=Capture_Init(TIM2, 2, 300.0, 1000, 1); // prio = 0
Renvoie la valeur 1028.
On a donc :
0 �s ---------------------->  0
100�s ------>  1028 (et non 1000)
(PSC calcul� vaut 0x14 (20). Comme Fck = 72MHz, le timer avance � 72/(20+1) = 3 428kHz.
Ainsi, pour avoir 300�s il faut compter 300�s*3 428kHz= 1028


La mesure repose sur une interruption d�clench�e sur front montant et sur front descendant. 
Le sens du front est invers� � chaque interruption. 

!!C'est au user de configurer l'IO correspondant  � la voie du timer,  en entr�e !!
*/


vu16 Lire_Duree_Pulse(TIM_TypeDef *Timer,int Voie);
/*
Le r�le de la fonction est de retourner le comptage op�r� pendant l'impulsion. 
Le nombre �volue de 0 � Largeur_Pulse_Max_us (sp�cifi� lors de l'appel � Capture_Init
*/





#endif
