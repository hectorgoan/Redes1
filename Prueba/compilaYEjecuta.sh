#! /bin/sh
gcc -g servidorDeFechas.c -o servidor
gcc -g clienteConsultorFecha.c -o cliente 
#lanza dos nuevas ventanas de terminal cada una con un ejecutable
#para terminar, hacer ctrl+c en la ventana de terminal principal

zenity --info --text="Se han lanzado 1 ventana!\nEn la terminal original se ejecuta el servidor.\nEn la nueva se ejecuta el cliente\nPulse crtl+c en el terminal principal para salir" --title="Atención!" &
./servidor & xterm -hold -e ./cliente 127.0.0.1

