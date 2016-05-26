int nombre_pipes()
{	
	//le nombre de "|" dans la ligne de commande
	int i = 0, pipes = 0;
	while(commande[i] != NULL) {
		if (strcmp(commande[i], "|") == 0)
			pipes++;
		i++;
	}
	return pipes;
}

int pipe(int nombre_pipes) {
    
    
    int nb_commands = nb_pipes + 1;
    int i = 0;

    int tab_pipes[2*nb_pipes];

    for(i = 0; i < nb_pipes; i++){
        if(pipe(tab_pipes + i*2) < 0) {
            printf("Erreur");
            return -1;
        }
    }

    int k = 1;
    int place;
    int start_of_commande[10]; 
    start_of_commande[0] = 0; 
    
    
    i = 0;
    while (commande[i] != NULL) {
        if(strcmp(commande[i], "|") == 0) {
            commande[i] = NULL;
            start_of_commande[k] = i+1;
            k++;
        }
        i++;
    }

	int j = 0;
	int pid, status;
    for (i = 0; i < nb_commands; i++) {
		
		pid = fork();
		if(pid == 0) { /* fils */
			
			if(i < nb_pipes) {
				if(dup2(tab_pipes[j + 1], 1) < 0) {
					printf("Erreur, dup2 a échoué.\n");
					return -1;
				}
			}

		
			if(j != 0 ) {
				if(dup2(tab_pipes[j-2], 0) < 0) {
					printf("Erreur, dup2 a échoué.\n");
					return -1;
				}
			}

			for(k = 0; k < 2*nb_pipes; k++){
					close(tab_pipes[k]);
			}

			
			if (internal_command(start_of_commande[i]) == 1) { /* commandes internes */
				my_exit(commande); 
									
			}
			else { /* commandes externes */
				const char* path = get_path(commande[start_of_commande[i]]); 
				if (strcmp(path, "echec") != 0) { 
					execv(path, commande + start_of_commande[i]);
				}
				else {
					printf("Erreur");
					my_exit(commande);
					return -1;
				}
			}
		}
		else if(pid < 0){ 
			printf("Erreur, fork a échoué.\n");
			return -1;
		}

        j += 2;
    }

    for(i = 0; i < 2 * nb_pipes; i++){
        close(tab_pipes[i]); /* fermeture des pipes */
    }

    for(i = 0; i < nb_pipes + 1; i++){
        waitpid(pid, &status, 0); 
    }
}
