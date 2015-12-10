#! /bin/sh

#####################################
# Fichero: compilaYEjecuta.sh
# Autores: 
# Héctor Gonzalo Andrés DNI 71038384D
# Néstor 
#####################################

make

#Lanza una nueva ventana en la que se ejecuta el cliente
#el servidor se sigue ejecutando en la ventana de terminal principal
#para terminar, hacer ctrl+c en la ventana de terminal del servidor

zenity --info --text="Se han lanzado 1 ventana!\nEn la terminal original se ejecuta el servidor.\nEn la nueva se ejecuta el cliente\nPulse crtl+c en el servidor para salir" --title="Atención!" &
./servidor & xterm -hold -e ./cliente 127.0.0.1

