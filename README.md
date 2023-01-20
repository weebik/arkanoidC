# arkanoidC
### Klawiszologia
> <kbd>A</kbd> lub <kbd>🢀</kbd> &rarr; Ruch w lewo</br>
> <kbd>D</kbd> lub <kbd>🢂</kbd> &rarr; Ruch w prawo</br>
> <kbd>R</kbd> &rarr; Reset</br>
> <kbd>Q</kbd> &rarr; Wyjscie</br>

### Kompilacja i uruchomienie
> **Upewnij sie ze masz zainstalowane SDL2**</br>
>`sudo apt-get install libsdl2-dev`</br>
> Kompilacja:</br>
> `./build.sh`</br>
> Nastepnie:</br>
> `./start` (odpala gre ze standardowa mapa)</br>
> Opcjonalnie:</br>
> `./start [sciezka pliku]` (odpala gre z wczytana mapa z pliku)</br>
### Zawartosc pliku
> Maksymalnie 14 kolumn. Rzedow nie moze byc wiecej niz 9, a caly rzad MUSI byc wypelniony.</br>
> 0 - puste miejsce</br>
> 1 - cegielka</br>
> Podanie zbyt duzej ilosci kolumn lub rzedow albo nie wypelnienie calego rzedu zainicjuje odpalenie ze standardowa mapa.
### Przykladowa zawartosc pliku
> 01010101010101</br>
> 10101010101010</br>
> 01010101010101</br>
> 10101010101010</br>