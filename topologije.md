# Topologije

## Krožna point-to-point topologija

Krožna point-to-point topologija (ali **topologija obroča**) opisuje omrežje, kjer je vsaka naprava povezana z natanko dvema drugima napravama v zaprti zanki.

Krožne topologije omogočajo krožni prenos podatkov med *n* vozlišči. Podatkovni paketi potujejo v eno smer in prehajajo skozi vsako vozlišče v obroču, dokler ne dosežejo cilja.

V krožnih topologijah lahko podatke hkrati prenaša le eno vozlišče. To se nadzoruje z **žetonom**, ki kroži po omrežju in omogoča posameznemu vozlišču prenos podatkov. Vozlišče ne sme prenašati podatkov brez posedovanja žetona.

### Prednosti

- Naenkrat lahko prenaša le eno vozlišče, kar zmanjšuje trke paketov (*packet collision*).
- Implementacija je pogosto preprosta; kompleksnost dodajanja več vozlišč se linearno skalira, za razliko od kvadratnega skaliranja mrežaste (*mesh*) topologije.

### Slabosti

- Če eno vozlišče odpove, se celotno omrežje izklopi.
- Zmogljivost omrežja je enostavno preobremeniti.



## Mrežasta (mesh) topologija

V omrežju z mesh topologijo je vsaka naprava povezana direktno z vsako drugo napravo.
Tako omrežje je torej decentralizirano.

### Prednosti

- Več možnih poti omogoča boljšo odpornost do napak: če povezava odpove, uberemo drugo pot
- Raztegljivo (scalable) omrežje: dodajanje naprave ne spremeni strukture obstoječega omrežja
- Učinkovit prenos podatkov: ne potujejo skozi centralno napravo

### Slabosti

- Kompleksna izdelava in vzdrževanje: vsaka naprava je povezana z vsako (alternativa: delno mrežasta topologija)
- Visoka cena
- Možnost redundantnih povezav

## Fat tree topologija

## Zvezdna topologija

## Spine leaf topologija
