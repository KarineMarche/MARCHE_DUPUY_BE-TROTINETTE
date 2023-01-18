
/*
	!!!! NB : ALIMENTER LA CARTE AVANT DE CONNECTER L'USB !!!

VERSION 16/12/2021 :
- ToolboxNRJ V4
- Driver version 2021b (synchronisation de la mise à jour Rcy -CCR- avec la rampe)
- Validé Décembre 2021

*/


/*
STRUCTURE DES FICHIERS

COUCHE APPLI = Main_User.c : 
programme principal à modifier. Par défaut hacheur sur entrée +/-10V, sortie 1 PWM
Attention, sur la trottinette réelle, l'entrée se fait sur 3V3.
Attention, l'entrée se fait avec la poignée d'accélération qui va de 0.6V à 2.7V !

COUCHE SERVICE = Toolbox_NRJ_V4.c
Middleware qui configure tous les périphériques nécessaires, avec API "friendly"

COUCHE DRIVER =
clock.c : contient la fonction Clock_Configure() qui prépare le STM32. Lancée automatiquement à l'init IO
lib : bibliothèque qui gère les périphériques du STM : Drivers_STM32F103_107_Jan_2015_b
*/



#include "ToolBox_NRJ_v4.h"




//=================================================================================================================
// 					USER DEFINE
//=================================================================================================================
#define pi 3.14159

//composants
#define R 1
#define L 0.002
#define Kc (1.25/12) //gain du capteur
#define Kf 1.457 // gain du filtre
#define Kh 48  //gain du hacheur
#define Km 1/R //gain du moteur
#define Ktot Kh*Km*Kc*Kf

//fréquences caractéristiques
#define ft 400
#define fe 3000 //on doit être entre 1kHz et 10kHZ
#define te 3.333e-4


//constantes de temps du correcteur
#define to L/R
#define toi Ktot/(2*pi*ft)

//constantes de temps du filtre
#define to1 112.2e-6
#define to2 5e-6

// Choix de la fréquence PWM (en kHz)
#define FPWM_Khz 20.0
						
//coefficients du correcteur s(n)=s(n-1)+a0*e(n)+a1*e(n-1)
#define a0 (te+2*to)/(2*toi)
#define a1 (te-2*to)/(2*toi)


//==========END USER DEFINE========================================================================================

// ========= Variable globales indispensables et déclarations fct d'IT ============================================

void IT_Principale(void);
//=================================================================================================================


/*=================================================================================================================
 					FONCTION MAIN : 
					NB : On veillera à allumer les diodes au niveau des E/S utilisée par le progamme. 
					
					EXEMPLE: Ce progamme permet de générer une PWM (Voie 1) à 20kHz dont le rapport cyclique se règle
					par le potentiomètre de "l'entrée Analogique +/-10V"
					Placer le cavalier sur la position "Pot."
					La mise à jour du rapport cyclique se fait à la fréquence 1kHz.

//=================================================================================================================*/


float te_us;


int main (void)
{
// !OBLIGATOIRE! //	
Conf_Generale_IO_Carte();	

// ------------- Discret, choix de Te -------------------	
//Te=	1/fe; // en seconde
te_us=te*1000000.0; // conversion en µs pour utilisation dans la fonction d'init d'interruption
	

//______________ Ecrire ici toutes les CONFIGURATIONS des périphériques ________________________________	
// Paramétrage ADC pour entrée analogique
Conf_ADC();
// Configuration de la PWM avec une porteuse Triangle, voie 1 & 2 activée, inversion voie 2
Triangle (FPWM_Khz);
Active_Voie_PWM(1);	
Active_Voie_PWM(2);	
Inv_Voie(2);

Start_PWM;
R_Cyc_1(2048);  // positionnement à 50% par défaut de la PWM
R_Cyc_2(2048);

// Activation LED
LED_Courant_On;
LED_PWM_On;
LED_PWM_Aux_Off;
LED_Entree_10V_On;
LED_Entree_3V3_Off;
LED_Codeur_Off;

// Conf IT
Conf_IT_Principale_Systick(IT_Principale, te_us);


	while(1)
	{
		}

}





//=================================================================================================================
// 					FONCTION D'INTERRUPTION PRINCIPALE SYSTICK
//=================================================================================================================
float Courant_1,Cons_In, e_n_ADC;
float Cons_In_Test, Courant_1_Test;
float e_n, s_n;
float e_n_1 = 0; 
float s_n_1 = 0; 
float rapport_PWM;

void IT_Principale(void)
{
 //Acquisition de la consigne et du courant
	Cons_In=Entree_3V3(); 
	Courant_1=I1();
	
 //Calcul de epsilon=consigne-courant
	e_n_ADC=Cons_In-Courant_1;

	//conversion
	e_n=e_n_ADC*3.3/4095;
		
 //Calcul de s(n)
	s_n=s_n_1+a0*e_n+a1*e_n_1;	
	
 //saturation du s(n)
 if (s_n>=0.5) {
	s_n=0.5;
 }	 
 if (s_n<=(-0.5)) {
	s_n=-0.5;
 } 
 
 //envoi de la valeur de s(n) à la PWM
 rapport_PWM = 4096*s_n+2048;
 R_Cyc_1((int)rapport_PWM ); // pour le hâcheur PWM+
 R_Cyc_2((int)rapport_PWM);// pour le hâcheur PWM-
  
 //mise à jour des paramètres
 s_n_1=s_n;
 e_n_1=e_n;
	
}

