/*
    lc -Lcdv -O -m2 MvPSF
    gcc020 -o MvPSF_gcc -noixemul -m68030 MvPSF.c -lauto -g -v
    gcc020 -DAPURIFY -g MvPSF.c -o MvPSF.s -m68030 -noixemul -lauto -S
    Apurify -tb MvPSF.s -o MvPSFPure.s
    gcc020 -g MvPSFPure.s -o MvPSF_pure -m68030 -noixemul -lauto -lAPur
*/

/*
**      $VER: MvPSF.c 0.0 (09 Mar 1996)
*/

#include <ctype.h>
#include <dos/dos.h>
/*
#include <proto/dos.h>
*/
#include <exec/exec.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/memory.h>
/*
#include <proto/exec.h>
#include <clib/alib_protos.h>
*/
#include <utility/tagitem.h>
#ifdef __GNUC__
#include <inline/exec.h>
#include <inline/dos.h>
#include <inline/alib.h>
#endif

/* Fichiers AFM */
const char  AFMHeader[] = "StartFontMetrics";
#define AFMHeadlong ((unsigned long)strlen(AFMHeader))
const int PosAFM = 0;
const char AFMFNStr[] = "FontName";
const char AFMFHStr[] = "StartCharMetrics";

/* Fichiers PFB */
const char  PFBHeader[] = "\x80\x01";
#define PFBHeadlong ((unsigned long)strlen(PFBHeader))
const int PosPFB = 6;
const char PFBFNStr[] = "/FontName";
const char PFBFHStr[] = "eexec";

/* Fichiers PFA */
const char  PFA1Header[] = "%!PS-AdobeFont";
#define PFA1Headlong ((unsigned long)strlen(PFA1Header))
const char  PFA2Header[] = "%!FontType";
#define PFA2Headlong ((unsigned long)strlen(PFA1Header))
const int PosPFA = 0;
const char PFAFNStr[] = "/FontName";
const char PFAFHStr[] = "eexec";

const int LONG_BUFFER = 64;

typedef enum {TYF_ERR = -1,TYF_NON = 0,TYF_AFM,TYF_PFA,TYF_PFB} TYPE_FICHIER;

typedef struct
{
    struct MinNode UF_Node;
    int PosDebut;       /* début de la partie textuelle */
    char fontname[32];  /* chaîne de reconnaissance de FontName */
    char finhead[32];   /* chaîne indiquant la fin de l'en-tête */
    char type[16];      /* Extension du fichier */
    char nom[108];      /* Nom du fichier */
} Un_Fichier;


void traiter_fichier(Un_Fichier *fic);
BOOL lecture_repertoire(struct MinList *ma_liste,struct FileInfoBlock *info,BPTR rep);
BOOL ajoute_fichier(char *nom,Un_Fichier *fic);
TYPE_FICHIER type_fichier(BPTR);
BOOL recup_mot(BPTR fh,char *chaine,int longueur);


int main(int argc, char **argv)
{
    BPTR rep;
    struct FileInfoBlock *info;

    if (argc < 2)
    {
        Printf("Usage :\n\t%s directory\n",argv[0]);
        exit(RETURN_WARN);
    }

    info = AllocDosObjectTags(DOS_FIB,TAG_DONE,NULL);

    if (!info)
    {
        Printf("Allocation mémoire impossible\n");
        exit(RETURN_FAIL);
    }

    rep = Lock(argv[1],ACCESS_READ);

    if (rep)
    {

        if (Examine(rep,info))
        {
            BPTR ancien_rep;
            struct MinList ma_liste;
            struct MinNode *node;
            Un_Fichier *f_tmp;

            ancien_rep = CurrentDir(rep);
            NewList((struct List *)&ma_liste);

            if (lecture_repertoire(&ma_liste,info,rep))
            {
                for (node = ma_liste.mlh_Head ; node->mln_Succ ; node = node->mln_Succ)
                {
                    f_tmp = (Un_Fichier *)node;
                    Printf("Fichier %s type %s\n",f_tmp->nom,f_tmp->type);

                    traiter_fichier(f_tmp);
                }

            }

            Printf("Boucle Ok\n");

            CurrentDir(ancien_rep);

            Printf("Début libération\n");

            node = ma_liste.mlh_Head;

            while (node->mln_Succ)
            {
                struct MinNode *suivant = node->mln_Succ;

                f_tmp = (Un_Fichier *)node;

                Printf("Libération de %s\n",f_tmp->nom);
                FreeMem(f_tmp,sizeof(Un_Fichier));

                node = suivant;
            }

        }
        else
        {
            PrintFault(IoErr(),"Erreur lors d'Examine");
        }

        UnLock(rep);
    }
    else
    {
        PrintFault(IoErr(),"Erreur lors de Lock");
    }


    FreeDosObject(DOS_FIB,info);

}


/* ------------------------------ traiter_fichier ------------------------------

 Commentaire:

*/

void traiter_fichier(Un_Fichier *fic)
{
    BPTR fh;

    fh = Open(fic->nom,MODE_OLDFILE);

    if (fh)
    {
        char chaine[256];

        Seek(fh,fic->PosDebut,OFFSET_BEGINNING);

        do  /* Recherche du nom de la fonte dans le fichier */
        {
            if (recup_mot(fh,chaine,sizeof(chaine)))
            {
                if (!strcmp(chaine,fic->fontname))
                {
                    if (recup_mot(fh,chaine,sizeof(chaine)))
                    {
                        char tmp_nom[108];
                        int lchaine = strlen(chaine)-1;

                        /* On filtre le dernier caractère si c'est ) */
                        if (chaine[lchaine] == ')')
                        {
                            chaine[lchaine] = '\0';
                        }


                        /* On filtre le premier caractère si c'est ( ou / */
                        if ((*chaine == '/') || (*chaine == '('))
                        {
                            strcpy(tmp_nom,chaine+1);
                        }
                        else
                        {
                            strcpy(tmp_nom,chaine);
                        }

                        strcat(tmp_nom,fic->type);

                        if (strcmp(fic->nom,tmp_nom))
                        {
                            if (Rename(fic->nom,tmp_nom))
                            {
                                Printf("%s -> %s\n",fic->nom,tmp_nom);
                            }
                            else
                            {
                                PrintFault(IoErr(),fic->nom);
                            }

                        }
                        else
                        {
                            Printf("%s OK\n",fic->nom);
                        }

                        break;
                    }
                    else
                    {
                        PrintFault(IoErr(),fic->nom);
                        break;
                    }

                }
                else
                {
                    if (!strcmp(chaine,fic->finhead))
                    {
                        Printf("Erreur : %s pas de nom\n",fic->nom);
                        break;
                    }

                }

            }
            else
            {
                PrintFault(IoErr(),fic->nom);
                break;
            }

        } while(1);

        Close(fh);
    }
    else
    {
        PrintFault(IoErr(),fic->nom);
    }

}


/* --------------------------------- recup_mot ---------------------------------

 Commentaire:

*/

BOOL recup_mot(BPTR fh,char *chaine,int longueur)
{
    BOOL rv;
    int courant = 0;
    longueur--;

    do
    {
        LONG buffer = FGetC(fh);

        if (buffer < 0)
        {
            rv = FALSE;
            break;
        }

        if (isspace(buffer))
        {
            chaine[courant] = '\0';
            rv = TRUE;
            break;
        }
        else
        {
            chaine[courant] = buffer;
            courant++;

            if (courant >= longueur)
            {
                chaine[longueur] = '\0';
                Printf("Erreur : ligne trop longue (%s)\n",chaine);
                rv = FALSE;
                break;
            }
        }
    } while(1);

    return rv;

}



BOOL lecture_repertoire(struct MinList *ma_liste,struct FileInfoBlock *info,BPTR rep)
{
    BOOL rv;

    Un_Fichier *f_tmp = AllocMem(sizeof(Un_Fichier),MEMF_CLEAR);

    if (f_tmp)
    {
        do
        {

            if (!ExNext(rep,info))
            {
                long erreur;

                erreur = IoErr();

                if (erreur != ERROR_NO_MORE_ENTRIES)
                {
                    PrintFault(erreur,"Erreur lors d'ExNext");
                }

                break;
            }

            if (info->fib_DirEntryType < 0)
            {
                if (ajoute_fichier(info->fib_FileName,f_tmp))
                {
                    AddTail(ma_liste,f_tmp);
                    f_tmp = AllocMem(sizeof(Un_Fichier),MEMF_CLEAR);

                    if (!f_tmp)
                    {
                        Printf("Plus de mémoire disponible\n");
                        break;
                    }

                }

            }
            else
            {
                Printf("%s est un répertoire\n",info->fib_FileName);
            }

        } while(1);
    }
    else
    {
        Printf("Plus de mémoire disponible\n");
    }

    if (f_tmp)
    {
        FreeMem(f_tmp,sizeof(Un_Fichier));
        rv = TRUE;
    }
    else
    {
        rv = FALSE;
    }

    return rv;
}


BOOL ajoute_fichier(char *nom,Un_Fichier *fic)
{
    BOOL rv = TRUE;
    BPTR fichier;

    fichier = Open(nom,MODE_OLDFILE);

    if (fichier)
    {
        switch (type_fichier(fichier))
        {
            case (TYF_AFM) :
            {
                fic->PosDebut = PosAFM;
                strcpy(fic->fontname,AFMFNStr);
                strcpy(fic->type,".AFM");
                strcpy(fic->finhead,AFMFHStr);
                break;
            }

            case (TYF_PFA) :
            {
                fic->PosDebut = PosPFA;
                strcpy(fic->fontname,PFAFNStr);
                strcpy(fic->type,".PFA");
                strcpy(fic->finhead,PFAFHStr);
                break;
            }

            case (TYF_PFB) :
            {
                fic->PosDebut = PosPFB;
                strcpy(fic->fontname,PFBFNStr);
                strcpy(fic->type,".PFB");
                strcpy(fic->finhead,PFBFHStr);
                break;
            }

            case (TYF_NON) :
            {
                Printf("%s de type inconnu\n",nom);
                rv = FALSE;
                break;
            }

            case (TYF_ERR) :
            {
                PrintFault(IoErr(),"");
                rv = FALSE;
                break;
            }
        }

        if (rv)
        {
            Printf("%s reconnu\n",nom);
            strcpy(fic->nom,nom);
        }

        Close(fichier);

    }
    else
    {
        PrintFault(IoErr(),"Erreur d'ouverture");
        rv = FALSE;
    }

    return rv;

}


TYPE_FICHIER type_fichier(BPTR fichier)
{
    char buffer[64/*LONG_BUFFER*/];
    TYPE_FICHIER ret_val;

    if (Read(fichier,buffer,sizeof(buffer)) > 0)
    {
        if (!memcmp(buffer,AFMHeader,AFMHeadlong))
        {
            ret_val = TYF_AFM;
        }
        else if (!memcmp(buffer,PFBHeader,PFBHeadlong) &&
                 (!memcmp(buffer+PosPFB,PFA1Header,PFA1Headlong) ||
                  !memcmp(buffer+PosPFB,PFA2Header,PFA2Headlong)))
        {
            ret_val = TYF_PFB;
        }
        else if (!memcmp(buffer,PFA1Header,PFA1Headlong) ||
                 !memcmp(buffer,PFA2Header,PFA2Headlong))
        {
            ret_val = TYF_PFA;
        }
        else
        {
            ret_val = TYF_NON;
        }

    }
    else
    {
        ret_val = TYF_ERR;
    }

    return ret_val;
}


