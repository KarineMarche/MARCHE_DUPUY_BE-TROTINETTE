
%%%%%%%%%%%%%%%%%%%%%%%%%%%% PARTIE 1 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%% déterminer les paramètres du correcteur %%%%%%%%%%%%%% 

%spécifications
ft=400;

%capteur courant
Kc=1.25/12;
N=tf(Kc,1);

%moteur
R=1;
Km=1/R;
tom=0.002;
M=tf(Km,[tom 1]);

%Hâcheur
Kh=48;
H=tf(Kh,1);

%Filtre
R18=12000;
R12=10000;
R5=5100;
R8=10000;
Kf=(1+(R18/R12))*R8/(R5+R8);
to1=112.2*10^(-6);
to2=4.84*10^(-6);
F1=tf(Kf,[to1 1]);
F2=tf(1,[to2 1]);
F=F1*F2;

%général
G=F*H*M*N;
Ktot=Kc*Km*Kh*Kf;

%correcteur
to=1/(2*pi*80);
toi=Ktot/(2*pi*ft);
C=tf([to 1],[toi 0]);

%%%%%%%%%%%%%%%%%%%%%%%%%%%% PARTIE 2 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%% vérifier les paramètres du correcteur %%%%%%%%%%%%%%% 

FTBO=C*G;
h = bodeplot(FTBO);
p = bodeoptions('cstprefs');
p.FreqUnits = 'Hz';
p.PhaseVisible = 'on';

bodeplot(FTBO,p);

%%%%%%%%%%%%%%%%%%%%%%%%%%%% PARTIE 3 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%% tester en BF grace a simulink %%%%%%%%%%%%%%%%%% 

num=H*M;
denum=F*N;

%%%%%%%%%%%%%%%%%%%%%%%%%%%% PARTIE 4 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%% transformée en Z  %%%%%%%%%%%%%%%%%%%%%%%%
Fe=1000;
Te=1/Fe; % on prend fe=1000Hz>2*400Hz
CZ=c2d(C,Te,'tustin'); %,tustin


%%%%%%%%%%%%%%%%%%%%%%%%%%%% PARTIE 5 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%% récupération des résultats %%%%%%%%%%%%%%%%%%%

%out=sim("FTBF_sans_sat"); %pour voir le courant de sortie sans saturateur dans le circuit
%out=sim("FTBF_avc_sat"); % pour voir le courant de sortie une fois qu'on a ajouté
%un saturateur
out=sim("FTBFenZ"); %pour voir le courant de sortie quand on a fait la transformée bilinéaire du correcteur 
%Nous avons placé la sortie dans le simulink au niveau du courant Im.
%Toutefois, Nous pouvons également décider de mettre le out au niveau de
%l'erreur ou du alpha. Nous pourrions également l'observer. 
Sortie=out.simout;
plot(Sortie);