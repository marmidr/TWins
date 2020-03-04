# https://wiki.bash-hackers.org/scripting/terminalcodes

for a in 0 1 2 3 4 5 7; do
    echo "Attr=$a "

    for (( f=0; f<=8; f++ )) ; do
        for (( b=0; b<=8; b++ )) ; do
            #echo -ne "f=$f b=$b"
            echo -ne "\\033[${a};3${f};4${b}m"
            echo -ne "\\\\\\\\033[${a};3${f};4${b}m"
            echo -ne "\\033[0m "
        done
        echo
    done
    echo
done

echo