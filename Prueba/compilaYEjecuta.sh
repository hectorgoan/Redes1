#! /bin/sh
gcc -g servidorDeFlujos.c -o servidor
gcc -g clienteDeFlujos.c -o cliente 
#lanza dos nuevas ventanas de terminal cada una con un ejecutable
#para terminar, hacer ctrl+c en la ventana de terminal principal

zenity --info --text="Se han lanzado 2 ventanas!\n Una es para el cliente y la otra para el Servidor.\n Pulse crtl+c en el terminal principal para salir" --title="Atención!" &
xterm -hold -e ./servidor & xterm -hold -e ./cliente 127.0.0.1

