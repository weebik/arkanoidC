# arkanoidC
### Cel gry
> Celem gry jest stracenie wszystkich cegielek nie pozwalajac pilce spasc na ziemie. Gra zacznie sie po nacisnieciu dowolnego klawisza na klawiaturze. Po wygranej poziom uruchomi sie na nowo.
### Klawiszologia
> <kbd>A</kbd> lub <kbd>🢀</kbd> &rarr; Ruch w lewo</br>
> <kbd>D</kbd> lub <kbd>🢂</kbd> &rarr; Ruch w prawo</br>
> <kbd>R</kbd> &rarr; Reset</br>
> <kbd>E</kbd> &rarr; Ciemny Motyw</br>
> <kbd>Q</kbd> &rarr; Wyjscie</br>
### Kompilacja i uruchomienie
> **Upewnij sie ze masz zainstalowane SDL2**</br>
>`sudo apt-get install libsdl2-dev`</br>
>`sudo apt-get install libsdl2-ttf-dev`</br>
>`sudo apt-get install libsdl2-mixer-dev`</br>
> Kompilacja:</br>
> `./build.sh`</br>
> Nastepnie:</br>
> `./arkanoid` (odpala gre ze standardowa mapa)</br>
> Opcjonalnie:</br>
> `./arkanoid [sciezka pliku]` (odpala gre z wczytana mapa z pliku)</br>
### Zawartosc pliku
> Maksymalnie 14 kolumn. Rzedow nie moze byc wiecej niz 9, a caly rzad MUSI byc wypelniony.</br>
> 0 - puste miejsce</br>
> 1 - cegielka z 1 zyciem</br>
> 2 - cegielka z 2 zyciami</br>
> 3 - cegielka z 3 zyciami</br>
> 4 - niezniszczalna cegiełka</br>
> Podanie zbyt duzej ilosci kolumn lub rzedow albo nie wypelnienie calego rzedu zainicjuje odpalenie ze standardowa mapa. Badz swiadomy ze mozesz ulorzyc mape ktorej nie da sie przejsc!!!
### Przykladowa zawartosc pliku
> 01010101010101</br>
> 10101010101010</br>
> 01010101010101</br>
> 10101010101010</br>