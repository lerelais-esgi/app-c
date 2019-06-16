#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<string.h>
#include<stdlib.h> 
#include <curl/curl.h>
#include <unistd.h>
#include<cjson/cJSON.h>

struct curl_slist *session= NULL;

int lire(char *chaine, int longueur)

{

    char *positionEntree = NULL;

    // On lit le texte saisi au clavier

    if(fgets(chaine, longueur, stdin) != NULL)  // Pas d'erreur de saisie ?

    {
        positionEntree = strchr(chaine, '\n'); // On recherche l'"Entrée"

        if(positionEntree != NULL) // Si on a trouvé le retour à la ligne

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

struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

 
int login(CURL *curl, CURLcode res)
{
  char email[50] = {0};
  char password[50] = {0};
  char response[100]={0};

  printf("Saisissez votre email:\n");
  lire(email,30);
  printf("Saisissez votre mot de passe: \n");
  lire(password,30);

  char PostData[100] = "email=";
  strcat(strcat(PostData,email),"&password=");
  strcat(PostData,password);

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();

  if(curl) {

    struct string s;
    init_string(&s);
 
    curl_easy_setopt(curl, CURLOPT_URL, "http://api.lerelais.cf/login"); 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s); 
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, PostData);

    

    res = curl_easy_perform(curl);

    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    else{
	long http_response = 0;
	curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE, &http_response);
	fprintf(stderr, "\nSTATUS CODE : %d \n", http_response);
	printf("Response : %s\n", s.ptr);

	if(strstr(s.ptr,"true") != NULL){
		printf("Félicitation vous etes bien connecte! \n");
		cJSON *token = NULL;
		cJSON *request_json = cJSON_Parse(s.ptr);
		token = cJSON_GetObjectItemCaseSensitive(request_json,"token");
		session = curl_slist_append(session,"X-Auth-key:token->valuestring");
		printf("token: %s \n",token->valuestring);
		printf("token: %s \n",session);
		return 1;
		
	}
	else{
		printf("Une erreur est survenues: nous n'avons pas pu récupérer vos informations \n");
	}
	
    }

    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }

  return 0;
}

char* scanCode(CURL *curl, CURLcode res){
	char barcode[30]={0};
	int qt=0;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	printf("Saisissez le code barre de votre produit: \n");
	lire(barcode,30);
	printf("Saisissez quantité de produits concernés: \n");
	scanf("%d",&qt);

	curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();

        if(curl) {

		struct string s;
		init_string(&s);
	 	
		curl_easy_setopt(curl, CURLOPT_URL, "http://api.lerelais.cf/product/getInfo"); 
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, session);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		res= curl_easy_perform(curl);

		if(res != CURLE_OK)
		      fprintf(stderr, "curl_easy_perform() failed: %s\n",
			      curl_easy_strerror(res));
		else{

			long http_response = 0;
	                curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE, &http_response);
	                fprintf(stderr, "\nSTATUS CODE : %d \n", http_response);
	                printf("Response : %s\n", s.ptr);
			
		}

	/* always cleanup */ 
       curl_easy_cleanup(curl);
       }

       return 0;

}

int main(int argc, char **argv){

	CURL *curl;
	CURLcode res;
        login(curl, res);
	scanCode(curl,res);
	curl_slist_free_all(session);


	return 0;

}
