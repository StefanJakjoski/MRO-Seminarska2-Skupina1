# Topologiji

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
