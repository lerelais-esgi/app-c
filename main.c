#include <stdio.h>
#include<string.h>
#include <curl/curl.h>


int lire(char *chaine, int longueur)

{

    char *positionEntree = NULL;

    // On lit le texte saisi au clavier

    if (fgets(chaine, longueur, stdin) != NULL)  // Pas d'erreur de saisie ?

    {
        positionEntree = strchr(chaine, '\n'); // On recherche l'"Entrée"

        if (positionEntree != NULL) // Si on a trouvé le retour à la ligne

        {

            *positionEntree = '\0'; // On remplace ce caractère par \0

        }

        return 1; // On renvoie 1 si la fonction s'est déroulée sans erreur

    }

    else

    {

        return 0; // On renvoie 0 s'il y a eu une erreur

    }

}
 
int main(void)
{
  CURL *curl;
  CURLcode res;
  char b[15]={0};
  char q[15]={0};
  char link[60] = "http://fr.openfoodfacts.org/api/v0/produit/";
	printf("Saisissez le code barre de votre produit:\n");
	lire(b,15);
	strcat (strcat(link,b),".json");
	printf("%s \n", link);
	printf("Saisissez la quantité de produits: \n");
	lire(q,50);

 curl = curl_easy_init();
  if(curl) {
    	

    curl_easy_setopt(curl, CURLOPT_URL, link);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, NULL);
    
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
	printf(" \n Vous avez demandé le ramassage de %s produits du meme type, votre requete est envoye avec succes! \n Nous vous remercions d avoir utilise l application utilisateur du relais", q);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  return 0;
}

