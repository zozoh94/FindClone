#!/bin/bash

preds=( "repertoire courant par défaut avec print" "type avec les fichiers" "ls du repertoire Téléchargements" "uid avec mon uid" "gid avec mon gid" "user avec mon username" "group avec mon groupe" "ctime" "exec fihier .c" "perm" "perm masquée fichiers ayant un des droits d'écriture" )
find="./find.out"
commands=("$find"
	  "$find /home/enzo/Documents -type f"
	  "$find /home/enzo/Téléchargements -ls"
	  "$find /home/enzo/Téléchargements -uid 1000 -ls"
	  "$find /home/enzo/Téléchargements -gid 1000 -ls"
	  "$find /home/enzo/Téléchargements -user enzo -ls"
	  "$find /home/enzo/Téléchargements -group enzo -ls"
	  "$find /home/enzo/Documents -ctime -10 -ls"
	  "$find ../src -name *.c -exec cat {} ; -print"
	  "$find /home/enzo/Documents -perm 755 -ls"
	  "$find /home/enzo/Documents -perm /222 -ls")
	  
for ((i = 0; i < ${#preds[@]} && i < ${#commands[@]}; i++))
do
    echo -e "Test du predicat : \e[35m${preds[i]}\e[0m"
    echo -e "Commande : \e[36m${commands[i]}\e[0m"
    read -p "Voulez vous lancer la command (y/N)" continuer
    if [[ $continuer == "y" ]]
    then
	${commands[i]}
    fi
done
