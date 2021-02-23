/* cat: concatenate files
 * as seen in K&R2
 *
 * created by Anvesh G. Jhuboo
 * on Feb/18/21
 */

#include <stdio.h>
#include <stdlib.h>


/* cat: concatenate file, version 2 */
int main(int argc, char *argv[])
{
    FILE *fp;
    void filecopy(FILE *, FILE *);
    char *prog = argv[0];       /* program name for errors */
    
    if (argc == 1)  /* no args; copy standard input */
        filecopy(stdin, stdout);
    else
        while (--argc > 0)
            if ((fp = fopen(*++argv, "r")) == NULL) {
                fprintf(stderr, "%s: can't open %s\n", prog, *argv);
                exit(1);
            } else {
                filecopy(fp, stdout);
                fclose(fp);
            }

    if (ferror(stdout)) {
        fprintf(stderr, "%s: error writing stdout\n", prog);
        exit(2);
    }

    exit(0);
}


/* filecopy: copy file ifp to file otp */
void filecopy(FILE *ifp, FILE *otp)
{
    int c;
    while ((c = getc(ifp)) != EOF)
        putc(c, otp);
}
