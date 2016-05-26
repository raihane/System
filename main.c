#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include  <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <utime.h>
#include <time.h>
#include <string.h>

// Les constantes du programme
#define MAX_PATH 200  //longueur du nom d'un fichier
#define MAX_BUFF 300 //copie des caracteres d'un fichier
#define MAX_BUFF_COMMANDE 150
#define MAX_COMMANDES 30

// Les variables globales
char *history[MAX_COMMANDES];
int nbcommandeAct;
int onetime;
int num_arg;
char* full_path;

//les fonctions du programme
void find (char * dir_name);
void find_all();
void cd(char *mots);
void save_history(char *mots_commandes, int *nbcommandeAct);
void historyy(char *mots_commande[], int nbcommandeAct);
void touch(char *mots_commandes[]);
struct tm convert_date(char* date);
void copy(char *mots_commandes[]);
int copy_one(char path[MAX_PATH+1],char fichier_copie[MAX_PATH+1]);
int copie(int file1,int filecopie);
int copy_right(int file1,int filecopie);
void add(char **repertoireDest,char *source);
int copy_dir(char repertoire1[MAX_PATH+1],char repertoire_existe[MAX_PATH+1]);
int new_dir(char *repdest,char *repsrc);
char *get_myshell();
void executer_commande0(char *commande);
void executer_commande(char *mots_commande[]);
void myshell();
void copy_word(char *dest, char **src);
void lire_commande(char *liste,char *liste_commandes[]);
void execution_path(char *mots_commande[]);
int  error_fork(); 
int  error_wait(); 
char** reparation(char** src, int taille);
void cat(char *mots_commandes[]);
void redirection(char* out, int* id_stdout);
void redirection0(int id_sortie);


int main(int argc, char *argv[])
{
    char *liste = malloc(sizeof(char)*MAX_BUFF_COMMANDE);
    char *liste_commandes[MAX_COMMANDES];
    history[0] = NULL;
    num_arg = argc;
    
    while(1)
    {
        myshell();
        liste = get_myshell();
        lire_commande(liste,liste_commandes);
        save_history(liste, &nbcommandeAct); 
        executer_commande(liste_commandes);
    }
   
	
	//reparation(argv,argc);
	//executer_commande(argv);
	
    return 0;
}

char** reparation(char** src, int taille){
	int i;
	for(i=0;i<taille;i++)
		src[i]=src[i+1];
	return src;
}

void lire_commande(char *liste,char *liste_commandes[])
{
    int i=0;
    while(*liste!='\0')
    {
        liste_commandes[i] = malloc(sizeof(char)*MAX_BUFF_COMMANDE);
        copy_word(liste_commandes[i],&liste);
        i++;
    }
    liste_commandes[i] = NULL;
}

void copy_word(char *destination, char **source)
{
   
    while(**source!=' '&&**source!='\n')
    {
        *destination = **source;
        destination = destination+1;
        *source = (*source)+1;
    }
    *destination = '\0';
    do
    {
        *source = (*source)+1;
    }while(**source==' ');
}

void myshell()
{
    char buff[MAX_PATH];
    char* test=getcwd(buff,MAX_PATH);
    printf("raihane&Hamza_shell:-> ");
}

char *get_myshell()
{
    char *Buffer = malloc(sizeof(char)*MAX_BUFF_COMMANDE);

        if(!(fgets(Buffer,MAX_BUFF_COMMANDE,stdin)))
        {
            while(1)
            {
                            //int d;
                //d = getchar();
                exit(EXIT_SUCCESS);
            }
        }

    return Buffer;
}

void executer_commande0(char *commande)
{
    pid_t fk;
    fk = fork();
    if(fk<0)
    {
        perror("");
        return;
    }
    if(fk==0)//processus fils
    {

       while(!execlp(commande,commande,NULL))
       {
            perror("execlp");
            continue;
       }
    }

    else
    {
        int statut;
        pid_t w;
        w = waitpid(0,&statut,0);
        if (w == -1) {
                perror("waitpid");
                return;
            }
    }
}

void executer_commande(char *liste[])
{
    pid_t fk;
    fk = fork();
    int id;
    if(fk<0)
    {
        perror("");
        return;
    }
    
    if(fk==0)//processus fils
    {	
		
		//redirection(liste[2],&id); 
        if(strcmp(liste[0],"cd")==0)
        {
            cd(liste[1]);//on envoie l'adresse a atteindre qui correspond au second argument(apres cd)
        }
        else if(strcmp(liste[0],"history")==0)
        {
			historyy(liste,nbcommandeAct);
        }
        else if(strcmp(liste[0],"touch")==0)
        {
            touch(liste);
        }
        else if(strcmp(liste[0],"copy")==0)
        {
            copy(liste);
        }
        else if(strcmp(liste[0],"cat")==0)
        {
            cat(liste);
        }
        else if(strcmp(liste[0],"find")==0)
        {
			onetime=0;
			
            if(num_arg==2){
				 find(liste[1]);	
			}
            else if(num_arg==1) {
				find_all();
			}
        }
        else 
        {
	  //redirection(liste[3],NULL); 
           execution_path(liste);
          /*  int erreur;
            erreur = execvp(mots_commande[0],mots_commande);
            if(erreur<0)
            {
                perror("execvp");
                return;
            }*/
        }
       //redirection0(id); 
    }
		
    else
    {
        int statut;
        pid_t w;
        w = waitpid(0,&statut,0);
        if (w == -1) {
                perror("waitpid");
                return;
            }
    
    }
}

/*etape 3*/
void execution_path(char *mots_commande[])	
{
      char buff[MAX_PATH];
      char* file=getcwd(buff,MAX_PATH + 1);//nom du fichier actuel
      char *copie_premier_mot = malloc(sizeof(char)*MAX_PATH);
      sprintf(copie_premier_mot,"%s%s","/bin/",mots_commande[0]);//ajout du /bin/ devant le premier mot
      strcpy(mots_commande[0],copie_premier_mot);
      int i=0;
      while(mots_commande[i]!=NULL) i++;
      mots_commande[i] = malloc(sizeof(char)*MAX_PATH);
      strcpy(mots_commande[i],file);//le dernier mot est le nom du fichier actuel
      mots_commande[i+1] = NULL;
      int erreur = execv(mots_commande[0],mots_commande);
      if(erreur<0)
      {
            perror("execv");
            exit(1);
      }
}

int error_fork()
{
	perror("error processus");
        return(1);
}

int error_wait()
{
 perror("waitpid");
                return(1);
 
}

void find (char * dir_name)
{

  // on ouvre le repertoire dir_name
  	if(onetime==0){
		char original_path[] = {'/','h','o','m','e','/','r','a','i','h','a','n','e','/'};
		full_path = malloc( (strlen(dir_name)+14)*sizeof(char));
		strcpy(full_path,original_path);
		strcat(full_path,dir_name);
		//printf("%s\n",full_path);
		onetime=1;
	}
	
		DIR * d = opendir (full_path); 	
	
    while (1) {
		
        struct dirent * entry = readdir (d);
        char * d_name;
        if (! entry) {
            // le repertoire est vide donc on break      
            break;
        }

			d_name = entry->d_name;
			// on affiche le nom de repertoire 
			//printf ("%s/%s\n", dir_name, d_name);

			if (entry->d_type & DT_DIR) {

				 //Check that the directory is not "d" or d's parent. 
				
				if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0){
					   // int path_length;
						char path[MAX_PATH];
						//snprintf (path, MAX_PATH,"%s/%s",dir_name, d_name);
						//printf("%s\n",path);
						//taille=Add(path,original_path,taille);
						strcat(full_path,"/");
						strcat(full_path,entry->d_name);
						printf("%s\n",entry->d_name);
						find(path); // appel récursive de la fonction find
				}
			}//end DIR if
			else
				printf("%s\n",entry->d_name);
			
    }//end while
    // fermeture du repertoire 

     
	     closedir (d);
}

void find_all()
{
	
    //printf("%s\n",buff);
  // on ouvre me repertoire dir_name
  	if(onetime==0){
		char buff[MAX_PATH];
		getcwd(buff,MAX_PATH);
		full_path = malloc( strlen(buff)*sizeof(char));
		strcpy(full_path,buff);
	}
	
		DIR * d = opendir (full_path); 	
	onetime=1;
    while (1) {
		
		
        struct dirent * entry = readdir (d);
        char * d_name;
        if (! entry) {
            // le repertoire est vide donc on break      
            break;
        }

			d_name = entry->d_name;
			// on affiche le nom de repertoire 
			//printf ("%s/%s\n", dir_name, d_name);

			if (entry->d_type & DT_DIR) {

				 //Check that the directory is not "d" or d's parent. 
				
				if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0){
					   // int path_length;
						char path[MAX_PATH];
						//snprintf (path, MAX_PATH,"%s/%s",dir_name, d_name);
						//printf("%s\n",path);
						//taille=Add(path,original_path,taille);
						strcat(full_path,"/");
						strcat(full_path,entry->d_name);
						printf("%s\n",entry->d_name);
						
						find_all(); // appel récursive de la fonction find
				}
			}//end DIR if
			else
				printf("%s\n",entry->d_name);
			
    }//end while
    // fermeture du repertoire 

     
	     closedir (d);
}

void copy(char *mots_commandes[])
{
    //mots_commande[1] est la source
    DIR *isfolder = opendir(mots_commandes[1]);
    int success;
    if(isfolder!=NULL)
    {//on doit copier un repertoire
        closedir(isfolder);
        success = copy_dir(mots_commandes[1],mots_commandes[2]);
    }
    else
    {
        add(&mots_commandes[2],mots_commandes[1]);
        success = copy_one(mots_commandes[1],mots_commandes[2]);
    }
    if(success<0)
    {
        printf("COPY FAILED\n");
    }else printf("DONE!!\n");
}

/*Etape 1 : copie d'un seul fichier*/
int copy_one(char source[MAX_PATH+1],char destination[MAX_PATH+1])
{
    mode_t mode = S_IRUSR | S_IRGRP | S_IROTH;/*droits de lecture pour tout le monde;*/
    int filesource = open(source,O_RDONLY ,mode);/*ouverture en lecture*/
   if(filesource<0)
   {
        printf("ERROR\n");
        exit(1);
   }
   mode = S_IWUSR |S_IWGRP |S_IWOTH;
   /*ouverture en écriture avec effacement des données si il y en a et creation du fichier si il n'existe pas*/
   int  filedest = open(destination,O_WRONLY |O_TRUNC|O_APPEND|O_CREAT);
   if(filedest<0)
   {
        perror("ERROR");
        exit(1);
   }
    int nb =  copie(filesource,filedest);
    copy_right(filesource,filedest);

    close(filesource);
    close(filedest);
    return nb;
}

int copie(int filesrc,int filecopie)
{
    char * buffer = malloc(sizeof(char)*MAX_BUFF_COMMANDE);
    ssize_t bytes_read , bytes_write;
    size_t nbytes = sizeof(buffer);
    do
    {
        bytes_read = read(filesrc,buffer,nbytes);//lecture du fichier source
        if(bytes_read<0)
        {
            printf("ERROR\n");
            return -1;
        }
        bytes_write = write(filecopie,buffer,bytes_read);//ecriture dans le nouveau fichier
        if(bytes_write<0)
        {
            printf("ERROR\n");
            return -1;
        }
    }while(bytes_read>0);
    return 1;
}

/*etape 2 : copie des droits*/
int copy_right(int filesrc,int filecopie)
{
    struct stat filestat;//info sur le fichier
   int ok = fstat(filesrc,&filestat);
   if(ok<0)
   {
        perror("erreur de lecture des droits du premier fichier");
   }
    /*copie des droits dans le second fichier*/
    mode_t mode = filestat.st_mode;
    ok = fchmod(filecopie,mode);
    if(ok<0)
    {
        perror("erreur de copie des droits dans le second fichier");
    }
    return ok;
}
/*etape 3 et 4*/
void add(char **repertoireDest,char *source)
{
    char *motsrc = source;
    //on cree un repertoire dans repdest avec le meme nom que src :
    while(*source!='\0')
    {
        source = source+1;
        if(*source=='/')
        {
            motsrc = source+1;
        }
    }printf("%s\n",motsrc);
    sprintf(*repertoireDest,"%s/%s",*repertoireDest,motsrc);
}

int copy_dir(char repertoireSource[MAX_PATH+1],char repertoireDest[MAX_PATH+1])
{
    char *nom_complet_src = malloc(sizeof(char)*MAX_PATH);
    char * nom_complet_dest = malloc(sizeof(char)*MAX_PATH);
    //printf("passage dest %s\n",repertoireDest);
    add(&repertoireDest,repertoireSource);
     int nb = new_dir(repertoireDest,repertoireSource);
     if(nb <0){
        printf("ERROR\n"); exit(1);
     }
    DIR *pdirSrc = opendir(repertoireSource);
    if(pdirSrc==NULL)
    {
        perror("ERROR\n");
        exit(1);
    }
    DIR *pdirdest = opendir(repertoireDest);
    if(pdirdest==NULL)
    {
        perror("ERROR\n");
        exit(1);
    }
        struct dirent* fichierLu = NULL; //pointeur vers un des fichier du répertoire
        while ((fichierLu = readdir(pdirSrc))!= NULL)
        {
            if(strcmp(fichierLu->d_name,".")==0||strcmp(fichierLu->d_name,"..")==0||strchr(fichierLu->d_name,'~'))
            {
                continue;//on ne prend pas en compte le fichier parent et celui dans lequel on est
            }
            sprintf(nom_complet_src,"%s/%s",repertoireSource,fichierLu->d_name);
            sprintf(nom_complet_dest,"%s/%s",repertoireDest,fichierLu->d_name);
            if(fichierLu->d_type!=DT_DIR)//cas ou ce n'est pas un dossier
            {
                nb = copy_one(nom_complet_src,nom_complet_dest);
                if(nb <0) exit(1);

            }else {
                sprintf(nom_complet_dest,"%s",repertoireDest);
                nb =  copy_dir(nom_complet_src,nom_complet_dest);//on copie tout le contenue du repertoire
                 if(nb<0)
                {
                    printf("ERROR\n");
                    exit(1);
                }
            }
        }

        closedir(pdirdest);
        closedir(pdirSrc);
    return 1;
}

int new_dir(char *repdest,char *repsrc)
{
    int nb;
    struct stat infofichier;
    nb = stat(repsrc,&infofichier);//recuperation des infos du repertoire source
    if(nb<0)
    {
        perror("stat");
        return -1;
    }

    nb = mkdir(repdest,infofichier.st_mode);//creation du repertoire avec le meme mode que la source
    if(nb<0)
    {
        perror("mkdir");
        return -1;
    }
    return 0;
}

/*commande cat*/
void cat(char *mots_commandes[])
{
    FILE *fichier = fopen(mots_commandes[1],"r");
    if(fichier==NULL)
    {
        perror("fopen");
        return;
    }
    if(mots_commandes[2]!= NULL && strcmp(mots_commandes[2],"-n")==0 ){
		//affichage du contenu du fichier
    int caractere =0;
    int i=1;
    printf("0\t");
    do
    {
        caractere = fgetc(fichier);
        printf("%c",caractere);
        if(caractere=='\n' ){
			printf("%d\t",i);
			i++;
		}
    }while(caractere!=EOF);

	}
	else{
		//affichage du contenu du fichier
    int caractere =0;
    do
    {
        caractere = fgetc(fichier);
        printf("%c",caractere);
    }while(caractere!=EOF);

	}
	printf("\n");
        fclose(fichier);
}
/*commande touch*/
void touch(char *liste[])
{
    int retour;
    struct utimbuf dates;
    if(liste[1]==NULL)
    {
        printf("missing file operand\n");
        return;
    }
    else if(liste[2]==NULL)//sans option
    {
        time_t tpsActuel = time(NULL);
        if(tpsActuel==-1)
        {
            perror("erreur time");
        }
        //on met la date de lecture et la date de modification à la date actuel
        dates.actime = tpsActuel;
        dates.modtime = tpsActuel;
       retour  = utime(liste[1],&dates);
    }
    else if(strcmp("-m",liste[1])==0){
		time_t tpsActuel = time(NULL);
        if(tpsActuel==-1)
        {
            perror("erreur time");
        }
        //on met la date de lecture et la date de modification à la date actuel
        dates.actime = tpsActuel;
        dates.modtime = tpsActuel;
       retour  = utime(liste[2],&dates);
	}
    else if(strcmp(liste[1],"-r")==0)
    {
        //option de modification avec la date d'un fichier de référence
        struct stat buf;
        int retour = stat(liste[2],&buf);
        if(retour==-1)
        {
            perror("erreur -r");
            return;
        }
        dates.actime = buf.st_atime;//date du dernier acces
        dates.modtime = buf.st_mtime;//date de la derniere modification
        retour  = utime(liste[3],&dates);
    }
    else if(strcmp(liste[1],"-t")==0)
    {
        struct tm dateconvertie = convert_date(liste[2]);
        time_t date = mktime(&dateconvertie);
        if(date==-1)
        {
            perror("mktime");
        }else{
            dates.actime = date;
            dates.modtime = date;
            retour = utime(liste[3],&dates);
        }

    }
    else{
    printf("ERROR\n");}
    if(retour!=0)
       {   if(strcmp(liste[1],"-t")!=0 && strcmp(liste[1],"-r")!=0) {
		   fopen(liste[1],"w");
		   printf("File created\n");
			}
       }
       else{
            printf("touch reussi\n");
       }
}

struct tm convert_date(char *motdate)
{
    struct tm dateconverti;
    /* dateconverti.tm_sec = date%100;
    date/=100;*/
    char segment[2];
    char *copiedate = motdate;
    int nb=1;
    int passage=0;
    dateconverti.tm_sec = 0;//secondes en option
    while(nb!=0&&passage<6)
    {
        segment[0] = *copiedate;
        segment[1] = *(copiedate+1);
        nb = atoi(segment);
        switch(passage)
        {
            case 0 : dateconverti.tm_year = nb+100;break;//on suppose que l année est > 2000 et que le siecle n'est pas indiqué
            case 1 : dateconverti.tm_mon = nb-1;break;
            case 2 :dateconverti.tm_mday = nb;break;
            case 3 : dateconverti.tm_hour = nb;break;
            case 4 : dateconverti.tm_min = nb;break;
            case 5 : dateconverti.tm_sec = nb;break;

        }
        passage++;
        copiedate = copiedate+2;

    }

    return dateconverti;
}

/*commande history*/
void historyy(char *mots_commande[], int nbcommandeAct)
{
    int i = nbcommandeAct;
    int nb_ligne = nbcommandeAct;
    if(mots_commande[1]!=NULL)
    {//option n : affiche les n dernieres lignes
        nb_ligne = atoi(mots_commande[1]);
    }
    i=i-nb_ligne;
	
	 while(i<nbcommandeAct)
		{
		   printf("%3d  %s\n",i,history[i]);
			i++;
		}
}

void save_history(char *mots_commandes,int *nbcommandeAct)
{
    int i =0;
    while(i<MAX_COMMANDES&&history[i]!=NULL)
    {
        i++;
    }
    if(i<MAX_COMMANDES-1)
    {
        history[i+1] = NULL;
    }
    else if(i==MAX_COMMANDES)
    {
    //effacement de la premiere commande si le tableau est plein
        int j;
        for(j=0;j+1<MAX_COMMANDES;j++)
        {
            history[j] = history[j+1];
        }
    }
    history[i] = mots_commandes;
    *nbcommandeAct = i+1;

}

/*commande cd*/
void cd(char *adresse)
{
    int erreur;

    if(strcmp(adresse,"~")==0)
    {
        erreur = chdir("/home");
        if(erreur<0)
        {
            erreur = chdir("/HOME");
        }
    }
    else
    {
        erreur = chdir(adresse);
    }
    if(erreur<0)
    {
        perror("erreur chdir ");
        return;
    }
    printf("cd reussi\n");
}
void redirection(char* out, int* id_stdout)
{
 *id_stdout=dup(STDOUT_FILENO);
 int id_sortie=open(out, O_WRONLY| O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if(id_sortie<0)
    {
        perror("redirection");
    }
	 dup2(id_sortie,STDOUT_FILENO);
	 if(close(id_sortie) < 0) perror("close");
}

void redirection0(int id_sortie)
{
	dup2(id_sortie,STDOUT_FILENO);
}



