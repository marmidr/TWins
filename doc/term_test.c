//
// FROM http://0x80.pl/articles/terminals.html
// Autor:   Wojciech Muła
// Dodany:  2003(?)
//

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


int tty_fileno; /* deskryptor terminala */

typedef struct
{
    int  num;      /* ilość bajtów (0..8) */
    char array[8]; /* odczytane bajty     */
} keycode;

keycode get_key()
{
    static keycode kc;

    /* odczytane zostanie co najwyżej 8 bajtów */
    if ((kc.num = read(tty_fileno, kc.array, 8)) == -1)
        kc.num = 0;

    errno = 0;
    return kc;
}

keycode wait_for_key()
{
    fd_set read_set;

    FD_ZERO(&read_set);
    FD_SET (tty_fileno, &read_set);

    /* czekaj na klawisz */
    select(tty_fileno+1, &read_set, NULL, NULL, NULL);
    return get_key();
}

// extern int errno;
void check_errno()
{
    if (errno)
    {
        perror(strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int main()
{
    struct termios term;
    char    eof; /* kod znaku EOF */
    keycode k;
    int     i;

    tty_fileno = open("/dev/tty", O_RDONLY | O_NONBLOCK);
    check_errno();

    /* pobranie parametrów terminala */
    tcgetattr(tty_fileno, &term);
    check_errno();
    eof = term.c_cc[VEOF]; /* znak EOF, prawdopodobnie będzie to \004 */

    /* wyłączenie trybu kanonicznego oraz echa */
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(tty_fileno, TCSAFLUSH, &term);
    check_errno();

    puts("Start typing >\n");

    /* wyświetlane są kody klawiszy, koniec po naciśnięciu CTRL-D */
    while (1)
    {
        k = wait_for_key(); /* pobierz kod klawisza */

        if (k.num == 1 && k.array[0] == eof)
        {
            puts("CTRL-D!");
            break;
        }

        printf("%d: ", k.num);
        for (i=0; i<k.num; i++)
        {
            if ((unsigned char)k.array[i] < ' ')
                printf("\\x%02x ", (unsigned char)k.array[i]);
            else
                printf("%c ", (unsigned char)k.array[i]);
        }
        putchar('\n');
    }

    puts("naciśnij dowolny klawisz...");
    wait_for_key();

    /* przywrócenie zmienionych parametrów */
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(tty_fileno, TCSAFLUSH, &term);
    check_errno();

    return 0;
}
