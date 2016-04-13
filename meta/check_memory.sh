cd ..
valgrind --leak-check=yes --log-file="meta/valgrind.txt" --suppressions="meta/valgrind_suppresion.txt" ./bin/game_Debug