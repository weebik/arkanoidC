# arkanoidC
Projekt ARKANOIDA w SDL2 (C)
### Klawiszologia
> <kbd>A</kbd> lub <kbd>🢀</kbd> &rarr; Ruch w lewo
> <kbd>D</kbd> lub <kbd>🢂</kbd> &rarr; Ruch w prawo
> <kbd>R</kbd> &rarr; Reset
> <kbd>Q</kbd> &rarr; Wyjscie

### Kompilacja i uruchomienie
> Kompilacja:
> ./build.sh 
> Nastepnie:
> ./a.out (odpala gre ze standardowa mapa)
> Opcjonalnie:
> ./a.out [nazwa pliku] (odpala gre ze wczytana mapa z pliku)
### Zawartosc pliku
> Maksymalnie 14 kolumn. Rzedow nie moze byc wiecej niz 9, a caly rzad MUSI byc wypelniony.</br>
> 0 - puste miejsce</br>
> 1 - cegielka</br>
> Podanie zbyt duzej ilosci kolumn lub rzedow albo nie wypelnienie calego rzedu zainicjuje odpalenie ze standardowa mapa.
###### Przykladowa zawartosc pliku
> 01010101010101</br>
> 10101010101010</br>
> 01010101010101</br>
> 10101010101010</br>